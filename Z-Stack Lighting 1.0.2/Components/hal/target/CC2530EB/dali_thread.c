/*******************************************************************************
*
* dali_thread.c
*
* DALI forward frame format:
*
*  | S |        8 address bits         |        8 command bits         | stop  |
*  | 1 | 1 | 0 | 0 | 0 | 0 | 0 | 1 | 1 | 0 | 1 | 1 | 1 | 1 | 0 | 0 | 0 |   |   |
*
* -+ +-+ +---+ +-+ +-+ +-+ +-+   +-+ +---+   +-+ +-+ +-+ +---+ +-+ +-+ +--------
*  | | | |   | | | | | | | | |   | | |   |   | | | | | | |   | | | | | |
*  +-+ +-+   +-+ +-+ +-+ +-+ +---+ +-+   +---+ +-+ +-+ +-+   +-+ +-+ +-+
*
*  |2TE|2TE|2TE|2TE|2TE|2TE|2TE|2TE|2TE|2TE|2TE|2TE|2TE|2TE|2TE|2TE|2TE|  4TE  |
*
*
* DALI slave backward frame format:
*
*                   | S |         8 data bits           | stop  |
*                   | 1 | 1 | 0 | 0 | 0 | 0 | 0 | 1 | 1 |   |   |
*
*   +---------------+ +-+ +---+ +-+ +-+ +-+ +-+   +-+ +-------------
*   |               | | | |   | | | | | | | | |   | | |
*  -+               +-+ +-+   +-+ +-+ +-+ +-+ +---+ +-+
*
*   |4 + 7 to 22 TE |2TE|2TE|2TE|2TE|2TE|2TE|2TE|2TE|2TE|  4TE  |
*
* 2TE = 834 usec (1200 bps)
*
********************************************************************************
*  commands supported
*  ------------------
*  Type				Range			Repeat		Answer from slave
*  Power control	0 - 31 			N			N
*
*  Configuration	32-129			Y			N
*  Reserved			130-143			N			N
*
*  Query			144-157			N			Y
*  Reserved			158-159			N			N
*  Query			160-165			N			Y
*  Reserved			166-175			N			N
*  Query			176-197			N			Y
*  Reserved			198-223			N			N
*  Query,2xx Std.	224-254			?			?
*  Query			255				N			Y
*
*  Special			256-257			N			N
*  Special			258-259			Y			N
*  Special			260-261			N			N
*  Special			262-263			N			N
*  Special			264-267			N			N
*  Special			268-269			N			Y
*  Special			270				N			N
*  Reserved			271				N			N
*  Special			272				N			N
*******************************************************************************/

#include  "hal_mcu.h"
#include  "hal_types.h"
//#include "bsp.h"
#include "dali_thread.h"
#include "OnBoard.h"
#include "zcl.h"
#include "zcl_lighting.h"
#include "AF.h"
#include "ZComDef.h"
//#include "zcl_general.h"
#include "OSAL.h"

/***********************************************************/
/* Configuration flags                                     */
/***********************************************************/

/* in case of inverted RX path define INVERTED_RX */
#define INVERTED_RX

/***********************************************************/
/* Microcontroller and Board specific defines              */
/***********************************************************/


/* PIO pin P0.7 is used as DALI send (tx) pin */
#define DALI_SetOutputHigh() { P0_7=1; }
#define DALI_SetOutputLow()  { P0_7=0; }
#define DALI_ConfigOutput()  { P0DIR  |=  (1<<7); }

/* PIO pin P1.3 is used as DALI receive (rx) pin */
#ifdef INVERTED_RX
#define DALI_GetInput(x)     { x = P1_3 ? 0 : 1; }
#else
#define DALI_GetInput(x)     { x = P1_3 ? 1 : 0; }
#endif

/* For receive, this module uses T3-CAP1 input (capture and interrupt on both edges) */
/* T3-CAP1 input (P1.4) is connected to P1.3 (to check high / low level by software) */
/* So set P1.4 as CT3.CAP1 (= DALI receive pin). */
#define DALI_ConfigInput()   { P1SEL |= 0x10; }

/* T3 is used for DALI timing and capturing of DALI input */
#define GET_TIMER_REG_CR0(x) { x = T3CC1; }    //to get timestamp for the received message.
#define GET_TIMER_REG_IR(x)  { x = TIMIF; }    //to decide the interrupt type of T3.
#define SET_TIMER_REG_IR(x)  { TIMIF &= x; }    //to clear T3 overflow interrupt or cap0 capture interrupt.
#define SET_TIMER_REG_PR(x)  { T3CTL = x; }    //initialize timer counter 3.
#define SET_TIMER_REG_TC  { T3CTL |= 0x04; }    //clear T3 counter.
#define SET_TIMER_REG_CCR(x) { T3CCTL1 = x; }    //to enable or disable T3_cap1.
#define SET_TIMER_START { T3CTL |= 0x10; }    //to start T3.
#define SET_TIMER_STOP { T3CTL &= 0xef;  T3CTL |= 0x04; }    //stop and reset T3.
#define SET_TIMER_REG_MR0(x) { T3CC0 = x; }    //set match value to control the overflow time of T3.

/***********************************************************/
/* Type definitions and defines                            */
/***********************************************************/

#define MAX_BF_EDGES      18     // max 18 edges per backward frame

/* protocol timing definitions */
#define TE          (104)                   // half bit time = 417 usec
#if 0 /* strict receive timing according to specification (+/- 10%) */
#define MIN_TE      (TE     - (TE/10))      // minimum half bit time
#define MAX_TE      (TE     + (TE/10))      // maximum half bit time
#define MIN_2TE     ((2*TE) - ((2*TE)/10))  // minimum full bit time
#define MAX_2TE     ((2*TE) + ((2*TE)/10))  // maximum full bit time
#else /* More relaxed receive timing (+/- 20%) */
#define MIN_TE      (TE     - (TE/5)) 		// minimum half bit time
#define MAX_TE      (TE     + (TE/5))  		// maximum half bit time
#define MIN_2TE     ((2*TE) - ((2*TE)/5))   // minimum full bit time
#define MAX_2TE     ((2*TE) + ((2*TE)/5))   // maximum full bit time
#endif



typedef enum daliMsgTypeTag
{
    DALI_MSG_UNDETERMINED    = 0,
    DALI_MSG_SHORT_ADDRESS   = 1,
    DALI_MSG_GROUP_ADDRESS   = 2,
    DALI_MSG_BROADCAST       = 4,
    DALI_MSG_SPECIAL_COMMAND = 8
} daliMsgType_t;

typedef enum answerTypeTag
{
    ANSWER_NOT_AVAILABLE = 0,
    ANSWER_NOTHING_RECEIVED,
    ANSWER_GOT_DATA,
    ANSWER_INVALID_DATA,
    ANSWER_TOO_EARLY
} answer_t;

/* state machine related definitions */
typedef enum stateTag
{
    MS_IDLE = 0,                        // bus idle
    MS_TX_SECOND_HALF_START_BIT,        // 
    MS_TX_DALI_FORWARD_FRAME,           // sending the dali forward frame
    MS_TX_STOP_BITS,                    //
    MS_SETTLING_BEFORE_BACKWARD,        // settling between forward and backward - stop bits
    MS_SETTLING_BEFORE_IDLE,            // settling before going to idle, after forward frame
    MS_WAITING_FOR_SLAVE_START_WINDOW,  // waiting for 7Te, start of slave Tx window
    MS_WAITING_FOR_SLAVE_START,         // start of slave Tx window
    MS_RECEIVING_ANSWER,                // receiving slave message
    MS_RECEIVING_ANSWER_FINISH          //finish receiving slave message
} MASTER_STATE;

/* definition of the captured edge data */
typedef struct capturedDataType_tag
{
    uint8   TECNT;                     //time stamp of signal edge
    uint8   capturedTime;             // time stamp of signal edge
    uint8   bitLevel;                 // bit level *after* the edge
    uint8   levelType;                // indication of long or short duration *after* the edge
} capturedDataType;

typedef struct capturedFrameType_tag
{
    capturedDataType  capturedData[MAX_BF_EDGES];
    uint8           capturedItems;    // counter of the captured edges
} capturedFrameType;

/***********************************************************/
/* Global variables                                        */
/***********************************************************/

static volatile uint16     ForwardFrame;
static volatile uint8      BackwardFrame; // DALI slave answer
static volatile answer_t     BackwardFrameAnswer; //slave answer state
static volatile MASTER_STATE masterState;
static volatile bool         waitForAnswer;
static volatile bool         earlyAnswer;
static volatile uint32     daliForwardFrame; // converted DALI master command
static volatile capturedFrameType     capturedFrame;    // data structure for the capture

/***********************************************************/
/* Local functions                                         */
/***********************************************************/

static inline bool DALI_CheckLogicalError(void)
{
    uint8  bitLevel;
    uint16 receivedFrame;
    uint32 bitStream, i, item, pattern, bitPair;

    // build frame from captured bit levels in bitStream
    bitStream = 0;
    for (i=0, item=0;  ((i < MAX_BF_EDGES) && (item < capturedFrame.capturedItems)); item++)
    {
        bitLevel = capturedFrame.capturedData[item].bitLevel;
        bitStream |= (bitLevel << ((MAX_BF_EDGES - 1) - i));
        i++;
        // shift another bit in case of long symbol
        if (capturedFrame.capturedData[item].levelType == 'l')
        {
            bitStream |= (bitLevel << ((MAX_BF_EDGES - 1) - i));
            i++;
        }
    }
    // check if there are 3 zeros or 3 ones in a row
    for (i=0; i < (MAX_BF_EDGES - 2); i++)
    {
        pattern = 7 << i;
        if (((bitStream & pattern) == 0) ||
            ((bitStream & pattern) == pattern))
        {
            return true; // error, invalid data, so return immediately
        }
    }
    // compose answer byte in receivedFrame
    receivedFrame = 0;
    for (i=0; i < MAX_BF_EDGES; i += 2)
    {
        receivedFrame <<= 1;
        bitPair = (bitStream >> ((MAX_BF_EDGES - 2) - i)) & 3;
        if ((bitPair == 0) || bitPair == 3)
        {
            return true; // error '00' or '11' is not a valid bit
        }
        if (bitPair == 1) receivedFrame |= 1;
    }
    // need to have the start bit in position 9 for a valid frame
    if (!(receivedFrame & 0x100)) return true;
    // cast out the start bit for the answer byte
    BackwardFrame = (uint8) receivedFrame;
    return false;
}

static inline bool DALI_CheckTimingError(void)
{
    uint8 i, capTECNT1, capTECNT2, capT1, capT2, interval;

    for (i=0; i < (capturedFrame.capturedItems - 1); i++)
    {
        capTECNT1 = capturedFrame.capturedData[i].TECNT;
        capTECNT2 = capturedFrame.capturedData[i+1].TECNT;
        capT1 = capturedFrame.capturedData[i].capturedTime;
        capT2 = capturedFrame.capturedData[i+1].capturedTime;
        if (capTECNT1 == capTECNT2)
        {
         interval = capT2 - capT1;
        }
        else
        {
         interval = 230 - capT1 + capT2 + (capTECNT2 - capTECNT1 - 1) * 230;
        }
        if ((interval >= MIN_TE) && (interval <= MAX_TE))
        {
            capturedFrame.capturedData[i].levelType = 's';
        }
        else if ((interval >= MIN_2TE) && (interval <= MAX_2TE))
        {
            capturedFrame.capturedData[i].levelType = 'l';
        }
        else
        {
            return true; // timing error, so stop check immediately
        }
    }
    capturedFrame.capturedData[i].levelType = 'x'; // terminate the frame
    return false;
}

static inline bool DALI_Decode(void)
{
    if (DALI_CheckTimingError()) return false;
    if (DALI_CheckLogicalError()) return false;
    return true;
}

static inline uint32 DALI_ConvertForwardFrame(uint16 forwardFrame)
{
    uint32 convertedForwardFrame = 0;
    int8   i;
    
    for (i=15; i>=0; i--)
    {
        if (forwardFrame & (1 << i))
        {   // shift in bits values '0' and '1'
            convertedForwardFrame <<= 1;
            convertedForwardFrame <<= 1;
            convertedForwardFrame  |= 1;
        }
        else
        {   // shift in bits values '1' and '0'
            convertedForwardFrame <<= 1;
            convertedForwardFrame  |= 1;
            convertedForwardFrame <<= 1;
        }
    }
    return convertedForwardFrame;
}

static inline daliMsgType_t DALI_CheckMsgType(uint16 forwardFrame)
{
    daliMsgType_t type = DALI_MSG_UNDETERMINED;
    
    if ((forwardFrame & 0x8000) == 0)
    {
        type = DALI_MSG_SHORT_ADDRESS;
    }
    else if ((forwardFrame & 0xE000) == 0x8000)
    {
        type = DALI_MSG_GROUP_ADDRESS;
    }
    else if ((forwardFrame & 0xFE00) == 0xFE00)
    {
        type = DALI_MSG_BROADCAST;
    }
    else if (((forwardFrame & 0xFF00) >= 0xA000) &&
             ((forwardFrame & 0xFF00) <= 0xFD00))
    {
        type = DALI_MSG_SPECIAL_COMMAND;
    }
    return type;
}

static inline bool DALI_CheckWaitForAnswer(uint16 forwardFrame, daliMsgType_t type)
{
    bool waitFlag = false;

    if (type == DALI_MSG_SPECIAL_COMMAND)
    {
        // Special commands
        if ((forwardFrame == COMPARE) ||
            ((forwardFrame & 0xFF81) == VERIFY_SHORT_ADDRESS) ||
            (forwardFrame == QUERY_SHORT_ADDRESS))
        {
            waitFlag = true;
        }
    }
    else
    {
        // Query commands
        if ((((forwardFrame & 0x01FF) >= CMD144) && ((forwardFrame & 0x01FF) <= CMD157)) ||
            (((forwardFrame & 0x01FF) >= CMD160) && ((forwardFrame & 0x01FF) <= CMD165)) ||
            (((forwardFrame & 0x01FF) >= CMD176) && ((forwardFrame & 0x01FF) <= CMD197)) ||
            ((forwardFrame & 0x01FF) == CMD255))
        {
            waitFlag = true;
        }
    }
    return waitFlag;
}

static inline bool DALI_CheckRepeatCmd(uint16 forwardFrame, daliMsgType_t type)
{
    bool repeatCmd = false;
    
    if (type == DALI_MSG_SPECIAL_COMMAND)
    {
        // Special commands 'initialize' and 'randomize' shall be repeated within 100 ms
        if (((forwardFrame & 0xFF00) == INITIALISE) ||
            (forwardFrame == RANDOMISE))
        {
            repeatCmd = true;
        }
    }
    else
    {
        // Configuration commands (32 - 129) shall all be repeated within 100 ms
        if (((forwardFrame & 0x01FF) >= CMD32) &&
            ((forwardFrame & 0x01FF) <= CMD129))
        {
            repeatCmd = true;
        }
    }
    return repeatCmd;
}

static inline void DALI_DoTransmission(uint32 convertedForwardFrame, bool waitFlag)
{
    //bsp_set_led(LED_RTX_DALI_BUS, 0); // LED OFF MEANS TX TO DALI BUS
    // Claim the bus and setup global variables
    masterState      = MS_TX_SECOND_HALF_START_BIT;
    waitForAnswer    = waitFlag;
    daliForwardFrame = convertedForwardFrame;
    DALI_SetOutputLow();
    // Activate the timer module to output the forward frame
    //SET_TIMER_REG_TC(0);       // clear timer
    SET_TIMER_REG_MR0(TE);     // ~ 2400 Hz (half bit time)
    SET_TIMER_REG_CCR(0);      // disable capture
    //SET_TIMER_REG_MCR(3);      // interrupt on MR0, reset timer on match 0
    SET_TIMER_START;      // enable the timer
    while (masterState != MS_IDLE)
    {
        // wait till transmission is completed
        // __WFI();
    }
    if (waitForAnswer)
    {
        if (capturedFrame.capturedItems == 0)
        {
            BackwardFrameAnswer = ANSWER_NOTHING_RECEIVED;
        }
        else if (earlyAnswer)
        {
            BackwardFrameAnswer = ANSWER_TOO_EARLY;
        }
        else
        {
            if (DALI_Decode())
            {
                BackwardFrameAnswer = ANSWER_GOT_DATA;
            }
            else
            {
                BackwardFrameAnswer = ANSWER_INVALID_DATA;
            }
        }
        //Do something to get out the answer. Then set BackwardFrameAnswer = ANSWER_NOT_AVAILABLE.
    }
}

void DALI_Send(uint16 forwardFrame,uint8 * pbackwardFrame,uint8 * pbackwardFrameAnswer)
{
    uint8        i = 0;
    uint8        n = 1;
    uint32       convertedForwardFrame = DALI_ConvertForwardFrame(forwardFrame);
    daliMsgType_t  daliMsgType = DALI_CheckMsgType(forwardFrame);
    bool           waitFlag = DALI_CheckWaitForAnswer(forwardFrame,daliMsgType);
    
    if (DALI_CheckRepeatCmd(forwardFrame,daliMsgType)) n = 2;
    while (i < n)
    {
        DALI_DoTransmission(convertedForwardFrame, waitFlag);
        i++;
    }
    *pbackwardFrameAnswer = BackwardFrameAnswer;
    *pbackwardFrame = BackwardFrame;
    BackwardFrameAnswer = ANSWER_NOT_AVAILABLE;
}

void DALI_Init(void)
{
    // First init ALL the global variables
    ForwardFrame         = 0;
    BackwardFrame        = 0;
    BackwardFrameAnswer  = ANSWER_NOT_AVAILABLE;
    masterState             = MS_IDLE;
    waitForAnswer           = false;
    earlyAnswer             = false;
    daliForwardFrame        = 0;
    capturedFrame.capturedItems = 0;

    //bsp_set_led(LED_RTX_DALI_BUS, 1); // LED ON MEANS RX FROM DALI BUS

    // Initialize the phisical layer of the dali master
    DALI_ConfigOutput();
    DALI_SetOutputHigh();
    DALI_ConfigInput();
    T3IE = 1;
    SET_TIMER_REG_PR(0xee); // timer runs at (32MHz/128)0.25 MHz - 4usec per tick; enable timer interrupt.
    T3CCTL0 |= 0x54;  //set channel0 in compare mode
    SET_TIMER_REG_IR(0xf8); // clear possible interrupt flag.
    T3IF = 0;
}

/***********************************************************/
/* Exported Counter/Timer IRQ handler                      */
/***********************************************************/

/* the handling of the protocol is done in the IRQ */
HAL_ISR_FUNCTION( Timer3Isr, T3_VECTOR )
{
 static uint8 bitcount, TEcnt;
        uint8 irq_stat;

    GET_TIMER_REG_IR(irq_stat);
    if (irq_stat & 2)
    {   // T3 overflow interrupt
        SET_TIMER_REG_IR(0xfd);   // clear T3 overflow interrupt flag
        if (masterState == MS_TX_SECOND_HALF_START_BIT)
        {
            DALI_SetOutputHigh();
            bitcount = 0;
            TEcnt = 0;
            masterState = MS_TX_DALI_FORWARD_FRAME;
        }
        else if (masterState == MS_TX_DALI_FORWARD_FRAME)
        {
            if (daliForwardFrame & 0x80000000)
            {
                DALI_SetOutputHigh();
            }
            else
            {
                DALI_SetOutputLow();
            }
            daliForwardFrame <<= 1;
            bitcount++;
            if (bitcount == 32) masterState = MS_TX_STOP_BITS;
        }
        else if (masterState == MS_TX_STOP_BITS)
        {
            DALI_SetOutputHigh();
            // the first half of the first stop bit has just been output.
            // do we have to wait for an answer?
            if (waitForAnswer)
            {   // elapse until the end of the last half of the second stop bit
                //SET_TIMER_REG_MR0(4*TE);
                TEcnt++;
                if (TEcnt == 4)
                {
                 BackwardFrame = 0;
                 earlyAnswer = false;
                 capturedFrame.capturedItems = 0;
                 TEcnt = 0;
                 masterState = MS_SETTLING_BEFORE_BACKWARD;
                }
            }
            else
            {   // no answer from slave expected, need to wait for the remaining
                // bus idle time before next forward frame
            	// add additional 3 TE to minimum specification to be not at the edge of the timing specification
                //SET_TIMER_REG_MR0((4*TE) + (22*TE) + (3*TE));
                TEcnt++;
                if (TEcnt == 29)
                {
                 TEcnt = 0;
                 masterState = MS_SETTLING_BEFORE_IDLE;
                }
            }
        }
        else if (masterState == MS_SETTLING_BEFORE_BACKWARD)
        {
            //bsp_set_led(LED_RTX_DALI_BUS, 1); // LED ON MEANS RX FROM DALI BUS
            // setup the first window limit for the slave answer
            // slave should not respond before 7TE
            if (TEcnt == 0)
            {
             SET_TIMER_REG_MR0(MIN_TE);
             SET_TIMER_REG_CCR(0x43);   // enable receive, capture on both edges
            }
            TEcnt++;
            if (TEcnt == 8)
            {
             TEcnt = 0;
             masterState = MS_WAITING_FOR_SLAVE_START_WINDOW;
            }
        }
        else if (masterState == MS_WAITING_FOR_SLAVE_START_WINDOW)
        {   // setup the second window limit for the slave answer,
            // slave must start transmit within the next 23TE window
            if (TEcnt == 0) SET_TIMER_REG_MR0(MAX_TE);
            TEcnt++;
            if (TEcnt == 23)
            {
             TEcnt = 0;
             masterState = MS_WAITING_FOR_SLAVE_START;
            }
        }
        else if (masterState == MS_WAITING_FOR_SLAVE_START)
        {   // if we still get here, got 'no' or too early answer from slave
            // idle time of 23TE was already elapsed while waiting, so
            // immediately release the bus
            SET_TIMER_STOP;   // reset and stop the timer
            SET_TIMER_REG_CCR(0);   // disable capture
            SET_TIMER_REG_IR(0xfb); // clear possible capture interrupt flag
            masterState = MS_IDLE;
        }
        else if (masterState == MS_RECEIVING_ANSWER)
        {   
            TEcnt++;
            uint8 wait = ( 22*MAX_TE ) /230 + 1;
            if (TEcnt == wait)
            {
             TEcnt = 0;
             masterState = MS_RECEIVING_ANSWER_FINISH;
            }
        }
        else if (masterState == MS_RECEIVING_ANSWER_FINISH)
        {   // stop receiving
            // now idle the bus between backward and next forward frame
            // since we don't track the last edge of received frame,
            // conservatively we wait for 23 TE (>= 22 TE as for specification)
            // Receive interval considered anyway the max tolerance for
            // backward frame duration so >22TE should already be asserted
            if (TEcnt == 0)
            {
             SET_TIMER_REG_MR0(TE);
             SET_TIMER_REG_CCR(0);   // disable capture
             SET_TIMER_REG_IR(0xfb); // clear possible capture interrupt flag
            }
            TEcnt++;
            if (TEcnt == 22)
            {
             TEcnt = 0;
             masterState = MS_SETTLING_BEFORE_IDLE;
            }
        }
        else if (masterState == MS_SETTLING_BEFORE_IDLE)
        {
            //bsp_set_led(LED_RTX_DALI_BUS, 1); // LED ON MEANS RX FROM DALI BUS
            SET_TIMER_STOP;   // reset and stop the timer
	    masterState = MS_IDLE;
        }
    }
    else if (irq_stat & 0x04)
    {   // capture interrupt
        P0DIR |= 0x02;//test
        P0_1 = 1;//test
        SET_TIMER_REG_IR(0xfb);     // clear capture interrupt flag
        if (masterState == MS_SETTLING_BEFORE_BACKWARD)
        {   // slave should not answer yet, it came too early!!!!
            SET_TIMER_REG_CCR(0);   // disable capture
            earlyAnswer = true;
        }
        else if (masterState == MS_WAITING_FOR_SLAVE_START_WINDOW)
        {   // we got an edge, so the slave is transmitting now
            // allowed remaining answering time is 22TE
            SET_TIMER_REG_MR0(230);//avoid T3 overfiow interrupt conflict with capture interrupt.
            TEcnt = 0;
            SET_TIMER_REG_TC;    //clear timer counter
            SET_TIMER_REG_IR(0xfd);    // clear possible T3 channel0 compare interrupt flag
            // first pulse is begin of the start bit
            DALI_GetInput(capturedFrame.capturedData[0].bitLevel);
            capturedFrame.capturedData[0].TECNT = 0;
            capturedFrame.capturedData[0].capturedTime = 0;
            capturedFrame.capturedItems = 1;
            masterState = MS_RECEIVING_ANSWER;
        }
        else if (masterState == MS_RECEIVING_ANSWER)
        {   // this part just captures the frame data, evaluation is done
            // at the end of max backward frame duration
            if (capturedFrame.capturedItems < MAX_BF_EDGES)
            {
                uint32 index = capturedFrame.capturedItems;
                DALI_GetInput(capturedFrame.capturedData[index].bitLevel);
                capturedFrame.capturedData[index].TECNT = TEcnt;
                GET_TIMER_REG_CR0(capturedFrame.capturedData[index].capturedTime);
                capturedFrame.capturedItems++;
            }
        }
    }
}

void DALI_Addressing(void)
{
 uint8 i ,j, k ,diff;
 uint8 halfword[2] = {0xf ,0xf};
 uint8 searchaddrcmd[3] = {0xb1 ,0xb3 ,0xb5};

 DALI_Send(INITIALISE,&dali_answer,&dali_answer_state);    //addressing initialise
 DALI_Send(RANDOMISE,&dali_answer,&dali_answer_state);    //produce random sequence
 MicroWait(0xd6d8);
 MicroWait(0xd6d8);
 MicroWait(0xd6d8);    //wait for the DALI slaver to produce random sequence
 dali_answer_state = ANSWER_NOTHING_RECEIVED;
 do
 {
   if(dali_answer_state != ANSWER_NOTHING_RECEIVED)
   {
     for (k=0 ;k<3 ;k++)
     {
      for (j=0 ;j<2 ;j++)
      {
        halfword[j] = 8;
        dali_cmd = searchaddrcmd[k]*256 + halfword[0]*16 + halfword[1];    //rebuild dali_cmd
        DALI_Send(dali_cmd,&dali_answer,&dali_answer_state);
        DALI_Send(COMPARE,&dali_answer,&dali_answer_state);    //compare
       for (i=1 ;i<4 ;i++)
       {
        diff = 8 >> i;
        if(dali_answer_state == ANSWER_NOTHING_RECEIVED)
          halfword[j] += diff;
        else
          halfword[j] -= diff;
        dali_cmd = searchaddrcmd[k]*256 + halfword[0]*16 + halfword[1];    //rebuild dali_cmd
        DALI_Send(dali_cmd,&dali_answer,&dali_answer_state);
        DALI_Send(COMPARE,&dali_answer,&dali_answer_state);    //compare
       }
       if((halfword[j] == 1)&&(dali_answer_state != ANSWER_NOTHING_RECEIVED))
       {
        halfword[j] = 0;
        dali_cmd = searchaddrcmd[k]*256 + halfword[0]*16 + halfword[1];    //rebuild dali_cmd
        DALI_Send(dali_cmd,&dali_answer,&dali_answer_state);
        DALI_Send(COMPARE,&dali_answer,&dali_answer_state);    //compare
        if(dali_answer_state == ANSWER_NOTHING_RECEIVED)
        {
         halfword[j] = 1;
         dali_cmd = searchaddrcmd[k]*256 + halfword[0]*16 + halfword[1];    //rebuild dali_cmd
         DALI_Send(dali_cmd,&dali_answer,&dali_answer_state);
        }
       }
       if(dali_answer_state == ANSWER_NOTHING_RECEIVED)
        {
         halfword[j] ++;
         dali_cmd = searchaddrcmd[k]*256 + halfword[0]*16 + halfword[1];    //rebuild dali_cmd
         DALI_Send(dali_cmd,&dali_answer,&dali_answer_state);
        }
      }
      halfword[0] = halfword[1] = 0xf;
     }
     DALI_Send((0xb701|(dalishortaddr << 1)),&dali_answer,&dali_answer_state);    //program short address
     dalishortaddr++;    //the next short address to allocate
     MicroWait(0xd6d8);
     MicroWait(0xd6d8);
     MicroWait(0xd6d8);    //wait for the DALI slaver to store short address
//     uint8 buf[1];
//     buf[0] = dalishortaddr;
//     zcl_SendCommand(pInMsg->msg->endPoint,&(pInMsg->msg->srcAddr),pInMsg->msg->clusterId,
//                     COMMAND_BALLAST_DALI_ADDRESSING_NOTIFY,1,!pInMsg->hdr.fc.direction,
//                     0,pInMsg->hdr.manuCode,pInMsg->hdr.transSeqNum,1,buf);//send a notice to controller after allocate DALI address.
     DALI_Send(0xab00,&dali_answer,&dali_answer_state);    //withdraw
   }
  
  
   halfword[0] = halfword[1] = 0xf;
   DALI_Send(0xb1ff,&dali_answer,&dali_answer_state);
   DALI_Send(0xb3ff,&dali_answer,&dali_answer_state);
   DALI_Send(0xb5ff,&dali_answer,&dali_answer_state);    //searching address (ffff)
   DALI_Send(COMPARE,&dali_answer,&dali_answer_state);    //compare
 }while(dali_answer_state != ANSWER_NOTHING_RECEIVED);
}
 


/* EOF */

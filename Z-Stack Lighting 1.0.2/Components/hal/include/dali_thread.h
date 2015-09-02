#ifndef _DALI_THREAD_H
#define _DALI_THREAD_H

#include "zcl.h"

#define true 1
#define false 0


// Configuration commands
#define CMD32                (0x0120)
#define CMD129               (0x0181)

// Query commands
#define CMD144               (0x0190)
#define CMD157               (0x019D)
#define CMD160               (0x01A0)
#define CMD165               (0x01A5)
#define CMD176               (0x01B0)
#define CMD197               (0x01C5)
#define CMD255               (0x01FF)

// Special commands
#define INITIALISE           (0xA500) // command for starting initialization mode
#define RANDOMISE            (0xA700) // command for generating a random address
#define COMPARE              (0xA900)
#define VERIFY_SHORT_ADDRESS (0xB901)
#define QUERY_SHORT_ADDRESS  (0xBB00)

extern uint16 dali_cmd;
extern uint8 dalishortaddr ,dali_answer ,dali_answer_state;

extern void DALI_Init(void);
extern void DALI_Send(uint16 forwardFrame,uint8 * pbackwardFrame,uint8 * pbackwardFrameAnswer);
extern void DALI_Addressing(void);

#endif /* _DALI_THREAD_H */

/* EOF */

#include  "hal_mcu.h"
#include  "hal_types.h"
#include "dali_thread.h"
#define ANSWER_NOTHING_RECEIVED 1

void DALI_Addressing(void)
{
 uint8 i ,j, k ,diff;
 uint8 halfword[2] = {0xf ,0xf};
 uint8 searchaddrcmd[3] = {0xb1 ,0xb3 ,0xb5};

 DALI_Send(INITIALISE,&dali_answer,&dali_answer_state);    //addressing initialise
 DALI_Send(RANDOMISE,&dali_answer,&dali_answer_state);    //produce random sequence
 dali_answer_state = ANSWER_NOTHING_RECEIVED;
 do
 {
   if(dali_answer_state != ANSWER_NOTHING_RECEIVED)
   {
     for (k=0 ;k<3 ;k++)
     {
      for (j=0 ;j<2 ;j++)
      {
       for (i=1 ;i<5 ;i++)
       {
        diff = 16 / (2^i);
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
     }
   }
  
   DALI_Send((0xb701|(dalishortaddr << 1)),&dali_answer,&dali_answer_state);    //program short address
   dalishortaddr++;    //the next short address to allocate
   DALI_Send(0xab00,&dali_answer,&dali_answer_state);    //withdraw
  
   halfword[0] = halfword[1] = 0xf;
   DALI_Send(0xb1ff,&dali_answer,&dali_answer_state);
   DALI_Send(0xb3ff,&dali_answer,&dali_answer_state);
   DALI_Send(0xb5ff,&dali_answer,&dali_answer_state);    //searching address (ffff)
   DALI_Send(COMPARE,&dali_answer,&dali_answer_state);    //compare
 }while(dali_answer_state != ANSWER_NOTHING_RECEIVED);
}
        
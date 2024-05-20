#include <xc.h>
#include "main.h"

extern int sec,min,return_time;

void __interrupt() isr(void)
{
    static unsigned int count = 0;
    
    if (TMR2IF == 1)
    {
        if (++count == 1250)
        {
           
            if(sec--==0 && min > 0)
            {      sec=60;
                   min--;
            }
            else if(return_time>0 && (min != 0))
                 return_time--;
             
            count = 0;
        }
        
        TMR2IF = 0;
    }
}
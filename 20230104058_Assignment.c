#include <AT89X51.h>

sbit LED = P1^0;

#define COUNT_LIMIT  58     
#define HOLD_TICKS   200   

#define TH0_RELOAD   0xD8
#define TL0_RELOAD   0xF0

#define TH1_RELOAD   0xFF
#define TL1_RELOAD   0xEC   

unsigned int  t0_count   = 0;
unsigned int  hold_count = 0;
bit           hold_mode  = 0;

void timer0_ISR(void) interrupt 1
{
    TH0 = TH0_RELOAD;
    TL0 = TL0_RELOAD;

    if (hold_mode)
    {
        hold_count++;
        if (hold_count >= HOLD_TICKS)   
        {
            hold_mode  = 0;
            hold_count = 0;
            LED        = 0;    
            t0_count   = 0;
            TR1        = 1;     
        }
    }
    else
    {
        t0_count++;
        if (t0_count >= COUNT_LIMIT)   
        {
            LED      = !LED;
            t0_count = 0;
        }
    }
}


void ext1_ISR(void) interrupt 2
{
    LED      = !LED;    
    t0_count = 0;      
}


void timer1_ISR(void) interrupt 3
{
    TR1        = 0;       
    TH1        = TH1_RELOAD;
    TL1        = TL1_RELOAD;

    LED        = 1;        
    hold_mode  = 1;
    hold_count = 0;
    t0_count   = 0;
  
}


void main(void)
{
    LED  = 0;

   
    TMOD = 0x51;

    TH0 = TH0_RELOAD;
    TL0 = TL0_RELOAD;

    TH1 = TH1_RELOAD;
    TL1 = TL1_RELOAD;

    IT1 = 1;    

    EX1 = 1;  
    ET0 = 1;   
    ET1 = 1;   
    EA  = 1;   

    TR0 = 1;  
    TR1 = 1;    

    while (1)
    {
       
    }
}
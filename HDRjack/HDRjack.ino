/*****************************************************************************/
/*                                                                           */
/* HDR-Jack - The ultracompact interval and HDR trigger                      */
/*                                                                           */
/* Copyright by Lukasz Panek, 2008                                           */
/* http://www.doc-diy.net                                                    */
/*                                                                           */
/* program with:                                                             */
/* avrdude -p t25 -c STK200 -i 500 -U flash:w:hdr-jack.hex -U lfuse:w:0x64:m */
/*                                                                           */
/*                                                                           */
/*                               _______                                     */
/*                             1|       |8 capacitor                         */
/*                  foccus PB3 2|       |7 PB2 pushbutton                    */
/*                             3|       |6                                   */
/*                         GND 4|_______|5 PB0 shutter                       */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

#define SHUTTER 0
#define PUSHBUTTON 2

#include <inttypes.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>


void (*restart)( void ) = 0x0000;  // for software reset


void wait_inter_125ms(uint16_t t) // wait for 125 ms, waiting is interruptible
{
  uint16_t i;
  uint8_t wakeup;
  WDTCR=0b11000011;              // set prescaler to get 0.125 s
  for(i=0;i<t;i++)
  {
    PORTB |= (1 << PUSHBUTTON);  // button pull-up --> allow INT0 again
    asm volatile("nop"::);       //one clock delay before reading PINB
    wakeup = PINB;               //PINB; // & (1<<PUSHBUTTON);
    PORTB &= ~(1 << PUSHBUTTON);
    if (~wakeup & (1 << PUSHBUTTON))
    {
      restart();
    } 
    sleep_mode();
  }
}

void wait_16ms(uint8_t t)         // wait for 16 ms, waiting is not interruptible
{
  uint8_t i;
  WDTCR=0b11000000; // 16ms s
  for(i=0;i<t;i++)
  {
    sleep_mode();
  }
}

void wait_125ms(uint8_t t)         // wait for 125 ms, waiting not interruptible
{
  uint8_t i;
  WDTCR=0b11000011; // 125ms s
  for(i=0;i<t;i++)
  {
    sleep_mode();
  }
}





EMPTY_INTERRUPT(WDT_vect);         // watchdog interrupt for waking up


int main(void)
{

  uint8_t old_state;
  uint8_t new_state;
  uint8_t push;
  uint8_t cycles_idle;
  uint16_t cycles_pressed;
  uint8_t wakeup;
  uint8_t i;
  uint8_t j;

  push = 0;

  PORTB = (0 << PUSHBUTTON)|(0<<SHUTTER); // button pull-up, shutter low
  DDRB  = 0b00000000;                     // all inputs


  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // set sleep mode to power down

  sei();                             // enable interrupts


  wait_125ms(8);

  old_state = (1 << PUSHBUTTON);
  cycles_pressed = 0;

  while(1)
  {
    wait_125ms(1);

    PORTB |= (1 << PUSHBUTTON); // button pull-up --> allow INT0 again
    asm volatile("nop"::);  //one clock delay before reading PINB

    new_state = PINB;
    PORTB &= ~(1 << PUSHBUTTON);

    if (~new_state & (1 << PUSHBUTTON))
    {
      if (cycles_pressed<65535)
      {
        cycles_pressed++;
      }

    }

    //if (~wakeup & (1 << PUSHBUTTON))
    if ((~old_state & (1 << PUSHBUTTON)) && (new_state & (1 << PUSHBUTTON)))
    {
      break;
    } 

    old_state = new_state;

  }

  if (cycles_pressed<=4)
  {
    push = 1;
  }

  old_state = (1 << PUSHBUTTON);
  cycles_idle = 6;
  while(1)
  {
    wait_125ms(1);
    PORTB |= (1 << PUSHBUTTON);
    asm volatile("nop"::);
    new_state = PINB;
    PORTB &= ~(1 << PUSHBUTTON);

    if (new_state & (1 << PUSHBUTTON))
    {
      cycles_idle--;
    }

    if ((~old_state & (1 << PUSHBUTTON)) && (new_state & (1 << PUSHBUTTON)))
    {
      push++;
      cycles_idle = 6;
    } 

    if ( cycles_idle == 0 )
    {
      break;
    }

    old_state = new_state;
  }




  wait_125ms(10);



  // decode pushbutton sequence

  if (push==0)          // variable interval 
  {
    wait_125ms(2);
    for(;;) // infinite loop
    {
      DDRB |= (1<<SHUTTER); // pull down shutter by switching to output (low level)
      wait_inter_125ms(2);
      DDRB &= ~(1<<SHUTTER); // release shutter by switching to hi-z
      wait_inter_125ms(cycles_pressed-2);
    }
  }
  else if (push==1)     // 2s delayed trigger
  {
    DDRB |= (1<<SHUTTER);
    wait_inter_125ms(4);
    DDRB &= ~(1<<SHUTTER);
    restart();
  }
  else if (push==2)     // HDR trigger
  {
    DDRB |= (1<<SHUTTER); 
    wait_16ms(2);   // 1/200 -> 30ms
    DDRB &= ~(1<<SHUTTER);

    wait_inter_125ms(16);

    DDRB |= (1<<SHUTTER); 
    wait_16ms(3);   // 1/60 -> 40ms
    DDRB &= ~(1<<SHUTTER);

    wait_inter_125ms(16);

    DDRB |= (1<<SHUTTER); 
    wait_16ms(6);   // 1/15 -> 100ms
    DDRB &= ~(1<<SHUTTER);

    wait_inter_125ms(16);

    DDRB |= (1<<SHUTTER); 
    wait_16ms(17);   // 1/4 -> 270ms
    DDRB &= ~(1<<SHUTTER);

    wait_inter_125ms(16);

    DDRB |= (1<<SHUTTER); 
    wait_inter_125ms(8);   // 1 -> 1s
    DDRB &= ~(1<<SHUTTER);

    wait_inter_125ms(16);

    DDRB |= (1<<SHUTTER); 
    wait_inter_125ms(32);   // 4 -> 4s
    DDRB &= ~(1<<SHUTTER);

    wait_inter_125ms(16);

    DDRB |= (1<<SHUTTER); 
    wait_inter_125ms(128);   // 16 -> 16s
    DDRB &= ~(1<<SHUTTER);
    restart();
  }
  else if (push==3)     // HDR trigger with mirror lock-up
  {
    wait_inter_125ms(16); // mirror up
    DDRB |= (1<<SHUTTER);
    wait_inter_125ms(1);	 
    DDRB &= ~(1<<SHUTTER);
    wait_inter_125ms(8);

    DDRB |= (1<<SHUTTER); 
    wait_16ms(6);   // 1/8 -> 100ms
    DDRB &= ~(1<<SHUTTER);

    wait_inter_125ms(16); // mirror up
    DDRB |= (1<<SHUTTER);
    wait_inter_125ms(1);	 
    DDRB &= ~(1<<SHUTTER);
    wait_inter_125ms(8);

    DDRB |= (1<<SHUTTER); 
    wait_inter_125ms(4); // 1/2 -> 500ms
    DDRB &= ~(1<<SHUTTER);

    wait_inter_125ms(16); // mirror up
    DDRB |= (1<<SHUTTER);
    wait_inter_125ms(1);	 
    DDRB &= ~(1<<SHUTTER);
    wait_inter_125ms(8);

    DDRB |= (1<<SHUTTER); 
    wait_inter_125ms(16); // 2 -> 2s
    DDRB &= ~(1<<SHUTTER);

    wait_inter_125ms(16); // mirror up
    DDRB |= (1<<SHUTTER);
    wait_inter_125ms(1);	 
    DDRB &= ~(1<<SHUTTER);
    wait_inter_125ms(8);

    DDRB |= (1<<SHUTTER); 
    wait_inter_125ms(64); // 8 -> 8s
    DDRB &= ~(1<<SHUTTER);

    wait_inter_125ms(16); // mirror up
    DDRB |= (1<<SHUTTER);
    wait_inter_125ms(1);	 
    DDRB &= ~(1<<SHUTTER);
    wait_inter_125ms(8);

    DDRB |= (1<<SHUTTER); 
    wait_inter_125ms(255); // 32 -> 32s
    DDRB &= ~(1<<SHUTTER);

    restart();
  }
  else if (push==4)     // BULB mode
  {
    DDRB |= (1<<SHUTTER);
    for(;;)
    {
      wait_inter_125ms(255);
    }
  }
  else                   // discrete interval timer mode
  {
    for(;;) // infinite loop
    {

      // the watchdog timer is not exact, it runs a few percent too slow at the given 
      // supply voltage. this error will be corrected by composing the 1 second 
      // interval from shorter intervals
      //  --> 1 physical second is 7*125ms + 6*16ms watchdog time


      DDRB |= (1<<SHUTTER); // pull down shutter wire by switching to output (low level)

      WDTCR=0b11000011; // 125ms

      sleep_mode();     // 125ms
      sleep_mode();     //+125ms = 250ms

      DDRB &= ~(1<<SHUTTER); // release shutter by switching to hi-z


      for(i=0;i<5;i++) // 5*125ms
      {
        sleep_mode(); // 5*125ms
        PORTB |= (1 << PUSHBUTTON);  // button pull-up --> allow INT0 again
        asm volatile("nop"::);       // one clock delay before reading PINB
        wakeup = PINB;               // check for user interruption
        PORTB &= ~(1 << PUSHBUTTON);
        if (~wakeup & (1 << PUSHBUTTON))
        {
          restart();
        } 
      }

      WDTCR=0b11000000; // 16ms
      for(i=0;i<5;i++)
      {
        sleep_mode(); // the rest
      }



      for(i=0;i<(push-5);i++)  // loop is entered if button has been pushed more then 5 times
      {
        WDTCR=0b11000011; // 125ms
        for(j=0;j<7;j++)
        {
          PORTB |= (1 << PUSHBUTTON); // button pull-up --> allow INT0 again
          asm volatile("nop"::);  //one clock delay before reading PINB
          wakeup = PINB;       //PINB; // & (1<<PUSHBUTTON);
          PORTB &= ~(1 << PUSHBUTTON);
          if (~wakeup & (1 << PUSHBUTTON))
          {
            restart();
          } 
          sleep_mode();
        }

        WDTCR=0b11000000; // 16ms
        for(j=0;j<6;j++)
        {
          sleep_mode();
        }
      }

    }
  }



}



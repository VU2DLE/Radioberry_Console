/**************************************
 * piHPSDR Controller for Radioberry
 * firmware V1.0
 * DE VU2DLE Dileep
 **************************************/
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/sync.h"


#define ENC1A 11
#define ENC1B 10

#define ENC2A 18
#define ENC2B 19

#define ENC3A 17
#define ENC3B 16

#define ENC4A 13
#define ENC4B 12

#define ENC5A 14
#define ENC5B 15

#define KPR0 2 
#define KPR1 3
#define KPR2 5
#define KPR3 7

#define KPC0 0
#define KPC1 1
#define KPC2 4
#define KPC3 6
#define KPCX 99 // undefined column number!

uint32_t int_status;

//--------------------------------------------------------------------------
#define DIR_NONE 0x0
// Clockwise step.
#define DIR_CW 0x10
// Anti-clockwise step.
#define DIR_CCW 0x20

#define R_START 0x0
//#define HALF_STEP

#ifdef HALF_STEP
// Use the half-step state table (emits a code at 00 and 11)
#define R_CCW_BEGIN 0x1
#define R_CW_BEGIN 0x2
#define R_START_M 0x3
#define R_CW_BEGIN_M 0x4
#define R_CCW_BEGIN_M 0x5
const unsigned char ttable[6][4] = {
  // R_START (00)
  {R_START_M,            R_CW_BEGIN,     R_CCW_BEGIN,  R_START},
  // R_CCW_BEGIN
  {R_START_M | DIR_CCW, R_START,        R_CCW_BEGIN,  R_START},
  // R_CW_BEGIN
  {R_START_M | DIR_CW,  R_CW_BEGIN,     R_START,      R_START},
  // R_START_M (11)
  {R_START_M,            R_CCW_BEGIN_M,  R_CW_BEGIN_M, R_START},
  // R_CW_BEGIN_M
  {R_START_M,            R_START_M,      R_CW_BEGIN_M, R_START | DIR_CW},
  // R_CCW_BEGIN_M
  {R_START_M,            R_CCW_BEGIN_M,  R_START_M,    R_START | DIR_CCW},
};
#else
// Use the full-step state table (emits a code at 00 only)
#define R_CW_FINAL  0x1
#define R_CW_BEGIN  0x2
#define R_CW_NEXT   0x3
#define R_CCW_BEGIN 0x4
#define R_CCW_FINAL 0x5
#define R_CCW_NEXT  0x6

const unsigned char ttable[7][4] = {
  // R_START
  {R_START,    R_CW_BEGIN,  R_CCW_BEGIN, R_START},
  // R_CW_FINAL
  {R_CW_NEXT,  R_START,     R_CW_FINAL,  R_START | DIR_CW},
  // R_CW_BEGIN
  {R_CW_NEXT,  R_CW_BEGIN,  R_START,     R_START},
  // R_CW_NEXT
  {R_CW_NEXT,  R_CW_BEGIN,  R_CW_FINAL,  R_START},
  // R_CCW_BEGIN
  {R_CCW_NEXT, R_START,     R_CCW_BEGIN, R_START},
  // R_CCW_FINAL
  {R_CCW_NEXT, R_CCW_FINAL, R_START,     R_START | DIR_CCW},
  // R_CCW_NEXT
  {R_CCW_NEXT, R_CCW_FINAL, R_CCW_BEGIN, R_START},
};
#endif

unsigned char ENC1state, ENC2state, ENC3state, ENC4state, ENC5state;
volatile int  ENC1NewState, ENC2NewState, ENC3NewState, ENC4NewState, ENC5NewState;
volatile int  KeyPressed =  false, Keyval = 0;
unsigned int col;
volatile uint kp_gpio = KPCX;
//unsigned int Keyval = 0;
unsigned int keypad_Row[4] = { KPR0, KPR1, KPR2, KPR3 };
unsigned int keypad_4X3[4][4] = {
                                  { 1,  2,  3,  4  }, 
                                  { 5,  6,  7,  8  }, 
                                  { 9,  10, 11, 12 }, 
                                  { 13, 14, 15, 16 }
                                };
// char keypad_msg[16][5] = {
//   "KM01", "KM02", "KM03", "KM04",  // { 1,  2,  3,  4 }
//   "KM05", "KM06", "KM07", "KM08",  // { 5,  6,  7,  8 }
//   "KM09", "KM10", "KM11", "KM12",  // { 9, 10, 11, 12 }
//   "KM13", "KM14", "KM15", "KM16"   // {13, 14, 15, 16 }
// };

char keypad_msg[16][5] = {
  "KM04", "KM08", "KM12", "KM16",  // { 1,  2,  3,  4 }
  "KM03", "KM07", "KM11", "KM15",  // { 5,  6,  7,  8 }
  "KM02", "KM06", "KM10", "KM14",  // { 9, 10, 11, 12 }
  "KM01", "KM05", "KM09", "KM13"   // {13, 14, 15, 16 }
};


void ENC1_Handler() {
  int s = 0;  
  int_status = save_and_disable_interrupts();
  if (ENC1NewState == 32)  {s = 1; }
  if (ENC1NewState == 16)  {s = -1;}
  ENC1NewState = 0;
  restore_interrupts(int_status);
  
  if(s != 0) {
    //Take action here 
    if (s == 1) {printf("EARI\n");}
    if (s == -1) {printf("EALE\n");}
  } 
}
void ENC2_Handler() {
  int s = 0;  
  int_status = save_and_disable_interrupts();
  if (ENC2NewState == 32)  {s = 1; }
  if (ENC2NewState == 16)  {s = -1;}
  ENC2NewState = 0;
  restore_interrupts(int_status);
  
  if(s != 0) {
    //Take action here 
    if (s == 1) {printf("EBRI\n");}
    if (s == -1) {printf("EBLE\n");}
  } 
}
void ENC3_Handler() {
  int s = 0;  
  int_status = save_and_disable_interrupts();
  if (ENC3NewState == 32)  {s = 1; }
  if (ENC3NewState == 16)  {s = -1;}
  ENC3NewState = 0;
  restore_interrupts(int_status);
  
  if(s != 0) {
    //Take action here 
    if (s == 1) {printf("ECRI\n");}
    if (s == -1) {printf("ECLE\n");}
  } 
}
void ENC4_Handler() {
  int s = 0;  
  int_status = save_and_disable_interrupts();
  if (ENC4NewState == 32)  {s = 1; }
  if (ENC4NewState == 16)  {s = -1;}
  ENC4NewState = 0;
  restore_interrupts(int_status);
  
  if(s != 0) {
    //Take action here 
    if (s == 1) {printf("EDRI\n");}
    if (s == -1) {printf("EDLE\n");}
  } 
}
void ENC5_Handler() {
  int s = 0;  
  int_status = save_and_disable_interrupts();
  if (ENC5NewState == 32)  {s = 1; }
  if (ENC5NewState == 16)  {s = -1;}
  ENC5NewState = 0;
  restore_interrupts(int_status);
  
  if(s != 0) {
    //Take action here 
    if (s == 1) {printf("EERI\n");}
    if (s == -1) {printf("EELE\n");}
  } 
}
void keypad_Handler() {
    gpio_put(KPR0, 0); gpio_put(KPR1, 0); gpio_put(KPR2, 0); gpio_put(KPR3, 0);
    if(kp_gpio == KPC0) col = 0; else if(kp_gpio == KPC1) col = 1; 
    else if(kp_gpio == KPC2) col = 2; else if(kp_gpio == KPC3) col = 3;
    for (unsigned int row = 0; ((row < 4) && (!Keyval)); row ++)
    {
      gpio_put(keypad_Row[row], 1);
      if (gpio_get(kp_gpio)) {
        sleep_ms(150);
        if (gpio_get(kp_gpio)) {
          Keyval = keypad_4X3[row][col]; 
          KeyPressed = true;
        }
      }
      gpio_put(keypad_Row[row], 0);
    }
    gpio_put(KPR0, 1); gpio_put(KPR1, 1); gpio_put(KPR2, 1); gpio_put(KPR3, 1);

    if (KeyPressed) {
      keypad_msg[Keyval - 1][4] = '\0';
      printf("%s\n", keypad_msg[Keyval - 1]);
      Keyval = 0; KeyPressed = false;
      kp_gpio = KPCX; 
    }
}

//--------------------------------------------------------------------------
void ENC1_callback_Handler(void) {
    unsigned char pinstate = (gpio_get(ENC1B) << 1) | gpio_get(ENC1A);
    // Determine new state from the pins and state table.
    ENC1state = ttable[ENC1state & 0xf][pinstate];
    // Return emit bits, ie the generated event
    pinstate = ENC1state & 0x30;
    if (pinstate) {ENC1NewState = (int)pinstate;}
}
void ENC2_callback_Handler(void) {
    unsigned char pinstate = (gpio_get(ENC2B) << 1) | gpio_get(ENC2A);
    // Determine new state from the pins and state table.
    ENC2state = ttable[ENC2state & 0xf][pinstate];
    // Return emit bits, ie the generated event
    pinstate = ENC2state & 0x30;
    //printf("pinstate = %d\n", pinstate);
    if (pinstate) {ENC2NewState = (int)pinstate;}
}
void ENC3_callback_Handler(void) {
    unsigned char pinstate = (gpio_get(ENC3B) << 1) | gpio_get(ENC3A);
    // Determine new state from the pins and state table.
    ENC3state = ttable[ENC3state & 0xf][pinstate];
    // Return emit bits, ie the generated event
    pinstate = ENC3state & 0x30;
    //printf("pinstate = %d\n", pinstate);
    if (pinstate) {ENC3NewState = (int)pinstate;}
}
void ENC4_callback_Handler(void) {
    unsigned char pinstate = (gpio_get(ENC4B) << 1) | gpio_get(ENC4A);
    // Determine new state from the pins and state table.
    ENC4state = ttable[ENC4state & 0xf][pinstate];
    // Return emit bits, ie the generated event
    pinstate = ENC4state & 0x30;
    //printf("pinstate = %d\n", pinstate);
    if (pinstate) {ENC4NewState = (int)pinstate;}
}
void ENC5_callback_Handler(void) {
    unsigned char pinstate = (gpio_get(ENC5B) << 1) | gpio_get(ENC5A);
    // Determine new state from the pins and state table.
    ENC5state = ttable[ENC5state & 0xf][pinstate];
    // Return emit bits, ie the generated event
    pinstate = ENC5state & 0x30;
    //printf("pinstate = %d\n", pinstate);
    if (pinstate) {ENC5NewState = (int)pinstate;}
}

void Keypad4X3_callback_Handler(uint gpio) {
  //printf("Keypad4X3_callback_Handler\n");
  kp_gpio = gpio;
}

void gpio_callback(uint gpio, uint32_t events) {
    //printf("gpio_callback gpio = %d\n",gpio);
    int_status = save_and_disable_interrupts();
    if( (gpio == ENC1A) || (gpio == ENC1B)) {
        ENC1_callback_Handler();
    }
    else if( (gpio == ENC2A) || (gpio == ENC2B)) {
        ENC2_callback_Handler();
    }
    else if( (gpio == ENC3A) || (gpio == ENC3B)) {
        ENC3_callback_Handler();
    }
    else if( (gpio == ENC4A) || (gpio == ENC4B)) {
        ENC4_callback_Handler();
    }
    else if( (gpio == ENC5A) || (gpio == ENC5B)) {
        ENC5_callback_Handler();
    }
    else if( (gpio == KPC0) || (gpio == KPC1) || (gpio == KPC2) || (gpio == KPC3)) {
        Keypad4X3_callback_Handler(gpio);
    }
    restore_interrupts(int_status);
}


int main() {

    stdio_init_all();

    //configuring Encoder1 A
    gpio_init(ENC1A);
    gpio_set_dir(ENC1A, GPIO_IN);
    gpio_pull_up(ENC1A);     
    //configuring Encoder1 B
    gpio_init(ENC1B);
    gpio_set_dir(ENC1B, GPIO_IN);
    gpio_pull_up(ENC1B);
    //ENC1 IRQ
    gpio_set_irq_enabled_with_callback(ENC1A, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(ENC1B, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    //configuring Encoder2 A
    gpio_init(ENC2A);
    gpio_set_dir(ENC2A, GPIO_IN);
    gpio_pull_up(ENC2A);     
    //configuring Encoder2 B
    gpio_init(ENC2B);
    gpio_set_dir(ENC2B, GPIO_IN);
    gpio_pull_up(ENC2B);
    //ENC2 IRQ
    gpio_set_irq_enabled_with_callback(ENC2A, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(ENC2B, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    //configuring Encoder3 A
    gpio_init(ENC3A);
    gpio_set_dir(ENC3A, GPIO_IN);
    gpio_pull_up(ENC3A);     
    //configuring Encoder3 B
    gpio_init(ENC3B);
    gpio_set_dir(ENC3B, GPIO_IN);
    gpio_pull_up(ENC3B);
    //ENC3 IRQ
    gpio_set_irq_enabled_with_callback(ENC3A, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(ENC3B, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL , true, &gpio_callback);

    //configuring Encoder4 A
    gpio_init(ENC4A);
    gpio_set_dir(ENC4A, GPIO_IN);
    gpio_pull_up(ENC4A);     
    //configuring Encoder4 B
    gpio_init(ENC4B);
    gpio_set_dir(ENC4B, GPIO_IN);
    gpio_pull_up(ENC4B);
    //ENC4 IRQ
    gpio_set_irq_enabled_with_callback(ENC4A, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL , true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(ENC4B, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL , true, &gpio_callback);

    //configuring Encoder5 A
    gpio_init(ENC5A);
    gpio_set_dir(ENC5A, GPIO_IN);
    gpio_pull_up(ENC5A);     
    //configuring Encoder5 B
    gpio_init(ENC5B);
    gpio_set_dir(ENC5B, GPIO_IN);
    gpio_pull_up(ENC5B);
    //ENC5 IRQ
    gpio_set_irq_enabled_with_callback(ENC5A, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL , true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(ENC5B, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL , true, &gpio_callback);


    /* ----------------------- Configure 4X4 Keypad ------------------------------------*/
    gpio_init(KPC0);
    gpio_set_dir(KPC0, GPIO_IN);
    gpio_pull_down(KPC0); 
    gpio_init(KPC1);
    gpio_set_dir(KPC1, GPIO_IN);
    gpio_pull_down(KPC1); 
    gpio_init(KPC2);
    gpio_set_dir(KPC2, GPIO_IN);
    gpio_pull_down(KPC2);     
    gpio_init(KPC3);
    gpio_set_dir(KPC3, GPIO_IN);
    gpio_pull_down(KPC3); 
    gpio_set_irq_enabled_with_callback(KPC0,   GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(KPC1,   GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(KPC2,   GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(KPC3,   GPIO_IRQ_EDGE_RISE, true, &gpio_callback);

    gpio_init(KPR0);
    gpio_set_dir(KPR0, GPIO_OUT);
    gpio_put(KPR0, 1);
    gpio_init(KPR1);
    gpio_set_dir(KPR1, GPIO_OUT);
    gpio_put(KPR1, 1);
    gpio_init(KPR2);
    gpio_set_dir(KPR2, GPIO_OUT);
    gpio_put(KPR2, 1);
    gpio_init(KPR3);
    gpio_set_dir(KPR3, GPIO_OUT);
    gpio_put(KPR3, 1);
    /* ----------------------- Configure 4X4 Keypad ------------------------------------*/

    /* --- while(1) forever scheduler! --- */
    while (1)
    {
        ENC1_Handler();
        ENC2_Handler();
        ENC3_Handler();
        ENC4_Handler();
        ENC5_Handler();
        if(kp_gpio != KPCX) keypad_Handler();
    }
    return 0;
}

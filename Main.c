#ifdef __CPU_MB90F823__
#else
#define __CPU_MB90F823__
#endif
#include "C:/Softune/lib/907/include/sample/MB90820/_ffmc16.h"
/*#include "_ffmc16.h"*/

#ifdef __CPU_MB90F823__
#else
#define __CPU_MB90F823__
#endif
#define czasIR 4500
__interrupt void DefaultIRQHandler(void);
__interrupt void INT_IRQHandler(void);
#pragma intvect INT_IRQHandler 20
#define DELAY 0xf
#define wys0 0x3f
#define wys1 0x06
#define wys2 0x5b
#define wys3 0x4f
#define wys4 0x66
#define wys5 0x6d

int step = 0;
int stepRpm = 0;
int wejscia[7]={0};
int count=0;

void RpmSet() {

  unsigned int engineRpm[3];
  engineRpm[0] = 0x1800;
  engineRpm[1] = 0x1900;
  engineRpm[2] = 0x2000;

  stepRpm++;
  switch (stepRpm) {
  default: {
    stepRpm = 0;
  }

  case 0: {
    /*  IO_PDR0.byte = 0; */
    IO_PCNT0.bit.POEN = 0; /*0-GPIO  1-PPG*/
    IO_DDR3.bit.P37 = 1;
    IO_PDR3.bit.P37 = 0;
    break;
  }
  case 1: {
    IO_PDR3.bit.P37 = 0;
    IO_DDR3.bit.P37 = 0;
    IO_PCNT0.bit.POEN = 1; /*0-GPIO  1-PPG*/
                           /*   IO_PDR0.byte = wys1; */
    IO_PDUT0 = engineRpm[stepRpm - 1];
    break;
  }
  case 2: {
    IO_PDR3.bit.P37 = 0;
    IO_DDR3.bit.P37 = 0;
    IO_PCNT0.bit.POEN = 1; /*0-GPIO  1-PPG*/
                           /*   IO_PDR0.byte = wys2; */
    IO_PDUT0 = engineRpm[stepRpm - 1];
    break;
  }
  case 3: {
    IO_PDR3.bit.P37 = 0;
    IO_DDR3.bit.P37 = 0;
    IO_PCNT0.bit.POEN = 1; /*0-GPIO  1-PPG*/
                           /*  IO_PDR0.byte = wys3; */
    IO_PDUT0 = engineRpm[stepRpm - 1];
    break;
  }
  case 4: {
    /*    IO_PDR0.byte = wys4; */
    IO_PCNT0.bit.POEN = 0; /*0-GPIO  1-PPG*/
    IO_DDR3.bit.P37 = 1;
    IO_PDR3.bit.P37 = 1;
    break;
  }
  }
}
void StepUp() {
  int o;
  switch (step) {
  case 0:
    IO_PDR3.bit.P33 = 1;
    for (o = 0; o < DELAY; o++)
      ;
    IO_PDR3.bit.P33 = 0;
    step = 1;
    break;
  case 1:
    IO_PDR3.bit.P34 = 1;
    for (o = 0; o < DELAY; o++)
      ;
    IO_PDR3.bit.P34 = 0;
    step = 2;
    break;
  case 2:
    IO_PDR3.bit.P35 = 1;
    for (o = 0; o < DELAY; o++)
      ;
    IO_PDR3.bit.P35 = 0;
    step = 3;
    break;
  case 3:
    IO_PDR3.bit.P36 = 1;
    for (o = 0; o < DELAY; o++)
      ;
    IO_PDR3.bit.P36 = 0;
    step = 0;
    break;
  }
}
void StepDown() {
  int o;
  switch (step) {
  case 2:
    IO_PDR3.bit.P33 = 1;
    for (o = 0; o < DELAY; o++)
      ;
    IO_PDR3.bit.P33 = 0;
    step = 1;
    break;
  case 3:
    IO_PDR3.bit.P34 = 1;
    for (o = 0; o < DELAY; o++)
      IO_PDR3.bit.P34 = 0;
    step = 2;
    break;
  case 0:
    IO_PDR3.bit.P35 = 1;
    for (o = 0; o < DELAY; o++)
      IO_PDR3.bit.P35 = 0;
    step = 3;
    break;
  case 1:
    IO_PDR3.bit.P36 = 1;
    for (o = 0; o < DELAY; o++)
      IO_PDR3.bit.P36 = 0;
    step = 0;
    break;
  }
}
void main(void) {
  /* inicjalizacja wyjsc silnika krokowego*/
  IO_DDR3.bit.P33 = 1;
  IO_DDR3.bit.P34 = 1;
  IO_DDR3.bit.P35 = 1;
  IO_DDR3.bit.P36 = 1;
  /*///////////////////////////*/
  /* inicjalizacja wyjsc wyswitlacza i wejsc ustawialnych*/
  IO_DDR8.byte = 0x0f;
  IO_DDR0.byte = 0x7f; /* 0b01111111; */
  IO_PDR8.bit.P80 = 0;
  IO_PDR8.bit.P81 = 0;
  IO_PDR8.bit.P82 = 0;
  IO_PDR8.bit.P83 = 0;
  /* ////////////////////////////////// */
  IO_PCNT0.bit.CNTE = 1;
  IO_PCNT0.bit.OSEL = 0; /*pol*/
  IO_PCNT0.bit.PGMS = 0;
  IO_PCNT0.bit.MDSE = 1; /*pwm mode=0*/
                         /*   IO_PCNT0.bit.STGR = 1;/*start licznik*/

  /* wyłaczenie silnika */
  IO_PCNT0.bit.POEN = 0;
  IO_DDR3.bit.P37 = 1;
  IO_PDR3.bit.P37 = 0;
 /*0-GPIO  1-PPG*/
                         /* /////////////////////////////////////////////// */

  IO_PCNT0.bit.CKS = 3; /*1/4 dzilnik*/
  IO_PCSR0 = 0x4008;    /*0x4008*/

  /*  przerwania INT 0,1 */
  IO_ENIR.bit.EN1 = 1;
  IO_ELVR.bit.LALB1 = 2;
  IO_ICR04.bit.IL = 6;
  __set_il(7); /* Set ILM to 7 */
               /* allow all interrupts levels */
  __EI();      /* enable interrupts at all */

  IO_PDUT0 = 0;
  while (1) {
    int i;
    IO_PDR8.bit.P80 = 1;   /* wysnr*/
    IO_PDR0.byte = 0;/*  */
    for (i = 0; i < 0xff; i++)
      ;
    IO_PDR0.byte = 0;
    IO_PDR8.bit.P80 = 0;
    IO_PDR8.bit.P81 = 1; /* wysnr2 */

    switch (stepRpm) {
    default: {
      break;
    }
    case 0: {
      IO_PDR0.byte = 0;
      break;
    }
    case 1: {
      IO_PDR0.byte = wys1;
      break;
    }
    case 2: {
      IO_PDR0.byte = wys2;
      break;
    }
    case 3: {
      IO_PDR0.byte = wys3;
      break;
    }
    case 4: {
      IO_PDR0.byte = wys4;
      break;
    }
    }
    for (i = 0; i < 0xff; i++)
      ;
    IO_PDR0.byte = 0;
    IO_PDR8.bit.P81 = 0;
    IO_PDR8.bit.P82 = 1; /* kontrolkiwentylator */
    IO_PDR0.byte = 0;/*  */
    for (i = 0; i < 0xff; i++)
      ;
    IO_PDR0.byte = 0;
    IO_PDR8.bit.P82 = 0;
    IO_PDR8.bit.P83 = 1; /* kontrolki */
    IO_PDR0.byte = 0;/*  */
    for (i = 0; i < 0xff; i++)
      ;
    IO_PDR0.byte = 0;
    IO_PDR8.bit.P83 = 0;

    if (IO_PDR4.bit.P42 == 1) {
      __DI();
      RpmSet();
      __EI();
      while (IO_PDR4.bit.P42 == 1)
        ;
      for (i = 0; i < 0xffff; i++)
        ;
    }

    /*         int a = 0;
            int i = 0x0;
            for (; i <= 0x4000; i++)
            {
                for (a=0;a < 0xff;a++);
                IO_PDUT0 = i;
            }
            for (; i >= 0x0; i--)
            {
                for (a=0;a < 0xff;a++);
                IO_PDUT0 = i;
            } */
  }
}

__interrupt void INT_IRQHandler(void) {

  if (IO_EIRR.bit.ER1 == 1) {
    IO_EIRR.bit.ER1 = 0;
    IO_PCNT0.bit.STGR = 1;
  }
  if(IO_PDR1.bit.P10 == 0){
    int a,b,kodId=0,kod=0;
    for(a=0;a<8;a++){
    count=0;
    while(IO_PDR1.bit.P10 == 0)count++;
    kodId=count<czasIR?0:1<<a;
  }
      for(a=0;a<8;a++){
    count=0;
    while(IO_PDR1.bit.P10 == 0)count++;
    if(((kodId^(count<czasIR?1:0))<<a)&(1<<a)) return;
  }
      for(a=0;a<8;a++){
    count=0;
    while(IO_PDR1.bit.P10 == 0)count++;
    kod=count<czasIR?0:1<<a;
  }
      for(a=0;a<8;a++){
    count=0;
    while(IO_PDR1.bit.P10 == 0)count++;
    if(((kod^(count<czasIR?1:0))<<a)&(1<<a)) return;
  }}
}

__interrupt void DefaultIRQHandler(void) {
  __DI(); /* disable interrupts */
  while (1)
    __wait_nop(); /* halt system */
}

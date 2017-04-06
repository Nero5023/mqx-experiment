#ifndef VECTORS_H_
#define VECTORS_H_


  /* Interrupt vector table type definition */
  typedef void (*const tIsrFunc)(void);
  typedef struct {
    void * __ptr;
    tIsrFunc __fun[0x2F];
  } tVectorTable;
  
  extern const tVectorTable __vect_table;
  
extern void __boot(void);
void Cpu_Interrupt(void);
void Cpu_INT_NMIInterrupt(void); 
extern void _int_kernel_isr(void);

//#define VECTOR_003 Cpu_Interrupt
//#define VECTOR_004 Cpu_Interrupt
//#define VECTOR_005 Cpu_Interrupt
//#define VECTOR_006 Cpu_Interrupt
//#define VECTOR_007 Cpu_Interrupt
//#define VECTOR_008 Cpu_Interrupt
//#define VECTOR_009 Cpu_Interrupt
//#define VECTOR_010 Cpu_Interrupt
//#define VECTOR_011 Cpu_Interrupt
//#define VECTOR_012 Cpu_Interrupt
//#define VECTOR_013 Cpu_Interrupt
//#define VECTOR_014 Cpu_Interrupt
//#define VECTOR_015 Cpu_Interrupt
//#define VECTOR_016 Cpu_Interrupt
//#define VECTOR_017 Cpu_Interrupt
//#define VECTOR_018 Cpu_Interrupt
//#define VECTOR_019 Cpu_Interrupt
//#define VECTOR_020 Cpu_Interrupt
//#define VECTOR_021 Cpu_Interrupt
//#define VECTOR_022 Cpu_Interrupt
//#define VECTOR_023 Cpu_Interrupt
//#define VECTOR_024 Cpu_Interrupt
//#define VECTOR_025 Cpu_Interrupt
//#define VECTOR_026 Cpu_Interrupt
//#define VECTOR_027 Cpu_Interrupt
//#define VECTOR_028 Cpu_Interrupt
//#define VECTOR_029 Cpu_Interrupt
//#define VECTOR_030 Cpu_Interrupt
//#define VECTOR_031 Cpu_Interrupt
//#define VECTOR_032 Cpu_Interrupt
//#define VECTOR_033 Cpu_Interrupt
//#define VECTOR_034 Cpu_Interrupt
//#define VECTOR_035 Cpu_Interrupt
//#define VECTOR_036 Cpu_Interrupt
//#define VECTOR_037 Cpu_Interrupt
//#define VECTOR_038 Cpu_Interrupt
//#define VECTOR_039 Cpu_Interrupt
//#define VECTOR_040 Cpu_Interrupt
//#define VECTOR_041 Cpu_Interrupt
//#define VECTOR_042 Cpu_Interrupt
//#define VECTOR_043 Cpu_Interrupt
//#define VECTOR_044 Cpu_Interrupt
//#define VECTOR_045 Cpu_Interrupt
//#define VECTOR_046 Cpu_Interrupt
//#define VECTOR_047 Cpu_Interrupt
  

#include "isr.h"
  
#endif /* VECTORS_H_ */

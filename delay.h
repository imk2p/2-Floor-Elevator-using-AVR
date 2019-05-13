//#define nop() asm("nop")
#define DelayUs(x) { volatile char y = x; while(y--) {nop(); nop(); nop(); nop();}}

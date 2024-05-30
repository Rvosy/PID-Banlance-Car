#ifndef __DELAY_H
#define __DELAY_H
#include <stdint.h>
void delay_init();
void delay_s(uint32_t s);

void delay_us(u32 nus);
void delay_ms(u16 nms);

#endif

#ifndef __TEST_H
#define __TEST_H

#include "stdint.h"

// 注意要使用这里面的函数一定要使能这个宏.
#define USE_TEST_FUNC 1

/*使用链表*/
// 这个选项和上面并列
// 注意里面使用了个大数组,分配链表空间
#define USE_LINK_LIST 1

#if USE_TEST_FUNC == 1
/*一些异常*/
void a1(void);
void a2(void);
void a3(void);
void a4(void);
void a5(void);

void test_ultrasonic(void);
float get_distance(void);
void test_led_init(void);
void ram_excute(uint32_t a);
void test_addition_of_polynomials(void);

#endif


#endif
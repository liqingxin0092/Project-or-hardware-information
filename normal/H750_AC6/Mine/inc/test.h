#ifndef __TEST_H
#define __TEST_H

#include "stdint.h"

// ע��Ҫʹ��������ĺ���һ��Ҫʹ�������.
#define USE_TEST_FUNC 1

/*ʹ������*/
// ���ѡ������沢��
// ע������ʹ���˸�������,��������ռ�
#define USE_LINK_LIST 1

#if USE_TEST_FUNC == 1
/*һЩ�쳣*/
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
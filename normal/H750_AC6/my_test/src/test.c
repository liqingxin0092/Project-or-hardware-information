#include "test.h"

#if USE_TEST_FUNC == 1

#include "stdint.h"
#include "stdio.h"
#include "gpio.h"
#include "timer_m.h"
#include "data_structure_m.h"

/*非法读取*/
void a1(void)
{
    volatile uint32_t *p;
    uint32_t n;
    p = (uint32_t *)0xcccccccc;
    n = *p;
    (void)n;
}

/*非对齐访问*/
void a2(void)
{
    float *ptr;
    float fTest;
    ptr = (float *)(0x20010002);
    *ptr = 0.2f;
    *ptr += 0.1f;
    fTest = *ptr;
    (void)fTest;
}
/*非法写入*/
void a3(void)
{
    volatile uint32_t *p;
    uint32_t n;
    p = (uint32_t *)0xcccccccc;
    *p = 100;
    (void)n;
}
/*非法函数状态*/
void a4(void)
{
    typedef void (*t_funcPtr)(void);
    t_funcPtr MyFunc = (t_funcPtr *)(0x0001000 | 0x0);
    MyFunc();
}

/*除以0的问题*/
void a5(void)
{
    volatile int r;
    volatile unsigned int a;
    volatile unsigned int b;
    a = 1;
    b = 0;
    r = a / b;
    (void)r;
}

/*测试超声波模块*/
void test_ultrasonic(void)
{
    gpio_init_pin(GPIO_C, 7, 0, 0, 0, 0, 0); // echo
    gpio_init_pin(GPIO_C, 6, 1, 0, 2, 0, 0); // triger
}

/*超声波得到距离cm*/
// PC6
// PC7
float get_distance(void)
{
    float return_val;
    float cnt;

    GPIO_OUT(GPIO_C, 6, 1);
    delay_us(20);
    GPIO_OUT(GPIO_C, 6, 0);

    while (!GPIO_IN(GPIO_C, 7))
        ;

    soft_timer_start_measure();
    while (GPIO_IN(GPIO_C, 7))
        ;
    cnt = soft_timer_get_time();

    delay_ms(200);
    return cnt * 0.017;
}

/*一排灯四个*/
// PA0
// PA1
// PA2
// PA3
void test_led_init(void)
{
    gpio_init_pin(GPIO_A, 0, 1, 0, 0, 0, 0);
    gpio_init_pin(GPIO_A, 1, 1, 0, 0, 0, 0);
    gpio_init_pin(GPIO_A, 2, 1, 0, 0, 0, 0);
    gpio_init_pin(GPIO_A, 3, 1, 0, 0, 0, 0);

    GPIO_OUT(GPIO_A, 0, 1); // 默认不亮
    GPIO_OUT(GPIO_A, 1, 1);
    GPIO_OUT(GPIO_A, 2, 1);
    GPIO_OUT(GPIO_A, 3, 1);
}

/*单独函数代码放到外部执行*/
__attribute__((section("ex_flash"))) void ram_excute(uint32_t a)
{
    a++;
    (void)a;
    printf("nihao");
}
#endif


/*模拟多项式合并*/
static link_node_t *addition_of_polynomials(link_node_t *a, link_node_t *b)
{
    link_node_t *tempa = a;
    link_node_t *tempb = b;
    link_node_t *tempc = link_list_creat();
    content_t tempc_content;
    while (tempa->next != NULL || tempb->next != NULL) // 两个当中存在有数的
    {
        if (tempa->next != NULL && tempb->next != NULL) // 两个都还有数
        {
            if (tempa->next->content.power > tempb->next->content.power)
            {
                link_list_add_node(tempc, tempb->next->content);
                tempb = tempb->next;
            }
            else if (tempa->next->content.power == tempb->next->content.power)
            {
                if ((tempa->next->content.coefficient + tempb->next->content.coefficient) != 0)
                {
                    tempc_content.coefficient = tempa->next->content.coefficient + tempb->next->content.coefficient;
                    tempc_content.power = tempa->next->content.power;
                    link_list_add_node(tempc, tempc_content);
                }
                tempb = tempb->next;
                tempa = tempa->next;
            }
            else if (tempa->next->content.power < tempb->next->content.power)
            {
                link_list_add_node(tempc, tempa->next->content);
                tempa = tempa->next;
            }
        }
        else if (tempa->next != NULL && tempb->next == NULL)
        {
            link_list_add_node(tempc, tempa->next->content);
            tempa = tempa->next;
        }
        else if (tempa->next == NULL && tempb->next != NULL)
        {
            link_list_add_node(tempc, tempb->next->content);
            tempb = tempb->next;
        }
    }
    return tempc;
}

/*链表的例子*/
// 多项式相加
void test_addition_of_polynomials(void)
{
    // a:17  +2X^2     +4X^5-7X^7       +19X^15
    link_node_t *a = link_list_creat();
    content_t content = {17, 0};
    link_list_add_node(a, content);
    content.coefficient = 2;
    content.power = 2;
    link_list_add_node(a, content);
    content.coefficient = 4;
    content.power = 5;
    link_list_add_node(a, content);
    content.coefficient = -7;
    content.power = 7;
    link_list_add_node(a, content);
    content.coefficient = 19;
    content.power = 15;
    link_list_add_node(a, content);
    // b:    -3X^2+3X^3+8X^5+7X^7+21X^14
    link_node_t *b = link_list_creat();
    content.coefficient = -3;
    content.power = 2;
    link_list_add_node(b, content);
    content.coefficient = 3;
    content.power = 3;
    link_list_add_node(b, content);
    content.coefficient = 8;
    content.power = 5;
    link_list_add_node(b, content);
    content.coefficient = 6;
    content.power = 7;
    link_list_add_node(b, content);
    content.coefficient = 21;
    content.power = 14;
    link_list_add_node(b, content);
    content.coefficient = 21;
    content.power = 26;
    link_list_add_node(b, content);
    /*合并*/
    link_node_t *c = addition_of_polynomials(a, b);
    link_list_show(a);
    link_list_show(b);
    link_list_show(c);
}


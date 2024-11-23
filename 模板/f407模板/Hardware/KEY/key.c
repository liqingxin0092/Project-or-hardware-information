#include "key.h"
#include "delay.h"

void KEY_Init(void)
{
	/*GPIOB8,9初始化*/
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT; // 输入
	GPIO_InitStruct.Pull = GPIO_PULLUP;		// 上拉
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*GPIOA0初始化*/
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN; // 下拉
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

// 按键处理函数
// 返回按键值
// mode:0,不支持连续按;1,支持连续按;
// 0，没有任何按键按下
// 1，KEY0按下
// 2，KEY1按下
// 4，WKUP按下 WK_UP
// 注意此函数有响应优先级,KEY0>KEY1>WK_UP!!
u8 KEY_Scan(u8 mode)
{
	static u8 key_up = 1; // 按键按松开标志
	if (mode)
		key_up = 1; // 支持连按
	if (key_up && (KEY0 == 0 || KEY1 == 0 || WK_UP == 1))//上次松开并且这回有一个摁
	{
		delay_ms(10); // 去抖动
		key_up = 0;//说明摁了
		if (KEY0 == 0)
			return 1;
		else if (KEY1 == 0)
			return 2;
		else if (WK_UP == 1)
			return 4;
	}
	else if (KEY0 == 1 && KEY1 == 1 && WK_UP == 0)//全松
		key_up = 1;
	return 0; // 无按键按下
}

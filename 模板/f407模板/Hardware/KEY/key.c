#include "key.h"
#include "delay.h"

void KEY_Init(void)
{
	/*GPIOB8,9��ʼ��*/
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT; // ����
	GPIO_InitStruct.Pull = GPIO_PULLUP;		// ����
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*GPIOA0��ʼ��*/
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN; // ����
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

// ����������
// ���ذ���ֵ
// mode:0,��֧��������;1,֧��������;
// 0��û���κΰ�������
// 1��KEY0����
// 2��KEY1����
// 4��WKUP���� WK_UP
// ע��˺�������Ӧ���ȼ�,KEY0>KEY1>WK_UP!!
u8 KEY_Scan(u8 mode)
{
	static u8 key_up = 1; // �������ɿ���־
	if (mode)
		key_up = 1; // ֧������
	if (key_up && (KEY0 == 0 || KEY1 == 0 || WK_UP == 1))//�ϴ��ɿ����������һ����
	{
		delay_ms(10); // ȥ����
		key_up = 0;//˵������
		if (KEY0 == 0)
			return 1;
		else if (KEY1 == 0)
			return 2;
		else if (WK_UP == 1)
			return 4;
	}
	else if (KEY0 == 1 && KEY1 == 1 && WK_UP == 0)//ȫ��
		key_up = 1;
	return 0; // �ް�������
}

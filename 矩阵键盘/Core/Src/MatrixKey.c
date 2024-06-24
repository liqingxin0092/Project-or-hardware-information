#include "main.h"                 
#include "MatrixKey.h"
uint8_t MatrixKey_determine(void);

int8_t KeyArr[3];
uint8_t MatrixKey_GetValue()
{
	uint8_t Key;
    
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET );
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET );
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET );
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET );

	Key =  MatrixKey_determine();
    
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET );
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET );
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET );
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET );
	
	if(MatrixKey_determine())
	{
		Key = MatrixKey_determine() + 4;
	}
	
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET );
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET );
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET );
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET );
	

	if(MatrixKey_determine())
	{
		Key = MatrixKey_determine() + 8;
	}
	
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET );
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET );
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET );
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET );
	
	
	if(MatrixKey_determine())
	{
		Key = MatrixKey_determine() + 12;
	}
    
	return Key;
}

uint8_t MatrixKey_determine()
{
	uint8_t Key = 0;
	if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6 ) == 0)
	{
		HAL_Delay(10);
		if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6 ) == 0){Key = 1;}
	}
	
	if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7 ) == 0)
	{
		HAL_Delay(10);
		if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7 ) == 0){Key = 2;}
	}
	
	if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8 ) == 0)
	{
		HAL_Delay(10);
		if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8 ) == 0){Key = 3;}
	}
	
	if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9 ) == 0)
	{
		HAL_Delay(10);
		if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9 ) == 0){Key = 4;}
	}
	return Key;
}

int16_t Remapping(uint8_t keynum)
{
    if(1<=keynum&&keynum<=3)
        return keynum;
    if(keynum==4)
        return 0xff;
 
    else if(  5<=keynum&&keynum<=7)
        return keynum-1;
    else if(keynum==8)
        return 0xfe;

    else if(  9<=keynum&&keynum<=11)
        return keynum-2;
    else if(keynum==12)
        return 0xfd;
     else if(keynum==13)
        return 0xfc;
     else if(keynum==14)
        return 0;
     else if(keynum==15)
        return 0xfb;
     else if(keynum==16)
        return 0xfa;
     
     return 20;
}

uint16_t num;
uint8_t KeyNum;

void Get_Num()
{ 
    uint8_t i;
    OLED_ShowNum(0, 0,  0,1 ,OLED_6X8);
   OLED_ShowString(0, 8, "Input:", OLED_6X8);
   OLED_Update();
    
    KeyNum=20; //设成20，以进入  #循环
    
    
    for(i=0;i<3;i++)
    {
        while(KeyNum==20)   //   #循环
        {
            KeyNum =Remapping(MatrixKey_GetValue());    //输入1个数
        }
            
         KeyArr[i]=KeyNum;         //跳出循环，此时 数组存下用户输入的数字
        
         OLED_ShowNum(0, 0,  KeyNum,1 ,OLED_6X8);
         OLED_Update();
        
        KeyNum=20;    //设成20，以进入  #循环  
        
        HAL_Delay(400);

   }
    
   num=KeyArr[0]*100+KeyArr[1]*10+KeyArr[2];
   
   OLED_ClearArea(0, 0, 6, 8);
   OLED_ShowNum(36, 8,  num,3 ,OLED_6X8);
   OLED_Update();
}

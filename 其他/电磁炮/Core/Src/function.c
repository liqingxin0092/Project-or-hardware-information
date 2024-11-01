#include "function.h"
#include <math.h>
char String[30];
void USART_Printf( UART_HandleTypeDef * chuankou,char *format, ...)         //串口发送
{					
	va_list arg;						
	va_start(arg, format);					
	vsprintf(String, format, arg);		
	va_end(arg);					     
	HAL_UART_Transmit(chuankou, (uint8_t*)String, strlen(String),50);
}




void set_Angle1_Up(double Angle)  //设置上面那个舵机角度
{
	__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_1,(int)(Angle/45*500+1500));
}

void set_Angle1_Down(double Angle) //设置底下那个舵机角度
{
	__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_2,(int)(Angle/45*500+1500));
}


extern int Target_True;
int8_t angleN;
#define DELAY_TIME   30 
void scan_mood(void)                   //循环扫描，内部会退出
{
	
   for(uint8_t i=0;i<3;i++)
	{
					for(angleN=0;angleN<60;angleN++)
					{
						if(Target_True == 1)
							goto exit;
						HAL_Delay(DELAY_TIME);
						set_Angle1_Down(angleN);
					}
					for(angleN=60;angleN>-60;angleN--)
					{
						if(Target_True == 1)
							goto exit;
						HAL_Delay(DELAY_TIME);
						set_Angle1_Down(angleN);
					}
					for(angleN=-60;angleN<0;angleN++)
					{
						if(Target_True == 1)
							goto exit;
						HAL_Delay(DELAY_TIME);
						set_Angle1_Down(angleN);
					}
		
	}
	exit:while(0);
			
}


double angle_AutoSet;

void AutoCarryOut()             //找到目标后的操作
{
	float FirstAngle =  -AutoAngle; //记录首次角度  （树莓派角度 与 舵机角度 映射关系为 相反数）
	float FirstDistance =  -AutoDisatance; //记录首次角度
	
 	double OffsetAngle=4;        //角度补偿机制
	double OffsetDistance=-50;        //默认值
	
	if(200<FirstDistance-50&&OffsetDistance<=220)    //根据距离补偿
		OffsetDistance=-46;
	else if(220<FirstDistance-50&&OffsetDistance<=280)
		OffsetDistance=-55;
	else if(280<FirstDistance-50)
		OffsetDistance=-58;
	if(FirstAngle>0&&4<fabs(FirstAngle)&&fabs(FirstAngle)<=15)
    {
       OffsetAngle=2.2;
       OffsetDistance=-60;
    }
    else if(FirstAngle<0&&4<fabs(FirstAngle)&&fabs(FirstAngle)<=15)
    {
       OffsetAngle=3.5;
       OffsetDistance=-53;
    }
	else if(FirstAngle>0&&15<fabs(FirstAngle))
	{
		 OffsetAngle=3.0;
		OffsetDistance=-62;
	}
    else if(FirstAngle<0&&15<fabs(FirstAngle))
	{
		 OffsetAngle=4.1;
		OffsetDistance=-50;
	}
        
    
	HAL_Delay(500);
	float a; 
	//粗略转向树莓派发送的角度
	angle_AutoSet=   -AutoAngle;
	set_Angle1_Down(angle_AutoSet);  
	
	HAL_Delay(500);
	
	//略微精确转向树莓派发送的角度
	while(Target_True!=1)       
	{
		if(Target_True==2)
		{
			angle_AutoSet+=0.5;
			set_Angle1_Down(angle_AutoSet);
            HAL_Delay(100);	
//			TurnFlag=1;
		}
		else if(Target_True==3)
		{
			angle_AutoSet-=0.5;
			set_Angle1_Down(angle_AutoSet); 
			HAL_Delay(100);	
//			TurnFlag=2;
		}
        a=AutoDisatance;  
	}
		//结合实际微tiao
	angle_AutoSet+=OffsetAngle;
	set_Angle1_Down(angle_AutoSet);
	
	
	
		HAL_Delay(1000);  

    	set_Angle1_Up(Dis_To_Ang1(InpuD_To_ModiD(a+OffsetDistance)));//   最初  修正

				HAL_Delay(1000);
				lOOOms_charge();      // 自动充能1秒
				
				HAL_Delay(500);
				shoot();             //射击
				
				HAL_Delay(2500);
				set_Angle1_Up(0); //回归初始角度
				set_Angle1_Down(0);
				
				Target_True=0;
}

struct unit           //实验数据库
{
	double angle;
	double distance;
}experimental_data[] = { {-3,190},                                  //基于充电1000ms


	{-4,195},  {-5,201},  {-6,210},  {-7,218},  {-8,224},      //-12,-13,-14,-15,-16
	{-9,228},  {-10,231}, {-11,235}, {-12,241}, {-13,245},     //-17,-18,-19,-20,-21
	{-14,250}, {-15,254}, {-16,259}, {-17,262}, {-18,266},     //-22,-23,-24,-25,-26
	{-19,270}, {-20,274}, {-21,278}, {-22,282}, {-23,284},     //-27,-28,-29,-30,-31
	{-24,288}, {-25,290}, {-26,295}, {-27,301}, {-28,310},     //-32,-33,-34,-35,-36

	{-29,320},
};


double Dis_To_Ang2(double dis)         //点线
{
	int8_t i = 0;
	while (experimental_data[i].distance < dis)
		i++;
	return experimental_data[i - 1].angle - (dis - experimental_data[i - 1].distance) / (experimental_data[i].distance - experimental_data[i - 1].distance);
}

double DTA_3(double angle)        //三次
{
	return 1.487e-05 * angle * angle * angle - 0.01187 * angle * angle + 2.895 * angle - 228.1;
}

double Dis_To_Ang1(double dis)         //  最初
{
	int8_t i = 0;
	while (experimental_data[i].distance < dis)
		i++;
	if (fabs(experimental_data[i].distance - dis) > fabs(experimental_data[i - 1].distance - dis))
    {
     	OLED_ShowString(0, 40, "i:", OLED_6X8);
        OLED_ShowNum(75, 40, i,3, OLED_6X8);     //显示转化后的角度
     	OLED_ShowString(0, 0, "Dis To Ang:", OLED_6X8);
        OLED_ShowFloatNum(75, 0, experimental_data[i - 1].angle,3, 2, OLED_6X8);     //显示转化后的角度
		return experimental_data[i - 1].angle;
        
    }
	else
    {
     	OLED_ShowString(0, 40, "i", OLED_6X8);
        OLED_ShowNum(75, 40, i,3, OLED_6X8);     //显示转化后的角度
        OLED_ShowString(0, 0, "Dis To Ang:", OLED_6X8);
        OLED_ShowFloatNum(75, 0,experimental_data[i].angle,3, 2, OLED_6X8);     //显示转化后的角度
        return experimental_data[i].angle;
        
    }
}


uint16_t InpuD_To_ModiD(uint16_t dis)       //       最初 修正 
{
	if(185<=dis&&dis<=194)
		dis+=1;
	
	else if(195<=dis&&dis<=207)
		while(0);
	
	else if(208<=dis&&dis<=226)
		dis+=2;
		
	else if(227<=dis&&dis<=237)
		dis-=3;
			
	else if(238<=dis&&dis<=250)
		dis-=8;
	else if(251<=dis&&dis<=270)
		dis-=17;		
	else if(271<=dis&&dis<=280)
		dis-=18;
	
	else if(281<=dis&&dis<=290)
		dis-=20;
	
	else if(291<=dis&&dis<=300)
		 dis-=25;
	
	else if(301<=dis)
			dis-=25;
	return dis;
}



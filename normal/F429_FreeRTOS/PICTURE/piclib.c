#include "piclib.h"
#include "ltdc.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//图片解码 驱动代码-gif解码部分
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2019/5/28
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved
//********************************************************************************
//升级说明 
//无
//////////////////////////////////////////////////////////////////////////////////


extern u32 *ltdc_framebuf[2];	//LTDC LCD帧缓存数组指针,必须指向对应大小的内存区域
	
_pic_info picinfo;	 	//图片信息
_pic_phy pic_phy;		//图片显示物理接口	
//////////////////////////////////////////////////////////////////////////
//lcd.h没有提供划横线函数,需要自己实现
void piclib_draw_hline(u16 x0,u16 y0,u16 len,u16 color)
{
	if((len==0)||(x0>LTDC_PWIDTH)||(y0>LTDC_PHEIGHT))return;
	DMA2D_fill(x0,y0,x0+len-1,y0,color);	
}
//填充颜色
//x,y:起始坐标
//width，height：宽度和高度。
//*color：颜色数组
void piclib_fill_color(u16 x,u16 y,u16 width,u16 height,u16 *color)
{  
	u16 i,j;
	u32 param1;	
	u32 param2;
	u32 param3;
	u16* pdata;
	u32 *plcd;
	if(LTDC_PWIDTH!=0&&DISPLAY_DIR==0)//如果是RGB屏,且竖屏,则填充函数不可直接用
	{
		plcd=(u32 *)RGB_GRAM;	//指向RGBLCD当前显存
		param1=2*LTDC_PWIDTH*(LTDC_PHEIGHT-x-1)+2*y;	//将运算先做完,提高速度
		param2=2*LTDC_PWIDTH;
		for(i=0;i<height;i++)
		{
			param3=i*2+param1;
			pdata=color+i*width;
			for(j=0;j<width;j++)
			{ 
				*(u16*)((u32)plcd+param3-param2*j)=pdata[j]; 
			}
		}
	}else DMA2D_fill_color(x,y,x+width-1,y+height-1,color);//其他情况,直接填充	
} 
//////////////////////////////////////////////////////////////////////////
//画图初始化,在画图之前,必须先调用此函数
//指定画点/读点
void piclib_init(void)
{
	pic_phy.read_point=LTDC_read_point;  		//读点函数实现,仅BMP需要
	pic_phy.draw_point=LTDC_DrawPoint;	//画点函数实现
	pic_phy.fill=DMA2D_fill;					//填充函数实现,仅GIF需要
	pic_phy.draw_hline=piclib_draw_hline;  	//画线函数实现,仅GIF需要
	pic_phy.fillcolor=piclib_fill_color;  	//颜色填充函数实现,仅TJPGD需要 

	picinfo.lcdwidth=1024;	//得到LCD的宽度像素
	picinfo.lcdheight=600;//得到LCD的高度像素

	picinfo.ImgWidth=0;	//初始化宽度为0
	picinfo.ImgHeight=0;//初始化高度为0
	picinfo.Div_Fac=0;	//初始化缩放系数为0
	picinfo.S_Height=0;	//初始化设定的高度为0
	picinfo.S_Width=0;	//初始化设定的宽度为0
	picinfo.S_XOFF=0;	//初始化x轴的偏移量为0
	picinfo.S_YOFF=0;	//初始化y轴的偏移量为0
	picinfo.staticx=0;	//初始化当前显示到的x坐标为0
	picinfo.staticy=0;	//初始化当前显示到的y坐标为0
}
//快速ALPHA BLENDING算法.
//src:源颜色
//dst:目标颜色
//alpha:透明程度(0~32)
//返回值:混合后的颜色.
u16 piclib_alpha_blend(u16 src,u16 dst,u8 alpha)
{
	u32 src2;
	u32 dst2;	 
	//Convert to 32bit |-----GGGGGG-----RRRRR------BBBBB|
	src2=((src<<16)|src)&0x07E0F81F;
	dst2=((dst<<16)|dst)&0x07E0F81F;   
	//Perform blending R:G:B with alpha in range 0..32
	//Note that the reason that alpha may not exceed 32 is that there are only
	//5bits of space between each R:G:B value, any higher value will overflow
	//into the next component and deliver ugly result.
	dst2=((((dst2-src2)*alpha)>>5)+src2)&0x07E0F81F;
	return (dst2>>16)|dst2;  
}
//初始化智能画点
//内部调用
void ai_draw_init(void)
{
	float temp,temp1;	   
	temp=(float)picinfo.S_Width/picinfo.ImgWidth;
	temp1=(float)picinfo.S_Height/picinfo.ImgHeight;						 
	if(temp<temp1)temp1=temp;//取较小的那个	 
	if(temp1>1)temp1=1;	  
	//使图片处于所给区域的中间
	picinfo.S_XOFF+=(picinfo.S_Width-temp1*picinfo.ImgWidth)/2;
	picinfo.S_YOFF+=(picinfo.S_Height-temp1*picinfo.ImgHeight)/2;
	temp1*=8192;//扩大8192倍	 
	picinfo.Div_Fac=temp1;
	picinfo.staticx=0xffff;
	picinfo.staticy=0xffff;//放到一个不可能的值上面			 										    
}   
//判断这个像素是否可以显示
//(x,y) :像素原始坐标
//chg   :功能变量. 
//返回值:0,不需要显示.1,需要显示
u8 is_element_ok(u16 x,u16 y,u8 chg)
{				  
	if(x!=picinfo.staticx||y!=picinfo.staticy)
	{
		if(chg==1)
		{
			picinfo.staticx=x;
			picinfo.staticy=y;
		} 
		return 1;
	}else return 0;
}
//智能画图
//FileName:要显示的图片文件  BMP/JPG/JPEG/GIF
//x,y,width,height:坐标及显示区域尺寸
//fast:使能jpeg/jpg小图片(图片尺寸小于等于液晶分辨率)快速解码,0,不使能;1,使能.
//图片在开始和结束的坐标点范围内显示
u8 ai_load_picfile(const u8 *filename,u16 x,u16 y,u16 width,u16 height,u8 fast)
{	
	u8	res;//返回值
	u8 temp;	
    LTDC_fill( x,  y,  x+width,  y+height, background_color);
	if((x+width)>picinfo.lcdwidth)return PIC_WINDOW_ERR;		//x坐标超范围了.
	if((y+height)>picinfo.lcdheight)return PIC_WINDOW_ERR;		//y坐标超范围了.  
	//得到显示方框大小	  	 
	if(width==0||height==0)return PIC_WINDOW_ERR;	//窗口设定错误
	picinfo.S_Height=height-1;
	picinfo.S_Width=width-1;
	//显示区域无效
	if(picinfo.S_Height==0||picinfo.S_Width==0)
	{
		picinfo.S_Height=LTDC_PHEIGHT;
		picinfo.S_Width=LTDC_PWIDTH;
		return FALSE;   
	}
	if(pic_phy.fillcolor==NULL)fast=0;//颜色填充函数未实现,不能快速显示
	//显示的开始坐标点
	picinfo.S_YOFF=y;
	picinfo.S_XOFF=x;
	//文件名传递		 
	temp=exfuns_file_type((char*)filename);	//得到文件的类型
	switch(temp)
	{											  
		case T_BMP:
			res=stdbmp_decode(filename); 				//解码bmp	  	  
			break;
		case T_JPG:
		case T_JPEG:
			res=jpg_decode(filename,fast);				//解码JPG/JPEG	  	  
			break;
		case T_GIF:
			res=gif_decode(filename,x,y,width,height);	//解码gif  	  
			break;
		default:
	 		res=PIC_FORMAT_ERR;  						//非图片格式!!!  
			break;
	}  											   
	return res;
}
//动态分配内存
void *pic_memalloc (u32 size)			
{
	return (void*)mymalloc(SRAMIN,size);
}
//释放内存
void pic_memfree (void* mf)		 
{
	myfree(SRAMIN,mf);
}


//得到path路径下,目标文件的总个数
//path:路径		    
//返回值:总有效文件数
u16 pic_get_tnum( char *path)
{	  
	u8 res;
	u16 rval=0;
 	DIR tdir;	 		//临时目录
	FILINFO *tfileinfo;	//临时文件信息	    			     
	tfileinfo=(FILINFO*)mymalloc(SRAMIN,sizeof(FILINFO));//给临时文件申请内存
    res=f_opendir(&tdir,(const TCHAR*)path); 	//打开目录,信息存到tdir
	if(res==FR_OK&&tfileinfo)  //如果打开成功,并且文件内存申请成功
	{
		while(1)//查询总的有效文件数
		{
	        res=f_readdir(&tdir,tfileinfo);       		//读取目录下的一个文件,信息存到tfileinfo  	 
	        if(res!=FR_OK||tfileinfo->fname[0]==0)break;//错误了/到末尾了,退出	 		 
			res=exfuns_file_type(tfileinfo->fname); //取文件名
			if((res&0XF0)==0X50)//取高四位,看看是不是图片文件	
			{
				rval++;//有效文件数增加1
			}	    
		}  
	}  
	myfree(SRAMIN,tfileinfo);//释放内存
	return rval;
} 





















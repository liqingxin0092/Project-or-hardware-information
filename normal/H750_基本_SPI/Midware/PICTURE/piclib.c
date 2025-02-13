#include "piclib.h"
#include "lcd.h"
#include "stdio.h"
#include "string.h"
#include "delay.h"

extern u32 *ltdc_framebuf[2];	//LTDC LCD帧缓存数组指针,必须指向对应大小的内存区域
	
_pic_info picinfo;	 	//图片信息
_pic_phy pic_phy;		//图片显示物理接口	


//画图初始化,在画图之前,必须先调用此函数
//目前没有实现读点,BMP格式可能不支持
void piclib_init(void)
{
//	pic_phy.read_point=piclib_fill_color;  		//读点函数实现,仅BMP需要
	pic_phy.draw_point=LCD_DrawPoint;	//画点函数实现
	pic_phy.fill=picture_fill_rect;					//填充函数实现,仅GIF需要
	pic_phy.draw_hline=piclib_draw_hline;  	//画线函数实现,仅GIF需要
	pic_phy.fillcolor=LCD_Fill_Buffer_Colors;  	//颜色填充函数实现,仅TJPGD需要 

	picinfo.lcdwidth=240;	//得到LCD的宽度像素
	picinfo.lcdheight=240;//得到LCD的高度像素

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
    LCD_FillRect( x,  y,  width,  height);
	if((x+width)>picinfo.lcdwidth)return PIC_WINDOW_ERR;		//x坐标超范围了.
	if((y+height)>picinfo.lcdheight)return PIC_WINDOW_ERR;		//y坐标超范围了.  
	//得到显示方框大小	  	 
	if(width==0||height==0)return PIC_WINDOW_ERR;	//窗口设定错误
	picinfo.S_Height=height-1;
	picinfo.S_Width=width-1;
	//显示区域无效
	if(picinfo.S_Height==0||picinfo.S_Width==0)
	{
		picinfo.S_Height=LCD.Height;
		picinfo.S_Width=LCD.Width;
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

/*扫描图片并且显示*/
//内部自带循环
void picture_scan_and_display(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint16_t display_time)
{
        /*相关变量定义*/
        DIR picdir;           // 图片目录
        u16 totpicnum;        // 图片文件总数
        FILINFO *picfileinfo; // 文件信息
        u8 *pname;            // 带路径的文件名
        u32 *picoffsettbl;    // 图片文件offset索引表
        u8 res;               // 执行结果
        u16 curindex = 0;     // 图片当前索引
        u16 temp;

        /*尝试打开文件夹*/
        if (f_opendir(&picdir, "0:/PICTURE") != 0) // 打开图片文件夹
        {
            printf("图片目录打开失败\r\n");
            while (1)
                ;
        }

        /*获取图片数量*/
        totpicnum = pic_get_tnum("0:/PICTURE");
        if (totpicnum == 0) ////得到总有效文件数,图片文件为0
        {
            printf("图片数目为0\r\n");
            while (1)
                ;
        }
        else
        {
            printf("图片数目为%d\r\n", totpicnum);
        }
        f_closedir(&picdir);

        /*为图片信息申请空间*/
        picfileinfo = (FILINFO *)mymalloc(SRAMIN, sizeof(FILINFO)); // 给文件信息申请内存
        pname = mymalloc(SRAMIN, 20 * 2 + 1);               // 为带路径的文件名分配内存
        picoffsettbl = mymalloc(SRAMIN, 4 * totpicnum); // 申请4*totpicnum个字节的内存,用于存放图片索引
        if (!picfileinfo || !picoffsettbl)
        {
            printf("内存分配失败,picfileinfo:%#x,picoffsettbl:%#x", (u32)picfileinfo, (u32)picoffsettbl);
            while (1)
                ;
        }

        /*将图片文件夹中的图片文件偏移记录在表中*/
        if (f_opendir(&picdir, "0:/PICTURE") != FR_OK) // 打开目录
            while (1);
        while (1) // 全部查询一遍
        {
            temp = picdir.dptr;                    // 记录当前dptr偏移
            res = f_readdir(&picdir, picfileinfo); // 读取目录下的一个文件
            if (res != FR_OK || picfileinfo->fname[0] == 0)
                break; // 错误了/到末尾了,退出
            res = exfuns_file_type(picfileinfo->fname);
            if ((res & 0XF0) == 0X50) // 取高四位,看看是不是图片文件
            {
                picoffsettbl[curindex] = temp; // 记录索引
                curindex++;
                printf("picture fats name:%s\r\n", picfileinfo->fname);
            }
        }

        /*图片显示*/
        piclib_init(); // 初始化画图
        curindex = 0;  // 从0开始显示
        while (1)      // 打开成功
        {
            dir_sdi(&picdir, picoffsettbl[curindex]); // 改变当前目录索引
            res = f_readdir(&picdir, picfileinfo);    // 读取目录下的一个文件
            if (res != FR_OK || picfileinfo->fname[0] == 0)
                break;                                               // 错误了/到末尾了,退出
            strcpy((char *)pname, "0:/PICTURE/");                    // 复制路径(目录)
            strcat((char *)pname, (const char *)picfileinfo->fname); // 将文件名接在后面
            ai_load_picfile(pname, x, y, width, height, 1);              // 显示图片
            curindex++;
            if (curindex >= totpicnum)
                curindex = 0; // 到末尾的时候,自动从头开始
            delay_ms(display_time);
        }
        myfree(SRAMIN, picfileinfo);  // 释放内存
        myfree(SRAMIN, pname);        // 释放内存
        myfree(SRAMIN, picoffsettbl); // 释放内存
}



















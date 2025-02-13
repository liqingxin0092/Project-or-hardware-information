#include "piclib.h"
#include "lcd.h"
#include "stdio.h"
#include "string.h"
#include "delay.h"

extern u32 *ltdc_framebuf[2];	//LTDC LCD֡��������ָ��,����ָ���Ӧ��С���ڴ�����
	
_pic_info picinfo;	 	//ͼƬ��Ϣ
_pic_phy pic_phy;		//ͼƬ��ʾ����ӿ�	


//��ͼ��ʼ��,�ڻ�ͼ֮ǰ,�����ȵ��ô˺���
//Ŀǰû��ʵ�ֶ���,BMP��ʽ���ܲ�֧��
void piclib_init(void)
{
//	pic_phy.read_point=piclib_fill_color;  		//���㺯��ʵ��,��BMP��Ҫ
	pic_phy.draw_point=LCD_DrawPoint;	//���㺯��ʵ��
	pic_phy.fill=picture_fill_rect;					//��亯��ʵ��,��GIF��Ҫ
	pic_phy.draw_hline=piclib_draw_hline;  	//���ߺ���ʵ��,��GIF��Ҫ
	pic_phy.fillcolor=LCD_Fill_Buffer_Colors;  	//��ɫ��亯��ʵ��,��TJPGD��Ҫ 

	picinfo.lcdwidth=240;	//�õ�LCD�Ŀ������
	picinfo.lcdheight=240;//�õ�LCD�ĸ߶�����

	picinfo.ImgWidth=0;	//��ʼ�����Ϊ0
	picinfo.ImgHeight=0;//��ʼ���߶�Ϊ0
	picinfo.Div_Fac=0;	//��ʼ������ϵ��Ϊ0
	picinfo.S_Height=0;	//��ʼ���趨�ĸ߶�Ϊ0
	picinfo.S_Width=0;	//��ʼ���趨�Ŀ��Ϊ0
	picinfo.S_XOFF=0;	//��ʼ��x���ƫ����Ϊ0
	picinfo.S_YOFF=0;	//��ʼ��y���ƫ����Ϊ0
	picinfo.staticx=0;	//��ʼ����ǰ��ʾ����x����Ϊ0
	picinfo.staticy=0;	//��ʼ����ǰ��ʾ����y����Ϊ0
}
//����ALPHA BLENDING�㷨.
//src:Դ��ɫ
//dst:Ŀ����ɫ
//alpha:͸���̶�(0~32)
//����ֵ:��Ϻ����ɫ.
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
//��ʼ�����ܻ���
//�ڲ�����
void ai_draw_init(void)
{
	float temp,temp1;	   
	temp=(float)picinfo.S_Width/picinfo.ImgWidth;
	temp1=(float)picinfo.S_Height/picinfo.ImgHeight;						 
	if(temp<temp1)temp1=temp;//ȡ��С���Ǹ�	 
	if(temp1>1)temp1=1;	  
	//ʹͼƬ��������������м�
	picinfo.S_XOFF+=(picinfo.S_Width-temp1*picinfo.ImgWidth)/2;
	picinfo.S_YOFF+=(picinfo.S_Height-temp1*picinfo.ImgHeight)/2;
	temp1*=8192;//����8192��	 
	picinfo.Div_Fac=temp1;
	picinfo.staticx=0xffff;
	picinfo.staticy=0xffff;//�ŵ�һ�������ܵ�ֵ����			 										    
}   
//�ж���������Ƿ������ʾ
//(x,y) :����ԭʼ����
//chg   :���ܱ���. 
//����ֵ:0,����Ҫ��ʾ.1,��Ҫ��ʾ
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
//���ܻ�ͼ
//FileName:Ҫ��ʾ��ͼƬ�ļ�  BMP/JPG/JPEG/GIF
//x,y,width,height:���꼰��ʾ����ߴ�
//fast:ʹ��jpeg/jpgСͼƬ(ͼƬ�ߴ�С�ڵ���Һ���ֱ���)���ٽ���,0,��ʹ��;1,ʹ��.
//ͼƬ�ڿ�ʼ�ͽ���������㷶Χ����ʾ
u8 ai_load_picfile(const u8 *filename,u16 x,u16 y,u16 width,u16 height,u8 fast)
{	
	u8	res;//����ֵ
	u8 temp;	
    LCD_FillRect( x,  y,  width,  height);
	if((x+width)>picinfo.lcdwidth)return PIC_WINDOW_ERR;		//x���곬��Χ��.
	if((y+height)>picinfo.lcdheight)return PIC_WINDOW_ERR;		//y���곬��Χ��.  
	//�õ���ʾ�����С	  	 
	if(width==0||height==0)return PIC_WINDOW_ERR;	//�����趨����
	picinfo.S_Height=height-1;
	picinfo.S_Width=width-1;
	//��ʾ������Ч
	if(picinfo.S_Height==0||picinfo.S_Width==0)
	{
		picinfo.S_Height=LCD.Height;
		picinfo.S_Width=LCD.Width;
		return FALSE;   
	}
	if(pic_phy.fillcolor==NULL)fast=0;//��ɫ��亯��δʵ��,���ܿ�����ʾ
	//��ʾ�Ŀ�ʼ�����
	picinfo.S_YOFF=y;
	picinfo.S_XOFF=x;
	//�ļ�������		 
	temp=exfuns_file_type((char*)filename);	//�õ��ļ�������
	switch(temp)
	{											  
		case T_BMP:
			res=stdbmp_decode(filename); 				//����bmp	  	  
			break;
		case T_JPG:
		case T_JPEG:
			res=jpg_decode(filename,fast);				//����JPG/JPEG	  	  
			break;
		case T_GIF:
			res=gif_decode(filename,x,y,width,height);	//����gif  	  
			break;
		default:
	 		res=PIC_FORMAT_ERR;  						//��ͼƬ��ʽ!!!  
			break;
	}  											   
	return res;
}
//��̬�����ڴ�
void *pic_memalloc (u32 size)			
{
	return (void*)mymalloc(SRAMIN,size);
}
//�ͷ��ڴ�
void pic_memfree (void* mf)		 
{
	myfree(SRAMIN,mf);
}


//�õ�path·����,Ŀ���ļ����ܸ���
//path:·��		    
//����ֵ:����Ч�ļ���
u16 pic_get_tnum( char *path)
{	  
	u8 res;
	u16 rval=0;
 	DIR tdir;	 		//��ʱĿ¼
	FILINFO *tfileinfo;	//��ʱ�ļ���Ϣ	    			     
	tfileinfo=(FILINFO*)mymalloc(SRAMIN,sizeof(FILINFO));//����ʱ�ļ������ڴ�
    res=f_opendir(&tdir,(const TCHAR*)path); 	//��Ŀ¼,��Ϣ�浽tdir
	if(res==FR_OK&&tfileinfo)  //����򿪳ɹ�,�����ļ��ڴ�����ɹ�
	{
		while(1)//��ѯ�ܵ���Ч�ļ���
		{
	        res=f_readdir(&tdir,tfileinfo);       		//��ȡĿ¼�µ�һ���ļ�,��Ϣ�浽tfileinfo  	 
	        if(res!=FR_OK||tfileinfo->fname[0]==0)break;//������/��ĩβ��,�˳�	 		 
			res=exfuns_file_type(tfileinfo->fname); //ȡ�ļ���
			if((res&0XF0)==0X50)//ȡ����λ,�����ǲ���ͼƬ�ļ�	
			{
				rval++;//��Ч�ļ�������1
			}	    
		}  
	}  
	myfree(SRAMIN,tfileinfo);//�ͷ��ڴ�
	return rval;
} 

/*ɨ��ͼƬ������ʾ*/
//�ڲ��Դ�ѭ��
void picture_scan_and_display(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint16_t display_time)
{
        /*��ر�������*/
        DIR picdir;           // ͼƬĿ¼
        u16 totpicnum;        // ͼƬ�ļ�����
        FILINFO *picfileinfo; // �ļ���Ϣ
        u8 *pname;            // ��·�����ļ���
        u32 *picoffsettbl;    // ͼƬ�ļ�offset������
        u8 res;               // ִ�н��
        u16 curindex = 0;     // ͼƬ��ǰ����
        u16 temp;

        /*���Դ��ļ���*/
        if (f_opendir(&picdir, "0:/PICTURE") != 0) // ��ͼƬ�ļ���
        {
            printf("ͼƬĿ¼��ʧ��\r\n");
            while (1)
                ;
        }

        /*��ȡͼƬ����*/
        totpicnum = pic_get_tnum("0:/PICTURE");
        if (totpicnum == 0) ////�õ�����Ч�ļ���,ͼƬ�ļ�Ϊ0
        {
            printf("ͼƬ��ĿΪ0\r\n");
            while (1)
                ;
        }
        else
        {
            printf("ͼƬ��ĿΪ%d\r\n", totpicnum);
        }
        f_closedir(&picdir);

        /*ΪͼƬ��Ϣ����ռ�*/
        picfileinfo = (FILINFO *)mymalloc(SRAMIN, sizeof(FILINFO)); // ���ļ���Ϣ�����ڴ�
        pname = mymalloc(SRAMIN, 20 * 2 + 1);               // Ϊ��·�����ļ��������ڴ�
        picoffsettbl = mymalloc(SRAMIN, 4 * totpicnum); // ����4*totpicnum���ֽڵ��ڴ�,���ڴ��ͼƬ����
        if (!picfileinfo || !picoffsettbl)
        {
            printf("�ڴ����ʧ��,picfileinfo:%#x,picoffsettbl:%#x", (u32)picfileinfo, (u32)picoffsettbl);
            while (1)
                ;
        }

        /*��ͼƬ�ļ����е�ͼƬ�ļ�ƫ�Ƽ�¼�ڱ���*/
        if (f_opendir(&picdir, "0:/PICTURE") != FR_OK) // ��Ŀ¼
            while (1);
        while (1) // ȫ����ѯһ��
        {
            temp = picdir.dptr;                    // ��¼��ǰdptrƫ��
            res = f_readdir(&picdir, picfileinfo); // ��ȡĿ¼�µ�һ���ļ�
            if (res != FR_OK || picfileinfo->fname[0] == 0)
                break; // ������/��ĩβ��,�˳�
            res = exfuns_file_type(picfileinfo->fname);
            if ((res & 0XF0) == 0X50) // ȡ����λ,�����ǲ���ͼƬ�ļ�
            {
                picoffsettbl[curindex] = temp; // ��¼����
                curindex++;
                printf("picture fats name:%s\r\n", picfileinfo->fname);
            }
        }

        /*ͼƬ��ʾ*/
        piclib_init(); // ��ʼ����ͼ
        curindex = 0;  // ��0��ʼ��ʾ
        while (1)      // �򿪳ɹ�
        {
            dir_sdi(&picdir, picoffsettbl[curindex]); // �ı䵱ǰĿ¼����
            res = f_readdir(&picdir, picfileinfo);    // ��ȡĿ¼�µ�һ���ļ�
            if (res != FR_OK || picfileinfo->fname[0] == 0)
                break;                                               // ������/��ĩβ��,�˳�
            strcpy((char *)pname, "0:/PICTURE/");                    // ����·��(Ŀ¼)
            strcat((char *)pname, (const char *)picfileinfo->fname); // ���ļ������ں���
            ai_load_picfile(pname, x, y, width, height, 1);              // ��ʾͼƬ
            curindex++;
            if (curindex >= totpicnum)
                curindex = 0; // ��ĩβ��ʱ��,�Զ���ͷ��ʼ
            delay_ms(display_time);
        }
        myfree(SRAMIN, picfileinfo);  // �ͷ��ڴ�
        myfree(SRAMIN, pname);        // �ͷ��ڴ�
        myfree(SRAMIN, picoffsettbl); // �ͷ��ڴ�
}



















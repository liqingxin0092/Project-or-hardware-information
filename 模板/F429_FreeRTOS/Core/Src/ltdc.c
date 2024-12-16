#include "ltdc.h"
#include "FONT.H"
#include "touch.h"

LTDC_HandleTypeDef hltdc; // LTDC句柄
uint16_t background_color = WHITE; // 字符显示的背景色
uint16_t RGB_GRAM[1024][600] __attribute__((at(0XC0000000))); // 外挂SDRAM里的帧缓冲区
const uint16_t POINT_COLOR_TBL[5] = {PURPLE, CYAN, BLUE, WHITE, LIGHTBLUE};
void LTDC_config_layer(LTDC_HandleTypeDef *ltdcHandle)
{
  LTDC_LayerCfgTypeDef pLayerCfg = {0};

  /*层显示区域设置*/
  pLayerCfg.WindowX0 = 0;            // 窗口起始x位置
  pLayerCfg.WindowX1 = LTDC_PWIDTH;  // 窗口结束x位置
  pLayerCfg.WindowY0 = 0;            // 窗口起始y位置
  pLayerCfg.WindowY1 = LTDC_PHEIGHT; // 窗口结束y位置

  /*层像素格式设置*/
  pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;

  /*层混合因素*/
  pLayerCfg.Alpha = 255;                                // 当前层恒定Alpha值,也就是透明度,255是完全不透明
  pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA; // 配置混合因子
  pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA; // 配置混合因子

  /*帧缓冲区设置*/
  pLayerCfg.FBStartAdress = (uint32_t)RGB_GRAM; // 帧缓冲区地址
  pLayerCfg.ImageWidth = LTDC_PWIDTH;
  pLayerCfg.ImageHeight = LTDC_PHEIGHT;

  /*窗口之外区域的配置,同属这一层,由于这层全覆盖,配置以下无意义*/
  pLayerCfg.Alpha0 = 0; // 当前层默认透明值
  pLayerCfg.Backcolor.Blue = 0;
  pLayerCfg.Backcolor.Green = 0xff;
  pLayerCfg.Backcolor.Red = 0;

  /*初始化第一层*/
  HAL_LTDC_ConfigLayer(ltdcHandle, &pLayerCfg, LTDC_LAYER_1);
}
void MX_LTDC_Init(uint16_t color)
{
  /*配置LTDC时序*/
  hltdc.Instance = LTDC;
  hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;                                // T
  hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;                                // T
  hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;                                // T
  hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;                               // T
  hltdc.Init.HorizontalSync = LTDC_HSW - 1;                                  // T
  hltdc.Init.VerticalSync = LTDC_VSW - 1;                                    // T
  hltdc.Init.AccumulatedHBP = LTDC_HSW + LTDC_HBP - 1;                       // T
  hltdc.Init.AccumulatedVBP = LTDC_VSW + LTDC_VBP - 1;                       // T
  hltdc.Init.AccumulatedActiveW = LTDC_HSW + LTDC_HBP + LTDC_PWIDTH - 1;     // T
  hltdc.Init.AccumulatedActiveH = LTDC_VSW + LTDC_VBP + LTDC_PHEIGHT - 1;    // T
  hltdc.Init.TotalWidth = LTDC_HSW + LTDC_HBP + LTDC_PWIDTH + LTDC_HFP - 1;  // T
  hltdc.Init.TotalHeigh = LTDC_VSW + LTDC_VBP + LTDC_PHEIGHT + LTDC_VFP - 1; // T

  /*背景层设置*/
  hltdc.Init.Backcolor.Blue = 0;
  hltdc.Init.Backcolor.Green = 0xff;
  hltdc.Init.Backcolor.Red = 0xff;

  /*赋值*/
  HAL_LTDC_Init(&hltdc);

  /*点亮背光*/
  GPIOB->ODR |= 1 << 5;

  /*初始化第一层*/
  LTDC_config_layer(&hltdc);

  /*开DMA2D时钟*/
  __HAL_RCC_DMA2D_CLK_ENABLE();
  
  /*清屏*/
  LTDC_CLear(color);
}

/*初始化GPIO和时钟*/
void HAL_LTDC_MspInit(LTDC_HandleTypeDef *ltdcHandle)
{
  /*这里给LTDC配置50Mhz时钟*/
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
  PeriphClkInitStruct.PLLSAI.PLLSAIN = 120;
  PeriphClkInitStruct.PLLSAI.PLLSAIR = 2;
  PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    while (1)
      ;

  /*开GPIO和LTDC时钟*/
  __HAL_RCC_LTDC_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*初始化GPIO*/
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_10;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_11;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Alternate = GPIO_AF9_LTDC;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
}
/**
 * @brief 指定位置显示字符
 *
 * @param x
 * @param y
 * @param ascii 要显示的字符
 * @param size 尺寸,12,16,24
 * @param color 颜色
 * @param mode 1:叠加 , 0:非叠加(就是带背景色)
 */
void LTDC_ShowChar(uint16_t x, uint16_t y, uint8_t ascii, uint8_t size, uint16_t color, uint8_t mode)
{
  const uint8_t *temp;
  uint8_t temp_;
  uint8_t t1, t;
  uint16_t y0 = y;

  /*获得取模软件得到的不同尺寸符号的字节个数*/
  uint8_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2); // 得到字体一个字符对应点阵集所占的字节数

  /*获得符号在字库中的偏移*/
  ascii = ascii - ' ';

  /*获取字库*/
  if (size == 12)
    temp = &asc2_1206[ascii][0]; // 调用1206字体
  else if (size == 16)
    temp = &asc2_1608[ascii][0]; // 调用1608字体
  else if (size == 24)
    temp = &asc2_2412[ascii][0]; // 调用2412字体
  else
    return; // 没有的字库

  /*打印字符*/
  for (t = 0; t < csize; t++)
  {
    /*依次获取到字节*/
    temp_ = temp[t];
    /*对每个字节的每一位进行判断*/
    for (t1 = 0; t1 < 8; t1++)
    {
      if (temp_ & 0x80) // 依次判断首位
        LTDC_Draw_Point(x, y, color);
      else if (mode == 0) // 带背景的模式
        LTDC_Draw_Point(x, y, background_color);
      temp_ <<= 1; // 判断下次
      y++;
      /*如果发现y超界*/ /*y异常*/
      if (y >= LTDC_HEIGHT)
      {
        y = y0; // 让y等于最开始设置的y并且x++
        x++;
        /*根据字符尺寸跳转到后面的字节开始打印*/
        if (size == 24)
        {
          if (t % 3 == 0)
          {
            t += 2;
          }
          else if (t % 3 == 1)
          {
            t += 1;
          }
        }
        else if (size == 16 || size == 12)
        {
          if (!(t % 2))
            t++;
        }
        /*X异常直接退出整个函数*/
        if (x >= LTDC_WIDTH)
          return;
        break;
      }
      /*如果发现写完了1列就退出扫描8个字节的循环,开始下一列*/ /*y正常*/
      if ((y - y0) == size)
      {
        y = y0;
        x++;
        /*X异常直接退出整个函数*/
        if (x >= LTDC_WIDTH)
          return;
        break;
      }
    }
  }
}
/**
 * @brief
 *
 * @param x
 * @param y
 * @param width
 * @param height
 * @param size
 * @param p
 * @param color
 * @param mode
 */
void LTDC_ShowString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color, uint8_t mode)
{
  uint8_t x0 = x;
  width += x;
  height += y;

  while ((*p <= '~') && (*p >= ' ')) // 判断是不是非法字符!
  {
    if (x >= width)
    {
      x = x0;
      y += size;
    }

    if (y >= height)
      break; // 退出

    LTDC_ShowChar(x, y, *p, size, color, mode);
    x += size / 2;
    p++;
  }
}
/**
 * @brief
 *
 * @param sx 参数范围是0~1023
 * @param sy 0~599
 * @param ex
 * @param ey
 * @param color
 */
void DMA2D_fill(u16 sx, u16 sy, u16 ex, u16 ey, u32 color)
{
  uint32_t psx, psy, pex, pey;

#if DISPLAY_DIR == 0 // 0,横屏
  psx = sx;
  pex = ex;
  psy = sy;
  pey = ey;
#elif DISPLAY_DIR == 1 // 1,竖屏
  psx = sy;
  pex = ey;
  psy = LTDC_PHEIGHT - ex - 1;
  pey = LTDC_PHEIGHT - sx - 1;
#endif

  DMA2D->CR &= ~1; // 关DMA2D

  /*设置工作模式*/
  DMA2D->CR |= 3 << 16; // 寄存器到存储器模式

  /*颜色格式*/
  DMA2D->OPFCCR = 2; // RGB565

  /*输出存储器地址*/
  DMA2D->OMAR = (uint32_t)RGB_GRAM + 2 * (LTDC_PWIDTH * psy + psx);

  /*设置窗口*/
  DMA2D->OOR = LTDC_PWIDTH - (pex - psx + 1);
  DMA2D->NLR = ((pex - psx + 1) << 16) | (pey - psy + 1); // 一行的像素数,有多少行

  /*颜色寄存器*/
  DMA2D->OCOLR = color;

  /*启动DMA2D传输*/
  DMA2D->CR |= DMA2D_CR_START;

  /*等待传输并且清除标志*/
  while (!(DMA2D->ISR & 2))
    ;
  DMA2D->IFCR |= 2; // 清标志
}
void LTDC_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color)
{
    /*外层循环打印一堆行*/
    for(uint16_t j=0;j<ey-sy+1;j++)
    {
        /*内层循环打一行*/
        for(uint16_t i=0;i<ex-sx+1;i++)
        {
            __LTDC_draw_point_across(sx+i, sy+j, color);
        }
    }
}
static uint32_t LCD_Pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;
    while (n--)
        result *= m;
    return result;
}
/**
 * @brief 显示数字
 *
 * @param x
 * @param y
 * @param num
 * @param len 数字的位数
 * @param size
 * @param mode
 * @param mode: [7]:0,不填充;1,填充0.
 *              [6:1]:保留
 *              [0]:0,非叠加显示;1,叠加显示.
 */
void LTDC_ShowxNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint8_t color)
{
    uint8_t t, temp;
    uint8_t enshow = 0;
    for (t = 0; t < len; t++)
    {
        temp = (num / LCD_Pow(10, len - t - 1)) % 10;
        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                if (mode & 0X80)
                    LTDC_ShowChar(x + (size / 2) * t,  y, '0',size, color, mode & 0X01);
                
                else
                    LTDC_ShowChar(x + (size / 2) * t,  y, ' ',size, color, mode & 0X01);

                continue;
            }
            else
                enshow = 1;
        }
        LTDC_ShowChar(x + (size / 2) * t,  y, temp+'0',size, color, mode & 0X01);
    }
}
/**
 * @brief 可以用来放图片
 *
 * @param sx:注意这些范围坐标要自己算,卡着那个图片的大小
 * @param sy
 * @param ex
 * @param ey
 * @param color
 */
void DMA2D_fill_color(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height, const unsigned short *source_addr)
{
  uint32_t psx, psy, pex, pey;

#if DISPLAY_DIR == 0 // 0,横屏
  psx = sx;
  pex = sx+width-1;
  psy = sy;
  pey = sy+height-1;
#elif DISPLAY_DIR == 1 // 1,竖屏
  psx = sy;
  pex = sy+height-1;
  psy = LTDC_PHEIGHT - sx-width;
  pey = LTDC_PHEIGHT - sx - 1;
#endif

  /*设置工作模式*/
  DMA2D->CR = 0; // 设置M2M

  /*颜色格式*/
  DMA2D->FGPFCCR = 2; // RGB565

  /*输入,输出存储器地址*/
  DMA2D->FGMAR = (uint32_t)source_addr;
  DMA2D->OMAR = (uint32_t)RGB_GRAM + 2 * (LTDC_PWIDTH * psy + psx);

  /*设置窗口*/
  DMA2D->FGOR = 0; // 图片的数组整个都是关于这张图片的,不需要跳过像素
  DMA2D->OOR = LTDC_PWIDTH - (pex - psx + 1);
  DMA2D->NLR = ((pex - psx + 1) << 16) | (pey - psy + 1); // 一行的像素数,有多少行

  /*启动DMA2D传输*/
  DMA2D->CR = 1;

  /*等待传输并且清除标志*/
  while (!(DMA2D->ISR & 2))
    ;
  DMA2D->IFCR |= 2; // 清标志
}
void LTDC_CLear(uint16_t color)
{
#if DISPLAY_DIR == 0 // 0,横屏
  DMA2D_fill(0, 0, 1023, 599, color);
#elif DISPLAY_DIR == 1 // 0,横屏
  DMA2D_fill(0, 0, 599, 1023, color);
#endif
    
  /*更新打印字符时的背景色*/
  background_color=color;
}
//flag:专门用来改的
void LTDC_turn_area_color(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color1 ,uint16_t color2,uint8_t* flag)
{
    if ((*flag)++==0)
        DMA2D_fill( sx,  sy,  ex,  ey, color1);
    else
    {
        DMA2D_fill( sx,  sy,  ex,  ey, color2);
        (*flag)=0;
    }
}

void lcd_draw_hline(uint16_t x, uint16_t y, uint16_t len, uint16_t color)
{
    DMA2D_fill(x, y, x + len - 1, y, color);
}

void lcd_fill_circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color)
{
    uint32_t i;
    uint32_t imax = ((uint32_t)r * 707) / 1000 + 1;
    uint32_t sqmax = (uint32_t)r * (uint32_t)r + (uint32_t)r / 2;
    uint32_t xr = r;

    lcd_draw_hline(x - r, y, 2 * r, color);

    for (i = 1; i <= imax; i++)
    {
        if ((i * i + xr * xr) > sqmax)
        {
            /* draw lines from outside */
            if (xr > imax)
            {
                lcd_draw_hline (x - i + 1, y + xr, 2 * (i - 1), color);
                lcd_draw_hline (x - i + 1, y - xr, 2 * (i - 1), color);
            }

            xr--;
        }

        /* draw lines from inside (center) */
        lcd_draw_hline(x - xr, y + i, 2 * xr, color);
        lcd_draw_hline(x - xr, y - i, 2 * xr, color);
    }
}

void lcd_draw_bline(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t size, uint16_t color)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, row, col;

    if (x1 < size || x2 < size || y1 < size || y2 < size)
        return;

    delta_x = x2 - x1;                        /* 计算坐标增量 */
    delta_y = y2 - y1;
    row = x1;
    col = y1;
    if (delta_x > 0)
    {
        incx = 1;                             /* 设置单步方向 */
    }
    else if (delta_x == 0)
    {
        incx = 0;                             /* 垂直线 */
    }
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0)
    {
        incy = 1;
    }
    else if (delta_y == 0)
    {
        incy = 0;                               /* 水平线 */
    }
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }
    if (delta_x > delta_y)
        distance = delta_x;                     /* 选取基本增量坐标轴 */
    else
        distance = delta_y;
    for (t = 0; t <= distance + 1; t++)         /* 画线输出 */
    {
        lcd_fill_circle(row, col, size, color); /* 画点 */
        xerr += delta_x;
        yerr += delta_y;
        if (xerr > distance)
        {
            xerr -= distance;
            row += incx;
        }
        if (yerr > distance)
        {
            yerr -= distance;
            col += incy;
        }
    }
}

extern uint8_t touch_num;
extern uint8_t touch_flag;
void TOUCH_Line( uint8_t size)
{
   uint8_t t = 0;
   static uint16_t lastpos[10][2]; 
     for (t = 0; t < 5; t++)//轮询5个点
        {
            if(touch_flag & (1 << t))
            {
                if (lastpos[t][0] == 0xFFFF)
                {
                    lastpos[t][0] = tp_dev.x[t];
                    lastpos[t][1] = tp_dev.y[t];
                }
                lcd_draw_bline(lastpos[t][0], lastpos[t][1], tp_dev.x[t], tp_dev.y[t], size, POINT_COLOR_TBL[t]); /* 画线 */
                lastpos[t][0] = tp_dev.x[t];
                lastpos[t][1] = tp_dev.y[t];
            }
            else 
            {
                lastpos[t][0] = 0xFFFF;
            }
        }
}

u32 LTDC_read_point(uint16_t x,uint16_t y)
{
#if DISPLAY_DIR==0 //横屏
    return *(uint16_t *)((uint32_t)RGB_GRAM + 2 * (LTDC_PWIDTH * y + x));
#elif DISPLAY_DIR==1 //竖屏
    return *(uint16_t *)((uint32_t)RGB_GRAM + 2 * (LTDC_PWIDTH * (LTDC_PHEIGHT - x - 1) + y)); 
#endif
}

void LTDC_DrawPoint(u16 x, u16 y, u32 color)
{
#if DISPLAY_DIR==0 //横屏
    *(uint16_t *)((uint32_t)RGB_GRAM + 2 * (LTDC_PWIDTH * y + x))=color;
#elif DISPLAY_DIR==1 //竖屏
    *(uint16_t *)((uint32_t)RGB_GRAM + 2 * (LTDC_PWIDTH * (LTDC_PHEIGHT - x - 1) + y))=color; 
#endif
}


#include "usmart.h"
#include "usmart_str.h"

/******************************************************************************************/
/* 用户配置区
 * 这下面要包含所用到的函数所申明的头文件(用户自己添加)
 */

#define MY_FATFS_TEST 1
#include "delay.h"
#include "fattester.h"
/*这里声明要用到的函数*/
extern void write_addr(uint32_t addr, uint32_t val); //改某个地方的数
extern uint32_t read_addr(uint32_t addr);            //读某个地方的数
extern void LTDC_CLear(uint16_t color);              //LTDC清屏并且改背景色
extern void LTDC_ShowString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, uint8_t *p, uint16_t color, uint8_t mode); // 显示字符串
extern void text_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, char *str, uint8_t size, uint8_t mode, uint16_t color);
struct _m_usmart_nametab usmart_nametab[] =
{
#if USMART_USE_WRFUNS == 1      /* 如果使能了读写操作 */
    (void *)read_addr, "uint32_t read_addr(uint32_t addr)",
    (void *)write_addr, "void write_addr(uint32_t addr, uint32_t val)",
#endif

#if MY_FATFS_TEST == 1      /* 如果使能了读写操作 */
    (void *)mf_mount, "uint8_t mf_mount(uint8_t* path,uint8_t mt)",
    (void *)mf_open, "uint8_t mf_open(uint8_t*path,uint8_t mode)",
    (void *)mf_close, "uint8_t mf_close(void)",
    (void *)mf_read, "uint8_t mf_read(uint16_t len)",
    (void *)mf_write, "uint8_t mf_write(uint8_t*dat,uint16_t len)",
    (void *)mf_opendir, "uint8_t mf_opendir(uint8_t* path)",
    (void *)mf_closedir, "uint8_t mf_closedir(void)",
    (void *)mf_readdir, "uint8_t mf_readdir(void)",
    (void *)mf_scan_files, "uint8_t mf_scan_files(uint8_t * path)",
    (void *)mf_showfree, "uint32_t mf_showfree(uint8_t *path)",
    (void *)mf_lseek, "uint8_t mf_lseek(uint32_t offset)",
    (void *)mf_tell, "uint32_t mf_tell(void)",
    (void *)mf_size, "uint32_t mf_size(void)",
    (void *)mf_mkdir, "uint8_t mf_mkdir(uint8_t*path)",
    (void *)mf_fmkfs, "uint8_t mf_fmkfs(uint8_t* path,uint8_t opt, uint16_t au)",
    (void *)mf_unlink, "uint8_t mf_unlink(uint8_t *path)",
    (void *)mf_rename, "uint8_t mf_rename(uint8_t *oldname, uint8_t* newname)",
    (void *)mf_getlabel, "void mf_getlabel(uint8_t *path)",
    (void *)mf_setlabel, "void mf_setlabel(uint8_t *path)",
    (void *)mf_gets, "void mf_gets(uint16_t size)",
    (void *)mf_putc, "uint8_t mf_putc(uint8_t c)",
    (void *)mf_puts, "uint8_t mf_puts(uint8_t *str)",
#endif
        
    (void *)delay_ms, "void delay_ms(uint16_t nms)",
    (void *)delay_us, "void delay_us(uint32_t nus)",
    (void *)LTDC_CLear,"void LTDC_CLear(uint16_t color)",
    (void *)text_show_string,"void text_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, char *str, uint8_t size, uint8_t mode, uint16_t color)",
    (void *)LTDC_ShowString,"void LTDC_ShowString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, uint8_t *p, uint16_t color, uint8_t mode)",
};

/******************************************************************************************/

/* 函数控制管理器初始化
 * 得到各个受控函数的名字
 * 得到函数总数量
 */
struct _m_usmart_dev usmart_dev =
{
    usmart_nametab,
    usmart_init,
    usmart_cmd_rec,
    usmart_exe,
    usmart_scan,
    sizeof(usmart_nametab) / sizeof(struct _m_usmart_nametab), /* 函数数量 */
    0,      /* 参数数量 */
    0,      /* 函数ID */
    1,      /* 参数显示类型,0,10进制;1,16进制 */
    0,      /* 参数类型.bitx:,0,数字;1,字符串 */
    0,      /* 每个参数的长度暂存表,需要MAX_PARM个0初始化 */
    0,      /* 函数的参数,需要PARM_LEN个0初始化 */
};




















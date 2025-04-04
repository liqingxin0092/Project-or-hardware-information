#include "fonts.h"
#include "ff.h"
#include "w25qxx.h"


/*------------------------------------------------------------------------*/
/* OEM <==> Unicode conversions for static code page configuration        */
/* DBCS fixed code page                                                   */
/*------------------------------------------------------------------------*/
WCHAR ff_uni2oem (  /* Returns OEM code character, zero on error */
    DWORD   uni,    /* UTF-16 encoded character to be converted */
    WORD    cp      /* Code page for the conversion */
)
{
    WCHAR t[2];
    WCHAR c;
    uint32_t i, li, hi;
    uint16_t n;
    uint32_t gbk2uni_offset = 0;

    if (uni < 0x80)
    {
        c = uni;    /* ASCII,直接不用转换 */
    }
    else
    {
        hi = ftinfo.ugbksize / 2; /* 对半开 */
        hi = hi / 4 - 1;
        li = 0;

        for (n = 16; n; n--)    /* 二分法查找 */
        {
            i = li + (hi - li) / 2;
            W25QXX_Read((uint8_t *)&t, ftinfo.ugbkaddr + i * 4 + gbk2uni_offset, 4); /* 读出4个字节 */
            if (uni == t[0]) break;

            if (uni > t[0])
            {
                li = i;
            }
            else
            {
                hi = i;
            }
        }

        c = n ? t[1] : 0;
    }

    return c;
}


WCHAR ff_oem2uni (  /* Returns Unicode character, zero on error */
    WCHAR   oem,    /* OEM code to be converted */
    WORD    cp      /* Code page for the conversion */
)
{
    WCHAR t[2];
    WCHAR c;
    uint32_t i, li, hi;
    uint16_t n;
    uint32_t gbk2uni_offset = ftinfo.ugbksize / 2;

    if (oem < 0x80)
    {
        c = oem;    /* ASCII,直接不用转换 */
    }
    else
    {
        hi = ftinfo.ugbksize / 2; /* 对半开 */
        hi = hi / 4 - 1;
        li = 0;

        for (n = 16; n; n--)    /* 二分法查找 */
        {
            i = li + (hi - li) / 2;
            W25QXX_Read((uint8_t *)&t, ftinfo.ugbkaddr + i * 4 + gbk2uni_offset, 4);  /* 读出4个字节 */

            if (oem == t[0]) break;

            if (oem > t[0])
            {
                li = i;
            }
            else
            {
                hi = i;
            }
        }

        c = n ? t[1] : 0;
    }

    return c;
}


/*------------------------------------------------------------------------*/
/* Unicode up-case conversion                                             */
/*------------------------------------------------------------------------*/

DWORD ff_wtoupper (	/* Returns up-converted code point */
    DWORD uni		/* Unicode code point to be up-converted */
)
{
    const WORD *p;
    WORD uc, bc, nc, cmd;
    static const WORD cvt1[] =  	/* Compressed up conversion table for U+0000 - U+0FFF */
    {
        /* Basic Latin */
        0x0061, 0x031A,
        /* Latin-1 Supplement */
        0x00E0, 0x0317,
        0x00F8, 0x0307,
        0x00FF, 0x0001, 0x0178,
        /* Latin Extended-A */
        0x0100, 0x0130,
        0x0132, 0x0106,
        0x0139, 0x0110,
        0x014A, 0x012E,
        0x0179, 0x0106,
        /* Latin Extended-B */
        0x0180, 0x004D, 0x0243, 0x0181, 0x0182, 0x0182, 0x0184, 0x0184, 0x0186, 0x0187, 0x0187, 0x0189, 0x018A, 0x018B, 0x018B, 0x018D, 0x018E, 0x018F, 0x0190, 0x0191, 0x0191, 0x0193, 0x0194, 0x01F6, 0x0196, 0x0197, 0x0198, 0x0198, 0x023D, 0x019B, 0x019C, 0x019D, 0x0220, 0x019F, 0x01A0, 0x01A0, 0x01A2, 0x01A2, 0x01A4, 0x01A4, 0x01A6, 0x01A7, 0x01A7, 0x01A9, 0x01AA, 0x01AB, 0x01AC, 0x01AC, 0x01AE, 0x01AF, 0x01AF, 0x01B1, 0x01B2, 0x01B3, 0x01B3, 0x01B5, 0x01B5, 0x01B7, 0x01B8, 0x01B8, 0x01BA, 0x01BB, 0x01BC, 0x01BC, 0x01BE, 0x01F7, 0x01C0, 0x01C1, 0x01C2, 0x01C3, 0x01C4, 0x01C5, 0x01C4, 0x01C7, 0x01C8, 0x01C7, 0x01CA, 0x01CB, 0x01CA,
        0x01CD, 0x0110,
        0x01DD, 0x0001, 0x018E,
        0x01DE, 0x0112,
        0x01F3, 0x0003, 0x01F1, 0x01F4, 0x01F4,
        0x01F8, 0x0128,
        0x0222, 0x0112,
        0x023A, 0x0009, 0x2C65, 0x023B, 0x023B, 0x023D, 0x2C66, 0x023F, 0x0240, 0x0241, 0x0241,
        0x0246, 0x010A,
        /* IPA Extensions */
        0x0253, 0x0040, 0x0181, 0x0186, 0x0255, 0x0189, 0x018A, 0x0258, 0x018F, 0x025A, 0x0190, 0x025C, 0x025D, 0x025E, 0x025F, 0x0193, 0x0261, 0x0262, 0x0194, 0x0264, 0x0265, 0x0266, 0x0267, 0x0197, 0x0196, 0x026A, 0x2C62, 0x026C, 0x026D, 0x026E, 0x019C, 0x0270, 0x0271, 0x019D, 0x0273, 0x0274, 0x019F, 0x0276, 0x0277, 0x0278, 0x0279, 0x027A, 0x027B, 0x027C, 0x2C64, 0x027E, 0x027F, 0x01A6, 0x0281, 0x0282, 0x01A9, 0x0284, 0x0285, 0x0286, 0x0287, 0x01AE, 0x0244, 0x01B1, 0x01B2, 0x0245, 0x028D, 0x028E, 0x028F, 0x0290, 0x0291, 0x01B7,
        /* Greek, Coptic */
        0x037B, 0x0003, 0x03FD, 0x03FE, 0x03FF,
        0x03AC, 0x0004, 0x0386, 0x0388, 0x0389, 0x038A,
        0x03B1, 0x0311,
        0x03C2, 0x0002, 0x03A3, 0x03A3,
        0x03C4, 0x0308,
        0x03CC, 0x0003, 0x038C, 0x038E, 0x038F,
        0x03D8, 0x0118,
        0x03F2, 0x000A, 0x03F9, 0x03F3, 0x03F4, 0x03F5, 0x03F6, 0x03F7, 0x03F7, 0x03F9, 0x03FA, 0x03FA,
        /* Cyrillic */
        0x0430, 0x0320,
        0x0450, 0x0710,
        0x0460, 0x0122,
        0x048A, 0x0136,
        0x04C1, 0x010E,
        0x04CF, 0x0001, 0x04C0,
        0x04D0, 0x0144,
        /* Armenian */
        0x0561, 0x0426,

        0x0000	/* EOT */
    };
    static const WORD cvt2[] =  	/* Compressed up conversion table for U+1000 - U+FFFF */
    {
        /* Phonetic Extensions */
        0x1D7D, 0x0001, 0x2C63,
        /* Latin Extended Additional */
        0x1E00, 0x0196,
        0x1EA0, 0x015A,
        /* Greek Extended */
        0x1F00, 0x0608,
        0x1F10, 0x0606,
        0x1F20, 0x0608,
        0x1F30, 0x0608,
        0x1F40, 0x0606,
        0x1F51, 0x0007, 0x1F59, 0x1F52, 0x1F5B, 0x1F54, 0x1F5D, 0x1F56, 0x1F5F,
        0x1F60, 0x0608,
        0x1F70, 0x000E, 0x1FBA, 0x1FBB, 0x1FC8, 0x1FC9, 0x1FCA, 0x1FCB, 0x1FDA, 0x1FDB, 0x1FF8, 0x1FF9, 0x1FEA, 0x1FEB, 0x1FFA, 0x1FFB,
        0x1F80, 0x0608,
        0x1F90, 0x0608,
        0x1FA0, 0x0608,
        0x1FB0, 0x0004, 0x1FB8, 0x1FB9, 0x1FB2, 0x1FBC,
        0x1FCC, 0x0001, 0x1FC3,
        0x1FD0, 0x0602,
        0x1FE0, 0x0602,
        0x1FE5, 0x0001, 0x1FEC,
        0x1FF3, 0x0001, 0x1FFC,
        /* Letterlike Symbols */
        0x214E, 0x0001, 0x2132,
        /* Number forms */
        0x2170, 0x0210,
        0x2184, 0x0001, 0x2183,
        /* Enclosed Alphanumerics */
        0x24D0, 0x051A,
        0x2C30, 0x042F,
        /* Latin Extended-C */
        0x2C60, 0x0102,
        0x2C67, 0x0106, 0x2C75, 0x0102,
        /* Coptic */
        0x2C80, 0x0164,
        /* Georgian Supplement */
        0x2D00, 0x0826,
        /* Full-width */
        0xFF41, 0x031A,

        0x0000	/* EOT */
    };


    if (uni < 0x10000)  	/* Is it in BMP? */
    {
        uc = (WORD)uni;
        p = uc < 0x1000 ? cvt1 : cvt2;

        for (;;)
        {
            bc = *p++;								/* Get the block base */

            if (bc == 0 || uc < bc) break;			/* Not matched? */

            nc = *p++;
            cmd = nc >> 8;
            nc &= 0xFF;	/* Get processing command and block size */

            if (uc < bc + nc)  	/* In the block? */
            {
                switch (cmd)
                {
                    case 0:
                        uc = p[uc - bc];
                        break;		/* Table conversion */

                    case 1:
                        uc -= (uc - bc) & 1;
                        break;	/* Case pairs */

                    case 2:
                        uc -= 16;
                        break;			/* Shift -16 */

                    case 3:
                        uc -= 32;
                        break;			/* Shift -32 */

                    case 4:
                        uc -= 48;
                        break;			/* Shift -48 */

                    case 5:
                        uc -= 26;
                        break;			/* Shift -26 */

                    case 6:
                        uc += 8;
                        break;				/* Shift +8 */

                    case 7:
                        uc -= 80;
                        break;			/* Shift -80 */

                    case 8:
                        uc -= 0x1C60;
                        break;		/* Shift -0x1C60 */
                }

                break;
            }

            if (cmd == 0) p += nc;	/* Skip table if needed */
        }

        uni = uc;
    }

    return uni;
}




















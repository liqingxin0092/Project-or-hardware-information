#include "IM948.h"
F32 tmpX, tmpY, tmpZ, tmpAbs;
/******************************************************************************
                        �豸��IM948ģ��֮��Ĵ���ͨ�ſ�
�汾: V1.03
��¼: 1������ ���ټƺ����������̿�����
      2������ �ų�У׼��ʼ����
      3������ �����������Զ�У����ʶ����
      4������ ���þ�ֹ����ģʽ�Ĵ���ʱ��
*******************************************************************************/


U8 targetDeviceAddress=255; // ͨ�ŵ�ַ����Ϊ0-254ָ�����豸��ַ����Ϊ255��ָ���豸(���㲥), ����Ҫʹ��485������ʽͨ��ʱͨ���ò���ѡ��Ҫ�������豸���������Ǵ���1��1ͨ����Ϊ�㲥��ַ255����

U8 CalcSum1(U8 *Buf, int Len)
{
    U8 Sum = 0;
    while (Len-- > 0)
    {
        Sum += Buf[Len];
    }
    return Sum;
}
void *Memcpy(void *s1, const void *s2, unsigned int n)
{
    char *p1 = s1;
    const char *p2 = s2;
    if (n)
    {
        n++;
        while (--n > 0)
        {
            *p1++ = *p2++;
        }
    }
    return s1;
}

static void Cmd_Write(U8 *pBuf, int Len);
static void Cmd_RxUnpack(U8 *buf, U8 DLen);
/**
 * ����CMD����
 *
 * @param pDat Ҫ���͵�������
 * @param DLen ������ĳ���
 *
 * @return int 0=�ɹ�, -1=ʧ��
 */
int Cmd_PackAndTx(U8 *pDat, U8 DLen)
{
    U8 buf[50 + 5 + CmdPacketMaxDatSizeTx] =
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0xff}; // ���Ͱ����� ��ͷ50�ֽ���ǰ���룬���ڻ��ѿ��ܴ���˯��״̬��ģ��

    if((DLen == 0) || (DLen > CmdPacketMaxDatSizeTx) || (pDat==NULL))
    {// �Ƿ�����
        return -1;
    }

    buf[50] = CmdPacket_Begin; // ��ʼ��
    buf[51] = targetDeviceAddress; // Ŀǰ�豸��ַ��
    buf[52] = DLen;  // ����
    Memcpy(&buf[53], pDat, DLen); // ������
    buf[53+DLen] = CalcSum1(&buf[51], DLen+2);// CS �� ��ַ�뿪ʼ�㵽���������
    buf[54+DLen] = CmdPacket_End; // ������

    Cmd_Write(buf, DLen+55);
    return 0;
}
/**
 * ���ڲ������ݰ�, �û�ֻ��ѽ��յ���ÿ�ֽ����ݴ���ú�������
 * @param byte ������յ���ÿ�ֽ�����
 * @return U8 1=���յ��������ݰ�, 0δ��ȡ���������ݰ�
 */
U8 Cmd_GetPkt(U8 byte)
{
    static U8 CS=0; // У���
    static U8 i=0;
    static U8 RxIndex=0;

    static U8 buf[5+CmdPacketMaxDatSizeRx]; // ���հ�����
    #define cmdBegin    buf[0]  // ��ʼ��
    #define cmdAddress  buf[1]  // ͨ�ŵ�ַ
    #define cmdLen      buf[2]  // ����
    #define cmdDat     &buf[3]  // ������
    #define cmdCS       buf[3+cmdLen] // У���
    #define cmdEnd      buf[4+cmdLen] // ������

    CS += byte; // �������ݱ߼���У���룬У����Ϊ��ַ�뿪ʼ(������ַ��)��У����֮ǰ�����ݵĺ�
    switch (RxIndex)
    {
    case 0: // ��ʼ��
        if (byte == CmdPacket_Begin)
        {
            i = 0;
            buf[i++] = CmdPacket_Begin;
            CS = 0; // �¸��ֽڿ�ʼ����У����
            RxIndex = 1;
        }
        break;
    case 1: // ������ĵ�ַ��
        buf[i++] = byte;
        if (byte == 255)
        { // 255�ǹ㲥��ַ��ģ����Ϊ�ӻ������ĵ�ַ���ɻ����255
            RxIndex = 0;
            break;
        }
        RxIndex++;
        break;
    case 2: // ������ĳ���
        buf[i++] = byte;
        if ((byte > CmdPacketMaxDatSizeRx) || (byte == 0))
        { // ������Ч
            RxIndex = 0;
            break;
        }
        RxIndex++;
        break;
    case 3: // ��ȡ�����������
        buf[i++] = byte;
        if (i >= cmdLen+3)
        { // ������������
            RxIndex++;
        }
        break;
    case 4: // �Ա� Ч����
        CS -= byte;
        if (CS == byte)
        {// У����ȷ
            buf[i++] = byte;
            RxIndex++;
        }
        else
        {// У��ʧ��
            RxIndex = 0;
        }
        break;
    case 5: // ������
        RxIndex = 0;
        if (byte == CmdPacket_End)
        {// ����������
            buf[i++] = byte;

            if ((targetDeviceAddress == cmdAddress) || (targetDeviceAddress == 255))
            {// ��ַƥ�䣬��Ŀ���豸���������� �Ŵ���
                Dbp_U8_buf("rx: ", "\r\n",
                           "%02X ",
                           buf, i);
                Cmd_RxUnpack(&buf[3], i-5); // �������ݰ���������
                return 1;
            }
        }
        break;
    default:
        RxIndex = 0;
        break;
    }

    return 0;
}


// ================================ģ��Ĳ���ָ��=================================

// ˯�ߴ�����
void Cmd_02(void)
{
    U8 buf[1] = {0x02};

    Cmd_PackAndTx(buf, 1);
}
// ���Ѵ�����
void Cmd_03(void)
{
    U8 buf[1] = {0x03};
    Dbp("\r\nsensor on--\r\n");
    Cmd_PackAndTx(buf, 1);
}
// �ر����������ϱ�
void Cmd_18(void)
{
    U8 buf[1] = {0x18};
    Dbp("\r\nauto report off--\r\n");
    Cmd_PackAndTx(buf, 1);
}
// �������������ϱ�
void Cmd_19(void)
{
    U8 buf[1] = {0x19};
    Dbp("\r\nauto report on--\r\n");
    Cmd_PackAndTx(buf, 1);
}
// ��ȡ1�ζ��ĵĹ�������
void Cmd_11(void)
{
    U8 buf[1] = {0x11};
    Dbp("\r\nget report--\r\n");
    Cmd_PackAndTx(buf, 1);
}
// ��ȡ�豸���Ժ�״̬
void Cmd_10(void)
{
    U8 buf[1] = {0x10};
    Dbp("\r\nget report--\r\n");
    Cmd_PackAndTx(buf, 1);
}
/**
 * �����豸����
 * @param accStill    �ߵ�-��ֹ״̬���ٶȷ�ֵ ��λdm/s?
 * @param stillToZero �ߵ�-��ֹ�����ٶ�(��λcm/s) 0:������ 255:��������
 * @param moveToZero  �ߵ�-��̬�����ٶ�(��λcm/s) 0:������
 * @param isCompassOn 1=�迪���ų� 0=��رմų�
 * @param barometerFilter ��ѹ�Ƶ��˲��ȼ�[ȡֵ0-3],��ֵԽ��Խƽ�ȵ�ʵʱ��Խ��
 * @param reportHz ���������ϱ��Ĵ���֡��[ȡֵ0-250HZ], 0��ʾ0.5HZ
 * @param gyroFilter    �������˲�ϵ��[ȡֵ0-2],��ֵԽ��Խƽ�ȵ�ʵʱ��Խ��
 * @param accFilter     ���ټ��˲�ϵ��[ȡֵ0-4],��ֵԽ��Խƽ�ȵ�ʵʱ��Խ��
 * @param compassFilter �������˲�ϵ��[ȡֵ0-9],��ֵԽ��Խƽ�ȵ�ʵʱ��Խ��
 * @param Cmd_ReportTag ���ܶ��ı�ʶ
 */
void Cmd_12(U8 accStill, U8 stillToZero, U8 moveToZero,  U8 isCompassOn, U8 barometerFilter, U8 reportHz, U8 gyroFilter, U8 accFilter, U8 compassFilter, U16 Cmd_ReportTag)
{
    U8 buf[11] = {0x12};
    buf[1] = accStill;
    buf[2] = stillToZero;
    buf[3] = moveToZero;
    buf[4] = ((barometerFilter&3)<<1) | (isCompassOn&1); // bit[2-1]: BMP280���˲��ȼ�[ȡֵ0-3]   bit[0]: 1=�ѿ����ų� 0=�ѹرմų�
    buf[5] = reportHz;
    buf[6] = gyroFilter;
    buf[7] = accFilter;
    buf[8] = compassFilter;
    buf[9] = Cmd_ReportTag&0xff;
    buf[10] = (Cmd_ReportTag>>8)&0xff;
    Dbp("\r\nset parameters--\r\n");
    Cmd_PackAndTx(buf, 11);
}
// �ߵ���ά�ռ�λ������
void Cmd_13(void)
{
    U8 buf[1] = {0x13};
    Dbp("\r\nclear INS position--\r\n");
    Cmd_PackAndTx(buf, 1);
}
// �Ʋ�������
void Cmd_16(void)
{
    U8 buf[1] = {0x16};
    Dbp("\r\nclear steps--\r\n");
    Cmd_PackAndTx(buf, 1);
}
// �ָ�����У׼����
void Cmd_14(void)
{
    U8 buf[1] = {0x14};
    Dbp("\r\nRestore calibration parameters from factory mode--\r\n");
    Cmd_PackAndTx(buf, 1);
}
// ���浱ǰУ׼����Ϊ����У׼����
void Cmd_15(void)
{
    U8 buf[3] = {0x15, 0x88, 0x99};
    Dbp("\r\nSave calibration parameters to factory mode--\r\n");
    Cmd_PackAndTx(buf, 3);
}
// ���ټƼ���У׼ ģ�龲ֹ��ˮƽ��ʱ�����͸�ָ��յ��ظ���ȴ�9�뼴��
void Cmd_07(void)
{
    U8 buf[1] = {0x07};
    Dbp("\r\nacceleration calibration--\r\n");
    Cmd_PackAndTx(buf, 1);
}
/**
 * ���ټƸ߾���У׼
 * @param flag ��ģ��δ����У׼״̬ʱ��
 *                 ֵ0 ��ʾ����ʼһ��У׼���ɼ�1������
 *                 ֵ255 ��ʾѯ���豸�Ƿ�����У׼
 *             ��ģ������У׼��:
 *                 ֵ1 ��ʾҪ�ɼ���1������
 *                 ֵ255 ��ʾҪ�ɼ����1�����ݲ�����
 */
void Cmd_17(U8 flag)
{
    U8 buf[2] = {0x17};
    buf[1] = flag;
    if (flag == 0)
    {// ����ʼ1��У׼
        Dbp("\r\ncalibration request start--\r\n");
    }
    else if (flag == 1)
    {// ����ɼ��¸�����
        Dbp("calibration request next point--\r\n");
    }
    else if (flag == 255)
    {// ����ɼ����һ�����ݲ�������ǰ�����е�У׼
        Dbp("calibration request stop--\r\n");
    }
    else
    {
        return;
    }
    Cmd_PackAndTx(buf, 2);
}
// ��ʼ������У׼
void Cmd_32(void)
{
    U8 buf[1] = {0x32};
    Dbp("\r\ncompass calibrate begin--\r\n");
    Cmd_PackAndTx(buf, 1);
}
// ����������У׼
void Cmd_04(void)
{
    U8 buf[1] = {0x04};
    Dbp("\r\ncompass calibrate end--\r\n");
    Cmd_PackAndTx(buf, 1);
}
// z��ǹ���
void Cmd_05(void)
{
    U8 buf[1] = {0x05};
    Dbp("\r\nz-axes to zero--\r\n");
    Cmd_PackAndTx(buf, 1);
}
// xyz��������ϵ����
void Cmd_06(void)
{
    U8 buf[1] = {0x06};
    Dbp("\r\nWorldXYZ-axes to zero--\r\n");
    Cmd_PackAndTx(buf, 1);
}
// �ָ�Ĭ�ϵ���������ϵZ��ָ�򼰻ָ�Ĭ�ϵ���������ϵ
void Cmd_08(void)
{
    U8 buf[1] = {0x08};
    Dbp("\r\naxesZ WorldXYZ-axes to zero--\r\n");
    Cmd_PackAndTx(buf, 1);
}
/**
 * ����PCB��װ�������
 * @param accMatrix ���ټƷ������
 * @param comMatrix �����Ʒ������
 */
void Cmd_20(S8 *accMatrix, S8 *comMatrix)
{
    U8 buf[19] = {0x20};
    Memcpy(&buf[1],  accMatrix, 9);
    Memcpy(&buf[10], comMatrix, 9);
    Dbp("\r\nz-axes to zero--\r\n");
    Cmd_PackAndTx(buf, 19);
}
// ��ȡPCB��װ�������
void Cmd_21(void)
{
    U8 buf[1] = {0x21};
    Dbp("\r\nget PCB direction--\r\n");
    Cmd_PackAndTx(buf, 1);
}
/**
 * ���������㲥����
 * @param bleName ��������(���֧��15���ַ�����,��֧������)
 */
void Cmd_22(U8 *bleName)
{
    U8 buf[17] = {0x22};
    Memcpy(&buf[1],  bleName, 16);
    Dbp("\r\nset BLE name--\r\n");
    Cmd_PackAndTx(buf, 17);
}
// ��ȡ�����㲥����
void Cmd_23(void)
{
    U8 buf[1] = {0x23};
    Dbp("\r\nget BLE name--\r\n");
    Cmd_PackAndTx(buf, 1);
}
/**
 * ���ùػ���ѹ�ͳ�����
 * @param PowerDownVoltageFlag �ػ���ѹѡ�� 0=3.4V(﮵����) 1=2.7V(�����ɵ����)
 * @param charge_full_mV  ����ֹ��ѹ 0:3962mv 1:4002mv 2:4044mv 3:4086mv 4:4130mv 5:4175mv 6:4222mv 7:4270mv 8:4308mv 9:4349mv 10:4391mv
 * @param charge_full_mA ����ֹ���� 0:2ma 1:5ma 2:7ma 3:10ma 4:15ma 5:20ma 6:25ma 7:30ma
 * @param charge_mA      ������ 0:20ma 1:30ma 2:40ma 3:50ma 4:60ma 5:70ma 6:80ma 7:90ma 8:100ma 9:110ma 10:120ma 11:140ma 12:160ma 13:180ma 14:200ma 15:220ma
 */
void Cmd_24(U8 PowerDownVoltageFlag, U8 charge_full_mV, U8 charge_full_mA, U8 charge_mA)
{
    U8 buf[5] = {0x24};
    buf[1] = (PowerDownVoltageFlag <= 1)? PowerDownVoltageFlag:1;
    buf[2] = (charge_full_mV <= 10)? charge_full_mV:10;
    buf[3] = (charge_full_mA <= 7)? charge_full_mA:7;
    buf[4] = (charge_mA <= 15)? charge_mA:15;
    Dbp("\r\nset PowerDownVoltage and charge parameters--\r\n");
    Cmd_PackAndTx(buf, 5);
}
// ��ȡ �ػ���ѹ�ͳ�����
void Cmd_25(void)
{
    U8 buf[1] = {0x25};
    Dbp("\r\nget PowerDownVoltage and charge parameters--\r\n");
    Cmd_PackAndTx(buf, 1);
}
// �Ͽ��������� ��ָ����лظ�
void Cmd_26(void)
{
    U8 buf[1] = {0x26};
    Dbp("\r\nble disconnect--\r\n");
    Cmd_PackAndTx(buf, 1);
}
/**
 * �����û���GPIO����
 *
 * @param M 0=��������, 1=��������, 2=��������, 3=���0, 4=���1
 */
void Cmd_27(U8 M)
{
    U8 buf[2] = {0x27};
    buf[1] = (M<<4)&0xf0;
    Dbp("\r\nset gpio--\r\n");
    Cmd_PackAndTx(buf, 2);
}

// �豸����
void Cmd_2A(void)
{
    U8 buf[1] = {0x2A};
    Dbp("\r\nreset--\r\n");
    Cmd_PackAndTx(buf, 1);
}
// �豸�ػ�
void Cmd_2B(void)
{
    U8 buf[1] = {0x2B};
    Dbp("\r\npower off--\r\n");
    Cmd_PackAndTx(buf, 1);
}

/**
 * ���� ���йػ�ʱ��
 *
 * @param idleToPowerOffTime ������û��ͨ���������ڹ㲥�У�������ʱ�ﵽ��ô���10������ػ�  0=���ػ�
 */
void Cmd_2C(U8 idleToPowerOffTime)
{
    U8 buf[2] = {0x2C};
    buf[1] = idleToPowerOffTime;
    Dbp("\r\nset idleToPowerOffTime--\r\n");
    Cmd_PackAndTx(buf, 2);
}
// ��ȡ ���йػ�ʱ��
void Cmd_2D(void)
{
    U8 buf[1] = {0x2D};
    Dbp("\r\nget idleToPowerOffTime--\r\n");
    Cmd_PackAndTx(buf, 1);
}

/**
 * ���� ��ֹ������ʽ�������ƺͳ����� ��ʶ
 *
 * @param DisableBleSetNameAndCahrge 1=��ֹͨ�������������Ƽ������� 0=����(Ĭ��) ���ܿͻ��Ĳ�Ʒ�����ñ������������ģ���Ϊ1����
 */
void Cmd_2E(U8 DisableBleSetNameAndCahrge)
{
    U8 buf[2] = {0x2E};
    buf[1] = (DisableBleSetNameAndCahrge <= 1)? DisableBleSetNameAndCahrge:1;
    Dbp("\r\nset FlagForDisableBleSetNameAndCahrge--\r\n");
    Cmd_PackAndTx(buf, 2);
}
// ��ȡ ��ֹ������ʽ�������ƺͳ����� ��ʶ
void Cmd_2F(void)
{
    U8 buf[1] = {0x2F};
    Dbp("\r\nget FlagForDisableBleSetNameAndCahrge--\r\n");
    Cmd_PackAndTx(buf, 1);
}

/**
 * ���� ����ͨ�ŵ�ַ
 *
 * @param address �豸��ַֻ������Ϊ0-254
 */
void Cmd_30(U8 address)
{
    U8 buf[2] = {0x30};
    buf[1] = (address <= 254)? address:254;
    Dbp("\r\nset address--\r\n");
    Cmd_PackAndTx(buf, 2);
}
// ��ȡ ����ͨ�ŵ�ַ
void Cmd_31(void)
{
    U8 buf[1] = {0x31};
    Dbp("\r\nget address--\r\n");
    Cmd_PackAndTx(buf, 1);
}

/**
 * ���� ���ټƺ�����������
 *
 * @param AccRange  Ŀ����ٶ����� 0=2g 1=4g 2=8g 3=16g
 * @param GyroRange Ŀ������������ 0=256 1=512 2=1024 3=2048
 */
void Cmd_33(U8 AccRange, U8 GyroRange)
{
    U8 buf[3] = {0x33};
    buf[1] = (AccRange <= 3)? AccRange:3;
    buf[2] = (GyroRange <= 3)? GyroRange:3;
    Dbp("\r\nset accelRange and gyroRange--\r\n");
    Cmd_PackAndTx(buf, 3);
}
// ��ȡ ���ټƺ�����������
void Cmd_34(void)
{
    U8 buf[1] = {0x34};
    Dbp("\r\nget accelRange and gyroRange--\r\n");
    Cmd_PackAndTx(buf, 1);
}

/**
 * ���� �������Զ�У����ʶ
 *
 * @param GyroAutoFlag  1=�������Զ�У�������ȿ�  0=��
 */
void Cmd_35(U8 GyroAutoFlag)
{
    U8 buf[2] = {0x35};
    buf[1] = (GyroAutoFlag > 0)? 1:0;
    Dbp("\r\nset GyroAutoFlag--\r\n");
    Cmd_PackAndTx(buf, 2);
}
// ��ȡ ���ټƺ�����������
void Cmd_36(void)
{
    U8 buf[1] = {0x36};
    Dbp("\r\nget GyroAutoFlag--\r\n");
    Cmd_PackAndTx(buf, 1);
}

/**
 * ���� ��ֹ����ģʽ�Ĵ���ʱ��
 *
 * @param EcoTime_10s ��ֵ����0�������Զ�����ģʽ(��������˯�ߺ������ϱ�����ֹEcoTime_10s��10���Զ������˶����ģʽ����ͣ�����ϱ�)  0=�������Զ�����
 */
void Cmd_37(U8 EcoTime_10s)
{
    U8 buf[2] = {0x37};
    buf[1] = EcoTime_10s;
    Dbp("\r\nset EcoTime_10s--\r\n");
    Cmd_PackAndTx(buf, 2);
}
// ��ȡ ���ټƺ�����������
void Cmd_38(void)
{
    U8 buf[1] = {0x38};
    Dbp("\r\nget EcoTime_10s--\r\n");
    Cmd_PackAndTx(buf, 1);
}

float __tmpZ;
/**
 * �������յ����ĵ������岢�����û�������Ŀ���󣬹�ע�����Ӧ�����ݼ���--------------------
 * @param pDat Ҫ������������
 * @param DLen ������ĳ���
 */
static void Cmd_RxUnpack(U8 *buf, U8 DLen)
{
    U16 ctl;
    U8 L;
    U8 tmpU8;
    U16 tmpU16;
    U32 tmpU32;
    //F32 tmpX, tmpY, tmpZ, tmpAbs;

    switch (buf[0])
    {
    case 0x02: // ������ ��˯�� �ظ�
        Dbp("\t sensor off success\r\n");
        break;
    case 0x03: // ������ �ѻ��� �ظ�
        Dbp("\t sensor on success\r\n");
        break;
    case 0x32: // ������ ��ʼУ׼ �ظ�
        Dbp("\t compass calibrate begin\r\n");
        break;
    case 0x04: // ������ ����У׼ �ظ�
        Dbp("\t compass calibrate end\r\n");
        break;
    case 0x05: // z��� �ѹ��� �ظ�
        Dbp("\t z-axes to zero success\r\n");
        break;
    case 0x06: // ���� xyz��������ϵ���� �ظ�
        Dbp("\t WorldXYZ-axes to zero success\r\n");
        break;
    case 0x07: // ���ټƼ�У׼���ڽ��У�����9������  �ظ�
        Dbp("\t acceleration calibration, Hold still for 9 seconds\r\n");
        break;
    case 0x08: // �ָ�Ĭ�ϵ���������ϵZ��ָ�򼰻ָ�Ĭ�ϵ���������ϵ  �ظ�
        Dbp("\t axesZ WorldXYZ-axes to zero success\r\n");
        break;
    case 0x10: // ģ�鵱ǰ�����Ժ�״̬ �ظ�
        Dbp("\t still limit: %u\r\n", buf[1]);   // �ֽ�1 �ߵ�-��ֹ״̬���ٶȷ�ֵ ��λdm/s?
        Dbp("\t still to zero: %u\r\n", buf[2]); // �ֽ�2 �ߵ�-��ֹ�����ٶ�(��λmm/s) 0:������ 255:��������
        Dbp("\t move to zero: %u\r\n", buf[3]);  // �ֽ�3 �ߵ�-��̬�����ٶ�(��λmm/s) 0:������
        Dbp("\t compass: %s\r\n", ((buf[4]>>0) & 0x01)? "on":"off" );     // �ֽ�4 bit[0]: 1=�ѿ����ų� 0=�ѹرմų�
        Dbp("\t barometer filter: %u\r\n", (buf[4]>>1) & 0x03);           // �ֽ�4 bit[1-2]: ��ѹ�Ƶ��˲��ȼ�[ȡֵ0-3],��ֵԽ��Խƽ�ȵ�ʵʱ��Խ��
        Dbp("\t IMU: %s\r\n", ((buf[4]>>3) & 0x01)? "on":"off" );         // �ֽ�4 bit[3]: 1=�������ѿ���  0=��������˯��
        Dbp("\t auto report: %s\r\n", ((buf[4]>>4) & 0x01)? "on":"off" ); // �ֽ�4 bit[4]: 1=�ѿ������������������ϱ� 0=�ѹرմ��������������ϱ�
        Dbp("\t FPS: %u\r\n", buf[5]); // �ֽ�5 ���������ϱ��Ĵ���֡��[ȡֵ0-250HZ], 0��ʾ0.5HZ
        Dbp("\t gyro filter: %u\r\n", buf[6]);    // �ֽ�6 �������˲�ϵ��[ȡֵ0-2],��ֵԽ��Խƽ�ȵ�ʵʱ��Խ��
        Dbp("\t acc filter: %u\r\n", buf[7]);     // �ֽ�7 ���ټ��˲�ϵ��[ȡֵ0-4],��ֵԽ��Խƽ�ȵ�ʵʱ��Խ��
        Dbp("\t compass filter: %u\r\n", buf[8]); // �ֽ�8 �������˲�ϵ��[ȡֵ0-9],��ֵԽ��Խƽ�ȵ�ʵʱ��Խ��
        Dbp("\t subscribe tag: 0x%04X\r\n", (U16)(((U16)buf[10]<<8) | buf[9])); // �ֽ�[10-9] ���ܶ��ı�ʶ
        Dbp("\t charged state: %u\r\n", buf[11]); // �ֽ�11 ���״ָ̬ʾ 0=δ�ӵ�Դ 1=����� 2=�ѳ���
        Dbp("\t battery level: %u%%\r\n", buf[12]); // �ֽ�12 ��ǰʣ�����[0-100%]
        Dbp("\t battery voltage: %u mv\r\n", (U16)(((U16)buf[14]<<8) | buf[13])); // �ֽ�[14-13] ��صĵ�ǰ��ѹmv
        Dbp("\t Mac: %02X:%02X:%02X:%02X:%02X:%02X\r\n", buf[15],buf[16],buf[17],buf[18],buf[19],buf[20]); // �ֽ�[15-20] MAC��ַ
        Dbp("\t version: %s\r\n", &buf[21]); // �ֽ�[21-26] �̼��汾 �ַ���
        Dbp("\t product model: %s\r\n", &buf[27]); // �ֽ�[26-32] ��Ʒ�ͺ� �ַ���
        break;
    case 0x11: // ��ȡ���ĵĹ������� �ظ��������ϱ�
        ctl = ((U16)buf[2] << 8) | buf[1];// �ֽ�[2-1] Ϊ���ܶ��ı�ʶ��ָʾ��ǰ��������Щ����
        Dbp("\t subscribe tag: 0x%04X\r\n", ctl);
        Dbp("\t ms: %u\r\n", (U32)(((U32)buf[6]<<24) | ((U32)buf[5]<<16) | ((U32)buf[4]<<8) | ((U32)buf[3]<<0))); // �ֽ�[6-3] Ϊģ�鿪�����ʱ���(��λms)

        L =7; // �ӵ�7�ֽڿ�ʼ���� ���ı�ʶtag������ʣ�µ�����
        if ((ctl & 0x0001) != 0)
        {// ���ٶ�xyz ȥ�������� ʹ��ʱ��*scaleAccel m/s
            tmpX = (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleAccel; L += 2; Dbp("\taX: %.3f\r\n", tmpX); // x���ٶ�aX
            tmpY = (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleAccel; L += 2; Dbp("\taY: %.3f\r\n", tmpY); // y���ٶ�aY
            tmpZ = (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleAccel; L += 2; Dbp("\taZ: %.3f\r\n", tmpZ); // z���ٶ�aZ
            tmpAbs = sqrt(pow2(tmpX) + pow2(tmpY) + pow2(tmpZ)); Dbp("\ta_abs: %.3f\r\n", tmpAbs); // 3��ϳɵľ���ֵ
        }
        if ((ctl & 0x0002) != 0)
        {// ���ٶ�xyz ���������� ʹ��ʱ��*scaleAccel m/s
            tmpX = (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleAccel; L += 2; Dbp("\tAX: %.3f\r\n", tmpX); // x���ٶ�AX
            tmpY = (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleAccel; L += 2; Dbp("\tAY: %.3f\r\n", tmpY); // y���ٶ�AY
            tmpZ = (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleAccel; L += 2; Dbp("\tAZ: %.3f\r\n", tmpZ); // z���ٶ�AZ
            tmpAbs = sqrt(pow2(tmpX) + pow2(tmpY) + pow2(tmpZ)); Dbp("\tA_abs: %.3f\r\n", tmpAbs); // 3��ϳɵľ���ֵ
        }
        if ((ctl & 0x0004) != 0)
        {// ���ٶ�xyz ʹ��ʱ��*scaleAngleSpeed ��/s
            tmpX = (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleAngleSpeed; L += 2; Dbp("\tGX: %.3f\r\n", tmpX); // x���ٶ�GX
            tmpY = (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleAngleSpeed; L += 2; Dbp("\tGY: %.3f\r\n", tmpY); // y���ٶ�GY
            tmpZ = (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleAngleSpeed; L += 2; Dbp("\tGZ: %.3f\r\n", tmpZ); // z���ٶ�GZ
            tmpAbs = sqrt(pow2(tmpX) + pow2(tmpY) + pow2(tmpZ)); Dbp("\tG_abs: %.3f\r\n", tmpAbs); // 3��ϳɵľ���ֵ
        }
        if ((ctl & 0x0008) != 0)
        {// �ų�xyz ʹ��ʱ��*scaleMag uT
            tmpX = (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleMag; L += 2; Dbp("\tCX: %.3f\r\n", tmpX); // x�ų�CX
            tmpY = (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleMag; L += 2; Dbp("\tCY: %.3f\r\n", tmpY); // y�ų�CY
            tmpZ = (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleMag; L += 2; Dbp("\tCZ: %.3f\r\n", tmpZ); // z�ų�CZ
            tmpAbs = sqrt(pow2(tmpX) + pow2(tmpY) + pow2(tmpZ)); Dbp("\tC_abs: %.3f\r\n", tmpAbs); // 3��ϳɵľ���ֵ
        }
        if ((ctl & 0x0010) != 0)
        {// �¶� ��ѹ �߶�
            tmpX = (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleTemperature; L += 2; Dbp("\ttemperature: %.2f\r\n", tmpX); // �¶�

            tmpU32 = (U32)(((U32)buf[L+2] << 16) | ((U32)buf[L+1] << 8) | (U32)buf[L]);
            tmpU32 = ((tmpU32 & 0x800000) == 0x800000)? (tmpU32 | 0xff000000) : tmpU32;// ��24λ�������λΪ1�����ֵΪ��������תΪ32λ������ֱ�Ӳ���ff����
            tmpY = (S32)tmpU32 * scaleAirPressure; L += 3; Dbp("\tairPressure: %.3f\r\n", tmpY); // ��ѹ

            tmpU32 = (U32)(((U32)buf[L+2] << 16) | ((U32)buf[L+1] << 8) | (U32)buf[L]);
            tmpU32 = ((tmpU32 & 0x800000) == 0x800000)? (tmpU32 | 0xff000000) : tmpU32;// ��24λ�������λΪ1�����ֵΪ��������תΪ32λ������ֱ�Ӳ���ff����
            tmpZ = (S32)tmpU32 * scaleHeight; L += 3; Dbp("\theight: %.3f\r\n", tmpZ); // �߶�
        }
        if ((ctl & 0x0020) != 0)
        {// ��Ԫ�� wxyz ʹ��ʱ��*scaleQuat
            tmpAbs = (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleQuat; L += 2; Dbp("\tz: %.3f\r\n", tmpAbs); // w
            tmpX =   (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleQuat; L += 2; Dbp("\tw: %.3f\r\n", tmpX); // x
            tmpY =   (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleQuat; L += 2; Dbp("\tx: %.3f\r\n", tmpY); // y
            tmpZ =   (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleQuat; L += 2; Dbp("\ty: %.3f\r\n", tmpZ); // z
        }
        if ((ctl & 0x0040) != 0)
        {// ŷ����xyz ʹ��ʱ��*scaleAngle
            tmpX = (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleAngle; L += 2; Dbp("\tangleX: %.3f\r\n", tmpX); // x����
            tmpY = (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleAngle; L += 2; Dbp("\tangleY: %.3f\r\n", tmpY); // y����
            tmpZ = (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleAngle; L += 2; Dbp("\tangleZ: %.3f\r\n", tmpZ); // z����
						__tmpZ=tmpZ;
        }
        if ((ctl & 0x0080) != 0)
        {// xyz �ռ�λ�� ��λmm תΪ m
            tmpX = (S16)(((S16)buf[L+1]<<8) | buf[L]) / 1000.0f; L += 2; Dbp("\toffsetX: %.3f\r\n", tmpX); // x����
            tmpY = (S16)(((S16)buf[L+1]<<8) | buf[L]) / 1000.0f; L += 2; Dbp("\toffsetY: %.3f\r\n", tmpY); // y����
            tmpZ = (S16)(((S16)buf[L+1]<<8) | buf[L]) / 1000.0f; L += 2; Dbp("\toffsetZ: %.3f\r\n", tmpZ); // z����
        }
        if ((ctl & 0x0100) != 0)
        {// ��������
            tmpU32 = (U32)(((U32)buf[L+3]<<24) | ((U32)buf[L+2]<<16) | ((U32)buf[L+1]<<8) | ((U32)buf[L]<<0)); L += 4; Dbp("\tsteps: %u\r\n", tmpU32); // �Ʋ���
            tmpU8 = buf[L]; L += 1;
            Dbp("\t walking: %s\r\n", (tmpU8 & 0x01)?  "yes" : "no"); // �Ƿ�����·
            Dbp("\t running: %s\r\n", (tmpU8 & 0x02)?  "yes" : "no"); // �Ƿ����ܲ�
            Dbp("\t biking: %s\r\n",  (tmpU8 & 0x04)?  "yes" : "no"); // �Ƿ����ﳵ
            Dbp("\t driving: %s\r\n", (tmpU8 & 0x08)?  "yes" : "no"); // �Ƿ��ڿ���
        }
        if ((ctl & 0x0200) != 0)
        {// ���ٶ�xyz ȥ�������� ʹ��ʱ��*scaleAccel m/s
            tmpX = (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleAccel; L += 2; Dbp("\tasX: %.3f\r\n", tmpX); // x���ٶ�asX
            tmpY = (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleAccel; L += 2; Dbp("\tasY: %.3f\r\n", tmpY); // y���ٶ�asY
            tmpZ = (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleAccel; L += 2; Dbp("\tasZ: %.3f\r\n", tmpZ); // z���ٶ�asZ
            tmpAbs = sqrt(pow2(tmpX) + pow2(tmpY) + pow2(tmpZ)); Dbp("\tas_abs: %.3f\r\n", tmpAbs); // 3��ϳɵľ���ֵ
        }
        if ((ctl & 0x0400) != 0)
        {// ADC��ֵ
            tmpU16 = (U16)(((U16)buf[L+1]<<8) | ((U16)buf[L]<<0)); L += 2; Dbp("\tadc: %u\r\n", tmpU16); // 12λ����ADC����ֵ(0-4095),��Ӧ�ĵ�ѹ������0x24ָ���趨
        }
        if ((ctl & 0x0800) != 0)
        {// GPIO1��ֵ
            tmpU8 = buf[L]; L += 1;
            Dbp("\t GPIO1  M:%X, N:%X\r\n", (tmpU8>>4)&0x0f, (tmpU8)&0x0f);
        }
        break;
    case 0x12: // ���ò��� �ظ�
        Dbp("\t set parameters success\r\n");
        break;
    case 0x13: // �ߵ���ά�ռ�λ������ �ظ�
        Dbp("\t clear INS position success\r\n");
        break;
    case 0x14: // �ָ�����У׼���� �ظ�
        Dbp("\t Restore calibration parameters from factory mode success\r\n");
        break;
    case 0x15: // ���浱ǰУ׼����Ϊ����У׼���� �ظ�
        Dbp("\t Save calibration parameters to factory mode success\r\n");
        break;
    case 0x16: // �Ʋ������� �ظ�
        Dbp("\t clear steps success\r\n");
        break;
    case 0x17: // ���ټƸ߾���У׼ �ظ�
        if (buf[1] == 255)
        {// �ֽ�1 ֵ255 ��ʾ�ɼ���ɣ����ڽ���У׼(�豸��������־�ֹ�ȴ�10����)
            Dbp("\t calibration success, please wait 10 seconds\r\n");
        }
        else if (buf[1] == 254)
        {// �ֽ�1 ֵ254 ��ʾ�������Լ�ʧ��
            Dbp("\t calibration fail, gyro error\r\n");
        }
        else if (buf[1] == 253)
        {// �ֽ�1 ֵ253 ��ʾ���ټ��Լ�ʧ��
            Dbp("\t calibration fail, accelerometer error\r\n");
        }
        else if (buf[1] == 252)
        {// �ֽ�1 ֵ252 ��ʾ�������Լ�ʧ��
            Dbp("\t calibration fail, compass error\r\n");
        }
        else if (buf[1] == 251)
        {// �ֽ�1 ֵ251 ��ʾ�豸δ��У׼��
            Dbp("\t calibration fail, Hasn't started\r\n");
        }
        else if (buf[1] != 0)
        {// ֵ[1-250] ��ʾ��ǰ�Ѳɼ��Ĵ���
            Dbp("\t calibration, Points collected is %u\r\n", buf[1]);
        }
        else
        {// ֵ0 ��ʾģ���Ѿ���У׼��
            Dbp("\t calibration is running\r\n");
        }
        break;
    case 0x18: // �ѹر������ϱ� �ظ�
        Dbp("\t auto report off\r\n");
        break;
    case 0x19: // �Ѵ������ϱ� �ظ�
        Dbp("\t auto report on\r\n");
        break;
    case 0x20: // ����PCB��װ������� �ظ�
        Dbp("\t set PCB direction success\r\n");
        break;
    case 0x21: // ������ ��ȡ��װ�������
        Dbp_U8_buf("\t get PCB direction: 0x[", "]\r\n",
                   "%02x ",
                   &buf[1], 9); // �ֽ�[1-9]     Ϊ���ټư�װ�������
        Dbp_U8_buf("\t get PCB direction: 0x[", "]\r\n",
                   "%02x ",
                   &buf[10], 9); // �ֽ�[10-18] Ϊ�����ư�װ�������
        break;
    case 0x22: // ������ ���������㲥����
        Dbp("\t set BLE name success\r\n");
        break;
    case 0x23: // ��ȡ�����㲥���� �ظ�
        Dbp("\t get BLE name: %s\r\n", &buf[1]); // �ֽ�[1-16] Ϊ�����㲥�����ַ���
        break;
    case 0x24: // ���ùػ���ѹ�ͳ����� �ظ�
        Dbp("\t set PowerDownVoltage and charge parameters success\r\n");
        break;
    case 0x25: // ��ȡ�ػ���ѹ�ͳ����� �ظ�
        Dbp("\t PowerDownVoltageFlag: %u\r\n", buf[1]); // �ֽ�1 �ػ���ѹѡ���־ 0��ʾ3.4V, 1��ʾ2.7V
        Dbp("\t charge_full_mV: %u\r\n", buf[2]); // �ֽ�2 ����ֹ��ѹ 0:3962mv 1:4002mv 2:4044mv 3:4086mv 4:4130mv 5:4175mv 6:4222mv 7:4270mv 8:4308mv 9:4349mv 10:4391mv
        Dbp("\t charge_full_mA: %u ma\r\n", buf[3]); // �ֽ�3 ����ֹ���� 0:2ma 1:5ma 2:7ma 3:10ma 4:15ma 5:20ma 6:25ma 7:30ma
        Dbp("\t charge_mA: %u ma\r\n", buf[4]); // �ֽ�3 ������ 0:20ma 1:30ma 2:40ma 3:50ma 4:60ma 5:70ma 6:80ma 7:90ma 8:100ma 9:110ma 10:120ma 11:140ma 12:160ma 13:180ma 14:200ma 15:220ma
        break;
    case 0x27: // �����û���GPIO���� �ظ�
        Dbp("\t set gpio success\r\n");
        break;
    case 0x2A: // �����豸 �ظ�
        Dbp("\t will reset\r\n");
        break;
    case 0x2B: // �豸�ػ� �ظ�
        Dbp("\t will power off\r\n");
        break;
    case 0x2C: // ���ÿ��йػ�ʱ�� �ظ�
        Dbp("\t set idleToPowerOffTime success\r\n");
        break;
    case 0x2D: // ��ȡ���йػ�ʱ�� �ظ�
        Dbp("\t idleToPowerOffTime:%u minutes\r\n", buf[1]*10);
        break;
    case 0x2E: // ���ý�ֹ������ʽ�������ƺͳ�������ʶ �ظ�
        Dbp("\t set FlagForDisableBleSetNameAndCahrge success\r\n");
        break;
    case 0x2F: // ��ȡ��ֹ������ʽ�������ƺͳ�������ʶ �ظ�
        Dbp("\t FlagForDisableBleSetNameAndCahrge:%u\r\n", buf[1]);
        break;
    case 0x30: // ���ô���ͨ�ŵ�ַ �ظ�
        Dbp("\t set address success\r\n");
        break;
    case 0x31: // ��ȡ����ͨ�ŵ�ַ �ظ�
        Dbp("\t address:%u\r\n", buf[1]);
        break;
    case 0x33: // ���ü��ټƺ����������� �ظ�
        Dbp("\t set accelRange and gyroRange success\r\n");
        break;
    case 0x34: // ��ȡ���ټƺ����������� �ظ�
        Dbp("\t accelRange:%u gyroRange:%u\r\n", buf[1], buf[2]);
        break;
    case 0x35: // �����������Զ�У����ʶ �ظ�
        Dbp("\t set GyroAutoFlag success\r\n");
        break;
    case 0x36: // ��ȡ�������Զ�У����ʶ �ظ�
        Dbp("\t GyroAutoFlag:%u\r\n", buf[1]);
        break;
    case 0x37: // ���þ�ֹ����ģʽ�Ĵ���ʱ�� �ظ�
        Dbp("\t set EcoTime success\r\n");
        break;
    case 0x38: // ��ȡ��ֹ����ģʽ�Ĵ���ʱ�� �ظ�
        Dbp("\t EcoTime:%u\r\n", buf[1]);
        break;

        default:
            break;
    }
}

/**
 * ��������  ��Ҫ�û�ʵ������Ĵ��ڷ������ݷ���-------------------------------------
 * @param pBuf Ҫ���͵�����ָ��
 * @param Len Ҫ���͵��ֽ���
 */
static void Cmd_Write(U8 *pBuf, int Len)
{
    // ͨ��UART_Write��������ͨ�������������û���Եײ�Ӳ��ʵ��UART_Write������bufָ��ָ���Len�ֽ����ݷ��ͳ�ȥ����
    
    UART_Write(3,  pBuf, Len);

    Dbp_U8_buf("tx: ", "\r\n",
               "%02X ",
               pBuf, Len);
}

// ======================================����ʾ��==============================================
U8 im948_ctl = 0; // �û����Կڷ�����1�ֽڲ���ָ�� ���Կ��յ������ݸ�ֵ��im948_ctl ��ѭ������� im948_test() ������ʾ
void im948_test(void)
{
    U8 ctl = im948_ctl;
    if (im948_ctl)
    {
        im948_ctl = 0;
    }

    switch (ctl)
    {
    case '1':// 1 ˯�ߴ�����
        Cmd_02();
        break;
    case '2':// 2 ���Ѵ�����
        Cmd_03();
        break;

    case '3':// 3 �ر����������ϱ�
        Cmd_18();
        break;
    case '4':// 4 �������������ϱ�
        Cmd_19();
        break;

    case '5':// 5 ��ȡ1�ζ��ĵĹ�������
        Cmd_11();
        break;

    case '6':// 6 ��ȡ�豸���Ժ�״̬
        Cmd_10();
        break;
    case '7':// 7 �����豸����(����1)
        /**
         * �����豸����
         * @param accStill    �ߵ�-��ֹ״̬���ٶȷ�ֵ ��λdm/s?
         * @param stillToZero �ߵ�-��ֹ�����ٶ�(��λcm/s) 0:������ 255:��������
         * @param moveToZero  �ߵ�-��̬�����ٶ�(��λcm/s) 0:������
         * @param isCompassOn 1=�迪���ų� 0=��رմų�
         * @param barometerFilter ��ѹ�Ƶ��˲��ȼ�[ȡֵ0-3],��ֵԽ��Խƽ�ȵ�ʵʱ��Խ��
         * @param reportHz ���������ϱ��Ĵ���֡��[ȡֵ0-250HZ], 0��ʾ0.5HZ
         * @param gyroFilter    �������˲�ϵ��[ȡֵ0-2],��ֵԽ��Խƽ�ȵ�ʵʱ��Խ��
         * @param accFilter     ���ټ��˲�ϵ��[ȡֵ0-4],��ֵԽ��Խƽ�ȵ�ʵʱ��Խ��
         * @param compassFilter �������˲�ϵ��[ȡֵ0-9],��ֵԽ��Խƽ�ȵ�ʵʱ��Խ��
         * @param Cmd_ReportTag ���ܶ��ı�ʶ
         */
        Cmd_12(5, 255, 0,  1, 3, 2, 2, 4, 9, 0xFFFF);
        break;
    case '8':// 8 �����豸����(����2)
        Cmd_12(8,   6, 5,  0, 1,30, 1, 2, 7, 0x0002);
        break;

    case '9':// 9 �ߵ���ά�ռ�λ������
        Cmd_13();
        break;
    case 'a':// a �Ʋ�������
        Cmd_16();
        break;

    case 'b':// b �ָ�����У׼����
        Cmd_14();
        break;
    case 'c':// c ���浱ǰУ׼����Ϊ����У׼����
        Cmd_15();
        break;

    case 'd':// d ���ټƼ���У׼
        Cmd_07();
        break;

    case 'e':// e ���ټƸ߾���У׼ ��ʼ
        /**
         * ���ټƸ߾���У׼
         * @param flag ��ģ��δ����У׼״̬ʱ��
         *                 ֵ0 ��ʾ����ʼһ��У׼���ɼ�1������
         *                 ֵ255 ��ʾѯ���豸�Ƿ�����У׼
         *             ��ģ������У׼��:
         *                 ֵ1 ��ʾҪ�ɼ���1������
         *                 ֵ255 ��ʾҪ�ɼ����1�����ݲ�����
         */
        Cmd_17(0);
        break;
    case 'f':// f ���ټƸ߾���У׼ �ɼ�1���� �����ٲɼ�6����ֹ��ĵ�
        /**
         * ���ټƸ߾���У׼
         * @param flag ��ģ��δ����У׼״̬ʱ��
         *                 ֵ0 ��ʾ����ʼһ��У׼���ɼ�1������
         *                 ֵ255 ��ʾѯ���豸�Ƿ�����У׼
         *             ��ģ������У׼��:
         *                 ֵ1 ��ʾҪ�ɼ���1������
         *                 ֵ255 ��ʾҪ�ɼ����1�����ݲ�����
         */
        Cmd_17(1);
        break;
    case 'g':// g ���ټƸ߾���У׼ �ɼ����1�����ݲ����� ��ѯ���豸�Ƿ�����У׼
        /**
         * ���ټƸ߾���У׼
         * @param flag ��ģ��δ����У׼״̬ʱ��
         *                 ֵ0 ��ʾ����ʼһ��У׼���ɼ�1������
         *                 ֵ255 ��ʾѯ���豸�Ƿ�����У׼
         *             ��ģ������У׼��:
         *                 ֵ1 ��ʾҪ�ɼ���1������
         *                 ֵ255 ��ʾҪ�ɼ����1�����ݲ�����
         */
        Cmd_17(255);
        break;
    case 'H':// H ��ʼ������У׼
        Cmd_32();
    case 'h':// h ����������У׼
        Cmd_04();
        break;
    case 'i':// i z��ǹ���
        Cmd_05();
        break;
    case 'j':// j xyz��������ϵ����
        Cmd_06();
        break;
    case 'k':// k �ָ�Ĭ�ϵ���������ϵZ��ָ�򼰻ָ�Ĭ�ϵ���������ϵ
        Cmd_08();
        break;

    case 'l':// l ����PCB��װ������� Ϊģ���˿ӡ��ʶ����
        {
            S8 accMatrix[9] =
            {// ���ټ���ģ���ʶ����һ��
                1, 0, 0,
                0, 1, 0,
                0, 0, 1
            };
            S8 comMatrix[9] =
            {// ��������ģ���ʶ����һ��
                1, 0, 0,
                0,-1, 0,
                0, 0,-1
            };
            Cmd_20(accMatrix, comMatrix);
        }
        break;
    case 'm':// m ����PCB��װ������� Ϊģ���˿ӡ��ʶ������x����ת90�� ���ŷ�
        {
            S8 accMatrix[9] =
            {// ���� �����ټ���ģ���x����ת90��  ok
                1, 0, 0,
                0, 0,-1,
                0, 1, 0
            };
            S8 comMatrix[9] =
            {// ���� ����������ģ���x����ת90��  ok
                1, 0, 0,
                0, 0, 1,
                0,-1, 0
            };
            Cmd_20(accMatrix, comMatrix);
        }
        break;
    case 'n': // n ��ȡPCB��װ�������
        Cmd_21();
        break;

    case 'o': // o ���������㲥����Ϊ im948
        Cmd_22("im948");
        break;
    case 'p': // p ���������㲥����Ϊ helloBle
        Cmd_22("helloBle");
        break;
    case 'q': // q ��ȡ�����㲥����
        Cmd_23();
        break;

    case 'r': // r ����Ϊ�ػ���ѹ2.7V  ����ֹ��ѹ4.22V  ����ֹ����10ma ������50ma  ģ�����Ĭ�Ͼ����������
        Cmd_24(  1,                   6,                 3,              3);
        break;
    case 's': // s ����Ϊ�ػ���ѹ3.4V  ����ֹ��ѹ4.22V  ����ֹ����15ma  ������200ma
        Cmd_24(  0,                   6,                 4,              14);
        break;
    case 't': // t ��ȡ AD1���ŵ�ѹ��ⷶΧ��������͡��ػ���ѹ
        Cmd_25();
        break;

    case 'u': // u �Ͽ���������
        Cmd_26();
        break;

    case 'v': // v �����û���GPIO���� ��Ϊ��������
        Cmd_27(1);
        break;
    case 'w': // w �����û���GPIO���� ��Ϊ��������
        Cmd_27(2);
        break;


    case 'x': // x �����豸
        Cmd_2A();
        break;
    case 'y': // y �豸�ػ�
        Cmd_2B();
        break;

    case 'z': // z ���ÿ��йػ�ʱ��
        Cmd_2C(0); // ���в��Զ��ػ�
        break;
    case 'A': // A ���ÿ��йػ�ʱ��
        Cmd_2C(144); // ��������1���Զ��ػ�(��Ҳ�ǳ���Ĭ��ֵ)
        break;
    case 'B': // B ��ȡ���йػ�ʱ��
        Cmd_2D();
        break;

    case 'C': // C ���ý�ֹ������ʽ�������ƺͳ�������ʶ
        Cmd_2E(0); // ��Ϊ����(��Ҳ�ǳ���Ĭ��ֵ)
        break;
    case 'D': // D ���ý�ֹ������ʽ�������ƺͳ�������ʶ
        Cmd_2E(1); // ��Ϊ��ֹ
        break;
    case 'E': // E ��ȡ��ֹ������ʽ�������ƺͳ�������ʶ
        Cmd_2F();
        break;

    case 'F': // F ���ô���ͨ�ŵ�ַ ֻ������Ϊ0-254
        Cmd_30(0); // ��Ϊ0(��Ҳ�ǳ���Ĭ��ֵ)
        break;
    case 'G': // G ���ô���ͨ�ŵ�ַ
        Cmd_30(1); // ��Ϊ1
        break;
    case 'I': // I ��ȡ����ͨ�ŵ�ַ
        Cmd_31();
        break;

    case 'J': // J ���ü��ټƺ�����������
       /**
        * ���� ���ټƺ�����������
        * @param AccRange  Ŀ����ٶ����� 0=2g 1=4g 2=8g 3=16g
        * @param GyroRange Ŀ������������ 0=256 1=512 2=1024 3=2048
        */
        Cmd_33(3, 3); // ��Ϊ���ټ�16g, ������2048dps
        break;
    case 'K': // K ���ü��ټƺ�����������
        Cmd_33(1, 2); // ��Ϊ���ټ�4g, ������1024dps
        break;
    case 'L': // L ��ȡ���ټƺ�����������
        Cmd_34();
        break;

    case 'M': // M �����������Զ�У����ʶ 1=�������Զ�У�������ȿ�  0=��
        Cmd_35(1); // ��Ϊ1����(��Ҳ�ǳ���Ĭ��ֵ)
        break;
    case 'N': // N �����������Զ�У����ʶ
        Cmd_35(0); // ��Ϊ0�ر�
        break;
    case 'O': // O ��ȡ�������Զ�У����ʶ
        Cmd_36();
        break;

    case 'P': // P ���þ�ֹ����ģʽ�Ĵ���ʱ�� EcoTime_10s����0�������Զ�����ģʽ(��������˯�ߺ������ϱ�����ֹEcoTime_10s��10���Զ������˶����ģʽ����ͣ�����ϱ�)  0=�������Զ�����
        Cmd_37(0); // ��Ϊ0�ر�(��Ҳ�ǳ���Ĭ��ֵ)
        break;
    case 'Q': // Q ���þ�ֹ����ģʽ�Ĵ���ʱ��
        Cmd_37(6*5); // ��Ϊ5����
        break;
    case 'R': // R ��ȡ��ֹ����ģʽ�Ĵ���ʱ��
        Cmd_38();
        break;

    }
}



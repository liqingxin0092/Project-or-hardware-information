#include "28BYJ-48.h"
#include "Delay.h"

// 步进电机速度不能快，否则会丢步数
void ULN2003_Init(struct ULN2003_t *p)
{
    p->ULN_PORT = GPIOA;
    p->A_Pin = GPIO_PIN_0;
    p->B_Pin = GPIO_PIN_1;
    p->C_Pin = GPIO_PIN_2;
    p->D_Pin = GPIO_PIN_3;
    p->func = _3;
}

void _3(struct ULN2003_t *temp, double angle, uint16_t external_delay_us, uint8_t turn_flag)
{
    double temp_ = angle * 512 / 360.0;
    double temp__ = (uint16_t)temp_;
    uint16_t decimal_part_X_state = (temp_ - temp__) * 7;
    if (!turn_flag)
    {
        for (uint16_t i = 0; i < temp_; i++)
        {
            SET_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // A
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            SET_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // AB
            SET_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // B
            SET_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // BC
            SET_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            SET_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // C
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            SET_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // CD
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            SET_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            SET_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // D
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            SET_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // 空
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
        }
        if (decimal_part_X_state == 1)
        {
            SET_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // A
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
        }
        else if (decimal_part_X_state == 2)
        {
            SET_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // A
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            SET_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // AB
            SET_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
        }
        else if (decimal_part_X_state == 3)
        {
            SET_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // A
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            SET_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // AB
            SET_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // B
            SET_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
        }
        else if (decimal_part_X_state == 4)
        {
            SET_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // A
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            SET_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // AB
            SET_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // B
            SET_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // BC
            SET_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            SET_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
        }
        else if (decimal_part_X_state == 5)
        {
            SET_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // A
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            SET_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // AB
            SET_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // B
            SET_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // BC
            SET_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            SET_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // C
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            SET_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
        }
        else if (decimal_part_X_state == 6)
        {
            SET_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // A
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            SET_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // AB
            SET_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // B
            SET_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // BC
            SET_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            SET_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // C
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            SET_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // CD
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            SET_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            SET_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
        }
    }
    else if (turn_flag)
    {
        for (uint16_t i = 0; i < temp_; i++)
        {
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // D
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            SET_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // DC
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            SET_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            SET_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // C
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            SET_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // BC
            SET_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            SET_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // B
            SET_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            SET_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // BA
            SET_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            SET_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // A
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // 空
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
        }
        if (decimal_part_X_state == 1)
        {
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // D
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            SET_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
        }
        else if (decimal_part_X_state == 2)
        {
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // D
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            SET_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // DC
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            SET_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            SET_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
        }
        else if (decimal_part_X_state == 3)
        {
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // D
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            SET_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // DC
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            SET_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            SET_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // C
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            SET_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
        }
        else if (decimal_part_X_state == 4)
        {
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // D
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            SET_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // DC
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            SET_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            SET_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // C
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            SET_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // BC
            SET_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            SET_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
        }
        else if (decimal_part_X_state == 5)
        {
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // D
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            SET_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // DC
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            SET_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            SET_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // C
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            SET_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // BC
            SET_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            SET_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // B
            SET_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
        }
        else if (decimal_part_X_state == 6)
        {
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // D
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            SET_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // DC
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            SET_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            SET_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // C
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            SET_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // BC
            SET_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            SET_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // B
            SET_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
            SET_BIT(temp->ULN_PORT->ODR, temp->A_Pin); // BA
            SET_BIT(temp->ULN_PORT->ODR, temp->B_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->C_Pin);
            CLEAR_BIT(temp->ULN_PORT->ODR, temp->D_Pin);
            Delay_us(external_delay_us);
        }
    }
}

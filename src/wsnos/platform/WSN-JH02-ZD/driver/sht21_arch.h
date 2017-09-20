#ifndef __SENSOR_HUMITURE_H__
#define __SENSOR_HUMITURE_H__

#define HUMITURE_SENSOR_RH	0
#define HUMITURE_SENSOR_T	1

#define SHT21_ADDRESS   							0x40

#define HUMITURE_I2C_WRITE_START()                  \
    do                                              \
    {                                               \
        UCB2I2CSA = SHT21_ADDRESS;                  \
        while(UCB2CTL1 & UCTXSTP);                  \
        UCB2CTL1 |= UCTR;                           \
        UCB2CTL1 |= UCTXSTT;                        \
    } while(__LINE__ == -1)

#define HUMITURE_I2C_READ_START()                   \
    do                                              \
    {                                               \
        uint8_t i;                                  \
        while(UCB2CTL1 & UCTXSTP);                  \
        UCB2CTL1 &= ~UCTR;                          \
        UCB2CTL1 |= UCTXSTT;                        \
        i = UCB1RXBUF;                              \
        i = i;                                      \
    } while(__LINE__ == -1)

#define HUMITURE_I2C_WAIT_ADDR_ACK()                while(UCB2CTL1 & UCTXSTT)
#define HUMITURE_I2C_SEND_STOP_BIT()                UCB2CTL1 |= UCTXSTP
#define HUMITURE_I2C_WAIT_STOP()                    while(UCB2CTL1 & UCTXSTP)

#define HUMITURE_I2C_SEND_CHAR(x)                   \
    do                                              \
    {                                               \
        while(!(UCB2IFG & UCTXIFG));                \
        UCB2TXBUF = x;                              \
    } while(__LINE__ == -1)

#define HUMITURE_I2C_RECCEIVE_CHAR(x)               \
    do                                              \
    {                                               \
        while(!(UCB2IFG & UCRXIFG));                \
        x = UCB2RXBUF;                              \
    } while(__LINE__ == -1)

#define HUMITURE_I2C_IS_NO_ACK()                    (UCB2IFG & UCNACKIFG)
        
void sht20_i2c_init(void);
void sht20_i2c_deinit(void);
void sht20_pow_open(bool_t para);

#endif
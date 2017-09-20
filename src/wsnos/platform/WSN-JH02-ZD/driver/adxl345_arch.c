/***************************************************************************
* File        : adxl345_arch.c
* Summary     : 
* Version     : v0.1
* Author      : wangjifang
* Date        : 2016/10/25
* Change Logs :
* Date       Versi on     Author     Notes
* 2016/10/25         v0.1            wangjifang        first version
***************************************************************************/


#include "common/lib/lib.h"
#include "sys_arch/osel_arch.h"
#include "driver.h"
#include "adxl345_arch.h"

/*****************************************************************************
Function:
  bool_t adxl345_read_register( uint8_t reg_add , uint8_t *pvalue)
Description: 
   MSP430自动发送从机
Input:
	uint8_t reg_add    寄存器地址
Output:
    uint8_t *pvalue    out 寄存器内容
Return: 
	I2C_FAIL或I2C_OK
Others: 
******************************************************************************/
bool_t adxl345_read_register( uint8_t reg_add , uint8_t *pvalue)
{
    UCB1I2CSA = ADXL345_ADDRESS;
    UCB1CTL1 |= UCTR;                 // 写模式
    UCB1CTL1 |= UCTXSTT;              // 发送启动位和写控制字节

    UCB1TXBUF = reg_add;              // 发送数据，必须要先填充TXBUF
    // 等待UCTXIFG=1 与UCTXSTT=0 同时变化 等待一个标志位即可
    while(!(UCB1IFG & UCTXIFG))
    {
        if( UCB1IFG & UCNACKIFG )       // 若无应答 UCNACKIFG=1
        {
            return I2C_FAIL;
        }
    }

    UCB1CTL1 &= ~UCTR;                // 读模式
    UCB1CTL1 |= UCTXSTT;              // 发送启动位和读控制字节

    while(UCB1CTL1 & UCTXSTT);        // 等待UCTXSTT=0
    // 若无应答 UCNACKIFG = 1

    UCB1CTL1 |= UCTXSTP;              // 先发送停止位

    while(!(UCB1IFG & UCRXIFG));      // 读取数据，读取数据在发送停止位之后
    *pvalue = UCB1RXBUF;

    while( UCB1CTL1 & UCTXSTP );

    return I2C_OK ;
}
/*******************************************************************************
Function:
  bool_t adxl345_write_register( uint8_t reg_add , uint8_t reg_value)
Description: 
    向寄存器中写入单个字节数据
Input:
	uint8_t reg_add    寄存器地址
	uint8_t reg_value   寄存器内容
Output:
       无
Return: 
	I2C_FAIL或I2C_OK
Others: 
********************************************************************************/
bool_t adxl345_write_register( uint8_t reg_add , uint8_t reg_value)
{
    UCB1I2CSA = ADXL345_ADDRESS;
    while( UCB1CTL1 & UCTXSTP );
    UCB1CTL1 |= UCTR;                 // 写模式
    UCB1CTL1 |= UCTXSTT;              // 发送启动位

    UCB1TXBUF = reg_add;             // 发送寄存器地址
    // 等待UCTXIFG=1 与UCTXSTT=0 同时变化 等待一个标志位即可
    while(!(UCB1IFG & UCTXIFG))
    {
        if( UCB1IFG & UCNACKIFG )       // 若无应答 UCNACKIFG=1
        {
            return I2C_FAIL;
        }
    }

    UCB1TXBUF = reg_value;            // 发送寄存器内容
    while(!(UCB1IFG & UCTXIFG));      // 等待UCTXIFG=1

    UCB1CTL1 |= UCTXSTP;
    while(UCB1CTL1 & UCTXSTP);        // 等待发送完成

    return I2C_OK ;
}
/********************************************************************************
Function:
    bool_t adxl345_read_buff(uint8_t reg_add , uint8_t *pregbuf , uint8_t  len)
Description: 
    从传感器寄存器连续读取多个字节
Input:
	uint8_t RegAddr    寄存器地址
	uint8_t Len     读取字节长度
Output:
       uint8_t *pregbuf    out 寄存器内容
Return: 
	I2C_FAIL或I2C_OK
Others: 
********************************************************************************/
bool_t adxl345_read_buff(uint8_t reg_add , uint8_t *pregbuf , uint8_t  len)
{
    UCB1I2CSA = ADXL345_ADDRESS;
    while( UCB1CTL1 & UCTXSTP );
    UCB1CTL1 |= UCTR;                 // 写模式
    UCB1CTL1 |= UCTXSTT;              // 发送启动位和写控制字节

    UCB1TXBUF = reg_add;             // 发送数据
    // 等待UCTXIFG=1 与UCTXSTT=0 同时变化 等待一个标志位即可
    while(!(UCB1IFG & UCTXIFG))
    {
        if( UCB1IFG & UCNACKIFG )       // 若无应答 UCNACKIFG=1
        {
            return I2C_FAIL;
        }
    }

    UCB1CTL1 &= ~UCTR;                // 读模式
    UCB1CTL1 |= UCTXSTT;              // 发送启动位和读控制字节

    while(UCB1CTL1 & UCTXSTT);        // 等待UCTXSTT=0
    // 若无应答 UCNACKIFG = 1

    for(uint8_t i = 0; i < (len - 1); i++)
    {
        while(!(UCB1IFG & UCRXIFG));    // 读取数据
        *pregbuf++ = UCB1RXBUF;
    }

    UCB1CTL1 |= UCTXSTP;              // 在接收最后一个字节之前发送停止位

    while(!(UCB1IFG & UCRXIFG));      // 读取数据
    *pregbuf = UCB1RXBUF;

    while( UCB1CTL1 & UCTXSTP );

    return I2C_OK;
}

/*******************************************************************************
Function:
    static void accelerated_i2c_lock_init(void)
Description: 
    使用前I2C的释放操作
Input:
	无
Output:

Return: 
	
Others: 
********************************************************************************/
static void accelerated_i2c_lock_init(void)
{    
    //P5.4 作为模拟scl，输出9个信号
    P8SEL &= ~BIT6;// P5.4置成IO口模式
	P8DIR |= BIT6; //P5.4做为输出
    P8OUT |= BIT6;
    // 主设备模拟SCL，从高到低，输出9次，使得从设备释放SDA
    for(uint8_t i=0;i<9;i++)
    {
        P8OUT |= BIT6  ;
        delay_ms(1);
        P8OUT &= ~BIT6;
        delay_ms(1);
    }
    
}
/********************************************************************************
Function:
    static void accelerated_iic_init(void)
Description: 
    I2C模块的初始化:包括使用前I2C的释放操作和寄存器的配置
Input:
    void
Output:
    void
Return: 
Others: 
*********************************************************************************/
void accelerated_iic_init(void)
{
    accelerated_i2c_lock_init();
    
    P8SEL |= (BIT5 + BIT6);
    UCB1CTL1 |= UCSWRST;                    //软件复位使能
    UCB1CTL0 = UCMST + UCMODE_3 + UCSYNC ;  // I2C主机模式
    UCB1CTL1 |= UCSSEL_2;                   // 选择SMCLK
    UCB1BR0 = 40;                // I2C时钟约 100hz，F(i2c)=F(smclk)/BR,smclk=8m
    UCB1BR1 = 0;
    UCB1CTL0 &= ~UCSLA10;                   // 7位地址模式
    //UCB1I2CSA = ADXL345_ADDRESS;            // ADXL345
    UCB1CTL1 &= ~UCSWRST;       //软件清除UCSWRST可以释放USCI,使其进入操作状态
}

void accelerated_iic_deinit(void)
{
    P8SEL &= ~(BIT5 + BIT6);
    P8DIR &= ~(BIT5 + BIT6);
}

void accelerated_port1_init(void)
{
    TB0CCTL3 &= ~CCIE;
//    pmap_cfg(0, PM_TB0CCR3B);
  	P2SEL |= BIT0;
	P2DIR &= ~BIT0;
    TB0CCTL3 = CM_1 + CCIS_1 + SCS + CAP;   //CM_2上升沿捕获，CCIS_1选择CCIxB,
                                            //SCS同步捕获，CAP捕获模式  
}

void accelerated_port2_init(void)
{
    TB0CCTL4 &= ~CCIE;
//    pmap_cfg(1, PM_TB0CCR4B);
  	P2SEL |= BIT1;
	P2DIR &= ~BIT1;
    TB0CCTL4 = CM_1 + CCIS_1 + SCS + CAP;   //CM_2上升沿捕获，CCIS_1选择CCIxB,
                                            //SCS同步捕获，CAP捕获模式  
}

void accelerated_int1_cfg(void)
{
	TB0CCTL3 &= ~CCIE;
    TB0CCTL3 &= ~CCIFG;     //初始为中断未挂起
    TB0CCTL3 |= CCIE;     // 开启中断使能
}

void accelerated_int2_cfg(void)
{
	TB0CCTL4 &= ~CCIE;
    TB0CCTL4 &= ~CCIFG;     //初始为中断未挂起
    TB0CCTL4 |= CCIE;     // 开启中断使能
}

void acc_power_open(bool_t para)
{
    P5SEL &= ~(BIT7);
    P5DIR |= (BIT7);
    if(para == TRUE)
    {
        P5OUT |= BIT7;
    }
    else
    {
        P5OUT &= ~BIT7;
    }
}




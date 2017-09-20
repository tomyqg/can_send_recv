/***************************************************************************
* File        : adxl345_arch.c
* Summary     : 
* Version     : v0.1
* Author      : wangjifang
* Date        : 2017/8/7
* Change Logs :
* Date       Versi on     Author     Notes
* 2017/8/7         v0.1            wangjifang        first version
***************************************************************************/

#include "platform/platform.h"
#include "wsnos/sys_arch/wsnos/wsnos.h"
#include "adxl345_arch.h"

#define EVAL_I2Cx_TIMEOUT_MAX                   3000

static TIM_HandleTypeDef acce_time_handle;
I2C_HandleTypeDef hi2c_adxl345;
uint32_t I2cxTimeout = EVAL_I2Cx_TIMEOUT_MAX;

void adxl345_i2c_init(void)
{
    hi2c_adxl345.Instance             = ADXL345_I2Cx;
    hi2c_adxl345.Init.ClockSpeed      = I2C_SPEEDCLOCK;
    hi2c_adxl345.Init.DutyCycle       = I2C_DUTYCYCLE;
    hi2c_adxl345.Init.OwnAddress1     = 0;
    hi2c_adxl345.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
    hi2c_adxl345.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c_adxl345.Init.OwnAddress2     = 0;
    hi2c_adxl345.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c_adxl345.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;
    HAL_I2C_Init(&hi2c_adxl345);
}

void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  if(hi2c->Instance==ADXL345_I2Cx)
  {  
    /* 使能外设时钟 */
    ADXL345_I2C_RCC_CLK_ENABLE();        
    ADXL345_I2C_GPIO_CLK_ENABLE();
    GPIO_InitStruct.Pin = ADXL345_I2C_SCL_PIN|ADXL345_I2C_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(ADXL345_I2C_GPIO_PORT, &GPIO_InitStruct);
  }
  else if(hi2c->Instance==MMA845X_I2Cx)
  {  
    /* 使能外设时钟 */
    MMA845X_I2C_RCC_CLK_ENABLE();        
    MMA845X_I2C_GPIO_CLK_ENABLE();
    GPIO_InitStruct.Pin = MMA845X_I2C_SCL_PIN|MMA845X_I2C_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(MMA845X_I2C_GPIO_PORT, &GPIO_InitStruct);
  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c)
{
  if(hi2c->Instance==ADXL345_I2Cx)
  {
    /* 禁用外设时钟 */
    ADXL345_I2C_GPIO_CLK_DISABLE();
    HAL_GPIO_DeInit(ADXL345_I2C_GPIO_PORT, ADXL345_I2C_SCL_PIN|ADXL345_I2C_SDA_PIN);
  }
  if(hi2c->Instance==MMA845X_I2Cx)
  {
    /* 禁用外设时钟 */
    MMA845X_I2C_GPIO_CLK_DISABLE();
    HAL_GPIO_DeInit(MMA845X_I2C_GPIO_PORT, MMA845X_I2C_SCL_PIN|MMA845X_I2C_SDA_PIN);
  }
}

static void I2C_ADXL345_Error (void)
{
  /* 反初始化I2C通信总线 */
  HAL_I2C_DeInit(&hi2c_adxl345);
  
  /* 重新初始化I2C通信总线*/
  adxl345_i2c_init();
}

/**
  * 函数功能: 通过I2C写入一个值到指定寄存器内
  * 输入参数: Addr：I2C设备地址
  *           Reg：目标寄存器
  *           Value：值
  * 返 回 值: 无
  * 说    明: 无
  */
void I2C_ADXL345_WriteData(uint16_t Addr, uint8_t Reg, uint8_t Value)
{
  HAL_StatusTypeDef status = HAL_OK;
  
  status = HAL_I2C_Mem_Write(&hi2c_adxl345, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, &Value, 1, I2cxTimeout);
  
  /* 检测I2C通信状态 */
  if(status != HAL_OK)
  {
    /* 调用I2C通信错误处理函数 */
    I2C_ADXL345_Error();
  }
}

/**
  * 函数功能: 通过I2C写入一段数据到指定寄存器内
  * 输入参数: Addr：I2C设备地址
  *           Reg：目标寄存器
  *           RegSize：寄存器尺寸(8位或者16位)
  *           pBuffer：缓冲区指针
  *           Length：缓冲区长度
  * 返 回 值: HAL_StatusTypeDef：操作结果
  * 说    明: 在循环调用是需加一定延时时间
  */
HAL_StatusTypeDef I2C_ADXL345_WriteBuffer(uint16_t Addr, uint8_t Reg, uint16_t RegSize, uint8_t *pBuffer, uint16_t Length)
{
  HAL_StatusTypeDef status = HAL_OK;
  
  status = HAL_I2C_Mem_Write(&hi2c_adxl345, Addr, (uint16_t)Reg, RegSize, pBuffer, Length, I2cxTimeout); 

  /* 检测I2C通信状态 */
  if(status != HAL_OK)
  {
    /* 调用I2C通信错误处理函数 */
    I2C_ADXL345_Error();
  }        
  return status;
}


/**
  * 函数功能: 通过I2C读取一个指定寄存器内容
  * 输入参数: Addr：I2C设备地址
  *           Reg：目标寄存器
  * 返 回 值: uint8_t：寄存器内容
  * 说    明: 无
  */
uint8_t I2C_ADXL345_ReadData(uint16_t Addr, uint8_t Reg)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint8_t value = 0;
  
  status = HAL_I2C_Mem_Read(&hi2c_adxl345, Addr, Reg, I2C_MEMADD_SIZE_8BIT, &value, 1, I2cxTimeout);
 
  /* 检测I2C通信状态 */
  if(status != HAL_OK)
  {
    /* 调用I2C通信错误处理函数 */
    I2C_ADXL345_Error();
  
  }
  return value;
}

/**
  * 函数功能: 通过I2C读取一段寄存器内容存放到指定的缓冲区内
  * 输入参数: Addr：I2C设备地址
  *           Reg：目标寄存器
  *           RegSize：寄存器尺寸(8位或者16位)
  *           pBuffer：缓冲区指针
  *           Length：缓冲区长度
  * 返 回 值: HAL_StatusTypeDef：操作结果
  * 说    明: 无
  */
HAL_StatusTypeDef I2C_ADXL345_ReadBuffer(uint16_t Addr, uint8_t Reg, uint16_t RegSize, uint8_t *pBuffer, uint16_t Length)
{
  HAL_StatusTypeDef status = HAL_OK;

  status = HAL_I2C_Mem_Read(&hi2c_adxl345, Addr, (uint16_t)Reg, RegSize, pBuffer, Length, I2cxTimeout);
  
  /* 检测I2C通信状态 */
  if(status != HAL_OK)
  {
    /* 调用I2C通信错误处理函数 */
    I2C_ADXL345_Error();
  }        
  return status;
}

/**
  * 函数功能: 检测I2C设备是否处于准备好可以通信状态
  * 输入参数: DevAddress：I2C设备地址
  *           Trials：尝试测试次数
  * 返 回 值: HAL_StatusTypeDef：操作结果
  * 说    明: 无
  */
HAL_StatusTypeDef I2C_ADXL345_IsDeviceReady(uint16_t DevAddress, uint32_t Trials)
{ 
  return (HAL_I2C_IsDeviceReady(&hi2c_adxl345, DevAddress, Trials, I2cxTimeout));
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
    bool_t status = I2C_OK;

    I2C_ADXL345_WriteBuffer(ADXL345_I2C_ADDRESS, reg_add,I2C_MEMADD_SIZE_8BIT, &reg_value, 1 );

    return status ;
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
    bool_t status = I2C_OK;

    I2C_ADXL345_ReadBuffer(ADXL345_I2C_ADDRESS, reg_add, I2C_MEMADD_SIZE_8BIT, pregbuf, len);

    return status;
}

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
    return adxl345_read_buff(reg_add, pvalue, 1);
}


// interrupt handler
void TIM7_IRQHandler(void)
{
    OSEL_ISR_ENTRY();
    
    HAL_TIM_IRQHandler(&acce_time_handle);
    
    OSEL_ISR_EXIT();
}
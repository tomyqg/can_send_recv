#include "common/lib/lib.h"
#include "driver.h"

CAN_HandleTypeDef hCAN;
static CanTxMsgTypeDef      TxMessage;
static CanRxMsgTypeDef      RxMessage;
static uint8_t              can_device_id = 0x0000;
static uint8_t              recv_buf[DATA_LEN];

static can_interupt_cb_t can_interrupt_cb = NULL;

void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan)
{
    
    GPIO_InitTypeDef GPIO_InitStruct;
    if(hcan->Instance==CANx)
    {
        CANx_CLK_ENABLE();
        CANx_GPIO_CLK_ENABLE();
        
        GPIO_InitStruct.Pin = CANx_RX_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(CANx_GPIO_PORT, &GPIO_InitStruct);
        
        GPIO_InitStruct.Pin = CANx_TX_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(CANx_GPIO_PORT, &GPIO_InitStruct);
        
        CANx_AFIO_REMAP_CLK_ENABLE();
        CANx_AFIO_REMAP_RX_TX_PIN();
        
        HAL_NVIC_SetPriority(CANx_RX_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(CANx_RX_IRQn);
    }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* hcan)
{
    
    if(hcan->Instance==CANx)
    {

        CANx_FORCE_RESET();
        CANx_RELEASE_RESET();
        HAL_GPIO_DeInit(CANx_GPIO_PORT, CANx_TX_PIN|CANx_RX_PIN);
        HAL_NVIC_DisableIRQ(CANx_RX_IRQn);
    }
} 

void can_send_data(uint8_t *data_p, uint8_t data_len, uint8_t frm_cmd)
{						 
    hCAN.pTxMsg->StdId = (can_device_id<<4) + frm_cmd;		//使用的标准ID
    hCAN.pTxMsg->IDE=CAN_ID_STD;					//扩展模式
    hCAN.pTxMsg->RTR=CAN_RTR_DATA;				    //发送的是数据
    hCAN.pTxMsg->DLC=data_len;						//数据长度为2字节
    osel_memcpy(hCAN.pTxMsg->Data, data_p, data_len);
    HAL_CAN_Transmit(&hCAN,TIMEOUT);
}

static void can_int_cb_handle(uint8_t *data_p, uint8_t len)
{
    if (can_interrupt_cb != NULL)
    {
        can_interrupt_cb(data_p, len);
    }
}

void can_int_cb_reg(can_interupt_cb_t cb)
{
    if (cb != NULL)
    {
        can_interrupt_cb = cb;
    }
}

void can_init(uint8_t device_id_cfg, uint8_t host_id)
{
    CAN_FilterConfTypeDef  sFilterConfig;
    can_device_id = device_id_cfg;

    /*CAN单元初始化*/
    hCAN.Instance = CANx;             // CAN外设
    hCAN.pTxMsg = &TxMessage;
    hCAN.pRxMsg = &RxMessage;
    
    hCAN.Init.Prescaler = 4;                                        // BTR-BRP 波特率分频器  定义了时间单元的时间长度 36/(1+6+3)/4=0.9Mbps
    hCAN.Init.Mode = CAN_MODE_NORMAL;                               // 正常工作模式
    hCAN.Init.SJW = CAN_SJW_1TQ;                                    // BTR-SJW 重新同步跳跃宽度 2个时间单元
    hCAN.Init.BS1 = CAN_BS1_10TQ;                                    // BTR-TS1 时间段1 占用了6个时间单元
    hCAN.Init.BS2 = CAN_BS2_7TQ;                                    // BTR-TS1 时间段2 占用了3个时间单元
    hCAN.Init.TTCM = DISABLE;                                       // MCR-TTCM  关闭时间触发通信模式使能
    hCAN.Init.ABOM = ENABLE;                                        // MCR-ABOM  自动离线管理 
    hCAN.Init.AWUM = ENABLE;                                        // MCR-AWUM  使用自动唤醒模式
    hCAN.Init.NART = DISABLE;                                       // MCR-NART  禁止报文自动重传   DISABLE-自动重传
    hCAN.Init.RFLM = DISABLE;                                       // MCR-RFLM  接收FIFO 锁定模式  DISABLE-溢出时新报文会覆盖原有报文 
    hCAN.Init.TXFP = DISABLE;                                       // MCR-TXFP  发送FIFO优先级 DISABLE-优先级取决于报文标示符 
    HAL_CAN_Init(&hCAN);
    
    /*CAN过滤器初始化*/
    sFilterConfig.FilterNumber = 0;                                 //过滤器组0
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;               //工作在标识符屏蔽位模式
    sFilterConfig.FilterScale = CAN_FILTERSCALE_16BIT;              //过滤器位宽为单个16位。
    
    sFilterConfig.FilterIdHigh         = ((uint16_t)host_id)<<9;    //要过滤的ID高位 
    sFilterConfig.FilterIdLow          = ((uint16_t)host_id)<<9;    //要过滤的ID低位 
    sFilterConfig.FilterMaskIdHigh     = 0x007f << 9;                   //过滤器高16位每位必须匹配
    sFilterConfig.FilterMaskIdLow      = 0x007f << 9;;                  //过滤器低16位每位必须匹配
    sFilterConfig.FilterFIFOAssignment = CAN_FIFO0;                     //过滤器被关联到FIFO 0
    sFilterConfig.FilterActivation = ENABLE;                            //使能过滤器
    sFilterConfig.BankNumber = 14;
    HAL_CAN_ConfigFilter(&hCAN, &sFilterConfig);
    HAL_CAN_Receive_IT(&hCAN, CAN_FIFO0); 
}

void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef *hcan)
{
    
    HAL_CAN_Receive_IT(hcan, CAN_FIFO0);
    osel_memcpy(recv_buf, (uint8_t *)&(hcan->pRxMsg->StdId), sizeof(uint16_t));
    osel_memcpy(recv_buf + sizeof(uint16_t), hcan->pRxMsg->Data, hcan->pRxMsg->DLC);
    can_int_cb_handle(recv_buf, hcan->pRxMsg->DLC + 2);
    
}

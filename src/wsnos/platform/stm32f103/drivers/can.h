#ifndef __CAN_H__
#define __CAN_H__


/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/
#define TIMEOUT                         (100u)
#define DATA_LEN                        (10u)
#define CANx                            CAN1
#define CANx_CLK_ENABLE()               __HAL_RCC_CAN1_CLK_ENABLE()
#define CANx_FORCE_RESET()              __HAL_RCC_CAN1_FORCE_RESET()
#define CANx_RELEASE_RESET()            __HAL_RCC_CAN1_RELEASE_RESET()

#define CANx_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOA_CLK_ENABLE()
#define CANx_GPIO_PORT                 GPIOA
#define CANx_RX_PIN                    GPIO_PIN_11
#define CANx_TX_PIN                    GPIO_PIN_12

/* Definition for AFIO Remap */
#define CANx_AFIO_REMAP_CLK_ENABLE()   __HAL_RCC_AFIO_CLK_ENABLE()
#define CANx_AFIO_REMAP_RX_TX_PIN()    __HAL_AFIO_REMAP_CAN1_1()

#define CANx_RX_IRQn                   USB_LP_CAN1_RX0_IRQn

typedef void (*can_interupt_cb_t)(uint8_t *data_p, uint8_t data_len);

void can_init(uint8_t device_id_cfg, uint8_t host_id);
void can_send_data(uint8_t *data_p, uint8_t data_len, uint8_t frm_cmd);
void can_int_cb_reg(can_interupt_cb_t cb);

extern CAN_HandleTypeDef hCAN;
#endif


/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/

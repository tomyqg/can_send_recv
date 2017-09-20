# 平台说明
- core430 
- posix
- education_node
- sen_comm 
- SensingPalletV1.0.0

## 新平台添加
### platform目录添加
在platform目录下创建新平台目录，如果基于之前的MCU，可以从对应MCU的平台直接拷贝源代码到新平台目录，比如SensingPalletV1.0.0是基于STM32L开发，那么可以直接从STM32L的平台拷贝源代码到SensingPalletV1.0.0目录。

### 修改platform.h
在这里添加对应平台的宏，假定是PF_WSPR_SPV1，在platform.h里添加平台头文件：
```
#elif PF_WSPR_SPV1
#include "platform/SensingPalletV1.0.0/drivers/driver.h"
...
```

### 修改Port
如果是已经存在的MCU，在wsnos.h里修改：
```
/* 这里如果是相同MCU，指向的port文件是相同的 */
#if defined(PF_CORE_M3) || defined(PF_WSPR_SPV1) 
#include "sys_arch/ports/stm32l/wsnos_port.h"
```
然后在sys_arch/ports目录找到对应MCU，在wsnos_port.h里面添加：
```
/* 这里是相同MCU映射到不同平台 */
#elif PF_WSPR_SPV1
#include "platform/SensingPalletV1.0.0/libs/cmsis/stm32l1xx.h"
```


# 组件说明
## [设备](#)

## [文件系统](#)

## [抽象层](#)

## [通用库](#)
- **aes** 8bits的字节操作实现的AES加解密库，可以用资源少的单片机. This program uses the AES algorithm implementation (http://www.gladman.me.uk/)
by Brian Gladman.

- **clist** 单链表的一个实现库，从contiki移植过来，可以单向链表的操作.
- **cmac** This program uses the CMAC algorithm implementation
(http://www.cse.chalmers.se/research/group/dcs/masters/contikisec/) by
Lander Casado, Philippas Tsigas.
- **crc** 用静态数组的方式实现crc8的表查找计算
- **data_type_def** 开发环境的宏定义，数据类型定义
- **debug** 通用调试库，在系统添加调试等级，打印调试日志
- **id_rules** 设备ID生成规则库，定义设备产品、类型、功能
- **list** 双向链表的一个实现库，从linux移植过来，包含了双向链表的基本以及复杂操作.
- **mpool** 动态内存池库，实现了一个基本的内存池管理，用户可以动态申请、释放内存.
- **portablelonglong** long-long支持，一些C编译器不支持64bits整形，用这个库可以在所有单片机上实现支持.
- **printf** 一个开源printf库，实现了printf的'd' 'u' 'c' 's' 'x' 'X'.
- **serial** 通用串口组件，通过组合协议的头、长度、载荷、尾几种信息，支持不同协议的自动化解析工作.
- **sqqueue** 循环队列库，该队列有九个操作，分别为单元素入队列、多元素入队列、出队列，单元素撤销入队列(队尾删除)、取队列长度、判空、清空队列、遍历和删除指定位置.
- **srandom** 随机数生成库，可以基与时间、设备唯一ID生成伪随机数.

## [白盒测试](#)
unity测试框架，是一个简单且直接的自动化单元测试框架，全部用C实现的自动化测试框架.

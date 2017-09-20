## 1. 使用说明
使用wsnos开发只需要在应用文件里添加`gz_sdk.h`头文件即可。

### 1.1 [OS API](user_guide.md)

### 1.2 [通用组件](common/)

### 1.3 [白盒测试](test/)

## 2. 修改日志
-------------
2017-05-27
- modify: 移植LoRaWAN-V4.3.2，替代V4.3.0

2017-05-16
- modify: mpool增加多内存池功能

2017-03-24
- fix: 修复SSN网络下行配置成功率低问题

2017-03-07
- add: 新增白盒测试用例以及编写使用说明
- fix: 修复mpool在64位机器上指针被强制转换成32位的bug

2017-02-27
- add: mpool内存池
- add: common组件简单说明

2017-02-15, V2.0.1
- add: 添加LoRaWAN协议栈

2015-12-18, V2.0.0
- add: 新增contiki的pthread库，增加阻塞式线程方式
- modify:
- fix:



[TOC]

### 1. aes(加密)
<a href="http://baike.baidu.com/link?url=WnbPeKCwba_BGRIBls-MNxyghGRrAI3Ubu4bntdlnjqHKzOjEl5a3-w-E-XF0Mi-A8dEaFpC31ofGw-GOaC_ea" target="_blank">百度百科</a>

#### 1.1 说明

- 对不同长度的密钥，AES采用不同的加密轮次：

|  128位        | 192位        | 256位      |
| ------------- | -----------: | :--------: |
| 10            | 12           | 14         |

- 密钥扩展：简单说就是将原来的密钥扩展到足够用的长度：

    - 128位密钥： 扩展到 11x4x4
    - 192位密钥： 扩展到 13x4x4
    - 256位密钥： 扩展到 15x4x4

- AES加密过程是在一个4×4的字节矩阵上运作,所以一次加解密传入的最小块单位为16字节

#### 1.2 代码流程

```flow
st=>start: 加密开始
e=>end: 加密结束
op1=>operation: 加密的数据[简称密文]、密钥（16个字节）、输入数据块和输出数据块
op2=>operation: 调用接口"aes_set_key",完成密钥的扩展
op3=>operation: 调用接口"aes_encrypt",完成数据的加密
st->op1->op2->op3->e
```
```flow
st=>start: 解密开始
e=>end: 解密结束
op1=>operation: 解密的数据[简称密文]、密钥（16个字节）、输入数据块和输出数据块
op2=>operation: 调用接口"aes_set_key",完成密钥的扩展
op3=>operation: 调用接口"aes_encrypt",完成数据的解密
st->op1->op2->op3->e
```
#### 1.3 demo
- demo中会使用aes中的接口完成简单的加密和解密数据
- **将aes.h中的AES_ENC_PREKEYED和AES_DEC_PREKEYED置1**
- 详细使用请见<a href="../../apps/simple_aes.c" target="_blank">simple_aes.c</a>

### 2. cmac(类CRC)

#### 2.1 说明

- cmac是一种数据传输检错功能，以保证数据传输的正确性和完整性
- cmac基本原理是：通过对需要校验的数据奇偶校验、位移、AES加密获取一段16个字节大小的数组
- **lorawan使用cmac模块给MAC数据和加密数据做校验的优点：效率很高、安全性很高（在校验数据之前调用"AES_CMAC_Update"二次，增加了异变因子）**
- **困惑：为什么不使用CRC32**
- demo中只对需要校验的数据使用"AES_CMAC_Update"一次，无法确定lorawan增加一次"AES_CMAC_Update"的效果如何

#### 2.2 代码流程

```flow
st=>start: 校验开始
e=>end: 校验结束
op1=>operation: 需要校验的数据、密钥、密钥扩展表
op2=>operation: 调用接口"AES_CMAC_Init",完成密钥扩展表的初始化
op3=>operation: 调用接口"AES_CMAC_SetKey",完成密钥扩展表数据
op4=>operation: 调用接口"AES_CMAC_Update",完成数据的奇偶校验
op5=>operation: 调用接口"AES_CMAC_Final",生成16个字节的校验表
op6=>operation: 取表4个字节作为校验码
st->op1->op2->op3->op4->op5->op6->e
```

#### 2.3 demo
- 详细使用请见<a href="../../apps/simple_cmac.c" target="_blank">simple_cmac.c</a>

### 3. clist(简单链表)

#### 3.1 说明

- clist是list的简化版，下面给出常用的接口说明
- 接口说明：
    - define CLIST(name) 
    申明一个链表

    - void  clist_init(clist_t clist)       
    初始化一个链表

    - int    clist_length(clist_t clist)       
    获取链表中对象个数
        
    - void   clist_add_head(clist_t clist, void *item)
    把一个对象插入到链表头

    - void   clist_add_tail(clist_t clist, void *item)
    把一个对象插入到链表尾

    - void * clist_remove_head (clist_t clist)
    从链表头移除一个对象，并返回对象指针

    - void * clist_remove_tail(clist_t clist)
    从链表尾移除一个对象，并返回对象指针

    - void   clist_remove(clist_t clist, void *item);
    从链表中删除一个对象

    - define CLIST_FOR_EACH(clist,pos) 
    遍历链表中的对象

- **定义结构体对象时在首行添加 <a>struct clist *next</a> 来指向下一个对象**

#### 3.2 demo
- 详细使用请见<a href="../../apps/simple_clist.c" target="_blank">simple_clist.c</a>

```code
CLIST(my_list);     //放在文件的最开始，不在函数内申明

void simple_clist(void)
{
    node_t * node = NULL;
    // 往链表尾添加10个数据
    for (int i = 0; i < num; i++)
    {
        node = (node_t* )malloc(sizeof(node_t));  //可以使用mpool_alloc中来申请内存，这里为了方便使用系统函数
        node->index = i;
        clist_add_tail(my_list, node);
    }

    node = clist_remove_head(my_list);   // 从链表头移除一个
    free(node);     //取出后用完要记得释放

    node = clist_remove_tail(my_list);      //从链表尾移除一个对象
    free(node);

    int num = clist_length(my_list); //获取链表中剩余个数
}

```
### 4. sqqueue(队列)

#### 4.1 说明
- sqqueue 是一个成员变量固定长度的队列
- 使用的时候先创建一个"sqqueue_ctrl_t"类型的队列对象，调用初始化函数后就可以使用对象中的功能了

#### 4.2 demo
- 详细使用请见<a href="../../apps/simple_sqqueue.c" target="_blank">simple_sqqueue.c</a>

```code
typedef struct
{
    uint8_t x;
    uint8_t y;
} element_t;

sqqueue_ctrl_t queue;   //创建队列对象

void traverse_cb(const void *e)
{
    element_t *p = (element_t* )e;
}

void simple_sqqueue(void)
{
    int size = 10;
    //初始化队列
    if (FALSE == sqqueue_ctrl_init(&queue, sizeof(element_t), size))    //队列长度要比实际创建的长度-1
    {
        return;     //创建失败
    }

    // 添加测试元素
    for (int i = 1; i <= 10; i++)
    {
        element_t element;
        element.x = i * 10;
        element.y = i * 10;
        queue.enter(&queue, &element);  //将成员插入到队列中
    }

    queue.del(&queue);      //移除首元素
    queue.revoke(&queue);       //移除尾元素
    queue.remove(&queue, 3);        //删除相对队头指定偏移位置的元素
    queue.traverse(&queue, traverse_cb);         //遍历队列
    int num = queue.sqq.sqq_len - 1 - queue.get_len(&queue);    //队列剩余空间
    queue.clear_sqq(&queue);    //清空队列
}
```

### 5. serial(串口)

#### 5.1 说明
- serial具有识别数据帧起始和结束的功能，只需要定义好识别符以及长度，该模块会将完整的数据帧存储到数据队列中
- 使用起来也非常的方便，只要定义好识别符以及长度，就可以调用发送以及接收

#### 5.2 demo
- 详细使用请见<a href="../../apps/simple_serial.c" target="_blank">simple_serial.c</a>

```code
static void uart_recv_cb(void)
{
    //串口对象收到一条完整的数据帧后调用回调函数进入
    //发送消息给线程app_event_process
}

PROCESS(app_event_process, "app_event_process");
PROCESS_THREAD(app_event_process, ev, data)
{
    PROCESS_BEGIN();
    while (1)
    {
        if (ev == APP_UART_EVENT)
        {
            //根据定义好的串口协议解析数据
        }
        PROCESS_YIELD();
    }
    PROCESS_END();
}

static void serial_init(void)
{
    //配置串口协议对象
    serial_reg_t app_serial_reg;
    ***省略***

    serial_fsm_init(APP_UART);      //串口驱动初始化
    serial_reg(APP_UART, app_serial_reg);   //注册绑定
}

void simple_serial(void)
{
    serial_init();      //初始化，注册串口对象

    app_task = osel_task_create(....);  //建立一个任务
    osel_pthread_create(app_task, &app_event_process, NULL);    //接收数据使用消息响应,使用线程的方式

    uint8_t send_buf[] = "hello\r\n";
    serial_write(APP_UART, send_buf, 7);        //发送数据
}
```

### 6. printf(格式化输出)

#### 6.1 说明
- 在需要使用printf和sprintf系统函数时可以使用该模块中'tfp_printf'和'tfp_sprintf'
- 要使用printf，您需要提供自己的字符输出功能

#### 6.2 demo
- 详细使用请见<a href="../../apps/simple_printf.c" target="_blank">simple_printf.c</a>

```code
static void _putc ( void* p, char c)
{
    uint8_t *t = &c;
    serial_write(APP_UART, t, 1);   //串口打印结果
}

void simple_printf(void)
{
    serial_fsm_init(APP_UART);
    wsnos_init_printf(NULL, _putc);

    uint8_t buf[100];
    uint8_t buf1[10] = "hello";
    uint8_t buf2[10] = "world";
    wsnos_printf("%s\r\n", buf1);           //输出：hello

    wsnos_sprintf(buf, "%s %s", buf1, buf2);
    wsnos_printf("%s\r\n", buf);                //输出：hello world
}
```

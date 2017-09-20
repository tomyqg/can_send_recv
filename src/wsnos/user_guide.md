# WSNOS操作系统使用说明

---

**在开始之前请注意，文档里面出现的代码，很多都是伪代码，并不能直接用于运行测试，需要添加相应的依赖代码，但接口使用方法不变。**

## 1.OSEL环境初始化
```code
/** 
 * @brief 该接口实现了os的环境初始化，定义了系统堆空间，系统支持的最大优先级数；
 * @param[in] buf 系统堆空间的起始地址
 * @param[in] size 系统堆空间的大小
 * @param[in] max_prio 系统支持的最大优先级数
 * @code
 * static uint8_t osel_heap_buf[4096];
 * ...
 * int main(void)
 * {
 *     osel_env_init(osel_heap_buf, 4096, 8);
 *     ...
 *     osel_run();
 * } 
 * @endcode
 */
void osel_env_init(osel_uint8_t *buf, 
                   osel_uint16_t size,
                   osel_uint8_t max_prio);
```

## 2.任务与线程的区别
在wsnos早期版本里面，只有任务的概念，任务与任务之间是有优先级的，以事件触发的方式驱动；遵循的是处理时间短原则，没法实现阻塞机制；在最新的V2.0版本里面增加了contiki的protothread库，通过线程来实现阻塞，把线程封装进任务里面，实现了wsnos的任务阻塞。
![task与thread比较](http://img.blog.csdn.net/20151126103322284)

### 2.1 任务创建
首先我们看一下任务创建的接口。
```code
/**
 * @brief 创建一个任务，该版本里面，任务不进行实际任务操作，只是作为线程的载体，为线程调度的主体。
 * @param[in] entry 任务执行的函数指针,如果为空则不执行
 * @param[in] prio  任务的优先级
 * @param[in] event_sto[] 任务的消息队列实际存储空间的起始地址
 * @param[in] event_len 任务的消息队列实际存储空间的长度
 *
 * @return 指向任务控制块内存的地址
 * @code
 * static uint8_t osel_heap_buf[4096];
 * static osel_task_t *task_tcb = NULL;
 * static osel_event_t task_event_store[10];
 *
 * void task_tcb_entry(void *param)
 * {
 *     //*< 这里可以添加任务每次进入的日志打印信息，用于调试
 *     ...
 * }
 * 
 * int main(void)
 * {
 *     osel_env_init(osel_heap_buf, 4096, 8);
 *     //*< 这里创建了任务，但没有相关事件触发，任务的主体并不会得到执行
 *     task_tcb = osel_task_create(&task_tcb_entry, 6, task_event_store, 10);
 *
 *     osel_run();
 *     return 0;
 * }
 * @endcode
 */
osel_task_t *osel_task_create(void (*entry)(void *param),
                              osel_uint8_t prio,
                              osel_event_t event_sto[],
                              osel_uint8_t event_len);
```
任务的创建还是比较简单的，如果采用了新的线程库特性，那么可以在创建任务的时候把任务的入口参数置空。完成创建以后，任务并不会马上得到执行，需要由事件的触发来启动（如果你指定了事件）。

> 注：原先的wsnos事件与任务之间是由绑定机制来完成的，因为增加了线程库的支持，兼容性不好，所以放弃了。后面我们会详细介绍在线程里面如何编写事件处理流程。

### 2.2 线程创建
线程的运行是动态加载的，当我们定义申明了一个线程以后，通过线程创建接口把线程动态加载到任务控制块里面。
```code
/**
 * @brief 创建线程，把线程动态加载到任务控制体内
 * @param[in] tcb 指向任务控制块的指针
 * @param[in] p 指向线程控制块的指针
 * @param[in] data 线程初始化创建以后调用的参数
 *
 * @return 创建线程成功还是失败
 *
 * @code
 * static uint8_t osel_heap_buf[4096];
 * static osel_task_t *task_tcb = NULL;
 * static osel_event_t task_event_store[10];
 *
 * PROCESS_NAME(task_thread1_process);
 * PROCESS_NAME(task_thread2_process);
 *
 * PROCESS(task_thread1_process, "task thread 1 process");
 * PROCESS_THREAD(task_thread1_process, ev, data)
 * {
 *     PROCESS_BEGIN();
 *
 *     while(1)
 *     {
 *         if(ev == PROCESS_EVENT_INIT) //*< 系统自定义事件
 *         {
 *             ...
 *         }
 *         else if(ev == XXX_EVENT)     //*< 用户自定义事件
 *         {
 *             ...
 *         }
 *
 *         PROCESS_YIELD();       //*< 释放线程控制权，进行任务切换
 *     }
 *
 *     PROCESS_END();
 * }
 *
 * PROCESS(task_thread2_process, "task thread 2 process");
 * PROCESS_THREAD(task_thread2_process, ev, data)
 * {
 *     PROCESS_BEGIN();
 *
 *     while(1)
 *     {
 *         if(ev == PROCESS_EVENT_INIT) //*< 系统自定义事件
 *         {
 *             ...
 *         }
 *
 *         PROCESS_YIELD();       //*< 释放线程控制权，进行任务切换
 *     }
 *
 *     PROCESS_END();
 * }
 * 
 * int main(void)
 * {
 *     osel_env_init(osel_heap_buf, 4096, 8);
 *     task_tcb = osel_task_create(NULL, 6, task_event_store, 10);
 *
 *     osel_pthread_create(task_tcb, &task_thread1_process, NULL);
 *     osel_pthread_create(task_tcb, &task_thread2_process, NULL);
 *
 *     osel_run();
 *     return 0;
 * }
 *
 * @endcode
 */
osel_bool_t osel_pthread_create(osel_task_t *tcb,  osel_pthread_t *p, osel_param_t data);
```
这里我们创建了2个线程，注意他们都是绑定到了同一个任务里，那么这2个线程的优先级是相同的，那么如果在这2个线程之间传递消息，线程是顺序执行的，不存在抢占的问题。

> 注：线程在创建的过程中，会响应线程初始化事件，响应完以后进入等待态，等待下一次事件触发。

### 2.3 线程退出
在上面一节中，我们介绍了线程的创建，同时知道线程是动态加载的，那么相应的就可以动态卸载。
```code

/**
 * @brief 退出线程，把线程动态从任务的队列里面动态卸载
 * @param[in] tcb 指向任务控制块的指针
 * @param[in] p 指向线程控制块的指针
 * @param[in] fromprocess 对应执行了退出p线程的原始线程指针
 *
 * @return 创建线程成功还是失败
 *
 * @code
 * static uint8_t osel_heap_buf[4096];
 * static osel_task_t *task_tcb = NULL;
 * static osel_event_t task_event_store[10];
 *
 * PROCESS_NAME(task_thread1_process);
 * PROCESS_NAME(task_thread2_process);
 *
 * PROCESS(task_thread1_process, "task thread 1 process");
 * PROCESS_THREAD(task_thread1_process, ev, data)
 * {
 *     PROCESS_BEGIN();
 *
 *     while(1)
 *     {
 *         if(ev == PROCESS_EVENT_INIT) //*< 系统自定义事件
 *         {
 *             ...
 *         }
 *
 *         PROCESS_YIELD();     //*< 释放线程控制权，进行任务切换
 *     }
 *
 *     DBG_LOG(DBG_LEVEL_INFO, "task thread1 process exit\r\n");
 *
 *     PROCESS_END();
 * }
 *
 * PROCESS(task_thread2_process, "task thread 2 process");
 * PROCESS_THREAD(task_thread2_process, ev, data)
 * {
 *     PROCESS_BEGIN();
 *
 *     while(1)
 *     {
 *         if(ev == PROCESS_EVENT_INIT) //*< 系统自定义事件
 *         {
 *             //*< 从当前线程2里面把线程1从任务里面退出
 *             osel_pthread_exit(task_tcb, &task_thread1_process, PROCESS_CURRENT());
 *         }
 *           
 *         ...
 *
 *         PROCESS_YIELD();       //*< 释放线程控制权，进行任务切换
 *     }
 *
 *     PROCESS_END();
 * }
 * 
 * int main(void)
 * {
 *     osel_env_init(osel_heap_buf, 4096, 8);
 *     debug_init(DBG_LEVEL_INFO);
 *     task_tcb = osel_task_create(NULL, 6, task_event_store, 10);
 *     osel_pthread_create(task_tcb, &task_thread1_process, NULL);
 *
 *     //*< 创建完线程2以后，退出线程1
 *     osel_pthread_create(task_tcb, &task_thread2_process, NULL);
 *
 *     osel_run();
 *     return 0;
 * }
 *
 * @endcode
 */
osel_bool_t osel_pthread_exit(osel_task_t *tcb, osel_pthread_t *p, osel_pthread_t *fromprocess);
```

### 2.3 线程等待
在wsnos_pthread.h文件里面，我们看到下面一些宏：
```code
//*< 大家可以补全这部分注释
#define PROCESS_BEGIN()             PT_BEGIN(process_pt)

#define PROCESS_END()               PT_END(process_pt)

#define PROCESS_WAIT_EVENT()        PROCESS_YIELD()

#define PROCESS_WAIT_EVENT_UNTIL(c) PROCESS_YIELD_UNTIL(c)

#define PROCESS_YIELD()             PT_YIELD(process_pt)

#define PROCESS_YIELD_UNTIL(c)      PT_YIELD_UNTIL(process_pt, c)

#define PROCESS_WAIT_UNTIL(c)       PT_WAIT_UNTIL(process_pt, c)
#define PROCESS_WAIT_WHILE(c)       PT_WAIT_WHILE(process_pt, c)

#define PROCESS_EXIT()              PT_EXIT(process_pt)

#define PROCESS_PT_SPAWN(pt, thread)   PT_SPAWN(process_pt, pt, thread)

#define PROCESS_PAUSE()             do {                            \
    osel_event_t event;                                             \
    event.sig = PROCESS_EVENT_CONTINUE;                             \
    event.param = NULL;                                             \
    osel_post(NULL, PROCESS_CURRENT(), &event);                     \
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);         \
} while(0)

#define PROCESS_POLLHANDLER(handler) if(ev == PROCESS_EVENT_POLL) { handler; }

#define PROCESS_EXITHANDLER(handler) if(ev == PROCESS_EVENT_EXIT) { handler; }
```
这里宏为我们提供了线程等待的机制。我们看一个例子：
```code
PROCESS_THREAD(app2nwk_process, ev, data)
{
    PROCESS_BEGIN();
    static uint64_t key = 0xaabbull;
    while(1)
    {
        ...
        nwk_route_query_discovery(key);     //*< 发送一个指令，等待响应
    
        osel_etimer_ctor(&(sbuf->etimer), PROCESS_CURRENT(),
                         PROCESS_EVENT_TIMER, NULL);
        //*< ONE shot for 1000*10*hops*2 ms
        osel_etimer_arm(&(sbuf->etimer), 1000 * mac_pib_hops_get() * 2, 0);
        //*< 等待定时器超时到达或者响应到达
        PROCESS_WAIT_EVENT_UNTIL((ev == PROCESS_EVENT_TIMER) ||
                                 (ev == NWK_QUERY_RESP_EVENT));
        if (ev == PROCESS_EVENT_TIMER)  //*< 定时器超时
        {
            nwk_query_timeout_event(sbuf);
        }
        else if (ev == NWK_QUERY_RESP_EVENT)    //*< 响应到达
        {
           ...
        }
    }
    
    PROCESS_END();
}
```
上面的例子里面，我们可以很清楚的看到，通过PROCESS_WAIT_EVENT_UNTIL()宏，我们实现了线程的等待，并且针对多个事件进行了处理。


## 3.消息机制
经过上面一整节的介绍，我想我们已经对于任务和线程有了基本的概念，是不是想亲手测试一番？但我们还缺少一个关键的接口，**消息发送**：实现任务与任务、线程与线程、任务与线程之间的消息发送。
首先我们看一下消息发送的接口：
```code
/**
 * @brief 消息发送，把事件块发送到某个任务的某个线程，如果任务非空，则发送到任务所在的所有线程，如果任务为空，线程不为空，则消息发送给指定的线程.
 * @param[in] task 指定的任务控制块指针
 * @param[in] p 指定的线程控制块指针
 * @param[in] event 指向任务控制块的指针
 *
 * @return 
 */
void osel_post(osel_task_t *task, osel_pthread_t *p, osel_event_t *event);
```
### 3.1 发送消息给指定任务（包括任务下面所有的线程）
当我们从线程B-1发送一个消息给任务A的时候，如果没有指定目的线程，那么该消息会被任务A的线程队列里面所有的线程响应。
```code
#define TASK_A_LED_EVENT            (0x3000)

static uint8_t osel_heap_buf[4096];
static osel_task_t *task_a_tcb = NULL;
static osel_event_t task_a_event_store[10];
static osel_task_t *task_b_tcb = NULL;
static osel_event_t task_b_event_store[10];

PROCESS_NAME(task_a_thread1_process);
PROCESS_NAME(task_a_thread2_process);
PROCESS_NAME(task_b_thread1_process);

PROCESS(task_a_thread1_process, "task a thread 1 process");
PROCESS_THREAD(task_a_thread1_process, ev, data)
{
    PROCESS_BEGIN();
    while (1)
    {
        if (ev == TASK_A_LED_EVENT)
        {
            //*< 打印事件处理
            DBG_LOG(DBG_LEVEL_INFO, "task a thread 1 led handle\r\n");
        }
        PROCESS_YIELD();     //*< 释放线程控制权，进行任务切换
    }
    PROCESS_END();
}

PROCESS(task_a_thread2_process, "task a thread 2 process");
PROCESS_THREAD(task_a_thread2_process, ev, data)
{
    PROCESS_BEGIN();
    while (1)
    {
        if (ev == TASK_A_LED_EVENT)
        {
            //*< 打印事件处理
            DBG_LOG(DBG_LEVEL_INFO, "task a thread 2 led handle\r\n");
        }
        PROCESS_YIELD();       //*< 释放线程控制权，进行任务切换
    }
    PROCESS_END();
}

PROCESS(task_b_thread1_process, "task b thread 1 process");
PROCESS_THREAD(task_b_thread1_process, ev, data)
{
    PROCESS_BEGIN();
    while (1)
    {
        if (ev == PROCESS_EVENT_INIT) //*< 系统自定义事件
        {
            osel_event_t event;
            event.sig   = TASK_A_LED_EVENT;
            event.param = NULL;
            osel_post(task_a_tcb, NULL, &event);    //*< 把消息发送给任务A
        }
        PROCESS_YIELD();       //*< 释放线程控制权，进行任务切换
    }
    PROCESS_END();
}

int main(void)
{
    osel_env_init(osel_heap_buf, 4096, 8);
    debug_init(DBG_LEVEL_INFO);
    task_a_tcb = osel_task_create(NULL, 6, task_a_event_store, 10);
    osel_pthread_create(task_a_tcb, &task_a_thread1_process, NULL);
    osel_pthread_create(task_a_tcb, &task_a_thread2_process, NULL);
    task_b_tcb = osel_task_create(NULL, 7, task_b_event_store, 10);
    osel_pthread_create(task_b_tcb, &task_b_thread1_process, NULL);
    osel_run();
    return 0;
}
```
如果运行上面的代码，那么我们会发现任务A的线程1与线程2都对**TASK_A_LED_EVENT**事件进行了响应。


### 3.2 发送消息给指定线程
如果我们想把消息精确发送某个线程，那么我们只需要修改一下发送的接口，指定消息发送到的线程。那么上面的代码我们可以修改为：
```code
...
//*< 明确把消息发送给任务A的线程2
osel_post(task_a_tcb, &task_a_thread2_process, &event); //*< 把消息发送给线程2
...
```
如果我们不知道线程2属于任务A的，那么我们也可以用下面的代码：
```code
...
//*< 把消息发送给线程2，系统会帮我们自动发送到任务A
osel_post(NULL, &task_a_thread2_process, &event);   //*< 把消息发送给线程2
...
```
到了这里，我们介绍完了线程与任务、线程与线程、任务与任务之间的通信。如果是非阻塞式的编程，那么我们到这里也就结束了，合理利用上面的接口已经能够让我们顺畅的开发应用程序了。
但是不要忘记加入protothread库的初衷，阻塞的编程方式。那么我们继续往下看。

## 4. 事件定时器
这里我们又需要介绍一个强大的工具。事件定时器可以把2个异步操作连接起来。
比如下面的应用场景：

- 线程1执行完以后，需要延时5s以后执行线程2. 
- 线程1执行某个操作以后，需要等待3s继续执行.
- 线程1发送某条执行以后，在10s以内等待响应，如果10s内收到响应就执行流程1，否则执行流程2.

上面这些应用场景，如果我们没有一个定时器机制，那么我们就没法把这些单一的操作联合起来。

### 4.1 定时器初始化
```code
/**
 * @brief 初始化定时器
 * @param me 指向定时器的指针
 * @param p  定时器绑定的线程指针
 * @param sig  定时器响应以后发送的消息
 * @param param 定时器响应以后发送的参数
 */
void osel_etimer_ctor(osel_etimer_t *const me, 
                      osel_pthread_t *p,
                      osel_signal_t sig,
                      osel_param_t param);
```

### 4.2 定时器启动
```code
/**
 * @brief 启动某一个定时器
 * @param me 指向定时器的指针
 * @param ticks 定时器在多少个tick以后启动（每个tick由硬件设定）
 * @param interval 指定定时器触发以后下次启动的间隔时间
 *  - 0 定时器只触发一次，启动以后就释放
 *  - !0 定时器周期性启动，在第一次触发以后，以该值作为下次启动的周期
 */
void osel_etimer_arm(osel_etimer_t *const me,
                     etimer_ctr const ticks,
                     etimer_ctr const interval);
```

### 4.3 定时器取消
```code
/**
 * @brief 取消定时器执行
 * @param  me 指向要取消的定时器指针
 * @return 定时器是否取消成功
 *  - TRUE 定时器被完全取消
 *  - FALSE 定时器已经执行过了，取消失败
 */
osel_bool_t osel_etimer_disarm(osel_etimer_t *const me);
```

> 注：这里我们需要关注定时器取消接口的返回值，因为定时器的取消是一个异步操作，有可能取消的时候定时器中断已经触发了。

### 4.4 定时器重新启动
```code
/**
 * @brief 定时器重新设定，如果定时器之前是一次有效的，重新定时以后还是一次有效；
 * 多次有效的还是多次有效；
 * @param  me 指向要取消的定时器指针
 * @param  ticks 定时器在多少个tick以后启动（每个tick由硬件设定）
 * @return  定时器是否已经被触发过
 *  - TRUE 定时器没有被触发，且重新定时成功
 *  - FALSE 定时器已经被触发，且重新定时成功
 */
osel_bool_t osel_etimer_rearm(osel_etimer_t *const me, etimer_ctr const ticks);
```

> 注：这里我们同样需要关注定时器重新启动的返回值。

### 4.5 实际使用
上面4小节介绍了事件定时器的接口，下面我们来实际操作一番。
```code
/**
* 有一个线程1，周期性1s翻转LED，当运行1分钟以后，关闭翻转LED的操作。当运行2分钟以后，再次启动翻转LED。
*/
#define TASK_A_LED_EVENT        (0x3000)
#define TASK_A_STOP_EVENT       (0x3001)
#define TASK_A_START_EVENT      (0x3002)

static uint8_t osel_heap_buf[4096];
static osel_task_t *task_a_tcb = NULL;
static osel_event_t task_a_event_store[10];

static osel_etimer_t cycle_etimer;   //*< 该定时器用于周期性翻转LED
static osel_etimer_t stop_etimer;    //*< 该定时器用于超时停止
static osel_etimer_t start_etimer;   //*< 该定时器用于超时重启

PROCESS_NAME(task_a_thread1_process);

PROCESS(task_a_thread1_process, "task a thread 1 process");
PROCESS_THREAD(task_a_thread1_process, ev, data)
{
    PROCESS_BEGIN();
    
    static bool_t stop_flag = FALSE;    //*< 这里为什么用的静态变量？
    
    while(1)
    {
        if(ev == TASK_A_LED_EVENT)
        {
            if(stop_flag == FALSE)
            {
                hal_led_toggle(HAL_LED_GREEN);
            }
        }
        else if(ev == TASK_A_STOP_EVENT)
        {
            if(osel_etimer_disarm(&cycle_etimer) == FALSE)//*< 定时器已经被触发
            {
                stop_flag = TRUE;   //*< 这里考虑用一个变量作为标志位，是否合适，为什么？
            }
        }
        else if(ev == TASK_A_START_EVENT)
        {
            osel_etimer_rearm(&cycle_etimer, 100);
        }
        
        PROCESS_YIELD();     //*< 释放线程控制权，进行任务切换
    }
    
    PROCESS_END();
}


int main(void)
{
    osel_env_init(osel_heap_buf, 4096, 8);

    task_a_tcb = osel_task_create(NULL, 6, task_a_event_store, 10);
    osel_pthread_create(task_a_tcb, &task_a_thread1_process, NULL);
    
    osel_etimer_ctor(&cycle_etimer, &task_a_thread1_process, TASK_A_LED_EVENT, NULL);
    osel_etimer_ctor(&stop_etimer, &task_a_thread1_process, TASK_A_STOP_EVENT, NULL);
    osel_etimer_ctor(&start_etimer, &task_a_thread1_process, TASK_A_START_EVENT, NULL);
    
    osel_etimer_arm(&cycle_etimer, 100, 100);   //*< 周期性触发
    osel_etimer_arm(&stop_etimer, 6000, 0);     //*< 一次性触发
    osel_etimer_arm(&start_etimer, 12000, 0);    //*< 一次性触发
    
    osel_run();
    return 0;
}
```
运行上面的例子，我们就会发现启动以后线程1周期性1s翻转LED，当运行1分钟以后，关闭翻转LED的操作。当运行2分钟以后，再次启动翻转LED。完全符合我们的预期。

### 4.6 阻塞
到这里，很多人要说了，根本没有看到我想要的阻塞接口。其实上面都已经实现了。我们要做的只是封装一下接口。
```code
/**
 * @brief 该宏只能与pthread库一起使用
 */
#define OSEL_ETIMER_DELAY(et, time)                                 \
    osel_etimer_ctor(et,                                            \
                     PROCESS_CURRENT(),                             \
                     PROCESS_EVENT_TIMER,                           \
                     NULL);                                         \
    osel_etimer_arm(et, time, 0);                                   \
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
```
上面的宏，利用定时器和pthread库，实现了线程的阻塞机制。我们可以看个例子：
```code
...
PROCESS(cycle_process, "cycle_process");
PROCESS_THREAD(cycle_process, ev, data)
{
    PROCESS_BEGIN();

    while (1)
    {
        hal_led_toggle(HAL_LED_GREEN);
        OSEL_ETIMER_DELAY(&cycle_etimer, 100);
    }

    PROCESS_END();
}
...
```
上面的编码风格是不是很熟悉的阻塞式编程方式？运行上面的程序，你会发现led灯周期1秒进行翻转。

## 5.资源保护
嵌入式实时系统存在一种潜在的风险。当一个任务在使用某一个资源的过程中（还没有完全结束对资源的访问），被切换出去，使得资源处于不完整的状态，这个时候如果有另外一个任务或中断来访问这个资源，则会导致资源损坏或其他错误。考虑以下的情况：
### 5.1 异常情况
#### 5.1.1 访问外设
有2个任务试图都往串口发送数据：

- 任务A运行，往串口发送字符串“Hello World”。
- 任务A被任务B抢占，但此时串口才输出到“Hello W”。
- 任务B往串口输入“This is B”，然后让出控制权。
- 任务A从被打断的地方继续执行，完成剩余的字符输出“orld”。
- 最终串口输出的字符是“Hello WThis is Borld”。


#### 5.1.2 读-改-写操作
下面是一个简单的C代码，我们在16位单片机MSP430里面编译运行。
```code
//*< C code
static uint32_t sem_cnt = 0;
sem_cnt += 0xAABBCCDD;

//*< Asm code
01C13A 1840 50B2 CCDD   addx.w #0xCCDD, &sem_cnt ; 把32位整形的低16位写入变量对应内存的地地址
       282A                                      ; 地址对齐操作
01C142 1840 60B2 AABB   addcx.w #0xAABB, &0x282C ; 把32位整形的高16位写入变量对应内存的高地址
```

这是一个“非原子”操作，因为整个操作需要不止一条指令，所有操作过程可能被中断。考虑一下这样的情况（两个任务都试图更新一个名为sem_cnt的变量）：

- 任务A正在写入sen_cnt的值
- 任务A在完成写操作之前，被任务B抢占。
- 任务B完整的执行了对sem_cnt的更新流程，让出CPU的控制权。
- 任务A从被抢占的地方继续执行，他完成剩余的修改sem_cnt操作，但是这其实只是是在B修改的基础上又进行了修改。

任务A更新并回写了一个过期的值，在任务A执行完操作之前，任务B又修改了sem_cnt的值，之后任务A完成剩余操作，覆盖了任务B对sem_cnt的修改结果，效果上完全破坏了sem_cnt的值。与我们的预期结果完全不符。
上面完全符合读-改-写的操作，实际的**外围设备、寄存器**也是一样的情况。

#### 5.1.3 函数重入
如果一个函数可以安全地被多个任务调用，或是在任务与中断中均可被调用，则整个函数是可重入的。下面展示2个函数：
```code
uint32_t func1(uint32_t value)
{
    uint32_t var = 0;
    var = value + 100;
    return var;
}

uint32_t fun2(uint32_t value)
{
    static uint32_t var = 0;
    var = value + 100;
    return var;
}
```
上面的2个函数，函数1是可重入的函数，函数2是不可重入的函数。

### 5.2 解决方案
#### 5.2.1 临界区保护
临界区指的是宏OSEL_ENTER_CRITICAL()与OSEL_EXIT_CRITICAL()之间的代码区间，下面是一个代码示例：
```code
/**
* 保证全局变量的访问不被中断，将操作放入临界区
*/
osel_int_status_t s;
OSEL_ENTER_CRITICAL(s);

/**
 * 在临界区内不会切换到其他任务，中断不能被执行，无法嵌套，
 * 所以在这个里面的操作要保证时间劲量短
 */
sem_cnt += 0xAABBCCDD;

/**
 * 完成资源的访问以后，安全退出临界区
 */
OSEL_EXIT_CRITICAL(s);
```
临界区提供了一种非常原始的互斥功能。仅仅是简单的把中断关闭。抢占式的上下文切换只能在退出临界区以后才得到执行，所以临界区必须具有很短的时间，否则会反过来影响中断响应时间。
临界区嵌套是安全的，因为宏OSEL_ENTER_CRITICAL()会维护当前中断的状态，只有在真正退出临界区的时候，宏OSEL_EXIT_CRITICAL()才使能中断，系统才能再次响应中断。所以宏OSEL_ENTER_CRITICAL()和OSEL_EXIT_CRITICAL()必须成对出现。

#### 5.2.2 互斥锁
wsnos提供了一种简单的互斥锁机制，通过挂起调度器，使得代码区间不被其他的任务、线程抢占，但仍然能够响应中断。
如果一个关键代码段时间太长不适合简单的临界区保护，我们可以考虑采用互斥锁的方式，但是每次退出锁的时候都需要重新进行一次调度，会消耗一点时间。
下面是互斥锁的接口：
```code
/**
 * @brief 互斥锁，对临界区代码进行保护，在互斥锁锁定到解锁之间的代码是不能被任务
 *        切换打断的，但是能进行中断响应
 * @code
 *      ...
 *      osel_int8_t s = 0;
 *      s = osel_mutex_lock(OSEL_MAX_PRIO);
 *      // 这里就是临界区代码，需要用互斥锁进行保护
 *      osel_mutex_unlock(s);
 * @endcode
 */
osel_int8_t osel_mutex_lock(osel_int8_t prio_ceiling);

/**
 * @brief 互斥锁解锁
 * @see osel_mutex_lock()
 */
void osel_mutex_unlock(osel_int8_t org_prio);
```
这里的互斥锁是单线程的，通过锁定调度器来完成资源的互斥操作。所以他也是可以嵌套调用的。在嵌套使用过程中，只要关注锁定的优先级即可。

#### 5.2.3 守护线程
守护线程提供了一种干净的方式来实现了互斥功能，不用担心优先级翻转以及死锁。
守护线程对某一资源有唯一的所有权，只有守护线程才可以直接访问其守护的资源--其他线程要访问该资源只能间接的通过守护线程来访问。


## 6.低功耗的应用场景
wsnos的初衷就是应用于低功耗场景，所以这部分功能是非常简单的。
```code
/**
 * @brief 提供系统空闲任务的钩子函数，这个接口多用于低功耗应用。
 * @param[in] idle 空闲任务执行的钩子函数主体
 *
 * @code 
 * static uint8_t osel_heap_buf[4096];
 *
 * static void sys_enter_lpm_handler(void *p)
 * {
 *     debug_info_printf(); //*< 延时打印日志信息
 *     LPM3;
 * }
 *
 * int main(void)
 * {
 *     osel_env_init(osel_heap_buf, 4096, 8);
 *     osel_idle_hook(&sys_enter_lpm_handler);
 *
 *     osel_run();
 *     return 0;
 * }
 * @endcode
 */
void osel_idle_hook(osel_task_handler idle);
```
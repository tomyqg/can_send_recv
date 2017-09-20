本文档描述了如何用Unity测试框架进行白盒测试代码编写，同时使用仿真平台运行输出测试结果

## 编写测试用例
这里把测试用例编写放在前面是因为很多时候测试用例的编写与测试对象实现是同步进行的，所以测试用例可能是单独编写的，等待测试对象实现以后再进行测试验证。
### 测试组
测试组包含了测试模块所有的测试用例，针对测试模块需要首先定义一个测试组名。
```code
TEST_GROUP(测试组名);
```

### 测试用例环境初始化
每一个测试用例是一个独立的测试环境，通过测试用例环境初始化接口来定制化测试用例的测试环境。
```code
TEST_SETUP(测试组名)
{
    //*< 这里初始化测试用例的测试环境，在每一个测试用例调用前会调用一次
}
```

### 测试用例编写
测试用例需要用到测试夹具**TEST（测试组名，测试用例名）**。测试用例名建议能够体现出测试用例的测试内容。在测试夹具里面用测试预期宏判断测试结果
```code
TEST(测试组名, 测试用例1)
{
    ; //*< 这里编写测试用例，对测试结果进行判断
}
```
具体的测试用例预期宏请参考unity.h。

### 测试用例环境注销
当测试用例执行完以后，需要把当前测试环境给注销掉，防止测试用例影响到整个测试组。
```code
TEST_TEAR_DOWN(mpool)
{
    ;
}
```

## 准备环境
当测试用例编写完成，同时测试对象实现了，我们需要进行联调测试。

### 准备测试容器
针对每个测试模块我们写了很多测试用例，而我们有很多模块需要测试，那么就会有很多个测试组，这里我们用容器统一管理测试组。一般容器文件我们用后缀runner来修饰，比如app_test_runner.c。
```code
TEST_GROUP_RUNNER(测试组名1)
{
    /* 测试组1的测试用例1与测试组n的测试用例1即使申明一样也没有关系 */
    RUN_TEST_CASE(测试组名1, 测试用例1); 
    RUN_TEST_CASE(测试组名1, 测试用例2);
    ...
}

TEST_GROUP_RUNNER(测试组名n)
{
    RUN_TEST_CASE(测试组名n, 测试用例1);
    RUN_TEST_CASE(测试组名n, 测试用例2);
    ...
}
```

### 准备入口函数
用户代码都是从app_init开始的。而Unity测试框架的入口是从UnityMain开始。

- 把测试组导入UnityMain
- 在app_init里面启动测试线程
- 在测试线程里面启动UnityMain

```code
static void run_all_test(void)
{
    /* 这里添加启动测试组 */
    RUN_TEST_GROUP(测试组名1); 
    ...
    RUN_TEST_GROUP(测试组名n);
}

static char *arg_str[] = {
    "main",
    "-v",
};

PROCESS(app_test, "app_test");
PROCESS_THREAD(app_test, ev, data)
{
    PROCESS_BEGIN();
    static osel_etimer_t delay_timer;
    /* 这里就是测试代码启动的地方 */
    int argc = sizeof(arg_str)/sizeof(char *);
    UnityMain(argc, arg_str, run_all_test);

    OSEL_ETIMER_DELAY(&delay_timer, 10);  
    hal_board_reset(); //*< 这里为了让进程推出，否则集成测试无法停止输出结果
    PROCESS_END();
}
```

整个准备入口工作是一个标准化的流程，只需要增加**RUN_TEST_GROUP（测试组）**来加载测试组内容。

## 编译、运行
### 准备脚本
当前在linux下使用的scons作为编译脚本，需要在test目录下确认是否包含`Sconscript`,`Sconstruct`2个文件。在`Sconstruct`文件里面定义了编译以后的执行文件名字。

```code
TARGET = "执行文件名字." + TARGET_EXT  
```

### 准备scons
ubuntu在控制台下面输入`scons -v`可以查看当前是否已经安装了scons，如果没有，安装scons:

```code
sudo apt-get install scons
```

### 编译
在控制台下，`cd`到test目录，执行scons。等待编译完成。在test目录下查看到对应的执行文件。

### 执行
```code
./执行文件名字.out
```
查看白盒测试结果。
```
a Tests b Failures c Ignored
```
|   测试总数  |  失败数  |  忽略数 |
|   :---:     |  :---:   |  :---:  | 
|       a     |     b    |    c    |
在控制台可以直观的查看到测试用例的结果。
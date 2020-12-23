# VirtualTimerManager
VirtualTimerManager虚拟定时器管理器

用于软件模拟定时器，来弥补硬件资源不足，比如硬件定时器通道不够。

完全基于链表实现，可动态访问虚拟定时器和服务函数。

使用SysTick中断作为软件定时器的基准中断
定时器以毫秒为单位，最短时间间隔1ms。

1  用户注册的虚拟定时器实例通过句柄的形式进行访问。

2  可注册多个虚拟定时器，定时器成功注册后返回一个句柄，若注册失败，返回错误代码。

3  每个定时器实例拥有一个服务函数列表，允许注册多个服务函数，优先调用更早注册的服务函数。

4  每个定时器实例的优先级要求可调节，当优先级一致时，优先调用更早注册的定时器的服务函数列表

5  可通过句柄来操作对应定时器实例的服务函数列表，对列表进行 增加、删除等操作。
      增加：为定时器实例新建一个服务函数
      删除：删除指定的服务函数
      
6  定时器实例和服务函数都可被使能和失能

### 本管理器搭配了一个轻量化内存管理器
VirtualTimerManager.c 和 VTM.h 为VirtualTimerManager的本体文件，MenManager.c 和 MM.H 为内存管理器的文件。 

### 示例
```c 
TimerNode_TypeDef *personalTiemrList[8];//用来储存注册函数返回的句柄，下面三行也是这样
FuncNode_TypeDef *timer1Func[8];
FuncNode_TypeDef *timer2Func[8];
FuncNode_TypeDef *timer3Func[8];
VTM_TimerManagerInit();//管理器的初始化函数

personalTiemrList[0] = VTM_TimerRegister(50, 1, true, NULL, NULL);
//注册一个虚拟定时器
/*50为时间间隔，1为优先级(越大优先级越高)，true为定时器使能，
第一个NULL为服务函数指针(如果不为NULL，则认定该定时器只有一个服务函数，不会拥有服务函数列表)，第二个NULL为服务函数参数*/

timer1Func[0] = VTM_FuncNodeRegister(personalTiemrList[0], led1_func, NULL);
//为虚拟定时器注册一个服务函数
//personalTiemrList[0]为目标虚拟定时器的指针,led1_func为服务函数指针，NULL为服务函数的参数

timer1Func[1] = VTM_FuncNodeRegister(personalTiemrList[0], led2_func, NULL);
timer1Func[2] = VTM_FuncNodeRegister(personalTiemrList[0], led3_func, NULL);

personalTiemrList[1] = VTM_TimerRegister(100, 2, true, NULL, NULL);
timer2Func[0] = VTM_FuncNodeRegister(personalTiemrList[1], led4_func, NULL);
timer2Func[1] = VTM_FuncNodeRegister(personalTiemrList[1], led5_func, NULL);
timer2Func[2] = VTM_FuncNodeRegister(personalTiemrList[1], led6_func, NULL);

personalTiemrList[2] = VTM_TimerRegister(200, 3, true, NULL, NULL);
timer3Func[0] = VTM_FuncNodeRegister(personalTiemrList[2], led7_func, NULL);

personalTiemrList[3] = VTM_TimerRegister(100, 4, true, led8_func, NULL);
personalTiemrList[4] = VTM_TimerRegister(1000,1,true,FuncEnable,NULL);

VTM_Enable(true);//使能管理器
VTM_FuncNodeDel(personalTiemrList[0],timer1Func[1]);//删除一个服务函数
VTM_FuncNodeEnable(timer1Func[2],false);//失能一个服务函数
```

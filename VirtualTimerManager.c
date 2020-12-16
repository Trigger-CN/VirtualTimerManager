/*
VirtualTimerManager虚拟定时器管理器 Lib

使用SysTick中断作为软件定时器的基准中断，
定时器以毫秒为单位，最短时间间隔1ms。

需求：
1  用户注册的软件定时器实例应通过句柄的形式进行访问。
2  可注册多个软件定时器，定时器成功注册后返回一个句柄（允许用户不接受返回的句柄，使该定时器成为一个无法被引用的实例），若注册失败，返回错误代码。
3  每个定时器实例允许注册多个服务函数，优先调用更早注册的服务函数。
4  每个定时器实例的优先级要求可调节，当优先级一致时，优先调用更早注册的定时器的服务函数
5  可通过句柄来操作对应定时器实例的服务函数列表，对列表进行 增加、删除、查找、修改、清空 等操作。
      增加：为定时器实例新建一个服务函数
      删除：删除指定的服务函数
      修改：修改指定的服务函数，将其指向另一个函数指针
      清空：清空该定时器实例的全部服务函数
6  定时器实例可被使能和失能
*/
#include "VTM.h"

static void FuncList_Load(TimerNode_TypeDef *Node);
static void FuncList_Sort(void);
static void FuncList_NodeScan(void);

/*timerList虚拟定时器列表*/
static TimerNode_TypeDef *timerListHead;

/*funcList定时器函数列表*/
static TimerNode_TypeDef *funcList[FuncListSize];

/*fCont定时器函数计数*/
static uint8_t fCont = 0;

bool VTMEnable = false;

/**
  * @brief  Enable 虚拟定时器管理器使能
  * @param  en :使能
  * @retval 无
  */
void VTM_Enable(bool en)
{
    VTMEnable = en;
}

/**
  * @brief  TimerManager_Init 定时器列表初始化
  * @param  无
  * @retval 无
  */
void VTM_TimerManagerInit()
{
    timerListHead = (TimerNode_TypeDef *)MM_Alloc(sizeof(TimerNode_TypeDef));
    timerListHead->func = NULL;
    timerListHead->interval = 0;
    timerListHead->isEnable = false;
    timerListHead->priority = 0;
    timerListHead->perpareTime = 0;
    timerListHead->funcVal = 0;
    timerListHead->Next = NULL;
}


/**
  * @brief  Timer_Register 定时器注册函数
  * @param  intv:时间间隔
  * @param  prio:优先级，越大优先级越高(0-255)
  * @param  en:使能
  * @param  *f:含有一个参数的函数指针
  * @param  val:参数指针
  * @retval newNode:注册的节点的指针
  */
TimerNode_TypeDef * VTM_TimerRegister(uint16_t intv, uint8_t prio, bool en, VTM_Callback_t f, void *val)
{
    TimerNode_TypeDef *root = timerListHead;
    while(root->Next != NULL)
    {
        root = root->Next;
    }
    TimerNode_TypeDef *newNode = (TimerNode_TypeDef*)MM_Alloc(sizeof(TimerNode_TypeDef));
    root->Next = newNode;
    newNode->interval = intv;
    newNode->priority = prio;
    newNode->isEnable = en;
    newNode->func = f;
    newNode->funcVal = val;
    newNode->Next = NULL;
    if(!f)
    {
        newNode->funcListHead = (FuncNode_TypeDef*)MM_Alloc(sizeof(FuncNode_TypeDef));
        newNode->funcListHead->func = NULL;
        newNode->funcListHead->funcVal = NULL;
        newNode->funcListHead->Next = NULL;
    }
    if(newNode)
        return newNode;
    else
        return 0;
}


/**
  * @brief  FuncList_Load 装填定时器函数列表
  * @param  tOp:timerOption定时器列表中定时器下标
  * @retval 无
  */
static void FuncList_Load(TimerNode_TypeDef *Node)
{
    funcList[fCont] = Node;
    fCont++;
}



/**
  * @brief  FuncList_Sort 定时器函数列表排序，按优先级排序，冒泡排序法
  * @param  无
  * @retval 无
  */
static void FuncList_Sort()
{
    uint8_t sTmp = 1;
    for(int i = 0; i < fCont; i++)
    {
        for(int j = sTmp; j < fCont; j++)
        {
            if(funcList[i]->priority < funcList[j]->priority)
            {
                TimerNode_TypeDef *tmp = funcList[j];
                funcList[j] = funcList[i];
                funcList[i] = tmp;
            }
        }
        sTmp++;
    }
}


/**
  * @brief  FuncList_NodeScan 定时器函数列表节点扫描
  * @param  无
  * @retval 无
  */
static void FuncList_NodeScan()
{
    FuncNode_TypeDef *tmp;
    for(int i = 0; i < fCont; i++)
    {
        if(funcList[i]->isEnable)
        {
            if(funcList[i]->func != NULL)
                (*funcList[i]->func)(funcList[i]->funcVal);
            tmp =  funcList[i]->funcListHead;
            while(tmp)
            {
                if(tmp->isEnable && tmp->func != NULL)
                    (*tmp->func)(tmp->funcVal);
                tmp = tmp->Next;
            }
        }
    }
}


/**
  * @brief  OnceCycle_Finish 单次扫描完成
  * @param  无
  * @retval 无
  */
static void OneCycle_Finish()
{
    memset(funcList, 0, FuncListSize);
    fCont = 0;
}


/**
  * @brief  TimerScan 虚拟定时器扫描
  * @param  无
  * @retval 无
  */
void VTM_TimerScan()
{
    TimerNode_TypeDef *Node = timerListHead;
    while(Node)
    {
        Node->perpareTime = (Node->perpareTime + 1) % Node->interval;
        if(!Node->perpareTime&&(Node->func||Node->funcListHead))
            FuncList_Load(Node);
        Node = Node->Next;
    }
    if(timerListHead->Next)
    {
        FuncList_Sort();
        //FuncList_Scan();
        FuncList_NodeScan();
    }
    OneCycle_Finish();
}


/**
  * @brief  Timer_Enable 虚拟定时器使能设置
  * @param  Node :定时器节点
  * @param  en :使能
  * @retval 设置结果 设置失败返回0，设置成功返回1
  */
uint8_t VTM_TimerEnable(TimerNode_TypeDef *Node, bool en)
{
    if(Node)
    {
        Node->isEnable = en; 
        return 1;
    }
    return 0;
}

/**
  * @brief  Timer_SetPriority 虚拟定时器设置优先级
  * @param  Node :定时器节点
  * @param  prio :优先级
  * @retval 设置结果 设置失败返回0，设置成功返回优先级
  */
uint8_t VTM_TimerSetPriority(TimerNode_TypeDef *Node, uint8_t prio)
{
    if(Node)
        Node->priority = prio;
    else
        return 0;
    return Node->priority;
}

/**
  * @brief  Timer_SetInterval 虚拟定时器设置时间间隔
  * @param  Node :定时器节点
  * @param  intv :间隔时间
  * @retval 设置结果 设置失败返回0，设置成功返回：间隔时间
  */
uint16_t VTM_TimerSetInterval(TimerNode_TypeDef *Node, uint16_t intv)
{
    if(Node)
    {
        Node->interval = intv;
        return Node->interval;
    }
    return 0;
}

/**
  * @brief  TimerNodeDel  删除虚拟定时器节点
  * @param  Node :定时器节点
  * @retval 删除结果 删除失败返回0，删除成功返回1
  */
uint8_t VTM_TimerNodeDel(TimerNode_TypeDef *Node)
{
    TimerNode_TypeDef *tmp = timerListHead;
    if(tmp == 0 || Node == 0)
    {
        return 0;
    }
    if(tmp->Next == 0)
    {
        return 0;
    }
    TimerNode_TypeDef *tmpPrev;
    while(tmp != Node)
    {
        tmpPrev = tmp;
        tmp = tmp->Next;
    }
    if(Node->Next)
        tmpPrev->Next = tmp->Next;
    else
        tmpPrev->Next = NULL;
    return MM_Free(Node);
}

/**
  * @brief  VTM_FuncNodeRegister 定时器函数节点注册函数
  * @param  timerNode:目标定时器指针
  * @param  func:目标函数
  * @param  val:目标函数参数
  * @retval newFuncNode:注册的节点的指针
  */

FuncNode_TypeDef *VTM_FuncNodeRegister(TimerNode_TypeDef *timerNode, VTM_Callback_t func, void *val)
{
    FuncNode_TypeDef *root = timerNode->funcListHead;
    while(root->Next != NULL)
    {
        root = root->Next;
    }
    FuncNode_TypeDef *newFuncNode = (FuncNode_TypeDef*)MM_Alloc(sizeof(FuncNode_TypeDef));
    root->Next = newFuncNode;

    newFuncNode->func = func;
    newFuncNode->isEnable = true;
    newFuncNode->funcVal = val;
    newFuncNode->Next = NULL;

    if(newFuncNode)
        return newFuncNode;
    else
        return 0;

}

/**
  * @brief  FuncNodeEnable  函数节点使能
  * @param  timerNode :定时器节点
  * @param  FuncNode :函数节点
  * @param  en :使能状态
  * @retval 
  */
uint8_t VTM_FuncNodeEnable(FuncNode_TypeDef *FuncNode,bool en)
{
    FuncNode_TypeDef *tmp = FuncNode;
    if(tmp)
    {
        FuncNode->isEnable=en;
        return 1;
    }
    return 0;
}


/**
  * @brief  FuncNodeDel  删除函数节点
  * @param  timerNode :定时器节点
  * @param  FuncNode :函数节点
  * @retval 删除结果 删除失败返回0，删除成功返回1
  */
uint8_t VTM_FuncNodeDel(TimerNode_TypeDef *timerNode,FuncNode_TypeDef *FuncNode)
{
    FuncNode_TypeDef *tmp = timerNode->funcListHead;
    if(tmp == 0 || FuncNode == 0)
    {
        return 0;
    }
    if(tmp->Next == 0)
    {
        return 0;
    }
    FuncNode_TypeDef *tmpPrev;
    while(tmp != FuncNode)
    {
        tmpPrev = tmp;
        tmp = tmp->Next;
    }
    if(FuncNode->Next)
        tmpPrev->Next = tmp->Next;
    else
        tmpPrev->Next = NULL;
    return MM_Free(FuncNode);    
}


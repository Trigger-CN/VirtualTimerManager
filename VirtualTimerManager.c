/*
VirtualTimerManager���ⶨʱ�������� Lib

ʹ��SysTick�ж���Ϊ�����ʱ���Ļ�׼�жϣ�
��ʱ���Ժ���Ϊ��λ�����ʱ����1ms��

����
1  �û�ע��������ʱ��ʵ��Ӧͨ���������ʽ���з��ʡ�
2  ��ע���������ʱ������ʱ���ɹ�ע��󷵻�һ������������û������ܷ��صľ����ʹ�ö�ʱ����Ϊһ���޷������õ�ʵ��������ע��ʧ�ܣ����ش�����롣
3  ÿ����ʱ��ʵ������ע���������������ȵ��ø���ע��ķ�������
4  ÿ����ʱ��ʵ�������ȼ�Ҫ��ɵ��ڣ������ȼ�һ��ʱ�����ȵ��ø���ע��Ķ�ʱ���ķ�����
5  ��ͨ�������������Ӧ��ʱ��ʵ���ķ������б����б���� ���ӡ�ɾ�������ҡ��޸ġ���� �Ȳ�����
      ���ӣ�Ϊ��ʱ��ʵ���½�һ��������
      ɾ����ɾ��ָ���ķ�����
      �޸ģ��޸�ָ���ķ�����������ָ����һ������ָ��
      ��գ���ոö�ʱ��ʵ����ȫ��������
6  ��ʱ��ʵ���ɱ�ʹ�ܺ�ʧ��
*/
#include "VTM.h"

static void FuncList_Load(TimerNode_TypeDef *Node);
static void FuncList_Sort(void);
static void FuncList_NodeScan(void);

/*timerList���ⶨʱ���б�*/
static TimerNode_TypeDef *timerListHead;

/*funcList��ʱ�������б�*/
static TimerNode_TypeDef *funcList[FuncListSize];

/*fCont��ʱ����������*/
static uint8_t fCont = 0;

bool VTMEnable = false;

/**
  * @brief  Enable ���ⶨʱ��������ʹ��
  * @param  en :ʹ��
  * @retval ��
  */
void VTM_Enable(bool en)
{
    VTMEnable = en;
}

/**
  * @brief  TimerManager_Init ��ʱ���б��ʼ��
  * @param  ��
  * @retval ��
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
  * @brief  Timer_Register ��ʱ��ע�ắ��
  * @param  intv:ʱ����
  * @param  prio:���ȼ���Խ�����ȼ�Խ��(0-255)
  * @param  en:ʹ��
  * @param  *f:����һ�������ĺ���ָ��
  * @param  val:����ָ��
  * @retval newNode:ע��Ľڵ��ָ��
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
  * @brief  FuncList_Load װ�ʱ�������б�
  * @param  tOp:timerOption��ʱ���б��ж�ʱ���±�
  * @retval ��
  */
static void FuncList_Load(TimerNode_TypeDef *Node)
{
    funcList[fCont] = Node;
    fCont++;
}



/**
  * @brief  FuncList_Sort ��ʱ�������б����򣬰����ȼ�����ð������
  * @param  ��
  * @retval ��
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
  * @brief  FuncList_NodeScan ��ʱ�������б�ڵ�ɨ��
  * @param  ��
  * @retval ��
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
  * @brief  OnceCycle_Finish ����ɨ�����
  * @param  ��
  * @retval ��
  */
static void OneCycle_Finish()
{
    memset(funcList, 0, FuncListSize);
    fCont = 0;
}


/**
  * @brief  TimerScan ���ⶨʱ��ɨ��
  * @param  ��
  * @retval ��
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
  * @brief  Timer_Enable ���ⶨʱ��ʹ������
  * @param  Node :��ʱ���ڵ�
  * @param  en :ʹ��
  * @retval ���ý�� ����ʧ�ܷ���0�����óɹ�����1
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
  * @brief  Timer_SetPriority ���ⶨʱ���������ȼ�
  * @param  Node :��ʱ���ڵ�
  * @param  prio :���ȼ�
  * @retval ���ý�� ����ʧ�ܷ���0�����óɹ��������ȼ�
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
  * @brief  Timer_SetInterval ���ⶨʱ������ʱ����
  * @param  Node :��ʱ���ڵ�
  * @param  intv :���ʱ��
  * @retval ���ý�� ����ʧ�ܷ���0�����óɹ����أ����ʱ��
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
  * @brief  TimerNodeDel  ɾ�����ⶨʱ���ڵ�
  * @param  Node :��ʱ���ڵ�
  * @retval ɾ����� ɾ��ʧ�ܷ���0��ɾ���ɹ�����1
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
  * @brief  VTM_FuncNodeRegister ��ʱ�������ڵ�ע�ắ��
  * @param  timerNode:Ŀ�궨ʱ��ָ��
  * @param  func:Ŀ�꺯��
  * @param  val:Ŀ�꺯������
  * @retval newFuncNode:ע��Ľڵ��ָ��
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
  * @brief  FuncNodeEnable  �����ڵ�ʹ��
  * @param  timerNode :��ʱ���ڵ�
  * @param  FuncNode :�����ڵ�
  * @param  en :ʹ��״̬
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
  * @brief  FuncNodeDel  ɾ�������ڵ�
  * @param  timerNode :��ʱ���ڵ�
  * @param  FuncNode :�����ڵ�
  * @retval ɾ����� ɾ��ʧ�ܷ���0��ɾ���ɹ�����1
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


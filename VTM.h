#ifndef _VTM_H
#define _VTM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "MM.h"
    
#define FuncListSize 16
    
extern bool VTMEnable;
    
typedef void (*VTM_Callback_t)(void* param);
    
typedef struct FuncNode_TypeDef{
    VTM_Callback_t func;
    bool isEnable;
    void *funcVal;
    struct FuncNode_TypeDef *Next;
}FuncNode_TypeDef;

typedef struct TimerNode_TypeDef{
    bool isExist;
    uint16_t interval;
    uint8_t priority;
    bool isEnable;
    uint16_t perpareTime;
    FuncNode_TypeDef *funcListHead;
    VTM_Callback_t func;
    void *funcVal;
    struct TimerNode_TypeDef *Next;
} TimerNode_TypeDef;


void VTM_Enable(bool en);
void VTM_TimerManagerInit(void);
TimerNode_TypeDef * VTM_TimerRegister(uint16_t intv, uint8_t prio, bool en, VTM_Callback_t f, void* val);
FuncNode_TypeDef *VTM_FuncNodeRegister(TimerNode_TypeDef *timerNode,VTM_Callback_t func,void *val);
void VTM_TimerScan(void);
uint8_t VTM_TimerEnable(TimerNode_TypeDef *Node, bool en);
uint8_t VTM_TimerSetPriority(TimerNode_TypeDef *Node, uint8_t prio);
uint16_t VTM_TimerSetInterval(TimerNode_TypeDef *Node, uint16_t intv);
uint8_t VTM_TimerNodeDel(TimerNode_TypeDef *Node);
uint8_t VTM_FuncNodeEnable(FuncNode_TypeDef *FuncNode,bool en);
uint8_t VTM_FuncNodeDel(TimerNode_TypeDef *timerNode,FuncNode_TypeDef *FuncNode);

#ifdef __cplusplus
}// extern "C"
#endif

#endif

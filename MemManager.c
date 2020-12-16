/*
    MemManager�ڴ������
    ���������ڴ������
    ���������������ڲ����޶����������ڴ�����
    �������ڴ�����������в��������ڱ��������ڴ�أ��޷��������ڴ��������ڴ�
    
    ���ߣ�Trigger
    ���ڣ�2020.12.13
*/

#include "MM.h"
/*Pool_Size �ڴ�ش�С*/
#define Pool_Size 1024
/*AllocTabel_Size �ڴ�������С*/
#define AllocTabel_Size Pool_Size/8

/*memPool �ڴ��*/
static uint8_t memPool[Pool_Size];
/*memAllocTabel �ڴ�����*/
static uint8_t memAllocTabel[AllocTabel_Size];

/**
  * @brief  MemPoolInit ��ʼ���ڴ�غ��ڴ�����
  * @param  ��
  * @retval ��
  */
void MM_MemPoolInit()
{
    for (int i = 0; i < 1024; i++)
    {
        memPool[i] = 0;
        if(memPool[i])
            memAllocTabel[i / 8] = memAllocTabel[i / 8] |= (1 << i % 8);
        else
            memAllocTabel[i / 8] = memAllocTabel[i / 8] &= (~(1 << i % 8));
    }
}

///**
//  * @brief  MemPoolScan �ڴ��ɨ��
//  * @param  ��
//  * @retval ��
//  */
//static void MM_MemPoolScan()
//{
//    for (int i = 0; i < 1024; i++)
//    {
//        if (memPool[i])
//            memAllocTabel[i / 8] = memAllocTabel[i / 8] |= (1 << i % 8);
//        else
//            memAllocTabel[i / 8] = memAllocTabel[i / 8] &= (~(1 << i % 8));
//    }
//}

/**
  * @brief  SpaceSearch �ڴ�ռ���ң������ڴ��������ң�
  * @param  s��(size_t)�ռ��С
  * @retval (int16_t)���ʿռ���Ԫ���±�(δ�ҵ��ռ䷵��-1)
  */
static int16_t MM_SpaceSearch(size_t s)
{
    size_t spaceSize = 0;
    for (int i = 0; i < 1024; i++)
    {
        uint8_t tmp = memAllocTabel[i / 8];
        if (!(tmp & (0x80 >> (i % 8))))
            spaceSize++;
        else
            spaceSize = 0;
        if (spaceSize == s + 2)
        {
            return i - s;
        }
    }
    return -1;
}

/**
  * @brief  GetTableIndex ��ȡ��Ԫ�صķ�����±�
  * @param  Block (void*)Ŀ���ַ
  * @retval (uint16_t)������±�
  */
uint16_t MM_GetTableIndex(void* Block)
{
    uint8_t* poolP = memPool;
    return ((uint8_t*)Block - poolP) / 8;
}

/**
  * @brief  Alloc �����ڴ�
  * @param  s��(size_t)�ռ��С
  * @retval (void*)�ռ��ַ
  */
void* MM_Alloc(size_t s)
{
    int spaceIndex = MM_SpaceSearch(s);
    if(spaceIndex==-1)
        return 0;
    for (size_t i = spaceIndex; i < spaceIndex + s; i++)
    {
        memAllocTabel[i / 8] = memAllocTabel[i / 8] |= (0x80 >> i % 8);
    }
    return &memPool[spaceIndex];
}

/**
  * @brief  Free �ͷ��ڴ�
  * @param  Block (void*)Ŀ���ַ
  * @retval (uint8_t)�ͷŽ��
  */
uint8_t MM_Free(void* Block)
{
    uint8_t* poolP = memPool;
    uint16_t blockIndex = (uint8_t*)Block - poolP;
    uint16_t pDev = blockIndex;
    while ((memAllocTabel[pDev / 8] & (0x80 >> (pDev % 8))))
    {
        memAllocTabel[pDev / 8] = memAllocTabel[pDev / 8] &= (~(0x80 >> pDev % 8));
        pDev++;
    }
    if(!(memAllocTabel[blockIndex / 8] & (0x80 >> (blockIndex % 8))))
        return 1;
    else
        return 0;
}

/**
  * @brief  Set �����ڴ�ֵ
  * @param  Block (void*)Ŀ���ַ
  * @param  val (int)Ŀ��ֵ
  * @param  s (size_t)Ŀ���С
  * @retval (uint8_t)���ý��
  */
uint8_t MM_Set(void *Block, int val, size_t s)
{
    uint8_t *pBlock = (uint8_t*)Block;
    int valBlock = (int)Block;
    if(pBlock>=&memPool[0]&&pBlock<=&memPool[Pool_Size-1])
    {
        int blockIndex = valBlock-(int)memPool;
        for(int i = blockIndex;i<blockIndex+s;i++)
            memPool[i] = val;
    }
    else
        return 0;
    return 1;
}


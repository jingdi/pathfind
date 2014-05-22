#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "heap.h"

/****************************
*
*堆结构
*
*****************************/

#define DEFAULT_HEAPSIZE	1000
#define HEAPADDSIZE			1000


static void buildHeap(Heap *pHeap);
//如果是叶结点，返回TRUE
static int isLeaf(Heap *pHeap,int iPos);
//返回左孩子位置
static int leftchild(int pos);
	//返回右孩子位置
static int rightchild(int pos);

// 返回父结点位置
static int parent(int pos);

//从position向上开始调整，使序列成为堆
static void siftUp(Heap *pHeap,int iPosition);
//筛选法函数，参数left表示开始处理的数组下标
static void siftDown(Heap *pHeap,int iPosition);


Heap *create_Heap(const int* pItemArray,const int iArraySize,pfcompare pFunCompare,pfupdate pFunUpdate)
{
	int iDefaultSize = DEFAULT_HEAPSIZE;
	Heap *pHeap;
	if(iArraySize>iDefaultSize)
	{
		iDefaultSize = iArraySize;
	}
	pHeap =(Heap *)malloc(sizeof(Heap));
	if(!pHeap)
	{
		return 0;
	}
	pHeap->iMaxSize = iDefaultSize;
	pHeap->pItemArray = (int *)malloc(iDefaultSize*sizeof(int));
	if(!pHeap->pItemArray)
	{
		goto MEM_ERROR;
	}
	memcpy(pHeap->pItemArray,pItemArray,iArraySize*sizeof(int));
	pHeap->iCurrentSize = iArraySize;
	pHeap->pFunCompare = pFunCompare;
	pHeap->pFunUpdate = pFunUpdate;
	buildHeap(pHeap);
	return pHeap;

MEM_ERROR:		//木有内存咯
	if(pHeap->pItemArray)
	{
		free(pHeap->pItemArray);
	}
	if(pHeap)
	{
		free(pHeap);
	}
	return 0;
}

//建堆
void buildHeap(Heap *pHeap)
{
	//反复调用筛选函数，问题：CurrentSize<2?
	int i;
	for (i=pHeap->iCurrentSize/2-1; i>=0; i--)
	{
		siftDown(pHeap,i);	
	}
} 

//筛选法函数，参数left表示开始处理的数组下标
void siftDown(Heap *pHeap,int iPosition)
{
	int iCurrentSize = pHeap->iCurrentSize;
	int *pItemArray = pHeap->pItemArray;

	pfcompare pFunCompare = pHeap->pFunCompare;
	pfupdate pFunUpdate = pHeap->pFunUpdate;
	int i=iPosition;//标识父结点
	//此时j不一定是关键值较小的子结点，
	//但是希望它标识关键值较小的子结点。
	int j=2*i+1;		
//	void *pParent=(char *)pItemArray + i*iItemSize;//保存父结点

	int iTemp = pItemArray[i];
	//过筛
	while(j<iCurrentSize)
	{
		int iCurrentItem = pItemArray[j];
	    if((j<iCurrentSize-1)&&(pFunCompare(iCurrentItem,pItemArray[j+1])>0))
		{
			//while第一次循环时，如果进入，比较左结点和右结点的值
			//如果左结点的值大于右结点的值则，j指向数值较小的子结点
			j++;
			iCurrentItem = pItemArray[j];
		}			
		if(pFunCompare(iTemp,iCurrentItem)>=0)
		{
			pItemArray[i] = iCurrentItem;
			pFunUpdate(iCurrentItem,i);
			i=j;
			j=2*j+1;//向下继续
		}//end if
		else
		{
			break;
		}
	}//end if
	pItemArray[i] = iTemp;
	pFunUpdate(iTemp,i);
}

//从position向上开始调整，使序列成为堆
void siftUp(Heap *pHeap,int iPosition) 
{
	int iTempPos = iPosition;
	int *pItemArray = pHeap->pItemArray;
	int iParent = pItemArray[parent(iTempPos)];
	int iTemp = pItemArray[iTempPos];
//	void *pTemp = (char *)pItemArray + iTempPos*iItemSize;
	pfcompare pFunCompare = pHeap->pFunCompare;
	pfupdate pFunUpdate = pHeap->pFunUpdate;
	//请比较父子结点直接swap的方法
	while((iTempPos>0)&&(pFunCompare(iParent,iTemp) > 0))   
	{
		//iTemp = iParent;
		pItemArray[iTempPos] = iParent;
		pFunUpdate(iParent,iTempPos);
		iTempPos = parent(iTempPos);
		iParent = pItemArray[parent(iTempPos)];
	}
	pItemArray[iTempPos] = iTemp;
	pFunUpdate(iTemp,iTempPos);
}

//如果是叶结点，返回TRUE
int isLeaf(Heap *pHeap,int iPos)
{
	return (iPos>=pHeap->iCurrentSize/2)&&(iPos<pHeap->iCurrentSize);
}

//返回左孩子位置
int leftchild(int iPos)
{
	return 2*iPos+1;//返回左孩子位置
}

//返回右孩子位置
int rightchild(int iPos)
{
	return 2*iPos+2;//返回右孩子位置
}

// 返回父结点位置
int parent(int iPos)  
{
	return (iPos-1)/2;//返回父结点位置
}

//向堆中插入新元素newNode
int insert_Heap(Heap *pHeap,int iNewItem)
{
	int iCurrentSize = pHeap->iCurrentSize;
	int iMaxSize = pHeap->iMaxSize;
	int *pItemArray = pHeap->pItemArray;
	if(iCurrentSize == iMaxSize)//堆空间已经满 动态增加
	{
			int *pNewItemArray = malloc((iMaxSize + HEAPADDSIZE)*sizeof(int));
			if(pNewItemArray)
			{
				memcpy(pNewItemArray,pItemArray,iCurrentSize*sizeof(int));
				free(pItemArray);
				pHeap->pItemArray = pNewItemArray;
				pHeap->iMaxSize = iMaxSize + HEAPADDSIZE;
				pItemArray = pNewItemArray;
			}
			else
			{
				return 0;
			}
	}
	pItemArray[iCurrentSize] = iNewItem;
	siftUp(pHeap,iCurrentSize);//向上调整
	pHeap->iCurrentSize++;
	return 1;
}

//从堆顶删除最小值
int remove_Min(Heap *pHeap)
{
	int *pItemArray = pHeap->pItemArray;
	if(pHeap->iCurrentSize==0)
	{
		return 0;
	}
	else
	{		
		//复制堆顶到交换区 准备返回
		int iSwap = pItemArray[0];
		//交换堆顶和最后一个元素
		pHeap->iCurrentSize--;
		pItemArray[0] =  pItemArray[pHeap->iCurrentSize];
		if(pHeap->iCurrentSize>1)  // <=1就不要调整了
		{
			//从堆顶开始筛选
			siftDown(pHeap,0);
		}
		return iSwap;
	}//end else
}

void delete_Heap(Heap **pHeap)
{
	Heap *pRealHeap = *pHeap;
	if(pRealHeap)
	{
		if(pRealHeap->pItemArray)
		{
			free(pRealHeap->pItemArray);
		}
		free(pRealHeap);
	}
	*pHeap = 0;
}

/*******************************
*更新堆
*******************************/
void update_Heap(Heap *pHeap,int iPos,int iType)
{
	//find pItem first
	int iHeapSize = pHeap->iCurrentSize;
	int *pItemArray = pHeap->pItemArray;
	if(iPos >= iHeapSize)
	{
		return;
	}
	if(UPDATE_UP == iType)
	{
		siftUp(pHeap,iPos);
	}
	else
	{
		siftDown(pHeap,iPos);
	}
}
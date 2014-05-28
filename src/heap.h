
/*
Copyright (c) 2012-2014 threestone.cn and chunbai.com

www.threestone.cn 
www.chunbai.com
email:zhanglei0321@gmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/


#ifndef _HEAP_H_
#define _HEAP_H_

/***************************************************
*堆结构及最大/小堆实现
*堆元素数据类型为 int
***************************************************/

/*
*
*const define
*/
//更新堆的方式
#define UPDATE_UP 1		//适用于最小堆中值变小 最大堆中值变大			
#define UPDATE_DOWN 2		//使用于最小堆中值变大 最大堆中值变小
/*
function point
*/

//函数指针 比较a b 的大小
typedef int (*pfcompare) (int ,int,char*);		

typedef void (*pfupdate) (int,int,char*);

/*
struct 
*/

/*堆结构*/
typedef struct
{
	int iCurrentSize;	//当前堆中元素数目
	int iMaxSize;		//堆所能容纳的最大元素数目
	int *pItemArray;	//存放元素的数组
	pfcompare pFunCompare;	//比较函数	用于比较两个元素的大小
	pfupdate pFunUpdate;
	char *pNodeInfo;
}Heap;


/*
function api
*/

/*
*create_Heap 创建堆结构
*参数
*@pItemArray 初始元素数组
*@iArraySize 数组的大小
*@pFunCompare 比较函数  指定不同的比较函数 可实现对应的最大或者最小堆
*返回
*建好的堆  或者是null
**/
Heap *create_Heap(const int* pItemArray,const int iArraySize,pfcompare pFunCompare,pfupdate pFunUpdate,char *pInNodeInfo);

/*
*delete_Heap 销毁堆结构
*参数
*@pHeap  指向堆结构指针的指针
*说明 顺带帮忙将堆结构指针置为NULL 一条龙服务
*/
void delete_Heap(Heap **pHeap);

/*
*insert_Heap 插入新元素
*@pHeap 堆指针
*@iNewItem 新元素
*/
int insert_Heap(Heap *pHeap,int iNewItem);

/*
*remove_Min 删除最小值
*参数
*@pHeap 堆指针
*返回 最小值元素
*/
int remove_Min(Heap *pHeap);

/*
* update_Heap 更新堆
*参数
*@pHeap 堆指针
*@iPos 需要更新的元素的位置
*@iUpdateType 更新方式  向上或者向下
*
*/
void update_Heap(Heap *pHeap,int iPos,int iType);

#endif
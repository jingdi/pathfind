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
#ifndef	_NAV_MESH_H_ 
#define _NAV_MESH_H_

/*
	mesh数据文件结构说明
	1.int mesh数据块个数N(每个数据块是个联通的多边形) 4字节
	2.int 三角形总个数
	3.下面是N个三角形块

	三角形块结构说明
	1.int 三角形个数M	4字节
	2.M个三角形信息
	3.三角形块的包围盒信息 minX minY maxX maxY   4*4 = 16字节

	三角形结构说明(为了计算方便 三角形三个点按逆时针方向存储)
	1.三角形在块中的索引 从1开始  4字节
	2.p1点坐标 x，y  2*4 = 8字节
	3.p2点坐标 x, y	 2*4 = 8字节
	4.p3点坐标 x, y	 2*4 = 8字节
	5.三角形邻接边信息	(p1,p2) (p2,p3) (p3,p1)分别联通的三角形索引  3*4 =12字节  注 0为没有联通三角形	
*/

#include "triangle.h"

#define SUCCESS		0			//成功
#define ONLYFINDONE -1			//只找到了一个点
#define NOINRANGE	-2			//起点终点没在当前多边形内
#define FAILD	-3			//没找到路径  一般不太可能发生

typedef struct _MeshData
{
	int iTriNum;
	Rectangle recBox;			//包围盒
}MeshData;

//for Astar
//节点信息
typedef struct Snode
{
	int iF;
	int iG;
	int iH;
	int iIndex;			//三角形索引
	int iLinkEdge;		//与父节点的邻接边	
	struct Snode *pNext;
	struct Snode *pParent;
}Node;

#define OPEN	1		//在open里
#define CLOSE	2		//在colse里
#define NEVER	0		//还没有走到的

//节点状态信息
typedef struct SnodeState
{
	int pos;
	Node *pLink;
	char state;
}NodeState;


char *initData(char *pData,int *pIDataLen);

void unintData(char *pData);

int pathFind(char *pData,int iSX,int iSY,int iES,int iEY,Point **pPath,int *pPathLen);

#endif
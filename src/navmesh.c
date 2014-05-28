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
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

#include "datatype.h"
#include "heap.h"
#include "triangle.h"
#include "navmesh.h"

#pragma pack(1)
typedef struct
{
	int iMeshCount;
	MeshData mesh[];
}MeshHeader, *pMeshHeader;
#pragma pack()

//检查是否为小端cpu
int checkCpuLE()
{
	{
		union w
		{
			int a;
			char b;
		}c;
		c.a = 1;
		return c.b == 1;
	}
}

//读取int  小端
int readIntB(char **pInData)
{
	char *pData = *pInData;
	int iRet = *((int *)pData);
	pData += 4;
	*pInData = pData;
	return iRet;
}

//读取int  大端
int readIntL(char **pInData)
{
	char *pData = *pInData;
	int iRet = 0;
	char *pRet = (char *)&iRet;
	pRet[0] = pData[3];
	pRet[1] = pData[2];
	pRet[2] = pData[1];
	pRet[3] = pData[0];
	pData += 4;
	*pInData = pData;
	return iRet;

}
typedef int (*pfunreadint) (char **);


//初始化地图数据
char *initData(char *pData,int *iLen)
{
	MeshData *pMeshData = 0;	//数据块起始位置
	Triangle *pTriangle = 0;	//三角形信息其实位置
	char *pRead = pData;	//读取指针
	Triangle *pCurTri;
	int iMeshNum,iTotalTriNum;
	int iTriNum;
	int loop,loop1,loop2;
	MeshHeader *pRetData = 0;
	int iTriIndex = 0;
	int iDataLen = 0;		//返回数据总长度
	pfunreadint pfri = readIntL;

	if(!checkCpuLE())
	{
		pfri = readIntB;
	}
	 
	iMeshNum = pfri(&pRead);		//读取地图块个数
	iTotalTriNum = pfri(&pRead);	//读取三角形总个数
	iDataLen = sizeof(MeshHeader) + sizeof(MeshData)*iMeshNum + sizeof(Triangle)*iTotalTriNum;
	pRetData = (MeshHeader *)malloc(iDataLen);
	if(!pRetData)
	{
		goto MEM_ERR;
	}
	*iLen = iDataLen;
	pRetData->iMeshCount = iMeshNum;
	pMeshData = pRetData->mesh;			
	pTriangle = (Triangle *)(&(pMeshData[iMeshNum]));
	pCurTri = pTriangle;
	for(loop = 0;loop < iMeshNum;loop++)
	{
		READ_INT(pRead,iTriNum);		//读取三角形个数
		pMeshData[loop].iTriNum = iTriNum;		
		//循环读取三角形数据
		for(loop1 = 0;loop1 < iTriNum;loop1++)
		{
			int temp = pfri(&pRead);
			pCurTri->p1.iX = pfri(&pRead);
			pCurTri->p1.iY = pfri(&pRead);
			pCurTri->p2.iX = pfri(&pRead);
			pCurTri->p2.iY = pfri(&pRead);
			pCurTri->p3.iX = pfri(&pRead);
			pCurTri->p3.iY = pfri(&pRead);
			for(loop2 = 0;loop2<3;loop2++)	//读取三角形各个边邻接关系
			{
				pCurTri->linkIndex[loop2] = pfri(&pRead);
			}
			//生成包围盒
			createRectBox(pCurTri);
			pCurTri++;
		}
		//读取多边形的包围盒
		pMeshData[loop].recBox.iMinX = pfri(&pRead);
		pMeshData[loop].recBox.iMinY = pfri(&pRead);
		pMeshData[loop].recBox.iMaxX = pfri(&pRead);
		pMeshData[loop].recBox.iMaxY = pfri(&pRead);
	}
	return (char *)pRetData;

MEM_ERR:
	if(pRetData)
	{
		free(pRetData);
	}
	return 0;
}

//反初始化数据 应该用不着 ~>~
void unintData(char *pData)
{
	if(pData)
	{
		free(pData);
	}
}

Node *pNodeList;	//记录已经被遍历到的节点

NodeState *pNodeState;	//记录每个点的状态		三角形个数 * sizeof(NodeState)


/**
**比较a b 元素的大小
**A*中 即为比较二者估计值的大小
**/
int astar_compare(int a,int b)
{
	Node *pNodeA = pNodeState[a-1].pLink;
	Node *pNodeB = pNodeState[b-1].pLink;
	return pNodeA->iF - pNodeB->iF;
}

/**
** 用于二叉优先堆更新数据位置
**/
void update_Pos(int iIndex,int iPos)
{
	pNodeState[iIndex-1].pos = iPos;
}

//寻路
//pPath 指向返回路径的指针
//pPathLen 指向返回路径长度的指针
//算法步骤
//1.查找起点终点所在的三角形
//2.A*查找需要经过的三角形
//3.利用步骤2生成的三角形生成路径
int pathFindCore(MeshData *pMeshData,Triangle *pTriangle,Point start,Point end,Point **pPath,int *pPathLen)
{
	int iStartIndex = 0 ,iEndIndex = 0;
	int loop1;

	Heap *pOpenHeap;			
	Node *pCurrentNode;
	Node *pNewNode;
	Point *pCrossPoint = 0 ;
	Point *pPathRet = 0;

	int iCurrentIndex;

	int iFindFlag = 0;	//是否找到了结果
	//查找起点终点所在的三角形
	Triangle *pCurTri = pTriangle;
	int iTriNum = pMeshData->iTriNum;
	Rectangle *meshBox = &pMeshData->recBox;
	//检查两个点是否都在包围盒内
	{
		int checkStart = 0;
		int checkEnd = 0;
		if(
			start.iX > meshBox->iMaxX || 
			start.iX < meshBox->iMinX ||
			start.iY > meshBox->iMaxY ||
			start.iY < meshBox->iMinY
		   )
		{
			checkStart = 0;
		}
		else
		{
			checkStart = 1;
		}

		if(
			end.iX > meshBox->iMaxX || 
			end.iX < meshBox->iMinX ||
			end.iY > meshBox->iMaxY ||
			end.iY < meshBox->iMinY
		   )
		{
			checkEnd = 0;
		}
		else
		{
			checkEnd = 1;
		}

		if(checkStart == 0 && checkEnd == 0)
		{
			return NOINRANGE;
		}
		
		if(checkStart != checkEnd)
		{
			return ONLYFINDONE;
		}
	}

	for(loop1 = 0;loop1<iTriNum;loop1++)
	{
		if(iStartIndex == 0)
		{
			if(point_in_tri(pCurTri,&start))
			{
				iStartIndex = loop1+1;
			}
		}
		if(iEndIndex == 0)
		{
			if(point_in_tri(pCurTri,&end))
			{
				iEndIndex = loop1+1;
			}
		}
		if(iStartIndex != 0&&iEndIndex != 0)
		{
			break;
		}
		pCurTri++;
	}

	if(iStartIndex == 0 && iEndIndex == 0)	//点不在三角形网格内
	{

		return NOINRANGE;
	}

	if(iStartIndex == 0 || iEndIndex == 0)
	{
		return ONLYFINDONE;
	}
	pCurTri =  pTriangle;
	//astar
	// g(x) =  父节点的g + 与没有经过的父节点的一边的中位线长度
	// h(x) =  当前节点中心点到终点的理想距离
	// f(x) = g(x) + h(x)
	//初始化 open colse  以及搜索过的节点记录

	pCurrentNode = pNodeList;
	
	pNodeState = (NodeState*)malloc(iTriNum*sizeof(NodeState));
	if(!pNodeState)
	{
		goto MEM_ERROR;
	}
	memset(pNodeState,0,iTriNum*sizeof(NodeState));

	pOpenHeap = create_Heap(0,0,astar_compare,update_Pos);
	if(!pOpenHeap)
	{
		goto MEM_ERROR;
	}
	iCurrentIndex = iStartIndex;

	pNewNode = (Node*)malloc(sizeof(Node));
	pNewNode->iIndex = iStartIndex;
	pNewNode->pParent = 0;
	pNewNode->pNext = 0;
	pNewNode->iLinkEdge = 0;
	pNewNode->iG = 0;
	pNewNode->iH = ABS(end.iX - (pCurTri->rec.iMaxX - (pCurTri->rec.iMaxX - pCurTri->rec.iMinX)/2)) 
				 + ABS(end.iY - (pCurTri->rec.iMaxY - (pCurTri->rec.iMaxY - pCurTri->rec.iMinY)/2));
	pNewNode->iF = pNewNode->iG + pNewNode->iH;

	if(!insert_Heap(pOpenHeap,iCurrentIndex))
	{
		goto MEM_ERROR;
	}

	pNodeState[iCurrentIndex-1].state = OPEN;
	pNodeState[iCurrentIndex-1].pLink = pNewNode;
	pNodeList = pNewNode;
	pCurrentNode = pNodeList;
	
	while(pOpenHeap->iCurrentSize)
	{
		int iBestNodeIndex = remove_Min(pOpenHeap);
		int loop2;
		Triangle *pBestTri;
		Node *pBestNode = pNodeState[iBestNodeIndex-1].pLink;
		pNodeState[iBestNodeIndex-1].state = CLOSE;
		pBestTri = pCurTri+(pBestNode->iIndex-1);
		if(iEndIndex == iBestNodeIndex)
		{
			iFindFlag = 1;
			break;
		}
		//扩散到周围三个边
		for(loop1=0;loop1<3;loop1++)
		{
			int iNextIndex = pBestTri->linkIndex[loop1];
			int iCurState;
			int iLinkEdge;
			int iNewNextNodeG;
			Triangle *pChildTri = pCurTri+(iNextIndex-1);
			if(iNextIndex == 0)
			{
				continue;
			}
			iCurState = pNodeState[iNextIndex-1].state;
			iNewNextNodeG = pBestNode->iG;
			if(pBestNode->iLinkEdge == 0)			//和父节点没有公共边 可能是起始点 取父节点最短边
			{
				;
			}
			else
			{
				int iCorssEdge = 5 - pBestNode->iLinkEdge -  loop1;
				switch(iCorssEdge)
				{
					case 1:		//边1 对应 1-2点
						iNewNextNodeG += (ABS(pBestTri->p1.iX - pBestTri->p2.iX) + ABS(pBestTri->p1.iY - pBestTri->p2.iY))/2;
					case 2:
						iNewNextNodeG += (ABS(pBestTri->p2.iX - pBestTri->p3.iX) + ABS(pBestTri->p2.iY - pBestTri->p3.iY))/2;
					default:
						iNewNextNodeG += (ABS(pBestTri->p1.iX - pBestTri->p3.iX) + ABS(pBestTri->p1.iY - pBestTri->p3.iY))/2;
				}
			}
			if(OPEN == iCurState)		//next node in open 
			{
				Node *pNextNode = pNodeState[iNextIndex-1].pLink;
				if(iNewNextNodeG < pNextNode->iG)		//may be find a batter father
				{
					//查找与父节点的邻接表
					iLinkEdge = 1;
					for(loop2 = 0; loop2 < 3; loop2++)
					{
						if(pChildTri->linkIndex[loop2] == iBestNodeIndex)
						{
							iLinkEdge = loop2 + 1;
						}
					}
					pNextNode->iG = iNewNextNodeG;
					pNextNode->iF = iNewNextNodeG + pNextNode->iH;
					pNextNode->pParent = pBestNode;
					pNextNode->iLinkEdge = iLinkEdge;
					//update open heap 
					update_Heap(pOpenHeap, pNodeState[iNextIndex-1].pos,UPDATE_UP);
				}
			}
			else if(CLOSE == iCurState)		//next node in close
			{
				Node *pNextNode = pNodeState[iNextIndex-1].pLink;
				
				if(iNewNextNodeG < pNextNode->iG)		//may be find a batter father
				{
					//查找与父节点的邻接表
					iLinkEdge = 1;
					for(loop2 = 0; loop2 < 3; loop2++)
					{
						if(pChildTri->linkIndex[loop2] == iBestNodeIndex)
						{
							iLinkEdge = loop2 + 1;
						}
					}
					pNextNode->iG = iNewNextNodeG;
					pNextNode->iF = iNewNextNodeG + pNextNode->iH;
					pNextNode->pParent = pBestNode;

					pNodeState[iNextIndex-1].state = OPEN;
					//inster to  open heap 
					if(!insert_Heap(pOpenHeap, iNextIndex))
					{
						goto MEM_ERROR;
					}
				}
			}
			else	//还没有被访问过
			{
				iLinkEdge = 1;
				for(loop2 = 0; loop2 < 3; loop2++)
				{
					if(pChildTri->linkIndex[loop2] == iBestNodeIndex)
					{
						iLinkEdge = loop2 + 1;
					}
				}
				// create a node 
				pNewNode = (Node*)malloc(sizeof(Node));
				pNewNode->iIndex = iNextIndex;
				pNewNode->pParent = pBestNode;
				pNewNode->pNext = 0;
				pNewNode->iLinkEdge = iLinkEdge;
				pNewNode->iG = iNewNextNodeG;
				pNewNode->iH = ABS(end.iX - (pChildTri->rec.iMaxX - (pChildTri->rec.iMaxX - pChildTri->rec.iMinX)/2)) 
				 + ABS(end.iY - (pChildTri->rec.iMaxY - (pChildTri->rec.iMaxY - pChildTri->rec.iMinY)/2));
				pNewNode->iF = pNewNode->iG + pNewNode->iH;
				pNodeState[iNextIndex-1].state = OPEN;
				pNodeState[iNextIndex-1].pLink = pNewNode;
				pCurrentNode->pNext = pNewNode;
				pCurrentNode = pNewNode;

				if(!insert_Heap(pOpenHeap,iNextIndex))
				{
					goto MEM_ERROR;
				}
			}	 
		}	
	}
	if(iFindFlag)			//找到咯
	{
		Point *curLeft = 0;
		Point *curRight = 0;
		int iCrossNum = 0; 
		Node *pResult = pNodeState[iEndIndex-1].pLink;
		int ResultLen = 0;
		while(pResult)
		{
			ResultLen++;
			pResult = pResult->pParent;
		}
		//*pPathLen = ResultLen;
		pCrossPoint = (Point *)malloc(ResultLen * 2 * sizeof(Point));
		if(!pCrossPoint)
			goto MEM_ERROR;
		pResult = pNodeState[iEndIndex-1].pLink;
		//寻找拐点 用直线连接起点拐点和终点
		pCrossPoint[iCrossNum].iX = end.iX;
		pCrossPoint[iCrossNum].iY = end.iY;
		iCrossNum++;
		if(iStartIndex == iEndIndex)
		{			
			pCrossPoint[iCrossNum].iX = start.iX;
			pCrossPoint[iCrossNum].iY = start.iY;
			iCrossNum++;
		}
		else
		{
			//Triangle *pCurStartTri = pCurTri+(iEndIndex-1);
			Point *pCurStartPoint = &end;
			Node *pLeftNode = pResult;
			Node *pRightNode = pResult;
			while(pResult)
			{
				Triangle *pCurStartTri = pCurTri+(pResult->iIndex-1);
				Node *pLeftNode = pResult;
				Node *pRightNode = pResult;
				switch(pResult->iLinkEdge)
				{
					case 1:
						curLeft = &(pCurStartTri->p2);
						curRight = &(pCurStartTri->p1);
						break;
					case 2:
						curLeft = &(pCurStartTri->p3);
						curRight = &(pCurStartTri->p2);
						break;
					default:
						curLeft = &(pCurStartTri->p1);
						curRight = &(pCurStartTri->p3);
					break;
				}
				pResult = pResult->pParent;
				while(pResult)
				{

					Triangle *pChildTri;
					Point *nextLeft = 0;
					Point *nextRight = 0;
				/*	if(pResult->iLinkEdge == 0)
					{
						pResult = pResult->pParent;
						break;
					}
					*/
					pChildTri = pCurTri+(pResult->iIndex-1);
					switch(pResult->iLinkEdge)
					{
						case 0:
							nextLeft = &(start);
							nextRight = &(start);
							break;
						case 1:
							nextLeft = &(pChildTri->p2);
							nextRight = &(pChildTri->p1);
							break;
						case 2:
							nextLeft = &(pChildTri->p3);
							nextRight = &(pChildTri->p2);
							break;
						default:
							nextLeft = &(pChildTri->p1);
							nextRight = &(pChildTri->p3);
							break;
					}
				
					//比较左边
					if(!pointEqual(curLeft,nextLeft))
					{
						//如果左边的新点 还在右边视线的右边  毫无疑问 右边的老点就是个拐点
						if(multiply(pCurStartPoint,curRight,pCurStartPoint,nextLeft) < 0)
						{
							//将curRight 设置为拐点 新起点 重置 curLeft curRight
							pCrossPoint[iCrossNum].iX = curRight->iX;
							pCrossPoint[iCrossNum].iY = curRight->iY;
							iCrossNum++;
							pCurStartPoint = curRight;
							pResult = pRightNode;
							break;
						}
						else if(multiply(pCurStartPoint,curLeft,pCurStartPoint,nextLeft) <= 0)					
						{
							//如果 左边的新点 不在左边视线的左边 更新左边的视线
							curLeft = nextLeft;
							pLeftNode = pResult; 
						}
					}
					//同理 处理右边
					if(!pointEqual(curRight,nextRight))
					{
						//如果右边的新点 还在左边视线的左边  毫无疑问 左边的老点就是个拐点
						if(multiply(pCurStartPoint,curLeft,pCurStartPoint,nextRight) > 0)
						{
							//将curLeft 设置为拐点 新起点 重置 curLeft curRight
							pCrossPoint[iCrossNum].iX = curLeft->iX;
							pCrossPoint[iCrossNum].iY = curLeft->iY;
							iCrossNum++;
							pCurStartPoint = curLeft;
							pResult = pLeftNode;
							//curLeft = nextLeft;
							//curRight = nextRight;
							break;
						}
						else if(multiply(pCurStartPoint,curRight,pCurStartPoint,nextRight) >= 0)					
						{
							//如果 右边的新点 不在右边视线的右边 更新右边的视线
							curRight = nextRight;
							pRightNode = pResult;
						}
					}
					pResult = pResult->pParent;
				}
			}
			//比较起点
			/*
			if(multiply(pCurStartPoint,curRight,pCurStartPoint,&start) < 0)
			{
				pCrossPoint[iCrossNum].iX = curRight->iX;
				pCrossPoint[iCrossNum].iY = curRight->iY;
				iCrossNum++;
			}
			else if(multiply(pCurStartPoint,curLeft,pCurStartPoint,&start) > 0)
			{
				pCrossPoint[iCrossNum].iX = curLeft->iX;
				pCrossPoint[iCrossNum].iY = curLeft->iY;
				iCrossNum++;
			}
			*/
			if(pCrossPoint[iCrossNum-1].iX == start.iX && pCrossPoint[iCrossNum-1].iY == start.iY)
			{
				;
			}
			else
			{
				pCrossPoint[iCrossNum].iX = start.iX;
				pCrossPoint[iCrossNum].iY = start.iY;
				iCrossNum++;
			}
		}

		//处理拐点 顺序连接拐点
		{
			
			int pointLenth = 0;
			int listLength = 0;
			pPathRet = (Point *)malloc(1000*sizeof(Point));
			listLength = 1000;
			for(loop1 = iCrossNum - 1; loop1>0;loop1--)
			{
				Point *pStartPath = &(pCrossPoint[loop1]); 
				Point *pEndPath = &(pCrossPoint[loop1-1]);
				
				int iStartX = pStartPath->iX;
				int iEndX = pEndPath->iX;
				int iStartY = pStartPath->iY;
				int iEndY = pEndPath->iY;
				int iXoff = iEndX - iStartX;
				int iYoff = iEndY - iStartY;
				double fXadd = 0.0;
				double fYadd = 0.0;
				int absXOFF = ABS(iXoff);
				int absYOFF = ABS(iYoff);
				int iMaxStep = MAX(absXOFF,absYOFF);
				int i;
				double x,y;

				fXadd = ((double)iXoff)/iMaxStep;
				fYadd = ((double)iYoff)/iMaxStep;
				x = iStartX;
				y = iStartY;
				for(i=0;i<iMaxStep;i++)
				{
					if(pointLenth >= listLength)
					{
						Point *pNewPathRet = 0;
						listLength += 1000;
						pNewPathRet = (Point *)malloc(listLength*sizeof(Point));
						if(!pNewPathRet)
						{
							goto MEM_ERROR;
						}
						memcpy(pNewPathRet,pPathRet,pointLenth*sizeof(Point));
						free(pPathRet);
						pPathRet = pNewPathRet;
					}
					
					pPathRet[pointLenth].iX = (int)x;
					pPathRet[pointLenth].iY = (int)y;
					pointLenth++;
					x+=fXadd;
					y+=fYadd; 
				}
			}
			//加入终点
			if(pointLenth >= listLength)
			{
				Point *pNewPathRet = 0;
				listLength += 1;
				pNewPathRet = (Point *)malloc(listLength*sizeof(Point));
				if(!pNewPathRet)
				{
					goto MEM_ERROR;
				}
				memcpy(pNewPathRet,pPathRet,pointLenth*sizeof(Point));
				free(pPathRet);
				pPathRet = pNewPathRet;
			}
			pPathRet[pointLenth].iX = end.iX;
			pPathRet[pointLenth].iY = end.iY;
			pointLenth ++;
			
			*pPath = pPathRet;
			*pPathLen = pointLenth;
			/*
			*pPathLen = iCrossNum;
			*pPath = pCrossPoint;
			*/
			free(pNodeState);
			delete_Heap(&pOpenHeap);
			pCurrentNode = pNodeList;
			while(pCurrentNode)
			{
				Node *pTemp = pCurrentNode;
				pCurrentNode = pCurrentNode->pNext;
				free(pTemp);
			}
			free(pCrossPoint);
			return SUCCESS;
		}

	}
	else
	{	
		free(pNodeState);
		delete_Heap(&pOpenHeap);
		pCurrentNode = pNodeList;
		while(pCurrentNode)
		{
			Node *pTemp = pCurrentNode;
			pCurrentNode = pCurrentNode->pNext;
			free(pTemp);
		}

		return FAILD;
	}
	
MEM_ERROR:
	if(pNodeState)
	{
		free(pNodeState);
	}
	if(pOpenHeap)
	{
		delete_Heap(&pOpenHeap);
	}
	if(pNodeList)
	{
		pCurrentNode = pNodeList;
		while(pCurrentNode)
		{
			Node *pTemp = pCurrentNode;
			pCurrentNode = pCurrentNode->pNext;
			free(pTemp);
		}
	}
	if(pCrossPoint)
	{
		free(pCrossPoint);
	}
	if(pPathRet)
	{
		free(pPathRet);
	}
	return FAILD;
	
}

int pathFind(char *pData,Point start,Point end,Point **pPath,int *pPathLen)
{
	int loop;
	MeshHeader *pRetData = (MeshHeader *)pData;
	int iMeshNum = pRetData->iMeshCount;
	MeshData *pMeshData = pRetData->mesh;
	Triangle *pTriangle = (Triangle *)(&(pRetData->mesh[iMeshNum]));
	int pathFindResult = 0;
	Triangle *pTempTri = pTriangle;
	for(loop = 0;loop < iMeshNum;loop++)
	{
		pathFindResult = pathFindCore(&pMeshData[loop],pTempTri,start,end,pPath,pPathLen);
		pTempTri +=  pMeshData[loop].iTriNum;
		if(NOINRANGE == pathFindResult)		//两个点都不在当前矩形块
		{
			continue;
		}
		return pathFindResult; 		
	}
	return pathFindResult; 
}
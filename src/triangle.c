/*
Copyright (c) 2012-2014 streestone.cn and chunbai.com

www.streestone.cn 
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

#include "triangle.h"

//检查点是否在三角形内
int point_in_tri(Triangle *pTri,Point *pPoint)
{
	Rectangle *rect =  &(pTri->rec);
	//检查边界
	if(pPoint->iX < rect->iMinX)
		return 0;
	if(pPoint->iX >= rect->iMaxX)
		return 0;

	if(pPoint->iY < rect->iMinY)
		return 0;
	if(pPoint->iY >= rect->iMaxY)
		return 0;

	/*//TODO 待优化 用向量叉乘
	triangleArea = getArea(&pTri->p1,&pTri->p2,&pTri->p3);
	triangleArea -= getArea(pPoint,&pTri->p1,&pTri->p2);
	triangleArea -= getArea(pPoint,&pTri->p2,&pTri->p3);
	triangleArea -= getArea(pPoint,&pTri->p1,&pTri->p3);
	if(triangleArea == 0)
	{
		return 1;
	}
	*/
	if(multiply(&pTri->p1,&pTri->p2,&pTri->p1,pPoint) < 0)
	{
		return 0;
	}

	if(multiply(&pTri->p2,&pTri->p3,&pTri->p2,pPoint) < 0)
	{
		return 0;
	}

	if(multiply(&pTri->p3,&pTri->p1,&pTri->p3,pPoint) < 0)
	{
		return 0;
	}
	return 1;
}

//点是否相同
int pointEqual(Point *pP1,Point *pP2)
{
	if((pP1->iX == pP2->iX)&&(pP1->iY == pP2->iY))
		return 1;
	return 0;
}

//三个点组成的三角形面积的2倍
int getArea(Point *p1,Point *p2,Point *p3)
{
	int area = p1->iX * p2->iY 
			 + p2->iX * p3->iY 
			 + p3->iX * p1->iY 
			 - p2->iX * p1->iY 
			 - p3->iX * p2->iY 
			 - p1->iX * p3->iY;
    area = area > 0 ? area : -area;
    return area; 
}

//生成三角形包围盒
void createRectBox(Triangle *pTri)
{
	int iMinX = pTri->p1.iX;
	int iMinY = pTri->p1.iY;
	int iMaxX = iMinX;
	int iMaxY = iMinY;

	if(iMinX > pTri->p2.iX)
	{
		iMinX = pTri->p2.iX;
	}
	else if(iMaxX < pTri->p2.iX)
	{
		iMaxX = pTri->p2.iX;
	}

	if(iMinX > pTri->p3.iX)
	{
		iMinX = pTri->p3.iX;
	}
	else if(iMaxX < pTri->p3.iX)
	{
		iMaxX = pTri->p3.iX;
	}

	if(iMinY > pTri->p2.iY)
	{
		iMinY = pTri->p2.iY;
	}
	else if(iMaxY < pTri->p2.iY)
	{
		iMaxY = pTri->p2.iY;
	}

	if(iMinY > pTri->p3.iY)
	{
		iMinY = pTri->p3.iY;
	}
	else if(iMaxY < pTri->p3.iY)
	{
		iMaxY = pTri->p3.iY;
	}
	pTri->rec.iMinX = iMinX;
	pTri->rec.iMinY = iMinY;

	pTri->rec.iMaxX = iMaxX;
	pTri->rec.iMaxY = iMaxY;
}
/*
// 由三点(last->mid * mid->next)顺序组成的矢量叉积
int multiply(Point *last, Point *mid, Point *next)
{
	return ((mid->iX - last->iX) * (next->iY - mid->iY) - (next->iX - mid->iX) * (mid->iY - last->iY));
}
*/       
// 矢量 p1-> p2   p3->p4的叉积  
int multiply(Point *p1, Point *p2, Point *p3, Point *p4)
{
  return ((p2->iX - p1->iX)*(p4->iY - p3->iY) - (p4->iX - p3->iX)*(p2->iY - p1->iY)); 
}
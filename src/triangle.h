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

/*
* 三角形结构和操作
*/
#ifndef	_TRIANGLE_H_ 
#define _TRIANGLE_H_ 

//点
typedef struct
{
	int iX;
	int iY;
}Point;

typedef struct
{
	Point p1;
	Point p2;
}Line;

typedef struct
{
	int iMinX;
	int iMinY;
	int iMaxX;
	int iMaxY;
}Rectangle;
//三角形
typedef struct
{
	Point p1;
	Point p2;
	Point p3;
	int linkIndex[3];		//三个边的邻接关系
	Rectangle rec;			//包围盒
}Triangle;

void createRectBox(Triangle *pTri);

int point_in_tri(Triangle *pTri,Point *pPoint);

int pointEqual(Point *pP1,Point *pP2);

// 矢量 p1-> p2   p3->p4的叉积  
int multiply(Point *p1, Point *p2, Point *p3, Point *p4);

#endif
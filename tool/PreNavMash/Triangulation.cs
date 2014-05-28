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
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using System.Collections;
using System.Threading;
using System.IO;

namespace PreNavMash
{
    /// <summary>
    /// 三角形
    /// </summary>
    public class Triange
    {
        public Point _p1;
        public Point _p2;
        public Point _p3;


        public Triange(Point p1, Point p2, Point p3)
        {
            if (Util.CheckClockWise(p1, p2, p3))
            {
                this._p1.X = p3.X;
                this._p1.Y = p3.Y;

                this._p2.X = p2.X;
                this._p2.Y = p2.Y;

                this._p3.X = p1.X;
                this._p3.Y = p1.Y;
            }
            else
            {
                this._p1.X = p1.X;
                this._p1.Y = p1.Y;

                this._p2.X = p2.X;
                this._p2.Y = p2.Y;

                this._p3.X = p3.X;
                this._p3.Y = p3.Y;
            }
        }

        /// <summary>
        /// 检查点是否在三角形内
        /// </summary>
        /// <param name="point"></param>
        /// <returns></returns>
        public bool CheckPointInSide(Point point)
        {
            if (point.Equals(_p1) || point.Equals(_p2) || point.Equals(_p3))
                return false;

            if (Util.multiply(_p1, _p2, _p1, point) < 0 )
                return false;

            if (Util.multiply(_p2,_p3,_p2, point) < 0)
                return false;

            if (Util.multiply(_p3, _p1, _p3, point) < 0)
                return false;

            return true;
        }

        public void draw(Graphics g, Pen p)
        {
            g.DrawLine(p,_p1,_p2);
            g.DrawLine(p, _p2, _p3);
            g.DrawLine(p, _p3, _p1);
        }

        public static int getArea(Point p1,Point p2,Point p3)
        {
            int area = p1.X * p2.Y + p2.X *p3.Y + p3.X * p1.Y - p2.X * p1.Y - p3.X * p2.Y - p1.X * p3.Y;
            area = area > 0 ? area : -area;
            return area;
        }


        public Point[] getPointArray()
        {
            return new Point[] { _p1, _p2, _p3 };
        }
        public Rectangle getRect()
        {
            int minX = _p1.X;
            int minY = _p1.Y;
            int maxX = minX;
            int maxY = minY;
            if (_p2.X < minX)
            {
                minX = _p2.X; 
            }
            else if (_p2.X > maxX)
            {
                maxX = _p2.X;
            }

            if (_p3.X < minX)
            {
                minX = _p3.X;
            }
            else if (_p3.X > maxX)
            {
                maxX = _p3.X;
            }

            if (_p2.Y < minY)
            {
                minY = _p2.Y;
            }
            else if (_p2.Y > maxY)
            {
                maxY = _p2.Y;
            }

            if (_p3.Y < minY)
            {
                minY = _p3.Y;
            }
            else if (_p3.Y > maxY)
            {
                maxY = _p3.Y;
            }
            return new Rectangle(minX, minY, (maxX - minX), (maxY - minY));
        }

        public int getPublicEdge(Triange other)
        {
            Point[] otherPoints = other.getPointArray();
            Point[] myPoints = getPointArray();
            bool[] result = new bool[3] { false,false,false};
            for (int i = 0; i < 3; i++)
            {
                Point myPoint = myPoints[i];
                for (int j = 0; j < 3; j++)
                {
                    if (myPoint.Equals(otherPoints[j]))
                    {
                        result[i] = true;
                        break;
                    }
                }
            }
            if (result[0] && result[1])
            {
                return 1;
            }
            if (result[1] && result[2])
            {
                return 2;
            }
            if (result[0] && result[2])
            {
                return 3;
            }
            return 0;
        }

    }


    class Node 
    {
	    /** 坐标点 */
	    public Point v;		//点
	    /** 是否是交点 */
	    public bool i;
	    /** 是否已处理过 */
	    public bool p = false;	
	    /** 进点--false； 出点--true */
	    public bool o = false;
	    /** 交点的双向引用 */
	    public Node other;	
	    /** 点是否在主多边形中*/
	    public bool isMain;
	
	    /** 多边形的下一个点 */
	    public Node next;
	
	    public Node(Point pt, bool isInters, bool main) 
        {
		    this.v = pt;
		    this.i = isInters;
		    this.isMain = main;
	    }
	
    }
    /// <summary>
    /// 任意简单多边形的三角形划分
    /// </summary>
    public class Triangulation
    {

        public static int COLLINEAR = 0;
        public static int PARALELL = 1;
        public static int SEGMENTS_INTERSECT = 2;
        public static int A_BISECTS_B = 3;
        public static int B_BISECTS_A = 4;
        public static int LINES_INTERSECT = 5;

        public static int ON_LINE = 1;
        public static int RIGHT_SIDE = 2;
        public static int LEFT_SIDE = 3;

        public static string fileName = "test.meshdat";

        /// <summary>
        /// 
        /// </summary>
        /// <param name="points"></param>
        /// <returns></returns>
        public static Triange[] Process(Point[] _points)
        {
            Point[] points = CheckPolygon(_points);
            int pointNum = _points.Length;
            ArrayList pList = new ArrayList(_points);
            ArrayList result = ProcessLoop(pList);

            return (Triange [])result.ToArray(typeof(Triange));
        }

        /// <summary>
        /// 多个多边形 三角形分解
        /// </summary>
        /// <param name="polyList"> point 数组 列表</param>
        /// <returns>三角形数组列表</returns>
        public static ArrayList Process(ArrayList polyList)
        {
            //合并多边形  主要是合并包含关系  目前不关心相交的情况
            ArrayList _polyList = new ArrayList();
            foreach (ArrayList obj in polyList)
            {
                Point[] points = CheckPolygon((Point[])obj.ToArray(typeof(Point)));
                _polyList.Add(points);
            }
            ArrayList newPolyList = unionPolygon(_polyList);
            //return newPolyList;
            
            ArrayList result = new ArrayList();

            for (int i = 0; i < newPolyList.Count; i++)
            {
                ArrayList polyGroup = (ArrayList)newPolyList[i];
                ArrayList _result = new ArrayList();
                foreach (Object obj in polyGroup)
                {
                    _result.AddRange(Process((Point[])obj));
                }
                result.Add(_result);
            }
            return result; 
        }

        /// <summary>
        /// 保存数据
        /// </summary>
        /// 数据格式 三角总个数 int
        /// 三角形编号int 三个坐标 int*3*2 中心点位置 int*2
        /// 三角形邻接关系 个数*个数*byte
        /// <param name="trianges"></param>
        public static void saveData(ArrayList triList,string FilePath)
        {
            string realPathName = Path.Combine(FilePath,fileName);
            FileStream fs = new FileStream(realPathName, FileMode.Create);
            BinaryWriter bw = new BinaryWriter(fs, Encoding.Default);
            int meshNum = triList.Count;        //地图分块数
            bw.Write(meshNum);
            int totalTriNum = 0;
            for (int i = 0; i < meshNum; i++)      //各个块三角形数
            {
                totalTriNum += ((ArrayList)triList[i]).Count;
            }
            bw.Write(totalTriNum); //三角形总个数
            foreach (Object obj in triList)
            {
                ArrayList childTriList = (ArrayList)obj;
                Triange[] trianges = (Triange[])childTriList.ToArray(typeof(Triange));
                int triNum = trianges.Length;
                int minX = 0, minY = 0, maxX = 0, maxY = 0;
                bw.Write(triNum);
                for (int i = 0; i < triNum; i++)
                {
                    Triange tr = trianges[i];
                    bw.Write(i + 1);
                    bw.Write(tr._p1.X);
                    bw.Write(tr._p1.Y);

                    bw.Write(tr._p2.X);
                    bw.Write(tr._p2.Y);

                    bw.Write(tr._p3.X);
                    bw.Write(tr._p3.Y);

                    //Point [] trps= new Point[]{tr._p1,tr._p2,tr._p3};
                    Rectangle rec = tr.getRect();
                    if (i == 0)
                    {
                        minX = rec.X;
                        maxX = rec.X + rec.Width;

                        minY = rec.Y;
                        maxY = rec.Y + rec.Height;
                    }
                    else
                    {
                        int curMinX = rec.X;
                        int curMaxX = rec.X + rec.Width;

                        int curMinY = rec.Y;
                        int curMaxY = rec.Y + rec.Height;

                        if (curMinX < minX)
                        {
                            minX = curMinX;
                        }

                        if (curMaxX > maxX)
                        {
                            maxX = curMaxX;
                        }

                        if (curMinY < minY)
                        {
                            minY = curMinY;
                        }

                        if (curMaxY > maxY)
                        {
                            maxY = curMaxY;
                        }
                    }
                    int[] link = new int[] { 0, 0, 0 };
                    for (int j = 0; j < triNum; j++)
                    {
                        //有两个点相交即为邻接
                        if (j == i)
                            continue;
                        Triange temptr = trianges[j];
                        int linkIndex = tr.getPublicEdge(temptr);
                        if (linkIndex == 0)
                        {
                            continue;
                        }
                        link[linkIndex - 1] = j + 1;
                    }
                    bw.Write(link[0]);
                    bw.Write(link[1]);
                    bw.Write(link[2]);
                }

                bw.Write(minX);
                bw.Write(minY);
                bw.Write(maxX);
                bw.Write(maxY);
            }
            bw.Close();     
        }

        /// <summary>
        /// 合并多个相互嵌套的多边形 暂时不能处理相交的多边形
        /// </summary>
        /// <param name="polyList"></param>
        /// <returns></returns>
        private static ArrayList unionPolygon(ArrayList polyList)
        {                   
			//检查任意两个多边形之间的关系
            int[] father = new int[polyList.Count];
            ArrayList[] fatherList = new ArrayList[polyList.Count];      
            ArrayList[] childList = new ArrayList[polyList.Count];
            for (int i = 0; i < polyList.Count; i++)
            {
                fatherList[i] = new ArrayList();
                childList[i] = new ArrayList();

            }
            //如果父亲数为偶数个 则分离 即删除所有的父亲 否则 保留父亲最多的父亲为真正的父亲 
            for (int i = 0; i < polyList.Count-1; i++)
            {
                Point[] p1 = (Point[])polyList[i];
                for (int j = i + 1; j < polyList.Count; j++)
                {
                    Point[] p2 = (Point[])polyList[j];
                    int check = checkPolygonRelation(p1,p2);
                    if (check == 0)
                        continue;

                    if (check == 1)   //p1 包含 p2
                    {
                        fatherList[j].Add(i);
                    }
                    else             //p2 包含 p1
                    {
                        fatherList[i].Add(j);         
                    }
                }
            }

            for(int i = 0; i < polyList.Count; i++)
            {
                father[i] = fatherList[i].Count;
            } 
            for(int i = 0; i < polyList.Count; i++)
            {
                ArrayList myfather = fatherList[i];
                int realFather = -1;
                int maxFatherCount = -1;
                if (father[i] % 2 != 0)
                {
                    for (int j = 0; j < myfather.Count; j++)
                    {
                        int curFather = (int)myfather[j];
                        if (father[curFather] > maxFatherCount)
                        {
                            realFather = curFather;
                        }
                    }
                }
                fatherList[i].Clear();
                if (realFather != -1)
                {                
                    fatherList[i].Add(realFather);
                    childList[realFather].Add(i);
                }
            }
            //合并子节点不会空的多边形
            ArrayList resultList = new ArrayList();
            for (int i = 0; i < polyList.Count;i++)
            {
                ArrayList curResultList = new ArrayList();
                if (childList[i].Count == 0)
                {
                    if (fatherList[i].Count == 0)
                        curResultList.Add(polyList[i]);               
                }
                else
                {
                    ArrayList childPointList = new ArrayList();
                    foreach (Object childIndex in childList[i])
                    {
                        childPointList.Add(polyList[(int)childIndex]);
                    }
                    curResultList = unionPolygon((Point[])polyList[i],childPointList);
                }
                if (curResultList.Count > 0)
                    resultList.Add(curResultList);
            }
            return resultList;
        }

        /// <summary>
        /// 合并父子多边形
        /// </summary>
        /// <param name="father"></param>
        /// <param name="childList"></param>
        /// <returns></returns>
        private static ArrayList unionPolygon(Point[] father, ArrayList childList)
        {
            ArrayList result = new ArrayList();
            if (childList.Count == 0)
            {
                result.Add(father);
            }
            else
            {
                //a->b->d->c->a
                ArrayList p1 = new ArrayList();
                //a->c->d->b->a
                ArrayList p2 = new ArrayList();
                bool findit = false;
                //找任意子节点  和父节点连线 将父节点分割为两个多边形
                for( int loop = 0;loop < childList.Count;loop++)
                {
                    Point[] child = (Point[])childList[loop];
                    Point a;
                    Point b;
                    Point c;
                    Point d;
                    int i=0, j=0,k=0,l=0;                
                    for (i = 0; i < child.Length; i++)
                    {
                        a = child[i];
                        for (j = i+1; j <child.Length; j++)
                        {
                            b = child[j];
                            for (k = 0; k < father.Length; k++)
                            {
                                c = father[k];
                                if (lineIntersectPolygon(father, a, c))
                                    continue;
                                for (l = k+1; l <father.Length; l++)
                                {
                                    d = father[l];

                                    //与每个边检测相交关系
                                    if ((!lineIntersectPolygon(father, b, d)) && (!checkLineIntersect(a,c,b,d)))
                                    {
                                        int m;
                                        for (m = 0; m < childList.Count; m++)
                                        {
                                            if (lineIntersectPolygon((Point[])childList[m], a, c) ||
                                                lineIntersectPolygon((Point[])childList[m], b, d))
                                                break;
                                        }
                                        if (m == childList.Count)
                                        {
                                            findit = true;

                                        }
                                    }
                                    if (findit)
                                        break;
                                }
                                if (findit)
                                    break;
                            }
                            if (findit)
                                break;
                        }
                        if (findit)
                            break;
                    }
                    if (findit)
                    { 
                        for(int loop1 = k;loop1<=l;loop1++)
                        {
                            p1.Add(father[loop1]);
                        }
                        for (int loop1 = j; loop1 >= i; loop1--)
                        {
                            p1.Add(child[loop1]);
                        }
                        /*
                        for (int loop1 = 0; loop1 <= i; loop1++)
                        {
                            p1.Add(child[loop1]);
                        }
                         */

                        for (int loop1 = i; loop1 >= 0; loop1--)
                        {
                             p2.Add(child[loop1]);
                        }
                        for (int loop1 = child.Length-1; loop1 >= j; loop1--)
                        {
                            p2.Add(child[loop1]);
                        }

                        for (int loop1 = l; loop1 < father.Length; loop1++)
                        {
                            p2.Add(father[loop1]);
                        }

                        for (int loop1 = 0; loop1 <= k; loop1++)
                        {
                            p2.Add(father[loop1]);
                        }

                        childList.RemoveAt(loop);//删除被分解的子多边形
                        break;
                    }
                }
                if (!findit)
                    throw new Exception("unionPolygon no find");
                //判断两个新多变形和父节点的关系   
                ArrayList child1 = new ArrayList();
                ArrayList child2 = new ArrayList();
                Point [] newp1 = CheckPolygon((Point [])p1.ToArray(typeof(Point)));
                Point [] newp2 = CheckPolygon((Point [])p2.ToArray(typeof(Point)));
                for(int loop = 0;loop<childList.Count;loop++)
                {
                    if (checkPolygonRelation(newp1, (Point[])childList[loop]) == 1)
                    {
                        child1.Add(childList[loop]);
                    }
                    else
                    {
                        child2.Add(childList[loop]);
                    }
                }

                //递归
                ArrayList result1 = unionPolygon(newp1, child1);
                ArrayList result2 = unionPolygon(newp2, child2);

                foreach (Object obj in result1)
                {
                    result.Add(obj);
                }

                foreach (Object obj in result2)
                {
                    result.Add(obj);
                }

            }
            return result;
        }
        /// <summary>
        /// 两个多边形的关系 
        /// </summary>
        /// <param name="poly1"></param>
        /// <param name="poly2"></param>
        /// <returns>0 没关系 1 1包含2  2 2包含1</returns>
        private static int checkPolygonRelation(Point[] poly1, Point[] poly2)
        {
            Rectangle r1 = getRect(poly1);
            Rectangle r2 = getRect(poly2);
            if (r1.Contains(r2))        //r1 > r2
            {
                //判断r2中的任意一个点是否在多边形中
                for (int i = 0; i < poly2.Length; i++)
                {
                    if (!pointInPolygon(poly1, poly2[i]))
                    {
                        return 0;
                    }
                }
                return 1;
            }
            else if (r2.Contains(r1))  //r2 > r1
            {
                for (int i = 0; i < poly1.Length; i++)
                {
                    if (!pointInPolygon(poly2, poly1[i]))
                    {
                        return 0;
                    }
                }
                return 2;
            }
            else
            {
                return 0;
            }
        }
        /// <summary>
        /// 点和多边形的关系
        /// </summary>
        /// <param name="poly"></param>
        /// <param name="p"></param>
        /// <returns></returns>
        private static bool pointInPolygon(Point[] poly, Point p)
        {
            Point[] polyClone = new Point[poly.Length + 1];
            for (int i = 0; i < poly.Length; i++)
            {
                polyClone[i].X = poly[i].X - p.X;
                polyClone[i].Y = poly[i].Y - p.Y;
            }
            polyClone[poly.Length].X = poly[0].X - p.X;
            polyClone[poly.Length].Y = poly[0].Y - p.Y;
            int t1 = polyClone[0].X >= 0 ? (polyClone[0].Y >= 0 ? 0 : 3) : (polyClone[0].Y >= 0 ? 1: 2);
            int sum = 0,index = 1,f = 0,t2 = 0;
            for (sum = 0, index = 1; index < polyClone.Length; index++)
            {
                if (polyClone[index].X == 0 && polyClone[index].Y == 0)
                {
                    return true;
                }
                f = polyClone[index].Y * polyClone[index - 1].X - polyClone[index].X * polyClone[index - 1].Y;
                if (f == 0 && polyClone[index].X * polyClone[index - 1].X <= 0 && polyClone[index].Y * polyClone[index - 1].Y <= 0)
                {
                    return true;
                }
                t2 = polyClone[index].X >= 0 ? (polyClone[index].Y >= 0 ? 0 : 3) : (polyClone[index].Y >= 0 ? 1 : 2);
                if (t2 == (t1 + 1) % 4)
                    sum += 1;
                else if(t2 == (t1 + 3) % 4)
                    sum -= 1;
                else if (t2 == (t1 + 2) % 4)
                {
                    if (f > 0)
                        sum += 2;
                    else
                        sum -= 2;
                }
                t1 = t2;
            }
            if (sum != 0)
            {
                return true;
            }

            return false;
        }

        /// <summary>
        /// 线和多边形的相交关系
        /// </summary>
        /// <param name="poly"></param>
        /// <param name="a"></param>
        /// <param name="b"></param>
        /// <returns></returns>
        private static bool lineIntersectPolygon(Point [] poly,Point a,Point b)
        {
            for (int i = 0; i < poly.Length; i++)
            {
                Point pa = poly[i];
                Point pb = poly[(i + 1) % poly.Length];
                if (checkLineIntersect(pa, pb, a, b))
                {
                    /*System.Diagnostics.Trace.WriteLine("{" + pa.X.ToString() + "," + pa.Y.ToString() + "} " +
                                                       "{" + pb.X.ToString() + "," + pb.Y.ToString() + "} " +
                                                       "{" + a.X.ToString() + "," + a.Y.ToString() + "} " +
                                                       "{" + b.X.ToString() + "," + b.Y.ToString() + "} true");*/
                    return true;
                }
                else
                {
                  /*  System.Diagnostics.Trace.WriteLine("{" + pa.X.ToString() + "," + pa.Y.ToString() + "} " +
                                                       "{" + pb.X.ToString() + "," + pb.Y.ToString() + "} " +
                                                       "{" + a.X.ToString() + "," + a.Y.ToString() + "} " +
                                                       "{" + b.X.ToString() + "," + b.Y.ToString() + "} false");*/
                }
            }       
            return false;
        }

        /// <summary>
        /// 判断线段相交
        /// </summary>
        /// <param name="a"></param>
        /// <param name="b"></param>
        /// <param name="c"></param>
        /// <param name="d"></param>
        /// 顶点相交且不共线 不算   共线算相交
        /// <returns></returns>
        public static bool checkLineIntersect(Point a, Point b, Point c, Point d)
        {
            
            if (a.Equals(c) || a.Equals(d) || b.Equals(c) || b.Equals(d))
                return false;
            if ((Math.Max(a.X, b.X) >= Math.Min(c.X, d.X)) &&//快速排斥
                (Math.Max(c.X, d.X) >= Math.Min(a.X, b.X)) &&
                (Math.Max(a.Y, b.Y) >= Math.Min(c.Y, d.Y)) &&
                (Math.Max(c.Y, d.Y) >= Math.Min(a.Y, b.Y)) &&
                ((multiply(c, b, a) * multiply(b, d, a)) >= 0) &&  //跨立
                ((multiply(a, d,c) * multiply(d, b, c)) >= 0)
                )
                return true;
            return false;
        }
        /// <summary>
        /// 矢量叉乘
        /// </summary>
        /// <param name="sp"></param>
        /// <param name="ep"></param>
        /// <param name="op"></param>
        /// <returns></returns>
        private static double multiply(Point sp,Point ep,Point op) 
        {
            return((sp.X-op.X)*(ep.Y-op.Y)-(ep.X-op.X)*(sp.Y-op.Y));   
        }   

        /// <summary>
        /// 求多边形外围矩形
        /// </summary>
        /// <param name="p"></param>
        /// <returns></returns>
        private static Rectangle getRect(Point[] p)
        {
            int maxX = p[0].X;
            int maxY = p[0].Y;
            int minX = p[0].X;
            int minY = p[0].Y;
            for(int i = 1;i<p.Length;i++)
            {
                int curX = p[i].X;
                int curY = p[i].Y;
                if (maxX < curX)
                {
                    maxX = curX;
                }
                else if (minX > curX)
                {
                    minX = curX;
                }

                if (maxY < curY)
                {
                    maxY = curY;
                }
                else if (minY > curY)
                {
                    minY = curY;
                }
            }
            return new Rectangle(minX, minY, (maxX - minX), (maxY - minY));
        }
        private static ArrayList ProcessLoop(ArrayList plist)
        {
            ArrayList res = new ArrayList();
            if (plist.Count == 3)
            {
                Triange tr = new Triange((Point)plist[0], (Point)plist[1], (Point)plist[2]);
                res.Add(tr);
            }
            else
            {
               //找到凸点
                int minY = ((Point)plist[0]).Y;
                int minIndex = 0;
                for (int i = 1; i < plist.Count; i++)
                {
                    Point _p = (Point)plist[i];
                    if (minY > _p.Y)
                    {
                        minY = _p.Y;
                        minIndex = i;
                    }
                    else if (minY == _p.Y)
                    {
                        if (((Point)plist[minIndex]).X > _p.X)
                        {
                            minIndex = i;
                        }
                    }
                }
                int lastIndex = (plist.Count + minIndex - 1) % plist.Count;
                int nextIndex = (minIndex + 1) % plist.Count;
                Point p1 = (Point)plist[lastIndex];
                Point p2 = (Point)plist[minIndex];
                Point p3 = (Point)plist[nextIndex];
                Triange tr = new Triange(p1,p2,p3);
                ArrayList insidePoint = new ArrayList();
                //检查在三角形内的点
                for(int index = 0;index < plist.Count;index++)
                {
                    Point _p = (Point)plist[index];
                    if (tr.CheckPointInSide(_p))
                    {
                        insidePoint.Add(index);
                    }
                }
                if (insidePoint.Count > 0)
                {
                    //寻找离 线段p1p3最远的点p 连接p2和p 将多边形切割为两个
                    int maxPointIndex = (int)insidePoint[0];
                    double maxDistance = distance((Point)plist[maxPointIndex], p1, p3);

                    for (int i = 1; i < insidePoint.Count; i++)
                    {
                        int curIndex = (int)insidePoint[i];
                        double curDistance = distance((Point)plist[curIndex], p1, p3);
                        if (curDistance > maxDistance)
                        {
                            maxDistance = curDistance;
                            maxPointIndex = curIndex;
                        }
                    }

                    //分割多边形
                    ArrayList leftP = new ArrayList();      // minIndex -> maxPointIndex
                    ArrayList rightP = new ArrayList();     // maxPointIndex -> minIndex

                    int loop = minIndex;
                    while (true)
                    {
                        leftP.Add(plist[loop]);
                        if (loop == maxPointIndex)
                        {
                            break;
                        }
                        loop++;
                        if (loop >= plist.Count)
                        {
                            loop = 0;
                        }
                    }
                    loop = maxPointIndex;
                    while (true)
                    {
                        rightP.Add(plist[loop]);
                        if (loop == minIndex)
                        {
                            break;
                        }
                        loop++;
                        if (loop >= plist.Count)
                        {
                            loop = 0;
                        }
                    }
                    ArrayList otherRes1 = ProcessLoop(leftP);
                    ArrayList otherRes2 = ProcessLoop(rightP);
                    res.AddRange(otherRes1);
                    res.AddRange(otherRes2);
                }
                else
                {
                    //去除顶点
                    res.Add(tr);
                    ArrayList newPList = (ArrayList)plist.Clone();
                    newPList.RemoveAt(minIndex);
                    ArrayList otherRes = ProcessLoop(newPList);
                    res.AddRange(otherRes);
                }

            }
            return res;
        }

        private static Point[] CheckPolygon(ArrayList pointList)
        {
            return CheckPolygon((Point[])pointList.ToArray(typeof(Point)));
        }

        /// <summary>
        /// 将多边形点序列转换为顺时针顺序  并过滤掉共线的点（端点除外）
        /// </summary>
        /// <param name="points"></param>
        /// <returns></returns>
        private static Point[] CheckPolygon(Point[] points)
        {
            //排除共线点
            ArrayList plist = new ArrayList();
            for (int i = 0; i < points.Length; i++)
            {
                Point p = points[i];
                if (plist.Count >= 2)
                {
                    Point p1 = (Point)plist[plist.Count - 2];
                    Point p2 = (Point)plist[plist.Count - 1];
                    if (Util.CheckOnline(p1,p2,p))
                    {
                        plist.RemoveAt(plist.Count - 1);
                    }
                }            
                plist.Add(p);
            }
            //找最低点
            int minY = ((Point)plist[0]).Y;
            int minIndex = 0;
            for(int i = 1;i<plist.Count;i++)
            {
                Point p = (Point)plist[i];
                if (minY > p.Y)
                {
                    minY = p.Y;
                    minIndex = i;
                }
                else if (minY == p.Y)
                {
                    if (((Point)plist[minIndex]).X > p.X)
                    {
                        minIndex = i;
                    }
                }
            }
            int lastIndex = (plist.Count + minIndex - 1) % plist.Count;
            int nextIndex = (minIndex + 1) % plist.Count;

            // last -> min -> next  必须是顺时针
            if (Util.CheckClockWise((Point)plist[lastIndex], (Point)plist[minIndex], (Point)plist[nextIndex]))
            {
                plist.Reverse();
            }
            
            return (Point[])plist.ToArray(typeof(Point));
                  
        }

        /// <summary>
        /// 点p到线段p1 p2的距离
        /// </summary>
        /// <param name="p"></param>
        /// <param name="p1"></param>
        /// <param name="p2"></param>
        /// <returns></returns>
        private static double distance(Point p, Point p1, Point p2)
        {
            return Math.Abs((p2.Y - p1.Y) * p.X + (p1.X - p2.X) * p.Y + ((p2.X * p1.Y) - (p1.X * p2.Y))) / (Math.Sqrt((p2.Y - p1.Y)*(p2.Y - p1.Y) + (p1.X - p2.X)*(p1.X - p2.X)));
        }
    }
}

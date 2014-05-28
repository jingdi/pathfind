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

namespace PreNavMash
{
    public class Util
    {
        /// <summary>
        /// 判断last-> mid ->next 是否为顺时针方向
        /// </summary>
        /// <param name="last"></param>
        /// <param name="next"></param>
        /// <param name="mid"></param>
        /// <returns></returns>
        public static bool CheckClockWise(Point last, Point mid, Point next)
        {
            return multiply(last, mid, next) < 0;
        }

        /// <summary>
        /// 判断last-> mid ->next 是否为逆时针方向
        /// </summary>
        /// <param name="last"></param>
        /// <param name="mid"></param>
        /// <param name="next"></param>
        /// <returns></returns>
        public static bool CheckCounterClockWise(Point last, Point mid, Point next)
        {
            return multiply(last, mid, next) > 0;
        }

        /// <summary>
        /// 判断三点是否共线
        /// </summary>
        /// <param name="last"></param>
        /// <param name="mid"></param>
        /// <param name="next"></param>
        /// <returns></returns>
        public static bool CheckOnline(Point last, Point mid, Point next)
        {
            return multiply(last, mid, next) == 0;
        }
        /// <summary>
        /// 由三点(last->mid * mid->next)顺序组成的矢量叉积
        /// </summary>
        /// <param name="last"></param>
        /// <param name="next"></param>
        /// <param name="mid"></param>
        /// <returns></returns>
        public static int multiply(Point last, Point mid, Point next)
        {
            return ((mid.X - last.X) * (next.Y - mid.Y) - (next.X - mid.X) * (mid.Y - last.Y));
        }

        /// <summary>
        /// 矢量 p1-> p2   p3->p4的叉积
        /// </summary>
        /// <param name="p1"></param>
        /// <param name="p2"></param>
        /// <param name="p3"></param>
        /// <param name="p4"></param>
        /// <returns></returns>
        public static int multiply(Point p1, Point p2, Point p3, Point p4)
        {
            return ((p2.X - p1.X)*(p4.Y - p3.Y) - (p4.X - p3.X)*(p2.Y - p1.Y)); 
        }
    }
}

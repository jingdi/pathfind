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

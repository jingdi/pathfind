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
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Collections;
using System.IO;
using System.Threading;

namespace PreNavMash
{
    public partial class Form1 : Form
    {
        private ArrayList pointList = new ArrayList();		//所有点集 Vector
        private ArrayList drawVector = new ArrayList();

        private ArrayList triList = new ArrayList();

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Paint(object sender, PaintEventArgs e)
        {
           
        }

        private void Form1_SizeChanged(object sender, EventArgs e)
        {
            //redraw();
        }

        //清除
        private void clearToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.drawVector.Clear();
            this.pointList.Clear();
            this.panel1.CreateGraphics().Clear(Color.White);
           // this.polygonV.Add(createFirstPolygon());
        }
        //生成三角形网格
        private void createToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Graphics g = this.panel1.CreateGraphics();
           // g.Clear(Color.White);
            Pen pLine = new Pen(Color.Red, 2);
            Pen pPoint = new Pen(Color.Black, 2);
            ArrayList newpointList = new ArrayList();

            Brush brush = new SolidBrush(Color.LawnGreen);
            Font font = new Font("楷体GB-2312", 10, FontStyle.Bold);
        /*   foreach (ArrayList list in pointList)
            {
                newpointList.AddRange(list);
            }
         */
            triList.Clear();
            //ArrayList newpointList = Triangulation.unionPolygon(pointList);
            triList = Triangulation.Process(pointList);
            drawTri();
           /*
            for (int i = 0; i < triList.Count; i++)
            {
                ArrayList polyGroup = (ArrayList)triList[i];
                ArrayList _result = new ArrayList();
                foreach (Object obj in polyGroup)
                {
                    Point[] p = (Point[])obj;
                    for (int j = 0; j < p.Length - 1; j++)
                    {
                        g.DrawString(("(" + p[j].X.ToString() + "," + p[j].Y.ToString() + ")"), font, brush, new PointF((float)p[j].X, (float)p[j].Y));
                        g.DrawLine(pLine,p[j],p[j+1]);
                    }
                    g.DrawLine(pLine, p[p.Length - 1], p[0]);
                }
            }
            */
            
        }

        private void drawTri()
        {
            Graphics g = this.panel1.CreateGraphics();
            g.Clear(Color.White);
            Pen pLine = new Pen(Color.Red, 2);
            Pen pPoint = new Pen(Color.Black, 2);
            foreach (ArrayList item in triList)
            {
                foreach (Triange tr in item)
                {
                    //triList.Add(tr);
                    tr.draw(g, pLine);
                }
            }
        }

        //点击 面板
        private void panel1_MouseClick(object sender, MouseEventArgs e)
        {
            int x = e.X;
            int y = e.Y;
            Point cur = new Point(x, y);
            int lastVectorNum = drawVector.Count;
            Graphics g = this.panel1.CreateGraphics();
            Pen pLine = new Pen(Color.Black, 2);
            Brush brush = new SolidBrush(Color.LawnGreen);
            Font font = new Font("楷体GB-2312", 10, FontStyle.Bold);  
            //Pen pPoint = new Pen(Color.Black, 2);
            if (lastVectorNum > 0)
            {
                Point last = (Point)drawVector[lastVectorNum - 1];
                 if (lastVectorNum > 1)
                 {
                    Point first = (Point)drawVector[0];
                    if(Math.Abs(first.X - x) < 10 && Math.Abs(first.Y - y) < 10)
                    {
                         g.DrawLine(pLine, last,first);
                         this.pointList.Add(drawVector.Clone());
                         this.drawVector.Clear();
                         return ;
                    }               
                }
                g.DrawLine(pLine, last, new Point(x, y));
            }

            g.DrawString(("(" + x.ToString() + "," + y.ToString() + ")"), font, brush, new PointF((float)x, (float)y));
            drawVector.Add(cur);    
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog folderBrowserDialog1 = new FolderBrowserDialog();
            if (folderBrowserDialog1.ShowDialog() == DialogResult.OK)
            {
                string filePath = folderBrowserDialog1.SelectedPath;
                Triangulation.saveData(triList, filePath);
            }
        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            drawTri();
            Graphics g = this.panel1.CreateGraphics();
            string realPathName = "e://navmesh.result";
            FileStream fs = new FileStream(realPathName, FileMode.Open);
            BinaryReader br = new BinaryReader(fs, Encoding.Default);
           
            Pen pLine = new Pen(Color.Green, 1);
            while(true)
            {
                try
                {
                    int length = br.ReadInt32();
                    int lastX = br.ReadInt32();
                    int lastY = br.ReadInt32();
                    for (int i = 0; i < length - 1; i++)
                    {
                        int x = br.ReadInt32();
                        int y = br.ReadInt32();
                        g.DrawLine(pLine, new Point(lastX, lastY), new Point(x, y));
                        lastX = x;
                        lastY = y;
                        Thread.Sleep(10);
                    }                                
                }
                catch (Exception ex)
                {
                    break;
                }
            }
            fs.Close();
        }

        private void testToolStripMenuItem_Click(object sender, EventArgs e)
        {
            //{177,523},{813,524},{629,562},{512,400}
            Point p1 = new Point(177, 523);
            Point p2 = new Point(813, 524);
            Point p3 = new Point(629, 562);
            Point p4 = new Point(512, 400);
            bool result  = Triangulation.checkLineIntersect(p1,p2,p3,p4);
            System.Diagnostics.Trace.WriteLine(result.ToString());
        }

    }
}

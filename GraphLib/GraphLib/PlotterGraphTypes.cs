using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;
using System.ComponentModel;
using System.Drawing;

/* Copyright (c) 2008-2014 DI Zimmermann Stephan (stefan.zimmermann@tele2.at)
 *   
 * Permission is hereby granted, free of charge, to any person obtaining a copy 
 * of this software and associated documentation files (the "Software"), to 
 * deal in the Software without restriction, including without limitation the 
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or 
 * sell copies of the Software, and to permit persons to whom the Software is 
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software. 
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
 * THE SOFTWARE.
 */

namespace GraphLib
{
    public struct cPoint
    {
        public float x;
        public float y;

        public cPoint(float x, float y)
        {
            this.x = x;
            this.y = y;
        }
    }

    public class DataSource  
    {
        public delegate String OnDrawXAxisLabelEvent(DataSource src, int idx);
        public delegate String OnDrawYAxisLabelEvent(DataSource src, float value);

        public OnDrawXAxisLabelEvent OnRenderXAxisLabel = null;
        public OnDrawYAxisLabelEvent OnRenderYAxisLabel = null;

        private List<cPoint> samples = null;
      
        private String name = String.Empty;
        private int downSample = 1;
        private Color color = Color.Black;
        private float highestValue = 0, lowestValue = 999;

        public float VisibleDataRange_X = 0;
        public float DY = 0;      
        public float YD0 = -200;
        public float YD1 = 200;
        public float Cur_YD0 = -200;
        public float Cur_YD1 = 200;

        public float grid_distance_y = 200;       // grid distance in units ( draw a horizontal line every 200 units )       

        public float off_Y = 0;
        public float grid_off_y = 0;
        
        public bool yFlip = true;      
        
        public bool Active = true;
        
        private bool YAutoScaleGraph = false;
        
        private bool XAutoScaleGraph = false;
        
        public float XAutoScaleOffset = 100;
        
        public float CurGraphHeight = 1.0f;
        
        public float CurGraphWidth = 1.0f;

        public float HighestValue
        {
            get
            {
                return highestValue;
            }
            set
            {
                highestValue = value;
            }
        }
        public float LowestValue
        {
            get
            {
                return lowestValue;
            }
            set
            {
                lowestValue = value;
            }
        }
        public bool AutoScaleY
        {
            get
            {
                return YAutoScaleGraph;
            }
            set
            {
                YAutoScaleGraph = value;
            }
        }

        public bool AutoScaleX
        {
            get
            {
                return XAutoScaleGraph;
            }
            set
            {
                XAutoScaleGraph = value;
            }
        }

        public List<cPoint> Samples
        {
            get 
            {
                return samples; 
            }
            set 
            {
                samples = new List<cPoint>();
            }
        }

        public float  XMin
        {
            get
            {
                float x_min = float.MaxValue;
                if (samples.Count > 0)
                {
                    foreach (cPoint p in samples)
                    {
                        if (p.x < x_min)  x_min=p.x;
                    }
                }
                return x_min;
            }
        }

        public float XMax
        {
            get
            {
                float x_max = float.MinValue;
                if (samples.Count > 0)
                {
                    foreach (cPoint p in samples)
                    {
                        if (p.x > x_max) x_max = p.x;
                    }
                }
                return x_max;
            }
        }

        public float YMin
        {
            get
            {
                float y_min = float.MaxValue;
                if (samples.Count > 0)
                {
                    foreach (cPoint p in samples)
                    {
                        if (p.y < y_min) y_min = p.y;
                    }
                }
                return y_min;
            }
        }

        public float YMax
        {
            get
            {
                float y_max = float.MinValue;
                if (samples.Count > 0)
                {
                    foreach (cPoint p in samples)
                    {
                        if (p.y > y_max) y_max = p.y;
                    }
                }
                return y_max;
            }
        }

        public void findExtremes(float value)
        {
            if (value >= highestValue)
                highestValue = value;
            else
                lowestValue = value;
        }

        public void SetDisplayRangeY(float y_start, float y_end)
        {            
            YD0 = y_start;
            YD1 = y_end;
        }
        public PointF GetDisplayRangeY()
        {
            return new PointF(YD0, YD1);
        }
         
        public void SetGridDistanceY(  float grid_dist_y_units)
        {
            grid_distance_y = grid_dist_y_units;
        }

        public void SetGridOriginY(  float off_y)
        {           
            grid_off_y = off_y;
        }
      
        [Category("Properties")] // Take this out, and you will soon have problems with serialization;
        [DefaultValue(typeof(string), "")]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        public String Name
        {
            get { return name; }
            set { name = value; }
        }

        [Category("Properties")] // Take this out, and you will soon have problems with serialization;
        [DefaultValue(typeof(Color), "")]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        public Color GraphColor
        {
            get { return color; }
            set { color = value; }
        }

        [Category("Properties")] // Take this out, and you will soon have problems with serialization;
        [DefaultValue(typeof(int), "0")]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        public int Length
        {
            get { return samples.Count; }
        }
        
        [Category("Properties")] // Take this out, and you will soon have problems with serialization;
        [DefaultValue(typeof(int), "1")]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        public int Downsampling
        {
            get { return downSample; }
            set { downSample = value; }
        }

    } 
}

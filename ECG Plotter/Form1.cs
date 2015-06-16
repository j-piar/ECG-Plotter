// Form1.cs
using System;
using System.Collections.Generic;
using System.IO;
using System.Xml;
using System.Linq;
using System.Windows.Forms;
using GraphLib;
using System.Drawing;

#region Copyright (©) 2015 Juraj Piar
/**
 * Copyright (©) 2015 Juraj Piar (juraj.piar@hotmail.com)
 * http://github.com/j-piar/ECG-Plotter
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
 *
 **/
#endregion
namespace ECG_Plotter
{
    /// <summary>
    /// Simple plotting of ECG signals from an XML file.
    /// <remarks>Using "A simple C# library for graph plotting" library(http://goo.gl/nYsWTZ)</remarks>
    /// </summary>
    public partial class MainForm : Form
    {
        #region MEMBERS
        private XmlDocument doc = null;
        DataSource ds = null;
        private int tInc;
        #endregion

        #region CONSTRUCTORS
        public MainForm()
        {
            InitializeComponent();
        }
        #endregion

        #region EVENT FUNCTIONS
        /// <summary>
        /// Open button event function - executes process of loading and displaying XML data
        /// </summary>
        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                openFileDialogIn.CheckFileExists = true;
                var openDialog = openFileDialogIn.ShowDialog();

                if (openDialog == DialogResult.OK)
                {
                    var path = openFileDialogIn.FileName;
                    var fileReader = new FileStream(path, FileMode.Open, FileAccess.Read, FileShare.ReadWrite);
                    doc = new XmlDocument();
                    doc.Load(fileReader);
                }
            }
            catch (Exception exc)
            {
                MessageBox.Show(String.Format("An error occured. Original message: {0}", exc.Message));
            }
            try
            {
                if (!doc.Equals(null))
                {
                    toolStripDropDownButton_dMode.Enabled = true;
                    processData();
                }
            }
            catch (NullReferenceException nullE)
            {
                MessageBox.Show("No file selected!");
            }
        }

        /// <summary>
        /// Exit button event function
        /// </summary>
        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        /// <summary>
        /// Event function to zoom the plot in/out 
        /// </summary>
        private void display_MouseWheel(object sender, MouseEventArgs e)
        {
            int deltaFix, deltaCount;
            float pluginNumber;
            float zDetail;
            PointF CurRangeY, CurRangeX;
            
            deltaCount = 5; //max zoom -/+ level
            zDetail = (float)0.5; //number by which Y is zoomed

            // Fix delta to 120
            if (e.Delta < -120)
            {
                deltaFix = -120;
            }
            else
            {
                if (e.Delta > 120)
                {
                    deltaFix = 120;
                }
                else
                {
                    deltaFix = e.Delta;
                }
            }
            pluginNumber = deltaFix < 0 ? (float)-zDetail : (float)zDetail; 
            tInc += deltaFix;

            if ((tInc < 0 ? -tInc : tInc / 2) <= deltaCount * 120)
            {
                foreach (DataSource source in display.DataSources)
                {
                    CurRangeY = source.GetDisplayRangeY();
                    source.SetDisplayRangeY(CurRangeY.X - pluginNumber, CurRangeY.Y + pluginNumber);
                }
                CurRangeX = display.GetDisplayRangeX();
                display.SetDisplayRangeX(CurRangeX.X, CurRangeX.Y + deltaFix);
            }
            else
            {
                tInc -= deltaFix;
            }
        }

        /// <summary>
        /// Set display to a single view
        /// </summary>
        private void toolStripMenuItem_single_Click(object sender, EventArgs e)
        {
            display.PanelLayout = PlotterGraphPaneEx.LayoutMode.NORMAL;
            foreach (DataSource s in display.DataSources)
            {
                s.AutoScaleY = true;
            }
            checkBox1.Checked = true;
        }

        /// <summary>
        /// Set display to dual view
        /// </summary>
        private void toolStripMenuItem_dual_Click(object sender, EventArgs e)
        {
            display.PanelLayout = PlotterGraphPaneEx.LayoutMode.STACKED;
            foreach (DataSource s in display.DataSources)
            {
                s.AutoScaleY = false;
            }
            checkBox1.Checked = false;
        }
        
        /// <summary>
        /// Toggle Auto scale mode
        /// </summary>
        private void checkBox1_Click(object sender, EventArgs e)
        {
            switch (checkBox1.CheckState)
            {
                case CheckState.Unchecked:
                    foreach (DataSource s in display.DataSources)
                    {
                        if (s.AutoScaleY)
                        {
                            s.AutoScaleY = false;
                        }
                    }
                    break;
                case CheckState.Checked:
                    foreach (DataSource s in display.DataSources)
                    {
                        if (!s.AutoScaleY)
                        {
                            s.AutoScaleY = true;
                        }
                    }
                    break;
            }
        }

        /// <summary>
        /// Event function that sets values to draw as labels of the X axis
        /// </summary>
        /// <param name="s">Data source to get values from.</param>
        /// <param name="idx">Index number of current value</param>
        /// <returns></returns>
        private String RenderXLabel(DataSource s, int idx)
        {
            if (s.AutoScaleX)
            {
                var Value = (s.Samples[idx].x);
                return string.Empty + Value;
            }
            else
            {
                var Value = (s.Samples[idx].x) / 1000;
                var Label = string.Empty + Value + "\"";
                return Label;
            }
        }

        /// <summary>
        /// Event function that sets values to draw as labels of the Y axis
        /// </summary>
        /// <param name="s">Data source to get values from.</param>
        /// <param name="idx">Index number of current value</param>
        /// <returns></returns>
        private String RenderYLabel(DataSource s, float value)
        {
            return String.Format("{0:0}mV", value);
        }
        #endregion

        #region PRIVATE FUNCTIONS
        /// <summary>
        /// Setup PlotterGraph and extract data from the XML file
        /// </summary>
        private void processData()
        {
            SuspendLayout();
            display.DataSources.Clear();
            setDisplayToDefault();

            //load the dataPoints to display.DataSources
            calcData(PointDataType.RawData); 
            calcData(PointDataType.FilteredData);

            ResumeLayout();
            
            //Show Extremes
            textBox_highestRawData.Text = display.DataSources[0].HighestValue.ToString();
            textBox_lowestRawData.Text = display.DataSources[0].LowestValue.ToString();
            textBox_highestFilteredData.Text = display.DataSources[1].HighestValue.ToString();
            textBox_lowestFilteredData.Text = display.DataSources[1].LowestValue.ToString();
            display.Refresh();
        }

        /// <summary>
        /// Set colour to data plot
        /// </summary>
        /// <param name="i">Plot index number</param>
        /// <returns>Colour for plot</returns>
        private Color generateColour(int i)
        {
            var colours = new Color[] { Color.DimGray,
                Color.Black };
            return colours[i];
        }

        /// <summary>
        /// Loads and calculates plot data points from XML nodes
        /// </summary>
        /// <param name="type">decides what type of data is being loaded in</param>
        private void calcData(PointDataType type)
        {
            float x, y;
            display.DataSources.Add(new DataSource());
            ds = display.DataSources[(int)type];
            setAllGraphsToDeafault(ds, type);
            var fData = doc.GetElementsByTagName(type.ToString());
            var i = 0;
            foreach (XmlNode node_dataType in fData)
            {
                ds.Samples = new List<cPoint>();
                foreach (XmlNode node_dataPoint in node_dataType.ChildNodes)
                {
                    x = float.Parse(node_dataPoint.Attributes[0].Value);
                    y = (float)decimal.Parse(node_dataPoint.Attributes[1].Value, System.Globalization.NumberStyles.Float);
                    ds.Samples.Add(new cPoint(x * 1000, y * 1000));
                    ds.findExtremes(y);
                    i++;
                }
            }
        }

        /// <summary>
        /// Sets/Resets properties of data sources to their defaults
        /// </summary>
        /// <param name="ds">Data source to reset</param>
        /// <param name="type">Type of data to reset</param>
        private void setAllGraphsToDeafault(DataSource ds, PointDataType type)
        {
            ds.Name = type.ToString();
            ds.GraphColor = generateColour((int)type);
            ds.OnRenderXAxisLabel += RenderXLabel;
            ds.AutoScaleY = false;
            ds.SetDisplayRangeY((float)-3.5, (float)3.5);
            ds.SetGridOriginY(0);
            ds.SetGridDistanceY(1);
            ds.OnRenderYAxisLabel += RenderYLabel;
        }

        /// <summary>
        /// Sets/Resets graph plotter to its defaults
        /// </summary>
        private void setDisplayToDefault()
        {
            display.SetDisplayRangeX(0, 800);
            display.SetGridDistanceX(100);
            display.SetGridOriginX(0);
            display.Smoothing = System.Drawing.Drawing2D.SmoothingMode.None;
            display.PanelLayout = PlotterGraphPaneEx.LayoutMode.STACKED;

            display.BackgroundColorBot = Color.White;
            display.BackgroundColorTop = Color.PapayaWhip;
            display.SolidGridColor = Color.Red;
            display.DashedGridColor = Color.Red;
            display.ShowMovingGrid = false;
        }
        #endregion

        /// <summary>
        /// Enumeration of types of data points
        /// </summary>
        private enum PointDataType
        {
            RawData = 0,
            FilteredData = 1
        }
    }
}

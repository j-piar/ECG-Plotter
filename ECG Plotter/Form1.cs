using System;
using System.Collections.Generic;
using System.IO;
using System.Xml;
using System.Linq;
using System.Windows.Forms;
using GraphLib;
using System.Threading;
using System.Drawing;

namespace ECG_Plotter
{
    public partial class MainForm : Form
    {
        private XmlDocument doc = null;
        private object pad_lock = new object();

        public MainForm()
        {
            InitializeComponent();

        }

        /// <summary>
        /// Event function loading the file
        /// </summary>
        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                openFileDialogIn.CheckFileExists = true;
                DialogResult openDialog = openFileDialogIn.ShowDialog();

                if (openDialog == DialogResult.OK)
                {
                    string path = openFileDialogIn.FileName; // The Path to the .Xml file
                    FileStream fileReader = new FileStream(path, FileMode.Open, FileAccess.Read, FileShare.ReadWrite); //Set up the filestream (fileReader)
                    doc = new XmlDocument();
                    doc.Load(fileReader); //Load the data from the file into the XmlDocument
                }
            }
            catch (Exception exc)
            {
                MessageBox.Show(String.Format("An error occured. Original message: {0}", exc.Message));
            }
            try
            {
                if (!doc.Equals(null)) // if file loaded to XMLDocument doc
                {
                    this.toolStripDropDownButton_dMode.Enabled = true;
                    processData();
                }
            }
            catch (NullReferenceException nullE)
            {
                MessageBox.Show("No file selected!");
            }
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        /// <summary>
        /// Setup PlotterGraph and extract data from the XML file
        /// </summary>
        private void processData()
        {
            this.SuspendLayout();
            display.DataSources.Clear();
            setDisplayToDefault();

            calcData(PointDataType.RawData);
            calcData(PointDataType.FilteredData);
            //new Thread(() => calcData(PointDataType.RawData)).Start();
            //Thread.Sleep(100);
            //new Thread(() => calcData(PointDataType.FilteredData)).Start();
            this.ResumeLayout();

            this.textBox_highestRawData.Text = display.DataSources[0].HighestValue.ToString();
            this.textBox_lowestRawData.Text = display.DataSources[0].LowestValue.ToString();
            this.textBox_highestFilteredData.Text = display.DataSources[1].HighestValue.ToString();
            this.textBox_lowestFilteredData.Text = display.DataSources[1].LowestValue.ToString();
            display.Refresh();
        }

        private Color generateColour(int i)
        {
            Color[] colours = 
            {
                Color.DimGray,
                Color.Black,
            };
            return colours[i];
        }

        private void calcData(PointDataType type)
        {
            DataSource ds;
            float x, y;
            lock (pad_lock)
            {
                display.DataSources.Add(new DataSource());
                ds = display.DataSources[(int)type]; // set the default settings for the new graph
                setAllGraphsToDeafault(ds, type);
            }
            XmlNodeList fData = doc.GetElementsByTagName(type.ToString());
            int i = 0;
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
        delegate void SetTextCallback(string text);
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
        private enum PointDataType
        {
            RawData = 0,
            FilteredData = 1
        }
        private String RenderXLabel(DataSource s, int idx)
        {
            if (s.AutoScaleX)
            {
                float Value = (s.Samples[idx].x);
                return "" + Value;
            }
            else
            {
                float Value = (s.Samples[idx].x)/1000;
                String Label = "" + Value + "\"";
                return Label;
            }
        }

        private String RenderYLabel(DataSource s, float value)
        {
            return String.Format("{0:0}mV", value);
        }
        int tInc;

        /// <summary>
        /// Event function to zoom the plot in/out 
        /// </summary>
        private void display_MouseWheel(object sender, MouseEventArgs e)
        {

            int deltaFix, deltaCount;
            float pluginNumber;
            float zDetail;
            PointF CurRangeY, CurRangeX;

            switch (display.PanelLayout)
            {
                case PlotterGraphPaneEx.LayoutMode.NORMAL:
                    deltaCount = 5;
                    zDetail = (float)0.5;
                    break;
                case PlotterGraphPaneEx.LayoutMode.STACKED:
                    deltaCount = 5;
                    zDetail = (float)0.5;
                    break;
                default:
                    deltaCount = 5;
                    zDetail = (float)0.5;
                    break;
            }

            // fix mouse scroll to 1 notch
            if (e.Delta < -120)
                deltaFix = -120;
            else if (e.Delta > 120)
                deltaFix = 120;
            else
                deltaFix = e.Delta;
            pluginNumber = deltaFix < 0 ? (float)-zDetail : (float)zDetail;
            tInc += deltaFix;

            if ((tInc < 0 ? -tInc : tInc/2) <= deltaCount * 120)
            {
                Console.Out.WriteLine("---------------------------------");
                foreach (DataSource source in display.DataSources)
                {
                        CurRangeY = source.GetDisplayRangeY();
                        source.SetDisplayRangeY(CurRangeY.X - pluginNumber, CurRangeY.Y + pluginNumber);
                        Console.Out.WriteLine("Range Y: {0}", source.GetDisplayRangeY());
                }
                CurRangeX = display.GetDisplayRangeX();
                display.SetDisplayRangeX(CurRangeX.X, CurRangeX.Y + deltaFix);
                Console.Out.WriteLine("Range X: {0}", display.GetDisplayRangeX());
            }
            else
                tInc -= deltaFix;
        }

        private void toolStripMenuItem_single_Click(object sender, EventArgs e)
        {
            this.display.PanelLayout = PlotterGraphPaneEx.LayoutMode.NORMAL;
            foreach (DataSource s in display.DataSources)
            {
                s.AutoScaleY = true;
            }
            this.checkBox1.Checked = true;
        }

        private void toolStripMenuItem_dual_Click(object sender, EventArgs e)
        {
            this.display.PanelLayout = PlotterGraphPaneEx.LayoutMode.STACKED;
            foreach (DataSource s in display.DataSources)
            {
                s.AutoScaleY = false;
            }
            this.checkBox1.Checked = false;
        }

        private void button_autoScaleY_Click(object sender, EventArgs e)
        {
            
        }

        private void radioButton1_CheckedChanged(object sender, EventArgs e)
        {
            
        }

        private void checkBox1_Click(object sender, EventArgs e)
        {
            switch (this.checkBox1.CheckState)
            {
                case CheckState.Unchecked:
                    foreach (DataSource s in display.DataSources)
                    {
                        if (s.AutoScaleY)
                            s.AutoScaleY = false;
                    }
                    break;
                case CheckState.Checked:
                    foreach (DataSource s in display.DataSources)
                    {
                        if (!s.AutoScaleY)
                            s.AutoScaleY = true;
                    }
                    break;
            }
        }
    }
}

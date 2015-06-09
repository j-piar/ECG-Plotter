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

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            loadXML();
            try
            {
                if (!doc.Equals(null))
                {
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
        /// Loading XML file using openFileDialog
        /// </summary>
        private void loadXML()
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
            catch (Exception e)
            {
                MessageBox.Show(String.Format("An error occured. Original message: {0}", e.Message));
            }
        }

        /// <summary>
        /// Setup PlotterGraph and extract data from the XML file using separate threads for performance [O(n/2)]
        /// </summary>
        private void processData()
        {
            this.SuspendLayout();
            display.DataSources.Clear();
            display.SetDisplayRangeX(0, 400);
            display.SetGridDistanceX(50);
            display.SetGridOriginX(500);
            display.Smoothing = System.Drawing.Drawing2D.SmoothingMode.None;
            display.PanelLayout = PlotterGraphPaneEx.LayoutMode.STACKED;
            
            display.BackgroundColorBot = Color.White;
            display.BackgroundColorTop = Color.FromArgb(183, 183, 255);
            display.SolidGridColor = Color.Blue;
            display.DashedGridColor = Color.Blue;
            display.ShowMovingGrid = false;

            new Thread(() => calcData(PointDataType.RawData)).Start();
            //Thread.Sleep(100);
            new Thread(() => calcData(PointDataType.FilteredData)).Start();

            this.ResumeLayout();
            display.Refresh();
        }

        private Color colourChooser(int i)
        {
            Color[] colours = 
            {
                Color.DarkCyan,
                Color.DeepPink,
            };
            return colours[i];
        }

        private void calcData(PointDataType type)
        {
            DataSource ds;
            lock (pad_lock)
            {
                display.DataSources.Add(new DataSource());
            
                 ds = display.DataSources[(int)type];
                ds.Name = type.ToString();
                ds.GraphColor = colourChooser((int)type);
                ds.OnRenderXAxisLabel += RenderXLabel;
                ds.AutoScaleY = false;
                ds.SetDisplayRangeY(-2, 2);
                ds.SetGridOriginY(0);
                ds.SetGridDistanceY(1);
                ds.OnRenderYAxisLabel += RenderYLabel;
                ds.Length = 500000;
            }
            XmlNodeList fData = doc.GetElementsByTagName(type.ToString());
            int i = 0;
            foreach (XmlNode node_dataType in fData)
            {
                foreach (XmlNode node_dataPoint in node_dataType.ChildNodes)
                {
                    if (node_dataPoint.Name == "DataPoint")
                    {
                        ds.Samples[i].x = float.Parse(node_dataPoint.Attributes[0].Value) * 100;
                        ds.Samples[i].y = float.Parse(node_dataPoint.Attributes[1].Value) * 100;
                        i++;
                    }
                }
            }
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
                int Value = (int)(s.Samples[idx].x);
                return "" + Value;
            }
            else
            {
                int Value = (int)(s.Samples[idx].x);
                String Label = "" + Value/100 + "\"";
                return Label;
            }
        }

        private String RenderYLabel(DataSource s, float value)
        {
            return String.Format("{0:0.0}", value);
        }
    }
}

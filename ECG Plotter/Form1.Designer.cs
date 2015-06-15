namespace ECG_Plotter
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.toolStripDropDownButton1 = new System.Windows.Forms.ToolStripDropDownButton();
            this.newToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.toolStripDropDownButton_dMode = new System.Windows.Forms.ToolStripDropDownButton();
            this.toolStripMenuItem_single = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem_dual = new System.Windows.Forms.ToolStripMenuItem();
            this.mainPanel = new System.Windows.Forms.Panel();
            this.panel_info = new System.Windows.Forms.Panel();
            this.label6 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.textBox_lowestFilteredData = new System.Windows.Forms.TextBox();
            this.textBox_highestFilteredData = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.textBox_lowestRawData = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.textBox_highestRawData = new System.Windows.Forms.TextBox();
            this.display = new GraphLib.PlotterDisplayEx();
            this.openFileDialogIn = new System.Windows.Forms.OpenFileDialog();
            this.button_autoScaleY = new System.Windows.Forms.Button();
            this.toolStrip1.SuspendLayout();
            this.mainPanel.SuspendLayout();
            this.panel_info.SuspendLayout();
            this.SuspendLayout();
            // 
            // toolStrip1
            // 
            this.toolStrip1.BackColor = System.Drawing.SystemColors.ControlDarkDark;
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripDropDownButton1,
            this.toolStripSeparator1,
            this.toolStripDropDownButton_dMode});
            this.toolStrip1.Location = new System.Drawing.Point(0, 0);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Size = new System.Drawing.Size(837, 25);
            this.toolStrip1.TabIndex = 0;
            this.toolStrip1.Text = "toolStrip1";
            // 
            // toolStripDropDownButton1
            // 
            this.toolStripDropDownButton1.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.toolStripDropDownButton1.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newToolStripMenuItem,
            this.openToolStripMenuItem,
            this.saveToolStripMenuItem,
            this.exitToolStripMenuItem});
            this.toolStripDropDownButton1.Image = ((System.Drawing.Image)(resources.GetObject("toolStripDropDownButton1.Image")));
            this.toolStripDropDownButton1.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripDropDownButton1.Name = "toolStripDropDownButton1";
            this.toolStripDropDownButton1.Size = new System.Drawing.Size(38, 22);
            this.toolStripDropDownButton1.Text = "File";
            // 
            // newToolStripMenuItem
            // 
            this.newToolStripMenuItem.Name = "newToolStripMenuItem";
            this.newToolStripMenuItem.Size = new System.Drawing.Size(103, 22);
            this.newToolStripMenuItem.Text = "New";
            // 
            // openToolStripMenuItem
            // 
            this.openToolStripMenuItem.Name = "openToolStripMenuItem";
            this.openToolStripMenuItem.Size = new System.Drawing.Size(103, 22);
            this.openToolStripMenuItem.Text = "Open";
            this.openToolStripMenuItem.Click += new System.EventHandler(this.openToolStripMenuItem_Click);
            // 
            // saveToolStripMenuItem
            // 
            this.saveToolStripMenuItem.Name = "saveToolStripMenuItem";
            this.saveToolStripMenuItem.Size = new System.Drawing.Size(103, 22);
            this.saveToolStripMenuItem.Text = "Save";
            // 
            // exitToolStripMenuItem
            // 
            this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
            this.exitToolStripMenuItem.Size = new System.Drawing.Size(103, 22);
            this.exitToolStripMenuItem.Text = "Exit";
            this.exitToolStripMenuItem.Click += new System.EventHandler(this.exitToolStripMenuItem_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(6, 25);
            // 
            // toolStripDropDownButton_dMode
            // 
            this.toolStripDropDownButton_dMode.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.toolStripDropDownButton_dMode.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripMenuItem_single,
            this.toolStripMenuItem_dual});
            this.toolStripDropDownButton_dMode.Enabled = false;
            this.toolStripDropDownButton_dMode.Image = ((System.Drawing.Image)(resources.GetObject("toolStripDropDownButton_dMode.Image")));
            this.toolStripDropDownButton_dMode.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripDropDownButton_dMode.Name = "toolStripDropDownButton_dMode";
            this.toolStripDropDownButton_dMode.Size = new System.Drawing.Size(92, 22);
            this.toolStripDropDownButton_dMode.Text = "Display Mode";
            // 
            // toolStripMenuItem_single
            // 
            this.toolStripMenuItem_single.Name = "toolStripMenuItem_single";
            this.toolStripMenuItem_single.Size = new System.Drawing.Size(106, 22);
            this.toolStripMenuItem_single.Text = "Single";
            this.toolStripMenuItem_single.Click += new System.EventHandler(this.toolStripMenuItem_single_Click);
            // 
            // toolStripMenuItem_dual
            // 
            this.toolStripMenuItem_dual.Name = "toolStripMenuItem_dual";
            this.toolStripMenuItem_dual.Size = new System.Drawing.Size(106, 22);
            this.toolStripMenuItem_dual.Text = "Dual";
            this.toolStripMenuItem_dual.Click += new System.EventHandler(this.toolStripMenuItem_dual_Click);
            // 
            // mainPanel
            // 
            this.mainPanel.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.mainPanel.Controls.Add(this.panel_info);
            this.mainPanel.Controls.Add(this.display);
            this.mainPanel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.mainPanel.Location = new System.Drawing.Point(0, 25);
            this.mainPanel.Name = "mainPanel";
            this.mainPanel.Size = new System.Drawing.Size(837, 438);
            this.mainPanel.TabIndex = 1;
            // 
            // panel_info
            // 
            this.panel_info.BackColor = System.Drawing.SystemColors.ControlDarkDark;
            this.panel_info.Controls.Add(this.label6);
            this.panel_info.Controls.Add(this.label5);
            this.panel_info.Controls.Add(this.label3);
            this.panel_info.Controls.Add(this.label4);
            this.panel_info.Controls.Add(this.textBox_lowestFilteredData);
            this.panel_info.Controls.Add(this.textBox_highestFilteredData);
            this.panel_info.Controls.Add(this.label2);
            this.panel_info.Controls.Add(this.textBox_lowestRawData);
            this.panel_info.Controls.Add(this.label1);
            this.panel_info.Controls.Add(this.textBox_highestRawData);
            this.panel_info.Dock = System.Windows.Forms.DockStyle.Right;
            this.panel_info.Location = new System.Drawing.Point(753, 0);
            this.panel_info.Name = "panel_info";
            this.panel_info.Size = new System.Drawing.Size(84, 438);
            this.panel_info.TabIndex = 1;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(16, 53);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(29, 13);
            this.label6.TabIndex = 13;
            this.label6.Text = "Raw";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(16, 253);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(41, 13);
            this.label5.TabIndex = 12;
            this.label5.Text = "Filtered";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(16, 332);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(51, 13);
            this.label3.TabIndex = 11;
            this.label3.Text = "Lowest Y";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(14, 278);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(53, 13);
            this.label4.TabIndex = 10;
            this.label4.Text = "Highest Y";
            // 
            // textBox_lowestFilteredData
            // 
            this.textBox_lowestFilteredData.Location = new System.Drawing.Point(0, 348);
            this.textBox_lowestFilteredData.Name = "textBox_lowestFilteredData";
            this.textBox_lowestFilteredData.Size = new System.Drawing.Size(82, 20);
            this.textBox_lowestFilteredData.TabIndex = 9;
            // 
            // textBox_highestFilteredData
            // 
            this.textBox_highestFilteredData.Location = new System.Drawing.Point(0, 294);
            this.textBox_highestFilteredData.Name = "textBox_highestFilteredData";
            this.textBox_highestFilteredData.Size = new System.Drawing.Size(82, 20);
            this.textBox_highestFilteredData.TabIndex = 7;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(16, 130);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(51, 13);
            this.label2.TabIndex = 6;
            this.label2.Text = "Lowest Y";
            // 
            // textBox_lowestRawData
            // 
            this.textBox_lowestRawData.Location = new System.Drawing.Point(0, 146);
            this.textBox_lowestRawData.Name = "textBox_lowestRawData";
            this.textBox_lowestRawData.Size = new System.Drawing.Size(82, 20);
            this.textBox_lowestRawData.TabIndex = 5;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(14, 76);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(53, 13);
            this.label1.TabIndex = 4;
            this.label1.Text = "Highest Y";
            // 
            // textBox_highestRawData
            // 
            this.textBox_highestRawData.Location = new System.Drawing.Point(0, 92);
            this.textBox_highestRawData.Name = "textBox_highestRawData";
            this.textBox_highestRawData.Size = new System.Drawing.Size(82, 20);
            this.textBox_highestRawData.TabIndex = 3;
            // 
            // display
            // 
            this.display.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.display.BackColor = System.Drawing.Color.Transparent;
            this.display.BackgroundColorBot = System.Drawing.Color.White;
            this.display.BackgroundColorTop = System.Drawing.Color.White;
            this.display.DashedGridColor = System.Drawing.Color.DarkGray;
            this.display.DoubleBuffering = false;
            this.display.Location = new System.Drawing.Point(0, -1);
            this.display.Name = "display";
            this.display.PlaySpeed = 0.5F;
            this.display.Size = new System.Drawing.Size(754, 440);
            this.display.SolidGridColor = System.Drawing.Color.DarkGray;
            this.display.TabIndex = 0;
            this.display.MouseWheel += new System.Windows.Forms.MouseEventHandler(this.display_MouseWheel);
            // 
            // openFileDialogIn
            // 
            this.openFileDialogIn.FileName = "openFileDialogIn";
            // 
            // button_autoScaleY
            // 
            this.button_autoScaleY.Location = new System.Drawing.Point(184, 0);
            this.button_autoScaleY.Name = "button_autoScaleY";
            this.button_autoScaleY.Size = new System.Drawing.Size(72, 24);
            this.button_autoScaleY.TabIndex = 2;
            this.button_autoScaleY.Text = "AutoScaleY";
            this.button_autoScaleY.UseVisualStyleBackColor = true;
            this.button_autoScaleY.Click += new System.EventHandler(this.button_autoScaleY_Click);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(837, 463);
            this.Controls.Add(this.button_autoScaleY);
            this.Controls.Add(this.mainPanel);
            this.Controls.Add(this.toolStrip1);
            this.Name = "MainForm";
            this.Text = "ECG Plotter";
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            this.mainPanel.ResumeLayout(false);
            this.panel_info.ResumeLayout(false);
            this.panel_info.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripDropDownButton toolStripDropDownButton1;
        private System.Windows.Forms.ToolStripMenuItem newToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem openToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.Panel mainPanel;
        private System.Windows.Forms.OpenFileDialog openFileDialogIn;
        private GraphLib.PlotterDisplayEx display;
        private System.Windows.Forms.ToolStripDropDownButton toolStripDropDownButton_dMode;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem_single;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem_dual;
        private System.Windows.Forms.Button button_autoScaleY;
        private System.Windows.Forms.TextBox textBox_highestRawData;
        private System.Windows.Forms.Panel panel_info;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox textBox_lowestFilteredData;
        private System.Windows.Forms.TextBox textBox_highestFilteredData;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox textBox_lowestRawData;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label5;
    }
}


namespace GraphLib
{
    partial class PlotterGraphPaneEx
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

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.SuspendLayout();
            // 
            // PlotterGraphPaneEx
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Name = "PlotterGraphPaneEx";
            this.Size = new System.Drawing.Size(409, 150);
            this.Load += new System.EventHandler(this.OnLoadControl);
            this.Enter += new System.EventHandler(this.PlotterGraphPaneEx_Enter);
            this.MouseHover += new System.EventHandler(this.PlotterGraphPaneEx_MouseHover);
            this.MouseWheel += new System.Windows.Forms.MouseEventHandler(this.PlotterGraphPaneEx_MouseWheel);
            this.ResumeLayout(false);

        }

        #endregion

    }
}

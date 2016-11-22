﻿namespace EditorApplication.Forms
{
    partial class AssetsWindow
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
            this.TreeView = new System.Windows.Forms.TreeView();
            this.SuspendLayout();
            // 
            // TreeView
            // 
            this.TreeView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.TreeView.Location = new System.Drawing.Point(0, 0);
            this.TreeView.Name = "TreeView";
            this.TreeView.PathSeparator = "\\\\";
            this.TreeView.Size = new System.Drawing.Size(284, 262);
            this.TreeView.TabIndex = 0;
            this.TreeView.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.TreeView_AfterSelect);
            this.TreeView.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.TreeView_MouseDoubleClick);
            // 
            // AssetsWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(284, 262);
            this.Controls.Add(this.TreeView);
            this.Font = new System.Drawing.Font("Tahoma", 8F);
            this.HideOnClose = true;
            this.Name = "AssetsWindow";
            this.ShowHint = WeifenLuo.WinFormsUI.Docking.DockState.DockRight;
            this.Text = "Assets";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TreeView TreeView;
    }
}
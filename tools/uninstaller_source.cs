using System;
using System.IO;
using System.Drawing;
using System.Windows.Forms;
using Microsoft.Win32;
using System.Diagnostics;

namespace OpenAIRAC.Uninstaller
{
    public class UninstallForm : Form
    {
        private Panel headerPanel;
        private Label titleLabel;
        private Label subtitleLabel;
        private Label descLabel;
        private CheckBox removeSettingsCheckBox;
        private ProgressBar progressBar;
        private Label statusLabel;
        private Button uninstallButton;
        private Button cancelButton;

        public UninstallForm()
        {
            InitializeUI();
        }

        private void InitializeUI()
        {
            this.Text = "OpenAIRAC Map 2.3.0 Uninstall";
            this.Size = new Size(520, 340);
            this.StartPosition = FormStartPosition.CenterScreen;
            this.FormBorderStyle = FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.Font = new Font("Segoe UI", 9F);
            this.BackColor = Color.FromArgb(245, 245, 245);

            headerPanel = new Panel() {
                Dock = DockStyle.Top,
                Height = 65,
                BackColor = Color.FromArgb(160, 40, 40),
                Padding = new Padding(20, 12, 20, 10)
            };

            titleLabel = new Label() {
                Text = "Uninstall OpenAIRAC Map",
                ForeColor = Color.White,
                Font = new Font("Segoe UI", 12F, FontStyle.Bold),
                AutoSize = true,
                Location = new Point(20, 10)
            };

            subtitleLabel = new Label() {
                Text = "Remove OpenAIRAC Map from your computer.",
                ForeColor = Color.FromArgb(250, 220, 220),
                Font = new Font("Segoe UI", 8.5F),
                AutoSize = true,
                Location = new Point(20, 36)
            };

            headerPanel.Controls.Add(titleLabel);
            headerPanel.Controls.Add(subtitleLabel);
            this.Controls.Add(headerPanel);

            descLabel = new Label() {
                Text = "Are you sure you want to completely remove OpenAIRAC Map 2.3.0 and all of its components?",
                Location = new Point(25, 85),
                Size = new Size(460, 40)
            };
            this.Controls.Add(descLabel);

            removeSettingsCheckBox = new CheckBox() {
                Text = "Also remove user settings, caches, and custom flight plans in %APPDATA%",
                Checked = false,
                Location = new Point(25, 135),
                Size = new Size(460, 25)
            };
            this.Controls.Add(removeSettingsCheckBox);

            progressBar = new ProgressBar() {
                Location = new Point(25, 175),
                Size = new Size(455, 22),
                Visible = false
            };
            this.Controls.Add(progressBar);

            statusLabel = new Label() {
                Text = "",
                Location = new Point(25, 205),
                Size = new Size(455, 30),
                ForeColor = Color.FromArgb(70, 70, 70)
            };
            this.Controls.Add(statusLabel);

            uninstallButton = new Button() {
                Text = "Uninstall",
                Font = new Font("Segoe UI", 9F, FontStyle.Bold),
                Location = new Point(310, 255),
                Size = new Size(95, 30),
                BackColor = Color.FromArgb(160, 40, 40),
                ForeColor = Color.White,
                FlatStyle = FlatStyle.Flat
            };
            uninstallButton.Click += OnUninstallClicked;
            this.Controls.Add(uninstallButton);

            cancelButton = new Button() {
                Text = "Cancel",
                Location = new Point(415, 255),
                Size = new Size(80, 30)
            };
            cancelButton.Click += (s, e) => { this.Close(); };
            this.Controls.Add(cancelButton);
        }

        private async void OnUninstallClicked(object sender, EventArgs e)
        {
            uninstallButton.Enabled = false;
            cancelButton.Enabled = false;
            removeSettingsCheckBox.Enabled = false;
            progressBar.Visible = true;
            progressBar.Style = ProgressBarStyle.Marquee;
            statusLabel.Text = "Removing OpenAIRAC Map components...";

            string appDir = AppDomain.CurrentDomain.BaseDirectory.TrimEnd(Path.DirectorySeparatorChar, Path.AltDirectorySeparatorChar);

            try {
                await System.Threading.Tasks.Task.Run(() => {
                    // Remove Per-User shortcuts
                    string startMenuDir = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.Programs), "OpenAIRAC Map");
                    if (Directory.Exists(startMenuDir)) {
                        try { Directory.Delete(startMenuDir, true); } catch {}
                    }

                    string desktopShortcut = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.DesktopDirectory), "OpenAIRAC Map.lnk");
                    if (File.Exists(desktopShortcut)) {
                        try { File.Delete(desktopShortcut); } catch {}
                    }

                    // Remove HKCU registry key
                    try {
                        Registry.CurrentUser.DeleteSubKeyTree(@"Software\Microsoft\Windows\CurrentVersion\Uninstall\OpenAIRAC Map", false);
                    } catch {}

                    // Remove user settings if explicitly requested
                    if (removeSettingsCheckBox.Checked) {
                        string appDataDir = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "ABarthel");
                        if (Directory.Exists(appDataDir)) {
                            try { Directory.Delete(appDataDir, true); } catch {}
                        }
                    }
                });

                progressBar.Style = ProgressBarStyle.Continuous;
                progressBar.Value = 100;
                statusLabel.Text = "OpenAIRAC Map was successfully removed from your computer.";
                statusLabel.ForeColor = Color.FromArgb(40, 150, 40);

                uninstallButton.Text = "Close";
                uninstallButton.Enabled = true;
                uninstallButton.Click -= OnUninstallClicked;
                uninstallButton.Click += (s, ev) => {
                    // Self-delete via temp batch
                    ScheduleSelfDelete(appDir);
                    this.Close();
                };

            } catch (Exception ex) {
                statusLabel.Text = "Uninstall error: " + ex.Message;
                statusLabel.ForeColor = Color.Red;
                MessageBox.Show("An error occurred during uninstallation:\n" + ex.Message, "Uninstall Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                uninstallButton.Enabled = true;
            }
        }

        private void ScheduleSelfDelete(string dirToDelete)
        {
            try {
                string tempBat = Path.Combine(Path.GetTempPath(), "openairac_cleanup_" + Guid.NewGuid().ToString("N") + ".bat");
                string script = string.Format(
                    "@echo off\r\n" +
                    "ping 127.0.0.1 -n 2 > nul\r\n" +
                    ":retry\r\n" +
                    "rmdir /S /Q \"{0}\" > nul 2>&1\r\n" +
                    "if exist \"{0}\" (\r\n" +
                    "  ping 127.0.0.1 -n 2 > nul\r\n" +
                    "  goto retry\r\n" +
                    ")\r\n" +
                    "del \"%~f0\"\r\n",
                    dirToDelete
                );
                File.WriteAllText(tempBat, script);

                ProcessStartInfo psi = new ProcessStartInfo(tempBat) {
                    CreateNoWindow = true,
                    UseShellExecute = false,
                    WindowStyle = ProcessWindowStyle.Hidden
                };
                Process.Start(psi);
            } catch {}
        }

        [STAThread]
        public static void Main(string[] args)
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new UninstallForm());
        }
    }
}

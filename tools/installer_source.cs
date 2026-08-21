using System;
using System.IO;
using System.IO.Compression;
using System.Drawing;
using System.Windows.Forms;
using Microsoft.Win32;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Reflection;

namespace OpenAIRAC.Installer
{
    public class SetupForm : Form
    {
        private Panel headerPanel;
        private Label titleLabel;
        private Label subtitleLabel;
        private Panel mainPanel;
        private Label descLabel;
        private TextBox pathTextBox;
        private Button browseButton;
        private CheckBox startMenuCheckBox;
        private CheckBox desktopCheckBox;
        private CheckBox launchCheckBox;
        private ProgressBar progressBar;
        private Label statusLabel;
        private Button installButton;
        private Button cancelButton;
        private string payloadZipPath;
        private string uninstallerExePath;

        public SetupForm(string zipPath, string uninstallerPath)
        {
            this.payloadZipPath = zipPath;
            this.uninstallerExePath = uninstallerPath;
            InitializeUI();
        }

        private void InitializeUI()
        {
            this.Text = "OpenAIRAC Map 2.3.0 Setup";
            this.Size = new Size(580, 460);
            this.StartPosition = FormStartPosition.CenterScreen;
            this.FormBorderStyle = FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.Font = new Font("Segoe UI", 9F);
            this.BackColor = Color.FromArgb(245, 245, 245);

            headerPanel = new Panel() {
                Dock = DockStyle.Top,
                Height = 75,
                BackColor = Color.FromArgb(24, 76, 120),
                Padding = new Padding(20, 15, 20, 10)
            };

            titleLabel = new Label() {
                Text = "OpenAIRAC Map — Version 2.3.0 (64-bit)",
                ForeColor = Color.White,
                Font = new Font("Segoe UI", 12F, FontStyle.Bold),
                AutoSize = true,
                Location = new Point(20, 12)
            };

            subtitleLabel = new Label() {
                Text = "Open-source flight navigation data, EFB, and moving map. (FLIGHT SIMULATION ONLY)",
                ForeColor = Color.FromArgb(220, 235, 245),
                Font = new Font("Segoe UI", 8.5F),
                AutoSize = true,
                Location = new Point(20, 40)
            };

            headerPanel.Controls.Add(titleLabel);
            headerPanel.Controls.Add(subtitleLabel);
            this.Controls.Add(headerPanel);

            mainPanel = new Panel() {
                Location = new Point(20, 90),
                Size = new Size(525, 275)
            };

            descLabel = new Label() {
                Text = "Setup will install OpenAIRAC Map to the following destination folder.\nTo install to a different folder, click Browse and select another directory.",
                Location = new Point(0, 5),
                Size = new Size(520, 35)
            };
            mainPanel.Controls.Add(descLabel);

            Label destLabel = new Label() {
                Text = "Destination Folder:",
                Font = new Font("Segoe UI", 9F, FontStyle.Bold),
                Location = new Point(0, 45),
                AutoSize = true
            };
            mainPanel.Controls.Add(destLabel);

            string defaultPath = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ProgramFiles), "OpenAIRAC Map");
            pathTextBox = new TextBox() {
                Text = defaultPath,
                Location = new Point(0, 68),
                Size = new Size(425, 23)
            };
            mainPanel.Controls.Add(pathTextBox);

            browseButton = new Button() {
                Text = "Browse...",
                Location = new Point(435, 66),
                Size = new Size(85, 26)
            };
            browseButton.Click += (s, e) => {
                using (FolderBrowserDialog fbd = new FolderBrowserDialog()) {
                    fbd.SelectedPath = pathTextBox.Text;
                    if (fbd.ShowDialog() == DialogResult.OK) {
                        pathTextBox.Text = fbd.SelectedPath;
                    }
                }
            };
            mainPanel.Controls.Add(browseButton);

            startMenuCheckBox = new CheckBox() {
                Text = "Create Start Menu shortcut",
                Checked = true,
                Location = new Point(5, 110),
                AutoSize = true
            };
            mainPanel.Controls.Add(startMenuCheckBox);

            desktopCheckBox = new CheckBox() {
                Text = "Create Desktop shortcut",
                Checked = true,
                Location = new Point(5, 135),
                AutoSize = true
            };
            mainPanel.Controls.Add(desktopCheckBox);

            launchCheckBox = new CheckBox() {
                Text = "Launch OpenAIRAC Map after installation completes",
                Checked = true,
                Location = new Point(5, 160),
                AutoSize = true
            };
            mainPanel.Controls.Add(launchCheckBox);

            progressBar = new ProgressBar() {
                Location = new Point(0, 205),
                Size = new Size(520, 22),
                Visible = false
            };
            mainPanel.Controls.Add(progressBar);

            statusLabel = new Label() {
                Text = "Ready to install OpenAIRAC Map 2.3.0.",
                Location = new Point(0, 235),
                Size = new Size(520, 30),
                ForeColor = Color.FromArgb(70, 70, 70)
            };
            mainPanel.Controls.Add(statusLabel);

            this.Controls.Add(mainPanel);

            installButton = new Button() {
                Text = "Install",
                Font = new Font("Segoe UI", 9F, FontStyle.Bold),
                Location = new Point(360, 375),
                Size = new Size(95, 30),
                BackColor = Color.FromArgb(24, 76, 120),
                ForeColor = Color.White,
                FlatStyle = FlatStyle.Flat
            };
            installButton.Click += OnInstallClicked;
            this.Controls.Add(installButton);

            cancelButton = new Button() {
                Text = "Cancel",
                Location = new Point(465, 375),
                Size = new Size(80, 30)
            };
            cancelButton.Click += (s, e) => { this.Close(); };
            this.Controls.Add(cancelButton);
        }

        private async void OnInstallClicked(object sender, EventArgs e)
        {
            string targetDir = pathTextBox.Text.Trim();
            if (string.IsNullOrEmpty(targetDir)) {
                MessageBox.Show("Please specify a valid destination folder.", "Invalid Path", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            installButton.Enabled = false;
            browseButton.Enabled = false;
            pathTextBox.Enabled = false;
            startMenuCheckBox.Enabled = false;
            desktopCheckBox.Enabled = false;
            progressBar.Visible = true;
            progressBar.Style = ProgressBarStyle.Marquee;
            statusLabel.Text = "Extracting application binaries and navigation datasets...";

            try {
                if (!Directory.Exists(targetDir)) {
                    Directory.CreateDirectory(targetDir);
                }

                await System.Threading.Tasks.Task.Run(() => {
                    // Extract payload from embedded resource or disk
                    Stream resStream = Assembly.GetExecutingAssembly().GetManifestResourceStream("payload.zip");
                    if (resStream != null) {
                        using (ZipArchive archive = new ZipArchive(resStream)) {
                            foreach (ZipArchiveEntry entry in archive.Entries) {
                                string destPath = Path.Combine(targetDir, entry.FullName);
                                if (string.IsNullOrEmpty(entry.Name)) {
                                    Directory.CreateDirectory(destPath);
                                } else {
                                    Directory.CreateDirectory(Path.GetDirectoryName(destPath));
                                    entry.ExtractToFile(destPath, true);
                                }
                            }
                        }
                    } else if (File.Exists(payloadZipPath)) {
                        ZipFile.ExtractToDirectory(payloadZipPath, targetDir);
                    }

                    // Copy uninstaller
                    if (File.Exists(uninstallerExePath)) {
                        File.Copy(uninstallerExePath, Path.Combine(targetDir, "Uninstall.exe"), true);
                    }

                    // Register uninstaller in Windows Registry
                    RegisterUninstall(targetDir);

                    // Create shortcuts
                    string exePath = Path.Combine(targetDir, "littlenavmap.exe");
                    if (File.Exists(exePath)) {
                        if (startMenuCheckBox.Checked) {
                            string startMenuDir = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.CommonPrograms), "OpenAIRAC Map");
                            if (!Directory.Exists(startMenuDir)) {
                                Directory.CreateDirectory(startMenuDir);
                            }
                            CreateShortcut(Path.Combine(startMenuDir, "OpenAIRAC Map.lnk"), exePath, targetDir, "OpenAIRAC Map Flight Planner & EFB");
                        }

                        if (desktopCheckBox.Checked) {
                            string desktopDir = Environment.GetFolderPath(Environment.SpecialFolder.DesktopDirectory);
                            CreateShortcut(Path.Combine(desktopDir, "OpenAIRAC Map.lnk"), exePath, targetDir, "OpenAIRAC Map Flight Planner & EFB");
                        }
                    }
                });

                progressBar.Style = ProgressBarStyle.Continuous;
                progressBar.Value = 100;
                statusLabel.Text = "Installation completed successfully!";
                statusLabel.ForeColor = Color.FromArgb(40, 150, 40);

                installButton.Text = "Finish";
                installButton.Enabled = true;
                installButton.Click -= OnInstallClicked;
                installButton.Click += (s, ev) => {
                    if (launchCheckBox.Checked) {
                        string exePath = Path.Combine(targetDir, "littlenavmap.exe");
                        if (File.Exists(exePath)) {
                            Process.Start(new ProcessStartInfo(exePath) { WorkingDirectory = targetDir });
                        }
                    }
                    this.Close();
                };
                cancelButton.Visible = false;

            } catch (Exception ex) {
                progressBar.Visible = false;
                statusLabel.Text = "Installation failed: " + ex.Message;
                statusLabel.ForeColor = Color.Red;
                MessageBox.Show("An error occurred during installation:\n" + ex.Message, "Installation Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                installButton.Enabled = true;
            }
        }

        private void RegisterUninstall(string installDir)
        {
            try {
                string keyPath = @"Software\Microsoft\Windows\CurrentVersion\Uninstall\OpenAIRAC Map";
                using (RegistryKey key = Registry.LocalMachine.CreateSubKey(keyPath)) {
                    if (key != null) {
                        key.SetValue("DisplayName", "OpenAIRAC Map 2.3.0 (64-bit)");
                        key.SetValue("DisplayVersion", "2.3.0");
                        key.SetValue("Publisher", "OpenAIRAC Contributors");
                        key.SetValue("DisplayIcon", Path.Combine(installDir, "littlenavmap.exe") + ",0");
                        key.SetValue("UninstallString", "\"" + Path.Combine(installDir, "Uninstall.exe") + "\"");
                        key.SetValue("InstallLocation", installDir);
                        key.SetValue("URLInfoAbout", "https://github.com/bobberdolle1/openairac-map");
                        key.SetValue("HelpLink", "https://github.com/bobberdolle1/open-airac/tree/main/docs");
                        key.SetValue("NoModify", 1, RegistryValueKind.DWord);
                        key.SetValue("NoRepair", 1, RegistryValueKind.DWord);
                    }
                }
            } catch {
                // If LocalMachine fails due to permissions, write to CurrentUser
                try {
                    string userKeyPath = @"Software\Microsoft\Windows\CurrentVersion\Uninstall\OpenAIRAC Map";
                    using (RegistryKey key = Registry.CurrentUser.CreateSubKey(userKeyPath)) {
                        if (key != null) {
                            key.SetValue("DisplayName", "OpenAIRAC Map 2.3.0 (64-bit)");
                            key.SetValue("DisplayVersion", "2.3.0");
                            key.SetValue("Publisher", "OpenAIRAC Contributors");
                            key.SetValue("DisplayIcon", Path.Combine(installDir, "littlenavmap.exe") + ",0");
                            key.SetValue("UninstallString", "\"" + Path.Combine(installDir, "Uninstall.exe") + "\"");
                            key.SetValue("InstallLocation", installDir);
                            key.SetValue("URLInfoAbout", "https://github.com/bobberdolle1/openairac-map");
                            key.SetValue("HelpLink", "https://github.com/bobberdolle1/open-airac/tree/main/docs");
                        }
                    }
                } catch {}
            }
        }

        private void CreateShortcut(string shortcutPath, string targetPath, string workDir, string description)
        {
            try {
                Type shellType = Type.GetTypeFromProgID("WScript.Shell");
                dynamic shell = Activator.CreateInstance(shellType);
                dynamic shortcut = shell.CreateShortcut(shortcutPath);
                shortcut.TargetPath = targetPath;
                shortcut.WorkingDirectory = workDir;
                shortcut.Description = description;
                shortcut.Save();
            } catch {}
        }

        [STAThread]
        public static void Main(string[] args)
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            string exeDir = AppDomain.CurrentDomain.BaseDirectory;
            string zipPath = Path.Combine(exeDir, "payload.zip");
            string uninstallerPath = Path.Combine(exeDir, "Uninstall.exe");

            Application.Run(new SetupForm(zipPath, uninstallerPath));
        }
    }
}

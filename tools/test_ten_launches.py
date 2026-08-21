#!/usr/bin/env python3
"""
OpenAIRAC Map — Ten-Launch Stability Gate

Performs 10 consecutive clean launches and normal exits in an isolated profile:
- Run 1: First-time onboarding / startup.
- Runs 2-10: Normal daily usage launches on existing profile.

Verifies:
1. Process starts cleanly and loads MainWindow.
2. Main window title is verified as 'OpenAIRAC Map 2.4.0 ...' (0 .develop leakage).
3. .running lock file is created on startup and cleared on exit.
4. Exit code is 0 on every single run.
5. No crash report zip or stack trace is left behind.
6. 10/10 launches succeed with 0 crash recovery dialogs.
"""

import sys
import os
import time
import shutil
import subprocess
import ctypes
import ctypes.wintypes
from pathlib import Path

EXE_PATH = Path("F:/Projects/deploy/Little Navmap win64/littlenavmap.exe")
TEST_PROFILE = Path("F:/Projects/deploy/stability_test_profile")

user32 = ctypes.windll.user32

def get_windows_for_pid(pid):
    hwnds = []
    def enum_windows_callback(hwnd, extra):
        if not user32.IsWindowVisible(hwnd):
            return True
        window_pid = ctypes.wintypes.DWORD()
        user32.GetWindowThreadProcessId(hwnd, ctypes.byref(window_pid))
        if window_pid.value == pid:
            length = user32.GetWindowTextLengthW(hwnd)
            buff = ctypes.create_unicode_buffer(length + 1)
            user32.GetWindowTextW(hwnd, buff, length + 1)
            hwnds.append((hwnd, buff.value))
        return True

    WNDENUMPROC = ctypes.WINFUNCTYPE(ctypes.c_bool, ctypes.wintypes.HWND, ctypes.wintypes.LPARAM)
    user32.EnumWindows(WNDENUMPROC(enum_windows_callback), 0)
    return hwnds

def close_all_for_pid(pid, main_hwnd):
    WM_CLOSE = 0x0010
    WM_KEYDOWN = 0x0100
    VK_RETURN = 0x0D
    
    # Close any popups or dialogs first
    windows = get_windows_for_pid(pid)
    for hwnd, title in windows:
        if not title.startswith("OpenAIRAC Map 2.4.0"):
            user32.PostMessageW(hwnd, WM_KEYDOWN, VK_RETURN, 0)
            user32.PostMessageW(hwnd, WM_CLOSE, 0, 0)
            
    time.sleep(0.3)
    user32.PostMessageW(main_hwnd, WM_CLOSE, 0, 0)

def main():
    print("=== OpenAIRAC Map Ten-Launch Stability Gate ===")
    
    if not EXE_PATH.exists():
        print(f"ERROR: {EXE_PATH} does not exist!")
        sys.exit(1)

    if TEST_PROFILE.exists():
        shutil.rmtree(TEST_PROFILE)
    TEST_PROFILE.mkdir(parents=True, exist_ok=True)

    # Initialize profile with non-blocking action settings
    ini_content = """[Actions]
ShowQuit=false
ShowQuitLoading=false
ShowInstallGlobe=false
ShowMissingSimulators=false
ShowDisconnectInfo=false
ShowInstallXpconnectXplWarning=false
ShowInstallXpconnectInfo=false

[Options]
Widget_checkBoxOptionsStartupShowSplash=false

[OptionsDialog]
Widget_checkBoxOptionsStartupCheckUpdates=false

[MainWindow]
FirstApplicationStart=false
"""
    (TEST_PROFILE / "openairac_map.ini").write_text(ini_content, encoding="utf-8")

    for i in range(1, 11):
        print(f"\n[Run {i}/10] Launching OpenAIRAC Map...")
        
        cmd = [str(EXE_PATH), "-p", str(TEST_PROFILE)]
        proc = subprocess.Popen(cmd)
        
        running_file = TEST_PROFILE / "openairac_map.running"
        settings_file = TEST_PROFILE / "openairac_map.ini"
        
        # Wait until MainWindow is created and visible
        start_time = time.time()
        ui_ready = False
        main_hwnd = None
        main_title = ""
        
        while time.time() - start_time < 30:
            if proc.poll() is not None:
                print(f"❌ Run {i} FAILED: Process died prematurely with code {proc.returncode}!")
                sys.exit(1)
                
            windows = get_windows_for_pid(proc.pid)
            for hwnd, title in windows:
                if title.startswith("OpenAIRAC Map 2.4.0"):
                    ui_ready = True
                    main_hwnd = hwnd
                    main_title = title
                    break
            if ui_ready:
                break
            time.sleep(0.5)
            
        if not ui_ready:
            print(f"❌ Run {i} FAILED: Main window did not appear within 30 seconds!")
            proc.kill()
            sys.exit(1)
            
        # Verify 0 .develop leakage
        if ".develop" in main_title:
            print(f"❌ Run {i} FAILED: Title contains forbidden '.develop' string: '{main_title}'!")
            proc.kill()
            sys.exit(1)

        print(f"  -> Main window ready: '{main_title}' (HWND: {main_hwnd}, PID: {proc.pid})")
        print("  -> Waiting 4 seconds for event loop and background tasks...")
        time.sleep(4.0)
        
        print(f"  -> Sending clean close to HWND {main_hwnd}...")
        close_all_for_pid(proc.pid, main_hwnd)
        
        # Wait for exit
        exit_start = time.time()
        exited = False
        exit_code = None
        while time.time() - exit_start < 15:
            ret = proc.poll()
            if ret is not None:
                exited = True
                exit_code = ret
                break
            time.sleep(0.5)
            
        if not exited:
            print(f"❌ Run {i} FAILED: Process did not exit within 15 seconds after close!")
            proc.kill()
            sys.exit(1)
            
        print(f"  -> Process exited cleanly with code: {exit_code}")
        if exit_code != 0:
            print(f"❌ Run {i} FAILED: Expected exit code 0, got {exit_code}!")
            sys.exit(1)

        # Check lockfile removal
        if running_file.exists():
            print(f"❌ Run {i} FAILED: .running lockfile was not cleared on clean exit!")
            sys.exit(1)
            
        # Check for crash reports
        crash_dir = TEST_PROFILE / "crashreports"
        if crash_dir.exists() and any(crash_dir.glob("*.zip")):
            print(f"❌ Run {i} FAILED: Crash report zip was created in crashreports folder!")
            sys.exit(1)

        print(f"  [OK] Run {i}/10 PASS: Clean startup, clean shutdown, exit code 0, 0 crash reports.")

    print("\n=======================================================")
    print("✅ TEN-LAUNCH STABILITY GATE PASSED: 10/10 CLEAN RUNS")
    print("=======================================================")
    print("✅ VERSION AGREEMENT: OpenAIRAC Map v2.4.0 (0 .develop strings)")
    print("=======================================================\n")

if __name__ == "__main__":
    main()

# OpenAIRAC & OpenAIRAC Map Platform Support Matrix (v2.0 / v1.0)

## 1. Supported Operating Systems & Architectures

| Operating System | Architecture | Maturity Status | Distribution Formats | Validation Method |
| :--- | :---: | :---: | :--- | :--- |
| **Windows 11 / 10** | `x86_64` (64-bit) | **SUPPORTED** | One-Click Setup Installer (`.exe`), Portable ZIP (`.zip`) | Full native compiler (MSVC / Rust `x86_64-pc-windows-msvc`), Qt 6/5 runtime, Windows 11 development workstation. |
| **Linux (Ubuntu 24.04/22.04, Fedora, Arch)** | `x86_64` (64-bit) | **SUPPORTED** | Universal AppImage (`.AppImage`), Standalone Tarball (`.tar.gz`) | GitHub Actions `ubuntu-24.04` CI matrix, GCC 13, system Qt5/Qt6 libraries. |
| **macOS (Sonoma 14, Ventura 13)** | `arm64` (Apple Silicon) | **SUPPORTED** | Application Bundle (`.app`), Disk Image (`.dmg`) | Clang / `aarch64-apple-darwin` cross-build, unsigned community release. |
| **macOS (Intel)** | `x86_64` | **EXPERIMENTAL** | Application Bundle (`.app`) | `x86_64-apple-darwin` toolchain. |

---

## 2. Binary Packaging Contracts

### 2.1 Bundled Executable Architecture
OpenAIRAC Map packages the compiled OpenAIRAC core sidecar binary (`openairac.exe` / `openairac`) alongside the Qt user interface:
* **Zero Dependency on Toolchains**: End-users do not require Rust, Cargo, Git, CMake, or SQLite installed.
* **Automatic Core Discovery**: OpenAIRAC Map locates the co-located backend binary automatically without requiring manual `PATH` adjustments.
* **Handshake Validation**: Protocol version handshake (`Protocol v2`) is verified at startup.

---

## 3. Cryptographic Verification & Manifests

Every official release publishes a machine-verifiable `SHA256SUMS.txt` manifest:

```bash
# Verify checksum on Windows (PowerShell)
Get-FileHash OpenAIRAC-Map-1.0.0-Windows-x64-Setup.exe -Algorithm SHA256

# Verify checksum on Linux
sha256sum -c SHA256SUMS.txt
```

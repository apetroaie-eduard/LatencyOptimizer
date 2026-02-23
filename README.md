# LatencyOptimizer

A Windows 10/11 system-latency optimization tool that reduces input lag, network
jitter, DPC latency, and frame-time variance. Available as a **GUI application**
(ImGui/DX11) and a **standalone PowerShell script** (no build required).

Applies **52 tweaks across 12 categories** with automatic backup and full
restore capability. Every change is reversible.

---

## Table of Contents

- [Who Is This For](#who-is-this-for)
- [Two Ways to Run](#two-ways-to-run)
- [Option A: PowerShell Script (Recommended)](#option-a-powershell-script-recommended)
- [Option B: GUI Application (.exe)](#option-b-gui-application-exe)
- [Step-by-Step Usage Guide](#step-by-step-usage-guide)
- [Tweak Reference (All 52 Tweaks)](#tweak-reference-all-52-tweaks)
- [Risk Levels Explained](#risk-levels-explained)
- [DPC Latency -- Technical Background](#dpc-latency----technical-background)
- [Verifying Results](#verifying-results)
- [Backup and Restore](#backup-and-restore)
- [Troubleshooting](#troubleshooting)
- [Project Structure](#project-structure)
- [Building the GUI from Source](#building-the-gui-from-source)
- [Disclaimer](#disclaimer)

---

## Who Is This For

- **Competitive gamers** looking to minimize input lag and frame-time variance
- **Audio producers** experiencing crackles, pops, or dropouts caused by DPC latency
- **VR users** who need consistent frame delivery to avoid judder
- **Anyone** who wants a cleaner, leaner Windows installation with less background noise

The tool targets Windows 10 and Windows 11 (x64). Some tweaks are NVIDIA-specific
and will be silently skipped on AMD/Intel GPU systems.

---

## Two Ways to Run

| Method | Requirements | Best For |
|---|---|---|
| **PowerShell script** (`LatencyOptimizer.ps1`) | Windows + PowerShell 5.1+ (built-in) | Quick setup, automation, headless servers |
| **GUI application** (`LatencyOptimizer.exe`) | Build with Visual Studio + CMake | Visual interface, per-tweak control |

Both methods apply the same tweaks and produce the same results. The PowerShell
script is the easiest way to get started.

---

## Option A: PowerShell Script (Recommended)

### Prerequisites

- Windows 10 or 11 (x64)
- PowerShell 5.1 or later (pre-installed on all modern Windows)
- Administrator privileges

### Step 1: Open PowerShell as Administrator

1. Press `Win + X`
2. Click **Terminal (Admin)** or **Windows PowerShell (Admin)**
3. If prompted by UAC, click **Yes**

### Step 2: Navigate to the script

```powershell
cd C:\path\to\LatencyOptimizer
```

### Step 3: Run the script

**Interactive menu (recommended for first use):**

```powershell
powershell -ExecutionPolicy Bypass -File LatencyOptimizer.ps1
```

**Apply all tweaks immediately (no menu):**

```powershell
powershell -ExecutionPolicy Bypass -File LatencyOptimizer.ps1 -Apply
```

**Restore original settings from backup:**

```powershell
powershell -ExecutionPolicy Bypass -File LatencyOptimizer.ps1 -Restore
```

### Step 4: Reboot

After applying tweaks, reboot your PC for all changes to take effect. Some tweaks
(registry, BCD, services) only apply after a restart.

### Optional: Convert to .exe

If you want a double-clickable `.exe` file instead of running PowerShell commands:

```powershell
# Install ps2exe (one-time setup)
Install-Module ps2exe -Scope CurrentUser

# Convert the script to an .exe that auto-requests admin
Invoke-ps2exe -InputFile LatencyOptimizer.ps1 -OutputFile LatencyOptimizer.exe -requireAdmin
```

Double-click `LatencyOptimizer.exe` and it will run the interactive menu with a
UAC prompt.

### One-Liner (Copy-Paste)

To apply all tweaks in a single command without downloading the repo:

```powershell
Set-ExecutionPolicy Bypass -Scope Process -Force; .\LatencyOptimizer.ps1 -Apply
```

---

## Option B: GUI Application (.exe)

See [Building the GUI from Source](#building-the-gui-from-source) below for build
instructions. Once built:

1. Navigate to `build\Release\`
2. Double-click **LatencyOptimizer.exe**
3. UAC will prompt for administrator -- click **Yes**
4. The GUI window opens with all 52 tweaks organized by category

---

## Step-by-Step Usage Guide

### Using the PowerShell Interactive Menu

When you run the script without flags, you see this menu:

```
  =============================================
       LATENCY OPTIMIZER  -  PowerShell Edition
  =============================================

  [1] Apply ALL tweaks (recommended)
  [2] Apply by category...
  [3] Restore from backup
  [4] Export log
  [0] Exit
```

#### Option 1: Apply ALL Tweaks

1. Type `1` and press Enter
2. The script automatically creates a backup of your current settings
3. All 52 tweaks are applied in sequence across all 12 categories
4. Each tweak prints a `[+]` success or `[-]` failure message
5. A log file is saved to `%USERPROFILE%\LatencyOptimizer_Log.txt`
6. Reboot when done

#### Option 2: Apply by Category

1. Type `2` and press Enter
2. A category sub-menu appears:

```
  [1]  Services        (SysMain, WSearch, Spooler, Diag)
  [2]  Registry / UI   (Visual FX, Tips, Game Bar, Cortana, Fast Startup)
  [3]  Power           (Ultimate Perf, USB Suspend, Core Parking, Throttling)
  [4]  Network         (Nagle, Throttling, Auto-Tuning, ECN, Timestamps)
  [5]  GPU             (NVIDIA, HAGS, Fullscreen Opt)
  [6]  Memory          (Paging Executive, Large Cache, Compression)
  [7]  Timers          (HPET, Dynamic Tick)
  [8]  Interrupts      (NIC Affinity, MSI)
  [9]  Input           (Mouse Accel, Polling, Sticky Keys, Game Mode)
  [10] Scheduler       (Win32Priority, MMCSS Games, Pro Audio)
  [11] DPC Latency     (NVIDIA HDCP, Per-CPU DPC, ASPM, GPU MSI, PState)
  [12] Misc / Privacy  (Telemetry, Notifications, Background Apps, etc.)
  [0]  Back
```

3. Type the number of the category you want
4. A backup is created, then only that category's tweaks are applied
5. You can apply multiple categories one at a time

#### Option 3: Restore from Backup

1. Type `3` and press Enter
2. The script reads `%USERPROFILE%\LatencyOptimizer_Backup.json`
3. All registry values are restored to their original state
4. All services are set back to their original startup type
5. BCD, netsh, and powercfg settings are reverted
6. Reboot to complete the restore

#### Option 4: Export Log

1. Type `4` and press Enter
2. All actions from the current session are saved to `%USERPROFILE%\LatencyOptimizer_Log.txt`

### Using the GUI Application

1. **Launch** -- Double-click the `.exe`. It auto-elevates via UAC.
2. **Browse** -- The left sidebar shows all 12 categories. Click a category to filter.
3. **Select** -- Click any tweak in the list to see its description, risk level, and compatibility.
4. **Apply** -- Click **Apply** on a single tweak, or **Apply All Safe** for all low-risk tweaks.
5. **Revert** -- Click **Revert** on a single tweak, or **Revert All** for everything.
6. **Backup** -- Click **Create Restore Point** before making changes.
7. **Export** -- Click **Export Log** to save a text file with all actions.

---

## Tweak Reference (All 52 Tweaks)

### Services (5)

| # | Tweak | Risk | What It Does |
|---|---|---|---|
| 1 | Disable SysMain (Superfetch) | Safe | Stops Windows from preloading apps into RAM; reduces disk I/O spikes |
| 2 | Disable Windows Search Indexer | Safe | Stops background file indexing that causes periodic CPU/disk spikes |
| 3 | Disable Update Delivery Optimization P2P | Safe | Prevents Windows Update from using your bandwidth to share updates with other PCs |
| 4 | Disable Print Spooler | Safe | Stops the print service; safe unless you use a printer |
| 5 | Disable Diagnostic Services | Safe | Stops DiagSvc and dmwappushservice diagnostic data collection |

### Registry / UI (5)

| # | Tweak | Risk | What It Does |
|---|---|---|---|
| 6 | Disable Visual Effects | Safe | Turns off window animations, transparency, and smooth scrolling |
| 7 | Disable Windows Tips & Suggestions | Safe | Stops "Get Started" tips, app suggestions, and Start menu ads |
| 8 | Disable Xbox Game Bar | Safe | Turns off the Game Bar overlay and game DVR recording |
| 9 | Disable Cortana & Cloud Search | Safe | Prevents Cortana and web search from running in the background |
| 10 | Disable Fast Startup | Safe | Disables hybrid shutdown (Hiberboot) for a clean boot every time |

### Power (4)

| # | Tweak | Risk | What It Does |
|---|---|---|---|
| 11 | Ultimate Performance Power Plan | Safe | Activates the hidden Ultimate Performance plan (no CPU throttling) |
| 12 | Disable USB Selective Suspend | Safe | Prevents USB devices from entering low-power mode (fixes mouse/keyboard dropouts) |
| 13 | Disable CPU Core Parking | Safe | Keeps all CPU cores active at all times (no parking to save power) |
| 14 | Disable Power Throttling | Safe | Prevents Windows from throttling background processes to save power |

### Network (5)

| # | Tweak | Risk | What It Does |
|---|---|---|---|
| 15 | Disable Nagle's Algorithm | Safe | Sends TCP packets immediately instead of batching them (reduces ping) |
| 16 | Disable Network Throttling | Safe | Removes the 10-packet MMCSS throttle on non-multimedia network traffic |
| 17 | Disable TCP Auto-Tuning | Medium | Fixes the TCP receive window at a static size (helps on some ISPs, hurts on others) |
| 18 | Disable ECN Capability | Medium | Turns off Explicit Congestion Notification (some routers mishandle it) |
| 19 | Disable TCP Timestamps | Medium | Removes timestamp option from TCP headers (saves ~12 bytes per packet) |

### GPU (4)

| # | Tweak | Risk | What It Does |
|---|---|---|---|
| 20 | NVIDIA: Max Performance + Prerendered Frames=1 | Safe (NVIDIA) | Locks GPU clocks to maximum and limits the render queue to 1 frame |
| 21 | Disable HAGS | Medium | Turns off Hardware-Accelerated GPU Scheduling (helps on older GPUs) |
| 22 | Enable HAGS | Medium (AdvHW) | Turns on HAGS (helps on RTX 20-series+ and RDNA2+ GPUs) |
| 23 | Disable Fullscreen Optimizations | Safe | Allows true exclusive fullscreen instead of Windows' borderless compositing |

### Memory (3)

| # | Tweak | Risk | What It Does |
|---|---|---|---|
| 24 | Disable Paging Executive | Safe | Keeps the Windows kernel locked in RAM instead of allowing it to page to disk |
| 25 | Enable Large System Cache | Medium | Tells Windows to use more RAM for file system caching |
| 26 | Disable Memory Compression | Medium | Stops Windows from compressing RAM pages (trades memory for CPU cycles) |

### Timers (2)

| # | Tweak | Risk | What It Does |
|---|---|---|---|
| 27 | High-Resolution System Timer (HPET) | Safe | Enables the platform clock and enhanced TSC synchronization for precise timing |
| 28 | Disable Dynamic Tick | Safe | Forces the kernel to fire timer interrupts at a fixed rate instead of coalescing them |

### Interrupts (2)

| # | Tweak | Risk | What It Does |
|---|---|---|---|
| 29 | Network Adapter Interrupt Affinity | Medium | Pins all NIC interrupts to CPU 0 for predictable latency |
| 30 | Disable MSI for Network Adapter | High (AdvHW) | Forces the NIC to legacy INTx mode (only if MSI causes issues on your hardware) |

### Input (5)

| # | Tweak | Risk | What It Does |
|---|---|---|---|
| 31 | Disable Mouse Acceleration | Safe | Sets MouseSpeed, Threshold1, and Threshold2 to 0 for raw 1:1 input |
| 32 | Set Mouse Polling Rate (Registry) | Safe | Increases MouseDataQueueSize to 128 for smoother high-polling-rate mice |
| 33 | Disable Pointer Precision | Safe | Same as mouse acceleration -- ensures flat acceleration curve |
| 34 | Disable Sticky/Filter/Toggle Keys | Safe | Prevents accessibility key shortcuts from interrupting fullscreen games |
| 35 | Enable Game Mode | Safe | Turns on Windows Game Mode (prioritizes game processes) |

### Scheduler (3)

| # | Tweak | Risk | What It Does |
|---|---|---|---|
| 36 | Win32 Priority Separation = 26 | Safe | Short, variable quantum with foreground priority boost for responsive gaming |
| 37 | Game CPU Priority (High) | Safe | Sets MMCSS "Games" task to GPU Priority 8, CPU Priority 6, High scheduling |
| 38 | SFIO Priority (MMCSS) | Safe | Sets MMCSS "Pro Audio" task to High SFIO/scheduling priority |

### DPC Latency (7)

These tweaks specifically target high DPC execution times from NVIDIA's
`nvlddmkm.sys` driver -- the #1 cause of micro-stutter on NVIDIA systems.

| # | Tweak | Risk | What It Does |
|---|---|---|---|
| 39 | NVIDIA: Disable HDCP | Medium | Removes HDCP handshake logic from the driver's DPC routine |
| 40 | NVIDIA: Per-CPU DPC Distribution | Safe | Spreads DPC work across all cores instead of funneling to one (`RmGpsPsEnablePerCpuCoreDpc=1`) |
| 41 | NVIDIA: Disable GPU PCIe ASPM | Safe | Keeps PCIe link at full L0 speed, eliminating re-train DPC spikes (50-200us) |
| 42 | GPU: Enable MSI | Medium | Gives the GPU a dedicated interrupt vector, removing shared-IRQ DPC serialization |
| 43 | NVIDIA: Disable Telemetry Container | Safe | Stops NvTelemetryContainer service that triggers periodic kernel callbacks |
| 44 | GPU: Pin Interrupts to CPU 1 | Medium | Isolates GPU DPCs from NIC DPCs (NIC on CPU 0, GPU on CPU 1) |
| 45 | NVIDIA: Disable PState Deep Idle (P8) | Medium | Prevents idle-to-active DPC storms (>500us) by keeping GPU at higher PState |

### Misc / Privacy (7)

| # | Tweak | Risk | What It Does |
|---|---|---|---|
| 46 | Disable Delivery Optimization Service | Safe | Stops the DoSvc service (Windows Update background downloads) |
| 47 | Disable Notifications | Safe | Turns off the Action Center and toast notifications |
| 48 | Disable Background Apps | Safe | Prevents UWP apps from running in the background |
| 49 | Disable Windows Telemetry | Safe | Sets AllowTelemetry=0 and disables DiagTrack service |
| 50 | Disable Activity History | Safe | Stops Windows from collecting and uploading activity history |
| 51 | Disable Storage Sense | Safe | Stops automatic disk cleanup from running in the background |
| 52 | Disable Windows Error Reporting | Safe | Disables WerSvc and crash report uploads |

---

## Risk Levels Explained

| Level | Meaning | Can I Undo It? |
|---|---|---|
| **Safe** | Very unlikely to cause issues. Affects cosmetics, background services, or well-understood registry keys. | Yes -- one-click revert or restore from backup |
| **Medium** | May affect specific features (e.g., HDCP streaming, TCP throughput on some networks). Test before committing. | Yes -- fully reversible |
| **High** | Could cause instability on certain hardware combinations. Only apply if you understand the tradeoff. | Yes -- fully reversible, but may need Safe Mode if a driver issue occurs |

---

## DPC Latency -- Technical Background

DPC (Deferred Procedure Call) routines are kernel-mode callbacks that device
drivers use to complete interrupt processing. When a hardware interrupt fires,
the driver's ISR (Interrupt Service Routine) runs immediately, then schedules a
DPC to do the bulk of the work. High DPC execution times cause:

- **Audio crackle/pops** -- the audio driver's DPC can't run in time
- **Mouse stuttering** -- input processing is delayed behind a long DPC
- **Frame-time spikes** -- the GPU driver's DPC takes too long, delaying frame presentation

`nvlddmkm.sys` (the NVIDIA kernel-mode driver) is the most common offender.
The DPC Latency category targets its root causes:

| Root Cause | Tweak | Mechanism |
|---|---|---|
| HDCP validation runs in DPC context | Disable HDCP | Removes the entire HDCP code path from the driver DPC |
| All DPC work funneled to one CPU | Per-CPU DPC Distribution | `RmGpsPsEnablePerCpuCoreDpc=1` spreads callbacks across cores |
| PCIe L0s/L1 link re-training | Disable GPU PCIe ASPM | `RmDisableGpuASPMFlags=3` keeps the link at full L0 speed |
| Shared IRQ line causes DPC serialization | Enable GPU MSI | `MSISupported=1` gives the GPU a dedicated interrupt vector |
| NvTelemetryContainer kernel callbacks | Disable NV Telemetry | Stops periodic ETW flush and WMI query callbacks |
| GPU and NIC DPCs on same core | GPU Interrupt Affinity | Pins GPU to CPU 1, NIC to CPU 0 |
| P8 deep idle wake-up burst | Disable PState Deep Idle | `DisableDynamicPstate=1` prevents >500us wake-up DPC storms |

---

## Verifying Results

### Before applying tweaks

1. Download **LatencyMon** (free) from resplendence.com
2. Run it for 5-10 minutes during normal workload
3. Note the **Highest DPC execution time** for `nvlddmkm.sys` and `ndis.sys`
4. Note the **Highest ISR execution time** for the same drivers

### After applying tweaks and rebooting

1. Run LatencyMon again for 5-10 minutes under the same workload
2. Compare the numbers -- you should see measurably lower DPC times
3. The "Your system appears to be suitable for handling real-time audio" message
   should appear if it wasn't showing before

### What to look for

| Metric | Before (typical) | After (target) |
|---|---|---|
| Highest DPC (nvlddmkm.sys) | 500-2000 us | < 200 us |
| Highest DPC (ndis.sys) | 200-800 us | < 100 us |
| Highest ISR total | 100-500 us | < 50 us |
| Audio glitches detected | Yes | No |

---

## Backup and Restore

### How backup works

Every time you apply tweaks (either "Apply All" or a single category), the tool
automatically saves a snapshot of your current settings **before** making any
changes.

**PowerShell:** Saves to `%USERPROFILE%\LatencyOptimizer_Backup.json`
- Contains all registry values (with their original data)
- Contains all service startup types
- Timestamped so you know when the backup was created

**GUI:** Uses the built-in Backup Manager with restore points.

### How to restore

**PowerShell:**

```powershell
powershell -ExecutionPolicy Bypass -File LatencyOptimizer.ps1 -Restore
```

Or select option `[3]` from the interactive menu.

**GUI:** Click **Revert All** or restore from a specific restore point.

### Manual restore (worst case)

If you can't run the script (e.g., boot issues), boot into **Safe Mode** and
manually revert BCD settings:

```cmd
bcdedit /deletevalue useplatformclock
bcdedit /deletevalue tscsyncpolicy
bcdedit /deletevalue disabledynamictick
```

Then re-enable any disabled services via `services.msc`.

---

## Troubleshooting

### "This script must be run as Administrator"

Right-click PowerShell (or Terminal) and select **Run as Administrator** before
executing the script.

### Execution policy error

If you see "cannot be loaded because running scripts is disabled":

```powershell
powershell -ExecutionPolicy Bypass -File LatencyOptimizer.ps1
```

The `-ExecutionPolicy Bypass` flag overrides the policy for this session only.

### A tweak shows [-] FAIL

Some tweaks target services or registry keys that may not exist on your system
(e.g., NvTelemetryContainer on non-NVIDIA systems). Failures for hardware-specific
tweaks are expected and harmless.

### System boots slowly after applying Timer tweaks

The HPET/platform clock tweak can increase boot time on some systems. To revert:

```cmd
bcdedit /deletevalue useplatformclock
bcdedit /deletevalue tscsyncpolicy
```

### Print doesn't work after applying Services tweaks

The Print Spooler was disabled. Re-enable it:

```powershell
Set-Service -Name Spooler -StartupType Automatic
Start-Service -Name Spooler
```

### Netflix / streaming DRM doesn't work

The HDCP tweak (DPC Latency category) disables HDCP. Revert it:

**PowerShell:** Run the script with `-Restore`

**Manual:**
```powershell
Set-ItemProperty -Path "HKLM:\SYSTEM\CurrentControlSet\Control\Class\{4d36e968-e325-11ce-bfc1-08002be10318}\0000" -Name "RmHdcpEnable" -Value 1 -Type DWord
```

---

## Project Structure

```
LatencyOptimizer/
├── LatencyOptimizer.ps1    # Standalone PowerShell script (no build needed)
├── CMakeLists.txt          # Build system for the GUI (Win32 + DX11 + ImGui)
├── admin.manifest          # UAC elevation manifest (requireAdministrator)
├── README.md
├── resources/
│   ├── app.rc              # Windows resource script (icon + manifest)
│   └── LatencyOptimizer.ico
├── src/
│   ├── main.cpp            # WinMain entry, D3D11 bootstrap, tweak registration
│   ├── gui.h / gui.cpp     # ImGui DX11 interface (categories, popups, log)
│   ├── backup_manager.h/.cpp  # Restore points, registry/service backup
│   ├── utils/
│   │   ├── registry_utils.h/.cpp   # Windows Registry API wrapper
│   │   ├── service_utils.h/.cpp    # Service Control Manager wrapper
│   │   ├── cmd_utils.h/.cpp        # Command execution (powercfg, bcdedit, netsh)
│   │   └── privilege_utils.h/.cpp  # UAC elevation check and relaunch
│   └── tweaks/
│       ├── tweak_base.h            # Abstract base class for all tweaks
│       ├── services_tweaks.h/.cpp  # SysMain, WSearch, Spooler, Diag
│       ├── registry_tweaks.h/.cpp  # Visual FX, Tips, Game Bar, Cortana, Fast Startup
│       ├── power_tweaks.h/.cpp     # Ultimate Perf, USB Suspend, Core Parking
│       ├── network_tweaks.h/.cpp   # Nagle, Throttling, Auto-Tuning, ECN, Timestamps
│       ├── gpu_tweaks.h/.cpp       # NVIDIA max perf, HAGS, Fullscreen Opt
│       ├── memory_tweaks.h/.cpp    # Paging Executive, Large Cache, Compression
│       ├── timers_tweaks.h/.cpp    # HPET, Dynamic Tick
│       ├── interrupts_tweaks.h/.cpp # NIC Affinity, NIC MSI
│       ├── input_tweaks.h/.cpp     # Mouse Accel, Polling, Sticky Keys, Game Mode
│       ├── scheduler_tweaks.h/.cpp # Win32Priority, MMCSS Games, Pro Audio
│       ├── dpc_tweaks.h/.cpp       # NVIDIA DPC: HDCP, ASPM, Per-CPU, MSI, PState
│       └── misc_tweaks.h/.cpp      # Telemetry, Notifications, Background Apps
└── third_party/
    └── imgui/              # Clone Dear ImGui here (see build instructions)
```

---

## Building the GUI from Source

### Prerequisites

- **Windows 10/11** (x64)
- **Visual Studio 2022** (or 2019) with the *Desktop development with C++* workload
- **CMake 3.20+**
- **Git**

### Step 1: Clone the repository

```bat
git clone <repo-url>
cd LatencyOptimizer
```

### Step 2: Clone Dear ImGui

```bat
git clone https://github.com/ocornut/imgui.git third_party/imgui --depth=1
```

### Step 3: Configure the build

```bat
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
```

### Step 4: Build

```bat
cmake --build build --config Release
```

The executable is produced at `build\Release\LatencyOptimizer.exe`.

### Step 5: Run

Right-click `LatencyOptimizer.exe` -> **Run as Administrator**, or simply
double-click -- the embedded manifest triggers a UAC prompt automatically.

---

## Disclaimer

This tool modifies Windows registry values, boot configuration (BCD), service
startup types, and GPU driver parameters. While all changes are reversible:

- **Always create a backup** before applying tweaks (the tool does this automatically)
- **Test in a non-production environment** first if possible
- **Some tweaks increase power consumption** (disabling ASPM, core parking, GPU idle states)
- **Some tweaks disable features** (HDCP streaming, Print Spooler, Cortana)
- **Reboot is required** for most changes to take effect

The authors accept no responsibility for system instability resulting from the
use of this software.

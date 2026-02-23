# LatencyOptimizer

A Windows system-latency optimisation tool with an ImGui/DX11 GUI.
Applies **31 tweaks across 11 categories** to reduce input lag, network jitter,
and frame-time variance.  All changes are backed up and fully reversible.

---

## ASCII UI Mockup

```
┌──────────────────────────────────────────────────────────────────────────────┐
│  LatencyOptimizer v1.0                                          [About]       │
├──────────────────────────────────────────────────────────────────────────────┤
│  [Apply All Safe]  [Revert All]  [Create Restore Point]  [Export Log]        │
├────────────────┬─────────────────────────────────────────────────────────────┤
│ Categories     │ Tweak                                Status  Risk   Compat  │
│────────────────│─────────────────────────────────────────────────────────────│
│ All            │ Disable SysMain (Superfetch)         Default [Safe]          │
│ Services       │ Disable Windows Search Indexer       Applied [Safe]          │
│ Registry       │ Disable Update Delivery Optim. P2P  Default [Safe]          │
│ Power          │ Disable Visual Effects               Default [Safe]          │
│ Network        │ Disable Windows Tips & Suggestions   Default [Safe]          │
│ GPU            │ Disable Xbox Game Bar                Applied [Safe]          │
│ Memory         │ Ultimate Performance Power Plan      Default [Safe]          │
│ Timers         │ Disable USB Selective Suspend        Default [Safe]          │
│ Interrupts     │ Disable Nagle's Algorithm            Default [Safe]          │
│ Input          │ Disable Network Throttling           Default [Safe]          │
│ Scheduler      │ …                                                            │
│ Misc           │                                                              │
│                │─────────────────────────────────────────────────────────────│
│                │ Selected: Disable SysMain (Superfetch)                       │
│                │  [Apply]  [Revert]  [Details]                                │
│                │─────────────────────────────────────────────────────────────│
│                │ Log:                                                          │
│                │  10:01:33  Latency Optimizer started.                        │
│                │  10:02:15  Disable WSearch: Applied OK                       │
├────────────────┴─────────────────────────────────────────────────────────────┤
│ Status: Disable WSearch: Applied OK   RAM: 8192/16384 MB | Tweaks: 2 applied │
└──────────────────────────────────────────────────────────────────────────────┘
```

---

## Features

| Feature | Description |
|---|---|
| 31 tweaks | Across Services, Registry, Power, Network, GPU, Memory, Timers, Interrupts, Input, Scheduler, Misc |
| Backup & Restore | Manifest-based restore points before every change |
| Export Log | Save a full change history to a text file |
| Apply All Safe | One-click apply of all low-risk tweaks |
| Revert All | One-click rollback of every applied tweak |
| Detail popups | Per-tweak explanation, risk level, and compatibility |
| Admin elevation | Auto-relaunches with UAC if not already elevated |

---

## Tweak Summary (31 tweaks)

### Services (3)
| # | Tweak | Risk |
|---|---|---|
| 1 | Disable SysMain (Superfetch) | Safe |
| 2 | Disable Windows Search Indexer | Safe |
| 3 | Disable Update Delivery Optimization P2P | Safe |

### Registry (3)
| # | Tweak | Risk |
|---|---|---|
| 4 | Disable Visual Effects | Safe |
| 5 | Disable Windows Tips & Suggestions | Safe |
| 6 | Disable Xbox Game Bar | Safe |

### Power (2)
| # | Tweak | Risk |
|---|---|---|
| 7 | Ultimate Performance Power Plan | Safe |
| 8 | Disable USB Selective Suspend | Safe |

### Network (3)
| # | Tweak | Risk |
|---|---|---|
| 9 | Disable Nagle's Algorithm | Safe |
| 10 | Disable Network Throttling | Safe |
| 11 | Disable TCP Auto-Tuning | Medium |

### GPU (3)
| # | Tweak | Risk |
|---|---|---|
| 12 | NVIDIA: Max Performance + Prerendered Frames=1 | Safe (NVIDIA only) |
| 13 | Disable Hardware-Accelerated GPU Scheduling | Medium |
| 14 | Enable Hardware-Accelerated GPU Scheduling | Medium (AdvHW) |

### Memory (3)
| # | Tweak | Risk |
|---|---|---|
| 15 | Disable Paging Executive | Safe |
| 16 | Enable Large System Cache | Medium |
| 17 | Disable Memory Compression | Medium |

### Timers (2)
| # | Tweak | Risk |
|---|---|---|
| 18 | High-Resolution System Timer | Safe |
| 19 | Disable Dynamic Tick | Safe |

### Interrupts (2)
| # | Tweak | Risk |
|---|---|---|
| 20 | Network Adapter Interrupt Affinity | Medium |
| 21 | Disable MSI for Network Adapter | High (AdvHW) |

### Input (3)
| # | Tweak | Risk |
|---|---|---|
| 22 | Disable Mouse Acceleration | Safe |
| 23 | Set Mouse Polling Rate (Registry) | Safe |
| 24 | Disable Pointer Precision | Safe |

### Scheduler (3)
| # | Tweak | Risk |
|---|---|---|
| 25 | Win32 Priority Separation | Safe |
| 26 | Game CPU Priority (High) | Safe |
| 27 | SFIO Priority (MMCSS) | Safe |

### Misc (4)
| # | Tweak | Risk |
|---|---|---|
| 28 | Disable Delivery Optimization Service | Safe |
| 29 | Disable Notifications | Safe |
| 30 | Disable Background Apps | Safe |
| 31 | Disable Windows Telemetry | Safe |

---

## Project Structure

```
LatencyOptimizer/
├── CMakeLists.txt          # Build system (Win32+DX11+ImGui)
├── admin.manifest          # requireAdministrator UAC manifest
├── README.md
├── resources/
│   └── app.rc              # Icon + embedded manifest
├── src/
│   ├── main.cpp            # WinMain, D3D11 bootstrap, tweak registration
│   ├── gui.h / gui.cpp     # ImGui DX11 GUI
│   ├── backup_manager.h/.cpp
│   ├── utils/
│   │   ├── registry_utils.h/.cpp
│   │   ├── service_utils.h/.cpp
│   │   ├── cmd_utils.h/.cpp
│   │   └── privilege_utils.h/.cpp
│   └── tweaks/
│       ├── tweak_base.h
│       ├── services_tweaks.h/.cpp
│       ├── registry_tweaks.h/.cpp
│       ├── power_tweaks.h/.cpp
│       ├── network_tweaks.h/.cpp
│       ├── gpu_tweaks.h/.cpp
│       ├── memory_tweaks.h/.cpp
│       ├── timers_tweaks.h/.cpp
│       ├── interrupts_tweaks.h/.cpp
│       ├── input_tweaks.h/.cpp
│       ├── scheduler_tweaks.h/.cpp
│       └── misc_tweaks.h/.cpp
└── third_party/
    └── imgui/              # Clone Dear ImGui here (see Build Instructions)
```

---

## Build Instructions

### Prerequisites

- **Windows 10/11** (x64)
- **Visual Studio 2022** (or 2019) with the *Desktop development with C++* workload
- **CMake 3.20+**
- **Git**

### 1 – Clone Dear ImGui

```bat
git clone https://github.com/ocornut/imgui.git third_party/imgui --depth=1
```

### 2 – Configure and build

```bat
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

The executable is produced at `build/Release/LatencyOptimizer.exe`.

### 3 – Run

Right-click → *Run as Administrator*, or simply double-click — the app will
request UAC elevation automatically via the embedded manifest.

---

## Disclaimer

Some tweaks modify boot-time BCD settings (`bcdedit`) or disable system services.
Always use **Create Restore Point** before applying unfamiliar tweaks, and test in
a virtual machine or non-production environment first.  The authors accept no
responsibility for system instability resulting from the use of this software.

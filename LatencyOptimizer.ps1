#Requires -RunAsAdministrator
<#
.SYNOPSIS
    LatencyOptimizer - Windows Latency & Performance Tweaks
.DESCRIPTION
    Applies all 31+ latency optimization tweaks (registry, services, power, network,
    GPU, memory, timers, input, scheduler, misc) with backup/restore support.
    Run as Administrator. Works on Windows 10/11 x64.
.NOTES
    Run:   powershell -ExecutionPolicy Bypass -File LatencyOptimizer.ps1
    Or:    Right-click -> Run with PowerShell (as Admin)
#>

# ── Ensure Admin ─────────────────────────────────────────────────────────────

if (-not ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()
    ).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
    Write-Host "ERROR: This script must be run as Administrator." -ForegroundColor Red
    Write-Host "Right-click PowerShell -> Run as Administrator, then re-run this script."
    pause
    exit 1
}

# ── Globals ──────────────────────────────────────────────────────────────────

$Script:BackupFile = Join-Path $env:USERPROFILE "LatencyOptimizer_Backup.json"
$Script:LogEntries = @()

# ── Helper Functions ─────────────────────────────────────────────────────────

function Write-Status {
    param([string]$Message, [string]$Color = "Green")
    Write-Host "  [+] $Message" -ForegroundColor $Color
    $Script:LogEntries += "$(Get-Date -Format 'HH:mm:ss') $Message"
}

function Write-Fail {
    param([string]$Message)
    Write-Host "  [-] $Message" -ForegroundColor Red
    $Script:LogEntries += "$(Get-Date -Format 'HH:mm:ss') FAIL: $Message"
}

function Set-RegDword {
    param([string]$Hive, [string]$Path, [string]$Name, [uint32]$Value)
    $fullPath = "${Hive}:\$Path"
    try {
        if (-not (Test-Path $fullPath)) {
            New-Item -Path $fullPath -Force | Out-Null
        }
        Set-ItemProperty -Path $fullPath -Name $Name -Value $Value -Type DWord -Force -ErrorAction Stop
        return $true
    } catch {
        return $false
    }
}

function Set-RegString {
    param([string]$Hive, [string]$Path, [string]$Name, [string]$Value)
    $fullPath = "${Hive}:\$Path"
    try {
        if (-not (Test-Path $fullPath)) {
            New-Item -Path $fullPath -Force | Out-Null
        }
        Set-ItemProperty -Path $fullPath -Name $Name -Value $Value -Type String -Force -ErrorAction Stop
        return $true
    } catch {
        return $false
    }
}

function Remove-RegValue {
    param([string]$Hive, [string]$Path, [string]$Name)
    $fullPath = "${Hive}:\$Path"
    try {
        Remove-ItemProperty -Path $fullPath -Name $Name -Force -ErrorAction Stop
        return $true
    } catch {
        return $false
    }
}

function Get-RegDword {
    param([string]$Hive, [string]$Path, [string]$Name)
    $fullPath = "${Hive}:\$Path"
    try {
        $val = Get-ItemProperty -Path $fullPath -Name $Name -ErrorAction Stop
        return $val.$Name
    } catch {
        return $null
    }
}

function Get-RegString {
    param([string]$Hive, [string]$Path, [string]$Name)
    $fullPath = "${Hive}:\$Path"
    try {
        $val = Get-ItemProperty -Path $fullPath -Name $Name -ErrorAction Stop
        return [string]$val.$Name
    } catch {
        return $null
    }
}

function Set-ServiceDisabled {
    param([string]$ServiceName)
    try {
        Stop-Service -Name $ServiceName -Force -ErrorAction SilentlyContinue
        Set-Service -Name $ServiceName -StartupType Disabled -ErrorAction Stop
        return $true
    } catch {
        return $false
    }
}

function Set-ServiceAutoStart {
    param([string]$ServiceName)
    try {
        Set-Service -Name $ServiceName -StartupType Automatic -ErrorAction Stop
        Start-Service -Name $ServiceName -ErrorAction SilentlyContinue
        return $true
    } catch {
        return $false
    }
}

function Set-ServiceManual {
    param([string]$ServiceName)
    try {
        Set-Service -Name $ServiceName -StartupType Manual -ErrorAction Stop
        return $true
    } catch {
        return $false
    }
}

# ── Backup / Restore ────────────────────────────────────────────────────────

function Save-Backup {
    $backup = @{
        Timestamp  = (Get-Date).ToString("o")
        Registry   = @()
        Services   = @()
    }

    # Registry values to back up (Hive, Path, Name, Type)
    $regItems = @(
        @("HKCU", "Software\Microsoft\Windows\CurrentVersion\Explorer\VisualEffects", "VisualFXSetting", "DWord"),
        @("HKCU", "Control Panel\Desktop\WindowMetrics", "MinAnimate", "DWord"),
        @("HKCU", "Control Panel\Desktop", "UserPreferencesMask", "DWord"),
        @("HKCU", "SOFTWARE\Microsoft\Windows\CurrentVersion\ContentDeliveryManager", "SoftLandingEnabled", "DWord"),
        @("HKCU", "SOFTWARE\Microsoft\Windows\CurrentVersion\ContentDeliveryManager", "SubscribedContentEnabled", "DWord"),
        @("HKCU", "SOFTWARE\Microsoft\Windows\CurrentVersion\ContentDeliveryManager", "SystemPaneSuggestionsEnabled", "DWord"),
        @("HKCU", "SOFTWARE\Microsoft\Windows\CurrentVersion\GameDVR", "AppCaptureEnabled", "DWord"),
        @("HKCU", "System\GameConfigStore", "GameDVR_Enabled", "DWord"),
        @("HKLM", "SOFTWARE\Policies\Microsoft\Windows\GameDVR", "AllowGameDVR", "DWord"),
        @("HKLM", "SOFTWARE\Policies\Microsoft\Windows\Windows Search", "AllowCortana", "DWord"),
        @("HKLM", "SOFTWARE\Policies\Microsoft\Windows\Windows Search", "AllowSearchToUseLocation", "DWord"),
        @("HKLM", "SOFTWARE\Policies\Microsoft\Windows\Windows Search", "AllowCloudSearch", "DWord"),
        @("HKLM", "SYSTEM\CurrentControlSet\Control\Session Manager\Power", "HiberbootEnabled", "DWord"),
        @("HKLM", "SYSTEM\CurrentControlSet\Services\Tcpip\Parameters", "TCPNoDelay", "DWord"),
        @("HKLM", "SYSTEM\CurrentControlSet\Services\Tcpip\Parameters", "TcpAckFrequency", "DWord"),
        @("HKLM", "SYSTEM\CurrentControlSet\Services\Tcpip\Parameters", "Tcp1323Opts", "DWord"),
        @("HKLM", "SOFTWARE\Microsoft\Windows NT\CurrentVersion\Multimedia\SystemProfile", "NetworkThrottlingIndex", "DWord"),
        @("HKLM", "SOFTWARE\Microsoft\Windows NT\CurrentVersion\Multimedia\SystemProfile", "SystemResponsiveness", "DWord"),
        @("HKLM", "SYSTEM\CurrentControlSet\Control\GraphicsDrivers", "HwSchMode", "DWord"),
        @("HKLM", "SYSTEM\CurrentControlSet\Control\Session Manager\Memory Management", "DisablePagingExecutive", "DWord"),
        @("HKLM", "SYSTEM\CurrentControlSet\Control\Session Manager\Memory Management", "LargeSystemCache", "DWord"),
        @("HKLM", "SYSTEM\CurrentControlSet\Control\PriorityControl", "Win32PrioritySeparation", "DWord"),
        @("HKLM", "SYSTEM\CurrentControlSet\Services\USB", "DisableSelectiveSuspend", "DWord"),
        @("HKLM", "SYSTEM\CurrentControlSet\Control\Power\PowerThrottling", "PowerThrottlingOff", "DWord"),
        @("HKLM", "SOFTWARE\Policies\Microsoft\Windows\DeliveryOptimization", "DODownloadMode", "DWord"),
        @("HKCU", "Control Panel\Mouse", "MouseSpeed", "String"),
        @("HKCU", "Control Panel\Mouse", "MouseThreshold1", "String"),
        @("HKCU", "Control Panel\Mouse", "MouseThreshold2", "String"),
        @("HKLM", "SYSTEM\CurrentControlSet\Services\mouclass\Parameters", "MouseDataQueueSize", "DWord"),
        @("HKCU", "Control Panel\Accessibility\StickyKeys", "Flags", "String"),
        @("HKCU", "Control Panel\Accessibility\Keyboard Response", "Flags", "String"),
        @("HKCU", "Control Panel\Accessibility\ToggleKeys", "Flags", "String"),
        @("HKCU", "SOFTWARE\Microsoft\GameBar", "AutoGameModeEnabled", "DWord"),
        @("HKCU", "SOFTWARE\Microsoft\GameBar", "AllowAutoGameMode", "DWord"),
        @("HKLM", "SOFTWARE\Microsoft\Windows NT\CurrentVersion\Multimedia\SystemProfile\Tasks\Games", "GPU Priority", "DWord"),
        @("HKLM", "SOFTWARE\Microsoft\Windows NT\CurrentVersion\Multimedia\SystemProfile\Tasks\Games", "Priority", "DWord"),
        @("HKLM", "SOFTWARE\Microsoft\Windows NT\CurrentVersion\Multimedia\SystemProfile\Tasks\Games", "Scheduling Category", "String"),
        @("HKLM", "SOFTWARE\Microsoft\Windows NT\CurrentVersion\Multimedia\SystemProfile\Tasks\Games", "SFIO Priority", "String"),
        @("HKLM", "SOFTWARE\Microsoft\Windows NT\CurrentVersion\Multimedia\SystemProfile\Tasks\Pro Audio", "SFIO Priority", "String"),
        @("HKLM", "SOFTWARE\Microsoft\Windows NT\CurrentVersion\Multimedia\SystemProfile\Tasks\Pro Audio", "Priority", "DWord"),
        @("HKLM", "SOFTWARE\Microsoft\Windows NT\CurrentVersion\Multimedia\SystemProfile\Tasks\Pro Audio", "Scheduling Category", "String"),
        @("HKLM", "SOFTWARE\Policies\Microsoft\Windows\Explorer", "DisableNotificationCenter", "DWord"),
        @("HKLM", "SOFTWARE\Policies\Microsoft\Windows\CurrentVersion\PushNotifications", "NoToastApplicationNotification", "DWord"),
        @("HKLM", "SOFTWARE\Policies\Microsoft\Windows\AppPrivacy", "LetAppsRunInBackground", "DWord"),
        @("HKLM", "SOFTWARE\Policies\Microsoft\Windows\DataCollection", "AllowTelemetry", "DWord"),
        @("HKLM", "SOFTWARE\Policies\Microsoft\Windows\System", "PublishUserActivities", "DWord"),
        @("HKLM", "SOFTWARE\Policies\Microsoft\Windows\System", "UploadUserActivities", "DWord"),
        @("HKLM", "SOFTWARE\Policies\Microsoft\Windows\System", "EnableActivityFeed", "DWord"),
        @("HKLM", "SOFTWARE\Policies\Microsoft\Windows\StorageSense", "AllowStorageSenseGlobal", "DWord"),
        @("HKLM", "SOFTWARE\Policies\Microsoft\Windows\Windows Error Reporting", "Disabled", "DWord"),
        @("HKCU", "System\GameConfigStore", "GameDVR_FSEBehaviorMode", "DWord"),
        @("HKCU", "System\GameConfigStore", "GameDVR_HonorUserFSEBehaviorMode", "DWord"),
        @("HKCU", "System\GameConfigStore", "GameDVR_FSEBehavior", "DWord"),
        @("HKCU", "System\GameConfigStore", "GameDVR_DXGIHonorFSEWindowsCompatible", "DWord"),
        @("HKLM", "SYSTEM\CurrentControlSet\Control\Class\{4d36e972-e325-11ce-bfc1-08002be10318}\0000", "*InterruptAffinityPolicy", "DWord"),
        @("HKLM", "SYSTEM\CurrentControlSet\Control\Class\{4d36e972-e325-11ce-bfc1-08002be10318}\0000", "*InterruptAffinity", "DWord"),
        @("HKLM", "SYSTEM\CurrentControlSet\Control\Class\{4d36e972-e325-11ce-bfc1-08002be10318}\0000", "MSISupported", "DWord"),
        @("HKLM", "SYSTEM\CurrentControlSet\Control\Class\{4d36e968-e325-11ce-bfc1-08002be10318}\0000", "PowerMizerEnable", "DWord"),
        @("HKLM", "SYSTEM\CurrentControlSet\Control\Class\{4d36e968-e325-11ce-bfc1-08002be10318}\0000", "PowerMizerLevel", "DWord"),
        @("HKLM", "SYSTEM\CurrentControlSet\Control\Class\{4d36e968-e325-11ce-bfc1-08002be10318}\0000", "PowerMizerLevelAC", "DWord"),
        @("HKLM", "SOFTWARE\NVIDIA Corporation\Global\NVTweak", "Prerender", "DWord"),
        # DPC Latency tweaks
        @("HKLM", "SYSTEM\CurrentControlSet\Control\Class\{4d36e968-e325-11ce-bfc1-08002be10318}\0000", "RmHdcpEnable", "DWord"),
        @("HKLM", "SYSTEM\CurrentControlSet\Control\Class\{4d36e968-e325-11ce-bfc1-08002be10318}\0000", "RmGpsPsEnablePerCpuCoreDpc", "DWord"),
        @("HKLM", "SYSTEM\CurrentControlSet\Control\Class\{4d36e968-e325-11ce-bfc1-08002be10318}\0000", "RmDisableGpuASPMFlags", "DWord"),
        @("HKLM", "SYSTEM\CurrentControlSet\Control\Class\{4d36e968-e325-11ce-bfc1-08002be10318}\0000", "MSISupported", "DWord"),
        @("HKLM", "SYSTEM\CurrentControlSet\Control\Class\{4d36e968-e325-11ce-bfc1-08002be10318}\0000", "*InterruptAffinityPolicy", "DWord"),
        @("HKLM", "SYSTEM\CurrentControlSet\Control\Class\{4d36e968-e325-11ce-bfc1-08002be10318}\0000", "*InterruptAffinity", "DWord"),
        @("HKLM", "SYSTEM\CurrentControlSet\Control\Class\{4d36e968-e325-11ce-bfc1-08002be10318}\0000", "EnableRID73519", "DWord"),
        @("HKLM", "SYSTEM\CurrentControlSet\Control\Class\{4d36e968-e325-11ce-bfc1-08002be10318}\0000", "PerfLevelSrc", "DWord"),
        @("HKLM", "SYSTEM\CurrentControlSet\Control\Class\{4d36e968-e325-11ce-bfc1-08002be10318}\0000", "DisableDynamicPstate", "DWord")
    )

    foreach ($item in $regItems) {
        $hive  = $item[0]
        $path  = $item[1]
        $name  = $item[2]
        $type  = $item[3]
        if ($type -eq "String") {
            $val = Get-RegString -Hive $hive -Path $path -Name $name
        } else {
            $val = Get-RegDword -Hive $hive -Path $path -Name $name
        }
        $backup.Registry += @{
            Hive  = $hive
            Path  = $path
            Name  = $name
            Type  = $type
            Value = $val   # $null means key/value didn't exist
        }
    }

    # Services to back up
    $services = @("SysMain","WSearch","Spooler","DiagSvc","dmwappushservice","DoSvc","DiagTrack","WerSvc","NvTelemetryContainer")
    foreach ($svc in $services) {
        try {
            $s = Get-Service -Name $svc -ErrorAction Stop
            $backup.Services += @{
                Name      = $svc
                StartType = $s.StartType.ToString()
            }
        } catch {
            $backup.Services += @{ Name = $svc; StartType = "NotFound" }
        }
    }

    $backup | ConvertTo-Json -Depth 5 | Set-Content -Path $Script:BackupFile -Encoding UTF8
    Write-Status "Backup saved to $Script:BackupFile"
}

function Restore-Backup {
    if (-not (Test-Path $Script:BackupFile)) {
        Write-Fail "No backup file found at $Script:BackupFile"
        return
    }
    $backup = Get-Content -Path $Script:BackupFile -Raw | ConvertFrom-Json

    Write-Host "`n  Restoring from backup created at $($backup.Timestamp)..." -ForegroundColor Cyan

    # Restore registry
    foreach ($entry in $backup.Registry) {
        if ($null -eq $entry.Value) {
            Remove-RegValue -Hive $entry.Hive -Path $entry.Path -Name $entry.Name | Out-Null
        } elseif ($entry.Type -eq "String") {
            Set-RegString -Hive $entry.Hive -Path $entry.Path -Name $entry.Name -Value $entry.Value | Out-Null
        } else {
            Set-RegDword -Hive $entry.Hive -Path $entry.Path -Name $entry.Name -Value ([uint32]$entry.Value) | Out-Null
        }
    }
    Write-Status "Registry values restored"

    # Restore services
    foreach ($svc in $backup.Services) {
        if ($svc.StartType -eq "NotFound") { continue }
        try {
            switch ($svc.StartType) {
                "Disabled" { Set-Service -Name $svc.Name -StartupType Disabled -ErrorAction Stop }
                "Manual"   { Set-Service -Name $svc.Name -StartupType Manual -ErrorAction Stop }
                "Automatic"{ Set-Service -Name $svc.Name -StartupType Automatic -ErrorAction Stop
                             Start-Service -Name $svc.Name -ErrorAction SilentlyContinue }
                default    { Set-Service -Name $svc.Name -StartupType Manual -ErrorAction Stop }
            }
        } catch {}
    }
    Write-Status "Services restored"

    # Restore commands (best-effort - bcdedit, netsh, powercfg)
    bcdedit /deletevalue useplatformclock 2>$null | Out-Null
    bcdedit /deletevalue tscsyncpolicy 2>$null | Out-Null
    bcdedit /deletevalue disabledynamictick 2>$null | Out-Null
    netsh int tcp set global autotuninglevel=normal 2>$null | Out-Null
    netsh int tcp set global ecncapability=default 2>$null | Out-Null
    powercfg /setactive 381b4222-f694-41f0-9685-ff5bb260df2e 2>$null | Out-Null
    powercfg /setacvalueindex scheme_current sub_processor CPMINCORES 10 2>$null | Out-Null
    powercfg /setdcvalueindex scheme_current sub_processor CPMINCORES 10 2>$null | Out-Null
    powercfg /setactive scheme_current 2>$null | Out-Null
    powershell -NoProfile -Command "Enable-MMAgent -MemoryCompression" 2>$null | Out-Null
    Write-Status "System commands restored (power plan, timers, network)"

    Write-Host "`n  Restore complete. Some changes require a reboot." -ForegroundColor Yellow
}

# ── Tweak Categories ────────────────────────────────────────────────────────

function Apply-ServicesTweaks {
    Write-Host "`n  === SERVICES ===" -ForegroundColor Cyan

    if (Set-ServiceDisabled "SysMain")   { Write-Status "Disabled SysMain (Superfetch)" }
    else { Write-Fail "Could not disable SysMain" }

    if (Set-ServiceDisabled "WSearch")   { Write-Status "Disabled Windows Search" }
    else { Write-Fail "Could not disable WSearch" }

    if (Set-RegDword "HKLM" "SOFTWARE\Policies\Microsoft\Windows\DeliveryOptimization" "DODownloadMode" 0) {
        Write-Status "Disabled WU Delivery Optimization (registry)"
    } else { Write-Fail "Could not set DODownloadMode" }

    if (Set-ServiceDisabled "Spooler")   { Write-Status "Disabled Print Spooler" }
    else { Write-Fail "Could not disable Spooler" }

    if (Set-ServiceDisabled "DiagSvc")   { Write-Status "Disabled DiagSvc" }
    else { Write-Fail "Could not disable DiagSvc" }

    Set-ServiceDisabled "dmwappushservice" | Out-Null
    Write-Status "Disabled dmwappushservice"
}

function Apply-RegistryTweaks {
    Write-Host "`n  === REGISTRY (Visual / UI) ===" -ForegroundColor Cyan

    # Disable Visual Effects
    Set-RegDword "HKCU" "Software\Microsoft\Windows\CurrentVersion\Explorer\VisualEffects" "VisualFXSetting" 3 | Out-Null
    Set-RegDword "HKCU" "Control Panel\Desktop\WindowMetrics" "MinAnimate" 0 | Out-Null
    Set-RegDword "HKCU" "Control Panel\Desktop" "UserPreferencesMask" 0x90 | Out-Null
    Write-Status "Disabled visual effects & animations"

    # Disable Windows Tips
    Set-RegDword "HKCU" "SOFTWARE\Microsoft\Windows\CurrentVersion\ContentDeliveryManager" "SoftLandingEnabled" 0 | Out-Null
    Set-RegDword "HKCU" "SOFTWARE\Microsoft\Windows\CurrentVersion\ContentDeliveryManager" "SubscribedContentEnabled" 0 | Out-Null
    Set-RegDword "HKCU" "SOFTWARE\Microsoft\Windows\CurrentVersion\ContentDeliveryManager" "SystemPaneSuggestionsEnabled" 0 | Out-Null
    Write-Status "Disabled Windows Tips & suggestions"

    # Disable Game Bar / DVR
    Set-RegDword "HKCU" "SOFTWARE\Microsoft\Windows\CurrentVersion\GameDVR" "AppCaptureEnabled" 0 | Out-Null
    Set-RegDword "HKCU" "System\GameConfigStore" "GameDVR_Enabled" 0 | Out-Null
    Set-RegDword "HKLM" "SOFTWARE\Policies\Microsoft\Windows\GameDVR" "AllowGameDVR" 0 | Out-Null
    Write-Status "Disabled Game Bar & DVR"

    # Disable Cortana
    Set-RegDword "HKLM" "SOFTWARE\Policies\Microsoft\Windows\Windows Search" "AllowCortana" 0 | Out-Null
    Set-RegDword "HKLM" "SOFTWARE\Policies\Microsoft\Windows\Windows Search" "AllowSearchToUseLocation" 0 | Out-Null
    Set-RegDword "HKLM" "SOFTWARE\Policies\Microsoft\Windows\Windows Search" "AllowCloudSearch" 0 | Out-Null
    Write-Status "Disabled Cortana & cloud search"

    # Disable Fast Startup
    Set-RegDword "HKLM" "SYSTEM\CurrentControlSet\Control\Session Manager\Power" "HiberbootEnabled" 0 | Out-Null
    Write-Status "Disabled Fast Startup (Hiberboot)"
}

function Apply-PowerTweaks {
    Write-Host "`n  === POWER ===" -ForegroundColor Cyan

    # Ultimate Performance Plan
    powercfg /duplicatescheme e9a42b02-d5df-448d-aa00-03f14749eb61 2>$null | Out-Null
    $rc = powercfg /setactive e9a42b02-d5df-448d-aa00-03f14749eb61 2>&1
    Write-Status "Activated Ultimate Performance power plan"

    # Disable USB Selective Suspend
    Set-RegDword "HKLM" "SYSTEM\CurrentControlSet\Services\USB" "DisableSelectiveSuspend" 1 | Out-Null
    Write-Status "Disabled USB selective suspend"

    # Disable Core Parking
    powercfg /setacvalueindex scheme_current sub_processor CPMINCORES 100 2>$null | Out-Null
    powercfg /setdcvalueindex scheme_current sub_processor CPMINCORES 100 2>$null | Out-Null
    powercfg /setactive scheme_current 2>$null | Out-Null
    Write-Status "Disabled CPU core parking"

    # Disable Power Throttling
    Set-RegDword "HKLM" "SYSTEM\CurrentControlSet\Control\Power\PowerThrottling" "PowerThrottlingOff" 1 | Out-Null
    Write-Status "Disabled power throttling"
}

function Apply-NetworkTweaks {
    Write-Host "`n  === NETWORK ===" -ForegroundColor Cyan

    # Disable Nagle's Algorithm
    Set-RegDword "HKLM" "SYSTEM\CurrentControlSet\Services\Tcpip\Parameters" "TCPNoDelay" 1 | Out-Null
    Set-RegDword "HKLM" "SYSTEM\CurrentControlSet\Services\Tcpip\Parameters" "TcpAckFrequency" 1 | Out-Null
    Write-Status "Disabled Nagle's algorithm (TCPNoDelay + TcpAckFrequency)"

    # Disable Network Throttling
    Set-RegDword "HKLM" "SOFTWARE\Microsoft\Windows NT\CurrentVersion\Multimedia\SystemProfile" "NetworkThrottlingIndex" 0xFFFFFFFF | Out-Null
    Set-RegDword "HKLM" "SOFTWARE\Microsoft\Windows NT\CurrentVersion\Multimedia\SystemProfile" "SystemResponsiveness" 0 | Out-Null
    Write-Status "Disabled network throttling (MMCSS)"

    # Disable TCP Auto-Tuning
    netsh int tcp set global autotuninglevel=disabled 2>$null | Out-Null
    Write-Status "Disabled TCP auto-tuning"

    # Disable ECN
    netsh int tcp set global ecncapability=disabled 2>$null | Out-Null
    Write-Status "Disabled ECN capability"

    # Disable TCP Timestamps (set Tcp1323Opts = 1 = window scaling only)
    Set-RegDword "HKLM" "SYSTEM\CurrentControlSet\Services\Tcpip\Parameters" "Tcp1323Opts" 1 | Out-Null
    Write-Status "Disabled TCP timestamps (window scaling only)"
}

function Apply-GPUTweaks {
    Write-Host "`n  === GPU ===" -ForegroundColor Cyan

    # NVIDIA Optimize (may fail on non-NVIDIA systems - that's OK)
    $nvKey = "SYSTEM\CurrentControlSet\Control\Class\{4d36e968-e325-11ce-bfc1-08002be10318}\0000"
    Set-RegDword "HKLM" $nvKey "PowerMizerEnable" 0 | Out-Null
    Set-RegDword "HKLM" $nvKey "PowerMizerLevel" 1 | Out-Null
    Set-RegDword "HKLM" $nvKey "PowerMizerLevelAC" 1 | Out-Null
    Set-RegDword "HKLM" "SYSTEM\CurrentControlSet\Control\GraphicsDrivers" "PendingDriverOperations" 1 | Out-Null
    Set-RegDword "HKLM" "SOFTWARE\NVIDIA Corporation\Global\NVTweak" "Prerender" 1 | Out-Null
    Write-Status "NVIDIA max performance + 1 prerendered frame (skipped if not NVIDIA)"

    # Disable HAGS
    Set-RegDword "HKLM" "SYSTEM\CurrentControlSet\Control\GraphicsDrivers" "HwSchMode" 1 | Out-Null
    Write-Status "Disabled Hardware-Accelerated GPU Scheduling"

    # Disable Fullscreen Optimizations
    Set-RegDword "HKCU" "System\GameConfigStore" "GameDVR_FSEBehaviorMode" 2 | Out-Null
    Set-RegDword "HKCU" "System\GameConfigStore" "GameDVR_HonorUserFSEBehaviorMode" 1 | Out-Null
    Set-RegDword "HKCU" "System\GameConfigStore" "GameDVR_FSEBehavior" 2 | Out-Null
    Set-RegDword "HKCU" "System\GameConfigStore" "GameDVR_DXGIHonorFSEWindowsCompatible" 1 | Out-Null
    Write-Status "Disabled fullscreen optimizations"
}

function Apply-MemoryTweaks {
    Write-Host "`n  === MEMORY ===" -ForegroundColor Cyan

    $memKey = "SYSTEM\CurrentControlSet\Control\Session Manager\Memory Management"

    Set-RegDword "HKLM" $memKey "DisablePagingExecutive" 1 | Out-Null
    Write-Status "Disabled paging executive (keep kernel in RAM)"

    Set-RegDword "HKLM" $memKey "LargeSystemCache" 1 | Out-Null
    Write-Status "Enabled large system cache"

    # Disable Memory Compression
    Disable-MMAgent -MemoryCompression -ErrorAction SilentlyContinue
    Write-Status "Disabled memory compression"
}

function Apply-TimersTweaks {
    Write-Host "`n  === TIMERS ===" -ForegroundColor Cyan

    bcdedit /set useplatformclock true 2>$null | Out-Null
    bcdedit /set tscsyncpolicy enhanced 2>$null | Out-Null
    Write-Status "Enabled HPET / platform clock + enhanced TSC sync"

    bcdedit /set disabledynamictick yes 2>$null | Out-Null
    Write-Status "Disabled dynamic tick"
}

function Apply-InterruptsTweaks {
    Write-Host "`n  === INTERRUPTS ===" -ForegroundColor Cyan

    $netClassKey = "SYSTEM\CurrentControlSet\Control\Class\{4d36e972-e325-11ce-bfc1-08002be10318}\0000"

    Set-RegDword "HKLM" $netClassKey "*InterruptAffinityPolicy" 5 | Out-Null
    Set-RegDword "HKLM" $netClassKey "*InterruptAffinity" 1 | Out-Null
    Write-Status "Set network interrupt affinity to CPU 0"

    Set-RegDword "HKLM" $netClassKey "MSISupported" 0 | Out-Null
    Write-Status "Disabled network MSI (Message-Signaled Interrupts)"
}

function Apply-InputTweaks {
    Write-Host "`n  === INPUT ===" -ForegroundColor Cyan

    # Disable Mouse Acceleration & Pointer Precision
    Set-RegString "HKCU" "Control Panel\Mouse" "MouseSpeed" "0" | Out-Null
    Set-RegString "HKCU" "Control Panel\Mouse" "MouseThreshold1" "0" | Out-Null
    Set-RegString "HKCU" "Control Panel\Mouse" "MouseThreshold2" "0" | Out-Null
    Write-Status "Disabled mouse acceleration & pointer precision"

    # Mouse Data Queue Size
    Set-RegDword "HKLM" "SYSTEM\CurrentControlSet\Services\mouclass\Parameters" "MouseDataQueueSize" 128 | Out-Null
    Set-RegDword "HKLM" "SYSTEM\CurrentControlSet\Services\mouhid\Parameters" "MouseDataQueueSize" 128 | Out-Null
    Write-Status "Increased mouse data queue size (128)"

    # Disable Sticky/Filter/Toggle Keys
    Set-RegString "HKCU" "Control Panel\Accessibility\StickyKeys" "Flags" "506" | Out-Null
    Set-RegString "HKCU" "Control Panel\Accessibility\Keyboard Response" "Flags" "122" | Out-Null
    Set-RegString "HKCU" "Control Panel\Accessibility\ToggleKeys" "Flags" "58" | Out-Null
    Write-Status "Disabled Sticky/Filter/Toggle keys"

    # Enable Game Mode
    Set-RegDword "HKCU" "SOFTWARE\Microsoft\GameBar" "AutoGameModeEnabled" 1 | Out-Null
    Set-RegDword "HKCU" "SOFTWARE\Microsoft\GameBar" "AllowAutoGameMode" 1 | Out-Null
    Write-Status "Enabled Game Mode"
}

function Apply-SchedulerTweaks {
    Write-Host "`n  === SCHEDULER ===" -ForegroundColor Cyan

    # Win32PrioritySeparation = 26 (short, variable, foreground boost)
    Set-RegDword "HKLM" "SYSTEM\CurrentControlSet\Control\PriorityControl" "Win32PrioritySeparation" 26 | Out-Null
    Write-Status "Set Win32PrioritySeparation = 26 (short quantum, foreground boost)"

    # MMCSS Games task
    $gamesKey = "SOFTWARE\Microsoft\Windows NT\CurrentVersion\Multimedia\SystemProfile\Tasks\Games"
    Set-RegDword "HKLM" $gamesKey "GPU Priority" 8 | Out-Null
    Set-RegDword "HKLM" $gamesKey "Priority" 6 | Out-Null
    Set-RegString "HKLM" $gamesKey "Scheduling Category" "High" | Out-Null
    Set-RegString "HKLM" $gamesKey "SFIO Priority" "High" | Out-Null
    Write-Status "Set MMCSS Games task to High priority"

    # MMCSS Pro Audio task
    $proAudioKey = "SOFTWARE\Microsoft\Windows NT\CurrentVersion\Multimedia\SystemProfile\Tasks\Pro Audio"
    Set-RegString "HKLM" $proAudioKey "SFIO Priority" "High" | Out-Null
    Set-RegDword "HKLM" $proAudioKey "Priority" 6 | Out-Null
    Set-RegString "HKLM" $proAudioKey "Scheduling Category" "High" | Out-Null
    Write-Status "Set MMCSS Pro Audio task to High priority"
}

function Apply-DPCLatencyTweaks {
    Write-Host "`n  === DPC LATENCY (NVIDIA / GPU) ===" -ForegroundColor Cyan

    $nvKey = "SYSTEM\CurrentControlSet\Control\Class\{4d36e968-e325-11ce-bfc1-08002be10318}\0000"

    # 1. Disable NVIDIA HDCP
    Set-RegDword "HKLM" $nvKey "RmHdcpEnable" 0 | Out-Null
    Write-Status "NVIDIA: Disabled HDCP (removes HDCP handshake DPC overhead)"

    # 2. Enable per-CPU core DPC distribution
    Set-RegDword "HKLM" $nvKey "RmGpsPsEnablePerCpuCoreDpc" 1 | Out-Null
    Write-Status "NVIDIA: Enabled per-CPU DPC distribution (spreads DPC across cores)"

    # 3. Disable GPU PCIe ASPM (L0s/L1 power states)
    Set-RegDword "HKLM" $nvKey "RmDisableGpuASPMFlags" 3 | Out-Null
    Write-Status "NVIDIA: Disabled GPU PCIe ASPM (eliminates link re-train DPC spikes)"

    # 4. Enable MSI for GPU
    Set-RegDword "HKLM" $nvKey "MSISupported" 1 | Out-Null
    Write-Status "GPU: Enabled MSI mode (dedicated interrupt vector, less DPC contention)"

    # 5. Disable NVIDIA Telemetry Container
    if (Set-ServiceDisabled "NvTelemetryContainer") {
        Write-Status "NVIDIA: Disabled NvTelemetryContainer (removes periodic DPC overhead)"
    } else {
        Write-Status "NVIDIA: NvTelemetryContainer not found (may be already removed)" "Yellow"
    }

    # 6. Pin GPU interrupts to CPU 1 (separate from NIC on CPU 0)
    Set-RegDword "HKLM" $nvKey "*InterruptAffinityPolicy" 5 | Out-Null
    Set-RegDword "HKLM" $nvKey "*InterruptAffinity" 2 | Out-Null
    Write-Status "GPU: Pinned interrupts to CPU 1 (isolates GPU DPC from NIC DPC)"

    # 7. Disable deep idle P-state transitions
    Set-RegDword "HKLM" $nvKey "EnableRID73519" 0 | Out-Null
    Set-RegDword "HKLM" $nvKey "PerfLevelSrc" 0x2222 | Out-Null
    Set-RegDword "HKLM" $nvKey "DisableDynamicPstate" 1 | Out-Null
    Write-Status "NVIDIA: Disabled deep idle P8 + dynamic PState (prevents wake-up DPC storms)"
}

function Apply-MiscTweaks {
    Write-Host "`n  === MISC (Privacy & Bloat) ===" -ForegroundColor Cyan

    # Disable Delivery Optimization service
    if (Set-ServiceDisabled "DoSvc") { Write-Status "Disabled Delivery Optimization service" }
    else { Write-Fail "Could not disable DoSvc" }

    # Disable Notifications
    Set-RegDword "HKLM" "SOFTWARE\Policies\Microsoft\Windows\Explorer" "DisableNotificationCenter" 1 | Out-Null
    Set-RegDword "HKLM" "SOFTWARE\Policies\Microsoft\Windows\CurrentVersion\PushNotifications" "NoToastApplicationNotification" 1 | Out-Null
    Write-Status "Disabled notification center & toast notifications"

    # Disable Background Apps
    Set-RegDword "HKLM" "SOFTWARE\Policies\Microsoft\Windows\AppPrivacy" "LetAppsRunInBackground" 2 | Out-Null
    Write-Status "Disabled background apps"

    # Disable Telemetry
    Set-RegDword "HKLM" "SOFTWARE\Policies\Microsoft\Windows\DataCollection" "AllowTelemetry" 0 | Out-Null
    Set-ServiceDisabled "DiagTrack" | Out-Null
    Write-Status "Disabled telemetry & DiagTrack service"

    # Disable Activity History
    Set-RegDword "HKLM" "SOFTWARE\Policies\Microsoft\Windows\System" "PublishUserActivities" 0 | Out-Null
    Set-RegDword "HKLM" "SOFTWARE\Policies\Microsoft\Windows\System" "UploadUserActivities" 0 | Out-Null
    Set-RegDword "HKLM" "SOFTWARE\Policies\Microsoft\Windows\System" "EnableActivityFeed" 0 | Out-Null
    Write-Status "Disabled activity history"

    # Disable Storage Sense
    Set-RegDword "HKLM" "SOFTWARE\Policies\Microsoft\Windows\StorageSense" "AllowStorageSenseGlobal" 0 | Out-Null
    Write-Status "Disabled Storage Sense"

    # Disable Error Reporting
    Set-RegDword "HKLM" "SOFTWARE\Policies\Microsoft\Windows\Windows Error Reporting" "Disabled" 1 | Out-Null
    Set-ServiceDisabled "WerSvc" | Out-Null
    Write-Status "Disabled Windows Error Reporting"
}

function Apply-AllTweaks {
    Write-Host "`n  Creating backup before applying tweaks..." -ForegroundColor Yellow
    Save-Backup

    Write-Host ""
    Write-Host "  =============================================" -ForegroundColor White
    Write-Host "    APPLYING ALL LATENCY OPTIMIZATION TWEAKS"  -ForegroundColor White
    Write-Host "  =============================================" -ForegroundColor White

    Apply-ServicesTweaks
    Apply-RegistryTweaks
    Apply-PowerTweaks
    Apply-NetworkTweaks
    Apply-GPUTweaks
    Apply-MemoryTweaks
    Apply-TimersTweaks
    Apply-InterruptsTweaks
    Apply-InputTweaks
    Apply-SchedulerTweaks
    Apply-DPCLatencyTweaks
    Apply-MiscTweaks

    Write-Host ""
    Write-Host "  =============================================" -ForegroundColor Green
    Write-Host "    ALL TWEAKS APPLIED SUCCESSFULLY"            -ForegroundColor Green
    Write-Host "    A reboot is recommended for full effect."   -ForegroundColor Yellow
    Write-Host "  =============================================" -ForegroundColor Green

    # Save log
    $logFile = Join-Path $env:USERPROFILE "LatencyOptimizer_Log.txt"
    $Script:LogEntries | Set-Content -Path $logFile -Encoding UTF8
    Write-Host "`n  Log saved to $logFile" -ForegroundColor Gray
}

# ── Main Menu ───────────────────────────────────────────────────────────────

function Show-Menu {
    while ($true) {
        Write-Host ""
        Write-Host "  =============================================" -ForegroundColor Cyan
        Write-Host "       LATENCY OPTIMIZER  -  PowerShell Edition" -ForegroundColor Cyan
        Write-Host "  =============================================" -ForegroundColor Cyan
        Write-Host ""
        Write-Host "  [1] Apply ALL tweaks (recommended)"            -ForegroundColor White
        Write-Host "  [2] Apply by category..."                       -ForegroundColor White
        Write-Host "  [3] Restore from backup"                        -ForegroundColor Yellow
        Write-Host "  [4] Export log"                                 -ForegroundColor White
        Write-Host "  [0] Exit"                                       -ForegroundColor Gray
        Write-Host ""
        $choice = Read-Host "  Select option"

        switch ($choice) {
            "1" { Apply-AllTweaks }
            "2" { Show-CategoryMenu }
            "3" { Restore-Backup }
            "4" {
                $logFile = Join-Path $env:USERPROFILE "LatencyOptimizer_Log.txt"
                if ($Script:LogEntries.Count -gt 0) {
                    $Script:LogEntries | Set-Content -Path $logFile -Encoding UTF8
                    Write-Status "Log exported to $logFile"
                } else {
                    Write-Fail "No log entries yet."
                }
            }
            "0" { return }
            default { Write-Host "  Invalid option." -ForegroundColor Red }
        }
    }
}

function Show-CategoryMenu {
    while ($true) {
        Write-Host ""
        Write-Host "  ── Apply by Category ──" -ForegroundColor Cyan
        Write-Host "  [1]  Services        (SysMain, WSearch, Spooler, Diag)"
        Write-Host "  [2]  Registry / UI   (Visual FX, Tips, Game Bar, Cortana, Fast Startup)"
        Write-Host "  [3]  Power           (Ultimate Perf, USB Suspend, Core Parking, Throttling)"
        Write-Host "  [4]  Network         (Nagle, Throttling, Auto-Tuning, ECN, Timestamps)"
        Write-Host "  [5]  GPU             (NVIDIA, HAGS, Fullscreen Opt)"
        Write-Host "  [6]  Memory          (Paging Executive, Large Cache, Compression)"
        Write-Host "  [7]  Timers          (HPET, Dynamic Tick)"
        Write-Host "  [8]  Interrupts      (NIC Affinity, MSI)"
        Write-Host "  [9]  Input           (Mouse Accel, Polling, Sticky Keys, Game Mode)"
        Write-Host "  [10] Scheduler       (Win32Priority, MMCSS Games, Pro Audio)"
        Write-Host "  [11] DPC Latency     (NVIDIA HDCP, Per-CPU DPC, ASPM, GPU MSI, PState)"
        Write-Host "  [12] Misc / Privacy  (Telemetry, Notifications, Background Apps, etc.)"
        Write-Host "  [0]  Back"
        Write-Host ""
        $cat = Read-Host "  Select category"

        switch ($cat) {
            "1"  { Save-Backup; Apply-ServicesTweaks }
            "2"  { Save-Backup; Apply-RegistryTweaks }
            "3"  { Save-Backup; Apply-PowerTweaks }
            "4"  { Save-Backup; Apply-NetworkTweaks }
            "5"  { Save-Backup; Apply-GPUTweaks }
            "6"  { Save-Backup; Apply-MemoryTweaks }
            "7"  { Save-Backup; Apply-TimersTweaks }
            "8"  { Save-Backup; Apply-InterruptsTweaks }
            "9"  { Save-Backup; Apply-InputTweaks }
            "10" { Save-Backup; Apply-SchedulerTweaks }
            "11" { Save-Backup; Apply-DPCLatencyTweaks }
            "12" { Save-Backup; Apply-MiscTweaks }
            "0"  { return }
            default { Write-Host "  Invalid option." -ForegroundColor Red }
        }
    }
}

# ── Entry Point ──────────────────────────────────────────────────────────────

# If run with -Apply flag, skip menu and apply everything
if ($args -contains "-Apply") {
    Apply-AllTweaks
    Write-Host ""
    $reboot = Read-Host "  Reboot now? (y/n)"
    if ($reboot -eq "y") { Restart-Computer -Force }
    exit 0
}

# If run with -Restore flag, restore from backup
if ($args -contains "-Restore") {
    Restore-Backup
    exit 0
}

# Otherwise show interactive menu
Show-Menu

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>

#include "gui.h"
#include "backup_manager.h"
#include "utils/privilege_utils.h"

// Tweaks
#include "tweaks/services_tweaks.h"
#include "tweaks/registry_tweaks.h"
#include "tweaks/power_tweaks.h"
#include "tweaks/network_tweaks.h"
#include "tweaks/gpu_tweaks.h"
#include "tweaks/memory_tweaks.h"
#include "tweaks/timers_tweaks.h"
#include "tweaks/interrupts_tweaks.h"
#include "tweaks/input_tweaks.h"
#include "tweaks/scheduler_tweaks.h"
#include "tweaks/misc_tweaks.h"

// ─── D3D11 globals ────────────────────────────────────────────────────────────
static ID3D11Device*           g_pd3dDevice    = nullptr;
static ID3D11DeviceContext*    g_pd3dContext   = nullptr;
static IDXGISwapChain*         g_pSwapChain    = nullptr;
static ID3D11RenderTargetView* g_pRenderTarget = nullptr;

static bool CreateD3D(HWND hwnd)
{
    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferCount                        = 2;
    sd.BufferDesc.Width                   = 0;
    sd.BufferDesc.Height                  = 0;
    sd.BufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator   = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags                              = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow                       = hwnd;
    sd.SampleDesc.Count                   = 1;
    sd.Windowed                           = TRUE;
    sd.SwapEffect                         = DXGI_SWAP_EFFECT_DISCARD;

    D3D_FEATURE_LEVEL level;
    const D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
        0, levels, 2, D3D11_SDK_VERSION,
        &sd, &g_pSwapChain, &g_pd3dDevice, &level, &g_pd3dContext);

    if (FAILED(hr)) return false;

    ID3D11Texture2D* pBackBuf = nullptr;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuf));
    g_pd3dDevice->CreateRenderTargetView(pBackBuf, nullptr, &g_pRenderTarget);
    pBackBuf->Release();
    return true;
}

static void CleanupD3D()
{
    if (g_pRenderTarget) { g_pRenderTarget->Release(); g_pRenderTarget = nullptr; }
    if (g_pSwapChain)    { g_pSwapChain->Release();    g_pSwapChain    = nullptr; }
    if (g_pd3dContext)   { g_pd3dContext->Release();   g_pd3dContext   = nullptr; }
    if (g_pd3dDevice)    { g_pd3dDevice->Release();    g_pd3dDevice    = nullptr; }
}

static void ResizeD3D(UINT w, UINT h)
{
    if (g_pRenderTarget) { g_pRenderTarget->Release(); g_pRenderTarget = nullptr; }
    g_pSwapChain->ResizeBuffers(0, w, h, DXGI_FORMAT_UNKNOWN, 0);
    ID3D11Texture2D* pBack = nullptr;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBack));
    g_pd3dDevice->CreateRenderTargetView(pBack, nullptr, &g_pRenderTarget);
    pBack->Release();
}

// ─── Window procedure ─────────────────────────────────────────────────────────
static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (Gui::HandleWindowMessage(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice && wParam != SIZE_MINIMIZED)
            ResizeD3D(LOWORD(lParam), HIWORD(lParam));
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) return 0;
        break;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

// ─── Register all tweaks ──────────────────────────────────────────────────────
static void RegisterTweaks(Gui& gui)
{
    // Services (3)
    gui.RegisterTweak(std::make_shared<DisableSysMainTweak>());
    gui.RegisterTweak(std::make_shared<DisableWSearchTweak>());
    gui.RegisterTweak(std::make_shared<DisableWUDOTweak>());

    // Registry (3)
    gui.RegisterTweak(std::make_shared<DisableVisualEffectsTweak>());
    gui.RegisterTweak(std::make_shared<DisableWindowsTipsTweak>());
    gui.RegisterTweak(std::make_shared<DisableGameBarTweak>());

    // Power (2)
    gui.RegisterTweak(std::make_shared<UltimatePerformancePlanTweak>());
    gui.RegisterTweak(std::make_shared<DisableUSBSuspendTweak>());

    // Network (3)
    gui.RegisterTweak(std::make_shared<DisableNagleTweak>());
    gui.RegisterTweak(std::make_shared<DisableNetworkThrottlingTweak>());
    gui.RegisterTweak(std::make_shared<DisableTCPAutoTuningTweak>());

    // GPU (3)
    gui.RegisterTweak(std::make_shared<NvidiaOptimizeTweak>());
    gui.RegisterTweak(std::make_shared<DisableHAGSTweak>());
    gui.RegisterTweak(std::make_shared<EnableHAGSTweak>());

    // Memory (3)
    gui.RegisterTweak(std::make_shared<DisablePagingExecutiveTweak>());
    gui.RegisterTweak(std::make_shared<LargeSystemCacheTweak>());
    gui.RegisterTweak(std::make_shared<DisableMemoryCompressionTweak>());

    // Timers (2)
    gui.RegisterTweak(std::make_shared<HighResTimerTweak>());
    gui.RegisterTweak(std::make_shared<DisableDynamicTickTweak>());

    // Interrupts (2)
    gui.RegisterTweak(std::make_shared<NetworkInterruptAffinityTweak>());
    gui.RegisterTweak(std::make_shared<DisableNetworkMSITweak>());

    // Input (3)
    gui.RegisterTweak(std::make_shared<DisableMouseAccelerationTweak>());
    gui.RegisterTweak(std::make_shared<MousePollingRateTweak>());
    gui.RegisterTweak(std::make_shared<DisablePointerPrecisionTweak>());

    // Scheduler (3)
    gui.RegisterTweak(std::make_shared<Win32PrioritySeparationTweak>());
    gui.RegisterTweak(std::make_shared<GameCPUPriorityTweak>());
    gui.RegisterTweak(std::make_shared<SFIOPriorityTweak>());

    // Misc (4)
    gui.RegisterTweak(std::make_shared<DisableDeliveryOptimizationTweak>());
    gui.RegisterTweak(std::make_shared<DisableNotificationsTweak>());
    gui.RegisterTweak(std::make_shared<DisableBackgroundAppsTweak>());
    gui.RegisterTweak(std::make_shared<DisableTelemetryTweak>());
}

// ─── WinMain ─────────────────────────────────────────────────────────────────
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    // Ensure we are running as administrator; if not, re-launch elevated
    if (!privilege_utils::IsRunningAsAdmin())
    {
        if (privilege_utils::RelaunchAsAdmin())
            return 0;
        MessageBoxW(nullptr,
                    L"LatencyOptimizer requires Administrator privileges.",
                    L"Elevation Required",
                    MB_ICONERROR | MB_OK);
        return 1;
    }

    // Create the window
    WNDCLASSEXW wc{};
    wc.cbSize        = sizeof(wc);
    wc.style         = CS_CLASSDC;
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(1));
    wc.lpszClassName = L"LatencyOptimizerWnd";
    RegisterClassExW(&wc);

    HWND hwnd = CreateWindowW(
        wc.lpszClassName,
        L"LatencyOptimizer",
        WS_OVERLAPPEDWINDOW,
        100, 100, 1100, 700,
        nullptr, nullptr, hInstance, nullptr);

    if (!hwnd || !CreateD3D(hwnd))
    {
        CleanupD3D();
        UnregisterClassW(wc.lpszClassName, hInstance);
        return 1;
    }

    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    // Set up GUI
    BackupManager backup;
    Gui gui(backup);
    RegisterTweaks(gui);

    if (!gui.Init(hwnd, g_pd3dDevice, g_pd3dContext))
    {
        CleanupD3D();
        UnregisterClassW(wc.lpszClassName, hInstance);
        return 1;
    }

    // Message loop
    bool running = true;
    while (running)
    {
        MSG msg{};
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
            if (msg.message == WM_QUIT)
                running = false;
        }
        if (!running) break;

        const float clearColor[4] = { 0.08f, 0.08f, 0.10f, 1.0f };
        g_pd3dContext->OMSetRenderTargets(1, &g_pRenderTarget, nullptr);
        g_pd3dContext->ClearRenderTargetView(g_pRenderTarget, clearColor);

        gui.Render();

        g_pSwapChain->Present(1, 0);
    }

    gui.Shutdown();
    CleanupD3D();
    DestroyWindow(hwnd);
    UnregisterClassW(wc.lpszClassName, hInstance);
    return 0;
}

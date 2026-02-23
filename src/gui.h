#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d11.h>
#include <string>
#include <vector>
#include <memory>

#include "tweaks/tweak_base.h"
#include "backup_manager.h"

// Forward-declare ImGui types to avoid pulling all headers here
struct ImVec4;

class Gui {
public:
    explicit Gui(BackupManager& backup);
    ~Gui();

    // Register a tweak to be shown in the UI
    void RegisterTweak(TweakPtr tweak);

    // Initialise ImGui with the given window and D3D11 device/context.
    // Returns false on failure.
    bool Init(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* context);

    // Render one frame of the UI.  Call once per message loop iteration.
    void Render();

    // Release all ImGui resources.
    void Shutdown();

    // Called from the WndProc; returns true if the message was consumed.
    static LRESULT HandleWindowMessage(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

private:
    void DrawMainWindow();
    void DrawCategoryList();
    void DrawTweakList();
    void DrawStatusBar();
    void DrawDetailPopup();
    void DrawConfirmPopup();
    void DrawAboutPopup();
    void DrawQuickActions();

    void ApplyTweak(TweakBase* tweak);
    void RevertTweak(TweakBase* tweak);
    void ApplyAllSafe();
    void RevertAll();
    void CreateRestorePoint();
    void ExportLog();

    void Log(const std::string& msg);

    // Tweak registry
    struct TweakEntry {
        TweakPtr tweak;
        bool     selected = false;
    };

    std::vector<TweakEntry> m_tweaks;
    BackupManager&          m_backup;

    // UI state
    int         m_selectedCategory = -1;  // -1 = "All"
    int         m_selectedTweak    = -1;
    bool        m_showDetail       = false;
    bool        m_showConfirm      = false;
    bool        m_showAbout        = false;
    bool        m_confirmRevert    = false; // true = revert, false = apply
    std::string m_statusMsg;
    std::vector<std::string> m_log;

    // Collected category names
    std::vector<std::string> m_categories;
    void RebuildCategories();
};

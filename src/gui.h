#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d11.h>
#include <string>
#include <vector>
#include <memory>

#include "tweaks/tweak_base.h"
#include "backup_manager.h"

struct ImVec4;

class Gui {
public:
    explicit Gui(BackupManager& backup);
    ~Gui();

    void RegisterTweak(TweakPtr tweak);
    bool Init(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* context);
    void Render();
    void Shutdown();

    static LRESULT HandleWindowMessage(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

private:
    void DrawMainWindow();
    void DrawHeader();
    void DrawCategoryList();
    void DrawTweakList();
    void DrawStatusBar();
    void DrawDetailPopup();
    void DrawConfirmPopup();
    void DrawAboutPopup();

    void ApplyTweak(TweakBase* tweak);
    void RevertTweak(TweakBase* tweak);
    void ApplyAllSafe();
    void RevertAll();
    void CreateRestorePoint();
    void ExportLog();

    void Log(const std::string& msg);

    struct TweakEntry {
        TweakPtr tweak;
        bool     selected = false;
    };

    std::vector<TweakEntry> m_tweaks;
    BackupManager&          m_backup;

    // UI state
    int         m_selectedCategory = -1;
    int         m_selectedTweak    = -1;
    bool        m_showDetail       = false;
    bool        m_showConfirm      = false;
    bool        m_showAbout        = false;
    bool        m_confirmRevert    = false;
    std::string m_statusMsg;
    std::vector<std::string> m_log;
    char        m_searchBuf[128]   = {};

    // Category info
    std::vector<std::string> m_categories;
    void RebuildCategories();

    // Helpers
    int CountAppliedInCategory(const std::string& cat) const;
    int CountTotalInCategory(const std::string& cat) const;
    bool MatchesSearch(const TweakBase* t) const;
};

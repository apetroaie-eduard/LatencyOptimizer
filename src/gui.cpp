#include "gui.h"

// ImGui headers (from third_party/imgui)
#include "../third_party/imgui/imgui.h"
#include "../third_party/imgui/backends/imgui_impl_win32.h"
#include "../third_party/imgui/backends/imgui_impl_dx11.h"

#include <algorithm>
#include <sstream>
#include <chrono>
#include <ctime>

// Declared in imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
    HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// ─── Construction / destruction ───────────────────────────────────────────────

Gui::Gui(BackupManager& backup) : m_backup(backup) {}

Gui::~Gui()
{
    Shutdown();
}

// ─── Public interface ─────────────────────────────────────────────────────────

void Gui::RegisterTweak(TweakPtr tweak)
{
    m_tweaks.push_back({std::move(tweak), false});
    RebuildCategories();
}

bool Gui::Init(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* context)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Dark theme
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding    = 6.0f;
    style.FrameRounding     = 4.0f;
    style.PopupRounding     = 4.0f;
    style.ScrollbarRounding = 4.0f;
    style.GrabRounding      = 4.0f;
    style.Colors[ImGuiCol_WindowBg]         = ImVec4(0.10f, 0.10f, 0.12f, 1.0f);
    style.Colors[ImGuiCol_Header]           = ImVec4(0.20f, 0.45f, 0.80f, 0.45f);
    style.Colors[ImGuiCol_HeaderHovered]    = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    style.Colors[ImGuiCol_HeaderActive]     = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_Button]           = ImVec4(0.20f, 0.45f, 0.80f, 0.60f);
    style.Colors[ImGuiCol_ButtonHovered]    = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    style.Colors[ImGuiCol_ButtonActive]     = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);

    if (!ImGui_ImplWin32_Init(hwnd))           return false;
    if (!ImGui_ImplDX11_Init(device, context)) return false;

    Log("Latency Optimizer started.");
    return true;
}

void Gui::Render()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    DrawMainWindow();

    if (m_showDetail)  DrawDetailPopup();
    if (m_showConfirm) DrawConfirmPopup();
    if (m_showAbout)   DrawAboutPopup();

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void Gui::Shutdown()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    if (ImGui::GetCurrentContext())
        ImGui::DestroyContext();
}

LRESULT Gui::HandleWindowMessage(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    return ImGui_ImplWin32_WndProcHandler(hwnd, msg, wp, lp);
}

// ─── Private helpers ──────────────────────────────────────────────────────────

void Gui::RebuildCategories()
{
    m_categories.clear();
    for (const auto& e : m_tweaks)
    {
        std::string cat = e.tweak->Category();
        if (std::find(m_categories.begin(), m_categories.end(), cat) == m_categories.end())
            m_categories.push_back(cat);
    }
}

void Gui::Log(const std::string& msg)
{
    // Prepend timestamp
    std::time_t t = std::time(nullptr);
    char ts[20]{};
    std::strftime(ts, sizeof(ts), "%H:%M:%S", std::localtime(&t));
    m_log.push_back(std::string(ts) + "  " + msg);
    m_statusMsg = msg;
    if (m_log.size() > 200)
        m_log.erase(m_log.begin());
}

// ─── Main window ─────────────────────────────────────────────────────────────

void Gui::DrawMainWindow()
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);

    ImGui::Begin("LatencyOptimizer##main",
                 nullptr,
                 ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_NoResize   |
                 ImGuiWindowFlags_NoMove     |
                 ImGuiWindowFlags_NoBringToFrontOnFocus);

    // Title bar substitute
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.8f, 1.0f, 1.0f));
    ImGui::Text("  LatencyOptimizer v1.0");
    ImGui::PopStyleColor();
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - 80.0f);
    if (ImGui::SmallButton("About"))
        m_showAbout = true;
    ImGui::Separator();

    // Quick actions row
    DrawQuickActions();
    ImGui::Separator();

    float leftWidth  = 160.0f;
    float statusH    = 40.0f;
    float mainH      = io.DisplaySize.y - ImGui::GetCursorPosY() - statusH - 20.0f;

    // Left pane: category list
    ImGui::BeginChild("Categories", ImVec2(leftWidth, mainH), true);
    DrawCategoryList();
    ImGui::EndChild();

    ImGui::SameLine();

    // Right pane: tweak list + log
    ImGui::BeginChild("Tweaks", ImVec2(0, mainH), true);
    DrawTweakList();
    ImGui::EndChild();

    DrawStatusBar();
    ImGui::End();
}

// ─── Quick actions ────────────────────────────────────────────────────────────

void Gui::DrawQuickActions()
{
    if (ImGui::Button("Apply All Safe"))  ApplyAllSafe();
    ImGui::SameLine();
    if (ImGui::Button("Revert All"))      RevertAll();
    ImGui::SameLine();
    if (ImGui::Button("Create Restore Point")) CreateRestorePoint();
    ImGui::SameLine();
    if (ImGui::Button("Export Log"))      ExportLog();
}

// ─── Category list ────────────────────────────────────────────────────────────

void Gui::DrawCategoryList()
{
    ImGui::Text("Categories");
    ImGui::Separator();

    bool allSel = (m_selectedCategory == -1);
    if (ImGui::Selectable("All", allSel))
        m_selectedCategory = -1;

    for (int i = 0; i < (int)m_categories.size(); ++i)
    {
        bool sel = (m_selectedCategory == i);
        if (ImGui::Selectable(m_categories[i].c_str(), sel))
            m_selectedCategory = i;
    }
}

// ─── Tweak list ───────────────────────────────────────────────────────────────

static ImVec4 RiskColor(TweakRisk r)
{
    switch (r)
    {
        case TweakRisk::Safe:   return ImVec4(0.2f, 0.8f, 0.2f, 1.0f);
        case TweakRisk::Medium: return ImVec4(1.0f, 0.8f, 0.0f, 1.0f);
        case TweakRisk::High:   return ImVec4(1.0f, 0.3f, 0.3f, 1.0f);
    }
    return ImVec4(1, 1, 1, 1);
}

static const char* RiskLabel(TweakRisk r)
{
    switch (r)
    {
        case TweakRisk::Safe:   return "[Safe]";
        case TweakRisk::Medium: return "[Medium]";
        case TweakRisk::High:   return "[High]";
    }
    return "";
}

static const char* CompatLabel(TweakCompat c)
{
    switch (c)
    {
        case TweakCompat::All:         return "";
        case TweakCompat::Win10Only:   return " Win10";
        case TweakCompat::Win11Only:   return " Win11";
        case TweakCompat::NvidiaOnly:  return " NVIDIA";
        case TweakCompat::AdvancedHW:  return " AdvHW";
    }
    return "";
}

void Gui::DrawTweakList()
{
    // Determine which tweaks to show
    std::vector<int> visible;
    for (int i = 0; i < (int)m_tweaks.size(); ++i)
    {
        if (m_selectedCategory == -1)
            visible.push_back(i);
        else if (m_tweaks[i].tweak->Category() == m_categories[m_selectedCategory])
            visible.push_back(i);
    }

    // Header row
    ImGui::Text("%-40s %-10s %-10s %-10s", "Tweak", "Status", "Risk", "Compat");
    ImGui::Separator();

    float listH = ImGui::GetContentRegionAvail().y - 130.0f;
    ImGui::BeginChild("TweakListScroll", ImVec2(0, listH), false);

    for (int idx : visible)
    {
        auto& entry  = m_tweaks[idx];
        TweakBase* t = entry.tweak.get();

        bool applied  = t->IsApplied();
        bool selected = (m_selectedTweak == idx);

        // Status indicator colour
        ImVec4 statusCol = applied
            ? ImVec4(0.2f, 0.9f, 0.2f, 1.0f)
            : ImVec4(0.6f, 0.6f, 0.6f, 1.0f);

        ImGui::PushID(idx);

        // Selectable row
        if (ImGui::Selectable("##row", selected,
                               ImGuiSelectableFlags_SpanAllColumns, ImVec2(0, 22)))
            m_selectedTweak = idx;

        ImGui::SameLine();
        ImGui::Text("%-38s", t->Name());
        ImGui::SameLine(280.0f);
        ImGui::PushStyleColor(ImGuiCol_Text, statusCol);
        ImGui::Text("%-10s", applied ? "Applied" : "Default");
        ImGui::PopStyleColor();
        ImGui::SameLine(380.0f);
        ImGui::PushStyleColor(ImGuiCol_Text, RiskColor(t->Risk()));
        ImGui::Text("%-10s", RiskLabel(t->Risk()));
        ImGui::PopStyleColor();
        ImGui::SameLine(460.0f);
        ImGui::TextDisabled("%s", CompatLabel(t->Compat()));

        ImGui::PopID();
    }

    ImGui::EndChild();

    ImGui::Separator();

    // Action buttons for selected tweak
    if (m_selectedTweak >= 0 && m_selectedTweak < (int)m_tweaks.size())
    {
        TweakBase* t = m_tweaks[m_selectedTweak].tweak.get();
        ImGui::Text("Selected: %s", t->Name());
        ImGui::SameLine();

        if (ImGui::Button("Apply"))
        {
            m_showConfirm    = true;
            m_confirmRevert  = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("Revert"))
        {
            m_showConfirm    = true;
            m_confirmRevert  = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Details"))
            m_showDetail = true;
    }

    // Log section
    ImGui::Separator();
    ImGui::Text("Log:");
    ImGui::BeginChild("LogScroll", ImVec2(0, 0), false,
                       ImGuiWindowFlags_HorizontalScrollbar);
    for (const auto& line : m_log)
        ImGui::TextUnformatted(line.c_str());
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);
    ImGui::EndChild();
}

// ─── Status bar ───────────────────────────────────────────────────────────────

void Gui::DrawStatusBar()
{
    ImGui::Separator();
    ImGui::TextDisabled("Status: ");
    ImGui::SameLine();
    ImGui::Text("%s", m_statusMsg.c_str());

    // System info on the right
    MEMORYSTATUSEX ms{};
    ms.dwLength = sizeof(ms);
    GlobalMemoryStatusEx(&ms);
    DWORDLONG totalMB = ms.ullTotalPhys / (1024 * 1024);
    DWORDLONG freeMB  = ms.ullAvailPhys / (1024 * 1024);

    char sysInfo[128]{};
    snprintf(sysInfo, sizeof(sysInfo),
             "RAM: %llu / %llu MB  |  Tweaks: %d applied",
             freeMB, totalMB,
             (int)std::count_if(m_tweaks.begin(), m_tweaks.end(),
                                [](const TweakEntry& e){ return e.tweak->IsApplied(); }));
    float tw = ImGui::CalcTextSize(sysInfo).x;
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - tw);
    ImGui::TextDisabled("%s", sysInfo);
}

// ─── Detail popup ─────────────────────────────────────────────────────────────

void Gui::DrawDetailPopup()
{
    ImGui::OpenPopup("Tweak Details");
    ImVec2 centre = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(centre, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(480, 260), ImGuiCond_Appearing);

    if (ImGui::BeginPopupModal("Tweak Details", &m_showDetail))
    {
        if (m_selectedTweak >= 0 && m_selectedTweak < (int)m_tweaks.size())
        {
            TweakBase* t = m_tweaks[m_selectedTweak].tweak.get();
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.8f, 1.0f, 1.0f));
            ImGui::Text("%s", t->Name());
            ImGui::PopStyleColor();
            ImGui::Separator();
            ImGui::TextWrapped("%s", t->Detail());
            ImGui::Spacing();
            ImGui::Text("Category : %s", t->Category());
            ImGui::Text("Risk     : %s", RiskLabel(t->Risk()));
            ImGui::Text("Compat   : %s%s", "Windows", CompatLabel(t->Compat()));
            ImGui::Text("Backup   : %s", t->RequiresBackup() ? "Yes" : "No");
            ImGui::Separator();
            ImGui::Text("Current  : %s", t->IsApplied() ? "Applied" : "Default");
        }
        ImGui::Spacing();
        if (ImGui::Button("Close", ImVec2(100, 0)))
            m_showDetail = false;
        ImGui::EndPopup();
    }
}

// ─── Confirm popup ────────────────────────────────────────────────────────────

void Gui::DrawConfirmPopup()
{
    ImGui::OpenPopup("Confirm Action");
    ImVec2 centre = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(centre, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(380, 140), ImGuiCond_Appearing);

    if (ImGui::BeginPopupModal("Confirm Action", &m_showConfirm))
    {
        if (m_selectedTweak >= 0 && m_selectedTweak < (int)m_tweaks.size())
        {
            TweakBase* t = m_tweaks[m_selectedTweak].tweak.get();
            const char* action = m_confirmRevert ? "Revert" : "Apply";
            ImGui::TextWrapped("%s tweak: '%s'?", action, t->Name());
            ImGui::Spacing();
            if (ImGui::Button("Confirm", ImVec2(100, 0)))
            {
                if (m_confirmRevert) RevertTweak(t);
                else                 ApplyTweak(t);
                m_showConfirm = false;
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(80, 0)))
                m_showConfirm = false;
        }
        ImGui::EndPopup();
    }
}

// ─── About popup ─────────────────────────────────────────────────────────────

void Gui::DrawAboutPopup()
{
    ImGui::OpenPopup("About##popup");
    ImVec2 centre = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(centre, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(420, 220), ImGuiCond_Appearing);

    if (ImGui::BeginPopupModal("About##popup", &m_showAbout))
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.8f, 1.0f, 1.0f));
        ImGui::Text("LatencyOptimizer  v1.0");
        ImGui::PopStyleColor();
        ImGui::Separator();
        ImGui::TextWrapped(
            "A Windows system-latency optimisation tool.\n"
            "Applies 31 tweaks across 11 categories to reduce\n"
            "input latency, network jitter, and frame-time variance.\n\n"
            "All changes are backed up and fully reversible.\n\n"
            "DISCLAIMER: Run as Administrator. Some tweaks require\n"
            "a system restart to take effect.");
        ImGui::Spacing();
        if (ImGui::Button("Close", ImVec2(100, 0)))
            m_showAbout = false;
        ImGui::EndPopup();
    }
}

// ─── Tweak actions ────────────────────────────────────────────────────────────

void Gui::ApplyTweak(TweakBase* tweak)
{
    if (tweak->RequiresBackup())
        m_backup.CreateRestorePoint(std::string("Before: ") + tweak->Name());

    bool ok = tweak->Apply();
    std::string msg = std::string(tweak->Name()) + (ok ? ": Applied OK" : ": Apply FAILED");
    Log(msg);
}

void Gui::RevertTweak(TweakBase* tweak)
{
    bool ok = tweak->Revert();
    std::string msg = std::string(tweak->Name()) + (ok ? ": Reverted OK" : ": Revert FAILED");
    Log(msg);
}

void Gui::ApplyAllSafe()
{
    int count = 0;
    m_backup.CreateRestorePoint("Before: Apply All Safe");
    for (auto& entry : m_tweaks)
    {
        if (entry.tweak->Risk() == TweakRisk::Safe && !entry.tweak->IsApplied())
        {
            if (entry.tweak->Apply())
                ++count;
        }
    }
    std::ostringstream oss;
    oss << "Apply All Safe: " << count << " tweak(s) applied.";
    Log(oss.str());
}

void Gui::RevertAll()
{
    int count = 0;
    for (auto& entry : m_tweaks)
    {
        if (entry.tweak->IsApplied())
        {
            if (entry.tweak->Revert())
                ++count;
        }
    }
    std::ostringstream oss;
    oss << "Revert All: " << count << " tweak(s) reverted.";
    Log(oss.str());
}

void Gui::CreateRestorePoint()
{
    m_backup.CreateRestorePoint("Manual restore point");
    Log("Restore point created.");
}

void Gui::ExportLog()
{
    wchar_t path[MAX_PATH]{};
    OPENFILENAMEW ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFilter  = L"Text Files\0*.txt\0All Files\0*.*\0";
    ofn.lpstrFile    = path;
    ofn.nMaxFile     = MAX_PATH;
    ofn.lpstrDefExt  = L"txt";
    ofn.Flags        = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;

    if (GetSaveFileNameW(&ofn))
    {
        bool ok = m_backup.ExportLog(path);
        Log(ok ? "Log exported." : "Log export failed.");
    }
}

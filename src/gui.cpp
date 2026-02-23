#include "gui.h"

#include "../third_party/imgui/imgui.h"
#include "../third_party/imgui/backends/imgui_impl_win32.h"
#include "../third_party/imgui/backends/imgui_impl_dx11.h"

#include <algorithm>
#include <sstream>
#include <chrono>
#include <ctime>
#include <cctype>

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

    // ── Modern dark theme ──────────────────────────────────────────────────
    ImGui::StyleColorsDark();
    ImGuiStyle& s = ImGui::GetStyle();

    // Geometry
    s.WindowRounding    = 0.0f;
    s.FrameRounding     = 3.0f;
    s.PopupRounding     = 4.0f;
    s.ScrollbarRounding = 6.0f;
    s.GrabRounding      = 3.0f;
    s.TabRounding       = 3.0f;
    s.ChildRounding     = 0.0f;
    s.WindowPadding     = ImVec2(12, 12);
    s.FramePadding      = ImVec2(8, 5);
    s.ItemSpacing       = ImVec2(8, 6);
    s.ItemInnerSpacing  = ImVec2(6, 4);
    s.ScrollbarSize     = 12.0f;
    s.WindowBorderSize  = 0.0f;
    s.ChildBorderSize   = 1.0f;
    s.PopupBorderSize   = 1.0f;
    s.FrameBorderSize   = 0.0f;

    // Color palette: dark charcoal with teal/cyan accents
    ImVec4 bg        = ImVec4(0.09f, 0.09f, 0.11f, 1.0f);
    ImVec4 bgChild   = ImVec4(0.11f, 0.11f, 0.14f, 1.0f);
    ImVec4 bgPopup   = ImVec4(0.12f, 0.12f, 0.15f, 1.0f);
    ImVec4 border    = ImVec4(0.20f, 0.20f, 0.24f, 1.0f);
    ImVec4 accent    = ImVec4(0.0f,  0.72f, 0.84f, 1.0f);   // teal
    ImVec4 accentDim = ImVec4(0.0f,  0.50f, 0.60f, 0.70f);
    ImVec4 accentLit = ImVec4(0.0f,  0.85f, 1.0f,  1.0f);
    ImVec4 textMain  = ImVec4(0.90f, 0.92f, 0.94f, 1.0f);
    ImVec4 textDim   = ImVec4(0.50f, 0.52f, 0.56f, 1.0f);
    ImVec4 hdrBg     = ImVec4(0.14f, 0.14f, 0.17f, 1.0f);

    s.Colors[ImGuiCol_WindowBg]             = bg;
    s.Colors[ImGuiCol_ChildBg]              = bgChild;
    s.Colors[ImGuiCol_PopupBg]              = bgPopup;
    s.Colors[ImGuiCol_Border]               = border;
    s.Colors[ImGuiCol_BorderShadow]         = ImVec4(0, 0, 0, 0);
    s.Colors[ImGuiCol_Text]                 = textMain;
    s.Colors[ImGuiCol_TextDisabled]         = textDim;
    s.Colors[ImGuiCol_FrameBg]              = ImVec4(0.14f, 0.14f, 0.17f, 1.0f);
    s.Colors[ImGuiCol_FrameBgHovered]       = ImVec4(0.18f, 0.18f, 0.22f, 1.0f);
    s.Colors[ImGuiCol_FrameBgActive]        = ImVec4(0.22f, 0.22f, 0.27f, 1.0f);
    s.Colors[ImGuiCol_TitleBg]              = hdrBg;
    s.Colors[ImGuiCol_TitleBgActive]        = hdrBg;
    s.Colors[ImGuiCol_TitleBgCollapsed]     = hdrBg;
    s.Colors[ImGuiCol_MenuBarBg]            = hdrBg;
    s.Colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.10f, 0.10f, 0.12f, 0.0f);
    s.Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.30f, 0.30f, 0.35f, 1.0f);
    s.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.46f, 1.0f);
    s.Colors[ImGuiCol_ScrollbarGrabActive]  = accent;
    s.Colors[ImGuiCol_Header]               = ImVec4(0.16f, 0.16f, 0.20f, 1.0f);
    s.Colors[ImGuiCol_HeaderHovered]        = ImVec4(0.0f,  0.50f, 0.60f, 0.30f);
    s.Colors[ImGuiCol_HeaderActive]         = ImVec4(0.0f,  0.50f, 0.60f, 0.50f);
    s.Colors[ImGuiCol_Separator]            = border;
    s.Colors[ImGuiCol_SeparatorHovered]     = accent;
    s.Colors[ImGuiCol_SeparatorActive]      = accentLit;
    s.Colors[ImGuiCol_Button]               = accentDim;
    s.Colors[ImGuiCol_ButtonHovered]        = accent;
    s.Colors[ImGuiCol_ButtonActive]         = accentLit;
    s.Colors[ImGuiCol_Tab]                  = ImVec4(0.14f, 0.14f, 0.17f, 1.0f);
    s.Colors[ImGuiCol_TabHovered]           = accent;
    s.Colors[ImGuiCol_TabSelected]          = accentDim;
    s.Colors[ImGuiCol_TableHeaderBg]        = hdrBg;
    s.Colors[ImGuiCol_TableBorderStrong]    = border;
    s.Colors[ImGuiCol_TableBorderLight]     = ImVec4(0.18f, 0.18f, 0.22f, 1.0f);
    s.Colors[ImGuiCol_TableRowBg]           = ImVec4(0, 0, 0, 0);
    s.Colors[ImGuiCol_TableRowBgAlt]        = ImVec4(1.0f, 1.0f, 1.0f, 0.02f);

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

// ─── Helpers ──────────────────────────────────────────────────────────────────

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

int Gui::CountAppliedInCategory(const std::string& cat) const
{
    int n = 0;
    for (const auto& e : m_tweaks)
        if (e.tweak->Category() == cat && e.tweak->IsApplied())
            ++n;
    return n;
}

int Gui::CountTotalInCategory(const std::string& cat) const
{
    int n = 0;
    for (const auto& e : m_tweaks)
        if (e.tweak->Category() == cat)
            ++n;
    return n;
}

bool Gui::MatchesSearch(const TweakBase* t) const
{
    if (m_searchBuf[0] == '\0') return true;
    std::string query(m_searchBuf);
    std::string name(t->Name());
    std::string desc(t->Description());
    // Case-insensitive search
    auto toLower = [](std::string s) {
        for (auto& c : s) c = (char)std::tolower(c);
        return s;
    };
    query = toLower(query);
    return toLower(name).find(query) != std::string::npos
        || toLower(desc).find(query) != std::string::npos;
}

void Gui::Log(const std::string& msg)
{
    std::time_t t = std::time(nullptr);
    char ts[20]{};
    std::strftime(ts, sizeof(ts), "%H:%M:%S", std::localtime(&t));
    m_log.push_back(std::string(ts) + "  " + msg);
    m_statusMsg = msg;
    if (m_log.size() > 200)
        m_log.erase(m_log.begin());
}

// ─── Risk / compat helpers ────────────────────────────────────────────────────

static ImVec4 RiskColor(TweakRisk r)
{
    switch (r) {
        case TweakRisk::Safe:   return ImVec4(0.30f, 0.85f, 0.45f, 1.0f);
        case TweakRisk::Medium: return ImVec4(1.0f,  0.78f, 0.15f, 1.0f);
        case TweakRisk::High:   return ImVec4(1.0f,  0.35f, 0.30f, 1.0f);
    }
    return ImVec4(1, 1, 1, 1);
}

static const char* RiskLabel(TweakRisk r)
{
    switch (r) {
        case TweakRisk::Safe:   return "Safe";
        case TweakRisk::Medium: return "Medium";
        case TweakRisk::High:   return "High";
    }
    return "";
}

static const char* CompatLabel(TweakCompat c)
{
    switch (c) {
        case TweakCompat::All:         return "All";
        case TweakCompat::Win10Only:   return "Win10";
        case TweakCompat::Win11Only:   return "Win11";
        case TweakCompat::NvidiaOnly:  return "NVIDIA";
        case TweakCompat::AdvancedHW:  return "AdvHW";
    }
    return "All";
}

// ─── Main window ──────────────────────────────────────────────────────────────

void Gui::DrawMainWindow()
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("LatencyOptimizer##main", nullptr,
                 ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_NoResize   |
                 ImGuiWindowFlags_NoMove     |
                 ImGuiWindowFlags_NoBringToFrontOnFocus |
                 ImGuiWindowFlags_NoScrollbar);
    ImGui::PopStyleVar();

    DrawHeader();

    float sidebarW = 180.0f;
    float statusH  = 34.0f;
    float contentY = ImGui::GetCursorPosY();
    float contentH = io.DisplaySize.y - contentY - statusH;

    // ── Sidebar ──────────────────────────────────────────────────────────
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.07f, 0.07f, 0.09f, 1.0f));
    ImGui::BeginChild("Sidebar", ImVec2(sidebarW, contentH), false);
    DrawCategoryList();
    ImGui::EndChild();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();

    ImGui::SameLine(0, 0);

    // ── Main content ─────────────────────────────────────────────────────
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 12));
    ImGui::BeginChild("Content", ImVec2(0, contentH), false);
    DrawTweakList();
    ImGui::EndChild();
    ImGui::PopStyleVar();

    DrawStatusBar();

    ImGui::End();
}

// ─── Header bar ───────────────────────────────────────────────────────────────

void Gui::DrawHeader()
{
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.06f, 0.06f, 0.08f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 10));
    ImGui::BeginChild("Header", ImVec2(0, 52), false);

    // Title
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.85f, 1.0f, 1.0f));
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
    ImGui::Text("LatencyOptimizer");
    ImGui::PopStyleColor();

    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.45f, 0.47f, 0.50f, 1.0f));
    ImGui::Text("v2.0");
    ImGui::PopStyleColor();

    // Action buttons - right aligned
    float rightX = ImGui::GetContentRegionAvail().x;
    float btnW   = 0.0f;

    // Calculate button widths for right alignment
    const char* btns[] = { "Apply All Safe", "Revert All", "Restore Point", "Export Log", "About" };
    float totalBtnW = 0;
    for (const char* b : btns)
        totalBtnW += ImGui::CalcTextSize(b).x + 20.0f;
    totalBtnW += 4 * 6.0f; // spacing between buttons

    ImGui::SameLine(ImGui::GetWindowWidth() - totalBtnW - 20.0f);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 6));

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.10f, 0.55f, 0.30f, 0.80f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.15f, 0.65f, 0.35f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.10f, 0.72f, 0.40f, 1.0f));
    if (ImGui::Button("Apply All Safe")) ApplyAllSafe();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.60f, 0.20f, 0.15f, 0.80f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.75f, 0.25f, 0.20f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.85f, 0.30f, 0.22f, 1.0f));
    if (ImGui::Button("Revert All")) RevertAll();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    if (ImGui::Button("Restore Point")) CreateRestorePoint();
    ImGui::SameLine();
    if (ImGui::Button("Export Log")) ExportLog();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.18f, 0.22f, 1.0f));
    if (ImGui::Button("About")) m_showAbout = true;
    ImGui::PopStyleColor();

    ImGui::PopStyleVar();

    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

// ─── Category sidebar ─────────────────────────────────────────────────────────

void Gui::DrawCategoryList()
{
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.45f, 0.47f, 0.50f, 1.0f));
    ImGui::Text("CATEGORIES");
    ImGui::PopStyleColor();
    ImGui::Spacing();

    // "All" category
    {
        int applied = (int)std::count_if(m_tweaks.begin(), m_tweaks.end(),
            [](const TweakEntry& e) { return e.tweak->IsApplied(); });
        int total = (int)m_tweaks.size();
        bool sel = (m_selectedCategory == -1);

        if (sel)
        {
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.0f, 0.50f, 0.60f, 0.25f));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.85f, 1.0f, 1.0f));
        }

        char label[64];
        snprintf(label, sizeof(label), "All  (%d/%d)", applied, total);
        if (ImGui::Selectable(label, sel, 0, ImVec2(0, 24)))
            m_selectedCategory = -1;

        if (sel)
            ImGui::PopStyleColor(2);
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    for (int i = 0; i < (int)m_categories.size(); ++i)
    {
        int applied = CountAppliedInCategory(m_categories[i]);
        int total   = CountTotalInCategory(m_categories[i]);
        bool sel = (m_selectedCategory == i);

        if (sel)
        {
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.0f, 0.50f, 0.60f, 0.25f));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.85f, 1.0f, 1.0f));
        }

        char label[64];
        snprintf(label, sizeof(label), "%s  (%d/%d)", m_categories[i].c_str(), applied, total);
        if (ImGui::Selectable(label, sel, 0, ImVec2(0, 24)))
            m_selectedCategory = i;

        if (sel)
            ImGui::PopStyleColor(2);
    }
}

// ─── Tweak list (main content) ────────────────────────────────────────────────

void Gui::DrawTweakList()
{
    // Search bar
    ImGui::PushItemWidth(-1);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 7));
    ImGui::InputTextWithHint("##search", "Search tweaks...", m_searchBuf, sizeof(m_searchBuf));
    ImGui::PopStyleVar();
    ImGui::PopItemWidth();
    ImGui::Spacing();

    // Collect visible tweaks
    std::vector<int> visible;
    for (int i = 0; i < (int)m_tweaks.size(); ++i)
    {
        if (m_selectedCategory != -1 &&
            m_tweaks[i].tweak->Category() != m_categories[m_selectedCategory])
            continue;
        if (!MatchesSearch(m_tweaks[i].tweak.get()))
            continue;
        visible.push_back(i);
    }

    // Tweak table
    float tableH = ImGui::GetContentRegionAvail().y - 160.0f;
    if (tableH < 100.0f) tableH = 100.0f;

    ImGuiTableFlags tflags =
        ImGuiTableFlags_RowBg |
        ImGuiTableFlags_BordersInnerH |
        ImGuiTableFlags_ScrollY |
        ImGuiTableFlags_Resizable |
        ImGuiTableFlags_Hideable |
        ImGuiTableFlags_SizingStretchProp;

    if (ImGui::BeginTable("TweakTable", 5, tflags, ImVec2(0, tableH)))
    {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Tweak",    ImGuiTableColumnFlags_NoHide, 4.0f);
        ImGui::TableSetupColumn("Status",   0, 1.0f);
        ImGui::TableSetupColumn("Risk",     0, 0.8f);
        ImGui::TableSetupColumn("Compat",   0, 0.8f);
        ImGui::TableSetupColumn("Actions",  ImGuiTableColumnFlags_NoResize, 1.6f);
        ImGui::TableHeadersRow();

        for (int idx : visible)
        {
            auto& entry  = m_tweaks[idx];
            TweakBase* t = entry.tweak.get();
            bool applied = t->IsApplied();
            bool selected = (m_selectedTweak == idx);

            ImGui::PushID(idx);
            ImGui::TableNextRow(0, 28.0f);

            // Tweak name column
            ImGui::TableNextColumn();
            if (ImGui::Selectable("##sel", selected,
                    ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap,
                    ImVec2(0, 24)))
                m_selectedTweak = idx;
            ImGui::SameLine();
            if (applied) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.92f, 0.95f, 1.0f));
                ImGui::Text("%s", t->Name());
                ImGui::PopStyleColor();
            } else {
                ImGui::Text("%s", t->Name());
            }

            // Status column
            ImGui::TableNextColumn();
            if (applied) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.30f, 0.85f, 0.45f, 1.0f));
                ImGui::Text("Active");
                ImGui::PopStyleColor();
            } else {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.45f, 0.47f, 0.50f, 1.0f));
                ImGui::Text("Off");
                ImGui::PopStyleColor();
            }

            // Risk column
            ImGui::TableNextColumn();
            ImGui::PushStyleColor(ImGuiCol_Text, RiskColor(t->Risk()));
            ImGui::Text("%s", RiskLabel(t->Risk()));
            ImGui::PopStyleColor();

            // Compat column
            ImGui::TableNextColumn();
            ImGui::TextDisabled("%s", CompatLabel(t->Compat()));

            // Actions column
            ImGui::TableNextColumn();
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 2));
            if (!applied) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.10f, 0.50f, 0.28f, 0.70f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.15f, 0.60f, 0.33f, 1.0f));
                if (ImGui::SmallButton("Apply")) {
                    m_selectedTweak = idx;
                    m_showConfirm   = true;
                    m_confirmRevert = false;
                }
                ImGui::PopStyleColor(2);
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.18f, 0.13f, 0.70f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.70f, 0.22f, 0.17f, 1.0f));
                if (ImGui::SmallButton("Revert")) {
                    m_selectedTweak = idx;
                    m_showConfirm   = true;
                    m_confirmRevert = true;
                }
                ImGui::PopStyleColor(2);
            }
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.18f, 0.22f, 0.70f));
            if (ImGui::SmallButton("Info")) {
                m_selectedTweak = idx;
                m_showDetail = true;
            }
            ImGui::PopStyleColor();
            ImGui::PopStyleVar();

            ImGui::PopID();
        }

        ImGui::EndTable();
    }

    // Tweak count summary
    ImGui::Spacing();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.45f, 0.47f, 0.50f, 1.0f));
    ImGui::Text("Showing %d tweaks", (int)visible.size());
    ImGui::PopStyleColor();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Log section
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.45f, 0.47f, 0.50f, 1.0f));
    ImGui::Text("LOG");
    ImGui::PopStyleColor();
    ImGui::Spacing();

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.07f, 0.07f, 0.09f, 1.0f));
    ImGui::BeginChild("LogScroll", ImVec2(0, 0), false,
                       ImGuiWindowFlags_HorizontalScrollbar);
    for (const auto& line : m_log)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.55f, 0.58f, 0.62f, 1.0f));
        ImGui::TextUnformatted(line.c_str());
        ImGui::PopStyleColor();
    }
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);
    ImGui::EndChild();
    ImGui::PopStyleColor();
}

// ─── Status bar ───────────────────────────────────────────────────────────────

void Gui::DrawStatusBar()
{
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.06f, 0.06f, 0.08f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 8));
    ImGui::BeginChild("StatusBar", ImVec2(0, 34), false);

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.50f, 0.52f, 0.56f, 1.0f));

    // Status message (left)
    if (!m_statusMsg.empty())
        ImGui::Text("%s", m_statusMsg.c_str());
    else
        ImGui::Text("Ready");

    // System info (right)
    MEMORYSTATUSEX ms{};
    ms.dwLength = sizeof(ms);
    GlobalMemoryStatusEx(&ms);
    DWORDLONG totalMB = ms.ullTotalPhys / (1024 * 1024);
    DWORDLONG usedMB  = totalMB - ms.ullAvailPhys / (1024 * 1024);

    int appliedCount = (int)std::count_if(m_tweaks.begin(), m_tweaks.end(),
        [](const TweakEntry& e) { return e.tweak->IsApplied(); });

    char info[128]{};
    snprintf(info, sizeof(info), "RAM: %llu / %llu MB   |   %d / %d tweaks active",
             usedMB, totalMB, appliedCount, (int)m_tweaks.size());
    float tw = ImGui::CalcTextSize(info).x;
    ImGui::SameLine(ImGui::GetWindowWidth() - tw - 20.0f);
    ImGui::Text("%s", info);

    ImGui::PopStyleColor();
    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

// ─── Detail popup ─────────────────────────────────────────────────────────────

void Gui::DrawDetailPopup()
{
    ImGui::OpenPopup("Tweak Details");
    ImVec2 centre = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(centre, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(520, 300), ImGuiCond_Appearing);

    if (ImGui::BeginPopupModal("Tweak Details", &m_showDetail,
                                ImGuiWindowFlags_NoResize))
    {
        if (m_selectedTweak >= 0 && m_selectedTweak < (int)m_tweaks.size())
        {
            TweakBase* t = m_tweaks[m_selectedTweak].tweak.get();
            bool applied = t->IsApplied();

            // Title
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.85f, 1.0f, 1.0f));
            ImGui::Text("%s", t->Name());
            ImGui::PopStyleColor();
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // Description
            ImGui::TextWrapped("%s", t->Detail());
            ImGui::Spacing();
            ImGui::Spacing();

            // Info grid
            ImGui::Columns(2, nullptr, false);
            ImGui::SetColumnWidth(0, 100);

            ImGui::TextDisabled("Category");  ImGui::NextColumn();
            ImGui::Text("%s", t->Category()); ImGui::NextColumn();

            ImGui::TextDisabled("Risk");      ImGui::NextColumn();
            ImGui::PushStyleColor(ImGuiCol_Text, RiskColor(t->Risk()));
            ImGui::Text("%s", RiskLabel(t->Risk()));
            ImGui::PopStyleColor();           ImGui::NextColumn();

            ImGui::TextDisabled("Compat");    ImGui::NextColumn();
            ImGui::Text("%s", CompatLabel(t->Compat())); ImGui::NextColumn();

            ImGui::TextDisabled("Status");    ImGui::NextColumn();
            if (applied) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.30f, 0.85f, 0.45f, 1.0f));
                ImGui::Text("Active");
                ImGui::PopStyleColor();
            } else {
                ImGui::TextDisabled("Off");
            }

            ImGui::Columns(1);
        }

        ImGui::Spacing();
        ImGui::Separator();
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
    ImGui::SetNextWindowSize(ImVec2(400, 150), ImGuiCond_Appearing);

    if (ImGui::BeginPopupModal("Confirm Action", &m_showConfirm,
                                ImGuiWindowFlags_NoResize))
    {
        if (m_selectedTweak >= 0 && m_selectedTweak < (int)m_tweaks.size())
        {
            TweakBase* t = m_tweaks[m_selectedTweak].tweak.get();
            const char* action = m_confirmRevert ? "Revert" : "Apply";

            ImGui::TextWrapped("%s tweak: '%s'?", action, t->Name());
            ImGui::Spacing();
            ImGui::Spacing();

            float buttonW = 100.0f;
            float totalW  = buttonW * 2 + 8.0f;
            ImGui::SetCursorPosX((ImGui::GetWindowWidth() - totalW) * 0.5f);

            if (m_confirmRevert) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.60f, 0.20f, 0.15f, 0.80f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.75f, 0.25f, 0.20f, 1.0f));
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.10f, 0.55f, 0.30f, 0.80f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.15f, 0.65f, 0.35f, 1.0f));
            }
            if (ImGui::Button("Confirm", ImVec2(buttonW, 0)))
            {
                if (m_confirmRevert) RevertTweak(t);
                else                 ApplyTweak(t);
                m_showConfirm = false;
            }
            ImGui::PopStyleColor(2);

            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.22f, 0.22f, 0.26f, 1.0f));
            if (ImGui::Button("Cancel", ImVec2(buttonW, 0)))
                m_showConfirm = false;
            ImGui::PopStyleColor();
        }
        ImGui::EndPopup();
    }
}

// ─── About popup ──────────────────────────────────────────────────────────────

void Gui::DrawAboutPopup()
{
    ImGui::OpenPopup("About##popup");
    ImVec2 centre = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(centre, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(440, 250), ImGuiCond_Appearing);

    if (ImGui::BeginPopupModal("About##popup", &m_showAbout,
                                ImGuiWindowFlags_NoResize))
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.85f, 1.0f, 1.0f));
        ImGui::Text("LatencyOptimizer  v2.0");
        ImGui::PopStyleColor();
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        char desc[256];
        snprintf(desc, sizeof(desc),
            "A Windows system-latency optimisation tool.\n"
            "%d tweaks across %d categories to reduce input\n"
            "latency, network jitter, and frame-time variance.\n\n"
            "All changes are backed up and fully reversible.\n\n"
            "Run as Administrator. Some tweaks require a\n"
            "system restart to take effect.",
            (int)m_tweaks.size(), (int)m_categories.size());
        ImGui::TextWrapped("%s", desc);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        float btnW = 100.0f;
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - btnW) * 0.5f);
        if (ImGui::Button("Close", ImVec2(btnW, 0)))
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

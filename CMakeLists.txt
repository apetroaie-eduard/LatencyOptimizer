cmake_minimum_required(VERSION 3.20)
project(LatencyOptimizer VERSION 1.0.0 LANGUAGES CXX)

# ── Compiler settings ────────────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Only target Windows
if(NOT WIN32)
    message(FATAL_ERROR "LatencyOptimizer targets Windows only.")
endif()

# ── ImGui ────────────────────────────────────────────────────────────────────
# Expects ImGui sources at third_party/imgui/ (clone Dear ImGui there).
# git clone https://github.com/ocornut/imgui.git third_party/imgui --depth=1

set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/third_party/imgui")

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    # Win32 + DX11 backends
    ${IMGUI_DIR}/backends/imgui_impl_win32.cpp
    ${IMGUI_DIR}/backends/imgui_impl_dx11.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

# ── Application sources ──────────────────────────────────────────────────────
set(APP_SOURCES
    src/main.cpp

    # Utilities
    src/utils/registry_utils.cpp
    src/utils/service_utils.cpp
    src/utils/cmd_utils.cpp
    src/utils/privilege_utils.cpp

    # Tweaks
    src/tweaks/services_tweaks.cpp
    src/tweaks/registry_tweaks.cpp
    src/tweaks/power_tweaks.cpp
    src/tweaks/network_tweaks.cpp
    src/tweaks/gpu_tweaks.cpp
    src/tweaks/memory_tweaks.cpp
    src/tweaks/timers_tweaks.cpp
    src/tweaks/interrupts_tweaks.cpp
    src/tweaks/input_tweaks.cpp
    src/tweaks/scheduler_tweaks.cpp
    src/tweaks/misc_tweaks.cpp
    src/tweaks/dpc_tweaks.cpp

    # Core
    src/backup_manager.cpp
    src/gui.cpp

    # Resources (icon + embedded manifest)
    resources/app.rc
)

# ── WIN32 executable (no console window) ─────────────────────────────────────
add_executable(LatencyOptimizer WIN32 ${APP_SOURCES})

target_include_directories(LatencyOptimizer PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

target_link_libraries(LatencyOptimizer PRIVATE
    imgui
    d3d11
    dxgi
    advapi32      # Registry, SCM
    shell32       # ShellExecuteEx
    comdlg32      # GetSaveFileNameW
    comctl32      # Common controls
)

# ── Compiler definitions ──────────────────────────────────────────────────────
target_compile_definitions(LatencyOptimizer PRIVATE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    UNICODE
    _UNICODE
    _WIN32_WINNT=0x0A00   # Windows 10+
)

# MSVC-specific flags
if(MSVC)
    target_compile_options(LatencyOptimizer PRIVATE /W3 /MP)
    # Suppress secure CRT warnings
    target_compile_definitions(LatencyOptimizer PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

# ── Post-build: copy resources ────────────────────────────────────────────────
add_custom_command(TARGET LatencyOptimizer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/resources/app.rc"
        "$<TARGET_FILE_DIR:LatencyOptimizer>/resources/app.rc"
    COMMENT "Copying resources"
)

# ── Install ───────────────────────────────────────────────────────────────────
install(TARGETS LatencyOptimizer DESTINATION bin)

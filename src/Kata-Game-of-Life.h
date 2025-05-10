#pragma once

import utilities;
import window_utilities;
import lib_cell;
import lib_table;
#include <windows.h>
#include <map>
#include <string>
#include <vector>

// Global Variables:
extern HINSTANCE g_hInst;                              // current instance
extern HWND g_hWnd;                                    // Handle of top-level window

using namespace std;
using namespace RYANS_UTILITIES;
using namespace RYANS_UTILITIES::WINDOWS_GUI;
using namespace cell;
using namespace table;

inline auto g_table = WINDOWS_TABLE{ };
inline auto allCells = vector<WINDOW>{ };
inline LIFE_HISTORY history{ };
inline auto generationIndex = size_t{ 0 };
inline auto pause = true;
inline constexpr auto timeIncrement = 10ull; // miliseconds
inline constexpr auto timerId = 1000;
inline constexpr auto WM_ADVANCE_GENERATION = 1001;
inline constexpr auto ID_PLAY_PAUSE = 1002ul;
inline constexpr auto ID_GENERATION_LABEL = 1003ul;
inline constexpr auto IDC_TOGGLE_EXTENDED = 1004ul;
inline auto playPauseDisplay = WINDOW{ }, generationNumDisplay = WINDOW{ }, buttonToggleExtendedBrushes = WINDOW{ };

inline auto simpleBrushes = map<LIFE_STATE, HBRUSH>{
    { LIFE_STATE::STABLE_DEAD , CreateSolidBrush(RGB(0, 0, 0))},
    { LIFE_STATE::RECENTLY_DEAD , CreateSolidBrush(RGB(0, 0, 0))},
    { LIFE_STATE::ALIVE , CreateSolidBrush(RGB(0, 255, 0))},
    { LIFE_STATE::DYING , CreateSolidBrush(RGB(0, 255, 0))},
    { LIFE_STATE::WILL_LIVE , CreateSolidBrush(RGB(0, 0, 0))},
    { LIFE_STATE::VASCILATING , CreateSolidBrush(RGB(0, 0, 0))},
    { LIFE_STATE::RECENTLY_GROWN , CreateSolidBrush(RGB(0, 255, 0))},
    { LIFE_STATE::STABLE_LIVING , CreateSolidBrush(RGB(0, 255, 0))},
};

inline auto useExtendedBrushes = true;

inline auto extendedBrushes = map<LIFE_STATE, HBRUSH>{
    { LIFE_STATE::STABLE_DEAD , CreateSolidBrush(RGB(0, 0, 0))},
    { LIFE_STATE::RECENTLY_DEAD , CreateSolidBrush(RGB(100, 0, 0))},
    { LIFE_STATE::ALIVE , CreateSolidBrush(RGB(0, 0, 255))},
    { LIFE_STATE::DYING , CreateSolidBrush(RGB(255, 0, 255))},
    { LIFE_STATE::WILL_LIVE , CreateSolidBrush(RGB(0, 100, 0))},
    { LIFE_STATE::VASCILATING , CreateSolidBrush(RGB(100, 0, 100))},
    { LIFE_STATE::RECENTLY_GROWN , CreateSolidBrush(RGB(0, 255, 255))},
    { LIFE_STATE::STABLE_LIVING , CreateSolidBrush(RGB(255, 255, 255))},
};

LRESULT CALLBACK WndProc(HWND hFrame, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK CellWindowProc(HWND hCell, UINT message, WPARAM wParam, LPARAM lParam);

void InitializeGame(HWND hFrame);
void TogglePause();
void StepGeneration(int step);
void ResetTimer(HWND hFrame);
void ToggleExtendedBrushes(HWND hFrame);
void CleanupGameResources(HWND hFrame);
void CheckTimer(HWND hFrame);
void ToggleCellState(HWND hCell);
void RedrawCell(HWND hCell);

#include "framework.h"
#include "Kata-Game-of-Life.h"
#include "__Table.h"
using namespace std;
using namespace RYANS_UTILITIES;
using namespace RYANS_UTILITIES::WINDOWS_GUI;
auto g_table = WINDOWS_TABLE{ };
auto allCells = vector<WINDOW>{ };
LIFE_HISTORY history{ };
auto pause = true;
constexpr auto timeIncrement = 10ull; // miliseconds
constexpr auto timerId = 1000;
constexpr auto WM_ADVANCE_GENERATION = 1001;
constexpr auto ID_PLAY_PAUSE = 1002ul;

auto playPauseDisplay = WINDOW{ };

auto simpleBrushes = map<LIFE_STATE, HBRUSH>{
    { LIFE_STATE::STABLE_DEAD , CreateSolidBrush(RGB(0, 0, 0))},
    { LIFE_STATE::RECENTLY_DEAD , CreateSolidBrush(RGB(0, 0, 0))},
    { LIFE_STATE::ALIVE , CreateSolidBrush(RGB(0, 255, 0))},
    { LIFE_STATE::DYING , CreateSolidBrush(RGB(0, 255, 0))},
    { LIFE_STATE::WILL_LIVE , CreateSolidBrush(RGB(0, 0, 0))},
    { LIFE_STATE::VASCILATING , CreateSolidBrush(RGB(0, 0, 0))},
    { LIFE_STATE::RECENTLY_GROWN , CreateSolidBrush(RGB(0, 255, 0))},
    { LIFE_STATE::STABLE_LIVING , CreateSolidBrush(RGB(0, 255, 0))},
};

#define EXTENDED_LIFESTATE_COLORING

#ifdef EXTENDED_LIFESTATE_COLORING
auto extendedBrushes = map<LIFE_STATE, HBRUSH>{
    { LIFE_STATE::STABLE_DEAD , CreateSolidBrush(RGB(0, 0, 0))},
    { LIFE_STATE::RECENTLY_DEAD , CreateSolidBrush(RGB(100, 0, 0))},
    { LIFE_STATE::ALIVE , CreateSolidBrush(RGB(0, 0, 255))},
    { LIFE_STATE::DYING , CreateSolidBrush(RGB(255, 0, 255))},
    { LIFE_STATE::WILL_LIVE , CreateSolidBrush(RGB(0, 100, 0))},
    { LIFE_STATE::VASCILATING , CreateSolidBrush(RGB(100, 0, 100))},
    { LIFE_STATE::RECENTLY_GROWN , CreateSolidBrush(RGB(0, 255, 255))},
    { LIFE_STATE::STABLE_LIVING , CreateSolidBrush(RGB(255, 255, 255))},
};
#else
auto extendedBrushes = simpleBrushes;
#endif


void TogglePause() {
    pause = !pause;
    playPauseDisplay.Text(pause ? "Pause" : "Play");
    if (pause) { WINDOW{ g_hWnd }.Redraw(); }
}

LRESULT CALLBACK WndProc(HWND hFrame, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CREATE: { 
            g_hWnd = hFrame;
            g_table.InitializeTable();
            for (auto position : allPositions) {
                allCells.push_back(WINDOW{ WINDOWS_TABLE::CELL_ID{ position } });
            }
            auto pos = WINDOW_POSITION{ 650 , 0 };
            auto size = WINDOW_DIMENSIONS{ 100 , 50 };
            playPauseDisplay = ConstructChildWindow("static", hFrame, ID_PLAY_PAUSE);
            playPauseDisplay.Move(pos, size).Text("Pause");
        } break;
        case WM_KEYDOWN: {
            switch (wParam) {
                case VK_RETURN: { 
                    TogglePause();
                    if (!pause) { SendMessage(hFrame, WM_COMMAND, MAKEWPARAM(WM_ADVANCE_GENERATION, NULL), NULL); } // Restart life process
                } break;
            }
        } break;
        case WM_COMMAND: {
                // Parse the menu selections:
                int wmId = LOWORD(wParam);
                switch (wmId) {
                    case WM_ADVANCE_GENERATION: {
                        history.Advance();
                        for (auto cell : allCells) { cell.Redraw(); }
                        if (!pause) { 
                            SetTimer(g_hWnd, timerId, timeIncrement, (TIMERPROC) NULL);
                        }
                    } break;
                    case IDM_ABOUT: { DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hFrame, About); }break;
                    case IDM_EXIT: { DestroyWindow(hFrame); } break;
                    default: { return DefWindowProc(hFrame, message, wParam, lParam); }
                }
            } break;
        case WM_PAINT: { 
            WINDOW{ hFrame }.BeginPaint();  // Validate window by explicitly calling BeginPaint() and implicitly calling EndPaint()
        } break;
        case WM_DESTROY: { 
            for (auto brushPair : extendedBrushes) { DeleteObject(brushPair.second); }
            for (auto brushPair : simpleBrushes) { DeleteObject(brushPair.second); }
            KillTimer(hFrame, timerId);
            PostQuitMessage(0);
        } break;
        default: { return DefWindowProc(hFrame, message, wParam, lParam); } // Pass along any unhandled messages
    }

    // Explicitly check for WM_TIMER messages since they are given very low priority and are often suppressed 
    auto msg = MSG{ };
    PeekMessage(&msg, hFrame, WM_TIMER, WM_TIMER, PM_REMOVE);
    if (msg.message == WM_TIMER) {
        if (!pause) { SendMessage(hFrame, WM_COMMAND, MAKEWPARAM(WM_ADVANCE_GENERATION, NULL), NULL); }     // Keep going until paused
        else { KillTimer(hFrame, timerId); }                                                                // Terminate timer if paused
    }

    return 0;   // Terminate any message that is explicitly handled
}


// This window proceedure is what gives cells much of their UI functionality.
// Features include using arrow keys to move between cells and responding to user focus.
// Without this, cells would only display text on character input.
LRESULT CALLBACK CellWindowProc(HWND hCell, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_LBUTTONDOWN: {
                auto& frame = history[history.Generation()];
                auto id = WINDOWS_TABLE::CELL_ID{ hCell };
                auto state = frame[id].State();
                frame[id].TogleDeadAlive();
                WINDOW{ id }.Redraw();
                for (auto neighbor : adjacencyList[id]) {
                    WINDOW{ WINDOWS_TABLE::CELL_ID{ neighbor } }.Redraw();  // Re-render neighbors 
                }
                if (!pause) { TogglePause(); }  // Pause if not already paused
                SetFocus(g_hWnd);
            } break;
            case WM_PAINT: {
                const auto& frame = history[history.Generation()];
                auto id = WINDOWS_TABLE::CELL_ID{ hCell };
                auto state = frame.LifeState(CELL_POSITION{ id });
                auto myCell = WINDOW{ id };
                auto rekt = myCell.GetClientRect();
                auto paintToken = myCell.BeginPaint();
                auto& brush = pause ? extendedBrushes[state] : simpleBrushes[state];    // Extra info on pause
                FillRect(paintToken, &rekt, brush);
            } break;
            default: { DefWindowProc(hCell, message, wParam, lParam); }
    }

    return DefWindowProc(hCell, message, wParam, lParam);   // Pass along all messages to default processing
}
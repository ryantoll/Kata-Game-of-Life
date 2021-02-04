#include "framework.h"
#include "Kata-Game-of-Life.h"
#include "__Table.h"
using namespace std;
using namespace RYANS_UTILITIES;
using namespace RYANS_UTILITIES::WINDOWS_GUI;
auto g_table = WINDOWS_TABLE{ };
auto allCells = vector<WINDOW>{ };
LIFE_HISTORY history{ };
auto generationIndex = size_t{ 0 };
auto pause = true;
constexpr auto timeIncrement = 10ull; // miliseconds
constexpr auto timerId = 1000;
constexpr auto WM_ADVANCE_GENERATION = 1001;
constexpr auto ID_PLAY_PAUSE = 1002ul;
constexpr auto ID_GENERATION_LABEL = 1003ul;
constexpr auto IDC_TOGGLE_EXTENDED = 1004ul;
auto playPauseDisplay = WINDOW{ }, generationNumDisplay = WINDOW{ }, buttonToggleExtendedBrushes = WINDOW{ };

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

auto useExtendedBrushes = true;

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

void TogglePause() {
    pause = !pause;
    playPauseDisplay.Text(pause ? "Pause" : "Play");
    if (pause) { WINDOW{ g_hWnd }.Redraw(); }
}

// Move display forward/backward through generations
// Advancing past the end calculates one new generation and displays that one
void StepGeneration(int step) {
    if (step == 0) { throw invalid_argument{"A step size of zero is trivial."}; }
    else if (step > 0 && generationIndex == history.Generation()) {
        history.Advance();
        ++generationIndex;
    }
    else if (step > 0 && generationIndex < history.Generation()) { generationIndex += step; }
    else if (abs(step) > generationIndex) { generationIndex = 0; }
    else { generationIndex += step; }

    auto msg = to_wstring(generationIndex);
    if (generationIndex < history.Generation()) { msg = L"**"s + msg + L"**"s; }
    generationNumDisplay.Wtext(msg);
    for (auto& cell : allCells) { cell.Redraw(); }
}

// Window proceedure for the top-level frame window
LRESULT CALLBACK WndProc(HWND hFrame, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CREATE: { 
            g_hWnd = hFrame;
            g_table.InitializeTable();
            for (auto& position : allPositions) {
                allCells.push_back(WINDOW{ WINDOWS_TABLE::CELL_ID{ position } });
            }
            auto pos = WINDOW_POSITION{ 650 , 0 };
            auto size = WINDOW_DIMENSIONS{ 100 , 25 };
            playPauseDisplay = ConstructChildWindow("static", hFrame, ID_PLAY_PAUSE, pos, size).Text("Pause");
            pos.y += size.height;
            generationNumDisplay = ConstructChildWindow("static", hFrame, ID_GENERATION_LABEL, pos, size).Text("0");
            pos.y += size.height;
            size.width = 200;
            buttonToggleExtendedBrushes = ConstructChildWindow("button", hFrame, IDC_TOGGLE_EXTENDED, pos, size).Text("Toggle extra colors");
        } break;
        case WM_KEYDOWN: {
            switch (wParam) {
                case VK_RETURN: { 
                    TogglePause();
                    if (!pause) { SendMessage(hFrame, WM_COMMAND, MAKEWPARAM(WM_ADVANCE_GENERATION, NULL), NULL); } // Restart life process
                } break;
                case VK_RIGHT: {
                    StepGeneration(1);
                } break;
                case VK_LEFT: {
                    StepGeneration(-1);
                } break;
            }
        } break;
        case WM_COMMAND: {
                // Parse the menu selections:
                int wmId = LOWORD(wParam);
                switch (wmId) {
                    case WM_ADVANCE_GENERATION: {
                        StepGeneration(1);
                        if (!pause) { 
                            SetTimer(g_hWnd, timerId, timeIncrement, (TIMERPROC) NULL); // Timer to advance again in play-mode
                        }
                    } break;
                    case IDC_TOGGLE_EXTENDED: { 
                        useExtendedBrushes = !useExtendedBrushes; 
                        WINDOW{ g_hWnd }.Redraw().Focus();
                    } break;
                    case IDM_ABOUT: { DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hFrame, About); } break;
                    case IDM_EXIT: { DestroyWindow(hFrame); } break;
                    default: { return DefWindowProc(hFrame, message, wParam, lParam); }
                }
            } break;
        case WM_PAINT: { 
            static_cast<void>(WINDOW{ hFrame }.BeginPaint());  // Validate window by explicitly calling BeginPaint() and implicitly calling EndPaint()
        } break;
        case WM_DESTROY: { 
            for (auto& brushPair : extendedBrushes) { DeleteObject(brushPair.second); }
            for (auto& brushPair : simpleBrushes) { DeleteObject(brushPair.second); }
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
            if (generationIndex < history.Generation()) { return 0; }   // Only allow editing of latest generation
                auto& frame = history[generationIndex];
                auto id = WINDOWS_TABLE::CELL_ID{ hCell };
                frame[id].TogleDeadAlive();
                WINDOW{ id }.Redraw();
                for (auto& neighbor : adjacencyList[id]) {
                    WINDOW{ WINDOWS_TABLE::CELL_ID{ neighbor } }.Redraw();  // Re-render neighbors 
                }
                if (!pause) { TogglePause(); }  // Pause if not already paused
                SetFocus(g_hWnd);
            } break;
            case WM_PAINT: {
                const auto& frame = history[generationIndex];
                auto id = WINDOWS_TABLE::CELL_ID{ hCell };
                auto state = frame.LifeState(CELL_POSITION{ id });
                auto myCell = WINDOW{ id };
                auto rekt = myCell.GetClientRect();
                auto paintToken = myCell.BeginPaint();
                auto& brush = pause && useExtendedBrushes ? extendedBrushes[state] : simpleBrushes[state];    // Extra info on pause
                FillRect(paintToken, &rekt, brush);
            } break;
            default: { DefWindowProc(hCell, message, wParam, lParam); }
    }

    return DefWindowProc(hCell, message, wParam, lParam);   // Pass along all messages to default processing
}
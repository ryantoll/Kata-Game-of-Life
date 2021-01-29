#include "framework.h"
#include "Kata-Game-of-Life.h"
#include "__Table.h"
using namespace std;
using namespace RYANS_UTILITIES;
using namespace RYANS_UTILITIES::WINDOWS_GUI;
auto g_table = WINDOWS_TABLE{ };
auto allCells = vector<WINDOW>{ };
LIFE_HISTORY history{ };

//#define EXTENDED_LIFESTATE_COLORING

#ifdef EXTENDED_LIFESTATE_COLORING
auto brushes = map<LIFE_STATE, HBRUSH>{
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
auto brushes = map<LIFE_STATE, HBRUSH>{
    { LIFE_STATE::STABLE_DEAD , CreateSolidBrush(RGB(0, 0, 0))},
    { LIFE_STATE::RECENTLY_DEAD , CreateSolidBrush(RGB(0, 0, 0))},
    { LIFE_STATE::ALIVE , CreateSolidBrush(RGB(0, 255, 0))},
    { LIFE_STATE::DYING , CreateSolidBrush(RGB(0, 255, 0))},
    { LIFE_STATE::WILL_LIVE , CreateSolidBrush(RGB(0, 0, 0))},
    { LIFE_STATE::VASCILATING , CreateSolidBrush(RGB(0, 0, 0))},
    { LIFE_STATE::RECENTLY_GROWN , CreateSolidBrush(RGB(0, 255, 0))},
    { LIFE_STATE::STABLE_LIVING , CreateSolidBrush(RGB(0, 255, 0))},
};
#endif // EXTENDED_LIFESTATE_COLORING




LRESULT CALLBACK WndProc(HWND hFrame, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE: { 
        g_hWnd = hFrame;
        g_table.InitializeTable();
        for (auto position : allPositions) {
            allCells.push_back(WINDOW{ WINDOWS_TABLE::CELL_ID{ position } });
        }
        SendMessage(hFrame, WM_PAINT, NULL, NULL);  // Ensure window draws on load
    } break;
    case WM_KEYDOWN: {
        switch (wParam) {
        case VK_RETURN: { 
            history.Advance();
            for (auto cell: allCells) { cell.Redraw();  }
        } break;
        }
    } break;
    case WM_COMMAND: {        
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId) {
            case IDM_ABOUT: { DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hFrame, About); }break;
            case IDM_EXIT: { DestroyWindow(hFrame); } break;
            default: { return DefWindowProc(hFrame, message, wParam, lParam); }
            }
        }
        break;
    case WM_PAINT: { } break;
    case WM_DESTROY: { 
        for (auto brushPair : brushes) { DeleteObject(brushPair.second); }
        PostQuitMessage(0); 
    } break;
    default: { return DefWindowProc(hFrame, message, wParam, lParam); }
    }
    return 0;
}


// This window proceedure is what gives cells much of their UI functionality.
// Features include using arrow keys to move between cells and responding to user focus.
// Without this, cells would only display text on character input.
LRESULT CALLBACK CellWindowProc(HWND hCell, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message)
    {
    case WM_LBUTTONDOWN: {
            auto& frame = history[history.Generation()];
            auto id = WINDOWS_TABLE::CELL_ID{ hCell };
            auto state = frame[id].State();
            frame[id].TogleDeadAlive();
            WINDOW{ id }.Redraw();
            for (auto neighbor : adjacencyList[id]) {
                WINDOW{ WINDOWS_TABLE::CELL_ID{ neighbor } }.Redraw();  // Re-render neighbors 
            }
            SetFocus(g_hWnd);
        } break;
        case WM_PAINT: {
            const auto& frame = history[history.Generation()];
            auto id = WINDOWS_TABLE::CELL_ID{ hCell };
            auto state = frame.LifeState(CELL_POSITION{ id });
            auto myCell = WINDOW{ id };
            auto rekt = myCell.GetClientRect();
            auto cellToken = myCell.BeginPaint();
            FillRect(cellToken, &rekt, brushes[state]);
        } break;
        default: { DefWindowProc(hCell, message, wParam, lParam); }
    }

    return DefWindowProc(hCell, message, wParam, lParam);;
}
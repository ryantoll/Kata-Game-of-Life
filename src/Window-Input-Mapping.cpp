#include "Kata-Game-of-Life.h"

// Window proceedure for the top-level frame window
// Dispatches window messages to appropriate functions
LRESULT CALLBACK WndProc(HWND hFrame, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CREATE: { InitializeGame(hFrame); } break;
        case WM_KEYDOWN: {
            switch (wParam) {
                case VK_RETURN: { TogglePause(); ResetTimer(hFrame); } break;
                case VK_RIGHT: { StepGeneration(1); } break;
                case VK_LEFT: { StepGeneration(-1); } break;
            }
        } break;
        case WM_COMMAND: {
            // Parse the menu selections:
            int wmId = LOWORD(wParam);
            switch (wmId) {
                case WM_ADVANCE_GENERATION: { StepGeneration(1); ResetTimer(hFrame); } break;
                case IDC_TOGGLE_EXTENDED: { ToggleExtendedBrushes(hFrame); } break;
                case IDM_ABOUT: { DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hFrame, About); } break;
                case IDM_EXIT: { DestroyWindow(hFrame); } break;
                default: { return DefWindowProc(hFrame, message, wParam, lParam); }
            }
        } break;
        case WM_PAINT: { static_cast<void>(WINDOW{ hFrame }.BeginPaint()); } break;     // Re-validates window
        case WM_DESTROY: { CleanupGameResources(hFrame); PostQuitMessage(0); } break;
        default: { return DefWindowProc(hFrame, message, wParam, lParam); }             // Pass along any unhandled messages
    }

    CheckTimer(hFrame);     // Explicitly check for WM_TIMER messages since they are given very low priority and are often suppressed 
    return 0;               // Terminate any message that is explicitly handled
}


// This window proceedure is what gives cells much of their UI functionality.
// Features include using arrow keys to move between cells and responding to user focus.
// Without this, cells would only display text on character input.
LRESULT CALLBACK CellWindowProc(HWND hCell, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_LBUTTONDOWN: { ToggleCellState(hCell); return 0; } break;
        case WM_PAINT: { RedrawCell(hCell); } break;
        default: { DefWindowProc(hCell, message, wParam, lParam); }
    }

    return DefWindowProc(hCell, message, wParam, lParam);   // Pass along all messages to default processing
}


// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);
    switch (message) {
        case WM_INITDIALOG: { return (INT_PTR)TRUE; }
	    case WM_COMMAND:
		    if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
			    EndDialog(hDlg, LOWORD(wParam));
			    return (INT_PTR)TRUE;
		    } break;
	}

	return (INT_PTR)FALSE;
}

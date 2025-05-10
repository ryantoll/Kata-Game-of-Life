#include "Kata-Game-of-Life.h"
#include <stdexcept>

void InitializeGame(HWND hFrame) {
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
}

void TogglePause() {
    pause = !pause;
    playPauseDisplay.Text(pause ? "Pause" : "Play");
    if (pause) { WINDOW{ g_hWnd }.Redraw(); }
}

// Move display forward/backward through generations
// Advancing past the end calculates one new generation and displays that one
void StepGeneration(int step) {
    if (step == 0) { throw std::invalid_argument{ "A step size of zero is trivial." }; }
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

// Timer to space out step forward messages, lest the queue saturate and crowd out other input
void ResetTimer(HWND hFrame) { SetTimer(hFrame, timerId, timeIncrement, (TIMERPROC)NULL); }

void ToggleExtendedBrushes(HWND hFrame) {
    useExtendedBrushes = !useExtendedBrushes;
    WINDOW{ hFrame }.Redraw().Focus();
}

void CleanupGameResources(HWND hFrame) {
    for (auto& brushPair : extendedBrushes) { DeleteObject(brushPair.second); }
    for (auto& brushPair : simpleBrushes) { DeleteObject(brushPair.second); }
    KillTimer(hFrame, timerId);
}

void CheckTimer(HWND hFrame) {
    auto msg = MSG{ };
    PeekMessage(&msg, hFrame, WM_TIMER, WM_TIMER, PM_REMOVE);
    if (msg.message == WM_TIMER) {
        if (!pause) { SendMessage(hFrame, WM_COMMAND, MAKEWPARAM(WM_ADVANCE_GENERATION, NULL), NULL); }     // Keep going until paused
        else { KillTimer(hFrame, timerId); }                                                                // Terminate timer if paused
    }
}

void ToggleCellState(HWND hCell) {
    if (generationIndex < history.Generation()) { return; }   // Only allow editing of latest generation
    auto& frame = history.GetLatestGeneration();
    auto id = WINDOWS_TABLE::CELL_ID{ hCell };
    frame[id].TogleDeadAlive();
    WINDOW{ id }.Redraw();
    for (auto& neighbor : adjacencyList[id]) {
        WINDOW{ WINDOWS_TABLE::CELL_ID{ neighbor } }.Redraw();  // Re-render neighbors 
    }
    if (!pause) { TogglePause(); }  // Pause if not already paused
    SetFocus(g_hWnd);
}

void RedrawCell(HWND hCell) {
    const auto& frame = history[generationIndex];
    auto id = WINDOWS_TABLE::CELL_ID{ hCell };
    auto state = frame.LifeState(CELL_POSITION{ id });
    auto myCell = WINDOW{ id };
    auto rekt = myCell.GetClientRect();
    auto paintToken = myCell.BeginPaint();
    auto& brush = pause && useExtendedBrushes ? extendedBrushes[state] : simpleBrushes[state];    // Extra info on pause
    FillRect(paintToken, &rekt, brush);
}
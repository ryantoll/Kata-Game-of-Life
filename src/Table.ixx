import lib_cell;
import window_utilities;

#include <windows.h>
#include <string>

// Global Variables:
extern HWND g_hWnd;                                    // Handle of top-level window

export module lib_table;

using namespace std;
using namespace RYANS_UTILITIES;
using namespace RYANS_UTILITIES::WINDOWS_GUI;
using namespace cell;

export namespace table {

// Table class specialized for a Windows OS GUI
class WINDOWS_TABLE {
public:
	class CELL_ID;
	class CELL_WINDOW;
protected:
	unsigned int m_NumColumns{ 0 };
	unsigned int m_NumRows{ 0 };
	int m_Width{ 25 };
	int m_Height{ 25 };
	int m_X0{ 0 };
	int m_Y0{ 25 };
public:
	void AddRow() noexcept;
	void AddColumn() noexcept;
	void RemoveRow() noexcept;
	void RemoveColumn() noexcept;
	void InitializeTable() noexcept;
	void Resize() noexcept;
	void Redraw() const noexcept;
};

// This is a utility class for converting between window IDs and row/column indicies.
// The purpose here is to be able to encode the cell position in the window ID, which is stored by the Windows OS.
// The ID will correspond to the row and column number with 16 bits representing each.
// This is scoped within the WINDOWS_TABLE class to indicate to clients that it's intended usage is only within the context of WINDOWS_TABLE usage.
// Define constructors as 'explicit' to avoid any implicit conversions since they take exactly one argument.
class WINDOWS_TABLE::CELL_ID {
	CELL_POSITION position;
	unsigned long windowID{ 0 };
	void Win_ID_From_Position() noexcept { windowID = (position.column << 16) + position.row; }
	void Position_From_Win_ID() noexcept {
		position.column = windowID >> 16;
		position.row = windowID & UINT16_MAX;
	}
public:
	CELL_ID() = default;
	explicit CELL_ID(const CELL_POSITION newPosition) : position(newPosition) { Win_ID_From_Position(); }
	explicit CELL_ID(const unsigned long newWindowID) : windowID(newWindowID) { Position_From_Win_ID(); }
	explicit CELL_ID(const HWND h) : windowID(GetDlgCtrlID(h)) { Position_From_Win_ID(); }

	void WindowID(const int newID) noexcept { windowID = newID; Position_From_Win_ID(); }
	auto& Row(const unsigned int newRowIndex) noexcept { position.row = newRowIndex; Win_ID_From_Position(); return *this; }
	auto& Column(const unsigned int newColumnIndex) noexcept { position.column = newColumnIndex; Win_ID_From_Position(); return *this; }
	auto& SetCellPosition(const CELL_POSITION newPosition) noexcept { position = newPosition; Win_ID_From_Position(); return *this; }

	auto WindowID() const noexcept { return windowID; }
	auto Row() const noexcept { return position.row; }
	auto Column() const noexcept { return position.column; }

	auto& IncrementRow() noexcept { position.row++; Win_ID_From_Position(); return *this; }
	auto& DecrementRow() noexcept { position.row--; Win_ID_From_Position(); return *this; }
	auto& IncrementColumn() noexcept { position.column++; Win_ID_From_Position(); return *this; }
	auto& DecrementColumn() noexcept { position.column--; Win_ID_From_Position(); return *this; }

	operator CELL_POSITION() const noexcept { return position; }			// Allow for implicit conversion to CELL_POSITION
	operator HWND() const noexcept { return GetDlgItem(g_hWnd, windowID); }		// Allow for implicit conversion to HWND
	operator HMENU() const noexcept { return reinterpret_cast<HMENU>(static_cast<UINT_PTR>(windowID)); }	// Allow for implicit conversion to HMENU
};

// Initial window setup
void WINDOWS_TABLE::InitializeTable() noexcept {
	for (auto i = 0; i < layoutWidth; ++i) { AddColumn(); }
	for (auto i = 0; i < layoutHeight; ++i) { AddRow(); }
	Redraw();
}

void WINDOWS_TABLE::Redraw() const noexcept { }		// Hook for table redraw logic

// Create a new row of cells at bottom edge.
void WINDOWS_TABLE::AddRow() noexcept {
	auto cell_ID = CELL_ID{ }.Row(m_NumRows).Column(0);
	auto window = WINDOW{ };
	auto pos = WINDOW_POSITION{ };
	auto size = WINDOW_DIMENSIONS{ };
	size.width = m_Width; size.height = m_Height;

	while (cell_ID.Column() < m_NumColumns) {
		pos.x = cell_ID.Column() * m_Width;
		pos.y = m_NumRows * m_Height;
		window = ConstructChildWindow("Cell"s, g_hWnd, cell_ID, pos, size);
		window.Style(window.Style() | SS_NOTIFY);
		cell_ID.IncrementColumn();
	}
	++m_NumRows;
}

// Create a new column of cells at right edge.
void WINDOWS_TABLE::AddColumn() noexcept {
	auto cell_ID = CELL_ID{ }.Row(0).Column(++m_NumColumns);
	auto window = WINDOW{ };
	auto pos = WINDOW_POSITION{ };
	auto size = WINDOW_DIMENSIONS{ };
	size.width = m_Width; size.height = m_Height;

	while (cell_ID.Row() < m_NumRows) {
		pos.x = cell_ID.Column() * m_Width;
		pos.y = cell_ID.Row() * m_Height;
		window = ConstructChildWindow("Cell"s, g_hWnd, cell_ID, pos, size);
		window.Style(window.Style() | SS_NOTIFY);
		cell_ID.IncrementRow();
	}
}

// Remove bottom row of cells.
void WINDOWS_TABLE::RemoveRow() noexcept {
	auto id = CELL_ID{ };
	auto h = HWND{ id.Row(m_NumRows) };
	while (id.Column() <= m_NumColumns) { DestroyWindow(h); h = id.IncrementColumn(); }	// Increment through row and destroy cells
	m_NumRows--;
}

// Remove right column of cells.
void WINDOWS_TABLE::RemoveColumn() noexcept {
	auto id = CELL_ID{ };
	auto h = HWND{ id.Row(m_NumRows) };
	while (id.Row() <= m_NumRows) { DestroyWindow(h); h = id.IncrementRow(); }	// Increment through column and destroy cells
	m_NumColumns--;
}

// Resize table, creating/destroying cell windows as needed.
void WINDOWS_TABLE::Resize() noexcept { }

} // namespace table
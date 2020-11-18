#ifndef TABLE_CLASS_H
#define TABLE_CLASS_H
#include "framework.h"
#include "__Cell.h"
#include "__WINDOW.h"

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
	CELL::CELL_POSITION position;
	unsigned long windowID{ 0 };
	void Win_ID_From_Position() noexcept { windowID = (position.column << 16) + position.row; }
	void Position_From_Win_ID() noexcept {
		position.column = windowID >> 16;
		position.row = windowID & UINT16_MAX;
	}
public:
	CELL_ID() = default;
	explicit CELL_ID(const CELL::CELL_POSITION newPosition): position(newPosition) { Win_ID_From_Position(); }
	explicit CELL_ID(const unsigned long newWindowID): windowID(newWindowID) { Position_From_Win_ID(); }
	explicit CELL_ID(const HWND h) : windowID(GetDlgCtrlID(h)) { Position_From_Win_ID(); }

	void WindowID(const int newID) noexcept { windowID = newID; Position_From_Win_ID(); }
	auto& Row(const unsigned int newRowIndex) noexcept { position.row = newRowIndex; Win_ID_From_Position(); return *this; }
	auto& Column(const unsigned int newColumnIndex) noexcept { position.column = newColumnIndex; Win_ID_From_Position(); return *this; }
	auto& SetCellPosition(const CELL::CELL_POSITION newPosition) noexcept { position = newPosition; Win_ID_From_Position(); return *this; }

	auto WindowID() const noexcept { return windowID; }
	auto Row() const noexcept { return position.row; }
	auto Column() const noexcept { return position.column; }

	auto& IncrementRow() noexcept { position.row++; Win_ID_From_Position(); return *this; }
	auto& DecrementRow() noexcept { position.row--; Win_ID_From_Position(); return *this; }
	auto& IncrementColumn() noexcept { position.column++; Win_ID_From_Position(); return *this; }
	auto& DecrementColumn() noexcept { position.column--; Win_ID_From_Position(); return *this; }

	operator CELL::CELL_POSITION() const noexcept { return position; }			// Allow for implicit conversion to CELL_POSITION
	operator HWND() const noexcept { return GetDlgItem(g_hWnd, windowID); }		// Allow for implicit conversion to HWND
	operator HMENU() const noexcept { return reinterpret_cast<HMENU>(static_cast<UINT_PTR>(windowID)); }	// Allow for implicit conversion to HMENU
};

#endif //!TABLE_CLASS_H
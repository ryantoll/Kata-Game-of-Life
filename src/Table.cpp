#include "framework.h"
#include "__Table.h"
#include "__WINDOW.h"

using namespace std;
using namespace RYANS_UTILITIES;
using namespace RYANS_UTILITIES::WINDOWS_GUI;


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

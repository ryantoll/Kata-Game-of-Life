#include "framework.h"
#include "__CELL.h"
using namespace std;

[[nodiscard]] vector<CELL::CELL_POSITION> GetSurroundingPositions(const CELL::CELL_POSITION position) noexcept {
	auto adjacencyList = vector<CELL::CELL_POSITION>{ };
	for (auto i = -1; i <= 1; ++i) {
		for (auto j = -1; j <= 1; ++j) {
			if (i == 0 && j == 0) { continue; }		// Skip present position
			auto row = position.row + i;
			if (row < 0 || row > layoutWidth) { continue; }	// Skip out-of-bounds elements
			auto col = position.column + j;
			if (col < 0 || col > layoutHeight) { continue; }	// Skip out-of-bounds elements
			adjacencyList.push_back(CELL::CELL_POSITION{ position.row + i, position.column + j });
		}
	}
}

[[nodiscard]] CELL TIME_SLICE::Cell(CELL::CELL_POSITION position) const noexcept {
	return m_Layout[position.row][position.column];
}

const TIME_SLICE& LIFE_HISTORY::Advance() noexcept {
	CalculateNextGeneration();
	return m_History.back();
}

const TIME_SLICE& LIFE_HISTORY::Generation(unsigned long generationNumber) const noexcept {
	return m_History[generationNumber];
}

[[nodiscard]] unsigned long LIFE_HISTORY::Generation() const noexcept {
	return m_Generation;
}

unsigned int SurroundingCellNumber(const CELL::CELL_POSITION position, TIME_SLICE& timeSlice) noexcept {
	auto adjacencyList = GetSurroundingPositions(position);
	auto count = unsigned int{ 0 };
	for (auto pos: adjacencyList) {
		if (timeSlice.Cell(pos).m_State == CELL::LIFE_STATE::ALIVE) {
			++count;
		}
	}
	return count;
}

CELL::LIFE_STATE FindNextCellState(const CELL::CELL_POSITION position, TIME_SLICE& timeSlice) noexcept {
	auto count = SurroundingCellNumber(position, timeSlice);
	auto state = CELL::LIFE_STATE{ };
	if (count > 2 && count < 4) { state = CELL::LIFE_STATE::ALIVE; }
	else { state = CELL::LIFE_STATE::DEAD; }
	return state;
}

vector<CELL::CELL_POSITION> consteval AllCellPositions() {
	auto positionList = vector<CELL::CELL_POSITION>{ };
	for (auto i = 0; i < layoutWidth; ++i) {
		for (auto j = 0; j < layoutHeight; ++j) {
			positionList.emplace_back(i, j);
		}
	}
	return positionList;
}

void LIFE_HISTORY::CalculateNextGeneration() noexcept {
	auto nextGeneration = TIME_SLICE{ };
	const auto& timeSlice = m_History.back();
	auto position = CELL::CELL_POSITION{ };
	timeSlice.Cell(position);
}
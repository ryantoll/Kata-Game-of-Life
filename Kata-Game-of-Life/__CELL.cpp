#include "framework.h"
#include "__CELL.h"
using namespace std;

//ALL_CELL_POSITIONS allPositions;
//ADJACENCY_LIST adjacencyList;

ALL_CELL_POSITIONS::ALL_CELL_POSITIONS() {
	for (auto i = 0; i < layoutWidth; ++i) {
		for (auto j = 0; j < layoutHeight; ++j) {
			allPositions.emplace_back(i, j);
			//auto position = CELL_POSITION{ };
			//position.row = position.row + i;
			//position.column = position.column + j;
			//allPositions.push_back(position);
		}
	}
}

ADJACENCY_LIST::ADJACENCY_LIST() {
	for (auto position: allPositions) {
		auto iMin = position.row == 0 ? 0 : -1;
		auto iMax = position.row == layoutWidth - 1 ? 0 : 1;

		for (auto i = iMin; i <= iMax; ++i) {
			auto jMin = position.column == 0 ? 0 : -1;
			auto jMax = position.column == layoutHeight - 1 ? 0 : 1;

			for (auto j = jMin; j <= jMax; ++j) {
				if (i == 0 && j == 0) { continue; }
				auto row = position.row + i;
				auto col = position.column + j;
				adjacencyList[position.row][position.column].emplace_back(position.row + i, position.column + j);
			}
		}
	}
}

const std::vector<CELL_POSITION>& ADJACENCY_LIST::operator[] (CELL_POSITION position) const noexcept {
	return adjacencyList[position.row][position.column];
}

TIME_SLICE::PROXY_CELL& TIME_SLICE::PROXY_CELL::State(LIFE_STATE state) noexcept {
	auto increment = 0;
	if (state == m_Cell->state) { return *this; }
	else if (state == LIFE_STATE::ALIVE) { increment = 1; }
	else { increment = -1; }

	if (m_Frame->status == STATUS::GENERATED) { m_Frame->status = STATUS::MANUALLY_CHANGED; }
	m_Cell->state = state;
	for (auto position: adjacencyList[m_Position]) {
		(*m_Frame).m_Layout[position.row][position.column].neighborCount += increment;
	}
	return *this;
}

[[nodiscard]] LIFE_STATE TIME_SLICE::LifeState(CELL_POSITION position) const noexcept {
	return m_Layout[position.row][position.column].state;
}

[[nodiscard]] unsigned int TIME_SLICE::NeighborCount(CELL_POSITION position) const noexcept {
	return m_Layout[position.row][position.column].neighborCount;
}

[[nodiscard]] TIME_SLICE::PROXY_CELL TIME_SLICE::operator[] (CELL_POSITION position) noexcept {
	return PROXY_CELL{ &m_Layout[position.row][position.column], position, this };
}

// The cast-away of const **should** be safe
// The result is const, so that should preserve the const-ness
// Otherwise this doesn't work and this should be symetric with the non-const version
[[nodiscard]] const TIME_SLICE::PROXY_CELL TIME_SLICE::operator[] (CELL_POSITION position) const noexcept {
	return PROXY_CELL{ const_cast<CELL*>(&m_Layout[position.row][position.column]), position, const_cast<TIME_SLICE*>(this) };
}

const TIME_SLICE& LIFE_HISTORY::Advance() noexcept {
	CalculateNextGeneration();
	return m_History.back();
}

TIME_SLICE& LIFE_HISTORY::operator[] (unsigned long generationNumber) noexcept {
	return m_History[generationNumber];
}

[[nodiscard]] unsigned long LIFE_HISTORY::Generation() const noexcept {
	//return m_Generation;
	return m_History.size() - 1;
}

unsigned int SurroundingCellNumber(const CELL_POSITION position, const TIME_SLICE& timeSlice) noexcept {
	auto count = unsigned int{ 0 };
	for (auto pos: adjacencyList[position]) {
		if (timeSlice.LifeState(pos) == LIFE_STATE::ALIVE) {
			++count;
		}
	}
	return count;
}

void SetNextCellState(CELL_POSITION position, const TIME_SLICE& previousGeneration, TIME_SLICE& nextGeneration) noexcept {
	for (auto position : allPositions) {
		auto neighborCount = previousGeneration.NeighborCount(position);
		if (neighborCount < 2 || neighborCount > 3) {
			nextGeneration[position].State(LIFE_STATE::DEAD);
		}
		else if (neighborCount == 3) {
			nextGeneration[position].State(LIFE_STATE::ALIVE);
		}
		else if (neighborCount == 2 && previousGeneration.LifeState(position) == LIFE_STATE::ALIVE) {
			nextGeneration[position].State(LIFE_STATE::ALIVE);
		}
		else {
			nextGeneration[position].State(LIFE_STATE::DEAD);
		}
	}
}

void LIFE_HISTORY::CalculateNextGeneration() noexcept {
	auto nextGeneration = TIME_SLICE{ };
	nextGeneration.status = TIME_SLICE::STATUS::GENERATED;
	const auto& previousGeneration = m_History.back();
	
	// Calculate all living cells
	auto calc = [&](CELL_POSITION position) { SetNextCellState(position, previousGeneration, nextGeneration); };
	std::for_each(std::execution::par_unseq, allPositions.begin(), allPositions.end(), calc);
	/*for (auto position: allPositions) {
		auto neighborCount = timeSlice.NeighborCount(position);
		if (neighborCount < 2 || neighborCount > 3) {
			nextGeneration[position].State(LIFE_STATE::DEAD);
		}
		else if(neighborCount == 3) {
			nextGeneration[position].State(LIFE_STATE::ALIVE);
		}
		else if (neighborCount == 2 && timeSlice.LifeState(position) == LIFE_STATE::ALIVE) {
			nextGeneration[position].State(LIFE_STATE::ALIVE);
		}
		else {
			nextGeneration[position].State(LIFE_STATE::DEAD);
		}
	}*/

	// Then, calculate next generation living neighbor count
	/*for (auto position : allPositions) {
		auto count = 0;
		for (auto neighbor : adjacencyList[position]) {
			if (timeSlice.LifeState(neighbor) == LIFE_STATE::ALIVE ) {
				count++;
			}
		}
		nextGeneration[position].neighborCount = count;
	}*/

	m_History.push_back(std::move(nextGeneration));
	++m_Generation;
}
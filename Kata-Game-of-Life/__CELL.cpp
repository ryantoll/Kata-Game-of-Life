#include "framework.h"
#include "__CELL.h"
#include "__Utilities.h"
using namespace std;
using namespace RYANS_UTILITIES;

ALL_CELL_POSITIONS::ALL_CELL_POSITIONS() {
	for (auto i = 0; i < layoutWidth; ++i) {
		for (auto j = 0; j < layoutHeight; ++j) {
			positions.emplace_back(i, j);
		}
	}
}

ADJACENCY_LIST::ADJACENCY_LIST() {
	for (auto& position : allPositions) {
		auto iMin = position.row == 0 ? 0 : -1;
		auto iMax = position.row == layoutWidth - 1 ? 0 : 1;

		for (auto i = iMin; i <= iMax; ++i) {
			auto jMin = position.column == 0 ? 0 : -1;
			auto jMax = position.column == layoutHeight - 1 ? 0 : 1;

			for (auto j = jMin; j <= jMax; ++j) {
				if (i == 0 && j == 0) { continue; }
				adjacencyList[position.row][position.column].emplace_back(position.row + i, position.column + j);
			}
		}
	}
}

const std::vector<CELL_POSITION>& ADJACENCY_LIST::operator[] (CELL_POSITION position) const noexcept {
	return adjacencyList[position.row][position.column];
}

LIFE_STATE TIME_SLICE::PROXY_CELL::TogleDeadAlive() noexcept { 
	auto flippedState = EnumToggleFlag(m_Cell->state, LIFE_STATE::ALIVE);
	State(flippedState);
	return flippedState;
}

TIME_SLICE::PROXY_CELL& TIME_SLICE::PROXY_CELL::State(LIFE_STATE state) noexcept {
	auto increment = 0;
	auto isAlive = EnumHasFlag<LIFE_STATE>(state, LIFE_STATE::ALIVE);
	if (state == m_Cell->state) { return *this; }
	else if (isAlive) { increment = 1; }
	else { increment = -1; }

	// Update predicted next generation expected life state
	if (m_Cell->neighborCount == 3 || m_Cell->neighborCount == 2 && isAlive) {
		m_Cell->state = EnumAddFlag(state, LIFE_STATE::WILL_LIVE);
	}
	else { m_Cell->state = state; }

	if (m_Frame->status == STATUS::GENERATED) { m_Frame->status = STATUS::MANUALLY_CHANGED; }

	for (auto& position: adjacencyList[m_Position]) {
		auto& neighborCell = (*m_Frame).m_Layout[position.row][position.column];
		neighborCell.neighborCount += increment;

		// Update predicted next generation expected life state
		if (neighborCell.neighborCount == 3 || 
			neighborCell.neighborCount == 2 && EnumHasFlag<LIFE_STATE>(neighborCell.state, LIFE_STATE::ALIVE)) {
			neighborCell.state = EnumAddFlag<LIFE_STATE>(neighborCell.state, LIFE_STATE::WILL_LIVE);
		}
		else { neighborCell.state = EnumRemoveFlag(neighborCell.state, LIFE_STATE::WILL_LIVE); }
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

TIME_SLICE& LIFE_HISTORY::operator[] (size_t generationNumber) noexcept {
	return m_History[generationNumber];
}

[[nodiscard]] size_t LIFE_HISTORY::Generation() const noexcept {
	return m_History.size() - 1;
}

unsigned int SurroundingCellNumber(const CELL_POSITION position, const TIME_SLICE& timeSlice) noexcept {
	auto count = unsigned int{ 0 };
	for (auto& pos: adjacencyList[position]) {
		if (EnumHasFlag<LIFE_STATE>(timeSlice.LifeState(pos), LIFE_STATE::ALIVE)) {
			++count;
		}
	}
	return count;
}

void TIME_SLICE::SetNextCellState(const CELL_POSITION position, const TIME_SLICE& previousGeneration, TIME_SLICE& nextGeneration) noexcept {
	// Next Life State
	auto& nextState = nextGeneration[position].m_Cell->state;
	auto& priorState = previousGeneration[position].m_Cell->state;
	nextState = priorState >> 1u;

	// Next Neighbor Count
	auto& count = nextGeneration[position].m_Cell->neighborCount;
	count = 0;
	for (auto& pos : adjacencyList[position]) {
		if (EnumHasFlag<LIFE_STATE>(previousGeneration.LifeState(pos), LIFE_STATE::WILL_LIVE)) {
			++count;
		}
	}
}

void TIME_SLICE::CalculateNeighborCount(const CELL_POSITION position, TIME_SLICE& nextGeneration) noexcept {
	nextGeneration[position].m_Cell->neighborCount = SurroundingCellNumber(position, nextGeneration);
}

void TIME_SLICE::CalculateNextLifeState(const CELL_POSITION position, TIME_SLICE& nextGeneration) noexcept {
	auto neighborCount = nextGeneration.NeighborCount(position);
	auto& state = nextGeneration[position].m_Cell->state;
	if (neighborCount == 3 || neighborCount == 2 && EnumHasFlag<LIFE_STATE>(state, LIFE_STATE::ALIVE)) {
		state = EnumAddFlag<LIFE_STATE>(state, LIFE_STATE::WILL_LIVE);
	}
}

void LIFE_HISTORY::CalculateNextGeneration() noexcept {
	auto nextGeneration = TIME_SLICE{ };
	nextGeneration.status = TIME_SLICE::STATUS::GENERATED;
	const auto& previousGeneration = m_History.back();
	
	// Advance the CELL state
	// Then, compute the predicted next state after that
	auto calc1 = [&](CELL_POSITION position) { TIME_SLICE::SetNextCellState(position, previousGeneration, nextGeneration); };
	auto calc2 = [&](CELL_POSITION position) { TIME_SLICE::CalculateNextLifeState(position, nextGeneration); };
	std::for_each(std::execution::par_unseq, allPositions.begin(), allPositions.end(), calc1);
	std::for_each(std::execution::par_unseq, allPositions.begin(), allPositions.end(), calc2);

	m_History.push_back(std::move(nextGeneration));
	++m_Generation;
}
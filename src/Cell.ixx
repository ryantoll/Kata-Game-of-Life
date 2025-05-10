import utilities;
#include <execution>
#include <vector>

export module lib_cell;

export namespace cell{
using namespace std;
using namespace RYANS_UTILITIES;

constexpr auto layoutWidth = 25;
constexpr auto layoutHeight = 25;

struct CELL_POSITION {
	constexpr CELL_POSITION() = default;
	constexpr CELL_POSITION(const unsigned int row_input, const unsigned int column_input) : row{ row_input }, column{ column_input } { }
	unsigned int row{ 0 };
	unsigned int column{ 0 };
};

struct ALL_CELL_POSITIONS {
	friend class ADJACENCY_LIST;
	ALL_CELL_POSITIONS();
	const auto begin() const { return positions.cbegin(); }
	const auto end() const { return positions.cend(); }
private:
	std::vector<CELL_POSITION> positions;
};

class ADJACENCY_LIST {
	std::vector<std::vector<std::vector<CELL_POSITION>>> adjacencyList{
		layoutHeight, std::vector<std::vector<CELL_POSITION>>{
			layoutWidth, std::vector<CELL_POSITION> { }
		}
	};
public:
	ADJACENCY_LIST();
	const std::vector<CELL_POSITION>& operator[] (CELL_POSITION position) const noexcept;
};

inline const ALL_CELL_POSITIONS allPositions;
inline const ADJACENCY_LIST adjacencyList;

// Tracks Alive/Dead for present generation, the immediately preceeding one, and the next one
// Bit-masking is used to create composite states
// Ordering of foundational states allows bitwise shift to move flags through time
enum class LIFE_STATE {

	// Foundational states
	WAS_ALIVE = 1u,
	ALIVE = 2u,
	WILL_LIVE = 4u,
	// WILL_LIVE_N_GENERATIONS = 2^(n-1)			// Available, but not explcitly supported

	// Composite states
	STABLE_DEAD = ALIVE ^ ALIVE,					// 0
	RECENTLY_DEAD = WAS_ALIVE,						// 1
	DYING = ALIVE | WAS_ALIVE,						// 3
	VASCILATING = WAS_ALIVE | WILL_LIVE,			// 5
	RECENTLY_GROWN = ALIVE | WILL_LIVE,				// 6
	STABLE_LIVING = ALIVE | WAS_ALIVE | WILL_LIVE	// 7
};

// Move life-state history tracking flags by bitwise shift
// Reverse through time
// Future -> [ Future + 1 ], Present -> Future, Past -> Present, [ Dead ] -> Past
inline LIFE_STATE operator<< (LIFE_STATE state, const unsigned int shift) {
	return static_cast<LIFE_STATE>(static_cast<std::underlying_type_t<LIFE_STATE>>(state) << shift);
}

// Move life-state history tracking flags by bitwise shift
// Past -> [ discarded ], Present -> Past, Future -> Present, [ Future + 1 ] -> Future
inline LIFE_STATE operator>> (LIFE_STATE state, const unsigned int shift) {
	return static_cast<LIFE_STATE>(static_cast<std::underlying_type_t<LIFE_STATE>>(state) >> shift);
}

class TIME_SLICE {
	friend class LIFE_HISTORY;
	struct CELL {
		LIFE_STATE state{ LIFE_STATE::STABLE_DEAD };
		unsigned int neighborCount{ 0 };
	};
	std::vector<std::vector<CELL>> m_Layout{ layoutHeight, std::vector<CELL>{ layoutWidth, CELL{ } } };
public:
	enum class STATUS {
		GENERATED = 0,
		MANUALLY_CHANGED = 1
	};

	class PROXY_CELL {
		friend class TIME_SLICE;
		CELL* m_Cell{ nullptr };
		CELL_POSITION m_Position{ };
		TIME_SLICE* m_Frame{ nullptr };
		PROXY_CELL(CELL* cell, CELL_POSITION position, TIME_SLICE* frame) : m_Cell(cell), m_Position(position), m_Frame(frame) { }
	public:
		// Really, this is the whole point of the class
		// Users should only change the lifestate indirectly to preserve invariants
		// Sets the state and adjusts neighbor count
		LIFE_STATE TogleDeadAlive() noexcept;
	private:
		PROXY_CELL& State(LIFE_STATE state) noexcept;
	public:
		[[nodiscard]] LIFE_STATE State() const noexcept { return m_Cell->state; }
		[[nodiscard]] unsigned int NeighborCount() const noexcept { return m_Cell->neighborCount; }
	};

	STATUS status{ STATUS::MANUALLY_CHANGED };
	[[nodiscard]] LIFE_STATE LifeState(CELL_POSITION position) const noexcept;
	[[nodiscard]] unsigned int NeighborCount(CELL_POSITION position) const noexcept;
	[[nodiscard]] PROXY_CELL operator[] (CELL_POSITION position) noexcept;
	[[nodiscard]] const PROXY_CELL operator[] (CELL_POSITION position) const noexcept;

private:
	static void SetNextCellState(const CELL_POSITION position, const TIME_SLICE& previousGeneration, TIME_SLICE& nextGeneration) noexcept;
	static void CalculateNeighborCount(const CELL_POSITION position, TIME_SLICE& nextGeneration) noexcept;
	static void CalculateNextLifeState(const CELL_POSITION position, TIME_SLICE& nextGeneration) noexcept;
};

// LIFE_HISTORY records the entire history of the board
// <s> Each TIME_SLICE is constant because each generation is deterministic </s>
// <s> Use of contstants also greatly simplifies multithreading </s>
// Manual changes by the user will be accomplished by adding a new frame with the change
// 
class LIFE_HISTORY {
	std::vector<TIME_SLICE> m_History{ 1, TIME_SLICE{ } };
	unsigned long m_Generation{ 0 };

public:
	const TIME_SLICE& Advance() noexcept;
	TIME_SLICE& GetLatestGeneration() noexcept { return m_History.back(); }

	// Get layout of generation #
	const TIME_SLICE& operator[] (size_t generationNumber) const noexcept;

	// Get generation count
	[[nodiscard]] size_t Generation() const noexcept;
private:
	void CalculateNextGeneration() noexcept;
};

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
	auto isAlive = EnumHasFlag(state, LIFE_STATE::ALIVE);
	if (state == m_Cell->state) { return *this; }
	else if (isAlive) { increment = 1; }
	else { increment = -1; }

	// Update predicted next generation expected life state
	if (m_Cell->neighborCount == 3 || m_Cell->neighborCount == 2 && isAlive) {
		m_Cell->state = EnumAddFlag(state, LIFE_STATE::WILL_LIVE);
	}
	else { m_Cell->state = state; }

	if (m_Frame->status == STATUS::GENERATED) { m_Frame->status = STATUS::MANUALLY_CHANGED; }

	for (auto& position : adjacencyList[m_Position]) {
		auto& neighborCell = (*m_Frame).m_Layout[position.row][position.column];
		neighborCell.neighborCount += increment;

		// Update predicted next generation expected life state
		if (neighborCell.neighborCount == 3 ||
			neighborCell.neighborCount == 2 && EnumHasFlag(neighborCell.state, LIFE_STATE::ALIVE)) {
			neighborCell.state = EnumAddFlag(neighborCell.state, LIFE_STATE::WILL_LIVE);
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

const TIME_SLICE& LIFE_HISTORY::operator[] (size_t generationNumber) const noexcept {
	return m_History[generationNumber];
}

[[nodiscard]] size_t LIFE_HISTORY::Generation() const noexcept {
	return m_History.size() - 1;
}

unsigned int SurroundingCellNumber(const CELL_POSITION position, const TIME_SLICE& timeSlice) noexcept {
	auto count = unsigned int{ 0 };
	for (auto& pos : adjacencyList[position]) {
		if (EnumHasFlag(timeSlice.LifeState(pos), LIFE_STATE::ALIVE)) {
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
		if (EnumHasFlag(previousGeneration.LifeState(pos), LIFE_STATE::WILL_LIVE)) {
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
	if (neighborCount == 3 || neighborCount == 2 && EnumHasFlag(state, LIFE_STATE::ALIVE)) {
		state = EnumAddFlag(state, LIFE_STATE::WILL_LIVE);
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

} // namespace cell

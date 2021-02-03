#ifndef CELL_CLASS_H
#define CELL_CLASS_H
#include "framework.h"
#include "__WINDOW.h"

constexpr auto layoutWidth = 25;
constexpr auto layoutHeight = 25;

struct CELL_POSITION {
	constexpr CELL_POSITION() = default;
	constexpr CELL_POSITION(const unsigned int row_input, const unsigned int column_input) : row{ row_input }, column{ column_input } { }
	unsigned int row{ 0 };
	unsigned int column{ 0 };
};

struct ALL_CELL_POSITIONS{
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

	// Get layout of generation #
	TIME_SLICE& operator[] (size_t generationNumber) noexcept;

	// Get generation count
	[[nodiscard]] size_t Generation() const noexcept;
private:
	void CalculateNextGeneration() noexcept;
};

#endif // !CELL_CLASS_H

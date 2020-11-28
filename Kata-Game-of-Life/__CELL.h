#ifndef CELL_CLASS_H
#define CELL_CLASS_H
#include "framework.h"
#include "__WINDOW.h"

constexpr auto layoutWidth = 25;
constexpr auto layoutHeight = 25;

struct CELL_POSITION {
	CELL_POSITION() = default;
	CELL_POSITION(int row_input, int column_input) : row{ row_input }, column{ column_input } { }
	int row{ 0 };
	int column{ 0 };
};

struct ALL_CELL_POSITIONS{
	std::vector<CELL_POSITION> allPositions;
	ALL_CELL_POSITIONS();
	auto begin() { return allPositions.begin(); }
	auto end() { return allPositions.end(); }
};

class ADJACENCY_LIST {
	std::vector<std::vector<std::vector<CELL_POSITION>>> adjacencyList{ layoutHeight, std::vector<std::vector<CELL_POSITION>>{ layoutWidth, std::vector<CELL_POSITION> { } } };
public:
	ADJACENCY_LIST();
	const std::vector<CELL_POSITION>& operator[] (CELL_POSITION position) const noexcept;
};

inline ALL_CELL_POSITIONS allPositions;
inline ADJACENCY_LIST adjacencyList;

enum class LIFE_STATE {
	DEAD = 0,
	ALIVE = 1
};

struct CELL {
	LIFE_STATE state{ LIFE_STATE::DEAD };
	unsigned int neighborCount{ 0 };
};

struct TIME_SLICE {
private:
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
		PROXY_CELL& State(LIFE_STATE state) noexcept;
		[[nodiscard]] LIFE_STATE State() const noexcept { return m_Cell->state; }
		[[nodiscard]] unsigned int NeighborCount() const noexcept { return m_Cell->neighborCount; }
	};

	STATUS status{ STATUS::MANUALLY_CHANGED };
	[[nodiscard]] LIFE_STATE LifeState(CELL_POSITION position) const noexcept;
	[[nodiscard]] unsigned int NeighborCount(CELL_POSITION position) const noexcept;
	[[nodiscard]] PROXY_CELL operator[] (CELL_POSITION position) noexcept;
	[[nodiscard]] const PROXY_CELL operator[] (CELL_POSITION position) const noexcept;
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
	TIME_SLICE& operator[] (unsigned long generationNumber) noexcept;

	// Get generation count
	[[nodiscard]] unsigned long Generation() const noexcept;
private:
	void CalculateNextGeneration() noexcept;
};

#endif // !CELL_CLASS_H

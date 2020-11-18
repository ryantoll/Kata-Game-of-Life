#ifndef CELL_CLASS_H
#define CELL_CLASS_H
#include "framework.h"
#include "__WINDOW.h"

constexpr auto layoutWidth = 25;
constexpr auto layoutHeight = 25;

struct CELL{
public:
	// Position of cell: Column, Row
	struct CELL_POSITION {
		int column{ 0 };
		int row{ 0 };
	};

	enum class LIFE_STATE {
		DEAD,
		ALIVE,
		//EMPTY,
		//JUST_DIED,
		//WILL_DIE,
		//STABLE,
		//WILL_GROW
	};

//private:
	CELL_POSITION m_position{ };
	LIFE_STATE m_State{ LIFE_STATE::DEAD };
	//WINDOWS_GUI::WINDOW m_Window{ };

public:

};

struct TIME_SLICE {
private:
	std::vector<std::vector<CELL>> m_Layout;
public:
	enum class STATUS {
		GENERATED,
		MANUALLY_CHANGED
	};
	STATUS status;
	[[nodiscard]] CELL Cell(CELL::CELL_POSITION position) const noexcept;
};

// LIFE_HISTORY records the entire history of the board
// Each TIME_SLICE is constant because each generation is deterministic
// Use of contstants also greatly simplifies multithreading
// Manual changes by the user will be accomplished by adding a new frame with the change
// 
class LIFE_HISTORY {
	std::vector<TIME_SLICE> m_History;
	unsigned long m_Generation{ 0 };

public:
	const TIME_SLICE& Advance() noexcept;
	const TIME_SLICE& Generation(unsigned long generationNumber) const noexcept;
	[[nodiscard]] unsigned long Generation() const noexcept;

private:
	void CalculateNextGeneration() noexcept;
};

#endif // !CELL_CLASS_H

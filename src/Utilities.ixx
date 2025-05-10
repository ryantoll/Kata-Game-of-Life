#include <string>
#include <type_traits>

export module utilities;

export namespace RYANS_UTILITIES {
	// Tool used for parsing text.
	// Tests for enclosing chars and clears them out. Returns bool indicating success/failure.
	inline bool ClearEnclosingChars(const char c1, const char c2, std::string& text) noexcept {
		if (text.size() < 2) { return false; }
		if (text[0] != c1 || text[text.size() - 1] != c2) { return false; }
		text.erase(0, 1);
		text.erase(text.size() - 1);
		return true;
	}

	// Tool used for parsing text.
	// Tests for enclosing wcahr_t and clears them out. Returns bool indicating success/failure.
	inline bool ClearEnclosingChars(const wchar_t c1, const wchar_t c2, std::wstring& text) noexcept {
		if (text.size() < 2) { return false; }
		if (text[0] != c1 || text[text.size() - 1] != c2) { return false; }
		text.erase(0, 1);
		text.erase(text.size() - 1);
		return true;
	}

	// Test if a scoped (class) enum has the specified flag
	// The "state" is expected to be an enum that uses bit-masking to combine multiple flags
	// Variable "hasFlag" is the flag of interest, which my be a component or composite flag
	template<class MyEnum>
	bool EnumHasFlag(MyEnum state, MyEnum hasFlag) {
		return static_cast<MyEnum>(
			static_cast<std::underlying_type_t<MyEnum>>(state) &
			static_cast<std::underlying_type_t<MyEnum>>(hasFlag)
			) == hasFlag;
	}

	// Add the specified flag to a scoped (class) enum
	// The "state" is expected to be an enum that uses bit-masking to combine multiple flags
	// Variable "addFlag" is the flag of interest, which my be a component or composite flag
	template<class MyEnum>
	MyEnum EnumAddFlag(MyEnum state, MyEnum addFlag) {
		return static_cast<MyEnum>(
			static_cast<std::underlying_type_t<MyEnum>>(state) |
			static_cast<std::underlying_type_t<MyEnum>>(addFlag)
			);
	}

	// Remove the specified flag from a scoped (class) enum
	// The "state" is expected to be an enum that uses bit-masking to combine multiple flags
	// Variable "removeFlag" is the flag of interest, which my be a component or composite flag
	template<class MyEnum>
	MyEnum EnumRemoveFlag(MyEnum state, MyEnum removeFlag) {
		return static_cast<MyEnum>(
			static_cast<std::underlying_type_t<MyEnum>>(state) &
			~(static_cast<std::underlying_type_t<MyEnum>>(removeFlag))
			);
	}

	// Toggle the specified flag in a scoped (class) enum
	// The "state" is expected to be an enum that uses bit-masking to combine multiple flags
	// Variable "removeFlag" is the flag of interest, which my be a component or composite flag
	template<class MyEnum>
	MyEnum EnumToggleFlag(MyEnum state, MyEnum toggleFlag) {
		return static_cast<MyEnum>(
			static_cast<std::underlying_type_t<MyEnum>>(state) ^
			static_cast<std::underlying_type_t<MyEnum>>(toggleFlag)
			);
	}
}

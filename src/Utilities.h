#ifndef RYANS_UTILITIES_H
#define RYANS_UTILITIES_H

#include "framework.h"

namespace RYANS_UTILITIES {

// Windows Utilities
#ifdef _WINDOWS

	// Wrapper for Windows char conversion utility function
	inline std::wstring StringToWstring(const std::string& InputString) noexcept {
		size_t n = InputString.size() + 1;
		std::unique_ptr<wchar_t[]> OutputC_String(new wchar_t[n]);
		MultiByteToWideChar(CP_UTF8, 0, InputString.c_str(), -1, OutputC_String.get(), static_cast<int>(n));
		return OutputC_String.get();
	}

	// Wrapper for Windows char conversion utility function
	inline std::string WstringToString(const std::wstring& InputWstring) noexcept {
		size_t n = InputWstring.size() + 1;
		std::unique_ptr<char[]> OutputC_String(new char[n]);
		WideCharToMultiByte(CP_UTF8, 0, InputWstring.c_str(), -1, OutputC_String.get(), static_cast<int>(n), NULL, NULL);
		return OutputC_String.get();
	}

	// Returns the wstring contained within an Edit Box
	// May work for other window types with text or a title
	inline std::wstring EditBoxToWstring(const HWND window) noexcept {
		int n = GetWindowTextLength(window) + 1;
		std::unique_ptr<wchar_t[]>OutputC_String(new wchar_t[n]);
		GetWindowText(window, OutputC_String.get(), n);
		return OutputC_String.get();
	}

	inline std::string EditBoxToString(const HWND window) noexcept {
		return WstringToString(EditBoxToWstring(window));
	}

	// Append wstring to text of an Edit Box
	inline void AppendWstringToEditBox(HWND window, const std::wstring& text) noexcept {
		auto sel = GetWindowTextLength(window);
		SendMessage(window, EM_SETSEL, (WPARAM)(sel), (LPARAM)sel);
		SendMessage(window, EM_REPLACESEL, 0, (LPARAM)text.c_str());
	}

	// Append string to text of an Edit Box
	inline void Append_String_to_Edit_Box(HWND window, const std::string& text) noexcept { AppendWstringToEditBox(window, StringToWstring(text)); }
#endif // _WINDOWS

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

#endif // !RYANS_UTILITIES_H

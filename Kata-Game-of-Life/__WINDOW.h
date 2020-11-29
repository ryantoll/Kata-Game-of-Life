#ifndef WINDOWS_GUI_FOUNDATION_LIBRARY
#define WINDOWS_GUI_FOUNDATION_LIBRARY

#include "framework.h"
//#include <Windows.h>
//#include <string>
#include "__Utilities.h"

namespace RYANS_UTILITIES {
	namespace WINDOWS_GUI {

		// class WINDOW_POSITION;
		// class WINDOW_DIMENSIONS;
		// class PAINT_TOKEN;

		class WINDOW_POSITION {
			int m_xPos, m_yPos;
		public:
			WINDOW_POSITION& X(const int x) noexcept { m_xPos = x; return *this; }
			WINDOW_POSITION& Y(const int y) noexcept { m_yPos = y; return *this; }
			[[nodiscard]] int X() const noexcept { return m_xPos; }
			[[nodiscard]] int Y() const noexcept { return m_yPos; }
		};

		class WINDOW_DIMENSIONS {
			int m_width, m_height;
		public:
			WINDOW_DIMENSIONS& Width(const int w) noexcept { m_width = w; return *this; }
			WINDOW_DIMENSIONS& Height(const int h) noexcept { m_height = h; return *this; }
			[[nodiscard]] int Width() const noexcept { return m_width; }
			[[nodiscard]] int Height() const noexcept { return m_height; }
		};

		// A PAINT_TOKEN represents ownership of a window ready for Windows drawing operations
		// Pass in the token in place of an HDC in any C-style Windows drawing funciton
		// Token automatically ends drawing and releases resources when it falls out of scope
		// Token must be released in the same thread in which it was created per Windows docs
		class PAINT_TOKEN {
			friend class WINDOW;
			HDC m_DeviceContext{ nullptr };
			HWND m_Window{ nullptr };
			explicit PAINT_TOKEN(HWND window) : m_Window(window) { m_DeviceContext = GetDC(window); }
		public:
			~PAINT_TOKEN() { ReleaseDC(m_Window, m_DeviceContext); }
			explicit operator bool() const noexcept { return m_DeviceContext == nullptr; }
			operator HDC() const noexcept { return m_DeviceContext; }
		};

		// This is a wrapper class to handle classic C-style Windows API calls in an object-oriented fashion
		// A WINDOW is a non-owning reference to a specific window
		// Any window thusly wrapped is still free to participate in C-style API calls as normal
		// WINDOW implicitly converts to HWND for use in such functions
		class WINDOW {
			HWND m_Handle{ nullptr };

		public:
			WINDOW() = default;
			// Control existing window
			explicit WINDOW(HWND hWindow) : m_Handle(hWindow) { }

			/*WINDOW(const WINDOW&) = default;
			WINDOW(WINDOW&&) = default;
			WINDOW& operator=(const WINDOW&) = default;
			WINDOW& operator=(WINDOW&&) = default;
			virtual ~WINDOW() { }*/

			HWND Handle() const noexcept { return m_Handle; }
			operator HWND() const noexcept { return m_Handle; }			// Allow for implicit conversion to HWND
			HWND Parent() const noexcept { return reinterpret_cast<HWND>(GetWindowLongPtr(m_Handle, GWLP_HWNDPARENT)); }
			HINSTANCE Instance() const noexcept { return reinterpret_cast<HINSTANCE>(GetWindowLongPtr(m_Handle, GWLP_HINSTANCE)); }
			HMENU ID() const noexcept { return Menu(); }
			HMENU Menu() const noexcept { return reinterpret_cast<HMENU>(GetWindowLongPtr(m_Handle, GWLP_ID)); }
			void Destroy() noexcept { DestroyWindow(m_Handle); }
			RECT GetClientRect() const noexcept { auto rekt = RECT{ }; ::GetClientRect(m_Handle, &rekt); return rekt; }
			WINDOW& Focus() noexcept { SetFocus(m_Handle); return *this; }
			const WINDOW& Focus() const noexcept { SetFocus(m_Handle); return *this; }
			WINDOW& Text(const std::string& title) noexcept { SetWindowText(m_Handle, StringToWstring(title).c_str()); return *this; }
			const WINDOW& Text(const std::string& title) const noexcept { SetWindowText(m_Handle, StringToWstring(title).c_str()); return *this; }
			WINDOW& Wtext(const std::wstring& title) noexcept { SetWindowText(m_Handle, title.c_str()); return *this; }
			const WINDOW& Wtext(const std::wstring& title) const noexcept { SetWindowText(m_Handle, title.c_str()); return *this; }
			WINDOW& Message(UINT message, WPARAM wparam, LPARAM lparam) noexcept { SendMessage(m_Handle, message, wparam, lparam); return *this; }
			const WINDOW& Message(UINT message, WPARAM wparam, LPARAM lparam) const noexcept { SendMessage(m_Handle, message, wparam, lparam); return *this; }
			WINDOW& Style(LONG_PTR style) noexcept { SetWindowLongPtr(m_Handle, GWL_STYLE, style); return *this; }
			const WINDOW& Style(LONG_PTR style) const noexcept { SetWindowLongPtr(m_Handle, GWL_STYLE, style); return *this; }

			[[nodiscard]] std::string Text() const noexcept { return EditBoxToString(m_Handle); }
			[[nodiscard]] std::wstring Wtext() const noexcept { return EditBoxToWstring(m_Handle); }
			[[nodiscard]] LONG_PTR Style() const noexcept { return GetWindowLongPtr(m_Handle, GWL_STYLE); }
			[[nodiscard]] PAINT_TOKEN BeginPaint() const noexcept { return PAINT_TOKEN{ m_Handle }; }
			[[nodiscard]] RECT GetClientRectangle() const noexcept { RECT rekt{ }; ::GetClientRect(m_Handle, &rekt); return rekt; }

			// Move, optionally resize
			WINDOW& Move(const WINDOW_POSITION position) noexcept {
				SetWindowPos(m_Handle, nullptr, position.X(), position.Y(), 0, 0, SWP_NOZORDER | SWP_NOSIZE);
				return *this;
			}

			// Move, optionally resize
			WINDOW& Move(const WINDOW_POSITION position, const WINDOW_DIMENSIONS size) noexcept {
				SetWindowPos(m_Handle, nullptr, position.X(), position.Y(), size.Width(), size.Height(), SWP_NOZORDER);
				return *this;
			}

			// Resize, optionally move
			WINDOW& Resize(const WINDOW_DIMENSIONS size) noexcept {
				SetWindowPos(m_Handle, nullptr, 0, 0, size.Width(), size.Height(), SWP_NOZORDER | SWP_NOMOVE);
				return *this;
			}

			// Resize, optionally move
			WINDOW& Resize(const WINDOW_DIMENSIONS size, const WINDOW_POSITION position) noexcept {
				return Move(position, size);
			}

			// Set the window procedure, returning the previous procedure
			// This can be used for sub-classing a window to add additional behavior
			WNDPROC Procedure(WNDPROC procedure) noexcept {
				return reinterpret_cast<WNDPROC>(SetWindowLongPtr(m_Handle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(procedure)));
			}

			WINDOW& Redraw() noexcept {
				auto rekt = RECT{ };
				::GetClientRect(m_Handle, &rekt);
				InvalidateRect(m_Handle, &rekt, false);
				SendMessage(m_Handle, WM_PAINT, NULL, NULL);
				return *this;
			}

			const WINDOW& Redraw() const noexcept {
				auto rekt = RECT{ };
				::GetClientRect(m_Handle, &rekt);
				InvalidateRect(m_Handle, &rekt, false);
				SendMessage(m_Handle, WM_PAINT, NULL, NULL);
				return *this;
			}
		};

		// Create a child window
		inline WINDOW ConstructChildWindow(const std::string& type, HWND parent, HMENU id) {
			auto h = CreateWindow(StringToWstring(type).c_str(), L"", WS_CHILD | WS_BORDER | WS_VISIBLE,
				0, 0, 0, 0, parent, id, reinterpret_cast<HINSTANCE>(GetWindowLongPtr(parent, GWLP_HINSTANCE)), NULL);
			return WINDOW{ h };
		}

		// Create a child window
		inline WINDOW ConstructChildWindow(const std::string& type, HWND parent, unsigned long id) {
			return ConstructChildWindow(type, parent, reinterpret_cast<HMENU>(static_cast<UINT_PTR>(id)));
		}

		// Create a window without a parent
		inline WINDOW ConstructTopLevelWindow(const std::string& type, HINSTANCE inst, const std::string& title) {
			auto h = CreateWindow(StringToWstring(type).c_str(), StringToWstring(title).c_str(), WS_OVERLAPPEDWINDOW, 0, 0, 0, 0, nullptr, nullptr, inst, NULL);
			return WINDOW{ h };
		}


		/*class WINDOW_POSITION {
			int m_xPos, m_yPos;
		public:
			WINDOW_POSITION& X(const int x) noexcept { m_xPos = x; return *this; }
			WINDOW_POSITION& Y(const int y) noexcept { m_yPos = y; return *this; }
			[[nodiscard]] int X() const noexcept { return m_xPos; }
			[[nodiscard]] int Y() const noexcept { return m_yPos; }
		};

		class WINDOW_DIMENSIONS {
			int m_width, m_height;
		public:
			WINDOW_DIMENSIONS& Width(const int w) noexcept { m_width = w; return *this; }
			WINDOW_DIMENSIONS& Height(const int h) noexcept { m_height = h; return *this; }
			[[nodiscard]] int Width() const noexcept { return m_width; }
			[[nodiscard]] int Height() const noexcept { return m_height; }
		};

		// A PAINT_TOKEN represents ownership of a window ready for Windows drawing operations
		// Pass in the token in place of an HDC in any C-style Windows drawing funciton
		// Token automatically ends drawing and releases resources when it falls out of scope
		// Token must be released in the same thread in which it was created per Windows docs
		class PAINT_TOKEN {
			friend class WINDOW;
			HDC m_DeviceContext{ nullptr };
			HWND m_Window{ nullptr };
			explicit PAINT_TOKEN(HWND window) : m_Window(window) { GetDC(window); }
		public:
			~PAINT_TOKEN() { ReleaseDC(m_Window, m_DeviceContext); }
			explicit operator bool() const noexcept { return m_DeviceContext == nullptr; }
			operator HDC() const noexcept { return m_DeviceContext; }
		};*/
	}
}

#endif // !WINDOWS_GUI_FOUNDATION_LIBRARY

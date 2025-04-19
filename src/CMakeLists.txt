# CMakeList.txt : CMake project for Static-Reflection-Example-Serialization-And-Comparison, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (Kata-Game-of-Life WIN32 "Windows_Infrastructure.cpp")
target_compile_definitions(Kata-Game-of-Life
  PRIVATE "UNICODE;_UNICODE"
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Kata-Game-of-Life PROPERTY CXX_STANDARD 17)
endif()

target_sources(Kata-Game-of-Life PRIVATE 
	Window-Input-Mapping.cpp
	CELL.cpp
	Kata-Game-of-Life.cpp
	Kata-Game-of-Life.rc
	Table.cpp
)

target_link_libraries(${PROJECT_NAME} User32.lib Gdi32.lib Kernel32.lib Advapi32.lib)

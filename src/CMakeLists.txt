# Add source to this project's executable.

add_library(utilities)
target_sources(utilities PUBLIC FILE_SET CXX_MODULES FILES Utilities.ixx)

add_library(window-utilities)
target_compile_definitions(window-utilities PRIVATE "UNICODE;_UNICODE;WIN32_LEAN_AND_MEAN")
target_sources(window-utilities PUBLIC FILE_SET CXX_MODULES FILES Window-Utilities.ixx)
target_link_libraries(window-utilities PUBLIC utilities)

add_library(lib_cell)
target_sources(lib_cell PUBLIC FILE_SET CXX_MODULES FILES Cell.ixx)
target_link_libraries(lib_cell PUBLIC utilities)

add_library(lib_table)
target_sources(lib_table PUBLIC FILE_SET CXX_MODULES FILES Table.ixx)
target_link_libraries(lib_table PUBLIC window-utilities lib_cell)

add_executable (Kata-Game-of-Life WIN32 "Windows_Infrastructure.cpp")
target_compile_definitions(Kata-Game-of-Life PRIVATE "UNICODE;_UNICODE;WIN32_LEAN_AND_MEAN")

target_sources(Kata-Game-of-Life PRIVATE 
	Window-Input-Mapping.cpp
	Kata-Game-of-Life.cpp
	Kata-Game-of-Life.rc
)

set_target_properties(Kata-Game-of-Life PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(Kata-Game-of-Life PUBLIC lib_table)

# Add source to this project's executable.
add_executable (Kata-Game-of-Life WIN32 "Windows_Infrastructure.cpp")
target_compile_definitions(Kata-Game-of-Life
  PRIVATE "UNICODE;_UNICODE"
)

target_sources(Kata-Game-of-Life PRIVATE 
	Window-Input-Mapping.cpp
	Kata-Game-of-Life.cpp
	Kata-Game-of-Life.rc
	CELL.cpp
	Table.cpp
)

add_library(utilities)
target_sources(utilities
  PUBLIC FILE_SET CXX_MODULES FILES Utilities.ixx
)

set_target_properties(Kata-Game-of-Life PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(Kata-Game-of-Life utilities)

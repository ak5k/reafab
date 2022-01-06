add_library(
	reafab OBJECT
	api.cpp
	imgui.cpp
	main.cpp
	map_plugins.cpp
	reafab.cpp
	)

target_link_libraries(reafab common)

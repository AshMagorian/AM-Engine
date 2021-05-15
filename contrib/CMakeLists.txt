add_library(imgui
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/imconfig.h
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.h
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_demo.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_draw.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_impl_glfw.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_impl_glfw.h
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_impl_opengl3.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_impl_opengl3.h
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_internal.h
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_tables.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_widgets.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/imstb_rectpack.h
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/imstb_textedit.h
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/imstb_truetype.h
)

target_include_directories (imgui PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/imgui")
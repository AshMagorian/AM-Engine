cmake_minimum_required(VERSION 2.6)
project(A_M_ENGINE)

include_directories(
src
contrib
)

if(MSVC)
  include_directories(
    ${CMAKE_SOURCE_DIR}/contrib/glew/include
    ${CMAKE_SOURCE_DIR}/contrib/openal-soft/include
    ${CMAKE_SOURCE_DIR}/contrib/ogg/include
    ${CMAKE_SOURCE_DIR}/contrib/vorbis/include
    ${CMAKE_SOURCE_DIR}/contrib/GLFW/include
    ${CMAKE_SOURCE_DIR}/contrib/assimp/include
    ${CMAKE_SOURCE_DIR}/contrib/imgui/include
	${CMAKE_SOURCE_DIR}/contrib/jsoncpp/include
  )

  link_directories(
    ${CMAKE_SOURCE_DIR}/contrib/glew/lib
    ${CMAKE_SOURCE_DIR}/contrib/openal-soft/lib
    ${CMAKE_SOURCE_DIR}/contrib/ogg/lib
    ${CMAKE_SOURCE_DIR}/contrib/vorbis/lib
    ${CMAKE_SOURCE_DIR}/contrib/GLFW/lib
    ${CMAKE_SOURCE_DIR}/contrib/assimp/lib
	${CMAKE_SOURCE_DIR}/contrib/imgui/lib
	${CMAKE_SOURCE_DIR}/contrib/jsoncpp/lib
  )
endif()


add_subdirectory(src)

add_executable(game
	src/game/main.cpp
	src/game/FirstPersonCameraControls.h
	src/game/FirstPersonCameraControls.cpp
)

target_link_libraries(game AM_Engine)

if(MSVC)
  add_custom_command(TARGET game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
	  "${CMAKE_SOURCE_DIR}/contrib/glew/bin/glew32.dll"
	  "${CMAKE_SOURCE_DIR}/contrib/openal-soft/bin/OpenAL32.dll"
	  "${CMAKE_SOURCE_DIR}/contrib/GLFW/bin/glfw3.dll"
	  "${CMAKE_SOURCE_DIR}/contrib/assimp/bin/assimp-vc141-mtd.dll"
	  "${CMAKE_SOURCE_DIR}/contrib/jsoncpp/bin/jsoncpp.dll"
	  "$<TARGET_FILE_DIR:game>"
  )
endif()
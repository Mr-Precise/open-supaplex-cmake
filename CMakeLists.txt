cmake_minimum_required(VERSION 3.3)
project(opensupaplex C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s")

if(MINGW OR WIN32)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mwindows")
endif(MINGW OR WIN32)

find_package(PkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2MIXER REQUIRED SDL2_mixer)

file(GLOB_RECURSE ${PROJECT_NAME}_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/**.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/**.h
)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/OpenSupaplex-bin)

set(Res_File "")
if(MINGW OR WIN32)
 set(Res_File ${CMAKE_CURRENT_SOURCE_DIR}/src/windows/OpenSupaplex.rc)
endif(MINGW OR WIN32)

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2MIXER_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SRC} ${Res_File})

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2MIXER_LIBRARIES} m)
if(MINGW)
  target_link_libraries(${PROJECT_NAME} ssp)
endif(MINGW)

file(GLOB resources
  "resources/*"
)

file(COPY ${resources} DESTINATION ${CMAKE_BINARY_DIR}/OpenSupaplex-bin)

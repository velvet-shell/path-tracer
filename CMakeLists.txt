cmake_minimum_required(VERSION 3.8)
project(rt)

include(CheckCXXCompilerFlag)

function(enable_cxx_compiler_flag_if_supported flag)
    string(FIND "${CMAKE_CXX_FLAGS}" "${flag}" flag_already_set)
    if(flag_already_set EQUAL -1)
        check_cxx_compiler_flag("${flag}" flag_supported)
        if(flag_supported)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}" PARENT_SCOPE)
        endif()
        unset(flag_supported CACHE)
    endif()
endfunction()

enable_cxx_compiler_flag_if_supported("-std=c++11")
enable_cxx_compiler_flag_if_supported("-O5")
enable_cxx_compiler_flag_if_supported("-mavx")
enable_cxx_compiler_flag_if_supported("-fopenmp")

include_directories(include)

add_executable(rt src/main.cpp src/stb_image.cpp)

file(GLOB TEXTURES "textures/*")
foreach(TEXTURE ${TEXTURES})
    file(COPY ${TEXTURE} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/textures)
endforeach(TEXTURE)
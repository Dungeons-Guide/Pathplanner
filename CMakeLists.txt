cmake_minimum_required(VERSION 3.27)
project(dptsp)


set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64")
set(JAVA_AWT_LIBRARY NotNeeded)
set(JAVA_AWT_INCLUDE_PATH NotNeeded)

find_package(JNI REQUIRED)
include_directories(${JNI_INCLUDE_DIRS})
message(JNI_INCLUDE_DIRs="${JNI_INCLUDE_DIRS}")

set(CMAKE_CXX_STANDARD 20)

add_library(dptsp SHARED
        tsp.cpp
        kr_syeyoung_dungeonsguide_mod_dungeon_actions_route_DPTSP.h
        jni.cpp)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
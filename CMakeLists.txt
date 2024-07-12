cmake_minimum_required(VERSION 3.28)
project(Chess-AI)

set(CMAKE_CXX_STANDARD 26)

set(CMAKE_PREFIX_PATH "C:\\Qt\\6.7.2\\mingw_64\\")
find_package(Qt6Widgets REQUIRED)

add_executable(Chess-AI main.cpp
        main.h
        Board.cpp
        Board.h
        Bot.cpp
        Bot.h
        MainWindow.cpp
        MainWindow.h)

target_link_libraries(Chess-AI Qt6::Widgets)
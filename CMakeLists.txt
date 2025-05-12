cmake_minimum_required(VERSION 3.10)
project(CelesteClone)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置x64平台
set(CMAKE_GENERATOR_PLATFORM x64)

# 添加EasyX库
find_package(EasyX REQUIRED)

# 添加源文件
set(SOURCES
    src/main.cpp
    src/game.cpp
    src/win32_platform.cpp
    src/animation/Assets.cpp
    src/animation/AnimationState.cpp
    src/Player.cpp
    src/Level.cpp
    src/Input.cpp
    src/visuals/effects/DashEffect.cpp
)

# 添加头文件目录
include_directories(src)

# 创建可执行文件
add_executable(CelesteClone ${SOURCES})

# 链接EasyX库
target_link_libraries(CelesteClone EasyX) 
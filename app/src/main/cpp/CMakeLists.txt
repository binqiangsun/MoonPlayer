
cmake_minimum_required(VERSION 3.4.1)

file(GLOB allCpp *.cpp)


add_library(
             player-lib
             SHARED
             ${allCpp} )


find_library(
              log-lib
              log )

# 定义ffmpeg总目录的变量
set(FFMPEG ${CMAKE_SOURCE_DIR}/ffmpeg)

# 定义 rtmp总目录的变量
set(RTMP ${CMAKE_SOURCE_DIR}/rtmp)

# 倒入ffmpeg头文件
include_directories(${CMAKE_SOURCE_DIR}/ffmpeg/include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${CMAKE_SOURCE_DIR}/ffmpeg/libs/${CMAKE_ANDROID_ARCH_ABI}")

# RTMP的库文件
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${CMAKE_SOURCE_DIR}/rtmp/libs/${CMAKE_ANDROID_ARCH_ABI}")

target_link_libraries(
        player-lib
        -Wl,--start-group #忽略顺序引发的问题
        avcodec avfilter avformat avutil swresample swscale
        -Wl,--end-group
        z
        rtmp
        android
        OpenSLES
        ${log-lib}
)
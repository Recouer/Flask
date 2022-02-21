cmake_minimum_required(VERSION 3.21)
project(FlaskInC C)

set(CMAKE_C_STANDARD 99)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)



add_executable(FlaskInC main.c
		src/concurrent/Node_concurrent.c src/Node.h
		src/concurrent/Game_concurrent.c src/Game.h)

target_link_libraries(FlaskInC PRIVATE Threads::Threads)
cmake_minimum_required(VERSION 3.21)
project(FlaskInC C)

set(CMAKE_C_STANDARD 99)

add_executable(FlaskInC main.c
		src/memory/Node_memory.c src/Node.h
		src/memory/Game_memory.c src/Game.h)

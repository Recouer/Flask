cmake_minimum_required(VERSION 3.21)
project(FlaskInC C)

set(CMAKE_C_STANDARD 99)

add_executable(FlaskInC main.c
		src/Node.c src/Node.h
		src/Game.c src/Game.h)

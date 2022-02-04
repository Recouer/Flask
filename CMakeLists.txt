cmake_minimum_required(VERSION 3.21)
project(FlaskInC C)

set(CMAKE_C_STANDARD 99)

add_executable(FlaskInC main.c
		src/pile/Node_pile.c src/Node.h
		src/pile/Game_pile.c src/Game.h)

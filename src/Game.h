#ifndef UNTITLED_GAME_H
#define UNTITLED_GAME_H

#include "Node.h"

typedef struct Node Node;
typedef struct Game {
	int number_of_flask;
	int flask_length;
	Node *root_node;
}Game;

Game *create_game(char * configuration_path,
                  ssize_t string_length);

void delete_game(Game *game);

void brut_force_solution(Game *game);

#endif //UNTITLED_GAME_H

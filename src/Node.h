#ifndef UNTITLED_NODE_H
#define UNTITLED_NODE_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "Game.h"

typedef struct Node {
	struct Game *game;

	int **list_of_flask;

	int **list_of_moves;
	int number_of_moves;

	struct Node *parent;

	struct Node **children;
	int checked_children;
	int number_of_children;
	int children_array_length;
}Node;


Node *create_node_root(char * configuration_path,
					   ssize_t string_length,
					   struct Game *game);


Node *create_node_children(Node *node,
                  int *move,
                  int flask_length,
                  int number_of_flasks);

void delete_node_children(Node* node);

void delete_node_root(Node *node);

int *create_flask(const int *flask_values, unsigned int size);

int *create_move(int outgoing_flask, int incoming_flask);

void move(Node *node,
          int* move,
          int flask_length,
          int number_of_flasks);

int finished(Node *node);

int check_loop(Node *node);

int **available_moves(Node *node, int* number_of_moves);


#endif //UNTITLED_NODE_H

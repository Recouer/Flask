#ifndef UNTITLED_NODE_H
#define UNTITLED_NODE_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "Game.h"

typedef struct Node Node;
typedef struct Game Game;

//region memory mgmt functions

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

//endregion

//region control flow functions

int finished(Node *node);

int check_loop(Node *node);

//endregion

//region Structure function

void move(Node *node,
          int* move,
          int flask_length,
          int number_of_flasks);


int **available_moves(Node *node, int* number_of_moves);

//endregion

//region utilities functions

int quantity_sent(const Node *node, int flask_number);

int has_one_color_only(const Node *node, int flaskIndex);

int flask_size(const Node *node, int flask_number);

//endregion

#endif //UNTITLED_NODE_H

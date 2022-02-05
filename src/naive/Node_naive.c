#include "../Node.h"


struct Game {
	int number_of_flask;
	int flask_length;
	Node *root_node;
};

 struct Node {
	struct Game *game;

	int **list_of_flask;

	int **list_of_moves;
	int number_of_moves;

	struct Node *parent;

	struct Node **children;
	int checked_children;
	int number_of_children;
	int children_array_length;
};

static void transfuse_flask(int *outgoing_flask,
                            int *incoming_flask,
                            Node *node,
                            const int *move,
                            unsigned int flask_length);

static void add_move(int ***available_moves,
                     int giving_flask,
                     int receiving_flask,
                     int *moves_count,
                     int *available_moves_length);



//region memory mgmt functions

Node *create_node_root(char *configuration_path,
                       ssize_t string_length,
                       struct Game *game
) {
	Node *new_node = (Node *) malloc(sizeof(Node));
	new_node->game = game;
	new_node->list_of_moves = NULL;
	new_node->number_of_moves = 0;
	new_node->children = NULL;
	new_node->checked_children = 0;
	new_node->children_array_length = 0;
	new_node->number_of_children = 0;

	new_node->parent = NULL;

	ssize_t char_read;
	int fd, index = 0;
	char buff[256], temp_buffer[20][100] = {};
	char delim[6] = " ,[]", line_delim[2] = "\n";

	if ((fd = open(configuration_path, 0)) == 0)
		perror("error on open\n");

	if ((char_read = read(fd, buff, string_length)) == -1)
		perror("error on reading: file too long\n");

	// useful to make strtok foolproof when there is no EOF in a file
	buff[char_read] = '\0';


	char *token = strtok(buff, line_delim);
	while (token != NULL && index < 20) {
		strcpy(temp_buffer[index++], token);
		token = strtok(NULL, line_delim);
	}


	int *flask, value;
	int **list_of_flasks = (int **) malloc(index * sizeof(int *));
	for (int i = 0; i < index; ++i) {
		token = strtok(temp_buffer[i], delim);
		flask = (int *) malloc(4 * sizeof(int));
		for (int j = 0; j < 4; ++j) {
			if (token != NULL) {
				value = atoi(token);
				token = strtok(NULL, delim);
				flask[j] = value;
			} else
				flask[j] = 0;
		}
		list_of_flasks[i] = flask;
	}

	new_node->list_of_flask = list_of_flasks;
	new_node->game->flask_length = 4;
	new_node->game->number_of_flask = index;

	return new_node;
}

Node *create_node_children(Node *node,
                           int *move,
                           int flask_length,
                           int number_of_flasks
) {
	Node *new_node = (Node *) malloc(sizeof(Node));
	new_node->list_of_flask = (int **) malloc(number_of_flasks * sizeof(int *));
	new_node->game = node->game;
	new_node->parent = node;

	int giving_flask[flask_length], receiving_flask[flask_length];
	transfuse_flask(giving_flask, receiving_flask, node, move, flask_length);

	for (int i = 0; i < number_of_flasks; ++i) {
		if (i == move[0])
			new_node->list_of_flask[i] = create_flask(giving_flask, flask_length);
		else if (i == move[1])
			new_node->list_of_flask[i] = create_flask(receiving_flask, flask_length);
		else
			new_node->list_of_flask[i] = node->list_of_flask[i];
	}


	new_node->number_of_moves = node->number_of_moves + 1;
	new_node->list_of_moves = (int **) malloc(new_node->number_of_moves * sizeof(int *));
	for (int i = 0; i < node->number_of_moves; ++i) {
		new_node->list_of_moves[i] = node->list_of_moves[i];
	}
	new_node->list_of_moves[node->number_of_moves] = move;

	new_node->children = NULL;
	new_node->checked_children = 0;
	new_node->number_of_children = 0;
	new_node->children_array_length = 0;

	return new_node;
}

void delete_node_children(Node *node) {
	free(node->list_of_flask[node->list_of_moves[node->number_of_moves - 1][0]]);
	free(node->list_of_flask[node->list_of_moves[node->number_of_moves - 1][1]]);
	free(node->list_of_moves[node->number_of_moves - 1]);
	free(node->children);
	free(node->list_of_moves);
	free(node->list_of_flask);
	free(node);
}

void delete_node_root(Node *node) {
	for (int i = 0; i < node->game->number_of_flask; ++i)
		free(node->list_of_flask[i]);
	free(node->list_of_flask);

	if (node->number_of_moves != 0) {
		for (int i = 0; i < node->number_of_moves; ++i) {
			free(node->list_of_moves[i]);
		}
		free(node->list_of_moves);
	}

	free(node->children);
	free(node);
}

int *create_flask(const int *flask_values, unsigned int size) {
	int *new_flask = (int *) malloc(size * sizeof(int));

	for (int i = 0; i < size; ++i) {
		new_flask[i] = flask_values[i];
	}

	return new_flask;
}

int *create_move(int giving_flask, int receiving_flask) {
	int *new_move = (int *) malloc(2 * sizeof(int));
	new_move[0] = giving_flask;
	new_move[1] = receiving_flask;
	return new_move;
}

//endregion

//region control flow functions

int finished(Node *node) {
	for (int i = 0; i < node->game->number_of_flask; ++i) {
		for (int j = node->game->flask_length - 2; j >= 0; --j) {
			if (node->list_of_flask[i][j] != node->list_of_flask[i][j + 1]) {
				return 0;
			}
		}
	}
	return 1;
}

int check_loop(Node *node) {
	int **configuration_list[5];
	Node *temp_node = node;
	for (int i = 0; i < 5; ++i) {
		configuration_list[i] = temp_node->list_of_flask;
		if (temp_node->parent != NULL) {
			temp_node = temp_node->parent;
		}
		else return 0;
	}

	int test1_1 = 1, test1_2 = 1, test1_3 = 1, test1_4 = 1;
	for (int i = 0; i < node->game->number_of_flask; ++i) {
		for (int j = 0; j < node->game->flask_length; ++j) {
			test1_1 &= configuration_list[0][i][j] == configuration_list[1][i][j];
			test1_2 &= configuration_list[0][i][j] == configuration_list[2][i][j];
			test1_3 &= configuration_list[0][i][j] == configuration_list[3][i][j];
			test1_4 &= configuration_list[0][i][j] == configuration_list[4][i][j];
		}
	}
	int test1 = test1_1 || test1_2 || test1_3 || test1_4;


	int test2 = node->list_of_moves[node->number_of_moves - 1] == node->list_of_moves[node->number_of_moves - 3] &&
	            node->list_of_moves[node->number_of_moves - 2] == node->list_of_moves[node->number_of_moves - 4];

	return test1 || test2;
}

//endregion

//region Structure function

void move(Node *node,
          int *move,
          int flask_length,
          int number_of_flasks) {

	if (node->children_array_length == 0) {
		node->children = (Node **) malloc(4 * sizeof(Node *));
		node->children_array_length = 4;
		node->children[node->number_of_children++] = create_node_children(node,
		                                                                  move,
		                                                                  flask_length,
		                                                                  number_of_flasks);
	} else if (node->children_array_length == node->number_of_children) {
		Node **temp = node->children;
		node->children_array_length *= 2;
		node->children = (Node **) malloc(node->children_array_length * sizeof(Node *));
		for (int i = 0; i < node->number_of_children; ++i) {
			node->children[i] = temp[i];
		}
		node->children[node->number_of_children++] = create_node_children(node,
		                                                                  move,
		                                                                  flask_length,
		                                                                  number_of_flasks);
		free(temp);
	} else
		node->children[node->number_of_children++] = create_node_children(node,
		                                                                  move,
		                                                                  flask_length,
		                                                                  number_of_flasks);
}


#define flask_is_full(flask, index) ((flask)[index][0] != 0)

#define flask_is_empty(node, index) ((node)->list_of_flask[index][(node)->game->flask_length - 1] == 0)

#define one_color_fill_flask(node, giving, receiving) (quantity_sent(node, giving) == \
												((node)->game->flask_length - flask_size(node, receiving)))

#define one_color_mount(node, giving, receiving) (flask_size(node, giving) == \
												((node)->game->flask_length - flask_size(node, receiving)))

int **available_moves(Node *node, int *number_of_moves) {

	if (node->parent == NULL) {
	}

	int available_moves_length = 4;
	int moves_count = 0;
	int **available_moves = (int **) malloc(available_moves_length * sizeof(int *));

	for (int giving_flask = 0; giving_flask < node->game->number_of_flask; ++giving_flask) {
		for (int receiving_flask = 0; receiving_flask < node->game->number_of_flask; ++receiving_flask) {

			if ((flask_is_full(node->list_of_flask, receiving_flask)) ||
			    (flask_is_full(node->list_of_flask, giving_flask) && has_one_color_only(node, giving_flask)) ||
			    (has_one_color_only(node, giving_flask) && flask_is_empty(node, receiving_flask)))
				continue;

			if (!flask_is_empty(node, giving_flask)) {
				if (!flask_is_empty(node, receiving_flask)) {
					// if (quantity_sent(node, giving_flask) > node->game->flask_length - flask_size(node, receiving_flask) &&)


					int going_flask_color = 0, coming_flask_color = 0;
					for (int k = 0; k < node->game->flask_length; ++k) {
						if (!going_flask_color) going_flask_color = node->list_of_flask[receiving_flask][k];
						if (!coming_flask_color) coming_flask_color = node->list_of_flask[giving_flask][k];
					}

					if (going_flask_color == coming_flask_color && giving_flask != receiving_flask) {
						add_move(&available_moves,
						         giving_flask,
						         receiving_flask,
						         &moves_count,
						         &available_moves_length);

						if ((has_one_color_only(node, giving_flask) &&
						     one_color_mount(node, giving_flask, receiving_flask)) ||
						    (has_one_color_only(node, receiving_flask) &&
						     one_color_fill_flask(node, giving_flask, receiving_flask))) {

							for (int i = 0; i < moves_count; ++i)
								free(available_moves[i]);
							free(available_moves);

							int **shortcut_list = (int **) malloc(sizeof(int *));
							shortcut_list[0] = create_move(giving_flask, receiving_flask);
							*number_of_moves = 1;
							return shortcut_list;
						}
					}
				} else {
					add_move(&available_moves,
					         giving_flask,
					         receiving_flask,
					         &moves_count,
					         &available_moves_length);
				}
			}
		}
	}
	*number_of_moves = moves_count;
	return available_moves;
}

//endregion

//region utilities functions

int quantity_sent(const Node *node, int flask_number) {

	int color = 0, counter = 0;
	for (int i = 0; i < node->game->flask_length; ++i) {
		if (!color && node->list_of_flask[flask_number][i] != 0)
			color = node->list_of_flask[flask_number][i];

		if (color != 0 && node->list_of_flask[flask_number][i] == color) counter++;
		if (color != 0 && node->flasks_list[flask_number][i] != color) break;
	}
	return counter;
}

int has_one_color_only(const Node *node, int flaskIndex) {
	int flask_length = node->game->flask_length;
	int *flask = node->list_of_flask[flaskIndex];

	int retval = 1;
	if (flask_length < 2)
		perror("flask length too short\n");

	for (int i = flask_length - 2; i >= 0 ; --i) {
		if (flask[i] == 0)
			break;
		retval &= flask[flask_length - 1] == flask[i];
	}
	return retval;
}

int flask_size(const Node* node, int flask_number) {
	int i;
	for (i = 0; i < node->game->flask_length; ++i)
		if (node->list_of_flask[flask_number][i] != 0) break;

	return node->game->flask_length - i;
}

//endregion

//region static functions

static void transfuse_flask(int *outgoing_flask,
                            int *incoming_flask,
                            Node *node,
                            const int *move,
                            unsigned int flask_length) {
	for (int i = 0; i < flask_length; ++i) {
		outgoing_flask[i] = node->list_of_flask[move[0]][i];
		incoming_flask[i] = node->list_of_flask[move[1]][i];
	}

	int available_space;
	int transferable_fluid, start = 0, end = 0;
	for (available_space = 0;
	     incoming_flask[available_space] == 0 && available_space < flask_length; available_space++) {}
	while (outgoing_flask[start] == outgoing_flask[end]) {
		if (outgoing_flask[start] == 0) start++;
		end++;
	}
	transferable_fluid = end - start;

	int temp;
	for (int i = 0; i < transferable_fluid && i < available_space; ++i) {
		temp = incoming_flask[available_space - i - 1];
		incoming_flask[available_space - i - 1] = outgoing_flask[start + i];
		outgoing_flask[start + i] = temp;
	}
}

static void add_move(int ***available_moves,
                     int giving_flask,
                     int receiving_flask,
                     int *moves_count,
                     int *available_moves_length) {

	if (*moves_count >= *available_moves_length) {
		*available_moves_length *= 2;
		int **temp = *available_moves;
		*available_moves = (int **) malloc(*available_moves_length * sizeof(int *));
		for (int k = 0; k < *moves_count; ++k) {
			(*available_moves)[k] = temp[k];
		}
		free(temp);
	}
	(*available_moves)[(*moves_count)++] = create_move(giving_flask, receiving_flask);
}

//endregion
#include "Game.h"
#include "Node.h"


Game *create_game(char * configuration_path,
                       ssize_t string_length
							   ) {
	Game *new_game = (Game *) malloc(sizeof(Game));
	new_game->root_node = create_node_root(configuration_path, string_length, new_game);

	return new_game;
}

void delete_game(Game *game) {
	delete_node_root(game->root_node);
	free(game);
}

static void delete_tree(Node *node) {
	printf("deleting tree\n");
	do {
		for (int i = node->checked_children; i < node->number_of_children; ++i) {
			delete_node_children(node->children[i]);
		}
		node = node->parent;
	} while (node != NULL);
}

Node *go_up(Node *node) {
	if (node->parent != NULL) {
		Node *temp = node;
		node = node->parent;
		delete_node_children(temp);
		node->checked_children++;
		return node;
	} else {
		delete_tree(node);
		return NULL;
	}
}

void node_info_print(Node *node) {
	for (int j = 0; j < node->number_of_moves; ++j)
		printf("[%d %d]", node->list_of_moves[j][0], node->list_of_moves[j][1]);
	printf("\n[ ");
	for (int j = 0; j < node->game->number_of_flask; ++j) {
		for (int k = 0; k < node->game->flask_length; ++k) {
			printf("%d ", node->list_of_flask[j][k]);
		}
		if (j == node->game->number_of_flask - 1) printf(" ]\n\n");
		else printf("][ ");
	}
}

#define receiver_accepts_all(node, giving, receiving) (quantity_sent(node, giving) <= \
												((node)->game->flask_length - flask_size(node, receiving)))

int move_is_valid(const Node* node, int* move, int** moves, int moves_length) {
	if (!receiver_accepts_all(node, move[0], move[1])) {
		int a = 0;
		for (int i = 0; i < moves_length; ++i) {
			if (move[0] == moves[i][0]) a++;
		}
		return a == 2;
	}
	return 1;
}

void brut_force_solution(Game *game) {

	Node *node = game->root_node;
	int a = 0;
	long long int loop = 0, nodes_count = 0;



	for (int i = 0; i < 10000000; ++i) {

		if (node == NULL) {
			perror("something went wrong\n");
			return;
		}

		if (check_loop(node)) {
			node = go_up(node);
			a--; loop++;
			continue;
		}

		if (finished(node)) {
			Node *temp = node;

			while (node->parent != NULL) {
				node_info_print(node);
				node = node->parent;
			}

			printf("%llu %llu %d\n", loop, nodes_count, a);
			delete_tree(temp);
			return;
		}

		if(node->children == NULL) {

			int number_of_moves = 0, valid_moves = 0;
			int**moves = available_moves(node, &number_of_moves);

			for (int j = 0; j < number_of_moves; ++j) {

				if (move_is_valid(node, moves[j], moves, number_of_moves)) {
					move(node, moves[j], node->game->flask_length, node->game->number_of_flask);
					valid_moves++;
				} else free(moves[j]);
			}

			if (valid_moves == 0) {
				node = go_up(node);
				free(moves);
				a--;
				continue;
			}
			free(moves);
		}

		if (node->checked_children >= node->number_of_children) {
			node = go_up(node);
			a--;
		} else {
			nodes_count++;
			node = node->children[node->checked_children];
			a++;
		}
	}

	while (node->parent != NULL) {
		node_info_print(node);
		node = node->parent;
	}

	printf("%llu %llu %d\n", loop, nodes_count, a);
	delete_tree(node);
}

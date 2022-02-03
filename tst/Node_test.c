#include "../src/Game.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

int test_create_node_root() {

	int flasks[14][4] = {
			0, 0, 0, 0,
			0, 0, 0, 0,
			4, 3, 2, 1,
			8, 7, 6, 5,
			8, 9, 2, 3,
			6, 10, 4, 4,
			9, 10, 3, 1,
			3, 10, 5, 8,
			7, 12, 5, 11,
			12, 2, 4, 11,
			2, 1, 9, 7,
			12, 6, 9, 11,
			10, 11, 12, 1,
			7, 8, 5, 6
	};

	Game *game = create_game("/home/recouer/Exercise/Projets/FlaskInC/python/result.txt",
							 256);

	assert(game->number_of_flask == 14);
	assert(game->flask_length == 4);
	assert(game->root_node != NULL);

	Node *node = game->root_node;

	assert(node->game == game);
	assert(node->parent == NULL);

	for (int i = 0; i < game->number_of_flask; ++i) {
		for (int j = 0; j < game->flask_length; ++j) {
			assert(flasks[i][j] == node->list_of_flask[i][j]);
		}
	}

	delete_game(game);

	return 0;
}

static void print_flask(Node *node) {
	for (int i = 0; i < node->number_of_children; ++i) {
		for (int j = 0; j < node->game->number_of_flask; ++j) {
			for (int k = 0; k < node->game->flask_length; ++k) {
				printf("%d ", node->children[i]->list_of_flask[j][k]);
			}
			printf("\n");
		}
		printf("\n");
	}
	printf("\n");
}

int test_create_node_children() {


	Game *game = create_game("/home/recouer/Exercise/Projets/FlaskInC/python/test_files/test2.txt",
	                         256);
	int length;
	int **list = available_moves(game->root_node, &length);

	for (int i = 0; i < length; ++i) {
		move(game->root_node, list[i], game->flask_length, game->number_of_flask);
	}
	print_flask(game->root_node);

	return 0;
}

int test_move() {

	return 0;
}

int test_finished() {


	Game *game = create_game("/home/recouer/Exercise/Projets/FlaskInC/python/test_files/test1.txt",
	                   256);


	int a = finished(game->root_node);

	assert(a == 1);

	return 0;
}

int test_check_loop() {

	return 0;
}

static void free_game(Game *game, int **list, int length) {
	for (int i = 0; i < length; ++i) {
		free(list[i]);
	}
	free(list);
	delete_game(game);
}

static int test_available_moves() {

	Game *game = create_game("/home/recouer/Exercise/Projets/FlaskInC/python/result.txt",
	                         256);

	int result_test[24][2] = {
			0, 2,1, 2,
			0, 3,1, 3,
			0, 4,1, 4,
			0, 5,1, 5,
			0, 6,1, 6,
			0, 7,1, 7,
			0, 8,1, 8,
			0, 9,1, 9,
			0, 10,1, 10,
			0, 11,1, 11,
			0, 12,1, 12,
			0, 13,1, 13,
	};

	int length;
	int **list = available_moves(game->root_node, &length);


	for (int i = 0; i < length; ++i) {
		assert(result_test[i][0] == list[i][1]);
		assert(result_test[i][1] == list[i][0]);
	}
	free_game(game, list, length);


	game = create_game("/home/recouer/Exercise/Projets/FlaskInC/python/test_files/test1.txt",
	                         256);
	list = available_moves(game->root_node, &length);
	assert(length == 0);
	free_game(game, list, length);


	game = create_game("/home/recouer/Exercise/Projets/FlaskInC/python/test_files/test2.txt",
	                   256);
	list = available_moves(game->root_node, &length);


	int test2[5][2] = {0, 1,
					   1, 0,
					   3, 2,
					   3, 4,
					   4,  3
	};
	for (int i = 0; i < length; ++i) {
		assert(list[i][0] == test2[i][0]);
		assert(list[i][1] == test2[i][1]);
	}

	free_game(game, list, length);
	return 0;
}



int main(int argc, char *argv[]) {

	test_create_node_root();
	test_create_node_children();
	test_check_loop();
	test_finished();
	test_available_moves();
	test_move();


	return 0;
}



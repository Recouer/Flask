#include "../Game.h"
#include "../Node.h"

#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


struct Node_scheduler {
	Node ***Node_list;
	int Node_list_size, *prio_list_size, *prio_list_length;

	int max_priority_node;
};

struct Game {
	Node *root_node;

	struct Node_scheduler *Sched;
};

typedef struct moves_list {
	int *list;
	int list_size, list_length;
}moves_list;

struct Node {
	int **flasks_list;
	int number_of_flasks, flask_length;

	moves_list *list_of_moves;

	struct Node *parent;
	struct Node *child;

	int *children;
	int children_checked, children_length, children_size;
};

void node_info_print(Node *node) {
	for (int j = 0; j < node->list_of_moves->list_length; ++j)
		printf("[%d %d]", node->list_of_moves->list[2 * j], node->list_of_moves->list[2 * j + 1]);
	printf("\n[ ");
	for (int j = 0; j < node->number_of_flasks; ++j) {
		for (int k = 0; k < node->flask_length; ++k) {
			printf("%d ", node->flasks_list[j][k]);
		}
		if (j == node->number_of_flasks - 1) printf(" ]\n\n");
		else printf("][ ");
	}
}

Game *create_game(char * configuration_path,
                  ssize_t string_length
) {
	Game *new_game = (Game *) malloc(sizeof(Game));
	new_game->root_node = create_node_root(configuration_path, string_length, new_game);

	new_game->Sched = (struct Node_scheduler *) malloc( sizeof(struct Node_scheduler));
	new_game->Sched->Node_list_size = 50;
	new_game->Sched->prio_list_size = (int *) malloc(new_game->Sched->Node_list_size * sizeof(int));
	new_game->Sched->prio_list_length = (int *) malloc(new_game->Sched->Node_list_size * sizeof(int));

	new_game->Sched->Node_list = (Node ***) malloc(new_game->Sched->Node_list_size * sizeof(Node **));
	for (int i = 0; i < new_game->Sched->Node_list_size; ++i) {
		new_game->Sched->prio_list_size[i] = 8;
		new_game->Sched->prio_list_length[i] = 0;
		new_game->Sched->Node_list[i] = (Node **) malloc(new_game->Sched->prio_list_size[i] * sizeof(Node *));
	}

	new_game->Sched->max_priority_node = 0;
	new_game->Sched->Node_list[0][new_game->Sched->prio_list_length[0]++] = new_game->root_node;

	return new_game;
}

void delete_game(Game *game) {
	delete_node_root(game->root_node);
	for (int i = 0; i < game->Sched->Node_list_size; ++i)
		free(game->Sched->Node_list[i]);
	free(game->Sched->prio_list_size);
	free(game->Sched->prio_list_length);
	free(game->Sched->Node_list);
	free(game->Sched);
	free(game);
}

static void delete_tree(Node *node) {
	printf("deleting tree\n");
	while (node->parent != NULL) {
		Node *parent = node->parent;
		node_info_print(node);
		delete_node_children(node);
		node = parent;
	}
	printf("finished deleting tree\n");
}

Node *go_up(Node *node) {
	if (node->parent != NULL) {
		Node *temp = node;
		node = node->parent;
		delete_node_children(temp);
		return node;
	} else {
		delete_tree(node);
		return NULL;
	}
}


//region Thread Algorithm

struct thread_info {
	pthread_t thread_id;
	Game *game;
};


static void *thread_DFS(void *args) {
	struct thread_info *tinfo = args;
	Game *game = tinfo->game;

	Node *node = game->root_node;
	int a = 0;
	long long int loop = 0, nodes_count = 0;

	for (int i = 0; i < 10000000; ++i) {

		if (node == NULL) {
			perror("something went wrong\n");
			return NULL;
		}

		if (check_loop(node)) {
			node = go_up(node);
			a--; loop++;
			continue;
		}

		if (finished(node)) {
			printf("\nfinished\n");
			printf("%llu %llu %d\n", loop, nodes_count, a);
			delete_tree(node);
			return NULL;
		}

		if (assign_child_successful(node)) {
			nodes_count++;
			node = node->child;
			a++;
		} else {
			node = go_up(node);
			a--;
		}
	}
	printf("%llu %llu %d\n", loop, nodes_count, a);
	delete_tree(node);
	return NULL;
}


//endregion

//region Main Algorithm

void brut_force_solution(Game *game) {
	int s, num_threads;
	pthread_attr_t attr;
	void *res;

	long number_of_processors = sysconf(_SC_NPROCESSORS_ONLN);
	number_of_processors = 1;
	num_threads = (int) number_of_processors;

	if (pthread_attr_init(&attr) != 0) perror("pthread_attr_init");
	struct thread_info *tinfo = calloc(num_threads, sizeof(*tinfo));
	if (tinfo == NULL) perror("calloc");


	for (int tnum = 0; tnum < num_threads; tnum++) {
		tinfo[tnum].game = game;

		s = pthread_create(&tinfo[tnum].thread_id, &attr,
		                   &thread_DFS, &tinfo[tnum]);
		if (s != 0)
			perror("pthread_create");
	}


	if (pthread_attr_destroy(&attr) != 0) perror("pthread_attr_destroy");


	for (int tnum = 0; tnum < num_threads; tnum++) {
		s = pthread_join(tinfo[tnum].thread_id, &res);
		if (s != 0)
			perror("pthread_join");
		free(res);
	}

	free(tinfo);
}

//endregion
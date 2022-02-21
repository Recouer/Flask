#include "../Game.h"
#include "../Node.h"


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

	struct Node **children;
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
			printf("\nfinished\n");
			printf("%llu %llu %d\n", loop, nodes_count, a);
			delete_tree(node);
			return;
		}

		if (assign_child_successful(node)) {
			nodes_count++;
			node = node->children[node->children_checked];
			a++;
		} else {
			node = go_up(node);
			a--;
		}
	}
	printf("%llu %llu %d\n", loop, nodes_count, a);
	delete_tree(node);
}

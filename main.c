#include "src/Game.h"
#include <time.h>


int main(int argc, char *argv[]) {

	clock_t start, end;
	double cpu_time_used;

	start = clock();


	Game *game = create_game("/home/recouer/Exercise/Projets/FlaskInC/python/result.txt", 256);
	brut_force_solution(game);

	delete_game(game);

	end = clock();
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

	printf("the implementation took %f seconds to run\n", cpu_time_used);


	return 0;
}

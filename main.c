#include "src/Game.h"



int main(int argc, char *argv[]) {

	Game *game = create_game("/home/recouer/Exercise/Projets/FlaskInC/python/result.txt", 256);
	brut_force_solution(game);

	delete_game(game);

	return 0;
}

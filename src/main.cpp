#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include "cvrp.h"
#include "sim_annealing.cpp"

using namespace std;

int main(int argc, char** argv)
{
	int i = 0;

	// Definidas pela linha de comando
	float temperature = 10.0f;
	int num_it = 100000;
	int iteration_step = 100;
	float iteration_decrease = 0.1f;
	int stop_criteria = num_it;
	int num_repetitions = 1;
	bool gen_gnuplot_files = false;
	bool random_initial_solution = false;

	// Handle de linha de comando
	if (argc < 3)
	{
		std::cout << "usage: temperature max_iterations" << std::endl;
		std::cout << "Optional flags: " << std::endl;
		std::cout << 
			" -is [iteration_step] \t\t-> default = 100\t(int)\n"
			" -id [iteration_decrease] \t-> default = 0.1\t(float)\n"
			" -sc [stop_criteria] \t\t-> default = 100000\t(int)\n"
			" -n  [num_repetitions] \t\t-> default = 1\t\t(int)\n"
			" -gf \t\t\t\t-> generate gnuplot files\n"
			" -rs \t\t\t\t-> random initial solution" << std::endl;

		return EXIT_FAILURE;
	}
	else
	{
		temperature = atof(argv[1]);
		num_it = atoi(argv[2]);
		if (argc > 3)
		{
			for (int arg = 3; arg < argc; ++arg)
			{
				if (!strcmp("-is", argv[arg]))
				{
					iteration_step = atof(argv[arg + 1]);
				}
				if (!strcmp("-id", argv[arg]))
				{
					iteration_decrease = atof(argv[arg + 1]);
				}
				if (!strcmp("-sc", argv[arg]))
				{
					stop_criteria = atof(argv[arg + 1]);
				}
				if (!strcmp("-n", argv[arg]))
				{
					num_repetitions = atoi(argv[arg + 1]);
				}
				if (!strcmp("-gf", argv[arg]))
				{
					gen_gnuplot_files = true;
				}
			}
		}
	}
	
	// Aloca espaço para entrada
	char* data = new char[1024 * 1024];
	memset(data, 0, 1024 * 1024);

	// lê entrada -> linhas 256 bytes
	for (int c = 0; cin.getline(data + i, 256); i += c)
		c = cin.gcount();
	
	int num_lines = i / 256;

	// Faz o parse da entrada e gera uma instância do problema
	char* ptr = data;
	ProblemInstance prob = parse_data(&data);
	delete[] ptr;

	Timer timer;
	timer.StartTimer();

	// Gera solução aleatória se especificada
	// senão gera solução pela heurística
	std::vector<Vehicle> solution;
	if (random_initial_solution)
	{
		while (GenRandomSolution(&prob, solution, GEN_SEED) == 0);
	}
	else
	{
		GenHeuristicSolution(&prob, solution);
	}

	// Calculate initial cost
	float current_cost = 0;
	for (int i = 0; i < solution.size(); ++i)
		current_cost += CalculateCost(solution[i], prob.depot);
	std::cout << current_cost << "\n";

	// Initialize random for the algorithm run
	srand(time(0));
	int vrand1 = 0;
	int vrand2 = 0;
	int crand1 = 0;
	int crand2 = 0;

	do
	{
		int stop = 0;

		for (int it = 0; it < num_it; ++it)
		{
			// Simulated Annealing, diminui temperatura de acordo com iteration_step
			// se temperatura chegar a 1, permanece em 1 (Hill Climbing)
			if (it % iteration_step)
			{
				if (temperature >= 1)
					temperature -= iteration_decrease;
				else
					temperature = 1;
			}

			// Gera vizinho
			do
			{
				do {
					vrand1 = rand() % prob.vehicles;
					vrand2 = rand() % prob.vehicles;
					crand1 = rand() % solution[vrand1].route.size();
					crand2 = rand() % solution[vrand2].route.size();
				} while (crand1 == crand2);
			
			} while (!GetNeighbor(solution[vrand1], solution[vrand2], crand1, crand2));

			Client temp = solution[vrand1].route[crand1];
			solution[vrand1].route[crand1] = solution[vrand2].route[crand2];
			solution[vrand2].route[crand2] = temp;

			// Calcula o custo de mover para o vizinho gerado
			float next_cost = 0;
			for (int i = 0; i < solution.size(); ++i)
				next_cost += CalculateCost(solution[i], prob.depot);

			float delta_e = next_cost - current_cost;

			stop++;
			if (stop >= stop_criteria)
				break;

			// Calcula probabilidade de ir para esse vizinho
			float prob = 1.0f / (1 + pow(NUM_E, delta_e / temperature)) * 100;
			int prob_int = round(prob);

			// Move com probabilidade calculada
			int move = rand() % 100;
			if(move < prob_int)
			{
				current_cost = next_cost;	// Avança para o vizinho calculado
				stop = 0;					// reseta condição de parada pois o algoritmo trocou para vizinho
			}
			else
			{
				Client temp = solution[vrand1].route[crand1];
				solution[vrand1].route[crand1] = solution[vrand2].route[crand2];
				solution[vrand2].route[crand2] = temp;
			}
		}
	
		// Calculate and print to stdout the final cost and final solution
		float total_cost = 0;
		for (int i = 0; i < solution.size(); ++i)
		{
			total_cost += CalculateCost(solution[i], prob.depot);
			std::cout << "Rota " << i << ": ";
			for (int j = 0; j < solution[i].route.size(); ++j)
				std::cout << solution[i].route[j].id << " ";
			std::cout << "\n";
		}
		std::cout << "The total cost of the algorithm is " << total_cost << std::endl;
		std::cout << "Time elapsed: " << timer.ElapsedTime() << std::endl << std::endl;
		timer.StartTimer();
		num_repetitions--;
	} while (num_repetitions != 0);

	if (gen_gnuplot_files)
	{
		// Generate gnuplot files
		for (int i = 0; i < solution.size(); ++i)
		{
			std::string dat = "data" + std::to_string(i) + ".txt";
			std::ofstream file;
			file.open(dat);

			for (int j = 0; j < solution[i].route.size(); ++j)
				file << solution[i].route[j].x << " " << solution[i].route[j].y << "\n";
			file.close();
		}
	}

	return 0;
}
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
	// Aloca espaço para entrada
	char* data = new char[1024 * 1024];
	memset(data, 0, 1024 * 1024);

	// lê entrada -> linhas 256 bytes
	for (int c = 0; cin.getline(data + i, 256); i += c)
		c = cin.gcount();
	
	int num_lines = i / 256;

	char* ptr = data;
	ProblemInstance prob = parse_data(&data);
	delete[] ptr;

	// Generate initial solution for the problem
	std::vector<Vehicle> solution;
	//while (GenRandomSolution(&prob, solution, GEN_SEED) == 0);
	int s = GenHeuristicSolution(&prob, solution);

	// Calcula custo inicial
	float current_cost = 0;
	for (int i = 0; i < solution.size(); ++i)
		current_cost += CalculateCost(solution[i], prob.depot);


	srand(time(0));
	int vrand1 = 0;
	int vrand2 = 0;
	int crand1 = 0;
	int crand2 = 0;

	std::cout << current_cost << "\n";

	// Definidas pela linha de comando
	float temperature = 10.0f;
	int num_it = 100000;
	int iteration_step = 100;
	float iteration_decrease = 0.1f;
	int stop_criteria = 10000;

	int stop = 0;

	for (int it = 0; it < num_it; ++it)
	{
		if (it % iteration_step)
		{
			if (temperature >= 1)
				temperature -= iteration_decrease;
			else
				temperature = 1;
		}
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

		float next_cost = 0;
		for (int i = 0; i < solution.size(); ++i)
			next_cost += CalculateCost(solution[i], prob.depot);

		float delta_e = next_cost - current_cost;

		stop++;
		if (stop >= stop_criteria)
			break;

		float prob = 1.0f / (1 + pow(NUM_E, delta_e / temperature)) * 100;			// Calcula probabilidade de ir para esse vizinho
		int prob_int = round(prob);

		// Move com probabilidade calculada
		int move = rand() % 100;
		if(move < prob_int)
		{
			current_cost = next_cost;
			stop = 0;	// reseta condição de parada pois o algoritmo trocou para vizinho
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

	return 0;
}
#include <iostream>
#include <vector>
#include "cvrp.h"
#include "sim_annealing.cpp"
#include <fstream>
#include <string>

using namespace std;

ProblemInstance parse_data(char** data)
{
	ProblemInstance pi = {};
	bool eof = false;
	int counter = 0;
	do {
		int first_equal = cmp_first_word(*data, fields[counter]);
		if (!first_equal)
		{
			for (counter = 0; counter < _EOF + 1; ++counter)
			{
				first_equal = cmp_first_word(*data, fields[counter]);
				if (first_equal)
					break;
			}
		}
		if (first_equal)
		{
			if (counter < NODE_COORD_SECTION) 
			{
				skip(data, ':');
				eat_ws(data);
			}

			switch (counter)
			{
			case NAME:					copy_str(pi.name, *data);				skip(data, 0); eat_ws(data); break;
			case TYPE:					copy_str(pi.type, *data);				skip(data, 0); eat_ws(data); break;
			case COMMENT:				copy_str(pi.comment, *data);			skip(data, 0); eat_ws(data); break;
			case DIMENSION:				pi.dimension = atoi(*data);				skip(data, 0); eat_ws(data); break;
			case CAPACITY:				pi.capacity = atoi(*data);				skip(data, 0); eat_ws(data); break;
			case VEHICLES:				pi.vehicles = atoi(*data);				skip(data, 0); eat_ws(data); break;
			case EDGE_WEIGHT_TYPE:		copy_str(pi.edge_weight_type, *data);	skip(data, 0); eat_ws(data); break;
			case EDGE_WEIGHT_FORMAT:	copy_str(pi.edge_weight_format, *data);	skip(data, 0); eat_ws(data); break;
			case NODE_COORD_TYPE:		copy_str(pi.node_coord_type, *data);	skip(data, 0); eat_ws(data); break;
			case NODE_COORD_SECTION:	
			{
				skip(data, 0);
				eat_ws(data);

				while (!cmp_first_word(*data, "DEMAND_SECTION"))
				{
					skip(data, ' ');
					float xdata = atof(*data);
					skip(data, ' ');
					float ydata = atof(*data);
					pi.node_coord.push_back(vec2(xdata, ydata));
					skip(data, 0);
				}
			} break;
			case DEMAND_SECTION: {
				skip(data, 0);
				while (!cmp_first_word(*data, "DEPOT_SECTION"))
				{
					skip(data, ' ');
					int i = atoi(*data);
					pi.demand.push_back(i);
					skip(data, 0);
				}
			} break;
			case DEPOT_SECTION: {
				skip(data, 0);
				eat_ws(data);
				pi.depot.x = atof(*data);
				skip(data, ' ');
				eat_ws(data);
				pi.depot.y = atof(*data);
			} break;
			}
		}
		if (counter >= 12 || cmp_first_word(fields[counter], *data))
			eof = true;
		counter = 0;
	} while (!eof);

	// Se demanda do primeiro for 0, então este é depósito
	if (pi.demand[0] == 0)
	{
		pi.depot.x = pi.node_coord[0].x;
		pi.depot.y = pi.node_coord[0].y;
		pi.node_coord.erase(pi.node_coord.begin());
		pi.demand.erase(pi.demand.begin());
	}

	return pi;
}

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

	std::vector<Vehicle> solution;
	//while (GenRandomSolution(&prob, solution, GEN_SEED) == 0);
	int s = GenHeuristicSolution(&prob, solution);

	float current_cost = 0;
	for (int i = 0; i < solution.size(); ++i)
		current_cost += CalculateCost(solution[i], prob.depot);

	float temperature = 10.0f;

	srand(time(0));
	int vrand1 = 0;
	int vrand2 = 0;
	int crand1 = 0;
	int crand2 = 0;

	std::cout << current_cost << "\n";
	int num_it = 10000000;
	for (int it = 0; it < num_it; ++it)
	{
		if (it % 10000)
		{
			if (temperature >= 1)
				temperature -= 0.1f;
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
		float prob = 1.0f / (1 + pow(NUM_E, delta_e / temperature)) * 100;
		int prob_int = round(prob);

		int move = rand() % 100;

		//if (next_cost < current_cost)
		if(move < prob_int)
		{
			current_cost = next_cost;
		}
		else
		{
			Client temp = solution[vrand1].route[crand1];
			solution[vrand1].route[crand1] = solution[vrand2].route[crand2];
			solution[vrand2].route[crand2] = temp;
		}
	}
	
	// Calculate the final cost
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
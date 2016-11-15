#include "cvrp.h"
#include <time.h>

#define GEN_SEED 0

/*
	Generates a solution for the CVRP problem

	- return 0:
		the function could not generate a correct solution
		so the caller must call it again

	- return 1
		the function succeeded
*/
int GenSolution(ProblemInstance* instance, std::vector<Vehicle>& solution)
{
	int capacity = instance->capacity;
	int num_vehicles = instance->vehicles;
	int dimension = instance->dimension;
	
	int client = 0;

	for (int i = 0; i < num_vehicles; ++i)
	{
		Vehicle v;
		v.id = i;
		v.capacity = capacity;

		int cap = 0;
		for (; cap <= capacity && client < dimension - 1; ++client)
		{
			int c_demand = instance->demand[client];
			if (c_demand + cap > capacity)
				break;

			Client c(instance->node_coord[client].x, instance->node_coord[client].y, client, c_demand);
			v.route.push_back(c);
			cap += c.demand;
		}
		v.cargo = cap;
		solution.push_back(v);
	}

	// Verify that every client is supplied
	for (int i = 0; client < dimension - 1; ++i)
	{
		if (i >= num_vehicles)
			return 0;
		int c_demand = instance->demand[client];
		if (c_demand + solution[i].cargo <= capacity)
		{
			Client c(instance->node_coord[client].x, instance->node_coord[client].y, client, c_demand);
			solution[i].route.push_back(c);
			++client;
			i = 0;
		}
	}
	return 1;
}

/*
	Generates a "random" solution for the CVRP problem

	if the seed is 0 = GEN_SEED, it generates its own

	- return 0:
		the function could not generate a correct solution
		so the caller must call it again

	- return 1
		the function succeeded
*/
int GenRandomSolution(ProblemInstance* instance, std::vector<Vehicle>& solution, int seed)
{
	int client = 0;

	int capacity = instance->capacity;
	int num_vehicles = instance->vehicles;
	int dimension = instance->dimension;
	if (!seed)
		srand(time(0));
	else
		srand(seed);

	solution.resize(num_vehicles);

	std::vector<bool> assigned;
	assigned.resize(dimension - 1);
	for (bool b : assigned)
		b = false;
	int assign_count = 0;

	for (int i = 0; i < num_vehicles; ++i)
	{
		Vehicle v;
		v.id = i;
		v.capacity = capacity;

		int cap = 0;
		while (cap <= capacity && assign_count < dimension - 1)
		{
			int client = rand() % (dimension - 1);
			// Avoid colision
			while (assigned[client] == true)
			{
				++client;
				if (client >= dimension - 1)
					client = 0;
			}

			int c_demand = instance->demand[client];
			if (c_demand + cap > capacity)
				break;

			assigned[client] = true;
			++assign_count;

			Client c(instance->node_coord[client].x, instance->node_coord[client].y, client, instance->demand[client]);
			v.route.push_back(c);
			cap += c.demand;
		}
		v.cargo = cap;
		solution[i] = v;
	}

	// Verify that every client is supplied
	for (int i = 0; assign_count < dimension - 1; ++i)
	{
		if (i >= num_vehicles)
			return 0;
		int c_demand = instance->demand[assign_count];
		if (c_demand + solution[i].cargo <= capacity)
		{
			Client c(instance->node_coord[assign_count].x, instance->node_coord[assign_count].y, assign_count, c_demand);
			solution[i].route.push_back(c);
			++assign_count;
			i = 0;
		}
	}
	return 1;
}

int GetNeighbor(Vehicle& v1, Vehicle& v2, int index1, int index2)
{
	int new_cargo_v1 = v1.cargo - v1.route[index1].demand + v2.route[index2].demand;
	int new_cargo_v2 = v2.cargo - v2.route[index2].demand + v1.route[index1].demand;
	
	if (new_cargo_v1 > v1.capacity || new_cargo_v2 > v2.capacity)
		return 0;

	return 1;
}

/*
	Calculate the cost(distance) of the Vehicle route
*/
float CalculateCost(Vehicle& v, vec2 origin)
{
	float distance = 0;

	vec2 curr_node(v.route[0].x, v.route[0].y);
	distance += origin.distance(curr_node);

	for (int i = 1; i < v.route.size(); ++i)
	{
		vec2 temp = vec2(v.route[i].x, v.route[i].y);
		distance += curr_node.distance(temp);
		curr_node = temp;
	}

	distance += origin.distance(curr_node);

	return distance;
}
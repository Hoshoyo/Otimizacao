#ifndef OTIM_CVRP_H
#define OTIM_CVRP_H

#include <vector>
#include "../include/utils.h"

struct ProblemInstance
{
	char name[64];
	char type[256];
	char comment[256];
	int dimension;
	int capacity;
	int vehicles;
	char edge_weight_type[64];
	char edge_weight_format[64];
	char node_coord_type[64];

	std::vector<vec2> node_coord;
	std::vector<int> demand;

	vec2 depot;
};


char fields[][32] = { "NAME", "TYPE", "COMMENT", "DIMENSION", "CAPACITY", "VEHICLES", "EDGE_WEIGHT_TYPE", "EDGE_WEIGHT_FORMAT",
"NODE_COORD_TYPE", "NODE_COORD_SECTION", "DEMAND_SECTION", "DEPOT_SECTION", "EOF" };
enum Fields
{
	NAME,
	TYPE,
	COMMENT,
	DIMENSION,
	CAPACITY,
	VEHICLES,
	EDGE_WEIGHT_TYPE,
	EDGE_WEIGHT_FORMAT,
	NODE_COORD_TYPE,
	NODE_COORD_SECTION,
	DEMAND_SECTION,
	DEPOT_SECTION,
	_EOF
};

struct Client
{
	float x, y;
	int id;
	int demand;

	Client()
		: x(0), y(0)
	{
	}
	Client(float x, float y)
		: x(x), y(y)
	{
	}

	Client(float x, float y, int id, int demand)
		: x(x), y(y), id(id), demand(demand)
	{
	}
};

struct Vehicle
{
	int id;
	int capacity;
	int cargo;
	std::vector<Client> route;
	float route_cost;
};

#endif	// OTIM_CVRP_H

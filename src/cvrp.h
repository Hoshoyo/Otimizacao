#ifndef OTIM_CVRP_H
#define OTIM_CVRP_H

#define NUM_E 2.71828182846f

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

	if (pi.vehicles == 0)
	{
		std::string s(pi.name);
		int k_index = s.find('k', 0);
		s = s.substr(k_index + 1, s.size());
		pi.vehicles = std::stoi(s, nullptr, 10);
	}

	return pi;
}

#endif	// OTIM_CVRP_H

#if defined(_WIN32)
#define copy_str(A, B) strcpy_s(A, B)
#else
#define copy_str(A, B) strcpy(A, B)
#endif
#include <iostream>
#include <vector>
#include "../include/utils.h"

using namespace std;

struct vec2
{
	float x, y;
	vec2()
	{
		x = 0.0f;
		y = 0.0f;
	}
	vec2(float x, float y)
		: x(x), y(y)
	{
	}
};

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

	vector<vec2> node_coord;
	vector<int> demand;

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
	return 0;
}
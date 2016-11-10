#if defined(_WIN32)
#define copy_str(A, B) strcpy_s(A, B)
#else
#define copy_str(A, B) strcpy(A, B)
#endif
#include <iostream>
#include <vector>
using namespace std;

typedef unsigned char u8;

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
	char type[64];
	float comment;
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

void skip(char** data, char c)
{
	while (**data != c)
		(*data)++;
	(*data)++;
}

inline void eat_ws(char** data)
{
	char c = **data;
	while (c == ' ' || c == '\n' || c == '\t' || c == '\r')
	{
		(*data)++;
		c = **data;
	}
}

ProblemInstance parse_data(char** data)
{
	ProblemInstance pi = {};
	skip(data, ':');
	eat_ws(data);
	copy_str(pi.name, *data);		// nome
	
	skip(data, ':');
	eat_ws(data);
	copy_str(pi.type, *data);		// type

	skip(data, ':');
	eat_ws(data);
	pi.comment = atof(*data);		// comment

	skip(data, ':');
	eat_ws(data);
	pi.dimension = atoi(*data);		// dimension

	skip(data, ':');
	eat_ws(data);
	pi.capacity = atoi(*data);		// capacity

	skip(data, ':');
	eat_ws(data);
	pi.vehicles = atoi(*data);		// comment

	skip(data, ':');
	eat_ws(data);
	copy_str(pi.edge_weight_type, *data);		// edge weight type

	skip(data, ':');
	eat_ws(data);
	copy_str(pi.edge_weight_format, *data);		// edge weight format

	skip(data, ':');
	eat_ws(data);
	copy_str(pi.node_coord_type, *data);		// node coord type

	skip(data, 0);
	if (strcmp(*data, "NODE_COORD_SECTION"))
	{
		cerr << "Formato do arquivo incorreto!\n";
		exit(EXIT_FAILURE);
	}
	skip(data, 0);
	eat_ws(data);

	while (strcmp(*data, "DEMAND_SECTION"))
	{
		skip(data, ' ');
		float xdata = atof(*data);
		skip(data, ' ');
		float ydata = atof(*data);
		pi.node_coord.push_back(vec2(xdata, ydata));
		skip(data, 0);
	}
	skip(data, 0);
	while (strcmp(*data, "DEPOT_SECTION"))
	{
		skip(data, ' ');
		int i = atoi(*data);
		pi.demand.push_back(i);
		skip(data, 0);
	}

	skip(data, 0);
	eat_ws(data);
	pi.depot.x = atof(*data);
	skip(data, ' ');
	eat_ws(data);
	pi.depot.y = atof(*data);

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
	parse_data(&data);

	delete[] ptr;
	return 0;
}
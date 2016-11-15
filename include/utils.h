#ifndef OTIM_UTILS_H
#define OTIM_UTILS_H

#if defined(_WIN32)
#define copy_str(A, B) strcpy_s(A, B)
#else
#define copy_str(A, B) strcpy(A, B)
#endif

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

	const vec2& operator-(const vec2& r)
	{
		return vec2(this->x - r.x, this->y - r.y);
	}
	const vec2& operator+(const vec2& r)
	{
		return vec2(this->x + r.x, this->y + r.y);
	}

	float distance(vec2& r)
	{
		vec2 v = *this - r;
		return sqrt(v.x * v.x + v.y * v.y);
	}
};

void skip(char** data, char c)
{
	char dt = **data;
	if (c == ' ')
	{
		while (dt != '\t' && dt != '\n' && dt != '\r' && dt != c && dt != 0)
		{
			(*data)++;
			dt = **data;
		}
	}
	else
	{
		while (dt != c && dt != 0)
		{
			(*data)++;
			dt = **data;
		}
	}

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

int cmp_first_word(char* word, char* comp)
{
	for (int i = 0; word[i] != ' ' && word[i] != ':' && word[i] != '\n' && comp[i] != 0; ++i)
	{
		if (word[i] == comp[i])
			continue;
		return 0;
	}
	return 1;
}

#endif	// OTIM_UTILS_H
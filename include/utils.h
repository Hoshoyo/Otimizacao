#ifndef OTIM_UTILS_H
#define OTIM_UTILS_H

typedef unsigned char u8;

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
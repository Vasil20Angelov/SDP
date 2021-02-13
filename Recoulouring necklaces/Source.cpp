#include <iostream>
#include <fstream>
using namespace std;

int n_size, m_rules;
ifstream file("file.txt");

/* 
* File's format:
* N M
* necklace1, necklace2 (N symbols each)
* C1[1] C2[1] T[1] 
* C1[2] C2[2] T[2]
* ... (M times, where C1 - recolour from; C2 - recolour to; T is time)
*/

/* Solution: 
* 1. The data is read from a text file and saved in 2 strings (each coloumn in the table) and 1 int array (the times)
* 2. Find how many different colours are used in the table with recolouring rules and save them in a new string
* 3. Create a matrix with size (number of different colours)^2 and put the time in the correct cells
* 4. Use Floyd-Warshall algorithm to find the best time for recolouring each element to each element
* 5. Find the time needed to recolour 2 different necklaces (if possible)
*/

// Reads from file the necklace's colours
bool Add_necklace(char* str)
{
	if (!str)
	{
		cout << "Not enough memory" << endl;
		delete[] str;
		return false;
	}

	for (int i = 0; i < n_size; ++i)
	{
		file >> str[i];
		if (str[i] < '!' || str[i] > '~')
		{
			cout << "Invalid colour found!" << endl;
			delete[] str;
			return false;
		}
	}

	str[n_size] = '\0';
	return true;
}

// Reads the recolouring rules and returns the number of different elements
int Different_colours(char* c1, char* c2, int* time) 
{
	int different_elements = 1;
	bool dif1 = true, dif2 = true;
	file >> c1[0] >> c2[0] >> time[0];
	if (c1[0] != c2[0])
		different_elements++;

	int i;
	for (i = 1; !file.eof() || i < m_rules; ++i)
	{
		file >> c1[i] >> c2[i] >> time[i];
		for (int j = 0; j < i; ++j)
		{
			if (dif1 && (c1[i] == c1[j] || c1[i] == c2[j]))
				dif1 = false;

			if (dif2 && (c2[i] == c1[j] || c2[i] == c2[j]) || c2[i] == c1[i])
				dif2 = false;

			if (!dif1 && !dif2)
				break;
		}
		if (dif1)
			++different_elements;
		if (dif2)
			++different_elements;

		dif1 = true;
		dif2 = true;
	}

	c1[m_rules] = '\0';
	c2[m_rules] = '\0';
	return different_elements;
}

// Creates an array containing all different colours from the table with recolouring rules
char* Colours(const char* c1, const char* c2, int elements) 
{
	char* colours = new (nothrow) char[elements + 1];
	if (!colours)
	{
		cout << "Not enough memory!" << endl;
		delete[] colours;
		return nullptr;
	}

	int k = 0;
	bool newel1 = true;
	bool newel2 = true;
	colours[k++] = c1[0];
	if (c1[0] != c2[0])
		colours[k++] = c2[0];

	for (int i = 1; i < m_rules; ++i)
	{
		for (int j = 0; j < k; ++j)
		{
			if (newel1 && c1[i] == colours[j])
				newel1 = false;

			if (newel2 && (c2[i] == colours[j] || c2[i] == c1[i]))
				newel2 = false;

			if (!newel1 && !newel2)
				break;
		}

		if (newel1)
			colours[k++] = c1[i];
		if (newel2)
			colours[k++] = c2[i];

		newel1 = true;
		newel2 = true;
	}

	colours[elements] = '\0';
	return colours;
}

// Creates a matrix with elements the times needed to recolour from c1 to c2
int** CreateMatrix(const char* c1, const char* c2, const int* time, const char* colours, int elements)
{
	int** matrix = new (nothrow) int* [elements];
	if (!matrix)
	{
		cout << "Not enough memory!" << endl;
		delete[] matrix;
		return nullptr;
	}
	for (int i = 0; i < elements; ++i)
	{
		matrix[i] = new int[elements];
		if (!matrix[i])
		{
			cout << "Not enough memory!" << endl;
			for (int j = 0; j < i; ++j)
				delete[] matrix[j];
			delete[] matrix;
			return nullptr;
		}
	}

	for (int i = 0; i < elements; ++i) // Sets default values
	{
		for (int j = 0; j < elements; ++j)
		{
			if (i == j)
				matrix[i][j] = 0;
			else
				matrix[i][j] = INT_MAX;
		}
	}

	int col = 0, row = 0;
	for (int i = 0; i < m_rules; ++i) // Fills the matrix with the times needed to recolour 2 elements
	{
		for (row = 0; row < elements; ++row)
		{
			if (colours[row] == c1[i])
				break;
		}
		for (col = 0; col < elements; ++col)
		{
			if (colours[col] == c2[i])
				break;
		}

		matrix[row][col] = time[i];
	}

	return matrix;
}

// Finds the shortes time to recolur each element to each element
void FindShortestTime(int** matrix, int elements, const char* colours)
{
	// Using Floyd - Warshall algorithm
	for (int k = 0; k < elements; k++)
	{
		for (int i = 0; i < elements; i++)
		{
			for (int j = 0; j < elements; j++)
			{
				if (matrix[i][k] < matrix[i][j] -  matrix[k][j] )
					matrix[i][j] = matrix[i][k] + matrix[k][j];
			}
		}
	}
}

// Recolours the necklaces and returns the time needed for it
int FixNecklaces(const char* necklace1, const char* necklace2, const char* colours, int** matrix, int elements)
{
	char* new_necklace = new (nothrow) char[n_size + 1];
	if (!new_necklace)
	{
		cout << "Not enough memory!" << endl;
		delete[] new_necklace;
		return -1;
	}

	int fix_time = 0, row1 = -1, row2 = -1, count = 0, letter, min = INT_MAX;
	bool match = false;
	for (int i = 0; i < n_size; ++i, row1 = -1, row2 = -1)
	{
		if (necklace1[i] != necklace2[i])
		{
			for (int k = 0; k < elements; ++k)
			{
				if (colours[k] == necklace1[i])
					row1 = k;
				if (colours[k] == necklace2[i])
					row2 = k;
			}

			if (row1 == -1 || row2 == -1)
			{
				cout << "Not possible!" << endl;
				delete[] new_necklace;
				return -1;
			}

			for (int l = 0; l < elements; ++l)
			{
				if (matrix[row1][l] < INT_MAX && matrix[row2][l] < INT_MAX)
				{
					if (min > matrix[row1][l] + matrix[row2][l])
					{
						min = matrix[row1][l] + matrix[row2][l];
						letter = l;
					}
				}
			}

			if (min == INT_MAX)
			{
				cout << "Not possible" << endl;
				delete[] new_necklace;
				return -1;
			}

			fix_time += min;
			new_necklace[count++] = colours[letter];
		}
		else
			new_necklace[count++] = necklace1[i];

		min = INT_MAX;
	}

	new_necklace[n_size] = '\0';
	cout << "The necklaces have been recoloured to:  ";
	for (int i = 0; i < count; ++i)
		cout << new_necklace[i];
	cout << "\nTime:  ";

	delete[] new_necklace;

	return fix_time;
}

// Prints the matrix on the screen
void printMatrix(int** matrix, const char* colours, int elements) 
{
	cout << endl << "  ";
	for (int i = 0; i < elements; ++i)
		cout << colours[i] << " ";
	cout << endl;

	for (int i = 0; i < elements; i++)
	{
		cout << colours[i] << " ";
		for (int j = 0; j < elements; j++)
		{
			if (matrix[i][j] == INT_MAX)
				cout << "- ";
			else
				cout << matrix[i][j] << " ";
		}
		cout << endl;
	}

	cout << endl;
}

// Clears memory
void clear_memory(char*& necklace1, char*& necklace2, char*& c1, char*& c2, char*& colours, int*& time, int**& matrix, int elements)
{
	delete[] necklace1;
	delete[] necklace2;
	delete[] c1;
	delete[] c2;
	delete[] colours;
	delete[] time;
	for (int i = 0; i < elements; ++i)
		delete[] matrix[i];
	delete[] matrix;
}

int main()
{
	// Detects if the memory is leaking (it isn't in the case)
	// _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	if (!file.is_open())
	{
		cout << "Couldn't open the text file" << endl;
		return -1;
	}
	file >> n_size >> m_rules;
	char* necklace1 = new (nothrow) char[n_size + 1];
	if (!Add_necklace(necklace1))
	{
		file.close();
		return -1;
	}
	char* necklace2 = new (nothrow) char[n_size + 1];
	if (!Add_necklace(necklace2))
	{
		delete[] necklace1;
		file.close();
		return -1;
	}
	
	if (!strcmp(necklace1, necklace2))
	{
		cout << "Same necklaces! No need to be recoloured!" << endl;
		delete[] necklace1, necklace2;
		file.close();
		return 0;
	}

	int* time = new (nothrow) int[m_rules];
	char* c1 = new (nothrow) char[m_rules + 1];
	char* c2 = new (nothrow) char[m_rules + 1];
	if (!time || !c1 || !c2)
	{
		cout << "Not enough memory!" << endl;
		delete[] time, c1, c2, necklace1, necklace2;
		file.close();
		return -1;
	}

	int different_elements = Different_colours(c1, c2, time);
	char* colours = Colours(c1, c2, different_elements);
	file.close();

	int** matrix = CreateMatrix(c1, c2, time, colours, different_elements);
	if (!matrix)
	{
		delete[] necklace1, necklace2, time, colours, c1, c2;
		return -1;
	}

	cout << "Matrix before using Floyd-Warshall algorithm:";
	printMatrix(matrix, colours, different_elements);
	FindShortestTime(matrix, different_elements, colours);
	cout << "Floyd-Warshall algorithm:";
	printMatrix(matrix, colours, different_elements);

	cout << FixNecklaces(necklace1, necklace2, colours, matrix, different_elements) << endl;

	clear_memory(necklace1, necklace2, c1, c2, colours, time, matrix, different_elements);

	return 0;
}

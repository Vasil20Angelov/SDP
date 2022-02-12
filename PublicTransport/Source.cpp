#include <iostream>
#include <fstream>
#include "PublicTransport.h"
using namespace std;
int main()
{
	ifstream f("Map.txt");	
	PublicTransport pTransport(f);
	f.close();

	cout << endl << endl;
	pTransport.menu();

	return 0;
}
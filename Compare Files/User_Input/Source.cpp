#include <iostream>
#include <fstream>
#include <exception>
#include "../HomeWork3/HashMap.h"
#include "../HomeWork3/interface.h"

float calcPercent(int common, int total)
{
	if (total == 0)
		return 100;

	return (static_cast<float>(common) / static_cast<float>(total) * 100.0);
}

int main(const int argc, const char* argv[])
{

	if (argc != 3)
	{
		throw std::invalid_argument("Invalid arguments count!");
		return -1;
	}

	std::ifstream a(argv[1]);
	if (!a.is_open())
	{
		throw new std::exception("Error openning file 1");
		return -1;
	}

	std::ifstream b(argv[2]);
	if (!b.is_open())
	{
		a.close();
		throw new std::exception("Error openning file 2");
		return -1;
	}

	Comparator c;
	try {
		c.init(a, b);
	}
	catch (std::exception& e) {
		std::cout << e.what();
		a.close();
		b.close();
		return -1;
	}

	std::cout << "File 1 contains " << c.WordsCountInStream1() << " words and " << c.CommonWordsCount()
		<< " of them are also in file2 (" << floor(calcPercent(c.CommonWordsCount(), c.WordsCountInStream1())) << "%)\n";

	std::cout << "File 2 contains " << c.WordsCountInStream2() << " words and " << c.CommonWordsCount()
		<< " of them are also in file1 (" << floor(calcPercent(c.CommonWordsCount(), c.WordsCountInStream2())) << "%)\n";

	a.close();
	b.close();

	return 0;
}
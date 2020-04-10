#include <iostream>
#include <fstream>
#include "dijkstra_algorithm.h"

int main()
{
	std::ifstream file("info.txt");

	List<std::string> lines;

	while(file)
	{
		std::string str;
		getline(file, str);

		lines.push_back(str);
	}
	lines.pop_back();
	
	List<std::string*> normalized_info = convert(lines);

	List<std::string> namesList = get_unique_names(normalized_info);

	Graph G(namesList);

	G.createFrom(normalized_info);

	std::string start, end;
	cout << "City 1: ";
	cin >> start;
	cout << "\nCity 2: ";
	cin >> end;
	cout << endl << endl;
	cout << G.dijkstra(start, end);
	cout << endl << endl;

	for (size_t i = 0; i < normalized_info.getSize(); ++i)
	{
		delete[] normalized_info.at(i);
	}
	
	return 0;
}

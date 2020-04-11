#pragma once
#include "Queue.h"
#include "List.h"
#include "Map.h"
#include "priority_queue.h"
#include <string>
#include <stdexcept>

constexpr uint8_t CITY_FROM = 0;
constexpr uint8_t CITY_TO = 1;
constexpr uint8_t COST_FORWARD = 2;
constexpr uint8_t COST_BACKWARD = 3;
constexpr uint8_t DATA_INPUT_WORDS_AMOUNT = 4;


template<class T>
List<T> reverse(List<T> lst);


class Graph
{
private:
	Map<uint16_t, uint64_t>* adjList; //adjacency list that stores vertices number and edge weight
	List<std::string> namesList; //represents link between city name and vertices number
	priority_queue<uint16_t> markList; //stores vertices number and its mark
	size_t size;

	//creates from start point to the destination point using marked graph
	List<std::string> buildPath(priority_queue<uint16_t>& visitedMarksList, const uint16_t& start, const uint16_t& destination)
	{
		List<std::string> path;

		//checks if the straight route from start to destination is the cheapest
		if (adjList[start].contains(destination))
		{
			if (adjList[start].find(destination) == visitedMarksList.get_priority(destination))
			{
				path.push_front(namesList.at(destination));
				path.push_front(namesList.at(start));
				return path;
			}
			
		}

		//checks if destination point is reachable from the start
		if (visitedMarksList.get_priority(destination) == UINT64_MAX)
		{
			path.push_front("There is no route between " + namesList.at(start) + " and " + namesList.at(destination));
			return path;
		}

		path.push_front(namesList.at(destination));
		auto current = destination;
		auto cur_mark = visitedMarksList.get_priority(destination);

		//the algorithm makes path from the destination point to the start
		while(current != start)
		{
			std::string city;

			//getting all current's neighbors
			auto neighbors_list = adjList[current].get_keys();
			uint16_t neighbor;

			//checks every neighbor's mark if (current's mark - edge weight) equals it
			for(size_t i = 0; i < neighbors_list.getSize(); ++i)
			{
				neighbor = neighbors_list.at(i);

				if(cur_mark - adjList[neighbor].find(current) == visitedMarksList.get_priority(neighbor))
				{
					city = namesList.at(neighbor);
					break;
				}
			}

			path.push_front(city);
			current = neighbor;
			cur_mark = visitedMarksList.get_priority(neighbor);
		}

		return path;
	}
	
public:

	//default graph initializing constructor
	explicit Graph(List<std::string>& namesList) 
		: size(namesList.getSize())
	{
		this->namesList = namesList;
		adjList = new Map<uint16_t, uint64_t>[size];

		for (size_t i = 0; i < size; ++i)
			markList.insert(i, UINT64_MAX);
	}

	
	~Graph()
	{
		delete[] adjList;
	}

	//creates graph from list of words arrays
	void createFrom(List<std::string*>& info) const
	{	
		for (size_t i= 0; i < info.getSize(); ++i)
		{
			const auto line = info.at(i);

			uint16_t city_from_number = namesList.find(line[CITY_FROM]);
			uint16_t city_to_number = namesList.find(line[CITY_TO]);
			uint64_t cost_forward = std::stoull(line[COST_FORWARD]);
			uint64_t cost_backward = std::stoull(line[COST_BACKWARD]);

			//filling the adjacency list from input info
			for(auto j = 1; j <= 2; ++j)
			{

				adjList[city_from_number].insert(city_to_number, cost_forward);

				std::swap(city_from_number, city_to_number);
				std::swap(cost_forward, cost_backward);
			}

		}
	}

	//finding the cheapest/shortest way with dijkstra algorithm 
	std::string dijkstra(const std::string& start_point,const std::string& destination_point) 
	{
		if (start_point == destination_point)
			return "Start and destination points must be different!";

		if (!namesList.contains(start_point) || !namesList.contains(destination_point))
			return "Irrelevant start or end city name!";

		priority_queue<uint16_t> visitedMarksList; //stores visited vertices with their final marks
		const uint16_t start = namesList.find(start_point); //beginning of the required path
		const uint16_t destination = namesList.find(destination_point); //end of the required path

		markList.update(start, 0);

		while (markList.getSize())
		{
			const auto current_mark = markList.get_priority();
			const auto current = markList.extract_min(); // vertices that is currently being processed			
			const auto neighbors_list = adjList[current].get_keys(); //getting all current's neighbors

			for (size_t j = 0; j < neighbors_list.getSize(); ++j)
			{
				const auto neighbor = neighbors_list.at(j); //getting current being processed neighbor
				const auto visited = !markList.contains(neighbor); //find out if this neighbor has been already visited
				const auto edge_weight = adjList[current].find(neighbor);
				
				if(!visited && edge_weight !=0 && current_mark != UINT64_MAX)
				{
					const auto mark = current_mark + edge_weight;
					
					if (markList.get_priority(neighbor) > mark)
						markList.update(neighbor, mark);
				}

			}
			
			visitedMarksList.insert(current, current_mark);
		}

		//getting route from marked graph
		const auto way = buildPath(visitedMarksList, start, destination);
		std::string result;
		
		if (way.getSize() > 1)
		{
			result = "The cheapest route: ";

			for (size_t i = 0; i < way.getSize() - 1; ++i)
				result += way.at(i) + " -> ";

			result += way.at(way.getSize() - 1) + "\nTotal cost: " + std::to_string(visitedMarksList.get_priority(destination));
		}
		else
			result = way.at(0);
		
		return result;
			
	}
};

//converts input list of lines into list of words arrays
inline List<std::string*> convert(List<std::string>& info)
{
	if (info.getSize() == 0)
		throw std::length_error("Input was empty!");
	
	List<std::string*> separated_info;

	for (size_t i = 0; i < info.getSize(); ++i)
	{
		const auto line = new std::string[DATA_INPUT_WORDS_AMOUNT];
		auto word_number = 0;

		for (auto ch : info.at(i))
		{
			if (ch == ';')
				word_number++;
			else
				line[word_number] += ch;
		}

		if (word_number > DATA_INPUT_WORDS_AMOUNT)
			throw std::length_error("Irrelevant input format!");

		if (line[COST_FORWARD] == "N/A")
			line[COST_FORWARD] = "0";
		
		if (line[COST_BACKWARD] == "N/A")
			line[COST_BACKWARD] = "0";

		if (line[COST_FORWARD] == "0" && line[COST_BACKWARD] == "0")
			throw std::logic_error("Flight was unavailable in both directions!");
		
		separated_info.push_back(line);
	}

	return separated_info;
}

//returns list of all cities names (without repeats)
inline List<std::string> get_unique_names(List<std::string*>& info)
{
	List<std::string> namesList;

	for (size_t i = 0; i < info.getSize(); ++i)
	{
		if (!namesList.contains(info.at(i)[CITY_FROM]))
			namesList.push_back(info.at(i)[CITY_FROM]);

		if (!namesList.contains(info.at(i)[CITY_TO]))
			namesList.push_back(info.at(i)[CITY_TO]);
	}
	
	return namesList;
}

template<class T>
List<T> reverse(List<T> lst)
{
	List<T> reversed;

	for (size_t i = 1; i <= lst.getSize(); ++i)
		reversed.push_back(lst.at(lst.getSize() - i));

	return reversed;
}

/****************************************************************************
** Program name: greedyTSP.cpp
** Class name: CS325-400
** Author: Ben Fridkis
** Date: 5/22/2018
** Description: Program using a greedy method to determine an approximate
**				solution (normally within ~15-20% of Held- Karp lower bound,
**				https://web.tuke.sk/fei-cit/butka/hop/htsp.pdf).
*****************************************************************************/

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <queue>
#include <algorithm>
#include <tuple>
using std::vector;
using std::string;
using std::priority_queue;
using std::ifstream;
using std::ofstream;
using std::istringstream;
using std::ios;
using std::string;
using std::cout;
using std::getline;
using std::endl;
using std::find;
using std::tuple;
using std::get;

//Structure to represent an edge. Each CityDistance structure
//holds the distance to a particular city.
struct CityDistance{
	int city;
	int distanceToCity;
	CityDistance(int c, int dTC)
	{
		city = c;
		distanceToCity = dTC;
	}
};

//Used to modify the stl priority_heap from max heap to min heap.
//Reference: https://www.geeksforgeeks.org/implement-min-heap-using-stl/
class myComparator
{
public:
    int operator() (const CityDistance& c1, const CityDistance& c2)
    {
        return c1.distanceToCity > c2.distanceToCity;
    }
};

typedef priority_queue<CityDistance, vector<CityDistance>, myComparator> CityDistancePQ;

/**************************************************************************************
**                          loadGraphOfMap                                           **
** This function creates the map representation in memory. It returns a vector       **
** of priority queues (min heaps) which each have CityDistance structs (or edges).   **
** The first element (priority queue) of the returned vector holds the edges         **
** (as CityDistance structs) for city 0, the second element city 1, and so forth.    **
** The priority queues (min heaps) of CityDistances for each city maintain heap      **
** order based on closest city (i.e. the closest city is at the top or 'root' of the **
** heap).                                                                            **
**************************************************************************************/
vector<CityDistancePQ> loadGraphOfMap(char* dataInputFileName)
{
	ifstream inputData1, inputData2;
	inputData1.exceptions (ifstream::failbit | ifstream::badbit);
	if(dataInputFileName == nullptr){
        cout << "\nMust enter file name when running program." << endl
             << "Type './greedyTSP file.txt' in command line," << endl
             << "replacing 'file.txt' with the name of your file.\n" << endl;
        exit(1);
    }
    try{
        inputData1.open(dataInputFileName);
    }
    catch(ifstream::failure e){
        std::cerr << "\nFile cannot be found or opened.\n" << endl;
        exit(1);
    }
    inputData2.open(dataInputFileName);
	vector<CityDistancePQ> graph;
	string line;

	//For every vertex, the distances to all other vertices are calculated and stored (in min heap/priority queue)
	//since the TSP problem graph is complete (i.e. any city can be accessed from any other).
	int i = 0;
	do
	{
	    getline(inputData1, line);
        int startingCity, startingCityX, startingCityY;
        istringstream iss(line);
        iss >> startingCity >> startingCityX >> startingCityY;
        CityDistancePQ pq;
        while(getline(inputData2, line))
        {
            int city, cityX, cityY;
            istringstream iss(line);
            iss >> city >> cityX >> cityY;
            int distanceToCity =
                static_cast<int>(round(sqrt(pow(static_cast<double>(cityX) - static_cast<double>(startingCityX), 2) +
                                             pow(static_cast<double>(cityY) - static_cast<double>(startingCityY), 2))));
            pq.push(CityDistance(city, distanceToCity));
        }
        graph.push_back(pq);
        cout << "city adjacency list for city " << i << " added to graph." << endl;    //***************************************
        inputData2.clear();
        inputData2.seekg(0, ios::beg);
        i++;
	} while(i < graph[0].size());
	inputData1.close();
	inputData2.close();

	return graph;
}

//Used for testing only.
void printLoaded(vector<CityDistancePQ>& v)
{
	for(unsigned i = 0; i < v.size(); i++)
	{
		while(!v[i].empty())
		{
			cout << v[i].top().city << " " << v[i].top().distanceToCity << endl;
			v[i].pop();
		}
	}
}

/**************************************************************************************
**                                 loadTour                                          **
** This function returns a tuple with the total tour distance('<0>' of tuple) and a  **
** vector of cities ('<1>' of tuple) established in the order the cities are to be   **
** visited on the tour.                                                              **
**************************************************************************************/
tuple<int, vector<int>> loadTour(vector<CityDistancePQ>& graph)
{
    tuple<int, vector<int>> tspTour;
    int distance = 0;
    vector<int> tspTourCities;
    unsigned i, j;
    for(i = 0, j = 0; j < graph.size(); j++)
    {
        while(find(tspTourCities.begin(), tspTourCities.end(),
                   graph[i].top().city) != tspTourCities.end())
        {
            graph[i].pop();
        }

        tspTourCities.push_back(graph[i].top().city);
        cout << "city " << i << " added to tour." << endl;    //***************************************
        distance += graph[i].top().distanceToCity;
        i = graph[i].top().city;
    }

    //This loop is needed to find the distance from the last city of the tour
    //back to the home city. (Variable i is assigned last city of tour when previous
    //for loop exits.
    while(graph[0].top().city != i)
    {
        graph[0].pop();
    }
    get<0>(tspTour) = distance + graph[0].top().distanceToCity;     //Adds distance to last city before assigning to the tuple.
    get<1>(tspTour) = tspTourCities;

    return tspTour;

}

int main(int argc, char *argv[])
{
	vector<CityDistancePQ> graph = loadGraphOfMap(argv[1]);
	//printLoaded(graph);  -- Used only for testing
	tuple<int, vector<int>> tspTour = loadTour(graph);
	ofstream dataOut;
	string inputFileName = argv[1];
	dataOut.open(inputFileName + ".tour");
	dataOut << get<0>(tspTour) << "\n";
	for(unsigned i = 0; i < get<1>(tspTour).size(); i++)
    {
        dataOut << get<1>(tspTour)[i] << "\n";
    }
	
	return 0;
}

/******************************************************************************
** Program name: greedyTSP_w2Opt.cpp
** Class name: CS325-400
** Author: Ben Fridkis
** Date: 6/4/2018
** Description: Program using a greedy method to determine a solution
**				to the traveling salesman problem.
**				(Usually results in tour < 5% above Held- Karp bound,
**				https://web.tuke.sk/fei-cit/butka/hop/htsp.pdf).)
*******************************************************************************/

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <queue>
#include <algorithm>
#include <tuple>
#include <ctime>
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
using std::make_tuple;
using std::get;

//Structure to represent an edge. Each CityDistance structure
//holds the distance to a particular city.
struct CityDistance{
	int city;
	int nextCity;
	int distanceToCity;
	CityDistance(){};
	CityDistance(int c, int nc, int dTC)
	{
		city = c;
		nextCity = nc;
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
**                          loadGraphOfMapAsPriorityQueue                            **
** This function creates the map representation in memory. It returns a min-heap 	 **
** holding all edges of the graph, with the edge that has the minimum distance as	 **
** the "root" of the heap.                                                           **
**************************************************************************************/
CityDistancePQ loadGraphOfMapAsPriorityQueue(char* dataInputFileName)
{
	ifstream inputData1, inputData2;
	if(dataInputFileName == nullptr){
        cout << "\nMust enter file name when running program." << endl
             << "Type './greedyTSP file.txt' in command line," << endl
             << "replacing 'file.txt' with the name of your file.\n" << endl;
        exit(1);
    }
    try{
        inputData1.open(dataInputFileName);
        inputData1.exceptions (ifstream::failbit | ifstream::badbit);
    }
    catch(ifstream::failure e){
        std::cerr << "\nFile cannot be found or opened.\n" << endl;
        exit(1);
    }
    inputData2.open(dataInputFileName);
	CityDistancePQ graph;
	string line;

	//For every vertex, the distances to all other vertices are calculated and stored (in min heap/priority queue)
	//since the TSP problem graph is complete (i.e. any city can be accessed from any other).
	int i = 0;
	bool cityCountEstablished = false;
	int cityCount = 0;
	do
	{
	    getline(inputData1, line);
        int startingCity, startingCityX, startingCityY;
        istringstream iss(line);
        iss >> startingCity >> startingCityX >> startingCityY;
		while(getline(inputData2, line))
        {
            int city, cityX, cityY;
            istringstream iss(line);
            iss >> city >> cityX >> cityY;
            int distanceToCity =
                static_cast<int>(round(sqrt(pow(static_cast<double>(cityX) - static_cast<double>(startingCityX), 2) +
                                             pow(static_cast<double>(cityY) - static_cast<double>(startingCityY), 2))));
            graph.push(CityDistance(startingCity, city, distanceToCity));
			if(!cityCountEstablished)
			{
				cityCount++;
			}
        }
		cityCountEstablished = true;
        inputData2.clear();
        inputData2.seekg(0, ios::beg);
        i++;
	} while(i < cityCount);
	inputData1.close();
	inputData2.close();

	return graph;
}

/**************************************************************************************
**                          loadGraphOfMapAsVectors                                  **
** This function creates the map representation in memory. It returns a vector of    **
** vectors which each have CityDistance structs (or edges). The first element        **
** (vector) of the returned vector holds the edges (as CityDistance structs) for     **
** city 0, the second element city 1, and so forth. The vectors of CityDistances for **
** each city maintain sequential order of distances to all other cities.             **
** (i.e. graph[1][0] represents the distance from city 1 to city 0, and so forth).   **
** This graph representation is used specifically for the 2-Opt tour improvement.    **
**************************************************************************************/
vector<vector<int>> loadGraphOfMapAsVectors(char* dataInputFileName)
{
	ifstream inputData1, inputData2;
	if(dataInputFileName == nullptr){
        cout << "\nMust enter file name when running program." << endl
             << "Type './greedyTSP file.txt' in command line," << endl
             << "replacing 'file.txt' with the name of your file.\n" << endl;
        exit(1);
    }
    try{
        inputData1.open(dataInputFileName);
        inputData1.exceptions (ifstream::failbit | ifstream::badbit);
    }
    catch(ifstream::failure e){
        std::cerr << "\nFile cannot be found or opened.\n" << endl;
        exit(1);
    }
    inputData2.open(dataInputFileName);
	vector<vector<int>> graph;
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
        vector<int> v;
        while(getline(inputData2, line))
        {
            int city, cityX, cityY;
            istringstream iss(line);
            iss >> city >> cityX >> cityY;
            int distanceToCity =
                static_cast<int>(round(sqrt(pow(static_cast<double>(cityX) - static_cast<double>(startingCityX), 2) +
                                             pow(static_cast<double>(cityY) - static_cast<double>(startingCityY), 2))));
            v.push_back(distanceToCity);
        }
        graph.push_back(v);
        inputData2.clear();
        inputData2.seekg(0, ios::beg);
        i++;
	} while(i < static_cast<int>(graph[0].size()));
	inputData1.close();
	inputData2.close();

	return graph;
}

//Used for testing only.
void printLoaded(vector<CityDistancePQ>& v)
{
	for(int i = 0; i < static_cast<int>(v.size()); i++)
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
tuple<int, vector<int>> loadTour(CityDistancePQ& graph)
{
	int cityCount = sqrt(graph.size());
    tuple<int, vector<int>> tspTour;

	//The vector of tuples below (cityTourPositionTracker)
	//stores for each city a boolean to indicate if the city
	//has been added to the tour, an integer to indicate
	//the city's forward adjacent city (or "next city"),
	//and an integer to indicate the city's backward adjacent
	//city (or "previous city). A value of -1 for the next city
	//item (2nd tuple item) or previous city item (3rd tuple item)
	//indicates that a city has not been assigned a next city or
	//previous city, respectively.
	vector<tuple<bool, int, int>> cityTourPositionTracker;
	for(int i = 0; i < cityCount; i++)
	{
		cityTourPositionTracker.push_back(make_tuple(false, -1, -1));
	}

	int distance = 0;
    for(int i = 0; i < cityCount; i++)
    {
		bool edgeAdded = false;
		while(!edgeAdded)
		{
			//The while loop below removes any ineligible edge (except for those
            //that result in a cycle (which are handled in lines 253-266). Ineligible
			//edges removed here are those with cities (vertices) that already have
            //two adjacent cities (vertices) and self-referential edges.
			while(get<0>(cityTourPositionTracker[graph.top().city]) == true ||
				  get<2>(cityTourPositionTracker[graph.top().nextCity]) != -1 ||
                  graph.top().city == graph.top().nextCity)
			{
				graph.pop();
			}
			//Lines 253-266 check to make sure the current edge
			//will not create a cycle (except when adding the final
			//edge) when added to the tour. If adding the edge will
			//create a cycle, it is discarded and the compatibility
			//checks restart at line 242.
			int downstreamCity = graph.top().nextCity;
			bool edgeCreatesCycle = false;

            //(Note the last edge added is allowed to make the cycle, hence the condition
            //"i < cityCount - 1" in the while loop below.)
			while(get<0>(cityTourPositionTracker[downstreamCity]) == true &&
					get<1>(cityTourPositionTracker[downstreamCity]) != -1 && i < cityCount - 1)
			{
				downstreamCity = get<1>(cityTourPositionTracker[downstreamCity]);
				if(downstreamCity == graph.top().city)
				{
					graph.pop();
					edgeCreatesCycle = true;
					break;
				}
			}

			if(!edgeCreatesCycle)
			{
				get<0>(cityTourPositionTracker[graph.top().city]) = true;
				get<1>(cityTourPositionTracker[graph.top().city]) = graph.top().nextCity;
				get<2>(cityTourPositionTracker[graph.top().nextCity]) = graph.top().city;
				distance += graph.top().distanceToCity;
				graph.pop();
				edgeAdded = true;
			}
		}
	}
	for(int i = 0, j = 0; i < cityCount; i++)
	{
		get<1>(tspTour).push_back(j);
		j = get<1>(cityTourPositionTracker[j]);
	}
	get<0>(tspTour) = distance;

	return tspTour;

}

/****************************************************************************
**                             twoOptImprove                               **
** This function receives a tour tuple (tsp solution) and a vector graph   **
** (see 'loadMapAsGraphOfVectors' above), and attempts to restructure the  **
** the tour by 'swapping' eligible pairs of edges, if said swap reduces    **
** the total tour distance. This is in attempt to eliminate path cross-    **
** over that contributes to sub-optimality. When a swap occurs, the path   **
** in between the vertices swapped is reversed,to maintain the tour        **
** integrity. See https://en.wikipedia.org/wiki/2-opt and                  **
** http://pedrohfsd.com/2017/08/09/2opt-part1.html for more details.       **
****************************************************************************/
void twoOptImprove(tuple<int, vector<int>> &tspTour,
                   vector<vector<int>> &graph)
{
	//This variable (breakOutToOptimize) is set to allow the loop to repeat
	//until the optimal improvement is obtained for small data sizes (n <= 2500).
	//In this case, execution exits both the inner and outer loops when each swap
	//is made, and the process repeats until no further improvement is possible.
	//For large data sets, the improvement runs only once (all the way through) to
	//ensure a reasonable running time (at the expense of optimality).
	//(See related lines 321-324 and note the control statement
	//`breakOutToOptimize == false` in both the inner and outer for loops.)
	bool breakOutToOptimize;
	bool nExceeds2500;
	bool improved;
	if(get<1>(tspTour).size() > 2500)
	{
		nExceeds2500 = true;
	}
	else
	{
		nExceeds2500 = false;
	}

    do
    {
		improved = false;
		breakOutToOptimize = false;
		//(Can't swap 1st city so i starts at 1...)
        for(int i = 1; i < static_cast<int>(get<1>(tspTour).size()) - 2 &&
				breakOutToOptimize == false; i++)
        {
            for(int j = i, k = i + 1; k < static_cast<int>(get<1>(tspTour).size()) &&
					breakOutToOptimize == false; k++)
            {
				//Adjacent vertices are not eligible for consideration
				//because there is only one edge between them.
				if(k - j == 1)
				{
					continue;
				}

				//If distance(i to i + j -1) + distance(i + 1, j) <
				//distance(i to i + 1) + distance(j - 1 to j), the swap
				//will improve the tour. In other words, if taking out the two
				//edges before the swap and inserting two new edges (because of swap)
				//results in shorter tour, the cities are swapped in tour order.
				if(graph[get<1>(tspTour)[j]][get<1>(tspTour)[k - 1]] +
				   graph[get<1>(tspTour)[j + 1]][get<1>(tspTour)[k]] <
				   graph[get<1>(tspTour)[j]][get<1>(tspTour)[j + 1]] +
				   graph[get<1>(tspTour)[k - 1]][get<1>(tspTour)[k]])
				{

					//Update tour distance based on swapped edges.
					get<0>(tspTour) -=  (graph[get<1>(tspTour)[j]][get<1>(tspTour)[j + 1]] +
										 graph[get<1>(tspTour)[k - 1]][get<1>(tspTour)[k]]) -
										(graph[get<1>(tspTour)[j]][get<1>(tspTour)[k - 1]] +
										 graph[get<1>(tspTour)[j + 1]][get<1>(tspTour)[k]]);

					improved = true;
					//Only need to reverse cities in between swapped routes (edges).
					for(int l = j + 1, m = k - 1; l < m; l++, m--)
					{
						int temp = get<1>(tspTour)[l];
						get<1>(tspTour)[l] = get<1>(tspTour)[m];
						get<1>(tspTour)[m] = temp;
					}
					//Allow optimization of improvement if data size is manageable.
					//(Otherwise, additional time cost is unreasonable, run 2Opt-swap once over only.)
					//If enabled (i.e. data set <= 1000), execution exits both inner and outer
					//loop after improvement to start over.
					if(!nExceeds2500)
					{
						breakOutToOptimize = true;
					}
				}
            }
        }
    }while(improved);
}

int main(int argc, char *argv[])
{
	clock_t begin = clock();
	CityDistancePQ graph1 = loadGraphOfMapAsPriorityQueue(argv[1]);
	//printLoaded(graph);  -- Used only for testing
	tuple<int, vector<int>> tspTour = loadTour(graph1);

	//Effectively deallocates memory used for graph 1 once no longer needed.
	//See https://stackoverflow.com/questions/10464992/c-delete-vector-objects-free-memory
	CityDistancePQ().swap(graph1);

	vector<vector<int>> graph2 = loadGraphOfMapAsVectors(argv[1]);
	twoOptImprove(tspTour, graph2);
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	cout << "\nRunning Time: " << elapsed_secs << "\n" << endl;

	ofstream dataOut;
	string inputFileName = argv[1];
	dataOut.open(inputFileName + ".tour");
	dataOut << get<0>(tspTour) << "\n";
	for(int i = 0; i < static_cast<int>(get<1>(tspTour).size()); i++)
    {
        dataOut << get<1>(tspTour)[i] << "\n";
    }
}

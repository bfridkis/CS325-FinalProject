/******************************************************************************
** Program name: greedyTSP_w2Opt.cpp
** Class name: CS325-400
** Author: Ben Fridkis
** Date: 5/22/2018
** Description: Program using a greedy method to determine an approximate
**				solution (usually results in tour < 5% above Held- Karp bound,
**				https://web.tuke.sk/fei-cit/butka/hop/htsp.pdf).
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
using std::get;

//Structure to represent an edge. Each CityDistance structure
//hold the distance to a particular city.
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
**                          loadGraphOfMapAsMinHeaps                                 **
** This function creates the map representation in memory. It returns a vector       **
** of priority queues (min heaps) which each have CityDistance structs (or edges).   **
** The first element (priority queue) of the returned vector holds the edges         **
** (as CityDistance structs) for city 0, the second element city 1, and so forth.    **
** The priority queues (min heaps) of CityDistances for each city maintain heap      **
** order based on closest city (i.e. the closest city is at the top or 'root' of the **
** heap.                                                                             **
**************************************************************************************/
vector<CityDistancePQ> loadGraphOfMapAsMinHeaps(char* dataInputFileName)
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
        inputData2.clear();
        inputData2.seekg(0, ios::beg);
        i++;
	} while(i < static_cast<int>(graph[0].size()));
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
tuple<int, vector<int>> loadTour(vector<CityDistancePQ>& graph)
{
    tuple<int, vector<int>> tspTour;
    int distance = 0;
    vector<int> tspTourCities;
    int i, j;
    for(i = 0, j = 0; j < static_cast<int>(graph.size()); j++)
    {
		//Remove each subsequent closest city if it is already
		//added to the tour.
        while(find(tspTourCities.begin(), tspTourCities.end(),
                   graph[i].top().city) != tspTourCities.end())
        {
            graph[i].pop();
        }
		//Add closest city that is not already in tour, and
		//add associated distance to overall tour distance.
        tspTourCities.push_back(graph[i].top().city);
        distance += graph[i].top().distanceToCity;
        //Set i to city added
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
				//   distance(i to i + 1) + distance(j - 1 to j), the swap
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
	vector<CityDistancePQ> graph1 = loadGraphOfMapAsMinHeaps(argv[1]);
	//printLoaded(graph);  -- Used only for testing
	tuple<int, vector<int>> tspTour = loadTour(graph1);
	
	//Effectively deallocates memory used for graph 1 once no longer needed. 
	//See https://stackoverflow.com/questions/10464992/c-delete-vector-objects-free-memory
	vector<CityDistancePQ>().swap(graph1);		
	
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
	
	return 0;
}
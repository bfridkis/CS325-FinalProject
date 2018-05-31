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
#include <unordered_map>
#include <map>

#include "vector2.h"
#include "triangle.h"
#include "delaunay.h"

using std::vector;
using std::string;
using std::priority_queue;
using std::ifstream;
using std::ofstream;
using std::istringstream;
using std::ios;
using std::cout;
using std::getline;
using std::endl;
using std::find;
using std::tuple;
using std::get;
using std::unordered_map;
using std::map;

#define MAX_LOAD 0.25
#define TABU_PERCENTAGE 0.10
#define OPT_PERCENTAGE 0.80

//Structure to represent an edge. Each CityDistance structure
//hold the distance to a particular city.
struct CityDistance{
	int city;
	int distance;
};

//Used to modify the stl priority_heap from max heap to min heap.
//Reference: https://www.geeksforgeeks.org/implement-min-heap-using-stl/
class myComparator
{
public:
    int operator() (const CityDistance& c1, const CityDistance& c2)
    {
        return c1.distance > c2.distance;
    }
};

class myComparatorTwo
{
public:
    int operator() (const CityDistance& c1, const CityDistance& c2)
    {
        return c1.distance < c2.distance;
    }
};

typedef priority_queue<CityDistance, vector<CityDistance>, myComparator> CityDistancePQ;
typedef priority_queue<CityDistance, vector<CityDistance>, myComparatorTwo> CDPQAlt;

//Structure to represent an edge. Each CityDistance structure
//hold the distance to a particular city.
struct CityPoint{
	int x;
	int y;
	map<int, CityDistance> adj;
};

/**************************************************************************************
**                                 distCity                                          
** This function ?????????????????????????????                                                              
**************************************************************************************/
int distCity(int x1, int x2, int y1, int y2) {
	int xdiff = (x2 - x1) * (x2 - x1);
	int ydiff = (y2 - y1) * (y2 - y1);
	return static_cast<int>(round(sqrt(xdiff + ydiff)));
}

/**************************************************************************************
**                          loadPointsAsPlanarGraph                      
** This function ?????????????????????                                                    
**************************************************************************************/
unordered_map<int, CityPoint> loadPointsAsPlanarGraph(char* dataInputFileName, vector<vector<int>> &compG) {
	unordered_map<int, CityPoint> planarG;
	planarG.max_load_factor(MAX_LOAD);
	
	vector<Vector2<float>> points;
	ifstream inputData1;
	string line;
	Delaunay<float> triangulation;
	int city, cityX, cityY;
	int x1, x2, y1, y2, dTC;

	if(dataInputFileName == nullptr){
      cout << "\nMust enter file name when running program." << endl
           << "Type './greedyTSP file.txt' in command line," << endl
           << "replacing 'file.txt' with the name of your file.\n" << endl;
      exit(1);
  }

	inputData1.open(dataInputFileName);

	while (getline(inputData1, line)) {
		istringstream iss(line);
		iss >> city >> cityX >> cityY;
		CityPoint cityPoint;
		cityPoint.x = cityX;
		cityPoint.y = cityY;
		planarG[city] = cityPoint;
		points.push_back(Vector2<float>(city, cityX, cityY));
	}
	inputData1.close();

	int i;
	for (i = 0; i < static_cast<int>(planarG.size()); i++) {
		vector<int> subList (static_cast<int>(planarG.size()), -1);
		compG.push_back(subList);
	}

	const vector<Triangle<float>> triangles = triangulation.triangulate(points);
	const vector<Edge<float>> edges = triangulation.getEdges();

	for (const auto &e : edges) {
		if (planarG[e.p1.id].adj.find(e.p2.id) == planarG[e.p1.id].adj.end()) {
			x1 = planarG[e.p1.id].x;
			y1 = planarG[e.p1.id].y;
			x2 = planarG[e.p2.id].x;
			y2 = planarG[e.p2.id].y;
			dTC = distCity(x1, x2, y1, y2);
			CityDistance cd;
			cd.city = e.p2.id;
			cd.distance = dTC;
			planarG[e.p1.id].adj[e.p2.id] = cd;
			compG[e.p1.id][e.p2.id] = dTC;
		}
		if (planarG[e.p2.id].adj.find(e.p1.id) == planarG[e.p2.id].adj.end()) {
			x1 = planarG[e.p1.id].x;
			y1 = planarG[e.p1.id].y;
			x2 = planarG[e.p2.id].x;
			y2 = planarG[e.p2.id].y;
			dTC = distCity(x1, x2, y1, y2);
			CityDistance cd;
			cd.city = e.p1.id;
			cd.distance = dTC;
			planarG[e.p2.id].adj[e.p1.id] = cd;
			compG[e.p2.id][e.p1.id] = dTC;
		}	
	}

	return planarG;
}

/**************************************************************************************
**                                 getMoreNeighbors                                          
** This function ???????????????????????????                                                             
**************************************************************************************/
void getMoreNeighbors(int cityAdd, int citySearch, unordered_map<int, CityPoint> &planarG,
	unordered_map<int, CityPoint> &list, int level, vector<int> checks, vector<int> expands,
	vector<vector<int>> &compG) {
	map<int, CityDistance>::iterator it;                                       	
  map<int, CityDistance>::iterator it_two;
	int cityCheck;
	
 	for (it = planarG[citySearch].adj.begin(); it != planarG[citySearch].adj.end(); ++it) {
		if (level > 0) {
			if (expands[it->second.city] == 0) {
				expands[it->second.city] = 1;
				getMoreNeighbors(cityAdd, it->second.city, planarG, list, level - 1, checks, expands, compG);
			}
		} else {
 			for (it_two = planarG[it->second.city].adj.begin();
 				it_two != planarG[it->second.city].adj.end(); ++it_two) {
				if (checks[it_two->second.city] == 0) {
					cityCheck = it_two->second.city;
 					if ((list[cityAdd].adj.find(cityCheck) == list[cityAdd].adj.end()) &&
						(cityCheck != cityAdd) &&
						(list.find(cityCheck) != list.end())) {
						int x1, y1, x2, y2, dTC;
 						x1 = planarG[cityAdd].x;           			
 						y1 = planarG[cityAdd].y;
 						x2 = planarG[cityCheck].x;
 						y2 = planarG[cityCheck].y;
 						dTC = distCity(x1, x2, y1, y2);
 						CityDistance cd;
 						cd.city = cityCheck;
 						cd.distance = dTC;
 						list[cityAdd].adj[cityCheck] = cd;
						compG[cityAdd][cityCheck] = dTC;
					}
					checks[it_two->second.city]++;
				}
			}
		}
	}
}

/**************************************************************************************
**                                 loadTour                                          **
** This function returns a tuple with the total tour distance('<0>' of tuple) and a  **
** vector of cities ('<1>' of tuple) established in the order the cities are to be   **
** visited on the tour.                                                              **
**************************************************************************************/
tuple<int, vector<int>> loadTour(unordered_map<int, CityPoint> &planarG, int start,
	 vector<vector<int>> &compG, CityDistancePQ &tourPQ, CDPQAlt &tourPQTwo)
{
    tuple<int, vector<int>> tspTour;
    int distance = 0;
    vector<int> tspTourCities;
		map<int, CityDistance>::iterator it;
		bool expansion = true;
		unordered_map<int, CityPoint> list = planarG;
		int level = 0;

		int startingpoint;	
		int size = static_cast<int>(planarG.size());
		if (start != 0) {
			startingpoint = static_cast<int>(size/start);
		} else {
			startingpoint = 0;
		}

    int next, j;
    for(next = startingpoint, j = 0; j < static_cast<int>(planarG.size()) - 1; j++)
    {
			CityDistancePQ pq;
			for (it = list[next].adj.begin(); it != list[next].adj.end(); ++it) {
				if(list.find(it->second.city) != list.end()) {
					expansion = false;
					pq.push(it->second);
				}
			}		
			while (expansion) {
				level++;
				vector<int> checks(planarG.size(), 0);
				vector<int> expands(planarG.size(), 0);
				getMoreNeighbors(next, next, planarG, list, level, checks, expands, compG);
				for (it = list[next].adj.begin(); it != list[next].adj.end(); ++it) {
					if(list.find(it->second.city) != list.end()) {
						expansion = false;
						pq.push(it->second);
					}
				}
				if (level > 3) { // Dead end
					expansion = false;
					int k = 0;
					unordered_map<int, CityPoint>::iterator iter;
					for (iter = list.begin(); iter != list.end(); ++iter) {
						k++;
						if (iter->first != next) {
							CityDistance cd;
							cd.city = iter->first;
							cd.distance = distCity(list[next].x, iter->second.x, list[next].y, iter->second.y);
							pq.push(cd);
							compG[next][iter->first] = cd.distance;
						}
					}
				}
			}
			level = 0;
			expansion = true;
			if (next == startingpoint) {
				tspTourCities.push_back(startingpoint);
			}
			tspTourCities.push_back(pq.top().city);
			distance += pq.top().distance;
			list.erase(next);
			CityDistance repCity;
			repCity.city = next;
			repCity.distance = pq.top().distance;
			tourPQ.push(repCity);
			tourPQTwo.push(repCity);
			next = pq.top().city;
		}

		distance += distCity(planarG[startingpoint].x, planarG[next].x, planarG[startingpoint].y, planarG[next].y);
			
    get<0>(tspTour) = distance;     //Adds distance to last city before assigning to the tuple.
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
void twoOptImprove(tuple<int, vector<int>> &tspTour, unordered_map<int, CityPoint> &planarG,
		 vector<vector<int>> &compG, CityDistancePQ &tourPQ, CDPQAlt &tourPQTwo)
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
//	bool nExceeds2500;
	bool improved;
	int temp;

	unordered_map<int, CityPoint> list = planarG;
	int i;
	int G;
	int kLimit;
	int counter = -2;

	if(get<1>(tspTour).size() > 2500)
	{
		breakOutToOptimize = true;
	}
	else
	{
		breakOutToOptimize = false;
	}

	int tourSize = static_cast<int>(get<1>(tspTour).size());
	
	int tabuCount = 0;
	int tabuCity;
	vector<int> tabuList (tourSize, -1);
	if (breakOutToOptimize) {
		tabuCount = TABU_PERCENTAGE*tourSize;
		for (i = 0; i < tabuCount; i++) {
			tabuCity = tourPQ.top().city;
			tourPQ.pop();
			tabuList[tabuCity] = 1;
		}
	}

	int optCount = 0;
	int optCity;
	vector<int> optList (tourSize, -1);
	if (breakOutToOptimize) {
		optCount = OPT_PERCENTAGE*tourSize;
		for (i = 0; i < optCount; i++) {
			optCity = tourPQTwo.top().city;
			tourPQTwo.pop();
			optList[optCity] = 1;
		}
	}

	do {
	
		improved = false;
		//(Can't swap 1st city so i starts at 1...)
      for(i = 1; i < tourSize - 2; i++)		
      {
				if (breakOutToOptimize) {
					if (optList[i] == -1) {
						continue;
					}
				}
				//if (breakOutToOptimize) {
				//	kLimit = (i + 1) + (tourSize - (i + 1))*1/6;
				//} else {
					kLimit = tourSize;
				//}
				//kLimit = tourSize;
				//std::cout << "klimit: " << kLimit << "\n";
				for (int j = i, k = i + 1; k < kLimit; k++) {	
					if (k - j == 1) {
						continue;
					}
					int city1 = get<1>(tspTour)[j]; // Current edge j -> j + 1				
  				int city2 = get<1>(tspTour)[k - 1];
  				int city3 = get<1>(tspTour)[k]; // Current edge k - 1 -> k
  				int city4 = get<1>(tspTour)[j + 1];
					if (breakOutToOptimize) {
						if (tabuList[city1] == 1 || tabuList[city2] == 1 ||
								tabuList[city3] == 1 || tabuList[city4] == 1) {
							continue;
						}
					}				
				//If distance(i to i + j -1) + distance(i + 1, j) <
				//   distance(i to i + 1) + distance(j - 1 to j), the swap
				//will improve the tour. In other words, if taking out the two
				//edges before the swap and inserting two new edges (because of swap)
				//results in shorter tour, the cities are swapped in tour order.
					int dist1, dist2, dist3, dist4;
					if (compG[city1][city2] == -1) {
						dist1 = distCity(list[city1].x, list[city2].x, list[city1].y, list[city2].y);				
						compG[city1][city2] = dist1;
					} else {
						dist1 = compG[city1][city2];
					}
					if (compG[city4][city3] == -1) {
						dist2 = distCity(list[city4].x, list[city3].x, list[city4].y, list[city3].y);		
						compG[city4][city3] = dist2;
					} else {
						dist2 = compG[city4][city3];
					}
					if (compG[city1][city4] == -1) {
						dist3 = distCity(list[city1].x, list[city4].x, list[city1].y, list[city4].y);							 	
						compG[city1][city4] = dist3;
					} else {
						dist3 = compG[city1][city4];
					}
					if (compG[city2][city3] == -1) {
						dist4 = distCity(list[city2].x, list[city3].x, list[city2].y, list[city3].y);							
						compG[city2][city3] = dist4;
					} else {
						dist4 = compG[city2][city3];
					}

					G = (dist3 + dist4) - (dist1 + dist2);
 
					if(G > 0) {
					//Update tour distance based on swapped edges.
						get<0>(tspTour) -= (dist3 + dist4) - (dist1 + dist2);
						improved = true;

						//Only need to reverse cities in between swapped routes (edges).
						for(int l = j + 1, m = k - 1; l < m; l++, m--)
						{
							temp = get<1>(tspTour)[l];
							get<1>(tspTour)[l] = get<1>(tspTour)[m];
							get<1>(tspTour)[m] = temp;
						}
						
					}
				}
			}
			if (breakOutToOptimize) {
				counter++;
			} 
	} while(improved == true && counter < 0);
}

int main(int argc, char *argv[])
{
	clock_t timer1, timer2;
	timer1 = clock();
	ofstream dataOut;

	vector<vector<int>> compG;
	unordered_map<int, CityPoint> planarG;
	planarG.max_load_factor(MAX_LOAD);
	planarG = loadPointsAsPlanarGraph(argv[1], compG);

	CityDistancePQ tourPQ; // Find minimums
	CDPQAlt tourPQTwo; // Find maximums

	tuple<int, vector<int>> tspTour = loadTour(planarG, 0, compG, tourPQ, tourPQTwo);

	timer1 = clock() - timer1;
	std::cout << "Tour construction run time: " << timer1 << "\n";
	timer2 = clock();	

	twoOptImprove(tspTour, planarG, compG, tourPQ, tourPQTwo);
	timer2 = clock() - timer2;

	string inputFileName = argv[1];
	dataOut.open(inputFileName + ".tour");
	dataOut << get<0>(tspTour) << "\n";
	for(int i = 0; i < static_cast<int>(get<1>(tspTour).size()); i++)
    {
        dataOut << get<1>(tspTour)[i] << "\n";
    }
	dataOut.close();
	std::cout << "Tour improvement run time: " << timer2 << "\n";
	std::cout << "CLOCKS_PER_SEC on this system: " << CLOCKS_PER_SEC << "\n";
}

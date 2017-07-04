#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <set>

using std::multiset;
using std::vector;
using std::min;
using std::max;
using std::unordered_map;
using std::cin;
using std::cout;

// Lame hashing of int pairs
long long int _pair_hash(int ca, int cb) {
	return min(ca,cb)*10000000ll + max(ca,cb);
}

class Graph {
private:
public:
	// List of edge values
	vector<vector<double>> E;
	
	// Node colors
	vector<int> color;
	
	// Graph size
	size_t V;

	// Edges by endpoint
	unordered_map<long long int, vector<double>> E_ep;

	// Sum of all edges
	double E_sum;

	// Groups edges by matching endpoints
	void BuildEndpointEdges() {
		E_sum = 0;
		for(int i = 0; i < V-1; i++){
			for(int j = i+1; j < V; j++){
				E_ep[_pair_hash(color[i], color[j])].push_back(E[i][j]);
				E_sum += E[i][j];
			}
		}
	}

	Graph(int n) : V(n) {
		E.reserve(n);
		color.reserve(n);
		for(int i = 0; i < n; i++) E[i].reserve(n);
	}

};

class GraphMatching {
private:

public:
	Graph *g[2];

	// Between Edges
	vector <vector<double>> E; 
	double E_sum;

	// Sigma parameter
	double sigma;

	// Edge penalties
	double between_penalty;
	double within_penalty;

	// Default constructor
	GraphMatching(Graph* _ga, Graph* _gb, double _sigma = 0.5) : sigma(_sigma) {
		g[0] = _ga;
		g[1] = _gb;

		E.reserve(_ga->V);
		for(int i = 0; i < _ga->V; i++){
			E[i].reserve(_gb->V);
		}
	}

	double CalculateBetweenPenalty() {
		double ans = 0;
		E_sum = 0;
		for(int i = 0; i < g[0]->V; i++){
			for(int j = 0; j < g[1]->V; j++){
				if(g[0]->color[i] == g[1]->color[j]){
					ans += E[i][j];
				}
				E_sum += E[i][j];
			}
		}

		return between_penalty = ans/E_sum;
	}

	double CalculateWithinPenalty(){
		//Intersection of matching endpoints between nodes
		double ans = 0;
		for(int k = 0; k <= 1; k++) {
			for(auto ea : g[k]->E_ep){
				for(auto va : ea.second) {
					double cur = 0;
					size_t sz = g[1-k]->E_ep[ea.first].size();

					// Matching endpoints in other graph =  avg difference
					if(sz>0) {
						for (auto vb: g[1-k]->E_ep[ea.first]) {
							cur += fabs(vb - va);
						}

						ans += cur/((double)g[k]->E_ep[ea.first].size());

					// No matching endpoints = edge value
					} else {
						ans += va;
					}
				}	
			}
		}

		return within_penalty = ans/(g[0]->E_sum + g[1]->E_sum);
	}

	// Changes the color of a node and updates penalty
	void UpdateColor(int which, int ind, int new_color) {
		int old_color = g[which]->color[ind];

		// Update between distance:
		// Remove edges to old color
		// Add edges to new color
		for(int i = 0; i < g[1 - which]->V; i++){
			if(g[1 - which]->color[i] == old_color) {

				if(which == 0) between_penalty -= E[ind][i]/E_sum;
				else between_penalty -= E[i][ind]/E_sum;


			} else if (g[1-which]->color[i] == new_color) {
				if(which == 0) between_penalty += E[ind][i]/E_sum;
				else between_penalty += E[i][ind]/E_sum;

			}
		}

		// Updates within distance
		for(int i = 0; i < g[which]->V; i++){


		}

		g[which].color[ind] = new_color;
	}

};


int main() {
	// Fast cin
	std::ios_base::sync_with_stdio (false); 

	// Size variables 
	size_t la, lb;
	cin >> la >> lb;

	// All other variables
	Graph Ga(la), Gb(lb);
	GraphMatching G(&Ga, &Gb);
	
	// Reads graph coloring
	for(int i = 0; i < la; i++) cin >> Ga.color[i];
	for(int i = 0; i < lb; i++) cin >> Gb.color[i];

	// Reads graph within edges
	double wa[la][la], wb[lb][lb], between[la][lb];
	for(size_t i = 0; i < la; i++)
		for(size_t j = 0; j < la; j++)
			cin >> Ga.E[i][j];

	for(size_t i = 0; i < lb; i++)
		for(size_t j = 0; j < lb; j++)
			cin >> Gb.E[i][j];
	
	Ga.BuildEndpointEdges();
	Gb.BuildEndpointEdges();
	
	// Reads graph between edges
	for(size_t i = 0; i < la; i++) 
		for(size_t j = 0; j < lb; j++)
			cin >> G.E[i][j];

	cout << G.CalculateWithinPenalty() << "\n";
	cout << G.CalculateBetweenPenalty() << "\n";
}

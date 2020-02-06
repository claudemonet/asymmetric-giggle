#pragma once
#include <iostream>
#include <random>
#include <vector>
#include <time.h>
#include <list>
#include <chrono>
#include <Windows.h>
#include "adjacency.h"
#include "Distribution.h"
#include "Graph.h"
#include "Metric.h"

vector<Metric> metrics;
using namespace std;


void bounds_check(int& C, int& S, int& K)
{
	if (C > 10000) C = 10000;
	if (S > 100000) S = 100000;
	if (K > C) K = C;
}

void part2(Adjacency* adj, int &C, Metric* t)
{

	vector<string> color_method = { "SLVO", "Welsh-powell", "Random", "Iterate Edge" };

	Graph masterG(adj, C);
	
	//masterG.debug_create_EP();	//set fake E and P arrays
	masterG.build_edges();
	masterG.build_degree_list();

	const Graph constMasterG = masterG;

	for (int how_to_color = 0; how_to_color < 4; how_to_color++)
	{
		Metric* newT = new Metric(*t);
		Graph g = constMasterG;

		newT->stopwatch_click(newT->coloring_start);
		switch (how_to_color)
		{
		case 0:
			g.do_SLVO();
			newT->max_degree = g.max_degree;
			newT->terminal_clique = g.terminal_clique;
			break;
		case 1:
			g.welsh_powell();
			break;
		case 2:
			g.random_coloring();
			break;
		case 3:
			g.iterate_edge_coloring();
			break;
		}
		newT->stopwatch_click(newT->coloring_finish);
		newT->calculate_micro_seconds(newT->coloring_start, newT->coloring_finish, newT->coloring_duration);

		g.calculate_total_colors();
		g.calculate_average_original_degree();

		newT->total_colors_used = g.total_colors_used;
		newT->average_original_degree = g.average_original_degree;

		newT->coloring_method = color_method[how_to_color];
		metrics.emplace_back(*newT);
		//cout << "|-- "<< newT->coloring_method << "--|" << endl; 
		delete newT;

		//g.print_coloring_by_edge();
	}
}

int main()
{
	int C = 0;	//Number of courses being offered
	int S = 0;	//Number of Students
	int K = 0;	//Number of courses per student


	cout << "Running ";

	
	vector<int> c_vect{ 37,75,150,300,400,500,600,1000};
	vector<int> s_vect{ 10,20,40,50,75,100,200,250};
	vector<int> k_vect{ 10, 20,40,80,160,320,500,900 };

	vector<string> distro_names = { "Uniform", "Skewed_Dec", "Skewed_Inc", "4-tier" };
	vector<string> adj_method = { "List", "Matrix" };
	vector<string> color_method = { "SLVO", "Welsh-powell", "Random", "Iterate Edge" };

	for (int d = 0; d < 1; d++)
	{
		int distribution_type = 0;
		string adjType = adj_method[0];
		int how_to_color = 0;	///aka , color method
		bool part2_only = true;

		for (int ok = 0; ok < k_vect.size(); ok++)
		{

			C = c_vect[ok];
			S = s_vect[ok];
			K = k_vect[ok];

			bounds_check(C, S, K);
			list<course>* master_distro = new list<course>;

			Adjacency* adj = new Adjacency(C, adjType);
			int totalWeight = 0;

			/* -----------------------------
			0 = Uniform
			1 = Decreasing Skew
			2 = Increasing Skew
			3 = 4-tier Distribution
			   ----------------------------- */

			Metric* t = new Metric(C, S, K);

			t->distribution_type = distro_names[distribution_type];
			t->adj_storage_type = adj->getStorageType();
			t->stopwatch_click(t->total_start);

			switch (distribution_type)
			{
			case 0:
			{	//Uniform Distribution-----------------------------
				Uniform* uni = new Uniform(C, S, K);
				t->stopwatch_click(t->dist_start);
				uni->create_dist();
				t->stopwatch_click(t->dist_finish);

				//stopwatch_click(start);
				uni->do_run(adj, t);
				//stopwatch_click(finish);

				delete uni;
				break;
			}	//End Uniform Distribution--------------------------

			case 1:
			{	//Decreased Skew Distribution-----------------------------
				Skew* dec = new Skew(C, S, K);

				t->stopwatch_click(t->dist_start);
				dec->create_decreasing_dist();	//Make Distribution
				t->stopwatch_click(t->dist_finish);

				//stopwatch_click(start);
				dec->do_run(adj, t);
				//stopwatch_click(finish);

				delete dec;
				break;
			}	//End Decrease Skew Distribution--------------------------

			case 2:
			{	//Increasing Skew Distribution-----------------------------
				Skew* inc = new Skew(C, S, K);

				t->stopwatch_click(t->dist_start);
				inc->create_increasing_dist();	//Make Distribution
				t->stopwatch_click(t->dist_finish);

				//stopwatch_click(start);
				inc->do_run(adj, t);
				//stopwatch_click(finish);

				delete inc;
				break;
			}	//End Increasing Skew Distribution--------------------------

			case 3:
			{	//4-tier Distribution-----------------------------
				Tier* tiered = new Tier(C, S, K);

				t->stopwatch_click(t->dist_start);
				tiered->create_dist();	//Make Distribution
				t->stopwatch_click(t->dist_finish);

				//stopwatch_click(start);
				tiered->do_run(adj);
				//stopwatch_click(finish);

				delete tiered;
				break;
			}	//End 4-tier Distribution--------------------------
			}

			//adj->print_E_histogram();
			//cin.get();

			t->stopwatch_click(t->make_EP_start);
			adj->finalize();	//Make E and P 
			t->stopwatch_click(t->make_EP_finish);

			t->stopwatch_click(t->total_finish);	//Finish total part 1

			t->calculate(t->dist_start, t->dist_finish, t->dist_build_duration);
			t->calculate(t->make_EP_start, t->make_EP_finish, t->make_EP_duration);
			t->calculate(t->total_start, t->total_finish, t->total_duration);


			//adj->print_E();
			//adj->print_P();

			//END PART 1--------------------------

			//BEGIN PART 2--------------------------
			if (part2_only) 
			{ 
				part2(adj, C, t); 
				//break;
			}
			else
			{
				Graph g(adj, C);

				//g.debug_create_EP(); //set fake E and P arrays

				g.build_edges();
				g.build_degree_list();

				t->stopwatch_click(t->coloring_start);
				switch (how_to_color)
				{
				case 0:
					g.do_SLVO();
					break;
				case 1:
					g.welsh_powell();
					break;
				case 2:
					g.random_coloring();
					break;
				case 3:
					g.iterate_edge_coloring();
					break;
				}
				t->stopwatch_click(t->coloring_finish);
				t->calculate(t->coloring_start, t->coloring_finish, t->coloring_duration);

				g.print_coloring_by_edge();
			}
			//metrics.push_back(*t);
			delete adj; delete t;
			delete master_distro;
			cout << ok + 1 << "/" << k_vect.size() << " complete." << endl << flush;
		}
	}
	cout << endl;
	//Print metrics to screen 
	cout << "Dist, C, S, K, C*S, K*S, C*S*K, Storage, ";
		//Conflicts, Dist Conflicts, 
	cout << "Dist Build Duration, Class Pick Duration, ";
		//Make Adjacency Duration, Make E[]P[] Duration, Total Duration" 
	cout << "Coloring Method";
	cout << "Coloring Duration(microseconds!),";
	cout << "Total colors used,";
	cout << "Average original degree,";
	cout << "Max degree when deleted,";
	cout << "Terminal Clique"<< endl;

	for (auto iter = metrics.begin(); iter != metrics.end(); iter++)
	{
		cout << iter->distribution_type << ",";
		cout << iter->C << ",";
		cout << iter->S << ",";
		cout << iter->K << ",";
		cout << iter->C * iter->S << ",";
		cout << iter->K * iter->S << ",";
		cout << iter->C * iter->S * iter->K << ",";
		cout << iter->adj_storage_type << ",";
		//cout << iter->conflicts << ",";
		//cout << iter->distinctConflicts << ",";
		cout << iter->dist_build_duration << ",";
		cout << iter->class_pick_duration << ",";
		//cout << iter->make_adj_duration << ",";
		//cout << iter->make_EP_duration << ",";
		//cout << iter->total_duration << endl;
		cout << iter->coloring_method << ",";
		cout << iter->coloring_duration << ",";
		cout << iter->total_colors_used << ",";
		cout << iter->average_original_degree <<",";
		if (iter->coloring_method == "SLVO")
			cout << iter->max_degree << "," << iter->terminal_clique << endl;
		else
			cout << "NA,NA" << endl;
	}
	
	return 0;
}

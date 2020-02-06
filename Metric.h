#pragma once

#include <string>
#include <chrono>
#include <iostream>

using namespace std;

class Metric{
protected:

public:
	//Duration metrics:
	float total_duration = 0;
	float dist_build_duration = 0;
	float class_pick_duration = 0;
	float make_adj_duration = 0;
	float make_EP_duration = 0;
	float coloring_duration = 0;
	int total_colors_used = 0;
	float average_original_degree = 0;
	int max_degree = 0;
	int terminal_clique = 0;

	int C;
	int S;
	int K;
	int conflicts;
	float distinctConflicts;
	string distribution_type;
	string adj_storage_type;
	string coloring_method;

	//Time variables:
	chrono::high_resolution_clock::time_point total_start;
	chrono::high_resolution_clock::time_point total_finish;

	chrono::high_resolution_clock::time_point dist_start;
	chrono::high_resolution_clock::time_point dist_finish;

	chrono::high_resolution_clock::time_point class_pick_start;
	chrono::high_resolution_clock::time_point class_pick_finish;

	chrono::high_resolution_clock::time_point make_adj_start;
	chrono::high_resolution_clock::time_point make_adj_finish;

	chrono::high_resolution_clock::time_point make_EP_start;
	chrono::high_resolution_clock::time_point make_EP_finish;

	chrono::high_resolution_clock::time_point coloring_start;
	chrono::high_resolution_clock::time_point coloring_finish;

	//Metric();
	
	Metric(int& C, int& S, int& K)
	{
		this->C = C; this->S = S; this->K = K;
	}


	void stopwatch_click(chrono::high_resolution_clock::time_point&);
	void calculate(chrono::high_resolution_clock::time_point&, chrono::high_resolution_clock::time_point&, float&);
	void calculate_micro_seconds(chrono::high_resolution_clock::time_point&, chrono::high_resolution_clock::time_point&, float&);
};

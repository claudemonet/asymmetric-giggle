#include "Metric.h"

void Metric::stopwatch_click(chrono::high_resolution_clock::time_point& now) 
{
	now = chrono::high_resolution_clock::now(); 
}

void Metric::calculate(chrono::high_resolution_clock::time_point& start, chrono::high_resolution_clock::time_point& finish, float& var)
{
	var += chrono::duration_cast<chrono::milliseconds>(finish - start).count();
}

void Metric::calculate_micro_seconds(chrono::high_resolution_clock::time_point& start, chrono::high_resolution_clock::time_point& finish, float& var)
{
	var += chrono::duration_cast<chrono::microseconds>(finish - start).count();
}
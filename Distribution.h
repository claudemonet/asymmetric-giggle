#pragma once
#include <list>
#include "adjacency.h"
#include "Metric.h"


struct course {
	int course_num;
	int weight;
	course(int& c, int& w) { course_num = c; weight = w; }
};

class Distribution
{
protected:
	//list<course>* tmp;
	int C;
	int S;
	int K;
	bool adjList; 
	bool adjMatrix;

public:
	//int create_decreasing_dist(list<course>* tmp, int C);
	int create_increasing_dist();
	int create_tiered_dist();
	void process_thisStudentCourses(Adjacency *, list<int>*);
};

class Uniform : public Distribution
{
protected:
	vector<int> classes;	//This distro to be used for uniform distribution

public: 
	Uniform(int& C, int& S, int& K) { this->C = C, this->S = S, this->K = K; classes.resize(0); }
	void create_dist();
	int choose_course(vector<int>*);
	void do_run(Adjacency *, Metric* t);
};

class Skew : public Distribution
{
protected:
	list<course> classes;
	int totalWeight = -1;

public:
	Skew(int& C, int& S, int& K) { this->C = C, this->S = S, this->K = K; };
	void create_decreasing_dist();
	void create_increasing_dist();
	void do_run(Adjacency*, Metric* t);
	int do_choice(list<course>*, list<int>*, int&);
};

class Tier : public Distribution
{
protected:
	vector<int> tiers;
	vector<vector<int>> subtiers;

public: 
	Tier(int& C, int& S, int& K) {
		this->C = C, this->S = S, this->K = K;
		subtiers.resize(4);
		tiers.resize(0);	}
	
	void create_dist();
	void do_run(Adjacency*);
	int do_choice(vector<int>&);
};
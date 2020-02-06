#pragma once
#include <set>
#include <list>
#include <vector>
#include <iostream>

using namespace std; 
class Adjacency
{
protected:
	vector<set<int>> masterList;
	vector<vector<int>> masterMatrix;
	unsigned int conflicts;
	float distinctConflicts;
	vector<int>E;
	vector<int>P;
	bool adjList = false;
	bool adjMatrix = false;
	string adjStorage;

public:
	Adjacency(int C, string adj) { 
		masterList.resize(C); masterMatrix.resize(C, vector<int>(C)); 
		conflicts = 0; distinctConflicts = 0.0; 
		if (adj == "List") adjList = true, adjStorage = "List";
		if (adj == "Matrix") adjMatrix = true, adjStorage = "Matrix";
	}
	int get_conflict_count() { return conflicts; }
	float get_distinct_conflict_count() { return distinctConflicts; }
	vector<int>* get_E() { return new vector<int>(E); }
	vector<int>* get_P() { return new vector<int>(P); }
	bool getAdjListFlag() { return adjList; }
	bool getAdjMatrixFlag() { return adjMatrix; }
	string getStorageType() { return adjStorage; }

	void student_list_to_masterList(list<int>*);
	void student_list_to_masterMatrix(list<int>*);
	void print_E();
	void print_P();
	void print_E_histogram();

	void finalize();
	void masterList_to_E_P();
	void masterMatrix_to_E_P();
};


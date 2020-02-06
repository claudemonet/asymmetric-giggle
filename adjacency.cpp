#include "adjacency.h"

using namespace std;

void Adjacency::student_list_to_masterList(list<int>* studentList)
{
	//Add an individual student's class list to the master adjacency list
	int pivotClass = 0, midpoint = 0, k = 0;

	for (auto iter = studentList->begin(); iter != studentList->end(); iter++)
	{
		pivotClass = *iter;

		for (auto iter2 = studentList->begin(); iter2 != studentList->end(); iter2++)
		{
			if (*iter2 != pivotClass)
			{
				auto result = masterList[pivotClass].emplace(*iter2);
				if (!result.second)
				{
					//If already in set, add to conflicts
					distinctConflicts += 0.5;
					conflicts++;
				}
			}
		}
	}
}

void Adjacency::student_list_to_masterMatrix(list<int>* studentList)
{
	//Add an individual student's class list to the master adjacency matrix

	int pivotClass = 0;

	for (auto iter = studentList->begin(); iter != studentList->end(); iter++)
	{
		pivotClass = *iter;

		for (auto iter2 = studentList->begin(); iter2 != studentList->end(); iter2++)
		{
			if (*iter2 != pivotClass)
			{
				if (masterMatrix[pivotClass][*iter2] == 0) distinctConflicts++;
				if (masterMatrix[pivotClass][*iter2] > 0) conflicts++;
				masterMatrix[pivotClass][*iter2]++;
				masterMatrix[*iter2][pivotClass]++;
			}
		}
	}
}

void Adjacency::print_E()
{
	cout << endl << "\r\n---Output of E---" << endl;
	for (auto const& i : E)
		cout << "[" << i << "]";
}

void Adjacency::print_P()
{
	cout << endl << "\r\n---Output of P---" << endl;
	for (auto const& i : P)
		cout << "[" << i << "]";
}

void Adjacency::print_E_histogram()
{
	//Currently only works for masterList, not masterMatrix
	if (!adjList) return;

	vector<set<int>> n = masterList;
	// Source: https://www.tenouk.com/cpluscodesnippet/printsimplehistogram.html

	cout << endl;
	// display the table header...
	int SIZE = n.size();

	printf("%s%13s%17s\n", "Element/index", "Value", "Histogram");

	// do the iteration, outer for loop, read row by row...
	for (int i = 0; i <= (SIZE - 1); i++)
	{
		printf("%9d%15d ", i, n[i].size());

		// inner for loop, for every row, read column by column and print the bar...
		for (int j = 0; j < n[i].size(); j++)
			// print the 'bar', and repeat...
			printf("*");
		// go to new line for new row, and repeats...
		printf("\n");
	}
}

void Adjacency::finalize()
{
	if (adjList) { masterList_to_E_P();	}
	if (adjMatrix) { masterMatrix_to_E_P(); }

}

void Adjacency::masterList_to_E_P()
{
	//Make the E vector 
	//Iterate through each student
	int location = 0;
	P.push_back(location);	//Add the first 0 to accommodate for the isEmpty flag...
	bool isEmpty = true;

	//Get that row class graph
	for (int counter = 0; counter < masterList.size(); counter++)
	{
		if (!isEmpty)
		{
			P.push_back(location);
			isEmpty = true;		//need a flag in case the following loop has a 0 size array
								//otherwise, it'd push location multiple times
		}
		for (auto iter = masterList[counter].begin(); iter != masterList[counter].end(); iter++, location++)
		{
			E.push_back(*iter);
			isEmpty = false;
		}
	}
}
void Adjacency::masterMatrix_to_E_P()
{
	int location = 0;
	P.push_back(location);	//Add the first 0 to accommodate for the isEmpty flag.
	bool isEmpty = true;

	for (int i = 0; i < masterMatrix.size(); i++) 
	{
		if (!isEmpty)	//If not empty, push value to P
		{
			P.push_back(location);
			isEmpty = true;
		}

		for (int j = 0; j < masterMatrix[i].size(); j++) 
		{
			if (masterMatrix[i][j] > 0) 
			{
				E.push_back(j);
				location++;
				isEmpty = false;
			}
		}
	}
}
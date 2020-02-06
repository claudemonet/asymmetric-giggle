#include "Distribution.h"
#include <vector>
#include <time.h>
#include <random>

using namespace std;

void Distribution::process_thisStudentCourses(Adjacency *adj, list<int> *thisStudentCourses)
{
	if (adj->getAdjListFlag()) { adj->student_list_to_masterList(thisStudentCourses); }
	if (adj->getAdjMatrixFlag()) { adj->student_list_to_masterMatrix(thisStudentCourses); }
}

void Uniform::create_dist()
{
	for (int k = 0; k < C; k++)
		classes.push_back(k);
}

int Uniform::choose_course(vector<int>* tmpClasses)
{
	//Shuffles the classes once, and pops and returns the last value

	if (tmpClasses->size() == 0) return -1;
	
	if (tmpClasses->size() == C) //If tmpClasses = C, then it's a new array that needs randomization
	{
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		shuffle(tmpClasses->begin(), tmpClasses->end(), default_random_engine(seed));
	}
	int returnValue = tmpClasses->back();
	tmpClasses->pop_back();
	return returnValue;
}

void Uniform::do_run(Adjacency *adj, Metric* t)
{
	
	//Loop through each student
	for (int i = 0; i < S; i++)
	{
		//Do course selections for each student S
		list<int>* thisStudentCourses = new list<int>;	//Create a new course list for this student
		vector<int>* tmpMasterClasses = new vector<int>(classes);
		
		t->stopwatch_click(t->class_pick_start);
		for (int k = 0; k < K; k++)
			thisStudentCourses->emplace_back(choose_course(tmpMasterClasses));	//Select courses 
		t->stopwatch_click(t->class_pick_finish);

		t->calculate(t->class_pick_start, t->class_pick_finish, t->class_pick_duration);

		t->stopwatch_click(t->make_adj_start);
			process_thisStudentCourses(adj, thisStudentCourses);	//Add this student courses to master list or matrix
		t->stopwatch_click(t->make_adj_finish);
		
		t->calculate(t->make_adj_start, t->make_adj_finish, t->make_adj_duration);

		delete tmpMasterClasses;
		delete thisStudentCourses;
	}
	
}

void Skew::create_decreasing_dist()
{
	//Add classes and corresponding weights to a list
	//Return the sum of total weight

	int totalWeight = 0, k = 0, tmpC = C;

	do
	{
		course* new_course = new course(k, tmpC);
		classes.emplace_back(*new_course);
		delete new_course;
	
		totalWeight += tmpC;
		tmpC--;
		k++;

	} while (tmpC);

	this->totalWeight = totalWeight;
}

void Skew::create_increasing_dist()
{
	//Add classes and corresponding weights to a list
	//Return the sum of total weight

	int totalWeight = 0, k = 0, tmpC = 0;

	do
	{
		course* new_course = new course(k, tmpC);
		classes.emplace_back(*new_course);
		delete new_course;

		totalWeight += tmpC;
		tmpC++;
		k++;

	} while (tmpC < C);

	this->totalWeight = totalWeight;
}

void Skew::do_run(Adjacency* adj, Metric* t)
{
	int decrementWeight = 0, randVal = 0, k = 0, courseMean = 0, tmpWeight = -1;

	for (int i = 0; i < S; i++)
	{
		//This loop is to create a student, and pick courses for that student
		list<course>* tmpDist = new list<course>(classes);  //Create Temp distribution for each student, based off classes
		list<int>* thisStudentCourses = new list<int>;			//List of this student courses
		tmpWeight = totalWeight;	
		decrementWeight = 0;

		//Let's pick this student's classes
		t->stopwatch_click(t->class_pick_start);			
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		srand(seed);  //seed random
		for (k = 0; k < K; k++)	
		{

			tmpWeight -= decrementWeight;		//Once a class is chosen, decrement that course weight from totalWeight
												//Since we'll delete the chosen class
			if (tmpWeight < 1)
			{
				k--;
				continue;
			}
			randVal = rand() % tmpWeight;
			decrementWeight = do_choice(tmpDist, thisStudentCourses, randVal);
		}
		t->stopwatch_click(t->class_pick_finish);
		t->calculate(t->class_pick_start, t->class_pick_finish, t->class_pick_duration);
		//End picking this student's classes

		t->stopwatch_click(t->make_adj_start);
			process_thisStudentCourses(adj, thisStudentCourses);
		t->stopwatch_click(t->make_adj_finish);

		t->calculate(t->make_adj_start, t->make_adj_finish, t->make_adj_duration);

		delete tmpDist;
		delete thisStudentCourses;
	}
	//write final E P 
}

int Skew::do_choice(list<course>* tmp, list<int>* thisStudentCourses, int& randVal)
{
	int j = 0, returnVal = 0;
	for (auto iter = tmp->begin(); iter != tmp->end(); iter++, j++)
	{
		if (randVal < iter->weight)
		{
			//insert course to student list 
			thisStudentCourses->emplace_back(iter->course_num);
			returnVal = iter->weight;
			tmp->erase(iter);
			return returnVal;
		}
		randVal -= iter->weight;
	}
}

void Tier::create_dist()
{
	tiers.push_back(0);
	tiers.push_back(0);
	tiers.push_back(0);
	tiers.push_back(0);
	
	tiers.push_back(1);
	tiers.push_back(1);
	tiers.push_back(1);

	tiers.push_back(2);
	tiers.push_back(2);

	tiers.push_back(3);

	int interval = C / 4;
	int finish = 0, k = 0;

	for (int i = 0; i < subtiers.size(); i++)
	{
		for (k = 0; k < interval; k++, finish++)
			subtiers[i].push_back(finish);
	}
}

void Tier::do_run(Adjacency* adj)
{
	//Loop through each student
	for (int i = 0; i < S; i++)
	{
		//Do course selections for each student S
		list<int>* thisStudentCourses = new list<int>;	//Create a new course list for this student
		vector<vector<int>>* tmpMasterClasses = new vector<vector<int>>(subtiers);
		vector<int>* tmpTiers = new vector<int>(tiers);

		int subtier = -1, addClass = -1;
		int modVal = tmpMasterClasses->size();
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		srand(seed);

		for (int k = 0; k < K; k++)
		{
			//srand(time(NULL));	//Seed random
			subtier = (*tmpTiers)[rand() % modVal];
			if ((*tmpMasterClasses)[subtier].size() == 0)
			{
				//If we've depleted a tier, remove it:
				tmpTiers->erase(remove(tmpTiers->begin(), tmpTiers->end(), subtier), tmpTiers->end());
				modVal = tmpTiers->size() - 1;	//Adjust our modVal
				if (modVal == 0) modVal++;		//Case where one tier left
				k--;							//Decrement k, and and re-run
				continue;
			}
			else
			{
				addClass = do_choice((*tmpMasterClasses)[subtier]);			//Select course 
				thisStudentCourses->emplace_back(addClass);
			}
		}

		process_thisStudentCourses(adj, thisStudentCourses);	//Add this student courses to master list or matrix
		delete tmpMasterClasses;	
		delete thisStudentCourses;
		delete tmpTiers;
	}
}

int Tier::do_choice(vector<int> &subTierN)
{
	if (subTierN.size() == 0) return -1;

	if (subTierN.size() == C/4)		//If subTier = C/4, then it's a new array that needs randomization
	{
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		shuffle(subTierN.begin(), subTierN.end(), default_random_engine(seed));
	}

	int returnValue = -1;
	returnValue = subTierN.back();
	subTierN.pop_back();

	return returnValue;
}

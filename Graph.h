#pragma once
#include <vector>
#include <stack>
#include <queue>
#include "adjacency.h"

using namespace std;

//struct vertex_pointer {};

struct vertex {
	int class_number = -1;
	//vector<vertex_pointer>* ptr_edge_list;
	int degree = -1;
	int initial_degree = -1;
	bool deleted = false;
	int degree_at_deletion = -1;
	int max_degree_at_deletion = -1;
	int color = -1;
	int coloring_order;
	vertex* next = NULL;
	vertex* prev = NULL;

	//from class:
	//struct<vertext>* adj list head???
	//struct<vertext>* adj list next 
	//struct<vertext>* degree list previous
	//struct<vertext>* degree list next
	//struct<vertext>* deleted list
};

struct vertex_pointer {
	vertex* vp = NULL;
	vector<int>	colors_used{ 0 };
	vertex_pointer* next = NULL;
	vertex_pointer();
	vertex_pointer(vertex* tmp, vertex_pointer* tmpNext) { vp = tmp; next = tmpNext; }
};

class Graph
{
protected:
	int C;
	vector<int>* E;
	vector<int>* P;
	vector<vertex_pointer*> edge_list;			//==  E
	vector<int>edge_list_degrees;//?????
	vector<vertex*> degree_list;				//==V
	stack<vertex*> deleted_vertices;
	queue<vertex*> color_queue;

	bool SLVO = false;
	
public:
	int total_colors_used = 0;
	float average_original_degree = 0;
	int max_degree = 0;
	int terminal_clique = 0;

	Graph() {};
	Graph(Adjacency* tmpAdj, int &C) {
		this->C = C;
		this->E = tmpAdj->get_E();  this->P = tmpAdj->get_P();
		edge_list.resize(C, NULL);
		degree_list.resize(C, NULL);
		edge_list_degrees.resize(C, -1);

	}// ->size() - 1);

	void build_edges();
	void build_degree_list();
	//void delete_degrees()
	//void process();
	//void color();
	void add_edge(int&, int&);//vertex_pointer*);
	void add_vertex(int&, vertex*);
	vertex_pointer* create_new_vertex(int&);
	
	void do_SLVO();
	void move_vertex(vertex*, int);
	void delete_vertex(vertex*);
	void decrement_edge(vertex*);
	bool color_check(vertex*);

	void random_coloring();
	void welsh_powell();
	void iterate_edge_coloring();
	
	void pop_and_color();
	void print_coloring_by_edge();
	void color_this_edge(vertex*);
	void process_color_queue();
	void print_SLVO_data();

	void calculate_total_colors();
	void calculate_average_original_degree();

	void debug_create_EP();
};


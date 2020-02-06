#include "Graph.h"
#include <random>

using namespace std;

void Graph::add_edge(int& pivotClass, int& insertClass)
{
	if (edge_list[pivotClass] == NULL)	//If pivot vertex doesn't exist, create it 
		create_new_vertex(pivotClass);

	if (edge_list[insertClass] == NULL)	//If new class to be inserted doesn't exist, create it 
		create_new_vertex(insertClass);

	vertex_pointer* iter = edge_list[pivotClass]->next;

	while (iter != NULL)
	{
		if (iter->vp->class_number == insertClass) return;	//exit if already in list
		iter = iter->next;
	}
	delete iter;

	//If at this point, this should be a new vertex that needs to be added
	vertex_pointer* addThisNewPtr = new vertex_pointer(NULL, NULL);	//Create a VP

	addThisNewPtr->vp = edge_list[insertClass]->vp;

	//Add class to front of list (i.e., 1st node after the edge #)
	vertex_pointer* oldFront = edge_list[pivotClass]->next;
	edge_list[pivotClass]->next = addThisNewPtr;
	addThisNewPtr->next = oldFront;

	edge_list_degrees[pivotClass]++;
	edge_list[pivotClass]->vp->degree++;
	edge_list[pivotClass]->colors_used.resize(edge_list[pivotClass]->vp->degree); // Resize color vertex to accommodate all edges

}

vertex_pointer* Graph::create_new_vertex(int &where)
{
	//Creates a new vertex and vertex_pointer
	//Inserts vertex_pointer at at edge_list[where]
	//Returns vertex_pointer 

	if (edge_list[where] != NULL) return NULL;	//If a vertex_pointer already exists, return

	vertex* v = new vertex;
	vertex_pointer* ptr = new vertex_pointer(v, NULL);
	v->class_number = where;
	v->degree = 0;
	edge_list[where] = ptr;
	edge_list_degrees[where] = 0;
	return ptr;

}

void Graph::build_edges()
{
	int numEdges = -1;
	int pivotClass = -1;
	for (int i = 0; i < P->size()-1; i++)
	{
		numEdges = (*P)[i + 1] - (*P)[i];
		
		for (int j = (*P)[i], count = 0; count < numEdges; j++, count++)
		{
			pivotClass = (*E)[j];

			//Iterate around this pivot class, adding vertices
			for (auto iter = (*P)[i], count = 0; count < numEdges; iter++, count++)
			{
				if (pivotClass != (*E)[iter]) add_edge(pivotClass, (*E)[iter]);
			}
		}
	}
}

void Graph::build_degree_list()
{
	int location = 0;

	for (int j = 0; j < edge_list.size(); j++)
	{
		if (!edge_list[j]) continue;

		location = edge_list[j]->vp->degree;
		add_vertex(location, edge_list[j]->vp);
	}

	//Set initial degree variable and resize colors_used of each node now that degree list is built
	for (int w = 0; w < edge_list.size(); w++)
	{
		if (edge_list[w])
		{
			edge_list[w]->vp->initial_degree = edge_list[w]->vp->degree;
			edge_list[w]->colors_used.resize(edge_list[w]->vp->initial_degree, -1);

			if (edge_list[w]->vp->initial_degree > max_degree) max_degree = edge_list[w]->vp->initial_degree; 	//Set max degree variable
		}
	}
}

void Graph::add_vertex(int& location, vertex* addMe)
{
	//If vertex not in list, add it and return
	if (degree_list[location] == NULL)
	{
		degree_list[location] = addMe;
		return;
	}

	//Insert to front of DLL
	addMe->next = degree_list[location]->next; 
	addMe->prev = degree_list[location];
	if (addMe->next != NULL) addMe->next->prev = addMe;
	degree_list[location]->next = addMe;
}

void Graph::do_SLVO()
{
	for (int k = 0; k < degree_list.size(); k++)
	{
		if (!degree_list[k]) continue;	//go to next iteration if element is NULL
		
		vertex* deleteMe = degree_list[k];
		delete_vertex(deleteMe);

		//Get that deleted vertex's neighbors -- decrement them all
		vertex_pointer* iter = edge_list[deleteMe->class_number];
		vertex* neighbor = NULL;
		bool decremented = false;

		while (iter->next != NULL)
		{
			neighbor = iter->next->vp;
			if (!neighbor->deleted)		//only decrement if its still alive
			{
				decremented = true;
				decrement_edge(neighbor);	
			}
			iter = iter->next;
		}
		if (decremented) k -= 2;
	}

	//Edges have been deleted, now time to color 
	SLVO = true;
	pop_and_color();
}

void Graph::pop_and_color()
{
	vertex* colorMe = new vertex;
	int color, coloring_order = 0;
	int next_degree = 0, this_degree = 0;

	if (SLVO)
	{
		//cout << "[class,original_degree,degree_when_deleted,order_colored,max_degree_when_deleted]";//,terminal_clique]";
	}
	while (!deleted_vertices.empty())
	{
		colorMe = deleted_vertices.top();	//Get top 
		deleted_vertices.pop();				//pop it off stack


		colorMe->coloring_order = coloring_order++;
		color_this_edge(colorMe);

		if (SLVO)
		{
			if (!deleted_vertices.empty())
			{
				next_degree = deleted_vertices.top()->degree;
				this_degree = colorMe->degree_at_deletion;
				
				if (abs(this_degree - next_degree) > 1)
					terminal_clique = colorMe->initial_degree;
			}
			//cout << colorMe->class_number << "," << colorMe->initial_degree << "," << colorMe->degree_at_deletion << "," << colorMe->coloring_order << "," << colorMe->max_degree_at_deletion<< "," << endl;
		}
	}
}

void Graph::color_this_edge(vertex* colorMe)
{
	if (colorMe->color > 0) return;	//if this edge is already colored, no need to color again. Return.

	int color = 0;

	while (colorMe->color < 0)	//color default is -1, so keep iterating until we give it a number
	{
		if (color > edge_list[colorMe->class_number]->colors_used.size() - 1)	//If not enough colors, push back another
		{
			edge_list[colorMe->class_number]->colors_used.push_back(-1);
		}
		else if (edge_list[colorMe->class_number]->colors_used[color] > 0)	//If a color is >0, that means it's already picked
		{																//so increment color, and continue
			color++;
			continue;
		}
		colorMe->color = color;	//We should now have our color
		edge_list[colorMe->class_number]->colors_used[color] = 1;	//update it in the colors_used vector
	}

	/////update the colors of my neighbors/////
	vertex_pointer* iter = edge_list[colorMe->class_number]->next;

	while (iter != NULL)
	{
		if (color > edge_list[iter->vp->class_number]->colors_used.size() - 1)	////If not enough colors, push back another
		{
			edge_list[iter->vp->class_number]->colors_used.push_back(-1);
			continue;
		}

		edge_list[iter->vp->class_number]->colors_used[color] = 1;
		iter = iter->next;
	}
	/////end update colors/////
}



bool Graph::color_check(vertex* colorMe)
{
	vertex_pointer* iter = edge_list[colorMe->class_number]->next;
	while (iter != NULL)
	{
		if (colorMe->color == iter->vp->color) return false;
		iter = iter->next;
	}

	return true;
}


void Graph::delete_vertex(vertex* deleteMe)
{
	//This deletes vertex from the Degree_List, adds it to the deleted_vertices stack.

	deleteMe->deleted = true;
	deleteMe->degree_at_deletion = deleteMe->degree;
	deleteMe->max_degree_at_deletion = max_degree;
	deleted_vertices.push(deleteMe);	//Add to stack of deleted vertices

	if (deleteMe->prev == NULL && deleteMe->next == NULL)	//If front of empty list	
		degree_list[deleteMe->degree] = NULL;
	else if (deleteMe->prev == NULL && deleteMe->next != NULL)	//If front of non-empty list
	{
		deleteMe->next->prev = NULL;
		degree_list[deleteMe->degree] = deleteMe->next;
	}
	else
	{
		//Delete me from current location
		deleteMe->prev->next = deleteMe->next;
		if (deleteMe->next) deleteMe->next->prev = deleteMe->prev;
	}

	deleteMe->next = NULL;
	deleteMe->prev = NULL;
}


void Graph::decrement_edge(vertex* moveMe)
{
	//Decrement edge count by one and moves it to new spot in degree list
	move_vertex(moveMe, moveMe->degree-1);	//Move it
	moveMe->degree--;						//Then decrement it

	if (degree_list[moveMe->degree+1] == NULL && moveMe->degree + 1 == max_degree)	max_degree--;	//if this is/was the largest degree, 
																									//and the degree list is now empty
																									//let's also decrement max degree
}

void Graph::move_vertex(vertex* moveMe, int dest)
{
	//Deletes vertex from current location in degree_list, move to new one

	//If it's alone:
	if (moveMe->prev == NULL && moveMe->next == NULL) //If front of empty list
		degree_list[moveMe->degree] = NULL;
	else if (moveMe->prev == NULL && moveMe->next != NULL)	//If front of non-empty list
	{
		moveMe->next->prev = NULL;
		degree_list[moveMe->degree] = moveMe->next;
	}
	else
	{
		//Delete me from current location
		moveMe->prev->next = moveMe->next;
		if (moveMe->next) moveMe->next->prev = moveMe->prev;
	}
	
	//Clear out pointers
	moveMe->next = NULL;
	moveMe->prev = NULL;
	
	//Then add me to new home
	add_vertex(dest, moveMe);
}



void Graph::welsh_powell()
{
	
	for (int k = 0, color; k < degree_list.size(); k++)
	{
		if (!degree_list[k]) continue;	//go to next iteration if element is NULL
		vertex* colorMe = degree_list[k];
		color_queue.push(colorMe);
		
		vertex_pointer* iter = edge_list[colorMe->class_number]->next;
		while (iter != NULL)	//color this node's edges
		{
			color_queue.push(iter->vp);
			iter = iter->next;
		}

	}
	process_color_queue();
}


void Graph::random_coloring()
{
	vector<vertex_pointer*>tmpEdge_list;

	for (int k = 0; k < edge_list.size(); k++)
		if (edge_list[k] != NULL) tmpEdge_list.push_back(edge_list[k]);

	shuffle(tmpEdge_list.begin(), tmpEdge_list.end(), default_random_engine());

	for (int p = 0; p < tmpEdge_list.size(); p++)
		color_queue.push(tmpEdge_list[p]->vp);
	
	//pop_and_color();
	process_color_queue();
}

void Graph::process_color_queue()
{
	vertex* colorMe = new vertex;
	int color, coloring_order = 1;

	while (!color_queue.empty())
	{
		colorMe = color_queue.front();	//Get front element
		color_queue.pop();				//pop it off queue 
		colorMe->coloring_order = coloring_order++;
		color_this_edge(colorMe);
	}
}


void Graph::iterate_edge_coloring()
{
	//Iterates up edge list from 0, coloring each edge along the way
	for (int k = 0; k < edge_list.size(); k++)
	{
		if (!edge_list[k]) continue;	//go to next iteration if element is NULL

		vertex* colorMe = edge_list[k]->vp;
		vertex_pointer* iter = edge_list[k];

		while (iter->next != NULL)
		{
			if (colorMe->color < 0) colorMe->color = 0; //If at default value of -1, initialize it to 0
			while (!color_check(colorMe))
				colorMe->color++;
			iter = iter->next;
			colorMe = iter->vp;
		}
	}
}

void Graph::print_coloring_by_edge()
{

	cout << "\n[class:degrees:color_order:color]" << endl;
	vertex_pointer* iter = NULL;

	for (int k = 0; k < edge_list.size(); k++)
	{
		if (!edge_list[k]) continue;	//go to next iteration if element is NULL

		iter = edge_list[k];
		cout << "[" << iter->vp->class_number << ":" << iter->vp->initial_degree << ":" << iter->vp->coloring_order << ":" << iter->vp->color << "]->";

		iter = iter->next;
		while (iter != NULL)
		{
			cout << "[" << iter->vp->class_number << ":" << iter->vp->initial_degree << ":" << iter->vp->coloring_order << ":" << iter->vp->color << "]";
			iter = iter->next;
		}
		cout << endl;
	}
}

void Graph::debug_create_EP()
{
	vector<int> tmpE{ 1, 3, 5, 7, 9, 2, 4, 10, 11, 12, 13, 14, 15, 16 };
	vector<int> tmpP{ 0, 5, 7, 10, 14 };

	this->E->resize(0);
	this->P->resize(0);

	this->E->swap(tmpE); //= tmpE;
	this->P->swap(tmpP); //= tmpE;

}


void Graph::calculate_total_colors()
{
	#include <set>;
	set<int>total_colors;

	//Let's traverse each edge and insert their colors
	//Since set only holds unique elements, when finished, we'll know how many colors were used

	for (int k = 0; k < edge_list.size(); k++)
		if (edge_list[k] != NULL) total_colors.insert(edge_list[k]->vp->color);		
	
	total_colors_used = total_colors.size();

	if (SLVO && terminal_clique == 0)
		terminal_clique = edge_list[(*E)[0]]->vp->degree; //if terminal clique wasn't populated, the entire graph was terminal clique from the beginning
}
void Graph::calculate_average_original_degree()
{
	int sum = 0;
	for (int k = 0; k < edge_list.size(); k++)
		if (edge_list[k] != NULL) sum += edge_list[k]->vp->initial_degree;

	average_original_degree = sum / edge_list.size();
}
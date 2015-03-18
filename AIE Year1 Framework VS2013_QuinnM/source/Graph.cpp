#include "Graph.h"

GrNode::GrNode(int in_name) {
	name = in_name;
	visited = false;
	posX = 0.0f;
	posY = 0.0f;
	previousNode = nullptr;
	gScore = 0;
}

GrNode::GrNode(int in_name, float in_x, float in_y) {
	name = in_name;
	visited = false;
	posX = in_x;
	posY = in_y;
	previousNode = nullptr;
	gScore = 0;
}

GrNode::~GrNode() { }

void GrNode::RemoveEdgesTo(int in_name) {
	for (int i = 0; i < edges.size(); i++) {
		if (edges[i].end->name == in_name) {
			edges.erase(edges.begin() + i);
			i--;
		}
	}
}

Graph::Graph() {
	nextNodeName = 1;
}

Graph::~Graph() {
	for (int i = 0; i < nodes.size(); i++) {
		delete nodes[i];
	}
}

int Graph::AddNode() {
	nodes.emplace_back(new GrNode(nextNodeName));
	nextNodeName++;
	return nodes[nodes.size() - 1]->name;
}

int Graph::AddNode(float in_x, float in_y) {
	nodes.emplace_back(new GrNode(nextNodeName));
	nodes.back()->posX = in_x;
	nodes.back()->posY = in_y;
	nextNodeName++;
	return nodes[nodes.size() - 1]->name;
}

void Graph::RemoveNode(int in_name) {
	for (int i = 0; i < nodes.size(); i++) {
		nodes[i]->RemoveEdgesTo(in_name);
	}
	for (int i = 0; i < nodes.size(); i++) {
		if (nodes[i]->name == in_name) {
			delete nodes[i];
			nodes.erase(nodes.begin() + i);
		}
	}
}

void Graph::SetNodePos(int in_name, float in_x, float in_y){
	GrNode* target = FindNodeByName(in_name);
	target->posX = in_x;
	target->posY = in_y;
}

void Graph::GetNodePos(int in_name, float& in_x, float& in_y) {
	GrNode* target = FindNodeByName(in_name);
	in_x = target->posX;
	in_y = target->posY;
}

void Graph::AddEdge(int in_name_from, int in_name_to, float in_cost) {
	GrNode* source = FindNodeByName(in_name_from);
	GrNode* target = FindNodeByName(in_name_to);

	if (source != nullptr && target != nullptr) {
		GrEdge newEdge;
		newEdge.end = target;
		newEdge.cost = in_cost;

		source->edges.emplace_back(newEdge);
	}
}

void Graph::AddConnections(int in_name_end1, int in_name_end2, float in_cost) {
	GrNode* end1 = FindNodeByName(in_name_end1);
	GrNode* end2 = FindNodeByName(in_name_end2);

	if (end1 != nullptr && end2 != nullptr) {
		GrEdge newEdge;
		newEdge.cost = in_cost;

		newEdge.end = end1;
		end2->edges.emplace_back(newEdge);

		newEdge.end = end2;
		end1->edges.emplace_back(newEdge);
	}
}

void Graph::RemoveEdge(int in_name_from, int in_name_to) {
	GrNode* source = FindNodeByName(in_name_from);

	if (source != nullptr) {
		source->RemoveEdgesTo(in_name_to);
	}
}
void Graph::RemoveConections(int in_name_end1, int in_name_end2) {
	GrNode* end1 = FindNodeByName(in_name_end1);
	GrNode* end2 = FindNodeByName(in_name_end2);

	if (end1 != nullptr) {
		end1->RemoveEdgesTo(in_name_end2);
	}
	if (end2 != nullptr) {
		end2->RemoveEdgesTo(in_name_end1);
	}
}
void Graph::ClearEdges(int in_name) {
	GrNode* target = FindNodeByName(in_name);

	if (target != nullptr) {
		target->edges.clear();
	}
}

GrNode* Graph::FindNodeByName(int in_name) {
	for (int i = 0; i < nodes.size(); i++) {
		if (nodes[i]->name == in_name) {
			return nodes[i];
		}
	}
	return nullptr;
}

void Graph::ResetVisited() {
	for (int i = 0; i < nodes.size(); i++) {
		nodes[i]->visited = false;
	}
}

std::vector<int> Graph::GetNames() {
	std::vector<int>out;
	for (int i = 0; i < nodes.size(); i++) {
		out.emplace_back(nodes[i]->name);
	}
	return out;
}

bool Graph::IsConnectedDFS(int in_name_start, int in_name_end) {
	ResetVisited();
	GrNode* start = FindNodeByName(in_name_start);
	GrNode* end = FindNodeByName(in_name_end);

	std::stack<GrNode*> nodeStack = std::stack<GrNode*>();
	nodeStack.push(start);

	while (!nodeStack.empty())
	{
		GrNode* current = nodeStack.top();
		nodeStack.pop();

		if (current->visited == true)
		{
			continue;
		}

		current->visited = true;

		if (current == end)
		{
			return true;
		}

		for (int i = 0; i < current->edges.size(); ++i) {
			nodeStack.push(current->edges[i].end);
		}
	}

	return false;
}

int Graph::NodeDistanceDFS(int in_name_start, int in_name_end) {
	ResetVisited();
	GrNode* end = FindNodeByName(in_name_end);

	std::stack<GrNode*> nodeStack = std::stack<GrNode*>();
	nodeStack.push(FindNodeByName(in_name_start));

	std::vector<GrNode*> path = std::vector<GrNode*>();

	while (!nodeStack.empty())
	{
		GrNode* current = nodeStack.top();
		path.emplace_back(current);
		nodeStack.pop();

		if (current->visited == true)
		{
			continue;
		}

		current->visited = true;

		if (current == end)
		{
			return path.size() - 1;
		}

		if (current->edges.size() > 0) {
			for (int i = 0; i < current->edges.size(); ++i) {
				nodeStack.push(current->edges[i].end);
			}
		} else {
			path.erase(path.end());
		}

		for (int i = path.size() - 1; i >= 0; i--) {
			bool UnvisitedNodes = false;
			for (int j = 0; j < path[i]->edges.size(); j++) {
				if (path[i]->edges[j].end->visited == false) {
					UnvisitedNodes = true;
				}
			}
			if (!UnvisitedNodes) {
				path.erase(path.begin() + i);
			}
		}
	}

	return -1;
}
int Graph::TraverseCostDFS(int in_name_start, int in_name_end) {
	ResetVisited();
	GrNode* end = FindNodeByName(in_name_end);

	std::stack<GrNode*> nodeStack = std::stack<GrNode*>();
	nodeStack.push(FindNodeByName(in_name_start));

	std::vector<GrNode*> path = std::vector<GrNode*>();

	while (!nodeStack.empty())
	{
		GrNode* current = nodeStack.top();
		path.emplace_back(current);
		nodeStack.pop();

		if (current->visited == true)
		{
			continue;
		}

		current->visited = true;

		if (current == end)
		{
			int cost = 0;
			for (int i = 0; i < path.size() - 1; i++) {
				int nextEdgeCost;
				for (int j = 0; j < path[i]->edges.size(); j++) {
					if (path[i]->edges[j].end == path[i + 1]) {
						nextEdgeCost = path[i]->edges[j].cost;
					}
				}

				cost += 1 + nextEdgeCost;
			}
			return cost;
		}

		if (current->edges.size() > 0) {
			for (int i = 0; i < current->edges.size(); ++i) {
				nodeStack.push(current->edges[i].end);
			}
		}
		else {
			path.erase(path.end());
		}

		for (int i = path.size() - 1; i >= 0; i--) {
			bool UnvisitedNodes = false;
			for (int j = 0; j < path[i]->edges.size(); j++) {
				if (path[i]->edges[j].end->visited == false) {
					UnvisitedNodes = true;
				}
			}
			if (!UnvisitedNodes) {
				path.erase(path.begin() + i);
			}
		}
	}

	return -1;
}

void Graph::CreateGrid(int in_nodesWide, int in_nodesTall, float in_width, float in_height) {
	nodes.clear();
	for (float i = 0; i < in_nodesWide; i++) {
		for (float j = 0; j < in_nodesTall; j++) {
			AddNode(i / in_nodesWide * in_width, j / in_nodesTall * in_height);
		}
	}

	for (float i = 0; i < in_nodesWide; i++) {
		for (float j = 0; j < in_nodesTall; j++) {

			if (i > 0) { //left one
				AddEdge(NearestNode(i / in_nodesWide * in_width, j / in_nodesTall * in_height), NearestNode((i - 1) / in_nodesWide * in_width, j / in_nodesTall * in_height));
			}
			if (j > 0) {//up one
				AddEdge(NearestNode(i / in_nodesWide * in_width, j / in_nodesTall * in_height), NearestNode(i / in_nodesWide * in_width, (j - 1) / in_nodesTall * in_height));
			}
			
			if (i < in_nodesWide - 1) {//right one
				AddEdge(NearestNode(i / in_nodesWide * in_width, j / in_nodesTall * in_height), NearestNode((i + 1) / in_nodesWide * in_width, j / in_nodesTall * in_height));
			}
			if (j < in_nodesTall - 1) {//down one
				AddEdge(NearestNode(i / in_nodesWide * in_width, j / in_nodesTall * in_height), NearestNode(i / in_nodesWide * in_width, (j + 1) / in_nodesTall * in_height));
			}

			if (i > 0 && j > 0) {//up and left one
				AddEdge(NearestNode(i / in_nodesWide * in_width, j / in_nodesTall * in_height), NearestNode((i - 1) / in_nodesWide * in_width, (j - 1) / in_nodesTall * in_height));
			}
			if (i < in_nodesWide - 1 && j > 0) {//up and right one
				AddEdge(NearestNode(i / in_nodesWide * in_width, j / in_nodesTall * in_height), NearestNode((i + 1) / in_nodesWide * in_width, (j - 1) / in_nodesTall * in_height));
			}

			if (i > 0 && j < in_nodesTall - 1) {//down and left one
				AddEdge(NearestNode(i / in_nodesWide * in_width, j / in_nodesTall * in_height), NearestNode((i - 1) / in_nodesWide * in_width, (j + 1) / in_nodesTall * in_height));
			}
			if (i < in_nodesWide - 1 && j < in_nodesTall - 1) {//down and right one
				AddEdge(NearestNode(i / in_nodesWide * in_width, j / in_nodesTall * in_height), NearestNode((i + 1) / in_nodesWide * in_width, (j + 1) / in_nodesTall * in_height));
			}

		}
	}
}

int Graph::NearestNode(float in_x, float in_y) {
	int closestNode = -1;
	float shortestDistance = FLT_MAX;
	for (int i = 0; i < nodes.size(); i++) {
		float currentValue = std::abs(nodes[i]->posX - in_x) + std::abs(nodes[i]->posY - in_y);
		if (currentValue < shortestDistance) {
			shortestDistance = currentValue;
			closestNode = nodes[i]->name;
		}
	}

	return closestNode;
}


std::vector<int> Graph::FindPath(int in_name_start, int in_name_end) {
	PreparePathfind();
	ResetVisited();
	GrNode* start = FindNodeByName(in_name_start);
	GrNode* end = FindNodeByName(in_name_end);

	std::queue<GrNode*> priorityQueue = std::queue<GrNode*>();
	priorityQueue.push(start);
	start->previousNode = start;
	start->gScore = 0;

	while (!priorityQueue.empty()) {
		//get current node off the queue
		GrNode* current = priorityQueue.front();
		//remove it frome queue
		priorityQueue.pop();

		if (!current->visited) {

			//mark it as visited
			current->visited = true;

			//loop through edges
			for (int i = 0; i < current->edges.size(); i++) {

				//get cost to traverse
				int cost = current->gScore + current->edges[i].cost + 1;

				//calculate heuristic (by distance from target)
				int heuristic = (std::abs(current->edges[i].end->posX - end->posX) + std::abs(current->edges[i].end->posY - end->posY)) / 100;

				cost += heuristic;

				//if the cost calculated is less that the end's current cost:
				if (cost < current->edges[i].end->gScore /*+ heuristic*/) {

					//set it's previousNode to the current one
					current->edges[i].end->previousNode = current;

					//set it's gScore to the cost calculated
					current->edges[i].end->gScore = cost;

					//if it hasent been traversed add it the the queue
					if (current->edges[i].end->visited != true) {
						priorityQueue.push(current->edges[i].end);
					}//if not visited

				}//if traverse cost is cheaper
			}//for loop

		}//if not visited

	}//while loop

	//make output vector
	std::vector<int>out = std::vector<int>();

	//check to see if we actually found a path
	if (end->previousNode != nullptr) {
		//add the end point to list
		out.emplace_back(end->name);
		//prepare variable for while loop
		GrNode* next = end->previousNode;

		//run while loop until we find a node who has itself as it's previous node (the starting node)
		while (next->previousNode != next && end->name != start->name) {

			//add current node to list
			out.emplace(out.begin(), next->name);
			//move to next node
			next = next->previousNode;
		}
		//add the first node to list
		out.emplace(out.begin(), next->name);
	} else {
		//path not foud output error code
		out.emplace_back(-1);
	}

	return out;
}

void Graph::PreparePathfind() {
	for (int i = 0; i < nodes.size(); i++) {
		nodes[i]->previousNode = nullptr;
		nodes[i]->gScore = INT_MAX - 300000;
	}
}

std::vector<int> Graph::GetNodesConectedTo(int in_name) {
	GrNode* node = FindNodeByName(in_name);

	std::vector<int>out = std::vector<int>();

	for (int i = 0; i < node->edges.size(); i++) {
		out.emplace_back(node->edges[i].end->name);
	}

	return out;
}

std::ostream& operator<<(std::ostream& stream, Graph& graph) {
	//post graph
	stream << "Graph: {\n";
	//post all nodes
	for (int i = 0; i < graph.nodes.size(); i++) {
		//post node name
		stream << (*graph.nodes[i]);

		//check if there are any edges
		if (graph.nodes[i]->edges.size() > 0) {
			//post list of edges
			stream << "-> ";

			//post all edges
			for (int j = 0; j < graph.nodes[i]->edges.size(); j++) {
				//post edge
				stream << " " << graph.nodes[i]->edges[j] << " |";
			}

		} else {
			//show there are no edges
			stream << ";";
		}
		//end line for node
		stream << std::endl;
	}
	//close graph
	stream << "}\n";
	//return
	return stream;
}

std::ostream& operator<<(std::ostream& stream, GrNode& grNode) {
	stream << grNode.name << "(" << grNode.posX << ", "<< grNode.posY << ")";
	return stream;
}
std::ostream& operator<<(std::ostream& stream, GrEdge& grEdge) {
	stream << "T:" << grEdge.end->name << " C:" << grEdge.cost;
	return stream;
}
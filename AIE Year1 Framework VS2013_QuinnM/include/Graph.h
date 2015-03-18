#ifndef _Graph_h_
#define _Graph_h_

#include <iostream>
#include <vector>
#include <stack>
#include <queue>

class GrNode;

struct GrEdge {
	GrNode* end;

	float cost;
};

class GrNode {
public:
	GrNode(int in_name);
	GrNode(int in_name, float in_x, float in_y);
	~GrNode();

	void RemoveEdgesTo(int in_name);

	int name;

	float posX;
	float posY;

	bool visited;

	//dijkstra's algorithm
	GrNode* previousNode;
	int gScore;

	std::vector<GrEdge>edges;
};

class Graph {
public:
	Graph();
	~Graph();

	int AddNode();//returns node name;
	int AddNode(float in_x, float in_y);//returns node name;
	void RemoveNode(int in_name);
	void SetNodePos(int in_name, float in_x, float in_y);
	void GetNodePos(int in_name, float& in_x, float& in_y);
	std::vector<int> GetNodesConectedTo(int in_name);//returns list of node names

	int NearestNode(float in_x, float in_y);//returns node name

	void AddEdge(int in_name_from, int in_name_to, float in_cost = 0);
	void AddConnections(int in_name_end1, int in_name_end2, float in_cost = 0);

	void RemoveEdge(int in_name_from, int in_name_to);
	void RemoveConections(int in_name_end1, int in_name_end2);
	void ClearEdges(int in_name);

	bool IsConnectedDFS(int in_name_start, int in_name_end);
	int NodeDistanceDFS(int in_name_start, int in_name_end);
	int TraverseCostDFS(int in_name_start, int in_name_end);

	std::vector<int> FindPath(int in_name_start, int in_name_end);

	void CreateGrid(int in_nodesWide, int in_nodesTall, float in_width, float in_height);

	std::vector<int> GetNames();

	friend std::ostream& operator<<(std::ostream& stream, Graph& graph);

private:
	GrNode* FindNodeByName(int in_name);
	void ResetVisited();
	void PreparePathfind();

	int nextNodeName;
	std::vector<GrNode*>nodes;
};

std::ostream& operator<<(std::ostream& stream, GrNode& grNode);
std::ostream& operator<<(std::ostream& stream, GrEdge& grEdge);

#endif
#include "AIE.h"
#include <iostream>
#include <vector>
#include "Agent.h"
#include "Wall.h"
#include "Graph.h"

using namespace std;

//prototypes
void StartUp();
void Update();
void Draw();

void Collide();
void DrawGraph();
void AddAgent();//at mouse pos
void RemoveEntity();//nearest to mouse pos
void RemoveAgent();//nearest to mouse pos
void AddWall();//at mouse pos
void RemoveWall();//nearest to mouse pos

Entity* FindClosestEntity();//to mouse pos
Agent* FindClosestAgent();//to mouse pos
Wall* FindClosestWall();//to mouse pos

void AddBehavior();

//constant vars
const int SCREEN_MAX_X = 900, SCREEN_MAX_Y = 600;
const float FRAME_LIMIT = 1.0f / 60.0f;

//entity container
vector<Entity*>entities = vector<Entity*>();
//graph vars
Graph paths = Graph();
vector<int> graphNames = paths.GetNames();
//graph drawing
unsigned int graphSprite = 0;

//UI vars
bool inputDown = false;
bool paused = false;
bool drawGraph = false;

Agent* selected1 = nullptr;
Agent* selected2 = nullptr;
Point waypoint = Point(-1, -1);

enum SelBehav {
	SelNone,
	SelPursue,
	SelEvade,
	SelWander,
	SelFlock,
	SelGoTo,
	SelClear
};

SelBehav selectedB = SelNone;

int main( int argc, char* argv[] )
{	
	Initialise(SCREEN_MAX_X, SCREEN_MAX_Y, false, "AI Project");
    
    SetBackgroundColour(SColour(0, 0, 0, 255));

	StartUp();

	float time = 0.0f;

    //Game Loop
    do
	{

		time += GetDeltaTime();
		if (time >= FRAME_LIMIT) {
			time -= FRAME_LIMIT;

			Update();
		}

		Draw();

    } while(!FrameworkUpdate());

    Shutdown();

    return 0;
}

void StartUp() {
	//add graph nodes
	paths.CreateGrid(7, 5, SCREEN_MAX_X, SCREEN_MAX_Y, ((SCREEN_MAX_X / 7) / 2), ((SCREEN_MAX_Y / 5) / 2));

	//set Graph sprite
	graphSprite = CreateSprite("images/invaders/invaders_7_01.png", 10, 10, true);
	//sync graph name vector
	graphNames = paths.GetNames();
}

void Update() {
	//input handeling
	if (IsKeyDown('P')) {
		if (!inputDown) {//pause 
			paused = !paused;
			inputDown = true;
		}
	} else if (IsKeyDown('V')) {
		if (!inputDown) {//show velocities
			Agent::ToggleVelocityLine();
			inputDown = true;
		}
	} else if (IsKeyDown('G')) {
		if (!inputDown) {//show graph lines
			drawGraph = !drawGraph;
			inputDown = true;
		}
	} else if (IsKeyDown('A')) {
		if (!inputDown) {//create agent at mouse pos
			AddAgent();
			inputDown = true;
		}
	} else if (IsKeyDown('Z')) {
		if (!inputDown) {//delete agent nearest to mouse pos
			RemoveAgent();
			inputDown = true;
		}
	} else if (IsKeyDown('S')) {
		if (!inputDown) {//add wall at graphnode closest to mouse
			AddWall();
			inputDown = true;
		}
	} else if (IsKeyDown('X')) {
		if (!inputDown) {//delete wall closest to mouse
			RemoveWall();
			inputDown = true;
		}
	} else if (GetMouseButtonDown(MOUSE_BUTTON_1)) {//select agent nearest to the mouse as RED
		if (!inputDown) {
			Agent* newSelection = FindClosestAgent();
			if (newSelection == selected1) {
				selected1 = nullptr;
			} else {
				if (newSelection == selected2) {
					selected2 = nullptr;
				}

				selected1 = newSelection;
			}
			inputDown = true;
		}
	} else if (GetMouseButtonDown(MOUSE_BUTTON_2)) {//select agent nearest to the mouse as BLUE
		if (!inputDown) {
			double mouseX, mouseY;
			GetMouseLocation(mouseX, mouseY);

			float selectionDist = 0;

			if (waypoint.x != -1) {//if we have a waypoint
				selectionDist = std::sqrt(std::pow(mouseX - waypoint.x, 2) + std::pow((SCREEN_MAX_Y - mouseY) - waypoint.y, 2));

				if (selectionDist < 30) {//if our new selection is very close
					waypoint = Point(-1, -1);//clear the waypoint
				} else {
					Agent* newSelection = FindClosestAgent();//get the closest agent

					selectionDist = std::sqrt(std::pow(mouseX - newSelection->position.x, 2) + std::pow((SCREEN_MAX_Y - mouseY) - newSelection->position.y, 2));

					if (selectionDist < 30) {//if the closest agent close
						selected2 = newSelection;//set Selected2
						waypoint = Point(-1, -1);//clear the waypoint
					} else {
						waypoint = Point(mouseX, SCREEN_MAX_Y - mouseY);//move the waypoint
					}
				}
			} else {//if we don't have a waypoint

				Agent* newSelection = FindClosestAgent();//get the closest agent

				if (newSelection != nullptr) {
					selectionDist = std::sqrt(std::pow(mouseX - newSelection->position.x, 2) + std::pow((SCREEN_MAX_Y - mouseY) - newSelection->position.y, 2));

					if (selectionDist > 30) {//if the closest agent is far away
						waypoint = Point(mouseX, SCREEN_MAX_Y - mouseY);//set a waypoint 
						selected2 = nullptr;//clear out selected2
					} else {//if the closest is close
						if (newSelection == selected2) {//clear out if it's alredy selected
							selected2 = nullptr;
						} else {
							if (newSelection == selected1) {//clear selected1 if it is the same one
								selected1 = nullptr;
							}

							selected2 = newSelection;//set our selection
						}
					}
				} else {
					waypoint = Point(mouseX, SCREEN_MAX_Y - mouseY);
				}
			}
			inputDown = true;
		}
	} else if (IsKeyDown('1')) {//set selected behavior to Pursue
		if (!inputDown) {
			selectedB = SelPursue;
			inputDown = true;
		}
	} else if (IsKeyDown('2')) {//set selected behavior to Evade
		if (!inputDown) {
			selectedB = SelEvade;
			inputDown = true;
		}
	} else if (IsKeyDown('3')) {//set selected behavior to Wander (clears BLUE because it is unessisary)
		if (!inputDown) {
			selectedB = SelWander;
			selected2 = nullptr;
			inputDown = true;
		}
	} else if (IsKeyDown('4')) {//set selected behavior to Flock (clears BLUE because it is unessisary)
		if (!inputDown) {
			selectedB = SelFlock;
			selected2 = nullptr;
			inputDown = true;
		}
	} else if (IsKeyDown('0')) {//set selected behavior to none (will not add a behavior)
		if (!inputDown) {
			selectedB = SelNone;
			inputDown = true;
		}
	} else if (IsKeyDown('C')) {//set selected behavior to clear all behaviors
		if (!inputDown) {
			selectedB = SelClear;
			selected2 = nullptr;
			inputDown = true;
		}
	} else if (IsKeyDown(' ')) {//finalizes selected behavior
		if (!inputDown) {
			AddBehavior();
			inputDown = true;
		}
	} else {
		inputDown = false;
	}

	if (waypoint.x != -1) {
		if (selectedB != SelClear) {
			selectedB = SelGoTo;
		}
	} else if (selectedB == SelGoTo) {
		selectedB = SelNone;
	}

	//apply updates
	if (!paused) {
		
		for (int i = 0; i < entities.size(); i++) {
			entities[i]->Update();
		}
		Collide();
	}
}

void Draw() {
	ClearScreen();

	//draw entities
	for (int i = 0; i < entities.size(); i++) {
		entities[i]->Draw();
	}

	//highlight selections
	if (selected1 != nullptr) {
		DrawLine(selected1->position.x - (selected1->GetWidth() / 2), selected1->position.y - (selected1->GetHeight() / 2), selected1->position.x + (selected1->GetWidth() / 2), selected1->position.y - (selected1->GetHeight() / 2), SColour(255, 0, 0, 255));
		DrawString("Red", 50, 50, SColour(255, 0, 0, 255));
	}

	if (selected2 != nullptr) {
		DrawLine(selected2->position.x - (selected2->GetWidth() / 2), selected2->position.y - (selected2->GetHeight() / 2), selected2->position.x + (selected2->GetWidth() / 2), selected2->position.y - (selected2->GetHeight() / 2), SColour(0, 0, 255, 255));
	}

	if (waypoint.x != -1) {
		DrawLine(waypoint.x - 10, waypoint.y - 10, waypoint.x + 10, waypoint.y + 10, SColour(0, 0, 255, 255));
		DrawLine(waypoint.x + 10, waypoint.y - 10, waypoint.x - 10, waypoint.y + 10, SColour(0, 0, 255, 255));
	}

	if (selected2 != nullptr || waypoint.x != -1) {
		DrawString("Blue", 220, 50, SColour(0, 0, 255, 255));
	}

	//show behavior Type
	switch (selectedB)
	{
	case SelNone:
		break;
	case SelPursue:
		DrawString("Pursues", 110, 50);
		break;
	case SelEvade:
		DrawString("Evades", 110, 50);
		break;
	case SelWander:
		DrawString("Wanders", 110, 50);
		break;
	case SelFlock:
		DrawString("Flocks", 110, 50);
		break;
	case SelClear:
		DrawString("Clear All", 110, 50);
		break;
	case SelGoTo:
		DrawString("Go To", 110, 50);
		break;
	default:
		break;
		
	}

	//draw graph stuff
	if (drawGraph) {
		DrawGraph();
	}
}

void Collide() {
	//collide
	for (int i = 0; i < entities.size(); i++) {
		Agent* currentAgent = dynamic_cast<Agent*>(entities[i]);

		for (int j = 0; j < entities.size(); j++) {
			Wall* currentWall = dynamic_cast<Wall*>(entities[j]);
			if (currentWall != nullptr && currentAgent != nullptr) {//if the other item is a Wall

				if (currentAgent->Collide(*currentWall)) {//if we're inside the wall
					
					if (std::abs(currentAgent->GetVelocity().x) > std::abs(currentAgent->GetVelocity().y)) {//if moving more sideways than verticaly
						if (currentAgent->GetVelocity().x > 0) {//if moving right
							currentAgent->position.x = currentWall->position.x - ((currentWall->GetWidth() / 2) + (currentAgent->GetWidth() / 2));
						} else { //moveing left
							currentAgent->position.x = currentWall->position.x + ((currentWall->GetWidth() / 2) + (currentAgent->GetWidth() / 2));
						}
						Point invertedVel = currentAgent->GetVelocity();
						invertedVel.x *= -1;
						currentAgent->SetForce(invertedVel);
					} else {//if moving more veritcally
						if (currentAgent->GetVelocity().y > 0) {//if moving up
							currentAgent->position.y = currentWall->position.y - ((currentWall->GetHeight() / 2) + (currentAgent->GetHeight() / 2));
						} else {//moving down
							currentAgent->position.y = currentWall->position.y + ((currentWall->GetHeight() / 2) + (currentAgent->GetHeight() / 2));
						}
						Point invertedVel = currentAgent->GetVelocity();
						invertedVel.y *= -1;
						currentAgent->SetForce(invertedVel);
					}

				}

			}
		}
	}
}

void DrawGraph() {
	float x, y, x2, y2;
	for (int i = 0; i < graphNames.size(); i++) {
		paths.GetNodePos(graphNames[i], x, y);
		MoveSprite(graphSprite, x, y);
		DrawSprite(graphSprite);//draw the node itself
		vector<int> links = paths.GetNodesConectedTo(graphNames[i]);

		for (int j = 0; j < links.size(); j++) {
			paths.GetNodePos(links[j], x2, y2);
			if (paths.CanTraverse(graphNames[i]) && paths.CanTraverse(links[j])) {//draw the lines conecting 
				DrawLine(x, y, x2, y2, SColour(0, 50, 0, 255));
			} else {
				DrawLine(x, y, x2, y2, SColour(50, 0, 0, 255));
			}
		}
	}
}

void AddAgent() {
	double mouseX, mouseY;
	GetMouseLocation(mouseX, mouseY);

	Agent* newAgent = new Agent(mouseX, SCREEN_MAX_Y - mouseY);
	newAgent->SetSpeedCap(10);
	newAgent->ToggleDrag();

	entities.emplace_back(newAgent);
}

void RemoveEntity() {
	if (entities.size() > 0) {
		Entity* entityToRemove = FindClosestEntity();
		for (int i = 0; i < entities.size(); i++) {
			if (entities[i] == entityToRemove) {
				entities.erase(entities.begin() + i);
				delete entityToRemove;
				break;
			}
		}
	}

}

void RemoveAgent() {
	if (entities.size() > 0) {
		Entity* agentToRemove = FindClosestAgent();
		for (int i = 0; i < entities.size(); i++) {
			if (entities[i] == agentToRemove) {
				entities.erase(entities.begin() + i);
				if (agentToRemove == selected1) {
					selected1 = nullptr;
				}
				if (agentToRemove == selected2) {
					selected2 = nullptr;
				}
				delete agentToRemove;
				break;
			}
		}
	}
}

void AddWall() {
	double mouseX, mouseY;
	GetMouseLocation(mouseX, mouseY);

	int nearestNodeName = paths.NearestNode(mouseX, SCREEN_MAX_Y - mouseY);//find nearest graph node

	if (paths.CanTraverse(nearestNodeName)) {//check if there is alredy a wall on that node 
		float x, y;
		paths.GetNodePos(nearestNodeName, x, y);//get actual node pos
		paths.ToggleNode(nearestNodeName);

		entities.emplace_back(new Wall(x, y, (SCREEN_MAX_X / 7), (SCREEN_MAX_Y / 5)));//add wall
	}
}

void RemoveWall() {
	if (entities.size() > 0) {
		Wall* nearestWall = FindClosestWall();
		if (nearestWall != nullptr) {
			int nearestNode = paths.NearestNode(nearestWall->position.x, nearestWall->position.y);//find node so we can re-activate it for pathfinding

			paths.ToggleNode(nearestNode);

			for (int i = 0; i < entities.size(); i++) {
				if (entities[i] == nearestWall) {
					entities.erase(entities.begin() + i);
					delete nearestWall;
					break;
				}
			}
		}
	}
}

Entity* FindClosestEntity() {
	double mouseX, mouseY;
	GetMouseLocation(mouseX, mouseY);

	Entity* nearest = nullptr;
	float nearestDistance = FLT_MAX;
	for (int i = 0; i < entities.size(); i++) {
		float currentDistance = std::sqrt((mouseX * entities[i]->position.x) + (mouseY * entities[i]->position.y));
		if (currentDistance < nearestDistance) {
			nearest = entities[i];
			nearestDistance = currentDistance;
		}
	}

	return nearest;
}

Agent* FindClosestAgent() {
	double mouseX, mouseY;
	GetMouseLocation(mouseX, mouseY);

	Agent* nearest = nullptr;
	float nearestDistance = FLT_MAX;
	for (int i = 0; i < entities.size(); i++) {
		Agent* current = dynamic_cast<Agent*>(entities[i]);
		if (current != nullptr) {
			float currentDistance = std::sqrt(std::pow(mouseX - entities[i]->position.x, 2) + std::pow((SCREEN_MAX_Y - mouseY) - entities[i]->position.y, 2));
			if (currentDistance < nearestDistance) {
				nearest = current;
				nearestDistance = currentDistance;
			}
		}
	}

	return nearest;
}

Wall* FindClosestWall() {
	double mouseX, mouseY;
	GetMouseLocation(mouseX, mouseY);

	Wall* nearest = nullptr;
	float nearestDistance = FLT_MAX;
	for (int i = 0; i < entities.size(); i++) {
		Wall* current = dynamic_cast<Wall*>(entities[i]);
		if (current != nullptr) {
			float currentDistance = std::sqrt(std::pow(mouseX - entities[i]->position.x, 2) + std::pow((SCREEN_MAX_Y - mouseY) - entities[i]->position.y, 2));
			if (currentDistance < nearestDistance) {
				nearest = current;
				nearestDistance = currentDistance;
			}
		}
	}

	return nearest;
}

void AddBehavior() {
	if (selected1 != nullptr) {
		switch (selectedB)
		{
		case SelNone:
			break;
		case SelPursue:
			if (selected2 != nullptr) {
				selected1->AddPursue(selected2, 1);
			}
			break;
		case SelEvade:
			if (selected2 != nullptr) {
				selected1->AddEvade(selected2, 1);
			}
			break;
		case SelWander:
			selected1->AddWander(20, 10, 1, 1);
			break;
		case SelFlock:
			selected1->AddToFlock(1);
			break;
		case SelClear:
			selected1->ClearBehaviors();
			selected1->SetForce(Point(0, 0));
			break;
		case SelGoTo:
			selected1->GoTo(waypoint, 1);
		default:
			break;

		}
	}
}
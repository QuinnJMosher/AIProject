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

enum SelBehav {
	SelNone,
	SelPursue,
	SelEvade,
	SelWander,
	SelFlock,
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
		if (!inputDown) {
			paused = !paused;
			inputDown = true;
		}
	} else if (IsKeyDown('V')) {
		if (!inputDown) {
			Agent::ToggleVelocityLine();
			inputDown = true;
		}
	} else if (IsKeyDown('G')) {
		if (!inputDown) {
			drawGraph = !drawGraph;
			inputDown = true;
		}
	} else if (IsKeyDown('A')) {
		if (!inputDown) {
			AddAgent();
			inputDown = true;
		}
	} else if (IsKeyDown('Z')) {
		if (!inputDown) {
			RemoveAgent();
			inputDown = true;
		}
	} else if (IsKeyDown('S')) {
		if (!inputDown) {
			AddWall();
			inputDown = true;
		}
	} else if (IsKeyDown('X')) {
		if (!inputDown) {
			RemoveWall();
			inputDown = true;
		}
	} else if (GetMouseButtonDown(MOUSE_BUTTON_1)) {
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
	} else if (GetMouseButtonDown(MOUSE_BUTTON_2)) {
		if (!inputDown) {
			Agent* newSelection = FindClosestAgent();
			if (newSelection == selected2) {
				selected2 = nullptr;
			}
			else {
				if (newSelection == selected1) {
					selected1 = nullptr;
				}

				selected2 = newSelection;
			}
			inputDown = true;
		}
	} else if (IsKeyDown('1')) {
		if (!inputDown) {
			selectedB = SelPursue;
			inputDown = true;
		}
	} else if (IsKeyDown('2')) {
		if (!inputDown) {
			selectedB = SelEvade;
			inputDown = true;
		}
	} else if (IsKeyDown('3')) {
		if (!inputDown) {
			selectedB = SelWander;
			selected2 = nullptr;
			inputDown = true;
		}
	} else if (IsKeyDown('4')) {
		if (!inputDown) {
			selectedB = SelFlock;
			selected2 = nullptr;
			inputDown = true;
		}
	} else if (IsKeyDown('0')) {
		if (!inputDown) {
			selectedB = SelNone;
			inputDown = true;
		}
	} else if (IsKeyDown('C')) {
		if (!inputDown) {
			selectedB = SelClear;
			selected2 = nullptr;
			inputDown = true;
		}
	} else if (IsKeyDown(' ')) {
		if (!inputDown) {
			AddBehavior();
			inputDown = true;
		}
	} else {
		inputDown = false;
	}

	//apply updates
	if (!paused) {
		for (int i = 0; i < entities.size(); i++) {
			entities[i]->Update();
		}
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
	default:
		break;
		
	}

	//draw graph stuff
	if (drawGraph) {
		DrawGraph();
	}
}

void DrawGraph() {
	float x, y, x2, y2;
	for (int i = 0; i < graphNames.size(); i++) {
		paths.GetNodePos(graphNames[i], x, y);
		MoveSprite(graphSprite, x, y);
		DrawSprite(graphSprite);
		vector<int> links = paths.GetNodesConectedTo(graphNames[i]);

		for (int j = 0; j < links.size(); j++) {
			paths.GetNodePos(links[j], x2, y2);
			if (paths.CanTraverse(graphNames[i]) && paths.CanTraverse(links[j])) {
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

	int nearestNodeName = paths.NearestNode(mouseX, SCREEN_MAX_Y - mouseY);

	if (paths.CanTraverse(nearestNodeName)) {
		float x, y;
		paths.GetNodePos(nearestNodeName, x, y);
		paths.ToggleNode(nearestNodeName);

		entities.emplace_back(new Wall(x, y, (SCREEN_MAX_X / 7), (SCREEN_MAX_Y / 5)));
	}
}

void RemoveWall() {
	if (entities.size() > 0) {
		Wall* nearestWall = FindClosestWall();
		if (nearestWall != nullptr) {
			int nearestNode = paths.NearestNode(nearestWall->position.x, nearestWall->position.y);

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
	//show behavior Type
	if (selected1 != nullptr && selectedB != SelNone) {
		selected1->ClearBehaviors();
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
		default:
			break;

		}
	}
}
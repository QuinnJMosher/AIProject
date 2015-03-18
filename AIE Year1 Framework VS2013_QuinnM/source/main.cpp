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

        ClearScreen();

    } while(!FrameworkUpdate());

    Shutdown();

    return 0;
}

void StartUp() {
	//add starting agents
	entities.emplace_back(new Agent(200, 400));
	entities.emplace_back(new Agent(250, 450));

	Agent* agentptr = dynamic_cast<Agent*>(entities[0]);
	agentptr->AddEvade(dynamic_cast<Agent*>(entities[1]), 1.0f);
	agentptr->SetSpeedCap(11);

	agentptr = dynamic_cast<Agent*>(entities[1]);
	agentptr->AddPursue(dynamic_cast<Agent*>(entities[0]), 1.0f);
	agentptr->SetSpeedCap(10);

	//add a wall
	entities.emplace_back(new Wall(450, 300, 40, 40));

	//add graph nodes
	paths.AddNode(50, 50);
	paths.AddNode(40, 300);
	paths.AddEdge(1, 2);

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
	//draw entities
	for (int i = 0; i < entities.size(); i++) {
		entities[i]->Draw();
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
			paths.GetNodePos(links[i], x2, y2);
			DrawLine(x, y, x2, y2, SColour(0, 255, 0, 255));
		}
	}
}
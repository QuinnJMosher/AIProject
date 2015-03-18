#include "Agent.h"

Agent::Agent(float in_startX, float in_startY, float in_maxSpeed) {
	posX = in_startX;
	posY = in_startY;

	maxSpeed = in_maxSpeed;

	textureId = CreateSprite("images/invaders/invaders_1_00.png", 40, 40, true);

	pathfindingNodes = nullptr;
	walls = nullptr;
	float targetX = 0;
	float targetY = 0;
	isMoving = false;
}

Agent::Agent() {
	posX = 0;
	posY = 0;
	
	maxSpeed = 10;

	textureId = CreateSprite("images/invaders/invaders_1_00.png", 40, 40, true);

	pathfindingNodes = nullptr;
	walls = nullptr;
	float targetX = 0;
	float targetY = 0;
	isMoving = false;
}

Agent::~Agent() { }

void Agent::SetGraph(Graph* in_graph) {
	pathfindingNodes = in_graph;
}

void Agent::SetWalls(std::vector<Wall>* in_walls) {
	walls = in_walls;
}

void Agent::GoTo(float in_x, float in_y) {
	if (pathfindingNodes != nullptr) {
		int startNode = pathfindingNodes->NearestNode(posX, posY);
		int endNode = pathfindingNodes->NearestNode(in_x, in_y);

		path = pathfindingNodes->FindPath(startNode, endNode);
	}
	targetX = in_x;
	targetY = in_y;

	SmoothPath();

	isMoving = true;
}

void Agent::SmoothPath() {
	if (walls != nullptr && walls->size() > 0) {

		//check current position against nodes
		while (path.size() > 1) {

			bool hasColided = false;

			for (int box = 0; box < walls->size(); box++) {

				//get node
				float nodeX, nodeY;
				pathfindingNodes->GetNodePos(path[1], nodeX, nodeY);

				//check colide
				if (walls->at(box).IntersectsWith(posX, posY, nodeX, nodeY)) {
					hasColided = true;
					break;
				}

			}

			if (hasColided) {
				break;//if we've colided then we need to advance i
			} else {
				path.erase(path.begin());
			}
		}

		//check target w/ 2nd to last node
		while (path.size() > 1) {

			float node_x, node_y;
			pathfindingNodes->GetNodePos(path[path.size() - 2], node_x, node_y);

			bool hasColided = false;
			//check for any boxes in the way
			for (int box = 0; box < walls->size(); box++) {
				if (walls->at(box).IntersectsWith(node_x, node_y, targetX, targetY)) {
					hasColided = true;
					break;
				}
			}

			if (hasColided) {
				break;
			}
			else {
				path.erase(path.end() - 1);
			}

		}

		//check within the nodes
		if (path.size() > 2) {
			for (int i = 0; i < path.size() - 2; i++) {
				//get position of the start of the line
				float end1_x, end1_y;
				pathfindingNodes->GetNodePos(path[i], end1_x, end1_y);

				//check for unnessesary nodes after current node
				while (path.size() > i + 2) {//change to while loop (we will only need to access i and i + 2)
					float end2_x, end2_y;
					pathfindingNodes->GetNodePos(path[i + 2], end2_x, end2_y);

					bool hasColided = false;
					//check for any boxes in the way
					for (int box = 0; box < walls->size(); box++) {
						if (walls->at(box).IntersectsWith(end1_x, end1_y, end2_x, end2_y)) {
							hasColided = true;
							break;
						}
					}

					if (hasColided) {
						break;//if we've colided then we need to advance i
					}
					else {
						path.erase(path.begin() + (i + 1));
					}

				}
			}
		}

		//check current pos and target nodes
		if (path.size() == 1) {
			bool hasColided = false;

			for (int i = 0; i < walls->size(); i++) {
				if (walls->at(i).IntersectsWith(posX, posY, targetX, targetY)) {
					hasColided = true;
					break;
				}
			}

			if (!hasColided) {
				path.clear();
			}
		}

	} else {
		path.clear();
	}
}

void Agent::Update(float in_deltaTime) {
	
	//if we are moving
	if (isMoving) {
		float tarX, tarY;//create variables for current targets

		if (path.size() > 0) {//if there are any more nodes to hit on the way to our target

			//get the next node's position
			pathfindingNodes->GetNodePos(path[0], tarX, tarY);

			//see if we're allredy at the node
			if ((posX < tarX + 20 && posX > tarX - 20)
			&& (posY < tarY + 20 && posY > tarY - 20)) {

				//if we are then we will move on instead
				path.erase(path.begin());//erase the last node

				//check for more nodes
				if (path.size() > 0) {

					//if we still have node to go to then we'll head there
					pathfindingNodes->GetNodePos(path[0], tarX, tarY);

				} else {

					//if there arn't any then we need to head to our final target
					tarX = targetX;
					tarY = targetY;
				}
			}
		} else {//if we're headed to our final target

			//check if we've arrived at out goal
			if ((posX < targetX + 20 && posX > targetX - 20)
			&& (posY < targetY + 20 && posY > targetY - 20)) {
				//if we have then we are done moving
				isMoving = false;
				//also we arn't headed anywhere so we'll try to move to where we already are
				tarX = targetX;
				tarY = targetY;
			} else {
				//if where arn't at our target yet then we'll keep heading there
				tarX = targetX;
				tarY = targetY;
			}
		}

		//find our direction by calculating the diference between where we are and our goal
		float directionX = posX - tarX;
		float directionY = posY - tarY;

		//find the magnitude of the direction to find the distance
		float distance = sqrt((directionX * directionX) + (directionY * directionY));

		//normalize the direction
		directionX = directionX / distance;
		directionY = directionY / distance;

		//if we are more than one steap away from the goal then move less than one steap
		float currentSpeed;
		if (distance > maxSpeed * in_deltaTime) {
			currentSpeed = maxSpeed * in_deltaTime;
		} else {
			currentSpeed = (maxSpeed * in_deltaTime) - distance;
		}

		//move our position
		posX -= directionX * currentSpeed;
		posY -= directionY * currentSpeed;
	}
}

void Agent::Draw() {
	MoveSprite(textureId, posX, posY);
	DrawSprite(textureId);
}
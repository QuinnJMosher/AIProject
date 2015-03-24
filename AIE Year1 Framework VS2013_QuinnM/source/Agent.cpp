#include "Agent.h"

float const Agent::resistance = 0.06f;
char* const Agent::texture = "images/invaders/invaders_1_00.png";
bool Agent::drawVelocity = false;

unsigned int Agent::sprite = 0;
float const Agent::speedCap = 50;

std::vector<Agent*> Agent::flockingAgents = std::vector<Agent*>();
float Agent::sepatationPow = 0.6;
float Agent::allignmentPow = 0.03;
float Agent::cohesionPow = 0.45;
float Agent::neighbourhoodSize = 70;

Behaviour::Behaviour(BehaviourType in_type, Agent* in_target, float  in_strength) {
	type = in_type;
	target = in_target;
	strength = in_strength;
}

Agent::Agent(float in_x, float in_y) : Entity(in_x, in_y, 20, 20) {
	velocity.x = 0;
	velocity.y = 0;

	drag = true;

	maxVelocity = -1;

	behaiviourArray = std::vector<Behaviour>();

	if (sprite == 0) {
		sprite = CreateSprite(texture, 20, 20, true);
	}
}

Agent::~Agent() { } 

void Agent::SetSpeedCap(float in_speedCap) {
	maxVelocity = in_speedCap;
}

void Agent::ClearBehaviors() {
	behaiviourArray.clear();
}

void Agent::AddPursue(Agent* in_target, float in_strength) {
	behaiviourArray.emplace_back(Behaviour(Pursue, in_target, in_strength));
}

void Agent::AddEvade(Agent* in_target, float in_strength) {
	behaiviourArray.emplace_back(Behaviour(Evade, in_target, in_strength));
}

void Agent::AddWander(float in_circDist, float in_circRadius, float in_jitter, float in_strength) {
	circDist = in_circDist;
	circRadius = in_circRadius;
	jitter = in_jitter;
	wanderPoint = 1;
	for (int i = 0; i < behaiviourArray.size(); i++) {
		if (behaiviourArray[i].type == Wander) {
			behaiviourArray[i].strength = in_strength;
			return;
		}
	}

	behaiviourArray.emplace_back(Behaviour(Wander, nullptr, in_strength));
}

void Agent::RemovePursue(Agent* in_target) {
	for (int i = 0; i < behaiviourArray.size(); i++) {
		if (behaiviourArray[i].target == in_target && behaiviourArray[i].type == Pursue) {
			behaiviourArray.erase(behaiviourArray.begin() + i);
			break;
		}
	}
}

void Agent::RemoveEvade(Agent* in_target) {
	for (int i = 0; i < behaiviourArray.size(); i++) {
		if (behaiviourArray[i].target == in_target && behaiviourArray[i].type == Evade) {
			behaiviourArray.erase(behaiviourArray.begin() + i);
			break;
		}
	}
}

void Agent::RemoveWander() {
	for (int i = 0; i < behaiviourArray.size(); i++) {
		if (behaiviourArray[i].type == Wander) {
			behaiviourArray.erase(behaiviourArray.begin() + i);
			break;
		}
	}
}

void Agent::AddForce(Point force) {
	velocity += force;
}

void Agent::SetForce(Point force) {
	velocity = force;
}

Point Agent::GetVelocity() {
	return velocity;
}

void Agent::Update() {
	float speed;

	for (int i = 0; i < behaiviourArray.size(); i++) {
		switch (behaiviourArray[i].type) {
		case Pursue:
			velocity += GetPersue(behaiviourArray[i].target, maxVelocity * behaiviourArray[i].strength);
			break;
		case Evade:
			velocity += GetEvade(behaiviourArray[i].target, maxVelocity * behaiviourArray[i].strength);
			break;
		case Wander:
			velocity += GetWander(maxVelocity * behaiviourArray[i].strength);
			break;
		case Flock:
			velocity += GetFlock(maxVelocity * behaiviourArray[i].strength);
			break;
		}
	}

	speed = std::sqrt((velocity.x * velocity.x) + (velocity.y * velocity.y));

	//cap speed
	//personal cap
	if (maxVelocity != -1 && maxVelocity <= speedCap) {
		if (speed > maxVelocity) {
			//get difference in speed
			float speedReduction = speed - maxVelocity;
			//get normal of x and y of velocity
			float normal_x = velocity.x / speed;
			float normal_y = velocity.y / speed;
			//reduece velociy.x by normal_x * speedRedduction
			velocity.x -= normal_x * speedReduction;
			velocity.y -= normal_y * speedReduction;
		}
	} else {
		//global cap
		if (speed > speedCap) {
			//get difference in speed
			float speedReduction = speed - maxVelocity;
			//get normal of x and y of velocity
			float normal_x = velocity.x / speed;
			float normal_y = velocity.y / speed;
			//reduece velociy.x by normal_x * speedRedduction
			velocity.x -= normal_x * speedReduction;
			velocity.y -= normal_y * speedReduction;
		}
	}

	speed = std::sqrt((velocity.x * velocity.x) + (velocity.y * velocity.y));

	//add drag
	if (drag) {
		if (!(velocity.x < 0.00001 && velocity.x > -0.00001) || //float eq for if (velocity.x != 0 || velocity.y != 0)
			!(velocity.y < 0.00001 && velocity.y > -0.00001)) {
			float normal_x = velocity.x / speed;
			float normal_y = velocity.y / speed;

			if (speed > resistance) {
				velocity.x = normal_x * (speed - resistance);
				velocity.y = normal_y * (speed - resistance);
			} else {
				velocity.x = 0;
				velocity.y = 0;
			}
		}
	}

	//loop screen
	if (position.x + (width / 2) > 900) {
		//position.x = (width / 2);
		velocity.x *= -1;
		position.x = 900 - width;
	} else if (position.x - (width / 2) < 0) {
		//position.x = 900 - (width / 2);
		velocity.x *= -1;
		position.x = 0 + width;
	}

	if (position.y + (height / 2) > 600) {
		//position.y = (height / 2);
		velocity.y *= -1;
		position.y = 600 - height;
	} else if (position.y - (height / 2) < 0) {
		//position.y = 600 - (height / 2);
		velocity.y *= -1;
		position.y = 0 + height;
	}

	//move Agent
	position += velocity;
}

void Agent::Draw() {
	MoveSprite(sprite, position.x, position.y);
	DrawSprite(sprite);
	if (drawVelocity) {
		DrawLine(position.x, position.y, position.x + (velocity.x * 10), position.y + (velocity.y * 10), SColour(255, 0, 0, 255));
	}
}

void Agent::ToggleDrag() {
	drag = !drag;
}

void Agent::ToggleVelocityLine() {
	drawVelocity = !drawVelocity;
}

Point Agent::GetPersue(Agent* in_target, float in_speed) {
	Point out_velocity = Point(0, 0);

	//get target's position from the origin of position
	Point targetDirectPos = (in_target->position) - position;
	float targetDirectDist = std::sqrt((targetDirectPos.x * targetDirectPos.x) + (targetDirectPos.y * targetDirectPos.y));

	if (targetDirectDist > in_speed) {//if far away find where the target is going
		Point TargetRelPos = (in_target->position + in_target->GetVelocity()) - position;
		float targetRelDist = std::sqrt((TargetRelPos.x * TargetRelPos.x) + (TargetRelPos.y * TargetRelPos.y));
		TargetRelPos.x /= targetRelDist;
		TargetRelPos.y /= targetRelDist;

		out_velocity.x = (TargetRelPos.x * in_speed);
		out_velocity.y = (TargetRelPos.y * in_speed);

	}
	else {//if they're close then go strait to them
		targetDirectPos.x /= targetDirectDist;
		targetDirectPos.y /= targetDirectDist;

		//add to velocity
		out_velocity.x = (targetDirectPos.x * (in_speed - targetDirectDist));
		out_velocity.y = (targetDirectPos.y * (in_speed - targetDirectDist));

	}

	return out_velocity;
}

Point Agent::GetEvade(Agent* in_target, float in_speed) {
	Point out_velocity = Point(0, 0);

	//get target's position from the origin of position
	Point targetDirectPos = (in_target->position) - position;
	float targetDirectDist = std::sqrt((targetDirectPos.x * targetDirectPos.x) + (targetDirectPos.y * targetDirectPos.y));

	if (targetDirectDist > in_speed) {//if far away find where the target is going
		Point TargetRelPos = (in_target->position + in_target->GetVelocity()) - position;
		float targetRelDist = std::sqrt((TargetRelPos.x * TargetRelPos.x) + (TargetRelPos.y * TargetRelPos.y));
		TargetRelPos.x /= targetRelDist;
		TargetRelPos.y /= targetRelDist;

		out_velocity.x = (TargetRelPos.x * (in_speed * -1));
		out_velocity.y = (TargetRelPos.y * (in_speed * -1));

	}
	else {//if they're close then go strait to them
		targetDirectPos.x /= targetDirectDist;
		targetDirectPos.y /= targetDirectDist;

		//add to velocity
		out_velocity.x = (targetDirectPos.x * ((in_speed - targetDirectDist) * -1));
		out_velocity.y = (targetDirectPos.y * ((in_speed - targetDirectDist) * -1));

	}

	return out_velocity;
}

Point Agent::GetWander(float in_speed) {
	Point out;

	//find circle position (Where agent position is the origin)
	Point currentDir = velocity;
	float currentSpeed = std::sqrt((currentDir.x * currentDir.x) + (currentDir.y * currentDir.y));
	if (currentSpeed > 0.0000001 && currentSpeed < 0.0000001) { //float eq for currentSpeed != 0
		currentDir.x /= currentSpeed;
		currentDir.y /= currentSpeed;
	}

	Point circlePos = currentDir * circDist;

	//generate float from 0-2
	wanderPoint += ((((rand() % 200) - 100) / 1000.0f)) * jitter;
	if (wanderPoint < 0.0f) {
		wanderPoint *= -1;
	}
	while (wanderPoint > 2.0f)  {
		wanderPoint -= 2.0f;
	}

	//find position on circle at the generated number in radians
	Point RadPos;
	RadPos.x = circlePos.x + circRadius * std::cos(wanderPoint * std::_Pi);
	RadPos.y = circlePos.y + circRadius * std::sin(wanderPoint * std::_Pi);

	//normalize radpos
	float RadMag = std::sqrt((RadPos.x * RadPos.x) + (RadPos.y * RadPos.y));
	RadPos.x = RadPos.x / RadMag;
	RadPos.y = RadPos.y / RadMag;

	//set radpos's magnitude to the radius
	RadPos.x = RadPos.x * circRadius;
	RadPos.y = RadPos.y * circRadius;

	//add the new direction to our current velocity 
	out = RadPos;

	//compensate to keep velocity's magnitude the same
	float speed = std::sqrt((out.x * out.x) + (out.y * out.y));

	out.x = out.x / speed;
	out.y = out.y / speed;

	out.x = out.x * in_speed;
	out.y = out.y * in_speed;

	return out;
}

void Agent::AddToFlock(float in_strength) {
	for (int i = 0; i < behaiviourArray.size(); i++) {
		if (behaiviourArray[i].type == Flock) {
			behaiviourArray[i].strength = in_strength;
			return;
		}
	}

	behaiviourArray.emplace_back(Behaviour(Flock, nullptr, in_strength));
	flockingAgents.emplace_back(this);

}

void Agent::RemoveFromFlock() {
	for (int i = 0; i < behaiviourArray.size(); i++) {
		if (behaiviourArray[i].type == Flock) {
			behaiviourArray.erase(behaiviourArray.begin() + i);
			break;;
		}
	}

	for (int i = 0; i < flockingAgents.size(); i++) {
		if (flockingAgents[i] == this) {
			flockingAgents.erase(flockingAgents.begin() + i);
			return;
		}
	}
}

void Agent::SetFlockBallence(float in_separation, float in_alignment, float in_cohesion) {
	sepatationPow = in_separation;
	allignmentPow = in_alignment;
	cohesionPow = in_cohesion;
}

void Agent::SetNeighbourhoodSize(float in_size) {
	neighbourhoodSize = in_size;
}

Point Agent::GetFlock(float in_speed) {

	std::vector<Agent*>neighbourhood = std::vector<Agent*>();

	if (neighbourhoodSize != 0) {
		//loop through world;
		for (int i = 0; i < flockingAgents.size(); i++) {
			//if distance from current Agent is less than radius add to neighbourhood
			Point positionDiference = flockingAgents[i]->position - position;
			float distance = std::sqrt((positionDiference.x * positionDiference.x) + (positionDiference.y * positionDiference.y));

			if (distance < neighbourhoodSize && flockingAgents[i] != this) {
				neighbourhood.emplace_back(flockingAgents[i]);
			}

		}
	}

	Point out = Point(0, 0);

	if (neighbourhood.size() != 0) {
		out += Separation(in_speed * sepatationPow, neighbourhood);
		out += Alignment(in_speed * allignmentPow, neighbourhood);
		out += Cohesion(in_speed * cohesionPow, neighbourhood);
	}

	return out;
}

Point Agent::Separation(float in_power, std::vector<Agent*>& ref_neighbourhood) {

	if (ref_neighbourhood.size() == 0) {
		return Point(0, 0);
	}

	Point totalRepulsion = Point(0, 0);

	for (int i = 0; i < ref_neighbourhood.size(); i++) {
		Point currentRepulsion = (ref_neighbourhood[i]->position - position) * -1;
		float repulseMag = std::sqrt(std::pow(currentRepulsion.x, 2) + std::pow(currentRepulsion.y, 2));

		if (repulseMag < neighbourhoodSize / 2) {
			totalRepulsion += currentRepulsion;
		}
	}

	float magnetude = std::sqrt(std::pow(totalRepulsion.x, 2) + std::pow(totalRepulsion.y, 2));

	if (magnetude < 0.0000001 && magnetude > -0.0000001) {//float EQ for magnetude == 0;
		return Point(0, 0);
	}

	totalRepulsion.x /= magnetude;
	totalRepulsion.y /= magnetude;

	totalRepulsion.x *= in_power;
	totalRepulsion.y *= in_power;

	return totalRepulsion;
}

Point Agent::Alignment(float in_power, std::vector<Agent*>& ref_neighbourhood) {
	if (ref_neighbourhood.size() < 3) {
		return Point(0, 0);
	}

	Point desiredVelocity = Point(0, 0);

	for (int i = 0; i < ref_neighbourhood.size(); i++) {
		desiredVelocity += ref_neighbourhood[i]->GetVelocity();
	}

	desiredVelocity.x /= ref_neighbourhood.size();
	desiredVelocity.y /= ref_neighbourhood.size();

	desiredVelocity -= velocity;

	float desiredVelMag = std::sqrt(std::pow(desiredVelocity.x, 2) + std::pow(desiredVelocity.y, 2));

	if (desiredVelMag < 0.0000001 && desiredVelMag > -0.0000001) { // float eq for desiredVelMag == 0:
		return Point(0, 0);
	}

	desiredVelocity.x /= desiredVelMag;
	desiredVelocity.y /= desiredVelMag;

	desiredVelocity.x *= in_power;
	desiredVelocity.y *= in_power;

	return desiredVelocity;
}

Point Agent::Cohesion(float in_power, std::vector<Agent*>& ref_neighbourhood) {
	if (ref_neighbourhood.size() < 3) {
		return Point(0, 0);
	}

	Point averagePos = Point(0, 0);
	int numItems = 0;

	for (int i = 0; i < ref_neighbourhood.size(); i++) {
		Point currentDist = ref_neighbourhood[i]->position - position;
		float posDist = std::sqrt(std::pow(currentDist.x, 2) + std::pow(currentDist.y, 2));

		if (posDist > neighbourhoodSize / 2) {
			averagePos += ref_neighbourhood[i]->position;
			numItems++;
		}
	}

	if (numItems == 0) {
		return Point(0, 0);
	}

	averagePos.x /= numItems;
	averagePos.y /= numItems;

	averagePos -= position;

	float magnitude = std::sqrt(std::pow(averagePos.x, 2) + std::pow(averagePos.y, 2));

	averagePos.x /= magnitude;
	averagePos.y /= magnitude;

	averagePos.x *= in_power;
	averagePos.y *= in_power;

	return averagePos;
}
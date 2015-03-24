#ifndef _Agent_h_
#define _Agent_h_

#include "Entity.h" //parent
#include "AIE.h"
#include <random>
#include <ctime>
#include <vector>

class Agent;

enum BehaviourType {
	Pursue,
	Evade,
	Wander,
	Flock
};

struct Behaviour {
	Behaviour(BehaviourType in_type, Agent* in_target, float  in_strength);
	BehaviourType type;
	Agent* target;
	float strength;//a percent value
};

class Agent : public Entity {
public:
	Agent(float in_x, float in_y);
	~Agent();

	void SetSpeedCap(float in_speedCap);

	void ClearBehaviors();

	void AddPursue(Agent* in_target, float in_strength);
	void AddEvade(Agent* in_target, float in_strength);
	void AddWander(float in_circDist, float in_circRadius, float in_jitter, float in_strength);
	void AddToFlock(float in_strength);

	void RemovePursue(Agent* in_target);
	void RemoveEvade(Agent* in_target);
	void RemoveWander();
	void RemoveFromFlock();

	static void SetFlockBallence(float in_separation, float in_alignment, float in_cohesion);
	static void SetNeighbourhoodSize(float in_size);

	void AddForce(Point force);
	void SetForce(Point force);

	Point GetVelocity();

	virtual void Update();
	virtual void Draw();

	void ToggleDrag();
	static void ToggleVelocityLine();

private:
	//behavior meathods
	Point GetPersue(Agent* in_target, float in_speed);
	Point GetEvade(Agent* in_target, float in_speed);
	Point GetWander(float in_speed);
	//flocking behaviors
	Point GetFlock(float in_speed);
	Point Separation(float in_power, std::vector<Agent*>& ref_neighbourhood);
	Point Alignment(float in_power, std::vector<Agent*>& ref_neighbourhood);
	Point Cohesion(float in_power, std::vector<Agent*>& ref__neighbourhood);

	//physics
	Point velocity;
	bool drag;
	float maxVelocity;

	std::vector<Behaviour> behaiviourArray;

	//wander stuff;
	float wanderPoint;
	float circDist;
	float circRadius;
	float jitter;

	//flock Vars
	static std::vector<Agent*> flockingAgents;
	static float sepatationPow;
	static float allignmentPow;
	static float cohesionPow;
	static float neighbourhoodSize;

	static bool drawVelocity;

	static float const resistance;
	static float const speedCap;

	static char* const texture;
	static unsigned int sprite;
};

#endif
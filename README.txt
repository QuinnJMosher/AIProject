AI Project Control Documentation

Types of entities
agent - the mobile entity that can accept behaviours
wall - an area that agents cannot traverse, always aligned to the pre made grid

Adding ad removing entities;
A - add agent at mouse position
Z - remove agent nearest to mouse position
S - add wall at graph node nearest to mouse position
X - remove wall nearest to mouse position

Selection names
red - the agent to be given a behaviour;
blue -  the agent or waypoint that red will target

Agent Selection
Left click - set red agent to nearest agent to mouse position
Right click - set blue agent to nearest agent to mouse position, if no agent is nearby creates a waypoint

Behaviours
Pursue - attempts to get to the targeted agent as quickly as possible
	-requires both a red agent and a blue agent be selected before confirming
	-will path find if a direct rout is impossible
Evade - avoids the targeted agent by attempting to get as far away from it as possible
	-requires both a red agent and a blue agent be selected before confirming
Wander - heads in no particular direction, changing direction randomly
	-only requires a red agent, if a blue agent is selected it will be ignored
Flock - uses several behaviours to mimic a large group of birds
	-only requires a red agent, if a blue agent is selected it will be ignored
	Separation - agents find the mean evade vector from all their neighbours
	Alignment - agents find the mean velocity of their neighbours attempts to match it
	Cohesion - agents find the mean position and attempt to pursue it
Go To - agents go to a given point
	-requires a red agent and a blue waypoint be selected before confirming
	-automatically selected when the blue waypoint is created
Clear All -removes all behaviours from the agent
	-only requires a red agent, if a blue agent is selected it will be ignored
	
Behaviour selection
1 - selects Pursue
2 - selects Evade
3 - selects Wander
4 - selects Flock
0 - deselects all behaviours
C - selects Clear
Space Bar - confirms and adds behaviour

Misc inputs
P - pauses entity update until pressed again
V - draws a line based on each agent's velocity
G - show the path finding graph
N - set path finding algorithm to A*
M - set path finding algorithm to Dijkstra's algorithm
D - toggle drag on all agents
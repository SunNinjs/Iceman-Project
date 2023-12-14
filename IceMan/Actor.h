/*
ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
class StudentWorld;

class Actor : public GraphObject {
public:
	Actor(int imageID, int startX, int startY, Direction startDirection, float size, unsigned int depth, StudentWorld* world) :
		GraphObject(imageID, startX, startY, startDirection, size, depth) {
		m_alive = true;
		m_world = world;
		setVisible(true);
	}

	//Get StudentWorld Object;
	StudentWorld* getWorld() {
		return m_world;
	}

	//Check if alive
	bool isAlive() {
		return m_alive;
	}

	//Set actor to dead state
	void setDead() {
		m_alive = false;
	}

	//Virtual function for every actor to do during every tick
	virtual void doSomething() = 0;

	// Move this actor to x,y if possible, and return true; otherwise,
	// return false without moving.
	bool moveToIfPossible(int x, int y);

private:
	bool m_alive;
	StudentWorld* m_world;
};

class Ice : public Actor {
public:
	Ice(int X, int Y, StudentWorld* world) :
		Actor(IID_ICE, X, Y, right, 0.25, 3, world) {}

	virtual ~Ice() {}

	virtual void doSomething() {}
};

class Agent : public Actor {
public:
	Agent(int imageID, int startX, int startY, Direction startDirection, float size, unsigned int depth, StudentWorld* world, int hitpoints) :
		Actor(imageID, startX, startY, startDirection, size, depth, world) {
		m_hitpoints = hitpoints;
	}

	// Pick up a gold nugget.
	virtual void addGold() = 0;

	//returns hitpoints
	int getHP() {
		return m_hitpoints;
	}

	//returns new hitpoints
	int setHP(int hp) {
		m_hitpoints = hp;
		return hp;
	}

	//damage function
	void takeDamage(int dam) {
		m_hitpoints -= dam;
		if (m_hitpoints <= 0)
			Dead();
	}

	//Virtual function for when the player or protestors die
	virtual void Dead() = 0;
private:
	int m_hitpoints;
};

class IceMan : public Agent {
public:
	IceMan(StudentWorld* world) : Agent(IID_PLAYER, 30, 60, right, 1.0, 0, world, 10) {
		m_water = 5;
		m_sonar = 1;
		m_gold = 0;
	}

	virtual void doSomething() {
		if (!isAlive()) return;
	}

	// Pick up a sonar kit.
	void addSonar();

	// Pick up water.
	void addWater();

	// Get amount of gold
	unsigned int getGold() const;

	// Get amount of sonar charges
	unsigned int getSonar() const;

	// Get amount of water
	unsigned int getWater() const;

private:
	int m_water;
	int m_sonar;
	int m_gold;
};

class Protester : public Agent
{
public:
	Protester(StudentWorld* world, int startX, int startY, int imageID,
		unsigned int hitPoints, unsigned int score);

	// Set state to having gien up protest
	void setMustLeaveOilField();

	// Set number of ticks until next move
	void setTicksToNextMove();
};

class RegularProtester : public Protester
{
public:
	RegularProtester(StudentWorld* world, int startX, int startY, int imageID);
	virtual void move();
	virtual void addGold();
};

class HardcoreProtester : public Protester
{
public:
	HardcoreProtester(StudentWorld* world, int startX, int startY, int imageID);
	virtual void move();
	virtual void addGold();
};

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

#endif // ACTOR_H_
*/

#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <algorithm>

class StudentWorld;

class Actor : public GraphObject
{
public:
	Actor(StudentWorld* world, int startX, int startY, Direction startDir,
		bool visible, int imageID, double size, int depth);

	// Action to perform each tick.
	virtual void move() = 0;

	// Is this actor alive?
	bool isAlive() const;

	// Mark this actor as dead.
	void setDead();

	// Annoy this actor.
	virtual bool annoy(unsigned int amt);

	// Bribe
	virtual void bribe() {};

	// Get this actor's world
	StudentWorld* getWorld() const;

	// Can other actors pass through this actor?
	virtual bool canActorsPassThroughMe() const;

	// Can this actor dig through Ice?
	virtual bool canDigThroughIce() const;

	// Can this actor pick items up?
	virtual bool canPickThingsUp() const;

	// Does this actor hunt the IceMan?
	virtual bool huntsIceMan() const;

	// Can this actor need to be picked up to finish the level?
	virtual bool needsToBePickedUpToFinishLevel() const;

	// Move this actor to x,y if possible, and return true; otherwise,
	// return false without moving.
	bool moveToIfPossible(int x, int y);

	bool moveToIfPossible2(int x, int y);
private:
	bool m_alive;
	StudentWorld* m_world;
};

class Agent : public Actor
{
public:
	Agent(StudentWorld* world, int startX, int startY, Direction startDir,
		int imageID, unsigned int hitPoints);

	// Pick up a gold nugget.
	virtual void addGold() = 0;

	// How many hit points does this actor have left?
	unsigned int getHitPoints() const;
	unsigned int setHitPoints(int hp);

	virtual bool annoy(unsigned int amount);
	virtual bool canPickThingsUp() const;
private:
	unsigned int m_hp;
};

class IceMan : public Agent
{
public:
	IceMan(StudentWorld* world, int startX, int startY);
	virtual void move();
	virtual bool annoy(unsigned int amount);
	virtual void addGold();
	virtual bool canDigThroughIce() const;

	// Pick up a sonar kit.
	void addSonar();

	// Pick up water.
	void addWater();

	// Get amount of gold
	unsigned int getGold() const;

	// Get amount of sonar charges
	unsigned int getSonar() const;

	// Get amount of water
	unsigned int getWater() const;
private:
	unsigned int m_water;
	unsigned int m_sonar;
	unsigned int m_gold;
	int m_barrels;
};

class Protester : public Agent
{
public:
	Protester(StudentWorld* world, int startX, int startY, int imageID,
		unsigned int hitPoints, unsigned int score);
	virtual void move();
	virtual bool annoy(unsigned int amount);
	virtual void addGold();
	virtual bool huntsIceMan() const;

	// Moves a certain direction
	bool regularMove(int x, int y, Direction dir);

	// Make new direction
	Direction newDirection();

	enum State { LEAVE, HUNT };

	// Sets State
	void setState(State state) {
		m_state = state;
	}

	State getState() {
		return m_state;
	}

	// Sets wait ticks
	void setTicks(int ticks) {
		m_waitTicks = ticks;
	}

	int getTicks() {
		return m_waitTicks;
	}

	// Setnumsquares
	void setMovedir(int temp) {
		m_numSquaresToMoveInDir = temp;
	}

	int getMovedir() {
		return m_numSquaresToMoveInDir;
	}

	void setSinShout(int temp) {
		m_ticksSinceShouted = temp;
	}

	int getSinShout() {
		return m_ticksSinceShouted;
	}

	void setSinTurn(int temp) {
		m_ticksSinceTurned = temp;
	}

	int getSinTurn() {
		return m_ticksSinceTurned;
	}

	int getWaitTime() {
		return std::max(0, m_levelWaitTicks);
	}

private:
	State m_state;
	int m_waitTicks;
	int m_levelWaitTicks;
	int m_ticksSinceShouted;
	int m_ticksSinceTurned;
	int m_numSquaresToMoveInDir;
	unsigned int m_score;
};

class RegularProtester : public Protester
{
public:
	RegularProtester(StudentWorld* world, int startX, int startY, int imageID);
	virtual void move();
	virtual void addGold();
};

class HardcoreProtester : public Protester
{
public:
	HardcoreProtester(StudentWorld* world, int startX, int startY, int imageID);
	virtual void move();
	virtual void addGold();
};

class Ice : public Actor
{
public:
	Ice(StudentWorld* world, int startX, int startY);
	virtual ~Ice() {}
	virtual void move() {};
};

class Boulder : public Actor
{
public:
	Boulder(StudentWorld* world, int startX, int startY);
	virtual void move();
	virtual bool canActorsPassThroughMe() const;
private:
	enum BoulderState { STABLE, WAITING, FALLING }; //0 for stable, 1 for waiting, 2 for falling
	BoulderState m_state;
	int m_time;
};

class Squirt : public Actor
{
public:
	Squirt(StudentWorld* world, int startX, int startY, Direction startDir);
	virtual void move();
private:
	int m_distance;
};

class ActivatingObject : public Actor
{
public:
	ActivatingObject(StudentWorld* world, int startX, int startY, int imageID,
		int soundToPlay, bool activateOnPlayer,
		bool activateOnProtester, bool initallyActive, bool visible);
	virtual void move();

	// Function to run for every objects move function
	void check();

	// Set number of ticks until this object dies
	void setTicksToLive();

	int getSound();
	bool getActOnPlay();
	bool getIntAct();
	int getTime();
	int setTime(int time);

private:
	int m_sound;
	bool m_actOnPlayer;
	bool m_actOnProtester;
	bool m_intAct;
	int m_time;
};

class OilBarrel : public ActivatingObject
{
public:
	OilBarrel(StudentWorld* world, int startX, int startY);
	virtual void move();
	virtual bool needsToBePickedUpToFinishLevel() const;
private:
	bool m_finish;
};

class GoldNugget : public ActivatingObject
{
public:
	GoldNugget(StudentWorld* world, int startX, int startY, bool temporary, bool onPlayer, bool visible);
	virtual void move();
private:
	bool m_state; // True means temporary, false means permanent
	
};

class SonarKit : public ActivatingObject
{
public:
	SonarKit(StudentWorld* world, int time);
	virtual void move();
private:
};

class WaterPool : public ActivatingObject
{
public:
	WaterPool(StudentWorld* world, int startX, int startY, int time);
	virtual void move();
};

#endif // ACTOR_H_
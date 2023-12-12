/*#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <vector>
#include <string>
#include <algorithm>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
		: GameWorld(assetDir)
	{}

	~StudentWorld()
	{
		cleanUp();
	}

	virtual int init()
	{
		num_ice = 3840;
		num_protesters = 0;
		int i = 0;
		while (i < num_ice) {
			for (int x = 0; x < 64; x++) {
				for (int y = 0; y < 60; y++) {
					if (x >= 30 && x <= 33 && y > 3) continue;
					m_ice[i] = new Ice(x, y, this);
					i++;
				}
			}
		}
		return GWSTATUS_CONTINUE_GAME;
	}

	virtual int move()
	{
		// This code is here merely to allow the game to build, run, and terminate after you hit enter a few times.
		// Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}

	virtual void cleanUp()
	{
	}

private:
	int num_ice;
	int num_boulder;
	int num_gold;
	int num_barrel;
	int num_protesters;
	int account_barrel;
	Ice* m_ice[3840];
	IceMan* m_iceman;
	std::vector<Actor*> Actors;
};

#endif // STUDENTWORLD_H_
*/

#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GraphObject.h"
#include "GameWorld.h"
#include <vector>
#include <string>
#include <algorithm>

class Actor;
class IceMan;
class Ice;

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir) :
		GameWorld(assetDir) {}

    virtual int init();
    virtual int move();
    virtual void cleanUp();

	// Add gold
	void createGold(int x, int y, bool temp, bool onPlayer, bool visible);

	// Add Water
	void addWater(int x, int y, GraphObject::Direction dir);

	IceMan* getIceMan();

    // Clear a 4x4 region of Ice.
    void clearIce(int x, int y);

	// Check for ice
	bool checkIce(int x, int y, GraphObject::Direction dir);

	// Check for ice and Boulder
	bool checkIceBoulder(int x, int y, GraphObject::Direction dir);

    // Can actor move to x,y?
    bool canActorMoveTo(Actor* a, int x, int y) const;

    // Annoy all other actors within radius of annoyer, returning the
    // number of actors annoyed.
    int annoyAllNearbyActors(Actor* annoyer, int points, int radius);

    // Reveal all objects within radius of x,y.
    void revealAllNearbyObjects(int x, int y, int radius);

    // If the IceMan is within radius of a, return a pointer to the
    // IceMan, otherwise null.
    Actor* findNearbyIceMan(Actor* a, int radius) const;

    // If at least one actor that can pick things up is within radius of a,
    // return a pointer to one of them, otherwise null.
    Actor* findNearbyPickerUpper(Actor* a, int radius) const;

	// Check if any actors is within the radius of x and y
	// Returns true if an actor is in the radius
	bool checkRadius(int x, int y, double radius) const;

	// Raises any value to the power 2
	int intPow(int x) const;

	// Update Display Text Function
	void setDisplayText();

	// Removes all dead Actors
	void removeDeadActors();

	// Check if level is finished
	bool levelFinished();

    // Annoy the IceMan.
    void annoyIceMan();

	// How many barrels
	int getBarrels();

	// Decrease Barrel
	void decBarrels(int amount);

    // Is the Actor a facing toward the IceMan?
    bool facingTowardIceMan(Actor* a) const;

    // If the Actor a has a clear line of sight to the IceMan, return
    // the direction to the IceMan, otherwise GraphObject::none.
    GraphObject::Direction lineOfSightToIceMan(Actor* a) const;

    // Return whether the Actor a is within radius of IceMan.
    bool isNearIceMan(Actor* a, int radius) const;

    // Determine the direction of the first move a quitting protester
    // makes to leave the oil field.
    GraphObject::Direction determineFirstMoveToExit(int x, int y);

    // Determine the direction of the first move a hardcore protester
    // makes to approach the IceMan.
    GraphObject::Direction determineFirstMoveToIceMan(int x, int y);
private:
	IceMan* m_iceman;
	std::vector<Actor*> m_actors;
	Ice* ice_array[64][64];
	int m_barrelsleft;
	int num_protesters;
};

#endif // STUDENTWORLD_H_
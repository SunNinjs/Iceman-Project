#include "StudentWorld.h"
#include "Actor.h"
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

int StudentWorld::init()
{
	//Create Ice
	for (int x = 0; x < 64; x++) {
		for (int y = 0; y < 64; y++) {
			if (x >= 30 && x <= 33 && y > 3) {
				ice_array[x][y] = nullptr;
			}
			else if (y >= 60) {
				ice_array[x][y] = nullptr;
			}
			else {
				ice_array[x][y] = new Ice(this, x, y);
			}
		}
	}

	//Create IceMan
	m_iceman = new IceMan(this, 30, 60);

	int level = getLevel(); //Get Level
	int boulders = min((level / 2) + 2, 9); //Number of Boulders
	int gold = max((5 - level) / 2, 2); //Number of Gold
	int barrels = min(2 + level, 21); // Number of barrels
	m_barrelsleft = barrels;
	
	//Create Boulders
	for (int i = 0; i < boulders; ) {
		int x = rand() % 61;
		int y = rand() % (37 + 20);
		if (x > 26 && x < 34 && y > 0) continue;
		bool check = checkRadius(x, y, 6.0);
		if (check)
			continue;
		Boulder* boulder = new Boulder(this, x, y);
		m_actors.push_back(boulder);
		clearIce(x, y);
		i++;
	}

	//Create Gold
	for (int i = 0; i < gold;)
	{
		int x = rand() % 61;
		int y = rand() % 57;
		if (x > 26 && x < 34 && y > 0) continue;
		bool check = checkRadius(x, y, 6.0);
		if (check)
			continue;
		GoldNugget* gold = new GoldNugget(this, x, y, false, true, false);
		m_actors.push_back(gold);
		i++;
	}

	for (int i = 0; i < barrels;)
	{
		int x = rand() % 61;
		int y = rand() % 57;
		if (x > 26 && x < 34 && y > 0) continue;
		bool check = checkRadius(x, y, 6.0);
		if (check)
			continue;
		OilBarrel* barrel = new OilBarrel(this, x, y);
		m_actors.push_back(barrel);
		i++;
	}

	return GWSTATUS_CONTINUE_GAME;
}

/*
GWSTATUS_PLAYER_DIED
GWSTATUS_CONTINUE_GAME
GWSTATUS_FINISHED_LEVEL
*/
int StudentWorld::move()
{
	int G = getLevel() * 25 + 300;
	int num = rand() % G + 1;
	int tem = 300 - (10 * getLevel());
	int T = max(100, tem);
	if (num <= 1) {
		int pro = rand() % 5 + 1;
		if (pro <= 1) {
			m_actors.push_back(new SonarKit(this, T));
		}
		else if (pro > 1) {
			int x = rand() % 61;
			int y = rand() % 61;
			while (ice_array[x][y] != nullptr) {
				x = rand() % 61;
				y = rand() % 61;
			}
			m_actors.push_back(new WaterPool(this, x, y, T));
		}
	}

	setDisplayText();
	if (m_iceman->isAlive()) {
		m_iceman->move();
	}

	for (Actor* ele : m_actors) {
		if (ele->isAlive())
			ele->move();
		if (!(m_iceman->isAlive())) {
			decLives();
			return GWSTATUS_PLAYER_DIED;
		}
		if (levelFinished())
			return GWSTATUS_FINISHED_LEVEL;
	}

	removeDeadActors();

	if (!(m_iceman->isAlive())) {
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}

	if (levelFinished()) {
		return GWSTATUS_FINISHED_LEVEL;
	}

	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	// Deletes Iceman
	delete m_iceman;
	// Deletes Ice
	for (int x = 0; x < 64; x++) {
		for (int y = 0; y < 60; y++) {
			if (ice_array[x][y] != nullptr) {
				delete ice_array[x][y];
				ice_array[x][y] = nullptr;
			}
		}
	}

	// Deletes all actors
	for (auto it = m_actors.begin(); it != m_actors.end();)
	{
		delete (*it);
		it = m_actors.erase(it);
	}
}


void StudentWorld::createGold(int x, int y, bool temp, bool onPlayer, bool visible)
{
	m_actors.push_back(new GoldNugget(this, x, y, temp, onPlayer, visible));
	return;
}

void StudentWorld::addWater(int x, int y, GraphObject::Direction dir)
{
	switch (dir)
	{
	case GraphObject::up:
		m_actors.push_back(new Squirt(this, x, y + 4, GraphObject::up));
		break;
	case GraphObject::down:
		m_actors.push_back(new Squirt(this, x, y - 4, GraphObject::down));
		break;
	case GraphObject::left:
		m_actors.push_back(new Squirt(this, x - 4, y, GraphObject::left));
		break;
	case GraphObject::right:
		m_actors.push_back(new Squirt(this, x + 4, y, GraphObject::right));
		break;
	default:
		break;
	}
}

IceMan* StudentWorld::getIceMan()
{
	return m_iceman;
}

void StudentWorld::clearIce(int x, int y)
{
	for (int k = 0; k < 4; k++) {
		for (int t = 0; t < 4; t++) {
			if (ice_array[x + k][y + t] != nullptr) {
				delete ice_array[x + k][y + t];
				ice_array[x + k][y + t] = nullptr;
			}
		}
	}
}

bool StudentWorld::checkIce(int x, int y, GraphObject::Direction dir)
{
	if (ice_array[x][y] != nullptr) return true;
	for (int i = 0; i < 3; i++) {
		switch (dir)
		{
		case GraphObject::up:
			if (ice_array[x + i][y] != nullptr) return true;
			break;
		case GraphObject::down:
			if (ice_array[x + i][y] != nullptr) return true;
			break;
		case GraphObject::left:
			if (ice_array[x][y + i] != nullptr) return true;
			break;
		case GraphObject::right:
			if (ice_array[x][y + i] != nullptr) return true;
			break;
		} 
	}
	return false;
}

bool StudentWorld::checkIceBoulder(int x, int y, GraphObject::Direction dir)
{
	if (!checkIce(x, y, dir) && canActorMoveTo(m_iceman, x, y))
		return true;
	return false;
}

bool StudentWorld::canActorMoveTo(Actor* a, int x, int y) const
{
	for (Actor* ele : m_actors) {
		if (ele->canActorsPassThroughMe()) continue;
		int actor_x = ele->getX();
		int actor_y = ele->getY();
		double distance = sqrt(intPow(actor_x - x) + intPow(actor_y - y));
		if (distance < 3) {
			return false;
		}
	}
	return true;
}

int StudentWorld::annoyAllNearbyActors(Actor* annoyer, int points, int radius)
{
	if (!checkRadius(annoyer->getX(), annoyer->getY(), radius)) return 0;
	int num = 0;
	for (Actor* ele : m_actors) {
		if (!ele->canPickThingsUp()) continue;
		double distance = sqrt(intPow(annoyer->getX() - ele->getX()) + intPow(annoyer->getY() - ele->getY()));
		if (distance < radius) {
			ele->annoy(points);
			num++;
		}
	}
	return num;
}

void StudentWorld::revealAllNearbyObjects(int x, int y, int radius)
{
	if (!checkRadius(x, y, radius)) return;
	for (Actor* ele : m_actors) {
		int actor_x = ele->getX();
		int actor_y = ele->getY();
		double distance = sqrt(intPow(actor_x - x) + intPow(actor_y - y));
		if (distance <= radius) {
			ele->setVisible(true);
		}
	}
}

Actor* StudentWorld::findNearbyIceMan(Actor* a, int radius) const
{
	int actor_x = a->getX();
	int actor_y = a->getY();
	int x = m_iceman->getX();
	int y = m_iceman->getY();
	double distance = sqrt(intPow(actor_x - x) + intPow(actor_y - y));
	if (distance < radius) {
		return m_iceman;
	}
	return nullptr;
}

Actor* StudentWorld::findNearbyPickerUpper(Actor* a, int radius) const
{
	for (Actor* ele : m_actors) {
		if (!ele->canPickThingsUp()) continue;
		double distance = sqrt(intPow(a->getX() - ele->getX() ) + intPow(a->getY() - ele->getY()));
		if (distance <= radius) {
			return ele;
		}
	}
	return nullptr;
}

bool StudentWorld::checkRadius(int x, int y, double radius) const
{
	for (Actor* ele : m_actors) {
		int actor_x = ele->getX();
		int actor_y = ele->getY();
		double distance = sqrt(intPow(actor_x - x) + intPow(actor_y - y));
		if (distance > radius)
			continue;
		else
			return true;
	}
	return false;
}

int StudentWorld::intPow(int x) const
{
	return x * x;
}

void StudentWorld::setDisplayText()
{
	int level = getLevel();
	int lives = getLives();
	int health = m_iceman->getHitPoints() * 10;
	int squirts = m_iceman->getWater();
	int gold = m_iceman->getGold();
	int barrelsleft = m_barrelsleft;
	int sonar = m_iceman->getSonar();
	int score = getScore();

	ostringstream oss;
	oss << "Lvl: " << setw(2) << level << "  Lives: " << setw(1) << lives << "  Hlth: " << setw(3) << health * 10 << "%  Wtr: " << setw(2) << squirts << "  Gld: " << setw(2) << gold << "  Oil Left: " << setw(2) << barrelsleft << "  Sonar: " << setw(2) << sonar << "  Scr: ";
	oss.fill('0');
	oss << setw(6) << score << endl;
	string s = oss.str();
	setGameStatText(s);
}

void StudentWorld::removeDeadActors()
{
	for (auto it = m_actors.begin(); it != m_actors.end();)
		if (!(*it)->isAlive())
		{
			delete (*it);
			it = m_actors.erase(it);
		}
		else
			it++;
}

bool StudentWorld::levelFinished()
{
	if (m_barrelsleft == 0) {
		playSound(SOUND_FINISHED_LEVEL);
		return true;
	}
	return false;
}

int StudentWorld::getBarrels()
{
	return m_barrelsleft;
}

void StudentWorld::decBarrels(int amount)
{
	m_barrelsleft -= amount;
}

bool StudentWorld::isNearIceMan(Actor* a, int radius) const
{
	double distance = sqrt(intPow(a->getX() - m_iceman->getX()) + intPow(a->getY() - m_iceman->getY()));
	if (distance <= radius) {
		return true;
	}
	return false;
}

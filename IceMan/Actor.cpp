#include "Actor.h"
#include "StudentWorld.h"
using namespace std;

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

Actor::Actor(StudentWorld* world, int startX, int startY, Direction startDir, bool visible, int imageID, double size, int depth) :
	GraphObject(imageID, startX, startY, startDir, size, depth) {
	m_alive = true;
	m_world = world;
	setVisible(visible);
}

bool Actor::isAlive() const
{
	return m_alive;
}

void Actor::setDead()
{
	m_alive = false;
}

bool Actor::annoy(unsigned int amt)
{
	return false;
}

StudentWorld* Actor::getWorld() const
{
	return m_world;
}

bool Actor::canActorsPassThroughMe() const
{
	return true;
}

bool Actor::canDigThroughIce() const
{
	return false;
}

bool Actor::canPickThingsUp() const
{
	return false;
}

bool Actor::huntsIceMan() const
{
	return false;
}

bool Actor::needsToBePickedUpToFinishLevel() const
{
	return false;
}

bool Actor::moveToIfPossible(int x, int y)
{
	if (getWorld()->canActorMoveTo(this, x, y)) {
		moveTo(x, y);
		return true;
	}
	return false;
}

bool Actor::moveToIfPossible2(int x, int y) {
	if (getWorld()->checkIceBoulder(x, y, none)) {
		moveTo(x, y);
		return true;
	}
	return false;
}

Agent::Agent(StudentWorld* world, int startX, int startY, Direction startDir, int imageID, unsigned int hitPoints) :
	Actor(world, startX, startY, startDir, true, imageID, 1.0, 0) {
	m_hp = hitPoints;
}

unsigned int Agent::getHitPoints() const
{
	return m_hp;
}

unsigned int Agent::setHitPoints(int hp)
{
	m_hp = hp;
	return m_hp;
}

bool Agent::annoy(unsigned int amount)
{
	return false;
}

bool Agent::canPickThingsUp() const
{
	return false;
}

IceMan::IceMan(StudentWorld* world, int startX, int startY) :
	Agent(world, startX, startY, right, IID_PLAYER, 10) {
	m_water = 5;
	m_sonar = 1;
	m_gold = 0;
	m_barrels = 0;
}

void IceMan::move()
{
	// Check if IceMan is alive
	if (!isAlive()) return;

	// Remove Ice Under IceMan
	StudentWorld* world = getWorld();
	if (world->checkIce(getX(), getY(), down)) {
		world->clearIce(getX(), getY());
		world->playSound(SOUND_DIG);
	}

	//Recieve Input
	int inp;
	if (world->getKey(inp) == true) {
		switch (inp)
		{
		case KEY_PRESS_ESCAPE:
			setDead();
			break;
		case KEY_PRESS_SPACE:
			if (getWater() == 0) break;
			m_water--;
			world->playSound(SOUND_PLAYER_SQUIRT);
			world->addWater(getX(), getY(), getDirection());
			break;
		case KEY_PRESS_UP:
			if (getDirection() != up) {
				setDirection(up);
				break;
			}
			if (getY() <= 59) {
				moveToIfPossible(getX(), getY() + 1);
			}
			else if (getY() == 60) moveTo(getX(), getY());
			break;
		case KEY_PRESS_DOWN:
			if (getDirection() != down) {
				setDirection(down);
				break;
			}
			if (getY() >= 1) {
				moveToIfPossible(getX(), getY() - 1);
			}
			else if (getY() == 0) moveTo(getX(), getY());
			break;
		case KEY_PRESS_RIGHT:
			if (getDirection() != right) {
				setDirection(right);
				break;
			}
			if (getX() <= 59) {
				moveToIfPossible(getX() + 1, getY());
			}
			else if (getX() == 60) moveTo(getX(), getY());
			break;
		case KEY_PRESS_LEFT:
			if (getDirection() != left) {
				setDirection(left);
				break;
			}
			if (getX() >= 1) {
				moveToIfPossible(getX() - 1, getY());
			}
			else if (getX() == 0) moveTo(getX(), getY());
			break;
		case 'Z':
		case 'z':
			if (getSonar() == 0) break;
			m_sonar--;
			world->revealAllNearbyObjects(getX(), getY(), 12);
			world->playSound(SOUND_SONAR);
			break;
		case KEY_PRESS_TAB:
			if (getGold() == 0) break;
			m_gold--;
			world->createGold(getX(), getY(), true, false, true);
			break;
		default:
			break;
		}
	}
}

bool IceMan::annoy(unsigned int amount)
{
	int current_hp = getHitPoints();
	setHitPoints(current_hp - amount);

	if (current_hp <= 0) {
		setDead();
		getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
		return false;
	}

	return true;
}

void IceMan::addGold()
{
	m_gold += 1;
}

bool IceMan::canDigThroughIce() const
{
	return true;
}

void IceMan::addSonar()
{
	m_sonar += 1;
}

void IceMan::addWater()
{
	m_water += 5;
}

unsigned int IceMan::getGold() const
{
	return m_gold;
}

unsigned int IceMan::getSonar() const
{
	return m_sonar;
}

unsigned int IceMan::getWater() const
{
	return m_water;
}

Protester::Protester(StudentWorld* world, int startX, int startY, int imageID,
	unsigned int hitPoints, unsigned int score) :
	Agent(world, startX, startY, left, IID_PROTESTER, 5) {
	m_state = HUNT;
	m_numSquaresToMoveInDir = (rand() % 53) + 8;
	int ticks = 3 - getWorld()->getLevel() / 4;
	m_levelWaitTicks = ticks;
	m_waitTicks = max(0, m_levelWaitTicks);
	m_ticksSinceShouted = 15;
	m_ticksSinceTurned = 0;
	m_score = score;
}

void Protester::move() {}

bool Protester::annoy(unsigned int amount)
{
	if (m_state == LEAVE) return false;
	int current_hp = getHitPoints();

	setHitPoints(current_hp - amount);
	if (current_hp <= 0) {
		setDead();
		m_state = LEAVE;
		getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
		m_waitTicks = 0;
		if (amount == 100) {
			getWorld()->increaseScore(500);
		}
		else {
			getWorld()->increaseScore(100);
		}
		
		return false;
	}
	else {
		getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
		int temp = 100 - getWorld()->getLevel() * 10;
		int N = max(50, temp);
		m_waitTicks = N;
	}
	return true;
}

void Protester::addGold()
{}

bool Protester::huntsIceMan() const
{
	return true;
}

bool Protester::regularMove(int x, int y, Direction dir)
{
	switch (dir)
	{
	case GraphObject::up:
		return moveToIfPossible2(x, y + 1);
		break;
	case GraphObject::down:
		return moveToIfPossible2(x, y - 1);
		break;
	case GraphObject::left:
		return moveToIfPossible2(x - 1, y);
		break;
	case GraphObject::right:
		return moveToIfPossible2(x + 1, y);
		break;
	default:
		break;
	}
	return false;
}

GraphObject::Direction Protester::newDirection()
{
	Direction newDir = none;
	int x = getX();
	int y = getY();
	if (getWorld()->checkIceBoulder(x, y + 1, none)) newDir = up;
	if (getWorld()->checkIceBoulder(x + 1, y, none)) newDir = right;
	if (getWorld()->checkIceBoulder(x - 1, y, none)) newDir = left;
	if (getWorld()->checkIceBoulder(x, y - 1, none)) newDir = down;
	return newDir;
}

Ice::Ice(StudentWorld* world, int startX, int startY) :
	Actor(world, startX, startY, right, true, IID_ICE, 0.25, 3) {}

Boulder::Boulder(StudentWorld* world, int startX, int startY) :
	Actor(world, startX, startY, down, true, IID_BOULDER, 1.0, 1) {
	m_state = STABLE;
	m_time = 30;
}

void Boulder::move() {
	if (!isAlive()) return;

	switch (m_state)
	{
	case STABLE:
		// Check for ice
		if (getWorld()->checkIce(getX(), getY() - 1, down)) break;

		m_state = WAITING;
		break;
	case WAITING:
		if (m_time > 0) {
			m_time--;
		}
		else if (m_time == 0) {
			m_state = FALLING;
			getWorld()->playSound(SOUND_FALLING_ROCK);
		}
		break;
	case FALLING:
		if (getWorld()->checkIceBoulder(getX(), getY() - 1, down))
		{
			moveTo(getX(), getY() - 1);
			getWorld()->annoyAllNearbyActors(this, 100, 3);
		}

		else if (!getWorld()->checkIceBoulder(getX(), getY() - 1, down)) {
			setDead();
		}
		break;
	default:
		break;
	}
}

bool Boulder::canActorsPassThroughMe() const {
	return false;
}

Squirt::Squirt(StudentWorld* world, int startX, int startY, Direction startDir) :
	Actor(world, startX, startY, startDir, true, IID_WATER_SPURT, 1.0, 1) {
	m_distance = 0;
}

void Squirt::move() {
	if (getWorld()->annoyAllNearbyActors(this, 2, 3) >= 1) {
		setDead();
		return;
	}

	if (m_distance == 4) {
		setDead();
		return;
	}

	switch (getDirection())
	{
	case up:
		if (!getWorld()->checkIceBoulder(getX(), getY() + 1, up) || getY() >= 60)
			setDead();
		else
		{
			moveTo(getX(), getY() + 1);
		}
		break;
	case down:
		if (!getWorld()->checkIceBoulder(getX(), getY() - 1, down) || getY() <= 0)
			setDead();
		else
		{
			moveTo(getX(), getY() - 1);
		}
		break;
	case right:
		if (!getWorld()->checkIceBoulder(getX() + 1, getY(), right) || getX() >= 60)
			setDead();
		else
		{
			moveTo(getX() + 1, getY());
		}
		break;
	case left:
		if (!getWorld()->checkIceBoulder(getX() - 1, getY(), left) || getX() <= 0)
			setDead();
		else
		{
			moveTo(getX() - 1, getY());
		}
		break;
	default:
		break;
	}
	m_distance++;
}

ActivatingObject::ActivatingObject(StudentWorld* world, int startX, int startY, int imageID,
	int soundToPlay, bool activateOnPlayer,
	bool activateOnProtester, bool initallyActive, bool visible) :
	Actor(world, startX, startY, right, visible, imageID, 1.0, 2) {
	m_sound = soundToPlay;
	m_actOnPlayer = activateOnPlayer;
	m_actOnProtester = activateOnProtester;
	m_intAct = initallyActive;
	m_time = 0;
}

void ActivatingObject::move()
{
	return;
}

void ActivatingObject::check()
{
	if (!isAlive()) return;

	if (!isVisible() && getWorld()->isNearIceMan(this, 4)) {
		setVisible(true);
		return;
	}
}

void ActivatingObject::setTicksToLive()
{
	return;
}

int ActivatingObject::getSound()
{
	return m_sound;
}

bool ActivatingObject::getActOnPlay()
{
	return m_actOnPlayer;
}

bool ActivatingObject::getIntAct()
{
	return m_intAct;
}

int ActivatingObject::getTime()
{
	return m_time;
}

int ActivatingObject::setTime(int time)
{
	m_time = time;
	return m_time;
}

OilBarrel::OilBarrel(StudentWorld* world, int startX, int startY) :
	ActivatingObject(world, startX, startY, IID_BARREL, SOUND_FOUND_OIL, true, false, true, false) {
	m_finish = false;
}

void OilBarrel::move() {
	check();

	if (getWorld()->isNearIceMan(this, 3)) {
		setDead();
		getWorld()->playSound(SOUND_FOUND_OIL);
		getWorld()->increaseScore(1000);
		getWorld()->decBarrels(1);
	}
}

bool OilBarrel::needsToBePickedUpToFinishLevel() const {
	return true;
}

GoldNugget::GoldNugget(StudentWorld* world, int startX, int startY, bool temporary, bool onPlayer, bool visible) :
	ActivatingObject(world, startX, startY, IID_GOLD, SOUND_GOT_GOODIE, onPlayer, (!onPlayer), true, visible) {
	m_state = temporary;
	if (temporary) {
		setTime(100);
	}
}

void GoldNugget::move() {
	check();

	if (getActOnPlay() && getWorld()->isNearIceMan(this, 3)) {
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->increaseScore(10);
		getWorld()->getIceMan()->addGold();
		return;
	}

	Actor* nearAgent = getWorld()->findNearbyPickerUpper(this, 3);
	if (!getActOnPlay() && nearAgent != nullptr) {
		if (nearAgent->huntsIceMan()) {
			setDead();
			getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
			nearAgent->bribe();
			getWorld()->increaseScore(25);
			return;
		}
	}

	if (m_state) {
		if (getTime() == 0) {
			setDead();
		}

		setTime(getTime() - 1);
	}
}

SonarKit::SonarKit(StudentWorld* world, int time) :
	ActivatingObject(world, 0, 60, IID_SONAR, SOUND_GOT_GOODIE, true, false, false, true)
{
	setTime(time);

}

void SonarKit::move()
{
	if (!isAlive()) return;
	if (getWorld()->isNearIceMan(this, 3)) {
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->increaseScore(75);
		getWorld()->getIceMan()->addSonar();
		return;
	}
	else if (getTime() == 0) {
		setDead();
		return;
	}

	setTime(getTime() - 1);

}


WaterPool::WaterPool(StudentWorld* world, int startX, int startY, int time) :
	ActivatingObject(world, startY, startX, IID_WATER_POOL, SOUND_GOT_GOODIE, true, false, false, true)
{
	setTime(time);
}

void WaterPool::move()
{
	if (!isAlive()) return;
	if (getWorld()->isNearIceMan(this, 3)) {
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->increaseScore(100);
		getWorld()->getIceMan()->addWater();
		return;
	}

	else if (getTime() == 0) {
		setDead();
		return;
	}

	setTime(getTime() - 1);
}

RegularProtester::RegularProtester(StudentWorld* world, int startX, int startY, int imageID) :
	Protester(world, startX, startY, imageID, 5, 0)
{}

void RegularProtester::move()
{
	if (!isAlive()) return;

	if (getTicks() > 0) {
		setTicks(getTicks() - 1);
		return;
	}

	setTicks(getWaitTime());

	if (getState() == LEAVE) {
		if (getX() == 60 && getY() == 60) {
			setDead();
			return;
		}

		Direction wayToMove = getWorld()->determineFirstMoveToExit(getX(), getY());
		switch (wayToMove)
		{
		case GraphObject::up:
			moveToIfPossible2(getX(), getY() + 1);
			break;
		case GraphObject::down:
			moveToIfPossible2(getX(), getY() - 1);
			break;
		case GraphObject::left:
			moveToIfPossible2(getX() - 1, getY());
			break;
		case GraphObject::right:
			moveToIfPossible2(getX() + 1, getY());
			break;
		}
		return;
	}

	if (getWorld()->isNearIceMan(this, 4) && getWorld()->facingTowardIceMan(this)) {
		if (getSinShout() == 15) {
			getWorld()->playSound(SOUND_PROTESTER_YELL);
			getWorld()->annoyIceMan();
			setSinShout(0);
			setSinTurn(getSinTurn() + 1);
			return;
		}
		else {
			setSinShout(getSinShout() + 1);
			setSinTurn(getSinTurn() + 1);
		}
	}

	cout << getWorld()->lineOfSightToIceMan(this) << endl;
	if (getWorld()->lineOfSightToIceMan(this) != none) {
		Direction dir = getWorld()->lineOfSightToIceMan(this);
		setDirection(dir);
		regularMove(getX(), getY(), dir);
		setSinTurn(getMovedir());
		setSinShout(getSinShout() + 1);
		return;
	}

	if (getSinTurn() == getMovedir()) { // Needs to pick a new direction
		Direction newDir = newDirection();
		setDirection(newDir);

		setMovedir((rand() % 53) + 8);
	}
	else if (getSinTurn() >= 200)
	{
		if (regularMove(getX(), getY(), getDirection())) {
			setSinTurn(getSinTurn() + 1);
		}
		else {
			Direction newDir = newDirection();
			setDirection(newDir);
			setMovedir((rand() % 53) + 8);

		}
	}

	if (!regularMove(getX(), getY(), getDirection())) {
		setSinTurn(getMovedir());
		setSinShout(getSinShout() + 1);
	}
	else {
		setSinTurn(getSinTurn() + 1);
		setSinShout(getSinShout() + 1);
	}
}

void RegularProtester::addGold()
{
	getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
	getWorld()->increaseScore(25);
	setState(LEAVE);
}

HardcoreProtester::HardcoreProtester(StudentWorld* world, int startX, int startY, int imageID) : 
	Protester(world, startX, startY, imageID, 20, 0)
{}

void HardcoreProtester::move()
{
	if (!isAlive()) return;

	if (getTicks() > 0) {
		setTicks(getTicks() - 1);
		return;
	}

	setTicks(getWaitTime());

	if (getState() == LEAVE) {
		if (getX() == 60 && getY() == 60) {
			setDead();
			return;
		}

		Direction wayToMove = getWorld()->determineFirstMoveToExit(getX(), getY());
		switch (wayToMove)
		{
		case GraphObject::up:
			moveToIfPossible2(getX(), getY() + 1);
			break;
		case GraphObject::down:
			moveToIfPossible2(getX(), getY() - 1);
			break;
		case GraphObject::left:
			moveToIfPossible2(getX() - 1, getY());
			break;
		case GraphObject::right:
			moveToIfPossible2(getX() + 1, getY());
			break;
		}
		return;
	}

	if (getWorld()->isNearIceMan(this, 4) && getWorld()->facingTowardIceMan(this)) {
		if (getSinShout() == 15) {
			getWorld()->playSound(SOUND_PROTESTER_YELL);
			getWorld()->annoyIceMan();
			setSinShout(0);
			setSinTurn(getSinTurn() + 1);
			return;
		}
		else {
			setSinShout(getSinShout() + 1);
			setSinTurn(getSinTurn() + 1);
		}
	}

	int M = 16 + getWorld()->getLevel() * 2;
	if (getWorld()->determineHowManyMoves(getX(), getY()) <= 16) {
		regularMove(getX(), getY(), getWorld()->determineFirstMoveToIceMan(getX(), getY()));
	}

	cout << getWorld()->lineOfSightToIceMan(this) << endl;
	if (getWorld()->lineOfSightToIceMan(this) != none) {
		setDirection(getWorld()->lineOfSightToIceMan(this));
		setSinTurn(0);
		setSinShout(getSinShout() + 1);
		return;
	}

	if (getSinTurn() == getMovedir()) { // Needs to pick a new direction
		Direction newDir = newDirection();
		setDirection(newDir);

		setMovedir((rand() % 53) + 8);
	}
	else if (getSinTurn() >= 200)
	{
		if (regularMove(getX(), getY(), getDirection())) {
			setSinTurn(getSinTurn() + 1);
		}
		else {
			Direction newDir = newDirection();
			setDirection(newDir);
			setMovedir((rand() % 53) + 8);

		}
	}

	if (!regularMove(getX(), getY(), getDirection())) {
		setSinTurn(getMovedir());
		setSinShout(getSinShout() + 1);
	}
	else {
		setSinTurn(getSinTurn() + 1);
		setSinShout(getSinShout() + 1);
	}
}

void HardcoreProtester::addGold()
{
	getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
	getWorld()->increaseScore(50);
	int maxTicks = 100 - getWorld()->getLevel() * 10;
	int ticks_to_stare = max(50, maxTicks);
	setTicks(ticks_to_stare);
}

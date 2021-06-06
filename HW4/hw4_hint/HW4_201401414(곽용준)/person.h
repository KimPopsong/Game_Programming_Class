#pragma once
class Person {
	int health = 3;
public:
	Person(int health = 3);
	int getHealth() { return health; };
	void setHealth(int h) { health = h; };
	void damaged() { this->health--; };
};
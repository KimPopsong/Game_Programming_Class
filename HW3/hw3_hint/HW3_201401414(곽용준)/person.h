#pragma once
class Person {
	int health = 100;
public:
	Person(int health = 100);
	int getHealth() { return health; };
	void setHealth(int h) { health = h; };
	void damaged(int);
};
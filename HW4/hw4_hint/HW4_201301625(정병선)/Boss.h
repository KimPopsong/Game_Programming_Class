#pragma once
#include <d2d1.h>
#include <d2d1helper.h>
#include "Hw3.h"
#include "Bullet.h"
#include "FiniteStateMachine.h"


class Boss
{
public:
	enum StateID {
		STATE_UNDEFINED = 0,
		STATE_STAND,
		STATE_MOVE,
		STATE_MOVECENTER
	};

	enum EventID {
		EVENT_UNDEFINED = 0,
		EVENT_NORMAL,
		EVENT_ARRIVED,
		EVENT_STOPMOVE, //이동중지
		EVENT_5000, //피 5000이하
		
	};

	BOOL Visible;
	int hp = 10000;
	D2D1_RECT_F Source;
	D2D1_POINT_2F Position;
	D2D1_POINT_2F destPosition;
	int count=0;
	FiniteStateMachine* stateMachine;
	Bullets attack1_b[10];

public:
	Boss() {
		stateMachine = new FiniteStateMachine();

		DWORD BossStateTransitions[][3] = {
			{STATE_STAND, EVENT_NORMAL, STATE_MOVE},
			{STATE_STAND,EVENT_5000,STATE_MOVECENTER},
			{STATE_MOVE,EVENT_5000,STATE_MOVECENTER}
			
		};

		for (int i = 0; i < 3; i++) {
			stateMachine->addStateTransition(BossStateTransitions[i][0],
				BossStateTransitions[i][1], BossStateTransitions[i][2]);
		}

		stateMachine->setCurrentStateID(STATE_STAND);

		Position.x = 350;
		Position.y = 200;
	}
	void update(D2D1_POINT_2F dest) {
		switch (stateMachine->getCurrentStateID()) {

		case STATE_STAND:
			count += 1;
			if (hp > 5000) {
				if (count > 60) {
					count = 0;
					stateMachine->issueEvent(EVENT_NORMAL);
					break;
				}
			}
			else {
				stateMachine->issueEvent(EVENT_5000);
				break;
			}
			break;
		case STATE_MOVECENTER:
			Position.x = 350;
			Position.y = 200;;
			break;
		case STATE_MOVE:
			destPosition.x = dest.x+30;
			Position.x += (destPosition.x - Position.x) / 20;
			if (hp < 5000) {
				stateMachine->issueEvent(EVENT_5000);
				break;
			}
			break;
		}
	}
	

};




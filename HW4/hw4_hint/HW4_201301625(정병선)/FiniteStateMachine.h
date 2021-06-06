#pragma once
#include <windows.h>
#include <map>
#include <functional>

#define UNDEFINED		0

struct State
{
	DWORD stateID;
	std::map<DWORD, DWORD> transitions; //(key,value)=(inputEvent,outputStateID)

	State(DWORD _stateID) { stateID = _stateID; }
	~State() { transitions.clear(); }
	DWORD getStateID() { return stateID; }
	DWORD getNumTransitions() { return transitions.size(); }
	void addTransition(DWORD inputEvent, DWORD outputStateID) { transitions[inputEvent] = outputStateID; }
	void deleteTransition(DWORD inputEvent) { transitions.erase(inputEvent); }
	DWORD getOutputStateID(DWORD inputEvent)
	{
		// 입력이벤트에 따라서 전이 조건에 해당하는 전이후의 상태를 찾아낸다.
		std::map<DWORD, DWORD>::iterator iter = transitions.find(inputEvent);
		if (iter == transitions.end()) { return UNDEFINED; } // 등록된 전이가 없음.
		else return transitions[inputEvent];
	}
};


class FiniteStateMachine
{
private:
	std::map<DWORD, State*> states; //(key,value)=(stateID, state object)
	DWORD currentStateID;

public:
	FiniteStateMachine() {
		currentStateID = UNDEFINED;
	}

	~FiniteStateMachine()
	{
		std::map<DWORD, State*>::iterator iter = states.begin();
		while (iter != states.end()) {
			State* state = iter->second;
			if (state != NULL) delete state;
			iter++;
		}
		states.clear();
	}

	//현재 상태 ID 가져오기
	DWORD getCurrentStateID() {
		return currentStateID;
	}

	//상태 전이 정보 추가
	void addStateTransition(DWORD stateID, DWORD inputEvent, DWORD outputStateID)
	{
		// 동일한 State가 이미 존재하는지를 알아본다.
		std::map<DWORD, State*>::iterator iter = states.find(stateID);
		if (iter == states.end()) { // 새 상태를 만든다.
			states[stateID] = new State(stateID);
		}

		// 상태 전이 정보를 추가
		states[stateID]->addTransition(inputEvent, outputStateID);
	}

	//상태 전이 정보 삭제
	void deleteTransition(DWORD stateID, DWORD inputEvent)
	{
		std::map<DWORD, State*>::iterator iter = states.find(stateID);
		if (iter == states.end()) return; //상태가 없음.

		State* state = iter->second;
		// State의 전이 정보 삭제
		state->deleteTransition(inputEvent);
		if (state->getNumTransitions() == 0) { // 더이상 전이가 없다면 상태 삭제
			delete state;
			states.erase(iter);
		}
	}

	//상태 ID 설정
	void setCurrentStateID(DWORD stateID)
	{
		std::map<DWORD, State*>::iterator iter = states.find(stateID);
		if (iter == states.end()) return; //stateID에 해당하는 상태가 없음.

		currentStateID = stateID;
	}

	//이벤트 발생시 상태 전이
	void issueEvent(int inputEvent)
	{
		// 입력 이벤트에 따라서 상태전이를 수행하여 현재 상태를 바꿈.
		if (currentStateID == UNDEFINED) return; // 현재 상태가 정의되어 있지 않음.

		DWORD outputStateID = states[currentStateID]->getOutputStateID(inputEvent);
		if (outputStateID == UNDEFINED) return; //상태 전이 못함.
		currentStateID = outputStateID; //현재 상태를 바꿈.
	}

};

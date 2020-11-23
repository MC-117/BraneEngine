#pragma once
#ifndef _ANIMATIONSTATEMACHINE_H_
#define _ANIMATIONSTATEMACHINE_H_

#include "Unit.h"

class AnimationStateMachine
{
public:
	typedef bool(*TransmitionFunc)(AnimationStateMachine*, void*);
	struct Transmition;
	struct State
	{
		string name;
		string animationName;
		map<State*, int> transmitionList;
		vector<Transmition> transmitions;

		bool addTransmition(State* state, TransmitionFunc func, void* ptr = NULL);
	};
	struct Transmition
	{
		State* target;
		TransmitionFunc func;
		void* ptr;
	};

	State* getCurrentState();
	State* getState(const string& name);
	void setBool(const string& name, bool value);
	bool getBool(const string& name);
	bool addState(const string& name, const string& animationName);
	bool addTransmition(const string& from, const string& to, TransmitionFunc func, void* ptr = NULL);

	bool update(float deltaTime);
protected:
	map<string, State> states;
	map<string, bool> boolField;

	State* initialState = NULL;
	State* currentState = NULL;
};

#endif // !_ANIMATIONSTATEMACHINE_H_

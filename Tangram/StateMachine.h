#pragma once
#ifndef _STATEMACHINE_H_
#define _STATEMACHINE_H_

#include "Unit.h"

class IState
{
public:
	virtual void enterState() const = 0;
	virtual void inState(float deltaTime) const = 0;
	virtual void leaveState() const = 0;
};

class ITransition
{
public:
	virtual bool canTransit() const = 0;
};

class StateMachine
{
public:
	typedef bool(*ConditionFunc)(StateMachine*, const string& tag, void*);

	struct state_node;

	struct transition_node
	{
		ITransition* transition;
		state_node* from;
		state_node* to;
	};

	struct transition_node_sort
	{
		bool operator()(const transition_node& a, const transition_node& b) const {
			if (a.from < b.from)
				return true;
			else
				return a.to < b.to;
		}
	};

	struct state_node
	{
		const string name;
		IState* state;
		set<transition_node*> transitions;

		state_node(const string& name) : name(name) {};
		state_node(const string& name, IState& state) : name(name), state(&state) {};
	};
	struct state_sort
	{
		bool operator()(const state_node& a, const state_node& b) const {
			return a.name < b.name;
		}
	};
	set<state_node, state_sort> states;
	set<transition_node, transition_node_sort> transitions;

	state_node* startState = NULL;
	state_node* currentState = NULL;

	map<string, bool> boolField;
	map<string, float> floatField;
	map<string, string> stringField;

	StateMachine();

	state_node* getState(const string& name) const;
	transition_node* getTransitions(const string& from, const string& to) const;
	bool addState(const string& name, IState& state);
	bool removeState(const string& name);
	bool setStartState(const string& name);
	bool addTransition(ITransition& transition, const string& from, const string& to);
	bool removeTransition(const string& from, const string& to);

	void update(float deltaTime);
};

#endif // !_STATEMACHINE_H_

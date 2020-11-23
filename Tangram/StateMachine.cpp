#include "StateMachine.h"

StateMachine::StateMachine()
{
	
}

StateMachine::state_node * StateMachine::getState(const string & name) const
{
	auto iter = states.find(state_node(name));
	if (iter != states.end())
		return &iter._Ptr->_Myval;
	return NULL;
}

StateMachine::transition_node * StateMachine::getTransitions(const string & from, const string & to) const
{
	state_node f = state_node(from), t = state_node(to);
	transition_node tn;
	tn.from = &f;
	tn.to = &t;
	auto iter = transitions.find(tn);
	if (iter != transitions.end())
		return &iter._Ptr->_Myval;
	return NULL;
}

bool StateMachine::addState(const string& name, IState & state)
{
	return states.emplace(state_node(name, state)).second;
}

bool StateMachine::removeState(const string & name)
{
	auto iter = states.find(state_node(name));
	if (iter != states.end()) {
		states.erase(iter);
		return true;
	}
	return false;
}

bool StateMachine::setStartState(const string & name)
{
	auto iter = states.find(state_node(name));
	if (iter != states.end()) {
		startState = &iter._Ptr->_Myval;
		return true;
	}
	return false;
}

bool StateMachine::addTransition(ITransition& transition, const string & from, const string & to)
{
	state_node* f = getState(from);
	state_node* t = getState(to);
	if (f == NULL || t == NULL)
		return false;
	transition_node tn = { &transition, f, t };
	auto iter = transitions.find(tn);
	if (iter != transitions.end())
		return false;
	f->transitions.emplace(&transitions.emplace(tn).first._Ptr->_Myval);
	return true;
}

bool StateMachine::removeTransition(const string & from, const string & to)
{
	state_node* f = getState(from);
	state_node* t = getState(to);
	if (f == NULL || t == NULL)
		return false;
	transition_node tn;
	tn.from = f;
	tn.to = t;
	auto iter = transitions.find(tn);
	if (iter != transitions.end())
		return false;
	transitions.erase(iter);
	auto _iter = f->transitions.find(&iter._Ptr->_Myval);
	f->transitions.erase(_iter);
	return true;
}

void StateMachine::update(float deltaTime)
{
	if (currentState == NULL) {
		if (startState == NULL) {
			if (states.empty())
				return;
			else
				startState = &states.begin()._Ptr->_Myval;
		}
		currentState = startState;
		currentState->state->enterState();
	}
	for (auto b = currentState->transitions.begin(), e = currentState->transitions.end();
		b != e; b++)
		if ((*b)->transition->canTransit()) {
			currentState->state->leaveState();
			currentState = (*b)->to;
			currentState->state->enterState();
			break;
		}
	currentState->state->inState(deltaTime);
}

#include "AnimationStateMachine.h"

bool AnimationStateMachine::State::addTransmition(State * state, TransmitionFunc func, void* ptr)
{
	auto iter = transmitionList.find(state);
	if (iter != transmitionList.end())
		return false;
	transmitionList.insert(pair<State*, int>(state, transmitions.size()));
	transmitions.push_back({ state, func, ptr });
	return true;
}

AnimationStateMachine::State * AnimationStateMachine::getCurrentState()
{
	return currentState;
}

AnimationStateMachine::State * AnimationStateMachine::getState(const string & name)
{
	auto iter = states.find(name);
	if (iter == states.end())
		return NULL;
	return &iter->second;
}

void AnimationStateMachine::setBool(const string & name, bool value)
{
	boolField[name] = value;
}

bool AnimationStateMachine::getBool(const string & name)
{
	auto iter = boolField.find(name);
	if (iter == boolField.end())
		return false;
	return iter->second;
}

bool AnimationStateMachine::addState(const string & name, const string & animationName)
{
	if (getState(name) != NULL)
		return false;
	states.insert(pair<string, State>(name, { name, animationName }));
	return true;
}

bool AnimationStateMachine::addTransmition(const string & from, const string & to, TransmitionFunc func, void* ptr)
{
	State* fs = getState(from);
	if (fs == NULL)
		return false;
	State* ts = getState(to);
	if (ts == NULL)
		return false;
	fs->addTransmition(ts, func, ptr);
	return true;
}

bool AnimationStateMachine::update(float deltaTime)
{
	if (states.empty())
		return false;
	if (currentState == NULL) {
		if (initialState == NULL) {
			initialState = &states[0];
		}
		currentState = initialState;
	}
	for (int i = 0; i < currentState->transmitions.size(); i++) {
		Transmition& tran = currentState->transmitions[i];
		if (tran.func(this, tran.ptr)) {
			currentState = tran.target;
			return true;
		}
	}
	return false;
}

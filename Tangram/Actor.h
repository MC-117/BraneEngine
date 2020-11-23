#pragma once
#ifndef _ACTOR_H_
#define _ACTOR_H_

#include "Transform.h"
#include "AudioSource.h"

class Actor : public ::Transform
{
public:
	Serialize(Actor);

	vector<AudioSource*> audioSources;

	Actor(string name = "Actor");
	~Actor();

	virtual void begin();
	virtual void tick(float deltaTime);
	virtual void afterTick();
	virtual void end();
	virtual void prerender();

	virtual void setHidden(bool value);
	virtual bool isHidden();
	virtual AudioSource* addAudioSource(AudioData& audioData);

	static Serializable* instantiate(const SerializationInfo& from);
	virtual bool deserialize(const SerializationInfo& from);
	virtual bool serialize(SerializationInfo& to);
};

#endif // !_ACTOR_H_

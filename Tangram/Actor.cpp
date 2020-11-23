#include "Actor.h"
#include "Asset.h"

SerializeInstance(Actor);

Actor::Actor(string name) : ::Transform::Transform(name)
{
}

Actor::~Actor()
{
	for (auto b = audioSources.begin(), e = audioSources.end(); b != e; b++)
		delete *b;
}

void Actor::begin()
{
	::Transform::begin();
}

void Actor::tick(float deltaTime)
{
	::Transform::tick(deltaTime);
}

void Actor::afterTick()
{
	::Transform::afterTick();
	for (auto b = audioSources.begin(), e = audioSources.end(); b != e; b++)
		(*b)->setPosition(getPosition(WORLD));
}

void Actor::end()
{
	::Transform::end();
}

void Actor::prerender()
{
}

void Actor::setHidden(bool value)
{
}

bool Actor::isHidden()
{
	return false;
}

AudioSource * Actor::addAudioSource(AudioData & audioData)
{
	if (!audioData.isLoad())
		return NULL;
	AudioSource* s = new AudioSource(audioData);
	audioSources.push_back(s);
	return s;
}

Serializable * Actor::instantiate(const SerializationInfo & from)
{
	return new Actor(from.name);
}

bool Actor::deserialize(const SerializationInfo & from)
{
	if (!::Transform::deserialize(from))
		return false;
	const SerializationInfo* ainfo = from.get("Audio");
	if (ainfo != NULL) {
		for (int i = 0; i < ainfo->stringList.size(); i++) {
			AudioData* audioData = getAssetByPath<AudioData>(ainfo->stringList[i]);
			if (audioData != NULL)
				addAudioSource(*audioData);
		}
	}
	return true;
}

bool Actor::serialize(SerializationInfo & to)
{
	if (!::Transform::serialize(to))
		return false;
	to.type = "Actor";
	SerializationInfo* ainfo = to.add("Audio");
	if (ainfo == NULL)
		return false;
	ainfo->type = "Array";
	ainfo->arrayType = "String";
	for (int i = 0; i < audioSources.size(); i++) {
		string path = AudioDataAssetInfo::getPath(audioSources[i]->audioData);
		if (!path.empty())
			ainfo->push(path);
	}
	return true;
}

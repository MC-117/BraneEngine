#include "Entity.h"

Entity::Entity()
{
	regist();
	if (!Brane::born(typeId, this))
		throw std::runtime_error("Initial failed");
}

Entity::~Entity()
{
}

void Entity::Destroy()
{
	Brane::vanish(typeId, this);
	delete this;
}

void Entity::regist()
{
	typeId = typeid(Entity).hash_code();
}

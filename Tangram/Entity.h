#pragma once
#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "Brane.h"

#define REG(ClassName) protected: virtual void regist() { typeId = typeid(ClassName).hash_code(); };

class Entity
{
public:
	Entity();
	virtual ~Entity();

	void Destroy();
protected:
	TypeID typeId = 0;
	virtual void regist();
};

#endif // !_ENTITY_H_

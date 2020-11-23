#pragma once
#ifndef _PHYSICALMATERIAL_H_
#define _PHYSICALMATERIAL_H_

#include "Unit.h"

enum PhysicalType {
	STATIC, DYNAMIC, NOCOLLISIOIN
};

class PhysicalMaterial {
public:
	float mass;
	PhysicalType physicalType;

	PhysicalMaterial(float mass = 0, PhysicalType physicalType = STATIC);
	PhysicalMaterial(const PhysicalMaterial& mat);
	PhysicalMaterial(PhysicalMaterial&& mat);

	PhysicalMaterial& operator=(const PhysicalMaterial& mat);
};

#endif // !_PHYSICALMATERIAL_H_

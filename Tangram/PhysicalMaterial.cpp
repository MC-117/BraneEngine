#include "PhysicalMaterial.h"

PhysicalMaterial::PhysicalMaterial(float mass, PhysicalType physicalType) : mass(mass), physicalType(physicalType)
{
	if (physicalType == DYNAMIC && mass == 0)
		mass = 0.00001;
}

PhysicalMaterial::PhysicalMaterial(const PhysicalMaterial & mat)
{
	mass = mat.mass;
	physicalType = mat.physicalType;
}

PhysicalMaterial::PhysicalMaterial(PhysicalMaterial && mat)
{
	mass = mat.mass;
	physicalType = mat.physicalType;
}

PhysicalMaterial & PhysicalMaterial::operator=(const PhysicalMaterial & mat)
{
	mass = mat.mass;
	physicalType = mat.physicalType;
	return *this;
}

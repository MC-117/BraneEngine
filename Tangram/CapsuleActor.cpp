#include "CapsuleActor.h"
#include "Importer.h"
#include "PhysicalWorld.h"

Mesh CapsuleActor::hemisphere;
Mesh CapsuleActor::column;
bool CapsuleActor::isLoadDefaultResource = false;

CapsuleActor::CapsuleActor(const string & name)
	: capsuleShape(), CollisionActor(&capsuleShape, PhysicalMaterial(), name), upHemisphereRender(hemisphere, Material::defaultMaterial),
	downHemisphereRender(hemisphere, Material::defaultMaterial),
	columnRender(column, Material::defaultMaterial)
{
	loadDefaultResource();
	updataRigidBody(&capsuleShape);
	upHemisphereRender.canCastShadow = false;
	downHemisphereRender.canCastShadow = false;
	columnRender.canCastShadow = false;
}

CapsuleActor::CapsuleActor(const Capsule & capsuleShape, const PhysicalMaterial & physicalMaterial, const string & name)
	: capsuleShape(capsuleShape), CollisionActor(&(this->capsuleShape), physicalMaterial, name), upHemisphereRender(hemisphere, Material::defaultMaterial),
	downHemisphereRender(hemisphere, Material::defaultMaterial),
	columnRender(column, Material::defaultMaterial)
	
{
	loadDefaultResource();
	updataRigidBody(&this->capsuleShape, SIMPLE, physicalMaterial);
	upHemisphereRender.canCastShadow = false;
	downHemisphereRender.canCastShadow = false;
	columnRender.canCastShadow = false;
}

void CapsuleActor::setRadius(float radius)
{
	capsuleShape.setRadius(radius);
}

void CapsuleActor::setHalfLength(float halfLength)
{
	capsuleShape.setHalfLength(halfLength);
}

float CapsuleActor::getRadius()
{
	return capsuleShape.getRadius();
}

float CapsuleActor::getHalfLength()
{
	return capsuleShape.getWidth() / 2.0f;
}

void CapsuleActor::updataRigidBody(Shape * shape, ShapeComplexType complexType, const PhysicalMaterial & physicalMaterial)
{
	CollisionActor::updataRigidBody(&capsuleShape);
}

void CapsuleActor::prerender()
{
	CollisionActor::prerender();
	float r = capsuleShape.getRadius();
	float l = capsuleShape.getWidth() / 2.0f - r;
	Vector3f wpos = getPosition(WORLD);
	Quaternionf wrot = getRotation(WORLD);
	Matrix4f PT = Matrix4f::Identity();
	PT.block(0, 3, 3, 1) = wpos;
	PT.block(0, 0, 3, 3) = wrot.toRotationMatrix();
	Matrix4f US = Matrix4f::Identity(), DR = Matrix4f::Identity();
	Matrix4f HT = Matrix4f::Identity(), CS = Matrix4f::Identity();
	US(0, 0) = r;
	US(1, 1) = r;
	US(2, 2) = r;
	CS(0, 0) = r;
	CS(1, 1) = l;
	CS(2, 2) = r;
	AngleAxisf rot = AngleAxisf(PI, Vector3f(0, 0, 1));
	DR.block(0, 0, 3, 3) = rot.toRotationMatrix();
	HT(1, 3) = l;
	upHemisphereRender.transformMat = PT * HT * US;
	HT(1, 3) = -l;
	downHemisphereRender.transformMat = PT * HT * DR * US;
	columnRender.transformMat = PT * CS;

	unsigned int transID = RenderCommandList::setMeshTransform(upHemisphereRender.transformMat);
	upHemisphereRender.instanceID = transID;
	for (int i = 0; i < hemisphere.meshParts.size(); i++)
		RenderCommandList::setMeshPartTransform(&hemisphere.meshParts[i], upHemisphereRender.materials[i], transID);

	transID = RenderCommandList::setMeshTransform(downHemisphereRender.transformMat);
	downHemisphereRender.instanceID = transID;
	for (int i = 0; i < hemisphere.meshParts.size(); i++)
		RenderCommandList::setMeshPartTransform(&hemisphere.meshParts[i], downHemisphereRender.materials[i], transID);

	transID = RenderCommandList::setMeshTransform(columnRender.transformMat);
	columnRender.instanceID = transID;
	for (int i = 0; i < column.meshParts.size(); i++)
		RenderCommandList::setMeshPartTransform(&column.meshParts[i], columnRender.materials[i], transID);
}

Render * CapsuleActor::getRender()
{
	return &columnRender;
}

unsigned int CapsuleActor::getRenders(vector<Render*>& renders)
{
	renders.push_back(&upHemisphereRender);
	renders.push_back(&downHemisphereRender);
	renders.push_back(&columnRender);
	return 3;
}

void CapsuleActor::setHidden(bool value)
{
	upHemisphereRender.hidden = value;
	downHemisphereRender.hidden = value;
	columnRender.hidden = value;
}

bool CapsuleActor::isHidden()
{
	return upHemisphereRender.hidden &&
		downHemisphereRender.hidden &&
		columnRender.hidden;
}

void CapsuleActor::loadDefaultResource()
{
	if (isLoadDefaultResource)
		return;
	Importer imph = Importer("Engine/Shapes/hemisphere.fbx");
	if (!imph.getMesh(hemisphere))
		throw runtime_error("Load Engine/Shapes/hemisphere.fbx failed");
	Importer impc = Importer("Engine/Shapes/column.fbx");
	if (!impc.getMesh(column))
		throw runtime_error("Load Engine/Shapes/column.fbx failed");
	isLoadDefaultResource = true;
}

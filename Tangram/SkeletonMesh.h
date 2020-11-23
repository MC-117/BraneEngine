#pragma once
#ifndef _SKELETONMESH_H_
#define _SKELETONMESH_H_

#include "Mesh.h"

struct SkeletonMeshPart;

class MorphMeshData
{
public:
	// Data structure: |     vertex 0      |     vertex 1      |
	//                 | morph 0 | morph 1 | morph 0 | morph 1 |
	//                 | op | on | op | on | op | on | op | on |
	// op: offsetPosition	on: offsetNormal
	vector<Vector4f> verticesAndNormals;
	unsigned int vertexCount = 0;
	unsigned int morphCount = 0;

	GLBuffer morphWeightBuffer = GLBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(float));

	MorphMeshData();
	MorphMeshData(unsigned int vertexCount, unsigned int morphCount);

	void init(unsigned int vertexCount, unsigned int morphCount);

	unsigned int bindBase(unsigned int meshId, unsigned int weightId);
	void unbindBase(unsigned int meshId, unsigned int weightId);
protected:
	unsigned int sbo = 0;
};

class SkeletonMeshData : public MeshData
{
public:
	static set<SkeletonMeshData*> SkeletonMeshDataCollection;
	static SkeletonMeshData StaticSkeletonMeshData;

	vector<Matrix<unsigned int, VERTEX_MAX_BONE, 1>> boneIndexes;
	vector<Matrix<float, VERTEX_MAX_BONE, 1>> weights;
	unsigned int boneCount;
	//vector<Matrix4f> boneOffetMatrix;
	MorphMeshData morphMeshData;

	SkeletonMeshData();

	virtual void updateMorphWeights(vector<float>& weights);
	virtual unsigned int bindShape();
	static SkeletonMeshPart newSkeletonMeshPart(unsigned int vertCount, unsigned int elementCount, unsigned int boneCount, unsigned int morphVertCount = 0, unsigned int morphCount = 0);
protected:
	unsigned int bbo = NULL;
	unsigned int wbo = NULL;
};

struct SkeletonMeshPart : public MeshPart
{
	MorphMeshData* morphMeshData = NULL;

	SkeletonMeshPart();
	SkeletonMeshPart(SkeletonMeshData* meshData, unsigned int vertexFirst, unsigned int vertexCount,
		unsigned int indexFirst, unsigned int indexCount, MorphMeshData* morphData = NULL);

	inline Matrix<unsigned int, VERTEX_MAX_BONE, 1>& boneIndex(unsigned int index) {
		if (index >= vertexCount)
			throw overflow_error("SkeletonMeshPart boneIndex access overflow");
		return ((SkeletonMeshData*)meshData)->boneIndexes[vertexFirst + index];
	}
	inline Matrix<float, VERTEX_MAX_BONE, 1>& weight(unsigned int index) {
		if (index >= vertexCount)
			throw overflow_error("SkeletonMeshPart weight access overflow");
		return ((SkeletonMeshData*)meshData)->weights[vertexFirst + index];
	}
	inline Vector4f& morphVertex(unsigned int vertexIndex, unsigned int morphIndex) {
		if (morphMeshData == NULL)
			throw overflow_error("No MorphMeshData");
		if (morphIndex >= morphMeshData->morphCount || (vertexIndex + vertexFirst) >= morphMeshData->vertexCount)
			throw overflow_error("MorphMeshData vertex access overflow");
		return morphMeshData->verticesAndNormals[((vertexIndex + vertexFirst) * morphMeshData->morphCount + morphIndex) * 2];
	}

	inline Vector4f& morphNormal(unsigned int normalIndex, unsigned int morphIndex) {
		if (morphMeshData == NULL)
			throw overflow_error("No MorphMeshData");
		if (morphIndex >= morphMeshData->morphCount || (normalIndex + vertexFirst) >= morphMeshData->vertexCount)
			throw overflow_error("MorphMeshData normal access overflow");
		return morphMeshData->verticesAndNormals[((normalIndex + vertexFirst) * morphMeshData->morphCount + morphIndex) * 2 + 1];
	}
	operator MeshPart();
};

class SkeletonData;

struct BoneData
{
	string name;
	unsigned int index;
	unsigned int siblingIndex;
	Matrix4f offsetMatrix;
	Matrix4f transformMatrix;
	SkeletonData* skeletonData;
	BoneData* parent;
	vector<BoneData*> children;

	unsigned int addChild(const string& name, const Matrix4f& offsetMatrix, const Matrix4f & transformMatrix);
	BoneData* getParent();
	void setParent(BoneData* parent);
	BoneData* getChild(unsigned int index = 0);
	BoneData* getSibling(unsigned int index = 0);
	BoneData* getNext(BoneData* limitBone = NULL);
};

class SkeletonData
{
public:
	map<string, unsigned int> boneName;
	vector<BoneData> boneList;
	unsigned int baseBoneOffsetMatIndex;

	BoneData* rootBone = NULL;

	BoneData* getBoneData(const string& name);
	BoneData* getBoneData(unsigned int index);
};

class SkeletonMesh : public Mesh
{
public:
	SkeletonData skeletonData;
	vector<string> morphName;
	vector<bool> partHasMorph;

	virtual void updateMorphWeights(vector<float>& weights);
};

#endif // !_SKELETONMESH_H_

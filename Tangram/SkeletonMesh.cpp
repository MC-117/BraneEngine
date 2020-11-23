#include "SkeletonMesh.h"
#include "Shader.h"
#include "Utility.h"

set<SkeletonMeshData*> SkeletonMeshData::SkeletonMeshDataCollection;
SkeletonMeshData SkeletonMeshData::StaticSkeletonMeshData;

MorphMeshData::MorphMeshData()
{
}

MorphMeshData::MorphMeshData(unsigned int vertexCount, unsigned int morphCount)
{
	init(vertexCount, morphCount);
}

void MorphMeshData::init(unsigned int vertexCount, unsigned int morphCount)
{
	this->vertexCount = vertexCount;
	this->morphCount = morphCount;
	morphWeightBuffer.resize(morphCount);
	verticesAndNormals.resize(vertexCount * morphCount * 2);
}

unsigned int MorphMeshData::bindBase(unsigned int meshId, unsigned int weightId)
{
	if (sbo == 0) {
		glGenBuffers(1, &sbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, sbo);
		unsigned int offset = sizeof(float) * 4;
		float morphCountV[] = { morphCount, morphCount, morphCount, 1.0 };
		unsigned int size = sizeof(float) * 4 * verticesAndNormals.size();
		glBufferData(GL_SHADER_STORAGE_BUFFER, offset + size, NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, offset, morphCountV);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, verticesAndNormals.data()->data());
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, meshId, sbo);
	morphWeightBuffer.bindBase(weightId);
	return sbo;
}
void MorphMeshData::unbindBase(unsigned int meshId, unsigned int weightId)
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, meshId, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, weightId, 0);
}

SkeletonMeshData::SkeletonMeshData()
{
	id = typeid(SkeletonMeshData).hash_code();
	SkeletonMeshDataCollection.insert(this);
}

void SkeletonMeshData::updateMorphWeights(vector<float> & weights)
{
	morphMeshData.morphWeightBuffer.uploadData(weights.size(), weights.data());
	morphMeshData.morphWeightBuffer.unbind();
}

unsigned int SkeletonMeshData::bindShape()
{
	if (vao != 0 && vao == currentVao)
		return vao;
	glBindVertexArray(0);
	if (vao == NULL || vbo == NULL || (ubo == NULL && uvs.size() != 0) || (nbo == NULL && normals.size() != 0) || ibo == NULL ||
		bbo == NULL || wbo == NULL) {
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size() * 3, vertices.begin()->data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		if (uvs.size() != 0) {
			glGenBuffers(1, &ubo);
			glBindBuffer(GL_ARRAY_BUFFER, ubo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * uvs.size() * 2, uvs.begin()->data(), GL_STATIC_DRAW);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
		}

		if (normals.size() != 0) {
			glGenBuffers(1, &nbo);
			glBindBuffer(GL_ARRAY_BUFFER, nbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * normals.size() * 3, normals.begin()->data(), GL_STATIC_DRAW);
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}

		glGenBuffers(1, &bbo);
		glBindBuffer(GL_ARRAY_BUFFER, bbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned int) * boneIndexes.size() * VERTEX_MAX_BONE, boneIndexes.begin()->data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(3);
		glVertexAttribIPointer(3, VERTEX_MAX_BONE, GL_UNSIGNED_INT, 0, 0);

		glGenBuffers(1, &wbo);
		glBindBuffer(GL_ARRAY_BUFFER, wbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * weights.size() * VERTEX_MAX_BONE, weights.begin()->data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, VERTEX_MAX_BONE, GL_FLOAT, GL_FALSE, 0, 0);

		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * elements.size() * 3, elements.begin()->data(), GL_STATIC_DRAW);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		/*glGenBuffers(1, &offsetao);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, offsetao);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int) * boneOffetMatrix.size() * 16, boneOffetMatrix.begin()->data(), GL_STATIC_DRAW);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);*/
	}
	glBindVertexArray(vao);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, offsetao);
	//else
	//	throw runtime_error("");
	morphMeshData.bindBase(MORPHDATA_BIND_INDEX, MORPHWEIGHT_BIND_INDEX);
	return vao;
}

SkeletonMeshPart SkeletonMeshData::newSkeletonMeshPart(unsigned int vertCount, unsigned int elementCount, unsigned int boneCount, unsigned int morphVertCount, unsigned int morphCount)
{
	SkeletonMeshData* data = NULL;
	/*if (StaticSkeletonMeshData.isGenerated())
		for (auto b = SkeletonMeshDataCollection.begin(), e = SkeletonMeshDataCollection.end(); b != e; b++) {
			if (!(*b)->isGenerated()) {
				data = *b;
				break;
			}
		}
	else
		data = &StaticSkeletonMeshData;
	if (data == NULL) {*/
		data = new SkeletonMeshData();
	//}
	SkeletonMeshPart part = { data, (unsigned int)data->vertices.size(), vertCount, (unsigned int)data->elements.size(), elementCount };
	if (morphCount > 0)
		part.morphMeshData = &data->morphMeshData;
	data->vertices.resize(data->vertices.size() + vertCount);
	data->uvs.resize(data->uvs.size() + vertCount);
	data->normals.resize(data->normals.size() + vertCount);
	data->boneIndexes.resize(data->boneIndexes.size() + vertCount, Matrix<unsigned int, VERTEX_MAX_BONE, 1>::Zero());
	data->weights.resize(data->weights.size() + vertCount, Matrix<float, VERTEX_MAX_BONE, 1>::Zero());
	data->elements.resize(data->elements.size() + elementCount);
	data->boneCount = boneCount;
	data->morphMeshData.init(morphVertCount, morphCount);
	//data->boneOffetMatrix.resize(data->boneOffetMatrix.size() + boneCount);
	return part;
}

SkeletonMeshPart::SkeletonMeshPart() : MeshPart()
{
}

SkeletonMeshPart::SkeletonMeshPart(SkeletonMeshData * meshData, unsigned int vertexFirst, unsigned int vertexCount, unsigned int indexFirst, unsigned int indexCount, MorphMeshData* morphData) :
	MeshPart(meshData, vertexFirst, vertexCount, indexFirst, indexCount), morphMeshData(morphData)
{
}

SkeletonMeshPart::operator MeshPart()
{
	MeshPart p = { meshData, vertexFirst, vertexCount, elementFirst, elementCount };
	p.mesh = mesh;
	p.partIndex = partIndex;
	return p;
}

unsigned int BoneData::addChild(const string & name, const Matrix4f & offsetMatrix, const Matrix4f & transformMatrix)
{
	if (skeletonData == NULL)
		return -1;
	skeletonData->boneName.insert(pair<string, unsigned int>(name, skeletonData->boneList.size()));
	skeletonData->boneList.push_back({
		name, (unsigned int)skeletonData->boneList.size(),
		(unsigned int)children.size(), offsetMatrix,
		transformMatrix, skeletonData, this
	});
	children.push_back(&skeletonData->boneList.back());
	return skeletonData->boneList.size() - 1;
}

BoneData * BoneData::getParent()
{
	return parent;
}

void BoneData::setParent(BoneData * parent)
{
	this->parent = parent;
	if (parent == NULL)
		return;
	siblingIndex = parent->children.size();
	parent->children.push_back(this);
}

BoneData * BoneData::getChild(unsigned int index)
{
	return index < children.size() ? children[index] : NULL;
}

BoneData * BoneData::getSibling(unsigned int index)
{
	unsigned int i = siblingIndex + index;
	return parent == NULL ? NULL : (i < parent->children.size() ? parent->children[i] : NULL);
}

BoneData * BoneData::getNext(BoneData* limitBone)
{
	BoneData* b = getChild();
	if (b == NULL) {
		b = getSibling();
		if (b == NULL) {
			b = parent;
			while (b != limitBone) {
				BoneData* s = b->getSibling();
				if (s == NULL) {
					b = b->parent;
				}
				else {
					b = s;
					return b;
				}
			}
			return NULL;
		}
	}
	return b;
}

BoneData * SkeletonData::getBoneData(const string & name)
{
	auto iter = boneName.find(name);
	if (iter == boneName.end())
		return NULL;
	return &boneList[iter->second];
}

BoneData * SkeletonData::getBoneData(unsigned int index)
{
	return index < boneList.size() ? &boneList[index] : NULL;
}

void SkeletonMesh::updateMorphWeights(vector<float>& weights)
{
	((SkeletonMeshData*)totalMeshPart.meshData)->updateMorphWeights(weights);
}

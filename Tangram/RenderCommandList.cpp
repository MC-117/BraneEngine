#include "RenderCommandList.h"
#include "Camera.h"
#include "Render.h"
#include "DirectLight.h"
#include "PointLight.h"

bool TransDataTag::operator<(const TransDataTag & tag) const
{
	if (render < tag.render)
		return true;
	else if (render == tag.render)
		return mesh < tag.mesh;
	return false;
}

bool TransTag::operator<(const TransTag & tag) const
{
	if (mat < tag.mat)
		return true;
	else if (mat == tag.mat)
		return meshPart < tag.meshPart;
	return false;
}

RenderCommandList::MeshTransformDataPack RenderCommandList::meshTransformDataPack;
RenderCommandList::ParticleDataPack RenderCommandList::particleDataPack;
RenderCommandList::LightDataPack RenderCommandList::lightDataPack;

unsigned int RenderCommandList::MeshTransformData::setMeshTransform(const Matrix4f & transformMat)
{
	if (batchCount >= transforms.size())
		transforms.emplace_back(transformMat);
	else
		transforms[batchCount] = transformMat;
	batchCount++;
	return batchCount - 1;
}

unsigned int RenderCommandList::MeshTransformData::setMeshTransform(const vector<Matrix4f>& transformMats)
{
	unsigned int size = batchCount + transformMats.size();
	if (size > transforms.size()) {
		transforms.resize(size);
	}
	for (int i = batchCount; i < size; i++)
		transforms[i] = transformMats[i - batchCount];
	unsigned int id = batchCount;
	batchCount = size;
	return id;
}

bool RenderCommandList::MeshTransformData::updataMeshTransform(const Matrix4f & transformMat, unsigned int base)
{
	if (base >= batchCount)
		return false;
	transforms[base] = transformMat;
	return true;
}

bool RenderCommandList::MeshTransformData::updataMeshTransform(const vector<Matrix4f>& transformMats, unsigned int base)
{
	if (base + transformMats.size() >= batchCount)
		return false;
	for (int i = base; i < transformMats.size(); i++)
		transforms[i] = transformMats[i];
	return true;
}

void RenderCommandList::MeshTransformData::clean()
{
	batchCount = 0;
}

bool RenderCommandList::MeshTransformData::clean(unsigned int base, unsigned int count)
{
	if (base + count >= batchCount)
		return false;
	transforms.erase(transforms.begin() + base, transforms.begin() + (base + count));
	return true;
}

unsigned int RenderCommandList::MeshTransformDataPack::setMeshTransform(const Matrix4f & transformMat)
{
	return meshTransformData.setMeshTransform(transformMat);
}

unsigned int RenderCommandList::MeshTransformDataPack::setMeshTransform(const vector<Matrix4f>& transformMats)
{
	return meshTransformData.setMeshTransform(transformMats);
}

RenderCommandList::MeshTransformIndex * RenderCommandList::MeshTransformDataPack::getMeshPartTransform(MeshPart * meshPart, Material * material)
{
	if (meshPart == NULL || material == NULL)
		return NULL;
	TransTag tag = { material, meshPart };
	auto meshIter = meshTransformIndex.find(tag);
	if (meshIter != meshTransformIndex.end())
		return &meshIter->second;
	return NULL;
}

RenderCommandList::MeshTransformIndex * RenderCommandList::MeshTransformDataPack::setMeshPartTransform(MeshPart * meshPart, Material * material, unsigned int transformIndex)
{
	if (meshPart == NULL || material == NULL)
		return NULL;
	TransTag tag = { material, meshPart };
	auto meshIter = meshTransformIndex.find(tag);
	MeshTransformIndex *trans;
	if (meshIter == meshTransformIndex.end()) {
		trans = &meshTransformIndex.insert(pair<TransTag, MeshTransformIndex>(tag,
			MeshTransformIndex())).first->second;
	}
	else {
		trans = &meshIter->second;
	}
	if (trans->batchCount >= trans->indices.size())
		trans->indices.push_back(transformIndex);
	else
		trans->indices[trans->batchCount] = transformIndex;
	trans->batchCount++;
	totalTransformIndexCount++;
	return trans;
}

RenderCommandList::MeshTransformIndex * RenderCommandList::MeshTransformDataPack::setMeshPartTransform(MeshPart * meshPart, Material * material, MeshTransformIndex * transformIndex)
{
	if (meshPart == NULL || material == NULL || transformIndex != NULL)
		return NULL;
	TransTag tag = { material, meshPart };
	auto meshIter = meshTransformIndex.find(tag);
	MeshTransformIndex *trans;
	if (meshIter == meshTransformIndex.end()) {
		trans = &meshTransformIndex.insert(pair<TransTag, MeshTransformIndex>(tag,
			MeshTransformIndex())).first->second;
	}
	else {
		trans = &meshIter->second;
	}
	unsigned int size = trans->batchCount + transformIndex->batchCount;
	if (size > trans->indices.size())
		trans->indices.resize(size);
	for (int i = trans->batchCount; i < size; i++)
		trans->indices[i] = transformIndex->indices[i - trans->batchCount];
	trans->batchCount = size;
	totalTransformIndexCount += transformIndex->batchCount;
	return trans;
}

unsigned int RenderCommandList::MeshTransformDataPack::setStaticMeshTransform(const Matrix4f & transformMat)
{
	return staticMeshTransformData.setMeshTransform(transformMat);
}

unsigned int RenderCommandList::MeshTransformDataPack::setStaticMeshTransform(const vector<Matrix4f>& transformMats)
{
	return staticMeshTransformData.setMeshTransform(transformMats);
}

RenderCommandList::MeshTransformIndex * RenderCommandList::MeshTransformDataPack::getStaticMeshPartTransform(MeshPart * meshPart, Material * material)
{
	if (meshPart == NULL || material == NULL)
		return NULL;
	TransTag tag = { material, meshPart };
	auto meshIter = staticMeshTransformIndex.find(tag);
	if (meshIter != staticMeshTransformIndex.end())
		return &meshIter->second;
	return NULL;
}

RenderCommandList::MeshTransformIndex * RenderCommandList::MeshTransformDataPack::setStaticMeshPartTransform(MeshPart * meshPart, Material * material, unsigned int transformIndex)
{
	if (meshPart == NULL || material == NULL)
		return NULL;
	TransTag tag = { material, meshPart };
	auto meshIter = staticMeshTransformIndex.find(tag);
	MeshTransformIndex *trans;
	if (meshIter == staticMeshTransformIndex.end()) {
		trans = &staticMeshTransformIndex.insert(pair<TransTag, MeshTransformIndex>(tag,
			MeshTransformIndex())).first->second;
	}
	else {
		trans = &meshIter->second;
	}
	if (trans->batchCount >= trans->indices.size())
		trans->indices.push_back(transformIndex);
	else
		trans->indices[trans->batchCount] = transformIndex;
	trans->batchCount++;
	staticTotalTransformIndexCount++;
	return trans;
}

RenderCommandList::MeshTransformIndex * RenderCommandList::MeshTransformDataPack::setStaticMeshPartTransform(MeshPart * meshPart, Material * material, MeshTransformIndex * transformIndex)
{
	if (meshPart == NULL || material == NULL || transformIndex != NULL)
		return NULL;
	TransTag tag = { material, meshPart };
	auto meshIter = staticMeshTransformIndex.find(tag);
	MeshTransformIndex *trans;
	if (meshIter == staticMeshTransformIndex.end()) {
		trans = &staticMeshTransformIndex.insert(pair<TransTag, MeshTransformIndex>(tag,
			MeshTransformIndex())).first->second;
	}
	else {
		trans = &meshIter->second;
	}
	unsigned int size = trans->batchCount + transformIndex->batchCount;
	if (size > trans->indices.size())
		trans->indices.resize(size);
	for (int i = trans->batchCount; i < size; i++)
		trans->indices[i] = transformIndex->indices[i - trans->batchCount];
	trans->batchCount = size;
	staticTotalTransformIndexCount += transformIndex->batchCount;
	return trans;
}

void RenderCommandList::MeshTransformDataPack::uploadTransforms()
{
	if (staticUpdate) {
		staticTotalTransformIndexCount = 0;
		for (auto b = staticMeshTransformIndex.begin(), e = staticMeshTransformIndex.end(); b != e; b++) {
			staticTotalTransformIndexCount += b->second.batchCount;
		}
	}
	unsigned int dataSize = meshTransformData.batchCount + staticMeshTransformData.batchCount;
	unsigned int indexSize = totalTransformIndexCount + staticTotalTransformIndexCount;
	bool needUpdate = dataSize > transformBuffer.capacity || indexSize > transformIndexBuffer.capacity;
	transformBuffer.resize(dataSize);
	transformIndexBuffer.resize(indexSize);
	unsigned int transformBase = 0, transformIndexBase = 0;
	if (needUpdate || staticUpdate) {
		transformBuffer.uploadSubData(transformBase, staticMeshTransformData.batchCount,
			staticMeshTransformData.transforms.data()->data());
		transformBase += staticMeshTransformData.batchCount;
		for (auto b = staticMeshTransformIndex.begin(), e = staticMeshTransformIndex.end(); b != e; b++) {
			b->second.indexBase = transformIndexBase;
			transformIndexBuffer.uploadSubData(transformIndexBase, b->second.batchCount,
				b->second.indices.data());
			transformIndexBase += b->second.batchCount;
		}
	}
	else {
		transformBase = staticMeshTransformData.batchCount;
		transformIndexBase = staticTotalTransformIndexCount;
	}
	transformBuffer.uploadSubData(transformBase, meshTransformData.batchCount,
		meshTransformData.transforms.data()->data());
	for (auto b = meshTransformIndex.begin(), e = meshTransformIndex.end(); b != e; b++) {
		b->second.indexBase = transformIndexBase;
		for (int i = 0; i < b->second.batchCount; i++) {
			b->second.indices[i] += transformBase;
		}
		transformIndexBuffer.uploadSubData(transformIndexBase, b->second.batchCount,
			b->second.indices.data());
		transformIndexBase += b->second.batchCount;
	}
	staticUpdate = false;
}

void RenderCommandList::MeshTransformDataPack::bindTransforms()
{
	transformBuffer.bindBase(TRANS_BIND_INDEX);
	transformIndexBuffer.bindBase(TRANS_INDEX_BIND_INDEX);
}

void RenderCommandList::MeshTransformDataPack::clean()
{
	meshTransformData.clean();
	for (auto b = meshTransformIndex.begin(), e = meshTransformIndex.end(); b != e; b++) {
		b->second.batchCount = 0;
	}
	totalTransformIndexCount = 0;
}

void RenderCommandList::MeshTransformDataPack::cleanStatic(unsigned int base, unsigned int count)
{
	staticMeshTransformData.clean(base, count);
}

void RenderCommandList::MeshTransformDataPack::cleanPartStatic(MeshPart * meshPart, Material * material)
{
	auto iter = staticMeshTransformIndex.find({ material, meshPart });
	if (iter != staticMeshTransformIndex.end()) {
		staticUpdate = true;
		iter->second.batchCount = 0;
	}
}

RenderCommandList::ParticleData* RenderCommandList::ParticleDataPack::setParticles(Material* material, const list<Particle>& particles)
{
	auto miter = this->particles.find(material);
	ParticleData* pd;
	if (miter == this->particles.end()) {
		pd = &this->particles.emplace(pair<Material*, ParticleData>(material, ParticleData())).first->second;
	}
	else {
		pd = &miter->second;
	}
	int base = pd->batchCount;
	totalParticleCount += particles.size();
	pd->batchCount += particles.size();
	if (pd->batchCount > pd->particles.size())
		pd->particles.resize(pd->batchCount);
	int i = 0;
	for (auto b = particles.begin(), e = particles.end(); b != e; b++, i++) {
		pd->particles[base + i] = *b;
	}
	return pd;
}

RenderCommandList::ParticleData* RenderCommandList::ParticleDataPack::setParticles(Material* material, const vector<Particle>& particles)
{
	auto miter = this->particles.find(material);
	ParticleData* pd;
	if (miter == this->particles.end()) {
		pd = &this->particles.emplace(pair<Material*, ParticleData>(material, ParticleData())).first->second;
	}
	else {
		pd = &miter->second;
	}
	int base = pd->batchCount;
	totalParticleCount += particles.size();
	pd->batchCount += particles.size();
	if (pd->batchCount > pd->particles.size())
		pd->particles.resize(pd->batchCount);
	int i = 0;
	for (auto b = particles.begin(), e = particles.end(); b != e; b++, i++) {
		pd->particles[base + i] = *b;
	}
	return pd;
}

void RenderCommandList::ParticleDataPack::uploadParticles()
{
	int base = 0;
	particleBuffer.resize(totalParticleCount);
	for (auto b = particles.begin(), e = particles.end(); b != e; b++) {
		b->second.particleBase = base;
		particleBuffer.uploadSubData(base, b->second.batchCount, b->second.particles.data()->position.data());
		base += b->second.batchCount;
	}
}

void RenderCommandList::ParticleDataPack::bindParticles()
{
	particleBuffer.bindBase(PARTICLE_BIND_INDEX);
}

void RenderCommandList::ParticleDataPack::clean()
{
	for (auto b = particles.begin(), e = particles.end(); b != e; b++) {
		b->second.batchCount = 0;
	}
	totalParticleCount = 0;
}

void RenderCommandList::ParticleRenderPack::excute()
{
	if (particleData == NULL)
		return;
	cmd.first = 0;
	cmd.count = 1;
	cmd.baseInstance = particleData->particleBase;
	cmd.instanceCount = particleData->batchCount;
	glDrawArraysIndirect(GL_POINTS, &cmd);
}

void RenderCommandList::MeshDataRenderPack::setRenderData(MeshPart * part, MeshTransformIndex* data, ParticleData * modifier)
{
	meshParts.insert(pair<MeshPart*, MeshTransformIndex*>(part, data));
	if (modifier != NULL)
		particleModifier.addParticleData(modifier);
	//totaltransformCount += data->batchCount * data->batchSize;
}

void RenderCommandList::MeshDataRenderPack::excute()
{
	if (meshParts.empty())
		return;
	cmds.resize(meshParts.size());
	//transformBuffer.resize(totaltransformCount);
	//totaltransformCount = 0;
	int index = 0;//, base = 0;
	for (auto b = meshParts.begin(), e = meshParts.end(); b != e; b++, index++) {
		/*if (b->second->batchCount * b->second->batchSize == 0) {
			b->second->transforms.clear();
			index--;
			continue;
		}*/
		DrawElementsIndirectCommand& c = cmds[index];
		c.baseVertex = b->first->vertexFirst;
		c.count = b->first->elementCount * 3;
		c.firstIndex = b->first->elementFirst * 3;
		c.instanceCount = b->second->batchCount;
		c.baseInstance = b->second->indexBase;//base;
		//transformBuffer.uploadSubData(base, b->second->batchCount * b->second->batchSize,
		//	b->second->transforms.data()->data());
		//base += c.instanceCount * b->second->batchSize;
	}
	particleModifier.bind();

	cmdBuffer.uploadData(cmds.size(), cmds.data());
	//transformBuffer.bindBase(0);
	//glDrawArraysIndirect(GL_TRIANGLES, cmds.data());
	cmdBuffer.bind();
	/*int loc = Shader::getAttributeIndex(Shader::getCurrentProgramId(), "boneCount");
	if (loc != -1)
		glUniform1i(loc, (int)meshParts.begin()->second->batchSize);*/
	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, NULL, cmdBuffer.size, 0);
	cmdBuffer.unbind();
}

void RenderCommandList::setLight(Render * lightRender)
{
	if (lightRender->getRenderType() == IRendering::Light)
		lightDataPack.setLight((Light*)lightRender);
}

unsigned int RenderCommandList::setMeshTransform(const Matrix4f & transformMat)
{
	return meshTransformDataPack.setMeshTransform(transformMat);
}

unsigned int RenderCommandList::setMeshTransform(const vector<Matrix4f> & transformMats)
{
	return meshTransformDataPack.setMeshTransform(transformMats);
}

void * RenderCommandList::getMeshPartTransform(MeshPart * meshPart, Material * material)
{
	return meshTransformDataPack.getMeshPartTransform(meshPart, material);
}

void * RenderCommandList::setMeshPartTransform(MeshPart * meshPart, Material * material, unsigned int transformIndex)
{
	void* re = meshTransformDataPack.setMeshPartTransform(meshPart, material, transformIndex);
	if (re != NULL && material->getRenderOrder() >= 1000 && material->getRenderOrder() < 2450)
		meshTransformDataPack.setMeshPartTransform(meshPart, &Material::defaultDepthMaterial, transformIndex);
	return re;
}

void * RenderCommandList::setMeshPartTransform(MeshPart * meshPart, Material * material, void * transformIndex)
{
	void* re = meshTransformDataPack.setMeshPartTransform(meshPart, material, (MeshTransformIndex*)transformIndex);
	if (re != NULL && material->getRenderOrder() >= 1000 && material->getRenderOrder() < 2450)
		meshTransformDataPack.setMeshPartTransform(meshPart, &Material::defaultDepthMaterial, (MeshTransformIndex*)transformIndex);
	return re;
}

unsigned int RenderCommandList::setStaticMeshTransform(const Matrix4f & transformMat)
{
	return meshTransformDataPack.setStaticMeshTransform(transformMat);
}

unsigned int RenderCommandList::setStaticMeshTransform(const vector<Matrix4f>& transformMats)
{
	return meshTransformDataPack.setStaticMeshTransform(transformMats);
}

void * RenderCommandList::getStaticMeshPartTransform(MeshPart * meshPart, Material * material)
{
	return meshTransformDataPack.getStaticMeshPartTransform(meshPart, material);
}

void * RenderCommandList::setStaticMeshPartTransform(MeshPart * meshPart, Material * material, unsigned int transformIndex)
{
	void* re = meshTransformDataPack.setStaticMeshPartTransform(meshPart, material, transformIndex);
	if (re != NULL && material->getRenderOrder() >= 1000 && material->getRenderOrder() < 2450)
		meshTransformDataPack.setStaticMeshPartTransform(meshPart, &Material::defaultDepthMaterial, transformIndex);
	return re;
}

void * RenderCommandList::setStaticMeshPartTransform(MeshPart * meshPart, Material * material, void * transformIndex)
{
	void* re = meshTransformDataPack.setStaticMeshPartTransform(meshPart, material, (MeshTransformIndex*)transformIndex);
	if (re != NULL && material->getRenderOrder() >= 1000 && material->getRenderOrder() < 2450)
		meshTransformDataPack.setStaticMeshPartTransform(meshPart, &Material::defaultDepthMaterial, (MeshTransformIndex*)transformIndex);
	return re;
}

void RenderCommandList::cleanStaticMeshTransform(unsigned int base, unsigned int count)
{
	meshTransformDataPack.cleanStatic(base, count);
}

void RenderCommandList::cleanStaticMeshPartTransform(MeshPart * meshPart, Material * material)
{
	meshTransformDataPack.cleanPartStatic(meshPart, material);
}

bool RenderCommandList::setRenderCommand(const RenderCommand & cmd, bool isStatic)
{
	return setRenderCommand(cmd, isStatic, true);
}

bool RenderCommandList::setRenderCommand(const RenderCommand & cmd, bool isStatic, bool autoFill)
{
	if (cmd.material == NULL || cmd.material->isNull() || cmd.camera == NULL || (cmd.mesh == NULL && cmd.particles == NULL) ||
		(cmd.mesh == NULL && cmd.particles != NULL && cmd.particles->empty()))
		return false;
	Enum<ShaderFeature> shaderFeature;
	if (cmd.mesh == NULL)
		shaderFeature |= Shader_Particle;
	else {
		if (cmd.material->isDeferred)
			shaderFeature |= Shader_Deferred;
		if (cmd.mesh->isSkeleton())
			shaderFeature |= Shader_Skeleton;
		if (cmd.mesh->isMorph())
			shaderFeature |= Shader_Morph;
		if (cmd.particles != NULL)
			shaderFeature |= Shader_Modifier;
	}
	ShaderProgram* shader = cmd.material->getShader()->getProgram(shaderFeature);
	if (shader == NULL) {
		Console::warn("Shader %s don't have mode %d", cmd.material->getShaderName().c_str(), shaderFeature.enumValue);
		return false;
	}
	Camera* cam = cmd.camera;
	auto camIter = commandList.find(cam);
	map<ShaderProgram*, map<Material*, map<MeshData*, IRenderPack*>>, shader_order> *shaderMap;
	if (camIter == commandList.end()) {
		shaderMap = &commandList.insert(pair<Camera*, map<ShaderProgram*, map<Material*, map<MeshData*, IRenderPack*>>, shader_order>>(cam,
			map<ShaderProgram*, map<Material*, map<MeshData*, IRenderPack*>>, shader_order>())).first->second;
	}
	else {
		shaderMap = &camIter->second;
	}
	auto shaderIter = shaderMap->find(shader);
	map<Material*, map<MeshData*, IRenderPack*>> *matMap;
	if (shaderIter == shaderMap->end()) {
		matMap = &shaderMap->insert(pair<ShaderProgram*, map<Material*, map<MeshData*, IRenderPack*>>>(shader,
			map<Material*, map<MeshData*, IRenderPack*>>())).first->second;
	}
	else {
		matMap = &shaderIter->second;
	}
	Material* mat = cmd.material;
	auto matIter = matMap->find(mat);
	map<MeshData*, IRenderPack*> *meshDataMap;
	if (matIter == matMap->end()) {
		meshDataMap = &matMap->insert(pair<Material*, map<MeshData*, IRenderPack*>>(mat,
			map<MeshData*, IRenderPack*>())).first->second;
	}
	else {
		meshDataMap = &matIter->second;
	}
	MeshData* meshData = cmd.mesh == NULL ? NULL : cmd.mesh->meshData;
	auto meshDataIter = meshDataMap->find(meshData);
	IRenderPack *renderPack;
	if (meshDataIter == meshDataMap->end()) {
		if (cmd.mesh == NULL)
			renderPack = new ParticleRenderPack();
		else
			renderPack = new MeshDataRenderPack();
		renderPacks.push_back(renderPack);
		meshDataMap->insert(pair<MeshData*, IRenderPack*>(meshData, renderPack)).first->second;
	}
	else {
		renderPack = meshDataIter->second;
	}
	if (cmd.mesh == NULL) {
		if (cmd.particles != NULL) {
			ParticleRenderPack* prp = dynamic_cast<ParticleRenderPack*>(renderPack);
			/*if (prp == NULL)
				return false;*/
			prp->particleData = particleDataPack.setParticles(cmd.material, *cmd.particles);
		}
	}
	else {
		MeshDataRenderPack* meshDataPack = dynamic_cast<MeshDataRenderPack*>(renderPack);
		ParticleData* pdata = NULL;
		if (cmd.particles != NULL)
			pdata = particleDataPack.setParticles(cmd.material, *cmd.particles);
		if (isStatic) {
			auto meshTDIter = meshTransformDataPack.staticMeshTransformIndex.find({ cmd.material, cmd.mesh });
			if (meshTDIter != meshTransformDataPack.staticMeshTransformIndex.end())
				meshDataPack->setRenderData(cmd.mesh, &meshTDIter->second, pdata);
			else
				return false;
		}
		else {
			auto meshTDIter = meshTransformDataPack.meshTransformIndex.find({ cmd.material, cmd.mesh });
			if (meshTDIter != meshTransformDataPack.meshTransformIndex.end())
				meshDataPack->setRenderData(cmd.mesh, &meshTDIter->second, pdata);
			else
				return false;
		}
	}
	/*if (autoFill && shader->renderOrder >= 1000 && shader->renderOrder < 2450) {
		RenderCommand _cmd = cmd;
		_cmd.material = &Material::defaultDepthMaterial;
		return setRenderCommand(_cmd, isStatic, false);
	}*/
	return true;
}

/*
         |   0   |       10       |   500   |      750      |  1000  |    2450    |     2500    |  5000   |
 Forward | Light | Depth Pre-Pass |         |               | Opaque | Alpha Mask | Transparent | Overlay |
Deferred | Light | Depth Pre-Pass | Geomtry | Alpha Geomtry | Pixel  |            |             |         |
*/

void RenderCommandList::excuteCommand()
{
	Timer timer;
	meshTransformDataPack.uploadTransforms();
	particleDataPack.uploadParticles();
	lightDataPack.uploadLight();
	timer.record("Upload");
	Time setupTime, uploadBaseTime, uploadInsTime, execTime;
	for (auto camB = commandList.begin(), camE = commandList.end(); camB != camE; camB++) {
		Time t = Time::now();
		glDepthMask(GL_TRUE);
		camB->first->cameraRender.preRender();
		glClearColor(camB->first->clearColor.r, camB->first->clearColor.g, camB->first->clearColor.b, camB->first->clearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, camB->first->size.x, camB->first->size.y);

		camB->first->uploadCameraData();

		setupTime = setupTime + Time::now() - t;
		for (auto shaderB = camB->second.begin(), shaderE = camB->second.end(); shaderB != shaderE; shaderB++) {
			Time t = Time::now();
			if (shaderB->first->renderOrder < 500) {
				glColorMask(0, 0, 0, 0);
				glDisable(GL_BLEND);
				glDisable(GL_ALPHA_TEST);
				glEnable(GL_DEPTH_TEST);
			}
			else if (shaderB->first->renderOrder < 1000) {
				//glDepthMask(GL_TRUE);
				//glStencilMask(GL_TRUE);
				//glDepthFunc(GL_LEQUAL);
				glColorMask(1, 1, 1, 1);
				glDisable(GL_BLEND);
				glDisable(GL_ALPHA_TEST);
				glEnable(GL_DEPTH_TEST);
			}
			else if (shaderB->first->renderOrder < 2450) {
				//glDepthMask(GL_FALSE);
				//glStencilMask(GL_FALSE);
				//glDepthFunc(GL_LEQUAL);
				glColorMask(1, 1, 1, 1);
				glDisable(GL_BLEND);
				glDisable(GL_ALPHA_TEST);
				glEnable(GL_DEPTH_TEST);
			}
			else if (shaderB->first->renderOrder < 2500) {
				//glDepthMask(GL_TRUE);
				//glStencilMask(GL_TRUE);
				//glDepthFunc(GL_LEQUAL);
				glColorMask(1, 1, 1, 1);
				glDisable(GL_BLEND);
				glEnable(GL_ALPHA_TEST);
				glAlphaFunc(GL_GREATER, 0.0f);
				glEnable(GL_DEPTH_TEST);
			}
			else if (shaderB->first->renderOrder < 5000) {
				//glDepthMask(GL_FALSE);
				//glStencilMask(GL_FALSE);
				//glDepthFunc(GL_LEQUAL);
				glColorMask(1, 1, 1, 1);
				glEnable(GL_BLEND);
				glDisable(GL_ALPHA_TEST);
				glEnable(GL_DEPTH_TEST);
			}
			else {
				//glDepthMask(GL_FALSE);
				//glStencilMask(GL_FALSE);
				glColorMask(1, 1, 1, 1);
				glEnable(GL_BLEND);
				glDisable(GL_ALPHA_TEST);
				glDisable(GL_DEPTH_TEST);
			}
			unsigned int pid = shaderB->first->bind();
			setupTime = setupTime + Time::now() - t;

			if (!shaderB->second.empty()) {
				Time t = Time::now();

				camB->first->bindCameraData();

				uploadBaseTime = uploadBaseTime + Time::now() - t;
				meshTransformDataPack.bindTransforms();
				particleDataPack.bindParticles();
				lightDataPack.bindLight();
			}

			for (auto matB = shaderB->second.begin(), matE = shaderB->second.end(); matB != matE; matB++) {
				Time t = Time::now();
				if (lightDataPack.shadowTarget == NULL)
					matB->first->setTexture("depthMap", Texture2D::whiteRGBADefaultTex);
				else
					lightDataPack.shadowTarget->setTexture(*matB->first);
				if (matB->first->isTwoSide)
					glDisable(GL_CULL_FACE);
				else
					glEnable(GL_CULL_FACE);
				if (matB->first->cullFront)
					glCullFace(GL_FRONT);
				else
					glCullFace(GL_BACK);
				matB->first->processInstanceData();
				uploadInsTime = uploadInsTime + Time::now() - t;
				for (auto meshDataB = matB->second.begin(), meshDataE = matB->second.end(); meshDataB != meshDataE; meshDataB++) {
					if (meshDataB->first != NULL)
						meshDataB->first->bindShape();
					Time t = Time::now();
					meshDataB->second->excute();
					execTime = execTime + Time::now() - t;
				}
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
			}
		}
	}
	timer.record("Execute");
	Console::getTimer("RCMDL") = timer;
	Timer& execTimer = Console::getTimer("RCMDL Exec");
	execTimer.setIntervalMode(true);
	execTimer.reset();
	execTimer.record("Setup", setupTime);
	execTimer.record("Base", uploadBaseTime);
	execTimer.record("Instance", uploadInsTime);
	execTimer.record("Exec", execTime);
}

void RenderCommandList::resetCommand()
{
	meshTransformDataPack.clean();
	particleDataPack.clean();
	lightDataPack.clean();
	lightRenders.clear();
	commandList.clear();
	for (auto b = renderPacks.begin(), e = renderPacks.end(); b != e; b++)
		delete *b;
	renderPacks.clear();
}

bool RenderCommandList::camera_order::operator()(const Camera * s0, const Camera * s1) const
{
	if (s0->cameraRender.renderOrder < s1->cameraRender.renderOrder)
		return true;
	if (s0->cameraRender.renderOrder == s1->cameraRender.renderOrder && s0 < s1)
		return true;
	return false;
}

void RenderCommandList::ParticleModifier::addParticleData(ParticleData * pdata)
{
	data.emplace_back(pdata);
}

void RenderCommandList::ParticleModifier::bind()
{
	int size = data.size();
	if (size == 0)
		return;
	particleBases.resize(size);
	for (int i = 0; i < size; i++) {
		particleBases[i] = { data[i]->particleBase, data[i]->batchCount };
	}
	buffer.uploadData(size, particleBases.data()->data());
	buffer.bindBase(MODIFIER_BIND_INDEX);
}

void RenderCommandList::ParticleModifier::clean()
{
	data.clear();
}

void RenderCommandList::LightDataPack::setLight(Light * light)
{
	if (shadowTarget == NULL && light->getShadowRenderTarget() != NULL) {
		shadowTarget = light->getShadowRenderTarget();
	}
	DirectLight* directLight = dynamic_cast<DirectLight*>(light);
	if (directLight != NULL) {
		directLightData.direction = directLight->getForward(WORLD);
		directLightData.intensity = directLight->intensity;
		directLightData.lightSpaceMat = directLight->getLightSpaceMatrix();
		directLightData.color = Vector3f(directLight->color.r, directLight->color.g, directLight->color.b);
	}
	PointLight* pointLight = dynamic_cast<PointLight*>(light);
	if (pointLight != NULL) {
		PointLightData data;
		data.position = pointLight->getPosition(WORLD);
		data.intensity = pointLight->intensity;
		data.color = Vector3f(pointLight->color.r, pointLight->color.g, pointLight->color.b);
		data.attenuation = pointLight->attenuation;
		pointLightDatas.emplace_back(data);
	}
}

void RenderCommandList::LightDataPack::uploadLight()
{
	directLightData.pointLightCount = pointLightDatas.size();
	lightBuffer.resize(directLightData.pointLightCount + 3);
	lightBuffer.uploadSubData(0, 3, &directLightData);
	if (directLightData.pointLightCount != 0)
		lightBuffer.uploadSubData(3, directLightData.pointLightCount, pointLightDatas.data());
}

void RenderCommandList::LightDataPack::bindLight()
{
	lightBuffer.bindBase(LIGHT_BIND_INDEX);
}

void RenderCommandList::LightDataPack::clean()
{
	shadowTarget = NULL;
	directLightData.pointLightCount = 0;
	pointLightDatas.clear();
}

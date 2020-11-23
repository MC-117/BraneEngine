#include "ClothBody.h"
#include "PhysicalWorld.h"

ClothBody::ClothBody(::Transform & targetTransform, const PhysicalMaterial & material, Mesh* mesh)
	: PhysicalBody(targetTransform, material, mesh, NONESHAPE)
{
	bodyType = SOFT;
	PFabric* fabric = mesh->generateCloth();
	int vertCount = 0;
	for (int i = 0; i < mesh->meshParts.size(); i++) {
		meshParts.emplace(make_pair(&mesh->meshParts[i], make_pair(vertCount, vector<Particle>())));
		vertCount += mesh->meshParts[i].vertexCount;
	}
	vector<Vector4f> clothParticles;
	clothParticles.resize(mesh->vertCount);
	rawClothBody = PhysicalWorld::gNvClothFactory->createCloth(
	{ (PxVec4*)clothParticles.data(), (PxVec4*)clothParticles.data() + mesh->vertCount }, *fabric);
	nv::cloth::PhaseConfig* phases = new nv::cloth::PhaseConfig[fabric->getNumPhases()];
	for (int i = 0; i < fabric->getNumPhases(); i++)
	{
		phases[i].mPhaseIndex = i; // Set index to the corresponding set (constraint group)

								   //For this example we give very phase the same config
		phases[i].mStiffness = 1.0f;
		phases[i].mStiffnessMultiplier = 1.0f;
		phases[i].mCompressionLimit = 1.0f;
		phases[i].mStretchLimit = 1.0f;
	}
	rawClothBody->setPhaseConfig({ phases, phases + fabric->getNumPhases() });
	delete[] phases;
}

ClothBody::ClothBody(::Transform & targetTransform, const PhysicalMaterial & material, Mesh* mesh, const vector<unsigned int>& meshPartsIndex)
	: PhysicalBody(targetTransform, material, mesh, NONESHAPE)
{
	bodyType = SOFT;
	PFabric* fabric = mesh->generateCloth(meshPartsIndex);
	int vertCount = 0;
	for (int i = 0; i < meshPartsIndex.size(); i++) {
		if (meshPartsIndex[i] < mesh->meshParts.size()) {
			meshParts.insert(make_pair(&mesh->meshParts[meshPartsIndex[i]], make_pair(vertCount, vector<Particle>())));
			vertCount += mesh->meshParts[i].vertexCount;
		}
	}
	vector<Vector4f> clothParticles;
	clothParticles.resize(vertCount);
	unsigned int vertBase = 0;
	for (int i = 0; i < meshPartsIndex.size(); i++) {
		if (meshPartsIndex[i] < mesh->meshParts.size()) {
			MeshPart& meshPart = mesh->meshParts[meshPartsIndex[i]];
			for (int v = 0; v < meshPart.vertexCount; v++) {
				clothParticles[vertBase + v].block(0, 0, 3, 1) = meshPart.vertex(v);
				clothParticles[vertBase + v].w() = 0.5;
			}
		}
	}
	rawClothBody = PhysicalWorld::gNvClothFactory->createCloth(
	{ (PxVec4*)clothParticles.data(), (PxVec4*)clothParticles.data() + mesh->vertCount }, *fabric);
	nv::cloth::PhaseConfig* phases = new nv::cloth::PhaseConfig[fabric->getNumPhases()];
	for (int i = 0; i < fabric->getNumPhases(); i++)
	{
		phases[i].mPhaseIndex = i; // Set index to the corresponding set (constraint group)

								   //For this example we give very phase the same config
		phases[i].mStiffness = 1.0f;
		phases[i].mStiffnessMultiplier = 1.0f;
		phases[i].mCompressionLimit = 1.0f;
		phases[i].mStretchLimit = 1.0f;
	}
	rawClothBody->setPhaseConfig({ phases, phases + fabric->getNumPhases() });
	delete[] phases;
}

ClothBody::~ClothBody()
{
	NV_CLOTH_DELETE(rawClothBody);
}

void ClothBody::initBody()
{
}

void ClothBody::updateObjectTransform()
{
	auto clothParticles = rawClothBody->getCurrentParticles();
	for (auto b = meshParts.begin(), e = meshParts.end(); b != e; b++) {
		MeshPart* meshPart = b->first;
		unsigned int vertBase = b->second.first;
		vector<Particle>& particles = b->second.second;
		
		if (particles.empty())
			particles.resize(ceilf(meshPart->vertexCount / 4.0f));
		ClothParticle* pparticles = (ClothParticle*)particles.data();
		for (int i = 0; i < meshPart->vertexCount; i++) {
			pparticles[i].position = toVector3f(clothParticles[vertBase + i].getXYZ());
			pparticles[i].normal = Vector3f(0, 0, 0);
		}
	}
	for (auto b = meshParts.begin(), e = meshParts.end(); b != e; b++) {
		MeshPart* meshPart = b->first;
		unsigned int vertBase = b->second.first;
		vector<Particle>& particles = b->second.second;
		ClothParticle* pparticles = (ClothParticle*)particles.data();
		for (int i = 0; i < meshPart->elementCount; i++) {
			Matrix<unsigned int, 3, 1> &element = meshPart->element(i);
			const auto p0 = pparticles[element[0]].position;
			const auto p1 = pparticles[element[1]].position;
			const auto p2 = pparticles[element[2]].position;

			const auto normal = ((p2 - p0).cross(p1 - p0)).normalized();

			pparticles[element[0]].normal += normal;
			pparticles[element[0]].normal += normal;
			pparticles[element[0]].normal += normal;
		}
	}
}

void ClothBody::addToWorld(PhysicalWorld & physicalWorld)
{
	physicalWorld.clothSolver->addCloth(rawClothBody);
}

void ClothBody::removeFromWorld()
{
	if (physicalWorld != NULL)
		physicalWorld->clothSolver->removeCloth(rawClothBody);
}

void * ClothBody::getSoftObject() const
{
	return rawClothBody;
}

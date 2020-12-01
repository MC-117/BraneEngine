#include "GrassMeshActor.h"
#undef min
#undef max

GrassMeshActor::GrassMeshActor(Mesh & mesh, Material & material, string name)
	: MeshActor(mesh, material, name)
{
	meshRender.isStatic = true;
	meshRender.canCastShadow = false;
	delete rigidBody;
	rigidBody = NULL;
	events.registerFunc("updateData", [](void* obj) {
		((GrassMeshActor*)obj)->updateData();
	});
	events.registerFunc("setBoundX", [](void* obj, int x) {
		((GrassMeshActor*)obj)->bound.x() = x;
	});
	events.registerFunc("setBoundY", [](void* obj, int y) {
		((GrassMeshActor*)obj)->bound.y() = y;
	});
	events.registerFunc("setDensity", [](void* obj, float d) {
		((GrassMeshActor*)obj)->density = d;
	});
}

void GrassMeshActor::set(float density, Vector2i & bound)
{
	this->density = density;
	this->bound = bound;
}

void GrassMeshActor::updateData()
{
	RenderCommandList::setUpdateStatic();
}

void GrassMeshActor::end()
{
	MeshActor::end();
	RenderCommandList::setUpdateStatic();
}

void GrassMeshActor::prerender()
{
	if (RenderCommandList::willUpdateStatic()) {
		float w = bound.x() / density;
		int yd = bound.y() / w;
		float yo = (bound.y() - yd * w) * 0.5;
		float hx = bound.x() * 0.5, hy = bound.y() * 0.5;
		transCount = (yd + 1) * (density + 1);
		srand(time(NULL));
		for (int i = 0; i <= density; i++) {
			for (int j = 0; j <= yd; j++) {
				Matrix4f m = Matrix4f::Identity();
				m(0, 3) = i * w - hx + ((rand() / (double)RAND_MAX) - 0.5) * 2 * w;
				m(1, 3) = j * w - hy + yo + ((rand() / (double)RAND_MAX) - 0.5) * 2 * w;
				m.block(0, 0, 3, 3) = AngleAxisf((rand() / (double)RAND_MAX) * 2 * PI, Vector3f(0, 0, 1)).toRotationMatrix();
				unsigned int transID = RenderCommandList::setStaticMeshTransform(transformMat * m);
				if (baseTransID == -1) {
					baseTransID = transID;
				}
				if (i == 0 && j == 0) {
					meshRender.instanceID = transID;
					meshRender.instanceCount = transCount;
				}
				for (int i = 0; i < mesh.meshParts.size(); i++)
					RenderCommandList::setStaticMeshPartTransform(&mesh.meshParts[i], meshRender.materials[i], transID);
			}
		}
		update = false;
	}
}

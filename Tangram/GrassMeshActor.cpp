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
}

void GrassMeshActor::set(float density, Vector2i & bound)
{
	this->density = density;
	this->bound = bound;
}

void GrassMeshActor::updateData()
{
	update = true;
}

void GrassMeshActor::end()
{
	MeshActor::end();
	/*for (int i = 0; i < mesh.meshParts.size(); i++)
		RenderCommandList::cleanStaticMeshTransform(&mesh.meshParts[i], meshRender.materials[i]);*/
}

void GrassMeshActor::prerender()
{
	if (update) {
		float w = bound.x() / density;
		int yd = bound.y() / w;
		float yo = (bound.y() - yd * w) * 0.5;
		float hx = bound.x() * 0.5, hy = bound.y() * 0.5;
		int count = (yd + 1) * (density + 1);
		srand(time(NULL));
		/*RenderCommandList::cleanStaticMeshTransform(&mesh, &meshRender);
		for (int i = 0; i < mesh.meshParts.size(); i++)
			RenderCommandList::cleanStaticMeshPartTransform(&mesh.meshParts[i], meshRender.materials[i]);*/
		for (int i = 0; i <= density; i++) {
			for (int j = 0; j <= yd; j++) {
				Matrix4f m = Matrix4f::Identity();
				m(0, 3) = i * w - hx + ((rand() / (double)RAND_MAX) - 0.5) * 2 * w;
				m(1, 3) = j * w - hy + yo + ((rand() / (double)RAND_MAX) - 0.5) * 2 * w;
				m.block(0, 0, 3, 3) = AngleAxisf((rand() / (double)RAND_MAX) * 2 * PI, Vector3f(0, 0, 1)).toRotationMatrix();
				unsigned int transID = RenderCommandList::setStaticMeshTransform(transformMat * m);
				if (i == 0 && j == 0) {
					meshRender.instanceID = transID;
					meshRender.instanceCount = count;
				}
				for (int i = 0; i < mesh.meshParts.size(); i++)
					RenderCommandList::setStaticMeshPartTransform(&mesh.meshParts[i], meshRender.materials[i], transID);
			}
		}
		update = false;
	}
}

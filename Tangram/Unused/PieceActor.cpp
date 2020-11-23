#include "PieceActor.h"

PieceActor::PieceActor(vector<Unit2Di> vertices, Material& material, string name) : piece(vertices), MeshActor::MeshActor(_mesh, material, name)
{
}

void PieceActor::prerender()
{
	piece.toMesh(mesh);
	MeshActor::prerender();
}

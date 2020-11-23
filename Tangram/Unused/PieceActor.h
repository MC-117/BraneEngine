#pragma once
#ifndef _PIECEACTOR_H_
#define _PIECEACTOR_H_

#include "MeshActor.h"
#include "Piece.h"

class PieceActor : public MeshActor
{
public:
	Piece piece;

	PieceActor(vector<Unit2Di> vertices, Material& material, string name = "PieceActor");

	virtual void prerender();
protected:
	Mesh _mesh;
};

#endif // !_PIECEACTOR_H_

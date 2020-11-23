#pragma once
#ifndef _PIECEUI_H_
#define _PIECEUI_H_

#include "UIControl.h"

class PieceUI : public UIControl
{
public:
	bool *checkList = NULL;
	Unit2Di gridSize = { 10, 10 };
	Material& srcMaterial;
	char pieceName[20];
	Color pieceColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	Color pieceOutlineColor = { 0, 0, 0, 255 };

	PieceUI(Object& object, Material& material, string name = "PieceUI", bool defaultShow = false);
	PieceUI(Object& object, Material& material, Unit2Di gridSize, string name = "PieceUI", bool defaultShow = false);

	virtual void render(GUIRenderInfo& info);
protected:
	bool checkVertices(vector<Unit2Di>& res);
};

#endif // !_PIECE_H_

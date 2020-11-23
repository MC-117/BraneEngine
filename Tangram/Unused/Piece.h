#pragma once
#ifndef _PIECE_H_
#define _PIECE_H_

#include "MeshActor.h"

struct PPoint
{
	Unit2Di pos;
	int index;
};

class Piece
{
public:
	/**************************************************
	*  o o o   o o o       o   o       o o o   o o o  *
	*  o /       \ o     / o   o \     o / o   o \ o  *
	*  o           o   o o o   o o o   o o o   o o o  *
	*                                                 *
	*    0       1       2       3       4       5    *
	**************************************************/
	int* data = NULL;
	int rows = 0, cols = 0;
	Unit2Di pos = { 0, 0 };

	Piece();
	Piece(int rows, int cols);
	Piece(vector<Unit2Di>& vertex);
	Piece(const Piece& p);
	Piece(Piece&& p);
	virtual ~Piece();

	void emptyInit(int rows, int cols);
	virtual void shift(Piece& p);
	virtual void cloneFrom(const Piece& p);
	void clear();

	Piece& rot90();
	Piece& rot90r();
	Piece& rot180();
	void toMesh(Mesh& mesh);

	bool match(Piece& pad);

	Piece& operator=(const Piece& p);
	Piece& operator=(Piece&& p);
	int& operator()(int row, int col);
	int& operator()(int row, int col, bool& success);
	Piece& operator+=(Unit2Di t);
	Piece& operator-=(Unit2Di t);
	friend ostream& operator<<(ostream& os, Piece& p);
};

#endif // !_PIECE_H_

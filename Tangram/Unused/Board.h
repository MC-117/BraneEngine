#pragma once
#pragma once
#ifndef _BOARD_H_
#define _BOARD_H_

#include "Piece.h"

class Board : public Piece
{
public:
	Piece checkBoard;
	vector<Piece*> pieces;

	Board(int rows, int cols);
	Board(vector<Unit2Di>& vertex);
	Board(const Board& b);
	Board(Board&& b);

	virtual void shift(Board& p);
	virtual void cloneFrom(const Board& p);

	bool canPlace(Piece& piece);
	bool place(Piece& piece);
	bool isFulfilled();
	void reset();

	Board& operator=(const Board& b);
	Board& operator=(Board&& b);

	Board& rot90() = delete;
	Board& rot90r() = delete;
	Board& rot180() = delete;
	Board& operator+=(Unit2Di t) = delete;
	Board& operator-=(Unit2Di t) = delete;
};

#endif // !_BOARD_H_

#include "Board.h"

Board::Board(int rows, int cols) : Piece::Piece(rows, cols), checkBoard(rows, cols)
{
}

Board::Board(vector<Unit2Di>& vertex) : Piece::Piece(vertex)
{
	if ((rows * cols) != 0)
		checkBoard.emptyInit(rows, cols);
}

Board::Board(const Board & b)
{
	cloneFrom(b);
}

Board::Board(Board && b)
{
	shift(b);
}

void Board::shift(Board & p)
{
	Piece::shift(p);
	checkBoard.shift(p);
	pieces = p.pieces;
}

void Board::cloneFrom(const Board & p)
{
	Piece::cloneFrom(p);
	checkBoard.cloneFrom(p);
	pieces = p.pieces;
}

bool Board::canPlace(Piece & piece)
{
	if (data == NULL)
		throw runtime_error("Not initialize");
	Unit2Di dpos = piece.pos - pos;
	if (dpos.x < 0 || dpos.y < 0 || (dpos.x + piece.rows) > rows || (dpos.y + piece.cols) > cols)
		return false;
	for (int x = piece.pos.x - pos.x; x < rows; x++)
		for (int y = piece.pos.y - pos.y; y < cols; y++) {
			int pd = piece(x, y);
			if (pd != (*this)(x + pos.x, y + pos.y))
				return false;
			else
				switch (checkBoard(x + pos.x, y + pos.y))
				{
				case -1:
					return true;
				case 0:
					return pd == 2;
				case 1:
					return pd == 3;
				case 2:
					return pd == 0;
				case 3:
					return pd == 1;
				case 4:
				case 5:
					return false;
				default:
					return false;
				}
		}
	return false;
}

bool Board::place(Piece & piece)
{
	if (data == NULL)
		throw runtime_error("Not initialize");
	Unit2Di dpos = piece.pos - pos;
	if (!canPlace(piece))
		return false;
	for (int x = piece.pos.x - pos.x; x < rows; x++)
		for (int y = piece.pos.y - pos.y; y < cols; y++) {
			int pd = piece(x, y);
			int &cd = checkBoard(x + pos.x, y + pos.y);
			switch (cd)
			{
			case -1:
				cd = pd;
			case 0:
			case 2:
				cd = 4;
			case 1:
			case 3:
				cd = 5;
			}
		}
	pieces.push_back(&piece);
	return true;
}

bool Board::isFulfilled()
{
	return checkBoard.match(*this);
}

Board & Board::operator=(const Board & b)
{
	cloneFrom(b);
	return *this;
}

Board & Board::operator=(Board && b)
{
	shift(b);
	return *this;
}

void Board::reset()
{
	checkBoard.clear();
	pieces.clear();
}

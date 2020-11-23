#include "Piece.h"
#include <limits>

Piece::Piece()
{
}

Piece::Piece(int rows, int cols)
{
	this->rows = rows;
	this->cols = cols;
	data = new int[rows * cols];
	memset(data, -1, sizeof(int) * rows * cols);
}

Piece::Piece(vector<Unit2Di> & vertex)
{
	if (vertex.size() == 0)
		return;
	int xMin = INT_MAX, xMax = INT_MIN, yMin = INT_MAX, yMax = INT_MIN;
	for (int i = 0; i < vertex.size(); i++) {
		if (vertex[i].x < xMin)
			xMin = vertex[i].x;
		if (vertex[i].x > xMax)
			xMax = vertex[i].x;
		if (vertex[i].y < yMin)
			yMin = vertex[i].y;
		if (vertex[i].y > yMax)
			yMax = vertex[i].y;
	}
	rows = xMax - xMin;
	cols = yMax - yMin;
	data = new int[rows * cols];
	memset(data, -1, sizeof(int) * rows * cols);
	for (int i = 0; i < vertex.size(); i++) {
		Unit2Di &p0 = vertex[i], &p1 = vertex[(i + 1) % vertex.size()], &p2 = vertex[(i + 2) % vertex.size()];
		Unit2Di dp = p1 - p0;
		int pattern, dir = p2.posAtLine(p0, p1);
		bool cover = false;
		if (dp.x != 0 && dp.y != 0) {
			cover = true;
			double a = p0.slope(p1);
			pattern = (a > 0) + ((dir < 0) ^ (a < 0)) * 2;
		}
		dp.unitXY();
		for (Unit2Di u = p0, ou = { dp.x == 0 ? (dir < 0) : (dp.x < 0) , dp.y == 0 ? (dir < 0) : (dp.y < 0)  }; u != p1; u += dp) {
			int &d = data[(u.x - ou.x) * cols + u.y - ou.y];
			if (cover)
				d = pattern;
			else {
				if (d == -1)
					d = 5 - (u.x - ou.x + u.y - ou.y) % 2;
			}
		}
	}
	for (int r = 0; r < rows; r++) {
		bool start = false;
		int pattern = -1;
		int startIndex = 0;
		for (int c = 0; c < cols; c++) {
			int &d = data[r * cols + c];
			if (start) {
				if (d == -1) {
					if (c == cols - 1) {
						memset(data + r * cols + startIndex + 1, -1, sizeof(int) * c - startIndex);
					}
					else {
						d = pattern;
						pattern = 20 / pattern;
					}
				}
				else
					break;
			}
			else if (d != -1) {
				start = true;
				pattern = 5 - d % 2;
				startIndex = c;
			}
		}
	}
}

Piece::Piece(const Piece & p)
{
	cloneFrom(p);
}

Piece::Piece(Piece && p)
{
	shift(p);
}

Piece::~Piece()
{
	if (data != NULL) {
		delete[] data;
		data = NULL;
	}
}

void Piece::emptyInit(int rows, int cols)
{
	this->rows = rows;
	this->cols = cols;
	data = new int[rows * cols];
	memset(data, -1, sizeof(int) * rows * cols);
}

void Piece::shift(Piece & p)
{
	data = p.data;
	p.data = NULL;
	rows = p.rows;
	cols = p.cols;
	pos = p.pos;
}

void Piece::cloneFrom(const Piece & p)
{
	rows = p.rows;
	cols = p.cols;
	pos = p.pos;
	if (data != NULL)
		delete[] data;
	data = new int[rows * cols];
	memcpy(data, p.data, sizeof(int) * rows * cols);
}

void Piece::clear()
{
	if (data != NULL)
		memset(data, -1, sizeof(int) * rows * cols);
}

Piece& Piece::rot90()
{
	if (data == NULL)
		throw runtime_error("Not initialize");
	for (int i = 0, l = rows * cols; i < l; i++) {
		if (data[i] != -1) {
			if (data[i] < 4)
				data[i] = (data[i] + 1) % 4;
			else
				data[i] = (data[i] - 3) % 2 + 4;
		}
	}
	int* p = new int[rows * cols];
	for (int r = 0; r < rows; r++)
		for (int c = 0; c < cols; c++)
			p[r * cols + c] = data[(rows - 1 - r) * cols + c];
	swap(rows, cols);
	for (int r = 0; r < rows; r++)
		for (int c = 0; c < cols; c++)
			data[r * cols + c] = p[c * rows + r];
	delete[] p;
	return *this;
}

Piece& Piece::rot90r()
{
	if (data == NULL)
		throw runtime_error("Not initialize");
	for (int i = 0, l = rows * cols; i < l; i++) {
		if (data[i] != -1) {
			if (data[i] < 4)
				data[i] = (data[i] + 3) % 4;
			else
				data[i] = (data[i] - 1) % 2 + 4;
		}
	}
	int* p = new int[rows * cols];
	for (int r = 0; r < rows; r++)
		for (int c = 0; c < cols; c++)
			p[r * cols + c] = data[r * cols + cols - 1 - c];
	swap(rows, cols);
	for (int r = 0; r < rows; r++)
		for (int c = 0; c < cols; c++)
			data[r * cols + c] = p[c * rows + r];
	delete[] p;
	return *this;
}

Piece& Piece::rot180()
{
	if (data == NULL)
		throw runtime_error("Not initialize");
	for (int i = 0, l = rows * cols; i < l; i++) {
		if (data[i] != -1) {
			if (data[i] < 4)
				data[i] = (data[i] + 2) % 4;
			else
				data[i] = (data[i] - 2) % 2 + 4;
		}
	}
	int s = rows * cols / 2;
	for (int r = 0; r < rows; r++)
		for (int c = 0; c < cols; c++) {
			int i = r * cols + c;
			if (i >= s)
				break;
			swap(data[i], data[(rows - 1 - r) * cols + cols - 1 - c]);
		}
	return *this;
}

int addToVertSet(vector<Unit2Di>& verts, PPoint& point)
{
	if (point.index == -1) {
		point.index = verts.size();
		verts.push_back(point.pos);
	}
	return point.index;
}

void Piece::toMesh(Mesh & mesh)
{
	if (data == NULL)
		throw runtime_error("Not initialize");
	vector<Unit2Di> rawVerts;
	vector<Unit2Di> verts;
	for (int x = 0; x <= rows; x++)
		for (int y = 0; y <= cols; y++) {
			rawVerts.push_back({x + pos.x, y + pos.y});
		}
	int faceCount = 0;
	for (int r = 0; r < rows; r++) {
		for (int c = 0; c < cols; c++) {
			switch (data[r * cols + c])
			{
			case 0:
				verts.push_back(rawVerts[r * (cols + 1) + c]);
				verts.push_back(rawVerts[(r + 1) * (cols + 1) + c]);
				verts.push_back(rawVerts[r * (cols + 1) + c + 1]);
				faceCount++;
				break;
			case 1:
				verts.push_back(rawVerts[r * (cols + 1) + c]);
				verts.push_back(rawVerts[(r + 1) * (cols + 1) + c + 1]);
				verts.push_back(rawVerts[r * (cols + 1) + c + 1]);
				faceCount++;
				break;
			case 2:
				verts.push_back(rawVerts[(r + 1) * (cols + 1) + c]);
				verts.push_back(rawVerts[(r + 1) * (cols + 1) + c + 1]);
				verts.push_back(rawVerts[r * (cols + 1) + c + 1]);
				faceCount++;
				break;
			case 3:
				verts.push_back(rawVerts[r * (cols + 1) + c]);
				verts.push_back(rawVerts[(r + 1) * (cols + 1) + c]);
				verts.push_back(rawVerts[(r + 1) * (cols + 1) + c + 1]);
				faceCount++;
				break;
			case 4:
				verts.push_back(rawVerts[r * (cols + 1) + c]);
				verts.push_back(rawVerts[(r + 1) * (cols + 1) + c]);
				verts.push_back(rawVerts[r * (cols + 1) + c + 1]);
				verts.push_back(rawVerts[(r + 1) * (cols + 1) + c]);
				verts.push_back(rawVerts[(r + 1) * (cols + 1) + c + 1]);
				verts.push_back(rawVerts[r * (cols + 1) + c + 1]);
				faceCount += 2;
				break;
			case 5:
				verts.push_back(rawVerts[r * (cols + 1) + c]);
				verts.push_back(rawVerts[(r + 1) * (cols + 1) + c + 1]);
				verts.push_back(rawVerts[r * (cols + 1) + c + 1]);
				verts.push_back(rawVerts[r * (cols + 1) + c]);
				verts.push_back(rawVerts[(r + 1) * (cols + 1) + c]);
				verts.push_back(rawVerts[(r + 1) * (cols + 1) + c + 1]);
				faceCount += 2;
				break;
			default:
				break;
			}
		}
	}
	mesh.resize(3, faceCount);
	for (int v = 0; v < verts.size(); v++) {
		mesh.vertices(0, v) = verts[v].x;
		mesh.vertices(1, v) = verts[v].y;
		mesh.vertices(2, v) = 0;
	}
}

bool Piece::match(Piece & pad)
{
	if (data == NULL)
		throw runtime_error("Not initialize");
	Unit2Di dpos = pos - pad.pos;
	if (dpos.x < 0 || dpos.y < 0 || (dpos.x + rows) > pad.rows || (dpos.y + cols) > pad.cols)
		return false;
	for(int x = pos.x - pad.pos.x; x < pad.rows; x++)
		for (int y = pos.y - pad.pos.y; y < pad.cols; y++)
			if ((*this)(x, y) != pad(x + pos.x, y + pos.y))
				return false;
	return true;
}

Piece & Piece::operator=(const Piece & p)
{
	cloneFrom(p);
	return *this;
}

Piece & Piece::operator=(Piece && p)
{
	shift(p);
	return *this;
}

int & Piece::operator()(int row, int col)
{
	if (data == NULL)
		throw runtime_error("Not initialize");
	return data[row * cols + col];
}

int & Piece::operator()(int row, int col, bool & success)
{
	if (data == NULL)
		throw runtime_error("Not initialize");
	if (row >= rows || col >= cols) {
		success = false;
		return *data;
	}
	else {
		success = true;
		return data[row * cols + col];
	}

}

Piece & Piece::operator+=(Unit2Di t)
{
	pos.x += t.x;
	pos.y += t.y;
	return *this;
}

Piece & Piece::operator-=(Unit2Di t)
{
	pos.x -= t.x;
	pos.y -= t.y;
	return *this;
}

ostream & operator<<(ostream & os, Piece & p)
{
	if (p.data == NULL)
		throw runtime_error("Not initialize");
	for (int r = 0; r < p.rows; r++) {
		for (int c = 0; c < p.cols; c++) {
			os.width(3);
			os << p(r, c);
		}
		os << endl;
	}
	return os;
}

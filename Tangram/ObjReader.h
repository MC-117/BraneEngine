#pragma once
#ifndef _OBJREADER_H_
#define _OBJREADER_H_

#include "Mesh.h"

static class ObjReader
{
public:
	static bool load(string path, Mesh& mesh);
};

#endif // !_OBJREADER_H_

#pragma once
#ifndef _MATERIALLOADER_H_
#define _MATERIALLOADER_H_

#include "Material.h"

static class MaterialLoader
{
public:
	static bool loadMaterial(Material& material, const string& file);
protected:
	static pair<string, MatAttribute<float>> parseScalar(const string& src);
	static pair<string, MatAttribute<Color>> parseColor(const string& src);
	static pair<string, MatAttribute<string>> parseTexture(const string& src);
};

#endif // !_MATERIALLOADER_H_

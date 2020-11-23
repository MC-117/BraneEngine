#pragma once
#ifndef _FBXREADER_H_
#define _FBXREADER_H_

#include "Unit.h"
#include <fbxsdk.h>

using namespace fbxsdk;

class FbxLoader
{
public:
	FbxManager* manager = NULL;
	FbxImporter* importer = NULL;
	FbxScene* scene = NULL;

	FbxLoader();
	virtual ~FbxLoader();

	bool load(const string& file);
	void printScene();

	static string getAttributeTypeName(FbxNodeAttribute::EType type);
	static void printAttribute(FbxNodeAttribute* pAttribute, int tabs = 0);
	static void printNode(FbxNode* pNode, int tabs = 0);

protected:
	
};

#endif // !_FBXREADER_H_

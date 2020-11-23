#include "FbxLoader.h"

FbxLoader::FbxLoader()
{
	manager = FbxManager::Create();
	FbxIOSettings *ios = FbxIOSettings::Create(manager, IOSROOT);
	manager->SetIOSettings(ios);
	
}

FbxLoader::~FbxLoader()
{
	importer->Destroy();
	manager->Destroy();
}

bool FbxLoader::load(const string & file)
{
	if (importer != NULL)
		importer->Destroy();
	importer = FbxImporter::Create(manager, "");
	if (!importer->Initialize(file.c_str(), -1, manager->GetIOSettings()))
		return false;
	if (scene != NULL)
		scene->Destroy();
	scene = FbxScene::Create(manager, "ImportScene");
	if (!importer->Import(scene))
		return false;
	return true;
}

void FbxLoader::printScene()
{
	if (scene != NULL)
		printNode(scene->GetRootNode());
}

string FbxLoader::getAttributeTypeName(FbxNodeAttribute::EType type)
{
	switch (type) {
	case FbxNodeAttribute::eUnknown: return "unidentified";
	case FbxNodeAttribute::eNull: return "null";
	case FbxNodeAttribute::eMarker: return "marker";
	case FbxNodeAttribute::eSkeleton: return "skeleton";
	case FbxNodeAttribute::eMesh: return "mesh";
	case FbxNodeAttribute::eNurbs: return "nurbs";
	case FbxNodeAttribute::ePatch: return "patch";
	case FbxNodeAttribute::eCamera: return "camera";
	case FbxNodeAttribute::eCameraStereo: return "stereo";
	case FbxNodeAttribute::eCameraSwitcher: return "camera switcher";
	case FbxNodeAttribute::eLight: return "light";
	case FbxNodeAttribute::eOpticalReference: return "optical reference";
	case FbxNodeAttribute::eOpticalMarker: return "marker";
	case FbxNodeAttribute::eNurbsCurve: return "nurbs curve";
	case FbxNodeAttribute::eTrimNurbsSurface: return "trim nurbs surface";
	case FbxNodeAttribute::eBoundary: return "boundary";
	case FbxNodeAttribute::eNurbsSurface: return "nurbs surface";
	case FbxNodeAttribute::eShape: return "shape";
	case FbxNodeAttribute::eLODGroup: return "lodgroup";
	case FbxNodeAttribute::eSubDiv: return "subdiv";
	default: return "unknown";
	}
}

void FbxLoader::printAttribute(FbxNodeAttribute* pAttribute, int tabs) {
	if (!pAttribute) return;

	string typeName = getAttributeTypeName(pAttribute->GetAttributeType());
	string attrName = pAttribute->GetName();
	for (int i = 0; i < tabs; i++)
		cout << '\t';
	cout << typeName.c_str() << ": " << attrName.c_str() << endl;
}

void FbxLoader::printNode(FbxNode * pNode, int tabs)
{
	const char* nodeName = pNode->GetName();
	FbxDouble3 translation = pNode->LclTranslation.Get();
	FbxDouble3 rotation = pNode->LclRotation.Get();
	FbxDouble3 scaling = pNode->LclScaling.Get();

	for (int i = 0; i < tabs; i++)
		cout << '\t';

	cout <<
		nodeName << ' ' <<
		translation[0] << ' ' << translation[1] << ' ' << translation[2] << ' ' <<
		rotation[0] << ' ' << rotation[1] << ' ' << rotation[2] << ' ' <<
		scaling[0] << ' ' << scaling[1] << ' ' << scaling[2] << endl;

	for (int i = 0; i < pNode->GetNodeAttributeCount(); i++)
		printAttribute(pNode->GetNodeAttributeByIndex(i), tabs + 1);

	puts("-------------------------------------------------------------------------");

	// Recursively print the children.
	for (int j = 0; j < pNode->GetChildCount(); j++)
		printNode(pNode->GetChild(j), tabs + 1);

	for (int i = 0; i < tabs; i++)
		cout << '\t';
	cout << endl;
}

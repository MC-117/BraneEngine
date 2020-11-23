#include "ObjReader.h"
#include "Utility.h"
#include <fstream>
#include "Importer.h"

bool ObjReader::load(string path, Mesh & mesh)
{
	Importer imp = Importer(path, aiProcessPreset_TargetRealtime_MaxQuality);
	return imp.getMesh(mesh);
	//string line;
	//vector<vector<float>> vert;
	//vector<vector<float>> uv;
	//vector<vector<float>> norm;
	//vector<vector<unsigned int>> index;
	//map<string, MeshPart> meshParts;
	//MeshPart* currentMeshPart = NULL;
	//int vertPerFace = 0;
	//fstream f;
	//f.open(path, ios::in);
	//if (!f.is_open()) {
	//	cout << "Something Went Wrong When Opening Objfiles" << endl;
	//	return false;
	//}

	//while (getline(f, line)) {
	//	if (line.empty())
	//		continue;
	//	vector<string> parameters = split(line, ' ');
	//	if (parameters.size() == 0)
	//		continue;
	//	else if (parameters[0][0] == '#') {

	//	}
	//	else if (parameters[0] == "v") {
	//		if (parameters.size() != 4) {
	//			cout << "This obj file is broken" << endl;
	//			return false;
	//		}
	//		vector<GLfloat> Point;
	//		for (int i = 1; i < 4; i++) {
	//			GLfloat xyz = atof(parameters[i].c_str());
	//			Point.push_back(xyz);
	//		}
	//		vert.push_back(Point);
	//	}
	//	else if (parameters[0] == "vt") {
	//		if (parameters.size() != 3) {
	//			cout << "This obj file is broken" << endl;
	//			return false;
	//		}
	//		vector<GLfloat>Point;
	//		for (int i = 1; i < 3; i++) {
	//			GLfloat xy = atof(parameters[i].c_str());
	//			Point.push_back(xy);
	//		}
	//		uv.push_back(Point);
	//	}
	//	else if (parameters[0] == "vn") {
	//		if (parameters.size() != 4) {
	//			cout << "This obj file is broken" << endl;
	//			return false;
	//		}
	//		vector<GLfloat>Point;
	//		for (int i = 1; i < 4; i++) {
	//			GLfloat xyz = atof(parameters[i].c_str());
	//			Point.push_back(xyz);
	//		}
	//		norm.push_back(Point);
	//	}
	//	else if (parameters[0] == "f") {
	//		if (parameters.size() < 3) {
	//			cout << "This obj file is broken" << endl;
	//			return false;
	//		}
	//		if (vertPerFace == 0)
	//			vertPerFace = parameters.size() - 1;
	//		if (currentMeshPart == NULL) {
	//			currentMeshPart = &meshParts.insert(pair<string, MeshPart>("Default", { 0, (unsigned int)(index.size() * vertPerFace), 0 })).first->second;
	//		}
	//		for (int i = 1; i < parameters.size(); i++) {
	//			vector<string> info = split(parameters[i], '/');
	//			vector<unsigned int> indexSet;
	//			if (info.size() != 3) {
	//				cout << "This obj file is broken" << endl;
	//				return false;
	//			}
	//			for (int j = 0; j < info.size(); j++)
	//				indexSet.push_back(atoi(info[j].c_str()) - 1);
	//			index.push_back(indexSet);
	//		}
	//	}
	//	else if (parameters[0] == "usemtl") {
	//		if (parameters.size() != 2) {
	//			cout << "This obj file is broken" << endl;
	//			return false;
	//		}
	//		if (currentMeshPart != NULL) {
	//			currentMeshPart->vertexCount = index.size() - currentMeshPart->vertexFirst;
	//		}
	//		currentMeshPart = &meshParts.insert(pair<string, MeshPart>(parameters[1], { 0, (unsigned int)(index.size() * vertPerFace), 0 })).first->second;
	//	}
	//}
	//f.close();
	//if (currentMeshPart != NULL) {
	//	currentMeshPart->vertexCount = index.size() - currentMeshPart->vertexFirst;
	//}
	//MeshPart totalPart = MeshData::newMeshPart(vert.size());
	//mesh.totalMeshPart = totalPart;
	//for (auto b = meshParts.begin(), e = meshParts.end(); b != e; b++) {
	//	b->second.vertexFirst += totalPart.vertexFirst;
	//	b->second.meshData = totalPart.meshData;
	//	mesh.addMeshPart(b->first, b->second);
	//}
	////mesh.resize(vertPerFace, index.size());
	//Matrix<float, 3, 2> bound;
	//bound << FLT_MAX, FLT_MIN,
	//	FLT_MAX, FLT_MIN,
	//	FLT_MAX, FLT_MIN;
	//for (int i = 0; i < index.size(); i++) {
	//	for (int j = 0; j < 3; j++) {
	//		float t = vert[index[i][0]][j];
	//		if (t < bound(j, 0)) {
	//			if (bound(j, 0) != FLT_MAX && bound(j, 0) > bound(j, 1))
	//				bound(j, 1) = bound(j, 0);
	//			bound(j, 0) = t;
	//		}
	//		else if (t > bound(j, 1))
	//			bound(j, 1) = t;
	//		totalPart.vertex(i)(j) = t;
	//	}
	//	if (uv.size() != 0) {
	//		totalPart.uv(i)(0) = uv[index[i][1]][0];
	//		totalPart.uv(i)(1) = uv[index[i][1]][1];
	//	}
	//	if (norm.size() != 0) {
	//		totalPart.normal(i)(0) = norm[index[i][2]][0];
	//		totalPart.normal(i)(1) = norm[index[i][2]][1];
	//		totalPart.normal(i)(2) = norm[index[i][2]][2];
	//	}
	//}
	//mesh.bound = bound;
	//return true;
}

#pragma once
#ifndef _IKSOLVER_H_
#define _IKSOLVER_H_

#include "Transform.h"

class IKSolver : public ::Transform
{
public:
	::Transform *endTransform = NULL;
	::Transform *rootTransform = NULL;

	IKSolver(const string& name = "IKSolver");
	IKSolver(::Transform& root, const string& name = "IKSolver");
	IKSolver(::Transform& root, ::Transform& end, const string& name = "IKSolver");

	bool isEnable();
	bool canSolve();
	void setEnable(bool enable = true);
	bool init();
	void solve();
protected:
	bool _isEnable = true;
	bool _canSolve = false;

	struct TTable
	{
		::Transform* t;
		Vector3f opos;
		Vector3f pos;
		float length;
	};

	vector<TTable> transformTable;

	float solveIteriation(const Vector3f& goal);
};

#endif // !IKSOLVER_H_

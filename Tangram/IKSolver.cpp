#include "IKSolver.h"

IKSolver::IKSolver(const string& name) : ::Transform(name)
{
}

IKSolver::IKSolver(::Transform & root, const string& name) : ::Transform(name), rootTransform(&root)
{
}

IKSolver::IKSolver(::Transform & root, ::Transform & end, const string& name)
	: ::Transform(name), rootTransform(&root), endTransform(&end)
{
}

bool IKSolver::isEnable()
{
	return _isEnable;
}

bool IKSolver::canSolve()
{
	return _canSolve;
}

void IKSolver::setEnable(bool enable)
{
	_isEnable = enable;
}

bool IKSolver::init()
{
	if (rootTransform == NULL || endTransform == NULL || rootTransform == endTransform)
		return false;
	transformTable.clear();
	transformTable.push_back({ endTransform, Vector3f(0, 0, 0), Vector3f(0, 0, 0), 0 });
	Object *t = endTransform->parent;
	::Transform *lt = endTransform;
	while (t != NULL) {
		::Transform* _t = dynamic_cast<::Transform*>(t);
		if (_t != NULL) {
			transformTable.push_back({ _t, Vector3f(0, 0, 0), Vector3f(0, 0, 0), 0 });
			lt = _t;
		}
		if (t == rootTransform) { 
			_canSolve = true;
			return true;
		}
		t = t->parent;
	}
	return false;
}

void IKSolver::solve()
{
	if (_isEnable && _canSolve) {
		Vector3f rootPos = rootTransform->getPosition(WORLD);
		Vector3f pp;
		for (int i = 0; i < transformTable.size(); i++) {
			transformTable[i].opos = transformTable[i].t->getPosition(WORLD);
			transformTable[i].pos = transformTable[i].opos - rootPos;
			if (i != 0)
				transformTable[i].length = (pp - transformTable[i].opos).norm();
			pp = transformTable[i].opos;
		}
		Vector3f g = getPosition(WORLD) - rootTransform->getPosition(WORLD);
		float error = 0.01;
		while (solveIteriation(g) <= error);
		for (int i = 1; i < transformTable.size(); i++) {
			transformTable[i].t->rotate(Quaternionf::FromTwoVectors(transformTable[i - 1].pos - transformTable[i].pos, transformTable[i - 1].opos - transformTable[i].opos));
		}
	}
}

float IKSolver::solveIteriation(const Vector3f & goal)
{
	Vector3f v = goal - transformTable.begin()->pos;
	for (int i = 0; i < transformTable.size(); i++) {
		float l = v.norm();
		transformTable[i].pos += v * (1 - transformTable[i].length / l);
		if (i < transformTable.size() - 1)
			v = transformTable[i].pos - transformTable[i + 1].pos;
	}
	v = -transformTable.back().pos;
	float length = 0;
	for (int i = transformTable.size() - 1; i >= 0; i--) {
		float l = v.norm();
		transformTable[i].pos += v * (1 - length / l);
		length = transformTable[i].length;
		if (i > 0)
			v = transformTable[i].pos - transformTable[i - 1].pos;
	}
	return (transformTable.begin()->pos - goal).norm();
}

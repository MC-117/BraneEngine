#pragma once
#ifndef _PYTHONGRAPHWINDOW_H_
#define _PYTHONGRAPHWINDOW_H_

#include "GraphWindow.h"

class PythonGraphWindow : public GraphWindow
{
public:
	PythonGraphWindow(Object & object, string name = "PythonGraphWindow", bool defaultShow = true);

	virtual void onCreateNode(const Node& node);
	virtual bool onDeleteNode(const Node& node);
	virtual bool onTryConnect(const Connection& conection);
	virtual void onConnect(const Connection& conection);
	virtual bool onDisconnect(const Connection& conection);
	virtual bool canConnect(const Pin& p1, const Pin& p2);
};

#endif // !_PYTHONGRAPHWINDOW_H_

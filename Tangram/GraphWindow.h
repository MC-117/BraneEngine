#pragma once
#ifndef _GRAPHWINDOW_H_
#define _GRAPHWINDOW_H_

#include "UIWindow.h"
#include "imgui_node_editor.h"
#include "Math2D.h"
#include "Builders.h"
#include "Widgets.h"

class GraphWindow : public UIWindow
{
public:
	enum PinShape
	{
		Flow, Circle, Square, Grid, RoundSquare, Diamond
	};

	struct Pin;

	struct Node
	{
		long long id;
		size_t templateIndex;
		vector<Pin> inputPin, outputPin;
		void* ptr = NULL;

		Node();
		Node(const Node& node);
		Node(Node&& node);
	};

	struct Pin
	{
		long long id;
		Node* node;
		size_t index;
		size_t templateIndex;
		string name;
		bool isOutput;

		Pin();
		Pin(const Pin& pin);
	};

	struct Connection
	{
		long long id;
		const Pin *from, *to;
	};

	typedef void(*PinAction)(const Pin& pin, GraphWindow* self);

	struct PinTemplate
	{
		string type;
		Color color;
		PinShape shape = Circle;
		PinAction action = NULL;
	};

	struct NodeTemplate
	{
		string name;
		size_t index;
		bool isSimple = false;
		Color titleColor = { 164, 164, 165 };
		Color bodyColor;
		vector<pair<string, size_t>> inputs, outputs;
	};

	GraphWindow(Object & object, string name = "GraphWindow", bool defaultShow = true);
	virtual ~GraphWindow();

	virtual void onCreateNode(const Node& node) = 0;
	virtual bool onDeleteNode(const Node& node) = 0;
	virtual bool onTryConnect(const Connection& conection) = 0;
	virtual void onConnect(const Connection& conection) = 0;
	virtual bool onDisconnect(const Connection& conection) = 0;
	virtual bool canConnect(const Pin& p1, const Pin& p2);

	virtual void onRenderWindow(GUIRenderInfo& info);

protected:
	struct ConnectionOrder
	{
		bool operator()(const Connection& a, const Connection& b) const
		{
			return a.from < b.from || (a.from == b.from && a.to < b.to);
		}
	};

	struct CreateNodeContext
	{
		string filter;
	} createNodeContext;

	struct PinInfo
	{
		Node* node;
		bool isOutput;
		size_t index;
	};

	ArrayMap<string, PinTemplate> pinTemplates;
	ArrayMap<string, NodeTemplate> nodeTemplates;
	map<long long, Node> nodes;
	map<long long, PinInfo> pins;
	set<Connection, ConnectionOrder> connections;

	ax::NodeEditor::EditorContext* context = NULL;
	ax::NodeEditor::Utilities::BlueprintNodeBuilder builder;

	bool createNewNode = false;
	Pin* newNodeLinkPin = NULL;
	Pin* newLinkPin = NULL;
	ax::NodeEditor::NodeId contextNodeId = 0;
	ax::NodeEditor::PinId contextPinId = 0;
	ImVec2 popupCurserPos;
	float leftPaneWidth = 400.0f;
	float rightPaneWidth = 800.0f;

	long long _nextId = 1;

	long long getNextId();
	Node* getNode(long long id);
	Pin* getPin(long long id);

	Node& createNode(const NodeTemplate& temp, void* ptr = NULL);
	void deleteNode(long long id);
	void createConnection(const Pin& from, const Pin& to);
	void deleteConnection(long long id);

	void drawPin(const Pin& pin);
	void drawNode(const Node& node);

	virtual void pinContextMenu(const Pin& pin);
	virtual void nodeContextMenu(const Node& node);
	virtual void createNodeContextMenu();
};

#endif // !_GRAPHWINDOW_H_

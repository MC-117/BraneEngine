#include "GraphWindow.h"
#include "imgui_stdlib.h"

namespace ne = ax::NodeEditor;
namespace wg = ax::Widgets;

GraphWindow::GraphWindow(Object & object, string name, bool defaultShow) : UIWindow(object, name, defaultShow)
{
	context = ax::NodeEditor::CreateEditor();
}

GraphWindow::~GraphWindow()
{
	ne::DestroyEditor(context);
}

bool GraphWindow::canConnect(const Pin & p1, const Pin & p2)
{
	return p1.isOutput != p2.isOutput && p1.templateIndex == p2.templateIndex;
}

void GraphWindow::onRenderWindow(GUIRenderInfo & info)
{
	ne::SetCurrentEditor(context);

	ImGui::Splitter(true, 4.0f, &leftPaneWidth, &rightPaneWidth, 50.0f, 50.0f);
	ImGui::BeginChild("Selection", ImVec2(leftPaneWidth - 4.0f, 0));
	if (ImGui::CollapsingHeader("Nodes")) {
		for (auto b = nodes.begin(), e = nodes.end(); b != e; b++) {
			char str[256];
			sprintf_s(str, "Node %s\tID(%ld)", nodeTemplates.at(b->second.templateIndex).name.c_str(), b->first);
			if (ImGui::TreeNode(str)) {
				if (ImGui::TreeNode("Inputs")) {
					for (int i = 0; i < b->second.inputPin.size(); i++) {
						ImGui::Text("Pin %s ID(%ld)", b->second.inputPin[i].name.c_str(), b->second.inputPin[i].id);
					}
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("Outputs")) {
					for (int i = 0; i < b->second.outputPin.size(); i++) {
						ImGui::Text("Pin %s ID(%ld)", b->second.outputPin[i].name.c_str(), b->second.outputPin[i].id);
					}
					ImGui::TreePop();
				}
				ImGui::TreePop();
			}
		}
	}
	if (ImGui::CollapsingHeader("Pins")) {
		for (auto b = pins.begin(), e = pins.end(); b != e; b++) {
			char str[256];
			sprintf_s(str, "ID(%ld)", b->first);
			ImGui::MenuItem(str);
		}
	}
	if (ImGui::CollapsingHeader("Connections")) {
		for (auto b = connections.begin(), e = connections.end(); b != e; b++) {
			char str[256];
			sprintf_s(str, "ID(%ld)\tFromPin(%ld)\tToPin(%ld)", b->id, b->from->id, b->to->id);
			ImGui::MenuItem(str);
		}
	}
	ImGui::EndChild();

	ImGui::SameLine(0.0f, 12.0f);

	ne::Begin("Canvas");

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4, 3 });

	auto cursorTopLeft = ImGui::GetCursorScreenPos();

	for (auto b = nodes.begin(), e = nodes.end(); b != e; b++)
		drawNode(b->second);
	for (auto b = connections.begin(), e = connections.end(); b != e; b++) {
		const Connection& conn = *b;
		if (conn.from == NULL || conn.to == NULL)
			continue;
		try {
			PinTemplate& pf = pinTemplates.at(conn.from->templateIndex);
			PinTemplate& pt = pinTemplates.at(conn.to->templateIndex);
			Color color = (pf.color + pt.color) / 2;
			ne::Link(conn.id, conn.from->id, conn.to->id, *(ImVec4*)&color);
		}
		catch (exception e) {
			continue;
		}
	}

	if (!createNewNode) {
		if (ne::BeginCreate(ImColor(255, 255, 255))) {
			ne::PinId fromId = 0, toId = 0;
			if (ne::QueryNewLink(&fromId, &toId)) {
				Pin* fromPin = getPin((size_t)fromId);
				Pin* toPin = getPin((size_t)toId);
				newLinkPin = fromPin ? fromPin : toPin;
				if (fromPin && toPin) {
					if (!fromPin->isOutput)
						swap(fromPin, toPin);
					if (canConnect(*fromPin, *toPin)) {
						if (ne::AcceptNewItem(ImColor(128, 255, 128), 3.0f))
						{
							createConnection(*fromPin, *toPin);
						}
					}
					else
						ne::RejectNewItem(ImColor(255, 0, 0));
				}
			}

			ne::PinId pinId = 0;
			if (ne::QueryNewNode(&pinId))
			{
				newLinkPin = getPin((size_t)pinId);
				if (ne::AcceptNewItem())
				{
					createNewNode = true;
					newNodeLinkPin = newLinkPin;
					newLinkPin = NULL;
					ne::Suspend();
					ImGui::OpenPopup("CreateNodeMenu");
					ne::Resume();
				}
			}
		}
		else
			newLinkPin = NULL;
		ne::EndCreate();

		if (ne::BeginDelete())
		{
			ne::LinkId linkId = 0;
			while (ne::QueryDeletedLink(&linkId))
			{
				if (ne::AcceptDeletedItem())
				{
					deleteConnection((size_t)linkId);
				}
			}

			ne::NodeId nodeId = 0;
			while (ne::QueryDeletedNode(&nodeId))
			{
				if (ne::AcceptDeletedItem())
				{
					deleteNode((size_t)nodeId);
				}
			}
			ne::EndDelete();
		}

		ImGui::SetCursorScreenPos(cursorTopLeft);
	}

	ne::Suspend();

	popupCurserPos = ImGui::GetMousePos();

	if (ne::ShowNodeContextMenu(&contextNodeId))
		ImGui::OpenPopup("NodeContextMenu");
	else if (ne::ShowPinContextMenu(&contextPinId))
		ImGui::OpenPopup("PinContextMenu");
	else if (ne::ShowBackgroundContextMenu())
	{
		ImGui::OpenPopup("CreateNodeMenu");
		newNodeLinkPin = NULL;
	}

	Node* pNode = getNode((size_t)contextNodeId);
	Pin* pPin = getPin((size_t)contextPinId);
	if (pNode)
		nodeContextMenu(*pNode);
	if (pPin)
		pinContextMenu(*pPin);
	createNodeContextMenu();
	ne::Resume();

	ImGui::PopStyleVar();
	
	ne::End();
}

long long GraphWindow::getNextId()
{
	return _nextId++;
}

GraphWindow::Node * GraphWindow::getNode(long long id)
{
	auto iter = nodes.find(id);
	if (iter == nodes.end())
		return NULL;
	return &iter->second;
}

GraphWindow::Pin * GraphWindow::getPin(long long id)
{
	auto iter = pins.find(id);
	if (iter == pins.end())
		return NULL;
	Node* node = iter->second.node;
	if (node == NULL)
		return NULL;
	return iter->second.isOutput ? &node->outputPin[iter->second.index] : &node->inputPin[iter->second.index];
}

GraphWindow::Node & GraphWindow::createNode(const NodeTemplate& temp, void* ptr)
{
	long long nid = getNextId();
	Node& node = nodes.insert(make_pair(nid, Node())).first->second;
	node.id = nid;
	node.ptr = ptr;
	node.templateIndex = temp.index;
	int i = 0;
	for (auto p : temp.inputs) {
		node.inputPin.push_back(Pin());
		Pin& pin = node.inputPin.back();
		pin.id = getNextId();
		pin.index = i;
		pin.isOutput = false;
		pin.name = p.first;
		pin.templateIndex = p.second;
		pin.node = &node;
		pins.insert(make_pair(pin.id, PinInfo{ pin.node, false, pin.index }));
		i++;
	}

	i = 0;
	for (auto p : temp.outputs) {
		node.outputPin.push_back(Pin());
		Pin& pin = node.outputPin.back();
		pin.id = getNextId();
		pin.index = i;
		pin.isOutput = true;
		pin.name = p.first;
		pin.templateIndex = p.second;
		pin.node = &node;
		pins.insert(make_pair(pin.id, PinInfo{ pin.node, true, pin.index }));
		i++;
	}
	onCreateNode(node);
	return node;
}

void GraphWindow::deleteNode(long long id)
{
	auto iter = nodes.find(id);
	if (iter == nodes.end())
		return;
	if (onDeleteNode(iter->second)) {
		for (auto p : iter->second.inputPin)
			pins.erase(p.id);
		for (auto p : iter->second.outputPin)
			pins.erase(p.id);
		nodes.erase(id);
	}
}

void GraphWindow::createConnection(const Pin & from, const Pin & to)
{
	Connection conn;
	conn.id = 0;
	conn.from = &from;
	conn.to = &to;
	auto iter = connections.find(conn);
	if (iter == connections.end()) {
		if (onTryConnect(conn)) {
			conn.id = getNextId();
			connections.insert(conn);
			onConnect(conn);
		}
	}
}

void GraphWindow::deleteConnection(long long id)
{
	auto iter = std::find_if(connections.begin(), connections.end(), [id](auto& link) { return link.id == id; });
	if (iter != connections.end()) {
		if (onDisconnect(*iter))
			connections.erase(iter);
	}
}

void GraphWindow::drawPin(const Pin & pin)
{
	PinTemplate& pt = pinTemplates.at(pin.templateIndex);
	if (pin.isOutput) {
		builder.Output(pin.id);
		ImGui::TextUnformatted(pin.name.c_str());
	}
	else
		builder.Input(pin.id);
	bool linked = ne::PinHadAnyLinks(pin.id);
	float alpha = ImGui::GetStyle().Alpha * ((newLinkPin != NULL && !canConnect(*newLinkPin, pin) && &pin != newLinkPin) ? 0.19 : 1);
	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
	ImColor color = { pt.color.r, pt.color.g, pt.color.b, alpha * pt.color.a };
	wg::Icon(ImVec2(24, 24), (wg::IconType)pt.shape, linked, color, ImColor(0.12f, 0.12f, 0.12f, alpha));
	if (!pin.isOutput) {
		if (pt.action == NULL || linked)
			ImGui::TextUnformatted(pin.name.c_str());
		else
			pt.action(pin, this);
	}
	if (pin.isOutput)
		builder.EndOutput();
	else
		builder.EndInput();
	ImGui::PopStyleVar();
}

void GraphWindow::drawNode(const Node & node)
{
	NodeTemplate& temp = nodeTemplates.at(node.templateIndex);
	builder.Begin(node.id);

	if (!temp.isSimple) {
		builder.Header(*(ImVec4*)&temp.titleColor);
		ImGui::Spring(0);
		ImGui::TextUnformatted(temp.name.c_str());
		ImGui::Spring(1);
		ImGui::Dummy(ImVec2(0, 28));
		builder.EndHeader();
	}

	for (int i = 0; i < node.inputPin.size(); i++)
		drawPin(node.inputPin[i]);

	if (temp.isSimple) {
		builder.Middle();
		ImGui::Spring(1, 0);
		ImGui::TextUnformatted(temp.name.c_str());
		ImGui::Spring(1, 0);
	}

	for (int i = 0; i < node.outputPin.size(); i++)
		drawPin(node.outputPin[i]);

	builder.End();
}

void GraphWindow::pinContextMenu(const Pin & pin)
{
	if (ImGui::BeginPopup("PinContextMenu"))
	{
		ImGui::TextUnformatted("Pin Context Menu");
		ImGui::Separator();
		ImGui::Text("ID: %p", pin.id);
		if (pin.node)
			ImGui::Text("Node: %p", pin.node->id);
		else
			ImGui::Text("Node: %s", "<none>");

		ImGui::EndPopup();
	}
}

void GraphWindow::nodeContextMenu(const Node & node)
{
	if (ImGui::BeginPopup("NodeContextMenu"))
	{
		ImGui::TextUnformatted("Node Context Menu");
		ImGui::Separator();
		ImGui::Text("ID: %p", node.id);
		ImGui::Text("Inputs: %d", (int)node.inputPin.size());
		ImGui::Text("Outputs: %d", (int)node.outputPin.size());
		ImGui::Separator();
		if (ImGui::MenuItem("Delete"))
			deleteNode(node.id);
		ImGui::EndPopup();
	}
}

void GraphWindow::createNodeContextMenu()
{
	if (ImGui::BeginPopup("CreateNodeMenu")) {
		ImGui::InputText("Filter", &createNodeContext.filter);
		Node* node = NULL;
		pair<ArrayMap<string, NodeTemplate>::Iterator, ArrayMap<string, NodeTemplate>::Iterator> iter;
		if (createNodeContext.filter.empty()) {
			iter.first = nodeTemplates.begin();
			iter.second = nodeTemplates.end();
		}
		else {
			string bkey = createNodeContext.filter;
			bkey.back() += 1;
			iter = nodeTemplates.findSimilar(createNodeContext.filter, bkey);
		}
		while (iter.first != iter.second) {
			if (ImGui::MenuItem(iter.first.iter->first.c_str()))
				node = &createNode(*iter.first);
			iter.first++;
		}
		if (node) {
			createNewNode = false;
			ne::SetNodePosition(node->id, popupCurserPos);
		}
		ImGui::EndPopup();
	}
	else
		createNewNode = false;
}

GraphWindow::Node::Node()
{
}

GraphWindow::Node::Node(const Node & node)
{
	id = node.id;
	templateIndex = node.templateIndex;
	inputPin = node.inputPin, outputPin = node.outputPin;
	ptr = node.ptr;
}

GraphWindow::Node::Node(Node && node)
{
	id = node.id;
	templateIndex = node.templateIndex;
	inputPin = node.inputPin, outputPin = node.outputPin;
	ptr = node.ptr;
}

GraphWindow::Pin::Pin()
{
}

GraphWindow::Pin::Pin(const Pin & pin)
{
	id = pin.id;
	node = pin.node;
	index = pin.index;
	templateIndex = pin.templateIndex;
	name = pin.name;
	isOutput = pin.isOutput;
}

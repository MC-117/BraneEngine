#include "PythonGraphWindow.h"
#include "imgui_stdlib.h"

namespace ne = ax::NodeEditor;

PythonGraphWindow::PythonGraphWindow(Object & object, string name, bool defaultShow) : GraphWindow(object, name, defaultShow)
{
	PinTemplate* pt;

	//0
	pt = &*pinTemplates.insert("flow", PinTemplate()).first;
	pt->type = "flow";
	pt->shape = Flow;
	pt->color = Color(255, 255, 255);

	//1
	pt = &*pinTemplates.insert("bool", PinTemplate()).first;
	pt->type = "bool";
	pt->color = Color(220, 48, 48);
	pt->action = [](const Pin& pin, GraphWindow* self) {
		bool v = false;
		ImGui::Checkbox(pin.name.c_str(), &v);
	};

	//2
	pt = &*pinTemplates.insert("int", PinTemplate()).first;
	pt->type = "int";
	pt->color = Color(68, 201, 156);
	pt->action = [](const Pin& pin, GraphWindow* self) {
		int v = 0;
		ImGui::SetNextItemWidth(50);
		ImGui::DragInt(pin.name.c_str(), &v);
	};

	//3
	pt = &*pinTemplates.insert("float", PinTemplate()).first;
	pt->type = "float";
	pt->color = Color(147, 226, 74);
	pt->action = [](const Pin& pin, GraphWindow* self) {
		float v = 0;
		ImGui::PushItemWidth(50);
		ImGui::DragFloat(pin.name.c_str(), &v);
		ImGui::PopItemWidth();
	};
	
	//4
	pt = &*pinTemplates.insert("string", PinTemplate()).first;
	pt->type = "string";
	pt->color = Color(124, 21, 153);
	pt->action = [](const Pin& pin, GraphWindow* self) {
		string v;
		ImGui::PushItemWidth(100.0f);
		ImGui::InputText(pin.name.c_str(), &v);
		ImGui::PopItemWidth();
	};

	//5
	pt = &*pinTemplates.insert("object", PinTemplate()).first;
	pt->type = "object";
	pt->color = Color(51, 150, 215);

	//6
	pt = &*pinTemplates.insert("function", PinTemplate()).first;
	pt->type = "function";
	pt->color = Color(218, 0, 183);

	//7
	pt = &*pinTemplates.insert("delegate", PinTemplate()).first;
	pt->type = "delegate";
	pt->shape = Square;
	pt->color = Color(255, 48, 48);

	NodeTemplate* nt;

	nt = &*nodeTemplates.insert("Begin", NodeTemplate()).first;
	nt->name = "Begin";
	nt->index = nodeTemplates.size() - 1;
	nt->titleColor = Color(255, 128, 128);
	nt->outputs.push_back(make_pair("", 0));

	nt = &*nodeTemplates.insert("Tick", NodeTemplate()).first;
	nt->name = "Tick";
	nt->index = nodeTemplates.size() - 1;
	nt->titleColor = Color(255, 128, 128);
	nt->outputs.push_back(make_pair("", 0));
	nt->outputs.push_back(make_pair("deltaTime", 3));

	nt = &*nodeTemplates.insert("Print", NodeTemplate()).first;
	nt->name = "Print";
	nt->index = nodeTemplates.size() - 1;
	nt->titleColor = Color(164, 164, 165);
	nt->inputs.push_back(make_pair("", 0));
	nt->inputs.push_back(make_pair("object", 5));
	nt->outputs.push_back(make_pair("", 0));

	nt = &*nodeTemplates.insert("PrintString", NodeTemplate()).first;
	nt->name = "PrintString";
	nt->index = nodeTemplates.size() - 1;
	nt->titleColor = Color(164, 164, 165);
	nt->inputs.push_back(make_pair("", 0));
	nt->inputs.push_back(make_pair("string", 4));
	nt->outputs.push_back(make_pair("", 0));

	nt = &*nodeTemplates.insert("LessFloat", NodeTemplate()).first;
	nt->name = "<";
	nt->index = nodeTemplates.size() - 1;
	nt->titleColor = Color(164, 164, 165);
	nt->isSimple = true;
	nt->inputs.push_back(make_pair("", 3));
	nt->inputs.push_back(make_pair("", 3));
	nt->outputs.push_back(make_pair("", 1));

	nt = &*nodeTemplates.insert("AddFloat", NodeTemplate()).first;
	nt->name = "+";
	nt->index = nodeTemplates.size() - 1;
	nt->titleColor = Color(164, 164, 165);
	nt->isSimple = true;
	nt->inputs.push_back(make_pair("", 3));
	nt->inputs.push_back(make_pair("", 3));
	nt->outputs.push_back(make_pair("", 3));

	nt = &*nodeTemplates.insert("SubFloat", NodeTemplate()).first;
	nt->name = "-";
	nt->index = nodeTemplates.size() - 1;
	nt->titleColor = Color(164, 164, 165);
	nt->isSimple = true;
	nt->inputs.push_back(make_pair("", 3));
	nt->inputs.push_back(make_pair("", 3));
	nt->outputs.push_back(make_pair("", 3));

	nt = &*nodeTemplates.insert("MulFloat", NodeTemplate()).first;
	nt->name = "*";
	nt->index = nodeTemplates.size() - 1;
	nt->titleColor = Color(164, 164, 165);
	nt->isSimple = true;
	nt->inputs.push_back(make_pair("", 3));
	nt->inputs.push_back(make_pair("", 3));
	nt->outputs.push_back(make_pair("", 3));

	nt = &*nodeTemplates.insert("DivFloat", NodeTemplate()).first;
	nt->name = "/";
	nt->index = nodeTemplates.size() - 1;
	nt->titleColor = Color(164, 164, 165);
	nt->isSimple = true;
	nt->inputs.push_back(make_pair("", 3));
	nt->inputs.push_back(make_pair("", 3));
	nt->outputs.push_back(make_pair("", 3));

	nt = &*nodeTemplates.insert("LessInt", NodeTemplate()).first;
	nt->name = "<";
	nt->index = nodeTemplates.size() - 1;
	nt->titleColor = Color(164, 164, 165);
	nt->isSimple = true;
	nt->inputs.push_back(make_pair("", 2));
	nt->inputs.push_back(make_pair("", 2));
	nt->outputs.push_back(make_pair("", 1));

	nt = &*nodeTemplates.insert("AddInt", NodeTemplate()).first;
	nt->name = "+";
	nt->index = nodeTemplates.size() - 1;
	nt->titleColor = Color(164, 164, 165);
	nt->isSimple = true;
	nt->inputs.push_back(make_pair("", 2));
	nt->inputs.push_back(make_pair("", 2));
	nt->outputs.push_back(make_pair("", 2));

	nt = &*nodeTemplates.insert("SubInt", NodeTemplate()).first;
	nt->name = "-";
	nt->index = nodeTemplates.size() - 1;
	nt->titleColor = Color(164, 164, 165);
	nt->isSimple = true;
	nt->inputs.push_back(make_pair("", 2));
	nt->inputs.push_back(make_pair("", 2));
	nt->outputs.push_back(make_pair("", 2));

	nt = &*nodeTemplates.insert("MulInt", NodeTemplate()).first;
	nt->name = "¡Á";
	nt->index = nodeTemplates.size() - 1;
	nt->titleColor = Color(164, 164, 165);
	nt->isSimple = true;
	nt->inputs.push_back(make_pair("", 2));
	nt->inputs.push_back(make_pair("", 2));
	nt->outputs.push_back(make_pair("", 2));

	nt = &*nodeTemplates.insert("DivInt", NodeTemplate()).first;
	nt->name = "¡Â";
	nt->index = nodeTemplates.size() - 1;
	nt->titleColor = Color(164, 164, 165);
	nt->isSimple = true;
	nt->inputs.push_back(make_pair("", 2));
	nt->inputs.push_back(make_pair("", 2));
	nt->outputs.push_back(make_pair("", 2));

	nt = &*nodeTemplates.insert("Log", NodeTemplate()).first;
	nt->name = "Log";
	nt->index = nodeTemplates.size() - 1;
	nt->titleColor = Color(128, 195, 248);
	nt->inputs.push_back(make_pair("", 0));
	nt->inputs.push_back(make_pair("object", 5));
	nt->outputs.push_back(make_pair("", 0));

	nt = &*nodeTemplates.insert("Error", NodeTemplate()).first;
	nt->name = "Error";
	nt->index = nodeTemplates.size() - 1;
	nt->titleColor = Color(128, 195, 248);
	nt->inputs.push_back(make_pair("", 0));
	nt->inputs.push_back(make_pair("object", 5));
	nt->outputs.push_back(make_pair("", 0));

	nt = &*nodeTemplates.insert("If", NodeTemplate()).first;
	nt->name = "If";
	nt->index = nodeTemplates.size() - 1;
	nt->titleColor = Color(164, 164, 165);
	nt->inputs.push_back(make_pair("", 0));
	nt->inputs.push_back(make_pair("Condition", 1));
	nt->outputs.push_back(make_pair("Then", 0));
	nt->outputs.push_back(make_pair("Else", 0));

	nt = &*nodeTemplates.insert("While", NodeTemplate()).first;
	nt->name = "While";
	nt->index = nodeTemplates.size() - 1;
	nt->titleColor = Color(164, 164, 165);
	nt->inputs.push_back(make_pair("", 0));
	nt->inputs.push_back(make_pair("Condition", 1));
	nt->outputs.push_back(make_pair("Loop", 0));
	nt->outputs.push_back(make_pair("Complete", 0));
}

void PythonGraphWindow::onCreateNode(const Node & node)
{
}

bool PythonGraphWindow::onDeleteNode(const Node & node)
{
	return true;
}

bool PythonGraphWindow::onTryConnect(const Connection & conection)
{
	return true;
}

void PythonGraphWindow::onConnect(const Connection & conection)
{
}

bool PythonGraphWindow::onDisconnect(const Connection & conection)
{
	return true;
}

bool PythonGraphWindow::canConnect(const Pin & p1, const Pin & p2)
{
	if (GraphWindow::canConnect(p1, p2)) {
		return p1.node != p2.node && (p2.templateIndex == 0 || !ne::PinHadAnyLinks(p2.id));
	}
	else
		return false;
}

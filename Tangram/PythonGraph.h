#pragma once
#ifndef _PYTHONGRAPH_H_
#define _PYTHONGRAPH_H_

#include "PythonManager.h"
#include "Utility.h"

class PythonGraph
{
public:
	struct NodeTemplate;
	struct Node;
	struct NodeValue;
	class BlockCode;

	enum ValueType
	{
		Flow, Bool, Int, Float, String, Object, Vec3
	};

	enum NodeType
	{
		FunctionDefineType, FunctionType, ExpresionType, BrenchType, WhileType, ForType
	};

	struct NodeValueOrder
	{
		bool operator()(const NodeValue& a, const NodeValue& b) const
		{
			return a.node < b.node || (a.node == b.node && a.index < b.index);
		}
	};

	struct Variable
	{
		string name;
		ValueType type;
		BlockCode* block = NULL;
	};

	class BlockCode
	{
	public:
		string defineLine;
		list<Variable> variables;
	};

	class FunctionCode : public BlockCode
	{
	public:
		list<Variable> parameters;
		list<string> codeLines;

		void emitVariable(const NodeValue& value);
		void emitCodeLine(const string& code);
	protected:
		size_t nextVariableIndex = 0;
	};

	class ClassCode : public BlockCode
	{
	public:
		list<BlockCode*> functions;

	};

	struct NodeValue
	{
		Node* node;
		size_t index;
		bool isRef = false;
		Variable* variable = NULL;
		void* value = NULL;

		string toString();
	};

	struct Node
	{
		NodeTemplate* nodeTemplate;
		vector<NodeValue> inputs;
		vector<vector<NodeValue>> outputs;

		string toString();
	};

	struct NodeTemplate
	{
		NodeType type;
		string name;
		vector<pair<string, ValueType>> inputs;
		vector<pair<string, ValueType>> outputs;

		virtual string toString(const Node& node) const = 0;
	};
};

#endif // !_PYTHONGRAPH_H_

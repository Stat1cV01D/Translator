#ifndef CONTEXT_H_INCLUDED
#define CONTEXT_H_INCLUDED
#include <string>
#include <vector>

class TBlockContext
{
private:
	static bool EarlyFunctionDefPush;
	static TBlockContext *bl_context;

    TBlockContext *parent;
    int curSubBlock;
    std::string ns;
	//std::vector<TBlockContext*> children;

	TBlockContext(TBlockContext *parent, std::string& name)
		: children()
	{
		this->parent = parent;
		//parent->children.emplace_back(this);
		this->curSubBlock = 0;
		this->ns = name;
	}

	~TBlockContext()
	{
		/*
		auto it_end = parent->children.end();
		for (auto it = parent->children.begin(); it != it_end; it++)
		{
			if ((*it) == this)
			{
				parent->children.erase(it);
				break;
			}
		}
		*/
	}
public:
	static void Init();
	static void Push();
	static void Push_FunctionParametersDef(std::string &funcName);
	static void Pop();
	static TBlockContext *GetCurrent();
	std::string GetBlockNamepace();
	TBlockContext *GetParent();
};

#endif // CONTEXT_H_INCLUDED

#ifndef CONTEXT_H_INCLUDED
#define CONTEXT_H_INCLUDED
#include <string>

class TBlockContext
{
private:
	static TBlockContext *bl_context;

    TBlockContext *parent;
    int curSubBlock;
    std::string ns;

	TBlockContext(TBlockContext *parent, std::string& name)
	{
		this->parent = parent;
		curSubBlock = 0;
		this->ns = name;
	}

public:
	static void Init();
	static void Push();
	static void Pop();
	std::string GetBlockNamepace();
	static TBlockContext *GetCurrent();
	TBlockContext *GetParent();
};

#endif // CONTEXT_H_INCLUDED

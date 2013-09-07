#ifndef PT_H_INCLUDED
#define PT_H_INCLUDED

struct PtNode
{
    char *type;
    char *text;
    PtNode *next;
    PtNode *firstChild;
};

PtNode *createPtNode(const char *typeName);
void setPtNodeText(PtNode *ptNode, char *text);
void setPtNodeChildren(PtNode *ptNode, int childrenCount, ...);

PtNode *createPtNodeWithChildren(const char *typeName, int childrenCount, ...);


#endif // PT_H_INCLUDED

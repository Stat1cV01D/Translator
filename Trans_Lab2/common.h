#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include "types.h"

class AstNode;
struct PtNode;

struct Node
{
    AstNode *astNode;
    PtNode *ptNode;
};

Node *createNode(AstNode *astNode, PtNode *ptNode);
Node *createLexemNode(const char *typeName);

char *strcatn(int n, ...);
const char *typeToStr(enumTypes type);
//Node *createInterriorNode(AstNode *astNode, PtNode *ptNode, int childrenCount, ...);

#endif // COMMON_H_INCLUDED

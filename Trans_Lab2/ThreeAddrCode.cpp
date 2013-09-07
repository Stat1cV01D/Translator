#include "ThreeAddrCode.h"
#include "ast.h"

FILE* g_outputFile = NULL;

// forward node type callback functions declaration
int form_3addrCode(const AstNode *astNode, int tab); 
#define NODETYPE(NAME) int ThreeAddrLexeme_##NAME(const AstNode *astNode, int tab);
#include "nodeTypes.inc"
#undef NODETYPE

// node type callbacks array initialization
#define NODETYPE(NAME) ThreeAddrLexeme_##NAME,
LEXEMEPRINTCALLBACK ThreeACLexemeCallbacks[] = 
{
	form_3addrCode, //Dispatch callback
	#include "nodeTypes.inc"
};
#undef NODETYPE

//NOTE: operations and identifiers return values to $A (accumulator) register

int form_3addrCode(const AstNode *astNode, int tab)
{
	if (!astNode) 
		return 1;

	ThreeACLexemeCallbacks[astNode->GetASTType()](astNode, tab);
	return 0;
}

int ThreeAddrLexeme_EMPTY(const AstNode *astNode, int tab)
{
	return 0;
}

int ThreeAddrLexeme_STMNT_BLOCK(const AstNode *astNode, int tab)
{
	int i;
	for (i=0; i<astNode->body.stmntBlock.nStmnts; ++i)
	{
		ThreeACLexemeCallbacks[LexemeDispatch](astNode->body.stmntBlock.stmnts[i], tab + 2);
	}
	return 0;
}

int ThreeAddrLexeme_STMNT(const AstNode *astNode, int tab)
{
	ThreeACLexemeCallbacks[LexemeDispatch](astNode->body.stmnt, tab + 2);
	return 0;
}

int ThreeAddrLexeme_IF(const AstNode *astNode, int tab)
{
	int labelNumber_false = g_LastLabelNumber++;
	int labelNumber_end = labelNumber_false;
	if (astNode->body.ifStmnt.block2)
		labelNumber_end = g_LastLabelNumber++;

	ThreeACLexemeCallbacks[LexemeDispatch](astNode->body.ifStmnt.cond, tab + 4);
	fprintf(g_outputFile, "iffalse[$A]\tgoto\t$L%d", labelNumber_false);

	ThreeACLexemeCallbacks[LexemeDispatch](astNode->body.ifStmnt.block1, tab + 4);

	if (astNode->body.ifStmnt.block2)
	{
		fprintf(g_outputFile, "goto\t$L%d\n", labelNumber_end); // for the iftrue block to end
		ThreeACLexemeCallbacks[LexemeDispatch](astNode->body.ifStmnt.block2, tab + 4);
	}

	fprintf (g_outputFile, "$L%d:\n", labelNumber_end);
	return 0;
}

int ThreeAddrLexeme_OPER(const AstNode *astNode, int tab)
{
	ThreeACLexemeCallbacks[LexemeDispatch](astNode->body.oper.first, tab + 2);
	fprintf(g_outputFile, "$B\t=\t$A\n");

	if (astNode->body.oper.second)
	{
		fprintf(g_outputFile, " ");
		ThreeACLexemeCallbacks[LexemeDispatch](astNode->body.oper.second, tab + 2);
	}
	// NOTE: if we won't have the second op, then we'll still have first op in $A and $B
	fprintf(g_outputFile, "$C\t=\top[%s]\t$B", astNode->body.oper.text);
	fprintf(g_outputFile, "$A\t=\t$C", astNode->body.oper.text);

	return 0;
}

int ThreeAddrLexeme_DECL_ID(const AstNode *astNode, int tab)
{
	fprintf(g_outputFile, "%s\t", TypeStrings[astNode->resultType]);
	if (astNode->body.identifier.isConstant)
		fprintf(g_outputFile, "$c");
	else
		fprintf(g_outputFile, "$id");
	fprintf(g_outputFile, "%s", astNode->body.identifier.name);

	int *sizes = NULL;
	int i, numDimensions = GetDimensionInfo(astNode->body.identifier.dim_list, &sizes);
	for (i = numDimensions - 1; i >= 0 ; i--)
	{
		fprintf(g_outputFile, "[%d]", sizes[i]); 
	}
	fprintf(g_outputFile, "\n");
	return 0;
}

int ThreeAddrLexeme_ID(const AstNode *astNode, int tab)
{
	int numDimensions = GetDimensionInfo(astNode->body.identifier.dim_list, NULL);
	ThreeACLexemeCallbacks[LexemeDispatch](astNode->body.identifier.dim_list, tab + 4);

	YYLTYPE fake_loc = {0, 0, 0, 0};
	VALUE_TYPE var = getVar(astNode->body.identifier.name, 1, astNode->body.identifier.dim_list, fake_loc);

	// final 1-dimension value will be in $B
	int i = 0;
	for (int i = 0; i < numDimensions; i++)
	{
		fprintf(g_outputFile, "pop\t$A\n");
		fprintf(g_outputFile, "$A\t=\t$A\t*\t%d\n", var->dimensionSizesDecl[i++]);
		fprintf(g_outputFile, "$B\t=\t$B\t+\t$A\n");
	}

	// $A = $idIDNAME[$B]
	fprintf(g_outputFile, "$A\t=\t");
	if (astNode->body.identifier.isConstant)
		fprintf(g_outputFile, "$c");
	else
		fprintf(g_outputFile, "$id");

	fprintf(g_outputFile, "%s[$B]\n", astNode->body.identifier.name);

	return 0;
}

int ThreeAddrLexeme_LOOP(const AstNode *astNode, int tab)
{
	printf("loop\n");
	// base is for (init_expr; b_expr; post_expr) statement;
	// and the do{statement;}while(b_expr); loops (bool post_check for that)

	AstNode *init_expr = astNode->body.loop.cond->body.loop_cond.init_expr, 
			*b_expr = astNode->body.loop.cond->body.loop_cond.b_expr, 
			*post_expr = astNode->body.loop.cond->body.loop_cond.post_expr;
	int labelNumber_loop = g_LastLabelNumber++;
	int labelNumber_end = g_LastLabelNumber++;

	// decl
	if (init_expr)
	{
		ThreeACLexemeCallbacks[LexemeDispatch](init_expr, tab + 4);
		fprintf(g_outputFile, "$B\t=\t$A\n");
	}

	fprintf (g_outputFile, "$L%d:\n", labelNumber_loop);
	if (b_expr && !astNode->body.loop.post_check)
	{
		ThreeACLexemeCallbacks[LexemeDispatch](b_expr, tab + 4);
		fprintf(g_outputFile, "iffalse[$A]\tgoto\t$L%d", labelNumber_end);
	}

	if (astNode->body.loop.block)
	{
		ThreeACLexemeCallbacks[LexemeDispatch](astNode->body.loop.block, tab + 4);
	}

	if (post_expr)
	{
		ThreeACLexemeCallbacks[LexemeDispatch](post_expr, tab + 4);
	}
	
	if (b_expr && astNode->body.loop.post_check)
	{
		ThreeACLexemeCallbacks[LexemeDispatch](b_expr, tab + 4);
		fprintf(g_outputFile, "iftrue[$A]\tgoto\t$L%d", labelNumber_loop);
	}
	else
	{
		fprintf(g_outputFile, "goto\t$L%d\n", labelNumber_loop);
	}
	fprintf (g_outputFile, "$L%d:\n", labelNumber_end);

	return 0;
}

int ThreeAddrLexeme_DIMENSION(const AstNode *astNode, int tab)
{
	if (astNode->body.dimension.next_dim)
		ThreeACLexemeCallbacks[LexemeDispatch](astNode->body.dimension.next_dim, tab + 4);

	fprintf(g_outputFile, "push %d\n", astNode->body.dimension.number);

	return 0;
}

int ThreeAddrLexeme_LOOP_COND(const AstNode *astNode, int tab)
{
	// ignore this coz ThreeAddrLexeme_LOOP interprets this
	return 0;
}

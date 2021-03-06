%require "2.5"
%defines
%locations
%verbose
%debug

%code top
{
#include <stddef.h>
#include <string.h>
#include <string>
#include "parser.h"
#include "AstUtils.h"
#include "resources.h"
#include "variable.h"
#include "ast.h"
#include "pt.h"
#pragma warning(disable : 4003)
#pragma warning(disable : 4005)

void yyerror(const char *message);
#define YYDEBUG 1
}

%code requires
{
#include "parser.h"
#include "common.h"
struct Node;
}

%union {
	Node *_node;
	AstNode *_AstNode;
}

%token <_node> TOK_IDENTIFIER

%token <_node> TOK_B_AND
%token <_node> TOK_B_OR
%token <_node> TOK_B_XOR
%token <_node> TOK_B_NOT
%token <_node> TOK_AR_PLUS
%token <_node> TOK_AR_MINUS
%token <_node> TOK_AR_MUL
%token <_node> TOK_AR_DIV
%token <_node> TOK_AR_MOD

%token <_node> TOK_ASSIGN_OP
%token <_node> TOK_COMP_OP
%token <_node> TOK_IF
%token <_node> TOK_ELSE
%token <_node> TOK_ENDEXPR
%token <_node> TOK_OPENPAR
%token <_node> TOK_CLOSEPAR
%token <_node> TOK_OPENBR
%token <_node> TOK_CLOSEBR
%token <_node> TOK_OPENSQ
%token <_node> TOK_CLOSESQ
%token <_node> TOK_DOT
%token <_node> TOK_COMMA
%token <_node> TOK_DOUBLEDOT

%token <_node> TOK_ROM_DECL
%token <_node> TOK_INT_DECL
%token <_node> TOK_FLOAT_DECL
%token <_node> TOK_VOID_DECL

%token <_node> TOK_FOR_DECL
%token <_node> TOK_WHILE_DECL
%token <_node> TOK_DO_DECL

%token <_node> TOK_ROM_CONST
%token <_node> TOK_INT_CONST
%token <_node> TOK_FLOAT_CONST

%token <_node> TOK_PRINT
%token <_node> TOK_READ
%token <_node> TOK_BREAK
%token <_node> TOK_CONTINUE
%token <_node> TOK_GOTO
%token <_node> TOK_RETURN
%token <_node> TOK_STRUCT
%token <_node> TOK_UNION
%token <_node> TOK_SWITCH
%token <_node> TOK_CASE
%token <_node> TOK_DEFAULT

%token <_node> TOK_STRING_LITERAL

%nonassoc EXPR_ERROR
%nonassoc STMNT_BLOCK_ERROR

%nonassoc IF_WITHOUT_ELSE
%nonassoc TOK_ELSE

%right TOK_ASSIGN_OP
%left TOK_COMP_OP
%left TOK_AR_MOD
%left TOK_AR_MINUS TOK_AR_PLUS TOK_B_OR TOK_B_XOR
%left TOK_AR_MUL TOK_AR_DIV TOK_B_AND
%left UMINUS NOTX
%left TOK_DOT

%type <_node> start declaration_list stmnt stmnt_list stmnt_block_start stmnt_block functions_def_list
%type <_node> expression_statement expr_or_assignment expr struct_item identifier declaration_stmt declaration_block struct_type
%type <_node> if_stmt loop_decl switch_stmt print_stmt read_stmt assignment struct_def struct_head struct_body struct_tail
%type <_node> loop_for_expr instruction_body loop_while_expr type array type_name left_assign_expr const
%type <_node> switch_head case_list default case_stmt case_head case_body
%type <_node> default_head for_decl while_decl do_while_decl
%type <_node> lexemes parameter parameter_list parameter_type_list func_declarator 
%type <_node> declarator function_def_head function_call function_def

%code top
{
AstNode *astTree;
PtNode *ptTree;
ParserContext Context;
}

%{
Node* CreateExpressionNode(Node *op, bool isBooleanOp, Node *left, Node *right, const YYLTYPE left_loc, const YYLTYPE right_loc)
{
	BaseTypeInfo *type = (isBooleanOp ? new BoolType() : left->astNode->GetResultType()->Clone());
	AstNode *rightOpAst = (right != nullptr ? right->astNode : nullptr);
	PtNode *ptNode;

 	AssertOneOfTypes(left, left_loc, 4, BITS_TYPE, INT_TYPE, FLOAT_TYPE, ROM_TYPE);
	if (right != nullptr)
	{
		AssertOneOfTypes(right, right_loc, 4, BITS_TYPE, INT_TYPE, FLOAT_TYPE, ROM_TYPE);
		ptNode = createPtNodeWithChildren("expr", 3, left->ptNode, op->ptNode, right->ptNode);
	}
	else
	{
		ptNode = createPtNodeWithChildren("expr", 2, op->ptNode, left->ptNode);
	}

	return createNode(new OperatorAstNode(op->ptNode->text, left->astNode, rightOpAst, new VarAstNode(true, Context.GenerateNewTmpVar(type))), 
				ptNode);
}

TVariable *GetVariableForAssign(Node *node, YYLTYPE location)
{
	auto structItemAstNode = dynamic_cast<StructAddressAstNode*>(node->astNode);
	if (structItemAstNode != nullptr)
	{
		return structItemAstNode->GetField();
	}
	else
	{
		return Context.getVar(node->ptNode->firstChild->text, 1, NULL, location);
	}
}

std::vector<AstNode*> GetParametersList(AstNode *_node)
{
	std::vector<AstNode*> result;
	auto node = dynamic_cast<StatementBlockAstNode *>(_node);
	if (node == nullptr)
	{
		// means we only got one parameter
		result.emplace_back(_node);
	}
	else	
	{
		node->ProcessStatements(
			[&result](AstNode *node) -> int
			{
				auto blockAstNode = dynamic_cast<StatementBlockAstNode*>(node);
				if (blockAstNode != nullptr)
				{
					auto _result = GetParametersList(blockAstNode);
					for(auto it = _result.begin(); it != _result.end(); it++)
					{
						result.emplace_back(*it);
					}
				}
				else
				{
					result.emplace_back(node);
				}
				return 0;
			}
		);
	}
		
	return result;
}

%}

%%
start :  /* empty */
	{ 
		$$ = NULL; 
	}
	| declaration_list stmnt_list
	{
		PtNode *ptNode = createPtNode("start");
		setPtNodeChildren(ptNode, 1, $stmnt_list->ptNode);
		ptTree = ptNode;
		astTree = $stmnt_list->astNode;
	}
	| declaration_list functions_def_list stmnt_list
	{
		PtNode *ptNode = createPtNode("start");
		setPtNodeChildren(ptNode, 1, $stmnt_list->ptNode);
		ptTree = ptNode;

		auto startLabelNode = new LabelAstNode(Context.MakeLabel("$start"));

		auto _node1 = addStmntToBlock(createNode(new OperatorAstNode(OP_GOTO, startLabelNode), createPtNodeWithChildren("goto start", 0)), $2);		
		auto _node2 = addStmntToBlock(_node1, createNode(startLabelNode, nullptr));		
		auto _node = addStmntToBlock(_node2, $3);

		astTree = _node->astNode;
	}
	| declaration_list
	{
		$$ = NULL;
	}
	| stmnt_list
	{
		PtNode *ptNode = createPtNode("start");
		setPtNodeChildren(ptNode, 1, $stmnt_list->ptNode);
		astTree = $stmnt_list->astNode;
		ptTree = ptNode;
	}
	| stmnt_list declaration_list
	{
		$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, ERROR_DECLARATIONS_FIRST, @2),
				nullptr);
	}
	;

functions_def_list:
	function_def
	{
		// Can't define function in function
		if (Context.OnFunctionDefinition())
		{
			$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, FUNCTION_IN_FUNCTION, @1), 
					nullptr);
		}
		else
		{
			$$ = $1;
		}
	}
	| functions_def_list function_def
	{	
		$$ = addStmntToBlock($1, $2);
	}
	;

declaration_list: 
	declaration_block TOK_ENDEXPR[end]
	{
		$$ = NULL;
	}
	| declaration_list declaration_block TOK_ENDEXPR[end] 
	{
		$$ = NULL;
	}
	| declaration_block error
	{
		$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, EXPECTED_SEPARATOR, @error),
				nullptr);
	}
	;

stmnt_list: stmnt
	{
		$$ = $1;
	}
	| stmnt_list stmnt
	{
		$$ = addStmntToBlock($1, $2);
	}
	;

stmnt_block_start
    :
    TOK_OPENBR[st] {TBlockContext::Push();}
	{
		$$ = $st;
	}
    ;

stmnt_block
	: 
	stmnt_block_start[st] stmnt_list[stmnts] TOK_CLOSEBR[end] {TBlockContext::Pop();}
	{
		$$ = createNode($stmnts->astNode, 
				createPtNodeWithChildren("stmnt_block", 3, $st->ptNode, $stmnts->ptNode, $end->ptNode));
	}
	|
	stmnt_block_start[st] stmnt_list[stmnts] error %prec STMNT_BLOCK_ERROR
	{
		$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, EXPECTED_CLOSE_BRACE, @error), 
				nullptr);
	}
	| 
	stmnt_block_start[st] declaration_list[decls] TOK_CLOSEBR[end] {TBlockContext::Pop();}
	{
		$$ = createNode(nullptr, 
				createPtNodeWithChildren("stmnt_block", 2, $st->ptNode, $end->ptNode));
	}
	| 
	stmnt_block_start[st] declaration_list[decls] stmnt_list[stmnts] TOK_CLOSEBR[end] {TBlockContext::Pop();}
	{
		$$ = createNode($stmnts->astNode, 
				createPtNodeWithChildren("stmnt_block", 3, $st->ptNode, $stmnts->ptNode, $end->ptNode));
	}
	|
	stmnt_block_start[st] declaration_list[decls] stmnt_list[stmnts] error %prec STMNT_BLOCK_ERROR
	{
		$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, EXPECTED_CLOSE_BRACE, @error), 
				nullptr);
	}
	;

expr_or_assignment:
	expr
	{
		$$ = $1;
	}
	|
	assignment
	{
		$$ = $1;
	}
	;

expression_statement: 
	TOK_ENDEXPR[end]
	{
		$$ = createNode(nullptr, 
				createPtNodeWithChildren("expression_statement", 1, $end->ptNode));
	}
	| 
	expr_or_assignment TOK_ENDEXPR
	{
		$$ = createNode($1->astNode, 
				createPtNodeWithChildren("expression_statement", 2, $1->ptNode, $2->ptNode));
	}
	|
	expr error
	{
		$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, EXPECTED_SEPARATOR, @error), 
				nullptr);
	}
	;

lexemes:
	TOK_GOTO TOK_IDENTIFIER
	{
		char *labelName = $2->ptNode->text;
		TLabel* label =  Context.GetLabel(labelName);
		
		if(label == NULL)
		{
			label = Context.MakeLabel(labelName);
		}
		label->SetUsedLine((@1).first_line);

		$$ = createNode(new OperatorAstNode(OP_GOTO, new LabelAstNode(label)), 
				createPtNodeWithChildren("stmnt", 2, $1->ptNode, $2->ptNode));
	}
	| 
	TOK_BREAK
	{
		AstNode *astNode;
		 
		if(!Context.CanUseBreak())
		{
			astNode = new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, UNEXPECTED_BREAK_ERROR, @1);
		}
		else
		{
			TOperator *op = Context.OperatorStackTop();
			TLabel *EndLabel;
			switch(op->GetType())
			{
			case OT_FOR:
			case OT_WHILE:
			case OT_DO_WHILE:
				EndLabel = ((TSimpleOperator *)op)->GetOutLabel(); 
				break;
			case OT_SWITCH:
				EndLabel = ((TSwitchOperator *)op)->GetEndLabel();
				break;
			}
			astNode = new OperatorAstNode(OP_BREAK, new LabelAstNode(EndLabel));
		}
		$$ = createNode(astNode, 
				createPtNodeWithChildren("stmnt", 1, $1->ptNode));
	}
	| 
	TOK_CONTINUE
	{
		AstNode *astNode;
		if(!Context.CanUseContinue())
		{
			astNode = new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, UNEXPECTED_CONTINUE_ERROR, @1);
		}
		else
		{
			TOperator *op = Context.OperatorStackTop();
			TLabel *StartLabel;
			switch(op->GetType())
			{
			case OT_FOR:
			case OT_WHILE:
			case OT_DO_WHILE:
				StartLabel = ((TSimpleOperator *)op)->GetEntranceLabel(); 
				break;
			}
			astNode = new OperatorAstNode(OP_CONTINUE, new LabelAstNode(StartLabel));
		}
		$$ = createNode(astNode, 
				createPtNodeWithChildren("stmnt", 1, $1->ptNode));
	}
	|
	TOK_RETURN
	{
		AstNode *astNode;
		if (!Context.OnFunctionDefinition())
		{
			astNode = new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, UNEXPECTED_RETURN_ERROR, @1);
		}
		else
		{
			auto funcDefOp = dynamic_cast<TFunctionOperator*>(Context.OperatorStackTop());
			if (funcDefOp->GetResultType()->getID() != VOID_TYPE)
				astNode = new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, FUNCTION_INVALID_RETURN_ERROR, @1);
			else
				astNode = new OperatorAstNode(OP_RETURN, new LabelAstNode(funcDefOp->GetReturnLabel()));
		}
		$$ = createNode(astNode, 
				createPtNodeWithChildren("stmnt", 1, $1->ptNode));
	}
	|
	TOK_RETURN expr
	{
		AstNode *astNode;
		if (!Context.OnFunctionDefinition())
		{
			astNode = new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, UNEXPECTED_RETURN_ERROR, @1);
		}
		else
		{
			auto funcDefOp = dynamic_cast<TFunctionOperator*>(Context.OperatorStackTop());
			if (funcDefOp->GetResultType()->getID() != $2->astNode->GetResultType()->getID())
				astNode = new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, FUNCTION_INVALID_RETURN_ERROR, @2);
			else
				astNode = new OperatorAstNode(OP_RETURN, new LabelAstNode(funcDefOp->GetReturnLabel()), 
							$2->astNode, new VarAstNode(false, funcDefOp->GetReturnValue()));
		}
		$$ = createNode(astNode, 
				createPtNodeWithChildren("stmnt", 2, $1->ptNode, $2->ptNode));
	}
	;

stmnt: 
	expression_statement
	{
		$$ = $1;
	}
	| 
	TOK_IDENTIFIER TOK_DOUBLEDOT
	{
		AstNode *astNode;
		char *labelName = $1->ptNode->text;

		if(Context.IsLabelDeclared(labelName))
		{
			astNode = new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, LABEL_IS_ALREADY_DECLARED, @1);
		}
		else
		{
			TLabel* label = Context.GetLabel(labelName);
			if(label == NULL)
			{
				label = Context.MakeLabel(labelName);
			}
			label->SetDeclaredLine((@1).first_line);
			astNode = new LabelAstNode(label);
		}

		$$ = createNode(astNode, 
				createPtNodeWithChildren("stmnt", 2, $1->ptNode, $2->ptNode));
	}
	|
	if_stmt
	{
		$$ = $1;
	}
	|
	loop_decl
	{
		$$ = $1;
	}
	| 
	switch_stmt
	{
		$$ = $1;
	}
	|
	print_stmt
	{
		$$ = $1;
	}
	|
	read_stmt
	{
		$$ = $1;
	}
	|
	lexemes TOK_ENDEXPR
	{
		$$ = createNode($1->astNode, 
				createPtNodeWithChildren("statement", 2, $1->ptNode, $2->ptNode));
	}
	| 
	lexemes error
	{
		$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, EXPECTED_SEPARATOR, @error), 
				nullptr);
	}
	;

declaration_block
	: declaration_stmt
	{
		$$ = $1;
	}
	| struct_def
	{
		$$ = $1;
	}

declaration_stmt:
	type[decl] TOK_IDENTIFIER[id]
	{
		char *varName = $id->ptNode->text;
		$$ = nullptr;
		if(Context.OnUserTypeDefinition())
		{
			//if(!Context.IsBaseType(varName))
			//{
			//	$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, USER_TYPE_STRUCT_FIELD, @id),
			//			nullptr);
			//}
			//else
			{
				auto userType = dynamic_cast<StructType*>(Context.TopUserType());
				if(userType->IsFieldDefined(std::string(varName)))
				{
					$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, STRUCT_FIELD_REDEFINITION, @id),
							nullptr);
				}
				else
				{
					userType->AddField($decl->astNode->GetResultType()->Clone(), varName);
					$$ = createNode(new DeclIDAstNode($decl->astNode->GetResultType()->Clone()), 
							createPtNodeWithChildren("stmnt", 2, $decl->ptNode, $id->ptNode));
				}
			}
		}
		else
		{
			auto Var = Context.DeclVar(varName, $decl->astNode->GetResultType()->Clone(), @id);
			$$ = createNode(new VarAstNode(false, Var), 
					createPtNodeWithChildren("stmnt", 2, $decl->ptNode, $id->ptNode));
		}
	}
	;

struct_def: struct_head struct_body struct_tail
	{
		$$ = $1;
	}
	|
	struct_head error
	{
		$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, EXPECTED_CLOSE_BRACE, @error), 
				nullptr);
	}
	|
	struct_head struct_body error
	{
		$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, EXPECTED_CLOSE_BRACE, @error), 
				nullptr);
	}
	;
struct_head: struct_type[_struct] TOK_IDENTIFIER
	{
		AstNode *verboseNode = nullptr;
		// ��������� ����������� ���������������� ����� - ������
		if(Context.OnUserTypeDefinition())
			verboseNode = new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, EMBEDDED_USER_TYPE_DEFINITION, @_struct);
		// �������� �� ��������������� 
		else if(Context.IsTypeDefined($2->ptNode->text))
			verboseNode = new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, TYPE_REDEFINITION_ERROR, @2);
		else
		{
			// ��������� ��������� � ����
			auto t = (StructType*)$1;
			t->SetTypeName($2->ptNode->text);
			Context.PushUserType(t);
		}
		// NOTE: $_struct does NOT return Node* variable!
		$$ = createNode(verboseNode, 
				createPtNodeWithChildren("struct", 1, $2->ptNode));
	}
	|
	struct_type[_struct] error
	{
		$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, STRUCT_DECLARATION_ERROR, @error), 
				nullptr);
	}
	;
struct_type: 
	TOK_STRUCT
	{
		// NOTE: No actual Nodes here!
		$$ = (Node*)new StructType();
	}
	| 
	TOK_UNION
	{
		// NOTE: No actual Nodes here!
		$$ = (Node*)new UnionType();
	}
	;
struct_body: TOK_OPENBR declaration_list 
	{
		$$ = NULL;
	}
	;
struct_tail: TOK_CLOSEBR
	{
		// ������������ ��������� �� �����
		if(Context.OnUserTypeDefinition())
		{
			// ���������� � ������� �����
			Context.AddUserTypeToTable(Context.PopUserType());
		}
		$$ = NULL;
	}
	;

print_stmt:
	TOK_PRINT TOK_OPENPAR expr TOK_CLOSEPAR TOK_ENDEXPR
	{
		$$ = createNode(new OperatorAstNode(OP_OUTPUT, $3->astNode), 
				createPtNodeWithChildren("stmnt", 5, $1->ptNode, $2->ptNode, $3->ptNode, $4->ptNode, $5->ptNode)); 
	}
	| 
	TOK_PRINT error
	{
		$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, EXPECTED_OPEN_PARANTHESIS, @error),
				nullptr);
	}
	| 
	TOK_PRINT TOK_OPENPAR expr error
	{
		$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, EXPECTED_CLOSE_PARANTHESIS, @error),
				nullptr);
	}
	| 
	TOK_PRINT TOK_OPENPAR expr TOK_CLOSEPAR error
	{
		$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, EXPECTED_SEPARATOR, @error),
				nullptr);
	}
	;

read_stmt:
	TOK_READ TOK_OPENPAR left_assign_expr TOK_CLOSEPAR TOK_ENDEXPR
	{
		$$ = createNode(new OperatorAstNode(OP_INPUT, $3->astNode),
				createPtNodeWithChildren("stmnt", 5, $1->ptNode, $2->ptNode, $3->ptNode, $4->ptNode, $5->ptNode)); 
	}
	| TOK_READ error
	{
		$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, EXPECTED_OPEN_PARANTHESIS, @error),
				nullptr);
	}
	| TOK_READ TOK_OPENPAR left_assign_expr error
	{
		$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, EXPECTED_CLOSE_PARANTHESIS, @error),
				nullptr);
	}
	| TOK_READ TOK_OPENPAR left_assign_expr TOK_CLOSEPAR error
	{
		$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, EXPECTED_SEPARATOR, @error),
				nullptr);
	}
	;

if_stmt :
	TOK_IF[if] TOK_OPENPAR[open] expr[cond] TOK_CLOSEPAR[close] instruction_body[if_true] %prec IF_WITHOUT_ELSE
	{
		AssertOneOfTypes($cond, @cond, 1, BOOL_TYPE);

		$$ = createNode(new ConditionalAstNode($cond->astNode, $if_true->astNode), 
				createPtNodeWithChildren("stmnt", 5, $if->ptNode, $open->ptNode, $cond->ptNode, $close->ptNode, $if_true->ptNode));
	}
	|
	TOK_IF[if] TOK_OPENPAR[open] expr[cond] TOK_CLOSEPAR[close] instruction_body[if_true] TOK_ELSE[else] instruction_body[if_false]
	{
		AssertOneOfTypes($cond, @cond, 1, BOOL_TYPE);

		$$ = createNode(new ConditionalAstNode($cond->astNode, $if_true->astNode, $if_false->astNode), 
				createPtNodeWithChildren("stmnt", 7, $if->ptNode, $open->ptNode, $cond->ptNode, $close->ptNode,
					$if_true->ptNode, $else->ptNode, $if_false->ptNode));
	}
	|
	TOK_IF[if] TOK_OPENPAR[open] expr[cond] error
	{
		$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, EXPECTED_CLOSE_PARANTHESIS, @error),
				nullptr);
	}
	;

loop_decl: // TODO: make error checks!
	for_decl loop_for_expr instruction_body
	{
		auto condition = dynamic_cast<LoopConditionAstNode *>($2->astNode);
		auto OperatorData = dynamic_cast<TSimpleOperator*>(Context.OperatorStackPop());
		
		$$ = createNode(new LoopAstNode(condition, $3->astNode, OperatorData->GetEntranceLabel(), OperatorData->GetOutLabel()), 
				createPtNodeWithChildren("loop", 3, $1->ptNode, $2->ptNode, $3->ptNode));
		delete OperatorData;
	}
	|
	while_decl[loop] loop_while_expr instruction_body
	{
		auto condition = dynamic_cast<LoopConditionAstNode *>($2->astNode);
		auto OperatorData = dynamic_cast<TSimpleOperator *>(Context.OperatorStackPop());
		
		$$ = createNode(new LoopAstNode(condition, $3->astNode, OperatorData->GetEntranceLabel(), OperatorData->GetOutLabel()), 
				createPtNodeWithChildren("loop", 3, $1->ptNode, $2->ptNode, $3->ptNode));
		delete OperatorData;
	}
	|
	do_while_decl instruction_body TOK_WHILE_DECL[while] loop_while_expr TOK_ENDEXPR[end]
	{
		auto condition = dynamic_cast<LoopConditionAstNode *>($4->astNode);
		auto OperatorData = dynamic_cast<TSimpleOperator *>(Context.OperatorStackPop());
		
		$$ = createNode(new LoopAstNode(condition, $2->astNode, OperatorData->GetEntranceLabel(), OperatorData->GetOutLabel(), true), 
				createPtNodeWithChildren("loop", 5, $1->ptNode, $2->ptNode, $3->ptNode, $4->ptNode, $5->ptNode));
		delete OperatorData;
	}
	;

loop_while_expr:
	TOK_OPENPAR[open] expr[b_expr] TOK_CLOSEPAR[close]
	{
		$$ = createNode(new LoopConditionAstNode(nullptr, $b_expr->astNode, nullptr), 
				createPtNodeWithChildren("loop_statements", 3, $open->ptNode, $b_expr->ptNode, $close->ptNode));
	}
	;
loop_for_expr:
	TOK_OPENPAR[open] expression_statement[init_expr] expression_statement[b_expr] TOK_CLOSEPAR[close]
	{
		$$ = createNode(new LoopConditionAstNode($init_expr->astNode, $b_expr->astNode, nullptr), 
				createPtNodeWithChildren("loop_statements", 4, $open->ptNode, $init_expr->ptNode,
					$b_expr->ptNode, $close->ptNode));
	}
	|
	TOK_OPENPAR[open] expression_statement[init_expr] expression_statement[b_expr] expr_or_assignment[post_expr] TOK_CLOSEPAR[close]
	{
		$$ = createNode(new LoopConditionAstNode($init_expr->astNode, $b_expr->astNode, $post_expr->astNode), 
				createPtNodeWithChildren("loop_statements", 5, $open->ptNode, $init_expr->ptNode,
					$b_expr->ptNode, $post_expr->ptNode, $close->ptNode));
	}
	;

for_decl:
	TOK_FOR_DECL[for_decl]
	{
		TLabel 
			*controlFLowLabel = Context.GenerateNewLabel(),
			*end = Context.GenerateNewLabel();
		auto forOp = new TSimpleOperator(OT_FOR, controlFLowLabel, end);
		Context.OperatorStackPush(forOp);
		$$ = $1;
	};
while_decl:
	TOK_WHILE_DECL[loop]
	{
		TLabel 
			*controlFLowLabel = Context.GenerateNewLabel(),
			*end = Context.GenerateNewLabel();
		auto whileOp = new TSimpleOperator(OT_WHILE, controlFLowLabel, end);
		Context.OperatorStackPush(whileOp);
		$$ = $1;
	};
do_while_decl:
	TOK_DO_DECL[do]
	{
		TLabel 
			*controlFLowLabel = Context.GenerateNewLabel(),
			*end = Context.GenerateNewLabel();
		auto whileOp = new TSimpleOperator(OT_DO_WHILE, controlFLowLabel, end);
		Context.OperatorStackPush(whileOp);
		$$ = $1;
	};

instruction_body:
	stmnt_block
	{
		$$ = $1;
	}
	|
	stmnt
	{
		$$ = $1;
	}
	;

type:
	type_name[t] array[array_decl]
	{
		auto dimNode = dynamic_cast<DimensionAstNode*>($array_decl->astNode);
		if (dimNode->GetExpr() == nullptr)
		{
			$$ = $t;
		}
		else
		{
			$$ = nullptr;

			std::vector<int> sizes;
			for (DimensionAstNode *cur = dimNode; cur != nullptr && cur->GetNextDim() != nullptr; )
			{
				auto numValueNode = dynamic_cast<NumValueAstNode*>(cur->GetExpr());
				if (numValueNode == nullptr && $$ == nullptr)
				{
					$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, INVALID_ARRAY_DECLARATION, @array_decl),
							nullptr);
				}
				sizes.emplace_back(numValueNode->ToInt());
				
				// we will now delete the DimensionAstNode's as we no more need them
				// as we converted the info to a more comfortable vector format
				AstNode *del = cur;
				cur = cur->GetNextDim();
				delete del;
			}	

			if ($$ == nullptr) // IF there were no errors...
			{
				$$ = createNode(new DeclIDAstNode(new ArrayType($t->astNode->GetResultType()->Clone(), sizes)), 
						createPtNodeWithChildren("array decl", 2, $t->ptNode, $array_decl->ptNode));
			}

			// we will also delete the original DeclIDAstNode as we only need type info
			delete $t->astNode;
		}
	}

type_name:
	TOK_ROM_DECL[decl]
	{
        $$ = createNode(new DeclIDAstNode(new RomanType()), 
				createPtNodeWithChildren("type", 1, $decl->ptNode));
	}
	|
	TOK_FLOAT_DECL[decl]
	{
        $$ = createNode(new DeclIDAstNode(new FloatType()), 
				createPtNodeWithChildren("type", 1, $decl->ptNode));
	}
	|
	TOK_INT_DECL[decl]
	{
        $$ = createNode(new DeclIDAstNode(new IntType()), 
				createPtNodeWithChildren("type", 1, $decl->ptNode));
	}
	|
	TOK_VOID_DECL[decl]
	{
        $$ = createNode(new DeclIDAstNode(new VoidType()), 
				createPtNodeWithChildren("type", 1, $decl->ptNode));
	}
	| 
	struct_type[_struct_name] TOK_IDENTIFIER[id]
	{
		// Token for the user-defined type
		
		// NOTE: Deleting the allocated type var as we do not define it here - we use it.	
		delete $_struct_name; 
		
		StructType *typeRef = Context.GetUserType($id->ptNode->text);
		if(typeRef == nullptr)
		{
			$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, UNDECLARED_TYPE, @id),
					nullptr);
		}
		else
		{
			$$ = createNode(new DeclIDAstNode(typeRef), 
					createPtNodeWithChildren("type", 1, $id->ptNode));
		}
	}
	;

left_assign_expr
    :
	identifier[id]
	{
		$$ = $1;
	}
	|
	struct_item
	{
		$$ = $1;
	}
	;

assignment:
	left_assign_expr[left] TOK_ASSIGN_OP[op] expr[right]
	{
		BaseTypeInfo *type = $left->astNode->GetResultType();
		AssertOneOfTypes($right, @right, 1, type->getID());
		
		//TVariable *var = GetVariableForAssign($left, @left);

		$$ = createNode(new OperatorAstNode($op->ptNode->text, $left->astNode, $right->astNode), 
				createPtNodeWithChildren("expr", 3, $left->ptNode, $op->ptNode, $right->ptNode));
	}

expr :
	left_assign_expr
	{
		$$ = $1;
	}
	|
	const
	{
		$$ = $1;
	}
	|
	function_call
	{
		$$ = $1;
	}
	|
	expr[left] TOK_B_AND[op] expr[right]
	{
		$$ = CreateExpressionNode($op, true, $left, $right, @left, @right);
	}
	|
	expr[left] TOK_B_OR[op] expr[right]
	{
		$$ = CreateExpressionNode($op, true, $left, $right, @left, @right);
	}
	|
	expr[left] TOK_B_XOR[op] expr[right]
	{
		$$ = CreateExpressionNode($op, true, $left, $right, @left, @right);
	}
	|
	TOK_B_NOT[op] expr[left] %prec NOTX
	{
		YYLTYPE dummy = {0, 0, 0, 0};
		$$ = CreateExpressionNode($op, true, $left, nullptr, @left, dummy);
	}
	|
	TOK_AR_MINUS[op] expr[left] %prec UMINUS
	{
		YYLTYPE dummy = {0, 0, 0, 0};
		$$ = CreateExpressionNode($op, false, $left, nullptr, @left, dummy);
	}
	|
	expr[left] TOK_AR_PLUS[op] expr[right]
	{
		$$ = CreateExpressionNode($op, false, $left, $right, @left, @right);
	}
	|
	expr[left] TOK_AR_MINUS[op] expr[right]
	{
		$$ = CreateExpressionNode($op, false, $left, $right, @left, @right);
	}
	|
	expr[left] TOK_AR_MUL[op] expr[right]
	{
		$$ = CreateExpressionNode($op, false, $left, $right, @left, @right);
	}
	|
	expr[left] TOK_AR_DIV[op] expr[right]
	{
		$$ = CreateExpressionNode($op, false, $left, $right, @left, @right);
	}
	|
	expr[left] TOK_AR_MOD[op] expr[right]
	{
		$$ = CreateExpressionNode($op, false, $left, $right, @left, @right);
	}
	|
	expr[left] TOK_COMP_OP[op] expr[right]
	{
		$$ = CreateExpressionNode($op, false, $left, $right, @left, @right);
	}
	|
	TOK_OPENPAR expr[val] error
	{
		$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, EXPECTED_CLOSE_PARANTHESIS, @error),
				nullptr);
	}
	|
	TOK_OPENPAR[open] expr[val] TOK_CLOSEPAR[close]
	{
		$$ = createNode($val->astNode, 
				createPtNodeWithChildren("expr", 3, $open->ptNode, $val->ptNode, $close->ptNode));
	}
	 /*|
      error %prec EXPR_ERROR
      {
        print_error("Invalid expression", @$);
      } */
    ;

array:
	/* epsilon-������� */
    {
		$$ = createNode(new DimensionAstNode(nullptr, nullptr, nullptr), 
				createPtNode("array_end"));
	}
	|
	TOK_OPENSQ[open] expr[val] TOK_CLOSESQ[close] array[decl]
	{
		AstNode *astNode = new DimensionAstNode(
			$val->astNode->GetResultType(), 
			$val->astNode,
			dynamic_cast<DimensionAstNode*>($decl->astNode));
		PtNode *ptNode = createPtNodeWithChildren("array_dimension", 4, $decl->ptNode, $open->ptNode, $val->ptNode, $close->ptNode);
        
		$$ = createNode(astNode, ptNode);
	}
	|
	TOK_OPENSQ[open] error
	{
		$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, INVALID_ARRAY_ITEM, @error),
				nullptr);
	}
	|
	TOK_OPENSQ[open] expr[val] error
	{
		$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, EXPECTED_CLOSE_BRACKET, @error),
				nullptr);
	}
	;

struct_item: identifier[id] TOK_DOT TOK_IDENTIFIER[name]
	{
		AssertOneOfTypes($id, @id, 2, STRUCT_TYPE, UNION_TYPE);
		//if(FALSE == CheckNodeTypeByTypeId($1, TYPE_STRUCT) 
		//	&& FALSE == CheckNodeTypeByTypeId($1, TYPE_UNION))
		//{
		//	yyerror(INVALID_STRUCT_FIELD_L_VALUE);
		//	$$ = ErrorNode();
		//}
		//else 

		auto astNode = dynamic_cast<VarAstNode*>($id->astNode);
		if (astNode == nullptr)
		{
			$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, UNDECLARED_STRUCT_FIELD, @name),
					nullptr);
		}
		else
		{
			StructType *varType = dynamic_cast<StructType*>(astNode->GetResultType());
			std::string fieldName($3->ptNode->text);

			if(!varType->IsFieldDefined(fieldName))
			{
				$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, UNDECLARED_STRUCT_FIELD, @name),
						nullptr);
			}
			else
			{
				StructType *s = varType;
				TVariable *fieldVar = s->GetField(fieldName);
				//AstNode *fieldNode = new VariableAstNode(false, field);

				$$ = createNode(new StructAddressAstNode(astNode, fieldVar),
						createPtNodeWithChildren("identifier", 3, $1->ptNode, $2->ptNode, $3->ptNode));
			}
		}
	}
	;

identifier:
	TOK_IDENTIFIER[id] array[ar_decl]
	{
		char *id_name = $id->ptNode->text;
		auto dimNode = dynamic_cast<DimensionAstNode*>($ar_decl->astNode);
		TVariable *val = Context.getVar(id_name, 1, dimNode, @id);
		AstNode *astNode;
		if (val)
		{
			astNode = new VarAstNode(false, val);
			// TODO [SV] 15.08.13 12:18: possible checks for array\non-array type equality
			if (dimNode->GetExpr() != nullptr)
				astNode = new ArrayAddressAstNode(static_cast<VarAstNode*>(astNode), dimNode);				
		}
		else
		{
			astNode = new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, UNDECLARED_VARIABLE_ERROR, @id);
		}

		$$ = createNode(astNode, 
				createPtNodeWithChildren("identifier", 1, $id->ptNode));
	}
	;

const:
	TOK_INT_CONST[val]
	{
		char *id = strdup($val->ptNode->text);

		$$ = createNode(new NumValueAstNode(id, new IntType()), 
				createPtNodeWithChildren("const", 1, $val->ptNode));
	}
	|
	TOK_ROM_CONST[val]
	{
		char *id = strdup($val->ptNode->text);

		$$ = createNode(new NumValueAstNode(id, new RomanType()), 
				createPtNodeWithChildren("const", 1, $val->ptNode));
	}
	|
	TOK_FLOAT_CONST[val]
	{
		char *id = strdup($val->ptNode->text);

		$$ = createNode(new NumValueAstNode(id, new FloatType()), 
				createPtNodeWithChildren("const", 1, $val->ptNode));
	}
	|
	TOK_STRING_LITERAL[val]
	{
		char *id = strdup($val->ptNode->text);

		$$ = createNode(new NumValueAstNode($val->ptNode->text, new LiteralType(strlen(id))), 
				createPtNodeWithChildren("const", 1, $val->ptNode));
	}
	;

switch_stmt: switch_head case_list default TOK_CLOSEBR
	{
		auto switchOp = dynamic_cast<TSwitchOperator*>(Context.OperatorStackPop());

		$$ = createNode(new SwitchAstNode($1->astNode, $2->astNode, $3->astNode, switchOp),
				createPtNodeWithChildren("switch_stmt", 4, $1->ptNode, $2->ptNode, $3->ptNode, $4->ptNode));
	}
	| switch_head case_list TOK_CLOSEBR
	{
		auto switchOp = dynamic_cast<TSwitchOperator*>(Context.OperatorStackPop());
		
		$$ = createNode(new SwitchAstNode($1->astNode, $2->astNode, nullptr, switchOp),
				createPtNodeWithChildren("switch_stmt", 3, $1->ptNode, $2->ptNode, $3->ptNode));
	}
	;
switch_head: TOK_SWITCH TOK_OPENPAR expr TOK_CLOSEPAR TOK_OPENBR	/*<s1>*/
	{
		//AstNode *astNode;
		AssertOneOfTypes($3, @3, 1, INT_TYPE);

		if($3->astNode->GetResultType()->getID() != INT_TYPE)
		{
			Context.OperatorStackPush(new TSwitchOperator(nullptr, 0, 0));

			$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, INVALID_SWITCH_KEY_TYPE, @3),
					nullptr);
		}
		else
		{
			TLabel 
				*controlFLowLabel = Context.GenerateNewLabel(),
				*end = Context.GenerateNewLabel();
			TSwitchOperator *switchOp = new TSwitchOperator($3->astNode, controlFLowLabel, end);
			Context.OperatorStackPush(switchOp);
			$$ = createNode($3->astNode,
					createPtNodeWithChildren("switch_head", 4, $1->ptNode, $2->ptNode, $3->ptNode, $4->ptNode));
		}
	}
	;

case_list: case_stmt 
	{
		$$ = $1;
	}
	| case_stmt case_list
	{
		$$ = createNode(new OperatorAstNode(OP_LIST, $1->astNode, $2->astNode),
				createPtNodeWithChildren("case_list", 2, $1->ptNode, $2->ptNode));
	}
	;
case_stmt: case_head case_body
	{
		$$ = createNode(new OperatorAstNode(OP_CASE, $1->astNode, $2->astNode),
				createPtNodeWithChildren("case_stmt", 2, $1->ptNode, $2->ptNode));
	}
	;
case_head: TOK_CASE expr TOK_DOUBLEDOT	/* <s3> */
	{
		//TODO: �������� �������� �����! ��� �� ����� �����������
		AssertOneOfTypes($2, @2, 1, INT_TYPE);
		
		if(Context.IsRepeatedCaseKeyVal($2->astNode))
		{
			$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, REPEATED_CASE_KEY_VALUE, @2),
					nullptr);
		}
		else
		{
			TSwitchOperator *switchOp = dynamic_cast<TSwitchOperator *>(Context.OperatorStackTop());
			
			TLabel *label = Context.GenerateNewLabel();
			TCaseOperator *caseOp = new TCaseOperator($2->astNode, label);
			switchOp->AddCase(caseOp);
			$$ = createNode(new LabelAstNode(label),
					createPtNodeWithChildren("case_head", 3, $1->ptNode, $2->ptNode, $3->ptNode));
		}	
	}
	;
case_body: 
	instruction_body
	{
		$$ = $1;
	}
	| case_stmt
	{
		$$ = $1;
	}
	;
default: default_head instruction_body
	{
		$$ = createNode(new OperatorAstNode(OP_DEFAULT, $1->astNode, $2->astNode),
				createPtNodeWithChildren("default", 2, $1->ptNode, $2->ptNode));
	}
	;
default_head: TOK_DEFAULT TOK_DOUBLEDOT	/* <s4> */
	{
		TSwitchOperator *switchOp = dynamic_cast<TSwitchOperator *>(Context.OperatorStackTop());
		TLabel *label = Context.GenerateNewLabel();
		TDefaultOperator *defOp = new TDefaultOperator(label);
		switchOp->AddDefaultOp(defOp);

		$$ = createNode(new LabelAstNode(label),
				createPtNodeWithChildren("default_head", 2, $1->ptNode, $2->ptNode));
	}
	;

parameter
	: declaration_stmt
	{
		// function declaration
		$$ = $1;
	}
	| expr
	{
		// function call
		$$ = $1;
	}


parameter_list
	: parameter
	{
		$$ = $1;
	}
	| parameter_list TOK_COMMA parameter
	{
		$$ = addStmntToBlock($1, $3);
	}
	;

parameter_type_list
	: parameter_list
	{
		$$ = $1;
	}
	;
	//| parameter_list TOK_COMMA ELLIPSIS

declarator
	: TOK_OPENPAR parameter_type_list TOK_CLOSEPAR
	{
		$$ = createNode($2->astNode,
				createPtNodeWithChildren("declarator", 3, $1->ptNode, $2->ptNode, $3->ptNode));
	}
	| TOK_OPENPAR TOK_CLOSEPAR
	{
		$$ = createNode(nullptr,
				createPtNodeWithChildren("declarator", 2, $1->ptNode, $2->ptNode));
	}
	;

function_call:
	TOK_IDENTIFIER declarator
	{
		std::vector<AstNode*> callParameters; 
		if ($2->astNode != nullptr)
		{
			auto paramsList = GetParametersList($2->astNode);
			for(auto it = paramsList.begin(); it != paramsList.end(); it++)
			{
				callParameters.emplace_back(*it);
			}
		}

		std::string funcname($1->ptNode->text);

		if (!Context.IsFunctionDefined(funcname))
		{
			$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, UNDECLARED_FUNCTION_ERROR, @2),
					nullptr);
		}
		else
		{
			auto function = Context.GetFunction(funcname);
			auto funcParameters = function->GetParametersList();

			$$ = nullptr;

			if (callParameters.size() != funcParameters.size())
			{
				$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, FUNCTION_CALL_PAR_NUMBER_ERROR, @2),
						nullptr);
			}
			else
			{
				auto it_f = funcParameters.begin();
				auto it_c = callParameters.begin();

				for (; it_f != funcParameters.end(); it_f++, it_c++)
				{
					if ((*it_f)->GetType()->getID() != (*it_c)->GetResultType()->getID())
					{
						$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, FUNCTION_INVALID_PAR_ERROR, @2),
								nullptr);
						break;
					}
				}

				if ($$ == nullptr) // no errors
				{
					$$ = createNode(new FunctionCallAstNode(function, callParameters),
							createPtNodeWithChildren("function_call", 2, $1->ptNode, $2->ptNode));
					Context.AddFunction(function);
				}
			}
		}
	}
	;

function_def_head
	: type TOK_IDENTIFIER
	{
		TLabel *callLabel = Context.GenerateNewLabel();
		auto funcName = std::string($2->ptNode->text);

		TBlockContext::Push_FunctionParametersDef(funcName);
		auto nameSpace = TBlockContext::GetCurrent()->GetBlockNamepace();

		TVariable *resultVar = nullptr;
		if ($1->astNode->GetResultType()->getID() != VOID_TYPE)
			resultVar = Context.GenerateNewTmpVar($1->astNode->GetResultType()->Clone());

		auto functionOp = new TFunctionOperator(resultVar, funcName, nameSpace, callLabel, Context.GenerateNewLabel());
		Context.OperatorStackPush(functionOp);
	}
	;

func_declarator
	: declarator
	{
		std::vector<TVariable*> parameters;
		$$ = nullptr;
		if ($1->astNode != nullptr)
		{
			auto paramsList = GetParametersList($1->astNode);
			for(auto it = paramsList.begin(); it != paramsList.end(); it++)
			{
				auto varNode = dynamic_cast<VarAstNode*>(*it);
				if (varNode == nullptr)
				{
					$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, FUNCTION_INVALID_PAR_ERROR, @1),
								nullptr);
					break;
				}
				parameters.emplace_back(varNode->GetTableReference());
				delete (*it); // we don't need it anymore
			}
		}
		auto funcDefOp = dynamic_cast<TFunctionOperator*>(Context.OperatorStackTop());
		funcDefOp->SetParametersList(parameters);
		
		if ($$ == nullptr) // no errors
		{
			$$ = $1;
		}
	}
	;

function_def
	: function_def_head func_declarator stmnt_block
	{
		auto funcDefOp = dynamic_cast<TFunctionOperator*>(Context.OperatorStackPop());
		auto funcname = funcDefOp->GetName();

		if (Context.IsFunctionDefined(funcname))
		{
			$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, FUNCTION_REDEFINITION_ERROR, @1),
				nullptr);
			delete funcDefOp;
		}
		else
		{
			Context.AddFunction(funcDefOp);
			$$ = createNode(new FunctionAstNode(funcDefOp, $3->astNode),
					createPtNodeWithChildren("function_def", 3, $1->ptNode, $2->ptNode, $3->ptNode));
		}
	}
	/*|
	function_def_head error // shift/reduce conflist with ID declaration error
	{
		$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, INCOMPLETE_FUNC_DEF_ERROR, @error),
				nullptr);
	}*/
	|
	function_def_head func_declarator error
	{
		$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, INCOMPLETE_FUNC_DEF_ERROR, @error),
				nullptr);
	}
	;

%%

void yyerror(const char *message)
{
}

%require "2.5"
%defines
%locations
%verbose
%debug

%code top
{
#include <stddef.h>
#include <string.h>
#include "parser.h"
#include "AstUtils.h"
#include "resources.h"
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
%token <_node> TOK_COMMA
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
%token <_node> TOK_DOUBLEDOT

%token <_node> TOK_ROM_DECL
%token <_node> TOK_INT_DECL
%token <_node> TOK_FLOAT_DECL

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
%token <_node> TOK_STRUCT
%token <_node> TOK_UNION
%token <_node> TOK_SWITCH
%token <_node> TOK_CASE
%token <_node> TOK_DEFAULT

//duplicate 3AC definitions here as this header is included first
%token <_node> TMP_VAR LABEL NAME
%token <_node> INT_CONST 
%token <_node> FLOAT_CONST 
%token <_node> LITERAL
%token <_node> IFFALSE 
%token <_node> IFTRUE
%token <_node> ASSIGN 
%token <_node> OR 
%token <_node> AND 
%token <_node> XOR 
%token <_node> NOT 
%token <_node> INPUT 
%token <_node> OUTPUT
%token <_node> GOTO
%token <_node> CMP
//%type <_AstNode> start3ac program stmt stmt_list expression factor assign label go_to array_item _struct_item


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

%type <_node> start declaration_list stmnt stmnt_list stmnt_block_start stmnt_block 
%type <_node> expression_statement expr_or_assignment expr struct_item identifier declaration_stmt struct_type
%type <_node> if_stmt loop_decl switch_stmt print_stmt read_stmt assignment struct_def struct_head struct_body struct_tail
%type <_node> loop_for_expr instruction_body loop_while_expr type array type_name left_assign_expr num_const
%type <_node> switch_head case_list default switch_tail case_stmt case_head case_body
%type <_node> default_head for_decl while_decl do_while_decl

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
%}


%%
start :  /* empty */
	{ 
		$$ = NULL; 
	}
	| 
	declaration_list statement_list
	{
		PtNode *ptNode = createPtNode("start");
		setPtNodeChildren(ptNode, 1, $stmnt_list->ptNode);
		astTree = $stmnt_list->astNode;
		ptTree = ptNode;
	}
	|
	declaration_list
	{
		$$ = NULL;
	}
	|
	statement_list
	{
		PtNode *ptNode = createPtNode("start");
		setPtNodeChildren(ptNode, 1, $stmnt_list->ptNode);
		astTree = $stmnt_list->astNode;
		ptTree = ptNode;
	}
	;

declaration_list: 
	declaration
	{
		$$ = NULL;
	}
	| declaration_list declaration
	{
		$$ = NULL;
	}
	;

statement_list: statement
	{
		$$ = $1;
	}
	| statement_list statement
	{
		$$ = addStmntToBlock($1, $2);
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
		$$ = $1;
	}
	|
	expr error
	{
		$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, EXPECTED_SEPARATOR, @error), 
				nullptr);
	}
	;

jump_statement
	: TOK_GOTO TOK_IDENTIFIER
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
	| TOK_CONTINUE
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
	| TOK_BREAK
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
	| TOK_RETURN
	| TOK_RETURN expression
	;

labeled_statement
	: TOK_IDENTIFIER TOK_DOUBLEDOT statement
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
	| TOK_CASE num_const TOK_DOUBLEDOT statement
	{
		// TODO: check correct usage of the case operator! (outside of a switch)
		
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
			TCaseOperator *caseOp = new TCaseOperator($2->astNode, label, switchOp);
			switchOp->AddCase(caseOp);
		}

		$$ = createNode(new OperatorAstNode(OP_CASE, $2->astNode, $4->astNode),
				createPtNodeWithChildren("case_stmt", 4, $1->ptNode, $2->ptNode, $3->ptNode, $4->ptNode));
	}
	| TOK_DEFAULT TOK_DOUBLEDOT statement
	{
		TSwitchOperator *switchOp = dynamic_cast<TSwitchOperator *>(Context.OperatorStackTop());
		TLabel *label = Context.GenerateNewLabel();
		TDefaultOperator *defOp = new TDefaultOperator(label);
		switchOp->AddDefaultOp(defOp);

		$$ = createNode(new OperatorAstNode(OT_DEFAULT, $3->astNode),
				createPtNodeWithChildren("default", 2, $1->ptNode, $2->ptNode));
	}
	;


compound_statement_start
    : TOK_OPENBR[st] {TBlockContext::Push();}
	{
		$$ = $st;
	}
    ;

compound_statement
	: compound_statement_start TOK_CLOSEBR[end] {TBlockContext::Pop();}
	{
		$$ = createNode(nullptr, 
				createptNodeWithChildren($1->ptNode, $2->ptNode));
	}
	| compound_statement_start block_item_list TOK_CLOSEBR[end] {TBlockContext::Pop();}
	{
		$$ = createNode($stmnts->astNode, 
			createPtNodeWithChildren("stmnt_block", 3, $1->ptNode, $2->ptNode, $2->ptNode));
	}
	;

block_item_list
	: block_item
	{
		$$ = $1;
	}
	| block_item_list block_item
	{
		$$ = addStmntToBlock($1, $2);
	}
	;

block_item
	: declaration
	{
		$$ = $1;
	}
	| statement
	{
		$$ = $1;
	}
	;

statement: 
	labeled_statement
	{
		$$ = $1;
	}
	| compound_statement
	{
		$$ = $1;
	}
	| selection_statement
	{
		$$ = $1;
	}
	| iteration_statement
	{
		$$ = $1;
	}
	| expression_statement
	{
		$$ = $1;
	}
	| jump_statement TOK_ENDEXPR
	{
		$$ = createNode($1->astNode, 
				createPtNodeWithChildren("stmnt", 2, $1->ptNode, $2->ptNode));
	}
	| print_stmt
	{
		$$ = $1;
	}
	| read_stmt
	{
		$$ = $1;
	}
	| jump_statement error
	{
		$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, EXPECTED_SEPARATOR, @error), 
				nullptr);
	}
	;

declaration
	: type_specifier[type] init_declarator_list[decls] TOK_ENDEXPR[end]
	{
		char *typeName = $id->ptNode->text;
		$$ = nullptr;
		if(Context.OnUserTypeDefinition())
		{
			//if(!Context.IsBaseType(typeName))
			//{
			//	$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, USER_TYPE_STRUCT_FIELD, @id),
			//			nullptr);
			//}
			//else
			{
				auto userType = dynamic_cast<StructType*>(Context.TopUserType());
				if(userType->IsFieldDefined(std::string(typeName)))
				{
					$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, STRUCT_FIELD_REDEFINITION, @id),
							nullptr);
				}
				else
				{
					userType->AddField($decl->astNode->GetResultType()->Clone(), typeName);
				}
			}
		}
		else
		{
			Context.DeclVar(typeName, $decl->astNode->GetResultType()->Clone(), @id);
		}
		
		if ($$ == nullptr)
		{
			$$ = createNode(new DeclIDAstNode($decl->astNode->GetResultType()->Clone()), 
					createPtNodeWithChildren("stmnt", 2, $decl->ptNode, $id->ptNode));
		}
	}
	;

type_specifier
	: TOK_ROM_DECL[decl]
	{
        $$ = createNode(new DeclIDAstNode(new RomanType()), 
				createPtNodeWithChildren("type", 1, $decl->ptNode));
	}
	| TOK_FLOAT_DECL[decl]
	{
        $$ = createNode(new DeclIDAstNode(new FloatType()), 
				createPtNodeWithChildren("type", 1, $decl->ptNode));
	}
	| TOK_INT_DECL[decl]
	{
        $$ = createNode(new DeclIDAstNode(new IntType()), 
				createPtNodeWithChildren("type", 1, $decl->ptNode));
	}
	| struct_def
	{
		$$ = $1;
	}
	| type_specifier TOK_OPENSQ[open] expr_or_assignment[val] TOK_CLOSESQ[close]
	{
		DimensionAstNode *dimNode = dynamic_cast<DimensionAstNode*>($type_specifier->astNode);
		if (dimNode != nullptr)
		{
			AstNode *astNode = new DimensionAstNode(
				$val->astNode->GetResultType(), 
				$val->astNode,
				dimNode);
			PtNode *ptNode = createPtNodeWithChildren("array_dimension", 4, $type_specifier->ptNode, $open->ptNode, $val->ptNode, $close->ptNode);
        
			$$ = createNode(astNode, ptNode);
		}
		else
		{
			$$ = nullptr;

			std::vector<int> sizes;
			for (DimensionAstNode *cur = dimNode; cur != nullptr; )
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
				$$ = createNode(new DeclIDAstNode(new ArrayType($type_specifier->astNode->GetResultType()->Clone(), sizes)), 
						createPtNodeWithChildren("array decl", 2, $type_specifier->ptNode, $array_decl->ptNode));
			}

			// we will also delete the original DeclIDAstNode as we only need type info
			delete $type_specifier->astNode;
		}
	}
	| type_specifier TOK_OPENSQ[open] error
	{
		$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, INVALID_ARRAY_ITEM, @error),
				nullptr);
	}
	| type_specifier TOK_OPENSQ[open] expr[val] error
	{
		$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, EXPECTED_CLOSE_BRACKET, @error),
				nullptr);
	}
	;

init_declarator_list
	: init_declarator
	{
		$$ = $1;
	}
	| init_declarator_list TOK_COMMA init_declarator
	{
		$$ = addStmntToBlock($1, $3);
	}
	;

init_declarator
	: expr_or_assignment
	{
		$$ = $1;
	}
	| TOK_IDENTIFIER
	{
		$$ = $1;
	}
	;

struct_or_union
	: TOK_STRUCT
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

struct_def: struct_head struct_body struct_tail
	{
		$$ = $1;
	}
	;
struct_head: struct_or_union TOK_IDENTIFIER
	{
		AstNode *verboseNode = nullptr;
		// Вложенное определение пользовательских типов - ошибка
		if(Context.OnUserTypeDefinition())
			verboseNode = new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, EMBEDDED_USER_TYPE_DEFINITION, @1);
		// Проверка на переопределение 
		else if(Context.IsTypeDefined($2->ptNode->text))
			verboseNode = new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, TYPE_REDEFINITION_ERROR, @2);
		else
		{
			// Помещение структуры в стек
			auto t = (StructType*)$1;
			t->SetTypeName($2->ptNode->text);
			Context.PushUserType(t);
		}
		// NOTE: $_struct does NOT return Node* variable!
		$$ = createNode(verboseNode, 
				createPtNodeWithChildren("struct", 1, $2->ptNode));
	}
	;

struct_body: TOK_OPENBR declaration_list 
	{
		$$ = NULL;
	}
	;
struct_tail: TOK_CLOSEBR
	{
		// Выталкивание структуры из стека
		if(Context.OnUserTypeDefinition())
		{
			// Добавление в таблицу типов
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
	TOK_PRINT '(' expr error
	{
		$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, EXPECTED_CLOSE_PARANTHESIS, @error),
				nullptr);
	}
	| 
	TOK_PRINT '(' expr ')' error
	{
		$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, EXPECTED_SEPARATOR, @error),
				nullptr);
	}
	;

read_stmt:
	TOK_READ TOK_OPENPAR left_assign_expr TOK_CLOSEPAR TOK_ENDEXPR
	{
		$$ = createNode(new OperatorAstNode(OP_INPUT, $2->astNode),
				createPtNodeWithChildren("stmnt", 5, $1->ptNode, $2->ptNode, $3->ptNode, $4->ptNode, $5->ptNode)); 
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

iteration_statement: // TODO: make error checks!
	for_decl loop_for_expr statement
	{
		auto condition = dynamic_cast<LoopConditionAstNode *>($2->astNode);
		auto OperatorData = dynamic_cast<TSimpleOperator*>(Context.OperatorStackPop());
		
		$$ = createNode(new LoopAstNode(condition, $3->astNode, OperatorData->GetEntranceLabel(), OperatorData->GetOutLabel()), 
				createPtNodeWithChildren("loop", 3, $1->ptNode, $2->ptNode, $3->ptNode));
		delete OperatorData;
	}
	|
	while_decl[loop] loop_while_expr statement
	{
		auto condition = dynamic_cast<LoopConditionAstNode *>($2->astNode);
		auto OperatorData = dynamic_cast<TSimpleOperator *>(Context.OperatorStackPop());
		
		$$ = createNode(new LoopAstNode(condition, $3->astNode, OperatorData->GetEntranceLabel(), OperatorData->GetOutLabel()), 
				createPtNodeWithChildren("loop", 3, $1->ptNode, $2->ptNode, $3->ptNode));
		delete OperatorData;
	}
	|
	do_while_decl statement TOK_WHILE_DECL[while] loop_while_expr TOK_ENDEXPR[end]
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


/*
type_name:
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
*/

left_assign_expr
    :
	identifier[id]
	{
		TVariable *var = Context.getVar($id->ptNode->firstChild->text, 1, NULL, @id);
		var->SetInitialized(true);
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

		$$ = createNode(new OperatorAstNode($op->ptNode->text, $left->astNode, $right->astNode), 
				createPtNodeWithChildren("expr", 3, $left->ptNode, $op->ptNode, $right->ptNode));
	}

expr :
	left_assign_expr
	{
		$$ = $1;
	}
	|
	num_const[num]
	{
		$$ = $num;
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
			print_error("unknown identifier", @id); // I guess this would be better
			//astNode = new VarAstNode(id_name, VOID_TYPE, false, false, $ar_decl->astNode);
		}

		$$ = createNode(astNode, 
				createPtNodeWithChildren("identifier", 1, $id->ptNode));
	}
	;

num_const:
	TOK_INT_CONST[val]
	{
		char *id = strdup($val->ptNode->text);

		$$ = createNode(new NumValueAstNode($val->ptNode->text, new IntType()), 
				createPtNodeWithChildren("num_const", 1, $val->ptNode));
	}
	|
	TOK_ROM_CONST[val]
	{
		char *id = strdup($val->ptNode->text);

		$$ = createNode(new NumValueAstNode($val->ptNode->text, new RomanType()), 
				createPtNodeWithChildren("num_const", 1, $val->ptNode));
	}
	|
	TOK_FLOAT_CONST[val]
	{
		char *id = strdup($val->ptNode->text);

		$$ = createNode(new NumValueAstNode($val->ptNode->text, new FloatType()), 
				createPtNodeWithChildren("num_const", 1, $val->ptNode));
	}
	;

switch_stmt: switch_head statement switch_tail
	{
		$$ = createNode(new SwitchAstNode($1->astNode, $2->astNode, $3->astNode),
				createPtNodeWithChildren("switch_stmt", 3, $1->ptNode, $2->ptNode, $3->ptNode, $4->ptNode));
	}
	;

switch_head: TOK_SWITCH TOK_OPENPAR expression TOK_CLOSEPAR
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
			$$ = $3;
		}
	}
	;
switch_tail: TOK_CLOSEBR	/* <s2> */
	{
		delete Context.OperatorStackPop();
		// TODO: On-switch-end operations
		$$ = createNode(nullptr,
				createPtNodeWithChildren("switch_tail", 1, $1->ptNode));
	}
	;

%%

void yyerror(const char *message)
{
}

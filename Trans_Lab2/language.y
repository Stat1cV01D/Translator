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

%token <_node> TOK_ARITHMETICAL_OP
%token <_node> TOK_COMP_OP
%token <_node> TOK_ASSIGN_OP
%token <_node> TOK_U_ARITHMETICAL_OP
%token <_node> TOK_B_LOGICAL_OP
%token <_node> TOK_U_LOGICAL_OP
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

%left TOK_COMP_OP TOK_ARITHMETICAL_OP
%right TOK_U_ARITHMETICAL TOK_U_LOGICAL
%right TOK_ASSIGN_OP

%type <_node> start declaration_list stmnt stmnt_list stmnt_block_start stmnt_block 
%type <_node> expr struct_item identifier declaration_stmt struct_type
%type <_node> if_stmt loop_decl switch_stmt print_stmt read_stmt assignment struct_def struct_head struct_body struct_tail
%type <_node> loop_for_expr instruction_body loop_while_expr type array type_name left_assign_expr num_const
%type <_node> switch_head case_list default switch_tail case_stmt case_head case_body
%type <_node> default_head default_body 

%code top
{
AstNode *astTree;
PtNode *ptTree;
ParserContext Context;
}

%%
start :  /* empty */
	{ 
		$$ = NULL; 
	}
	| 
	declaration_list stmnt_list
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
	stmnt_list
	{
		PtNode *ptNode = createPtNode("start");
		setPtNodeChildren(ptNode, 1, $stmnt_list->ptNode);
		astTree = $stmnt_list->astNode;
		ptTree = ptNode;
	}
	;

declaration_list: 
	declaration_stmt TOK_ENDEXPR[end]
	{
		$$ = NULL;
	}
	| declaration_list declaration_stmt TOK_ENDEXPR[end] 
	{
		$$ = NULL;
	}
	| declaration_stmt error
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
		$$ = addStmntToBlock($2, $1);
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
	;

stmnt
	: 
	expr TOK_ENDEXPR[end]
	{
		$$ = createNode(new StatementAstNode($expr->astNode), 
				createPtNodeWithChildren("stmnt", 2, $expr->ptNode, $end->ptNode));
	}
	| 
	expr error
	{
		$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, EXPECTED_SEPARATOR, @error), 
				nullptr);
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
	TOK_GOTO TOK_IDENTIFIER TOK_ENDEXPR
	{
		char *labelName = $2->ptNode->text;
		TLabel* label =  Context.GetLabel(labelName);
		
		if(label == NULL)
		{
			label = Context.MakeLabel(labelName);
		}
		label->SetUsedLine((@1).first_line);

		$$ = createNode(new OperatorAstNode(OP_GOTO, new LabelAstNode(label)), 
				createPtNodeWithChildren("stmnt", 3, $1->ptNode, $2->ptNode, $3->ptNode));
	}
	|
	if_stmt
	|
	loop_decl
	| 
	switch_stmt
	|
	print_stmt
	|
	read_stmt
	|
	assignment TOK_ENDEXPR
	| 
	TOK_BREAK TOK_ENDEXPR
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
				createPtNodeWithChildren("stmnt", 2, $1->ptNode, $2->ptNode));
	}
	| 
	TOK_BREAK error
	{
		$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, EXPECTED_SEPARATOR, @error), 
				nullptr);
	}
	| 
	TOK_CONTINUE TOK_ENDEXPR
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
				createPtNodeWithChildren("stmnt", 2, $1->ptNode, $2->ptNode));
	}
	| 
	TOK_CONTINUE error
	{
		$$ = createNode(new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, EXPECTED_SEPARATOR, @error), 
				nullptr);
	}
	;

declaration_stmt:
	type[decl] TOK_IDENTIFIER[id]
	{
		Context.DeclVar($id->ptNode->text, $decl->astNode->GetResultType(), @id);

		$$ = createNode(new DeclIDAstNode($decl->astNode->GetResultType()), 
				createPtNodeWithChildren("stmnt", 2, $decl->ptNode, $id->ptNode));
	}
	| 
	struct_def
	;

struct_def: struct_head struct_body struct_tail
	{
		$$ = $1;
	}
	;
struct_head: struct_type[_struct] TOK_IDENTIFIER
	{
		AstNode *verboseNode = nullptr;
		// Вложенное определение пользовательских типов - ошибка
		if(Context.OnUserTypeDefinition())
			verboseNode = new VerboseAstNode(VerboseAstNode::LEVEL_ERROR, EMBEDDED_USER_TYPE_DEFINITION, @_struct);
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

loop_decl: // TODO: make error checks!
	TOK_FOR_DECL[for_decl] loop_for_expr instruction_body
	{
		LoopConditionAstNode *condition = dynamic_cast<LoopConditionAstNode *>($2->astNode);

		$$ = createNode(new LoopAstNode(condition, $3->astNode), 
				createPtNodeWithChildren("loop", 3, $for_decl->ptNode, $2->ptNode, $3->ptNode));
	}
	|
	TOK_WHILE_DECL[loop] loop_while_expr instruction_body
	{
		LoopConditionAstNode *condition = dynamic_cast<LoopConditionAstNode *>($2->astNode);

		$$ = createNode(new LoopAstNode(condition, $3->astNode), 
				createPtNodeWithChildren("loop", 3, $loop->ptNode, $2->ptNode, $3->ptNode));
	}
	|
	TOK_DO_DECL[do] instruction_body TOK_WHILE_DECL[while]
		loop_while_expr TOK_ENDEXPR[end]
	{
		LoopConditionAstNode *condition = dynamic_cast<LoopConditionAstNode *>($4->astNode);

		$$ = createNode(new LoopAstNode(condition, $2->astNode, true), 
				createPtNodeWithChildren("loop", 5, $do->ptNode, $instruction_body->ptNode, $while->ptNode, $loop_while_expr->ptNode, $end->ptNode));
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
	TOK_OPENPAR[open] expr[init_expr] TOK_ENDEXPR[end1] expr[b_expr] TOK_ENDEXPR[end2] expr[post_expr] TOK_CLOSEPAR[close]
	{
		$$ = createNode(new LoopConditionAstNode($init_expr->astNode, $b_expr->astNode, $post_expr->astNode), 
				createPtNodeWithChildren("loop_statements", 7, $open->ptNode, $init_expr->ptNode,
					$end1->ptNode, $b_expr->ptNode, $end2->ptNode, $post_expr->ptNode, $close->ptNode));
	}
	;

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
		DimensionAstNode *dimNode = dynamic_cast<DimensionAstNode*>($array_decl->astNode);
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
	TOK_IDENTIFIER[id]
	{
		// Token for the user-defined type

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
		TVariable *var = Context.getVar($id->ptNode->firstChild->text, 1, NULL, @id);
		var->SetInitialized(true);
		$$ = $1;
	}
	|
	struct_item
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
	identifier
	|
	num_const[num]
	{
		$$ = $num;
	}
	|
	expr[left] TOK_B_LOGICAL_OP[op] expr[right]
	{
		AssertOneOfTypes($left, @left, 4, BITS_TYPE, INT_TYPE, FLOAT_TYPE, ROM_TYPE);
		AssertOneOfTypes($right, @right, 4, BITS_TYPE, INT_TYPE, FLOAT_TYPE, ROM_TYPE);
		
		$$ = createNode(new OperatorAstNode($op->ptNode->text, $left->astNode, $right->astNode, new BoolType()), 
				createPtNodeWithChildren("expr", 3, $left->ptNode, $op->ptNode, $right->ptNode));
	}
	|
	TOK_U_LOGICAL_OP[op] expr[right] %prec TOK_U_LOGICAL
	{
		AssertOneOfTypes($right, @right, 4, BITS_TYPE, INT_TYPE, FLOAT_TYPE, ROM_TYPE);

		$$ = createNode(new OperatorAstNode($op->ptNode->text, $right->astNode, nullptr, new BoolType()), 
				createPtNodeWithChildren("expr", 2, $op->ptNode, $right->ptNode));
	}
	|
	TOK_U_ARITHMETICAL_OP[op] expr[right] %prec TOK_U_ARITHMETICAL
	{
		AssertOneOfTypes($right, @right, 4, BITS_TYPE, INT_TYPE, FLOAT_TYPE, ROM_TYPE);

		$$ = createNode(new OperatorAstNode($op->ptNode->text, $right->astNode), 
				createPtNodeWithChildren("expr", 2, $op->ptNode, $right->ptNode));
	}
	|
	expr[left] TOK_ARITHMETICAL_OP[op] expr[right]
	{
		AssertOneOfTypes($left, @left, 4, BITS_TYPE, INT_TYPE, FLOAT_TYPE, ROM_TYPE);
		AssertOneOfTypes($right, @right, 4, BITS_TYPE, INT_TYPE, FLOAT_TYPE, ROM_TYPE);

		$$ = createNode(new OperatorAstNode($op->ptNode->text, $left->astNode, $right->astNode), 
				createPtNodeWithChildren("expr", 3, $left->ptNode, $op->ptNode, $right->ptNode));
	}
	|
	expr[left] TOK_COMP_OP[op] expr[right]
	{
		AssertOneOfTypes($left, @left, 4, BITS_TYPE, ROM_TYPE, INT_TYPE, FLOAT_TYPE);
		AssertOneOfTypes($right, @right, 4, BITS_TYPE, ROM_TYPE, INT_TYPE, FLOAT_TYPE);

		$$ = createNode(new OperatorAstNode($op->ptNode->text, $left->astNode, $right->astNode, new BoolType()), 
				createPtNodeWithChildren("expr", 3, $left->ptNode, $op->ptNode, $right->ptNode));
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
	/* epsilon-правило */
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
			// TODO [SV] 15.08.13 12:18: possible checks for array\non-array type equality
			if (dimNode->GetExpr() != nullptr)
				astNode = new ArrayAddressAstNode(new VarAstNode(false, val), dimNode);
			else
				astNode = new VarAstNode(false, val);
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

switch_stmt: switch_head case_list default switch_tail
	{
		$$ = createNode(new SwitchAstNode($1->astNode, $2->astNode, $3->astNode),
				createPtNodeWithChildren("switch_stmt", 3, $1->ptNode, $2->ptNode, $3->ptNode, $4->ptNode));
	}
	| switch_head case_list switch_tail
	{
		$$ = createNode(new SwitchAstNode($1->astNode, $2->astNode, nullptr),
				createPtNodeWithChildren("switch_stmt", 2, $1->ptNode, $2->ptNode, $3->ptNode));
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
		//TODO: проверка значения ключа! оно не может повторяться
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
			$$ = $2;
		}
		
	}
	;
case_body: stmnt
	{
		$$ = $1;
	}
	| stmnt_block
	{
		$$ = $1;
	}
	| case_stmt
	{
		$$ = $1;
	}
	;
default: default_head default_body
	{
		$$ = createNode(new OperatorAstNode(OT_DEFAULT, $2->astNode),
				createPtNodeWithChildren("default", 2, $1->ptNode, $2->ptNode));
	}
	;
default_head: TOK_DEFAULT TOK_DOUBLEDOT	/* <s4> */
	{
		TSwitchOperator *switchOp = dynamic_cast<TSwitchOperator *>(Context.OperatorStackTop());
		TLabel *label = Context.GenerateNewLabel();
		TDefaultOperator *defOp = new TDefaultOperator(label);
		switchOp->AddDefaultOp(defOp);

		$$ = createNode(nullptr,
				createPtNodeWithChildren("default_head", 2, $1->ptNode, $2->ptNode));
	}
	;
default_body: stmnt
	{
		$$ = $1;
	}
	| stmnt_block
	{
		$$ = $1;
	}
	;

%%

void yyerror(const char *message)
{
}

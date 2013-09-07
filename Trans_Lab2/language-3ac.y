%{

#include <stdio.h>
#include "definitions.h"
#include "ast.h"
#include "hashtable.h"
#include "tml-generator.h"
#include <string>
#include <stdint.h>
	extern int taclex();
	extern int taclineno;
	extern THashTable<std::string, TVariable*> g_tmpVarsTable;
	extern THashTable<std::string, TVariable*> g_variableTable;
	extern THashTable<std::string, TLabel*> g_labelTable;
	extern THashTable<std::string, TVariable*> g_literalTable;
	extern void PrintVariable(FILE *stream, void *item);
	void tacerror(char* message);
	int i;

void tacerror(char *message)
{
	fprintf(stderr, "Syntax error: %s in line %d\n", message, taclineno);
}

%}

%union
{
	AstNode *_AstNode;
	Node *_node;
}

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

%type <_AstNode> start3ac program stmt stmt_list expression factor assign label go_to array_item _struct_item

%right ASSIGN
%left '+' '-' OR XOR
%left '*' '/' AND
%left '.'
%left _UMIN _NOT

%name-prefix "tac"
%verbose

%%
start3ac: program
	{
#ifdef PRINT_3AC_AST
		printf("\n3ac-AST:\n");
		PrintAST($1, 0);
#endif
		unsigned short codeSegmentSize;
		unsigned short dataSegmentSize;
		TMLHalt(g_OutputTmlFile);
		TMLFillDataSegment(g_OutputTmlFile);
		g_labelTable.ProcessHashTable(
			[=] (TLabel*& label) -> bool
			{
				for (auto it = label->jumpList.begin(); it != label->jumpList.end(); ++it)
				{
					long int offset;
					MachineInstruction i;
					int jumpIndex = (*it)->GetIndex();
					//TODO: ћагическое число - не очень хорошо
					offset = 
						sizeof(TMLHeader) +
						(int32_t) (jumpIndex * sizeof(MachineInstruction)) +
						(int32_t) sizeof(i.OpCode) +
						(int32_t) sizeof(i.AddrMode);

					fseek(g_OutputTmlFile, offset, SEEK_SET);
					unsigned int instruction_index = label->GetInstructionIndex();
					fwrite(&instruction_index, sizeof(unsigned int), 1, g_OutputTmlFile);
				}
				return true;
			}
		);
		codeSegmentSize = (unsigned short)(g_lastInstructionIndex*sizeof(MachineInstruction));
		dataSegmentSize = (unsigned short)(g_wordsCount * sizeof (TMemoryCell));
		TMLSegmentSize(g_OutputTmlFile, codeSegmentSize, dataSegmentSize);
		// TODO [SV]: Do something with this
		//FreeAST($1);
	}
	;
program: stmt_list
	{
		$$ = $1;
	}
	| /* пустое правило */
	{
		$$ = NULL;
	}
	;
stmt_list: stmt
	{
		$$ = $1;
	}
	| stmt_list stmt
	{
		// TODO [SV] 14.08.13 1:13: Check this behaviour!
		StatementBlockAstNode *astNode = new StatementBlockAstNode();
		astNode->AddStatement($1);
		astNode->AddStatement($2);
		$$ = astNode;
	}
	;
stmt:	label ':'
	{
		dynamic_cast<LabelAstNode*>($1)->GetLabel()->InitLabel(g_lastInstructionIndex);
		dynamic_cast<LabelAstNode*>($1)->GetLabel()->SetDeclaredLine(@label.first_line);
		$$ = $1;
	}
	| IFFALSE factor go_to
	{
		$$ = new ConditionalAstNode($2, nullptr, $3);
		TMLIf(g_OutputTmlFile, OP_IFFALSE, $2, $3);
	}
	| IFTRUE factor go_to
	{
		$$ = new ConditionalAstNode($2, $3, nullptr);
		TMLIf(g_OutputTmlFile, OP_IFTRUE, $2, $3);
	}
	| go_to
	{
		$$ = $1;
		TMLGoto(g_OutputTmlFile, OP_GOTO, $1);
	}
	| assign
	{
		$$ = $1;
	}
	| INPUT factor
	{
		$$ = new OperatorAstNode(OP_INPUT, $2);
		TMLInputOutput(g_OutputTmlFile, OP_INPUT, $2);
	}
	| OUTPUT factor
	{
		$$ = new OperatorAstNode(OP_OUTPUT, $2);
		TMLInputOutput(g_OutputTmlFile, OP_OUTPUT, $2);
	}
	;
assign:	TMP_VAR ASSIGN expression
	{
		TVariable *tmpVar;
		AstNode *tmpVarNode;
		auto tmpVarName = $1->ptNode->text;

		tmpVar = g_tmpVarsTable.st_get(std::string(tmpVarName));
		if(!tmpVar)
		{
			tmpVar = CreateVariable($3->GetResultType(), tmpVarName);
			g_tmpVarsTable.st_put(tmpVar->GetName(), tmpVar);
			tmpVar->StaticMalloc();
		}
		tmpVarNode = new VarAstNode(true, tmpVar);
		$$ = new OperatorAstNode(OP_ASSIGN, tmpVarNode, $3);
		TMLSaveLoad(g_OutputTmlFile, OP_SAVE, tmpVarNode);
	}
	|
	NAME ASSIGN expression
	{
		TVariable *var;
		AstNode *varNode;
		auto varName = $1->ptNode->text;

		var = g_variableTable.st_get(std::string(varName));
		varNode = new VarAstNode(false, var);
		$$ = new OperatorAstNode(OP_ASSIGN, varNode, $3);
		TMLSaveLoad(g_OutputTmlFile, OP_SAVE, varNode);
	}
	| _struct_item ASSIGN expression
	{
		OperatorAstNode *astNode = new OperatorAstNode(OP_ASSIGN, $1, $3);
		$$ = astNode;
		// Save from Accumulator
		TMLSaveLoadStructItem(g_OutputTmlFile, OP_SAVE, astNode->GetLeftOperand(), astNode->GetRightOperand());
	}
	| array_item ASSIGN expression
	{
		OperatorAstNode *astNode = new OperatorAstNode(OP_ASSIGN, $1, $3);
		$$ = astNode;
		// Save from Accumulator
		TMLSaveLoadArrayItem(g_OutputTmlFile, OP_SAVE, astNode->GetLeftOperand(), astNode->GetRightOperand());
	}
	;
expression: factor
	{
		$$ = $1;
		TMLSaveLoad(g_OutputTmlFile, OP_LOAD, $1);
	}
	| factor CMP factor
	{
		OperatorAstNode *astNode = new OperatorAstNode($2->ptNode->text, $1, $3);
		$$ = astNode;
		opEnum opID = astNode->GetOpID();
		TMLComparison(g_OutputTmlFile, opID, $1, $3);
	}
	| factor '+' factor
	{
		$$ = new OperatorAstNode('+', $1, $3);
		TMLBinaryArith(g_OutputTmlFile, '+', $1, $3);
	}
	| factor '-' factor
	{
		$$ = new OperatorAstNode('-', $1, $3);
		TMLBinaryArith(g_OutputTmlFile, '-', $1, $3);
	}
	| factor '*' factor
	{
		$$ = new OperatorAstNode('*', $1, $3);
		TMLBinaryArith(g_OutputTmlFile, '*', $1, $3);
	}
	| factor '/' factor
	{
		$$ = new OperatorAstNode('/', $1, $3);
		TMLBinaryArith(g_OutputTmlFile, '/', $1, $3);
	}
	| factor OR factor
	{
		$$ = new OperatorAstNode(OP_OR, $1, $3);
		TMLBinaryLogical(g_OutputTmlFile, OP_OR, $1, $3);
	}
	| factor AND factor
	{
		$$ = new OperatorAstNode(OP_AND, $1, $3);
		TMLBinaryLogical(g_OutputTmlFile, OP_AND, $1, $3);
	}
	| factor XOR factor
	{
		$$ = new OperatorAstNode(OP_XOR, $1, $3);
		TMLBinaryLogical(g_OutputTmlFile, OP_XOR, $1, $3);
	}
	| '-' factor %prec _UMIN
	{
		$$ = new OperatorAstNode(OP_UMIN, $2, nullptr);
		TMLUnaryArith(g_OutputTmlFile, OP_UMIN, $2);
	}
	| NOT factor %prec _NOT
	{
		$$ = new OperatorAstNode(OP_NOT, $2, nullptr);
		TMLUnaryLogical(g_OutputTmlFile, OP_NOT, $2);
	}
	| array_item
	{
		$$ = $1;
		auto *astNode = dynamic_cast<ArrayAddressAstNode*>($1);
		// Loading to Accumulator
		TMLSaveLoadArrayItem(g_OutputTmlFile, OP_LOAD, astNode);
	}
	| _struct_item
	{
		$$ = $1;
		auto *astNode = dynamic_cast<StructAddressAstNode*>($1);
		// Loading to Accumulator
		TMLSaveLoadStructItem(g_OutputTmlFile, OP_LOAD, astNode);
	}
	;
factor: TMP_VAR
	{
		auto tmpVarName = $1->ptNode->text;
		TVariable *tmpVar = g_tmpVarsTable.st_get(std::string(tmpVarName));
		$$ = new VarAstNode(true, tmpVar);
	}
	| NAME
	{
		auto varName = $1->ptNode->text;
		TVariable *var = g_variableTable.st_get(std::string(varName));
		$$ = new VarAstNode(false, var);
	}
	| INT_CONST
	{
		$$ = new NumValueAstNode($1->ptNode->text, new IntType());
	}
	| FLOAT_CONST
	{
		$$ = new NumValueAstNode($1->ptNode->text, new FloatType());
	}
	;
label: LABEL
	{
		auto labelName = $1->ptNode->text;

		TLabel* label = g_labelTable.st_get(std::string(labelName));
		if(label == nullptr)
		{
			label = new TLabel(std::string(labelName));
			g_labelTable.st_put(label->GetName(), label);
		}
		$$ = new LabelAstNode(label);
	}
	| NAME
	{
		auto labelName = $1->ptNode->text;
		TLabel* label = g_labelTable.st_get(std::string(labelName));
		$$ = new LabelAstNode(label);
	}
	;
go_to: GOTO label
	{
		dynamic_cast<LabelAstNode*>($2)->GetLabel()->SetUsedLine(@label.first_line);
		$$ = new OperatorAstNode(OP_GOTO, $2);
	}
	;
array_item: factor '[' factor ']'
	{
		$$ = new ArrayAddressAstNode(
				dynamic_cast<VarAstNode*>($1), 
				dynamic_cast<DimensionAstNode*>($3));
	}
	;
_struct_item: NAME '.' NAME
	{
		auto structName = $1->ptNode->text;
		auto fieldName = $3->ptNode->text;

		TVariable *var = g_variableTable.st_get(std::string(structName));
		StructType *varType = dynamic_cast<StructType *>(var->GetType());
		TVariable *field = varType->GetField(std::string(fieldName));

		$$ = new StructAddressAstNode(new VarAstNode(false, var), field);
	}
	;
%%
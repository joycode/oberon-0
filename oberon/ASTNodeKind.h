#ifndef AST_NODE_KIND_H
#define AST_NODE_KIND_H

class ASTNodeKind
{
public:
	enum T_ASTNodeKind {
		ERROR = 0,
		TERMINAL_SYMBOL,
		SELECTOR, NUMBER, FACTOR, TERM, SIMPLE_EXPRESSION, EXPRESSION,
		ASSIGNMENT, ACTUAL_PARAMETERS, PROCEDURE_CALL, IF_STATEMENT, WHILE_STATEMENT, STATEMENT, STATEMENT_SEQUENCE,
		IDENT_LIST, ARRAY_TYPE, FIELD_LIST, RECORD_TYPE, TYPE, 
		FP_SECTION, FORMAL_PARAMETERS, PROCEDURE_HEADING, PROCEDURE_BODY, PROCEDURE_DECLARATION, DECLATIONS, MODULE,
	};
};

#endif
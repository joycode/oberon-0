#ifndef PARSER_H
#define PARSER_H

#include <cstdio>
#include <map>
#include "ASTNode.h"
#include "Scanner.h"

class Parser
{
public:
	Parser(void);
	virtual ~Parser(void);

	// return NULL when failed
	ASTNode *parse(FILE *fp);

private:
	SymbolKind::T_SymbolKind getCurrentSymbol();
	SymbolKind::T_SymbolKind getNextSymbol();

	// caution: due to EBNF Production, even return true, *result may still be NULL
	bool selector(ASTNode **result);
	bool factor(ASTNode **result);
	bool term(ASTNode **result);
	bool simpleExpression(ASTNode **result);
	bool expression(ASTNode **result);

	bool assignment(ASTNode **result);
	bool actualParameters(ASTNode **result);
	bool procedureCall(ASTNode **result);
	bool ifStatement(ASTNode **result);
	bool whileStatement(ASTNode **result);
	// caution: due to EBNF Production, even return true, *result may still be NULL
	bool statement(ASTNode **result);
	// caution: due to EBNF Production, even return true, *result may still be NULL
	bool statementSequence(ASTNode **result);

	bool identList(ASTNode **result);
	bool arrayType(ASTNode **result);
	// caution: due to EBNF Production, even return true, *result may still be NULL
	bool fieldList(ASTNode **result);
	bool recordType(ASTNode **result);
	bool type(ASTNode **result);
	bool fpSection(ASTNode **result);
	bool formalParameters(ASTNode **result);
	bool procedureHeading(ASTNode **result);
	bool procedureBody(ASTNode **result);
	bool procedureDeclaration(ASTNode **result);
	// caution: due to EBNF Production, even return true, *result may still be NULL
	bool declarations(ASTNode **result);
	bool module(ASTNode **result);

private:
	Scanner *m_scanner;
	std::map<SymbolObject*, std::string> m_identValuesDict;
	std::map<SymbolObject*, int> m_numberValuesDict;
};

#endif
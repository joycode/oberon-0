#ifndef Parser_H
#define Parser_H

#include <hash_set>
#include <hash_map>
#include "ASTNode.h"
#include "Scanner.h"

using namespace std;

class Parser
{
public:
	Parser(void);
	virtual ~Parser(void);

	// return NULL when failed
	SymbolObject *parse(string filePath);

private:

	// return NULL when failed
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
	bool statement(ASTNode **result);
	bool statementSequence(ASTNode **result);

	bool identList(ASTNode **result);
	bool arrayType(ASTNode **result);
	bool fieldList(ASTNode **result);
	bool recordType(ASTNode **result);
	bool type(ASTNode **result);
	bool fpSection(ASTNode **result);
	bool formalParameters(ASTNode **result);
	bool procedureHeading(ASTNode **result);
	bool procedureBody(ASTNode **result);
	bool procedureDeclaration(ASTNode **result);
	bool declarations(ASTNode **result);
	bool module(ASTNode **result);

private:
	Scanner *m_scanner;
	hash_map<SymbolObject*, string> m_identValuesDict;
	hash_map<SymbolObject*, int> m_numberValuesDict;
};

#endif
#ifndef AST_NODE_H
#define AST_NODE_H

#include <vector>
#include <list>
#include "ASTNodeKind.h"
#include "SymbolObject.h"

class ASTNode {
public:
	ASTNode(ASTNodeKind::T_ASTNodeKind astNodeKind)
		: m_children()
	{
		m_astNodeKind = astNodeKind;
		m_symbolObject = NULL;
	}

	ASTNode(SymbolObject *symbolObject)
		: m_children()
	{
		m_astNodeKind = ASTNodeKind::TERMINAL_SYMBOL;
		m_symbolObject = symbolObject;
	}

	virtual ~ASTNode(void)
	{
	}

private:
	ASTNode(void)
	{
	}

public:
	const ASTNodeKind::T_ASTNodeKind getASTNodeKind() const
	{
		return m_astNodeKind;
	}

	// not NULL when ASTNode stands for a terminal symbol
	const SymbolObject *getSymbolObject() const
	{
		return m_symbolObject;
	}

	const std::list<ASTNode*> *getChildren() const
	{
		return &m_children;
	}

	void addChild(ASTNode *astNode)
	{
		m_children.push_back(astNode);
	}

private:
	ASTNodeKind::T_ASTNodeKind m_astNodeKind;
	// not NULL when @m_astNodeKind stands for a terminal symbol
	SymbolObject *m_symbolObject;
	std::list<ASTNode*> m_children;
};

#endif
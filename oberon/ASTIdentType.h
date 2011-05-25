#ifndef AST_IDENT_TYPE_H
#define AST_IDENT_TYPE_H

#include <cassert>
#include <string>
#include "ASTNode.h"

class ASTIdentType
{
public:
	enum T_IdentType {
		None = 0, ARRAY, RECORD, CUSTOM,
	};

	ASTIdentType(ASTIdentType::T_IdentType identType, ASTNode *astNodeOfType)
	{
		assert((identType != ASTIdentType::None) && (astNodeOfType != NULL));
		m_identType = identType;
		m_astNodeOfType = astNodeOfType;
	}

	virtual ~ASTIdentType(void)
	{
	}

private:
	ASTIdentType(void)
	{
		m_identType = ASTIdentType::None;
		m_astNodeOfType = NULL;
	}

public:
	const T_IdentType getIdentType() const
	{
		return m_identType;
	}

	const ASTNode *getASTNodeOfType() const
	{
		return m_astNodeOfType;
	}

private:
	T_IdentType m_identType;
	ASTNode *m_astNodeOfType;
};

#endif
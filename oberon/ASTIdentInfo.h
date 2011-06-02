#ifndef AST_IDENT_INFO_H
#define AST_IDENT_INFO_H

#include <cassert>
#include <string>
#include <list>
#include "SymbolObject.h"
#include "ASTIdentType.h"

class ASTIdentInfo
{
public:

	ASTIdentInfo(ASTIdentType *identType, ASTNode *astNode)
		: m_fields()
	{
		assert((identType != NULL) && (astNode != NULL));

		m_identType = identType;
		m_astNode = astNode;
	}

	virtual ~ASTIdentInfo(void)
	{
	}

private:
	ASTIdentInfo(void)
	{
	}

public:
	const ASTNode* getASTNode() const
	{
		return m_astNode;
	}
	const ASTIdentType *getIdentType() const
	{
		return m_identType;
	}

	// test if @ident is a field of this ident
	// return NULL when not
	const ASTIdentInfo *findField(std::string ident) const
	{
		//list<const ASTIdentInfo* const>::const_iterator it = m_fields.begin();
		//while (it != m_fields.end()) {
		//	const ASTIdentInfo* const field = *it;
		//	if (field->getASTNode()->getSymbolObject()->getIdentValue().compare(ident) == 0) {
		//		return field;
		//	}
		//}

		return NULL;
	}

	void pushField(ASTIdentInfo *ident)
	{
		m_fields.push_back(ident);
	}

private:
	ASTIdentType *m_identType;
	ASTNode *m_astNode;

	std::list<const ASTIdentInfo* const> m_fields;
};

#endif
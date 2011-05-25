#ifndef SYMBOL_OBJECT_H
#define SYMBOL_OBJECT_H

#include "SymbolKind.h"

class SymbolObject
{
public:
	SymbolObject(SymbolKind::T_SymbolKind symbolKind)
	{
		m_symbolKind = symbolKind;
	}

	virtual ~SymbolObject(void)
	{
	}

private:
	SymbolObject(void)
	{
		m_symbolKind = SymbolKind::ERROR;
	}

public:
	const SymbolKind::T_SymbolKind getSymbolKind() const
	{
		return m_symbolKind;
	}

private:
	SymbolKind::T_SymbolKind m_symbolKind;
};

#endif
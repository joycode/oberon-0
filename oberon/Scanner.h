#ifndef SCANNER_H
#define SCANNER_H

#include <cstdio>
#include <string>
#include "SymbolKind.h"
#include "SourceLocation.h"

class Scanner {
public:
	Scanner(FILE *fp);
	virtual ~Scanner();

	void mark(std::string errMsg);
	bool init();
	SymbolKind::T_SymbolKind get();

	SymbolKind::T_SymbolKind getCurrentSymbolKind() const
	{
		return m_currentSymbolKind;
	}

	std::string getIdentValue() const
	{
		return m_ident;
	}

	int getNumberValue() const
	{
		return m_number;
	}

private:
	// real function for get symbol
	SymbolKind::T_SymbolKind getInternal();

	// thin wrapper of getc(@m_fp), for updating @m_sourceLocation
	char peekChar();
	bool isWhiteSpace(char ch);

	bool skipComment();
	SymbolKind::T_SymbolKind getIdent();
	void getNumber();

private:
	FILE *m_fp;
	SymbolKind::T_SymbolKind m_currentSymbolKind;
	char m_peekChar;
	int m_number;
	std::string m_ident;
	SourceLocation m_sourceLocation;
};

#endif

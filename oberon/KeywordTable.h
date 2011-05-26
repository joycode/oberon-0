#ifndef KEYWORD_TABLE_H
#define KEYWORD_TABLE_H

#include <cassert>
#include <string>
#include "SymbolKind.h"
#include "Util.h"

struct KeywordInfo;
typedef const KeywordInfo * CP_KeywordInfo;

class KeywordTable {
private:
	struct KeywordInfo {
		std::string m_keyword;
		SymbolKind::T_SymbolKind m_symbolKind;

		KeywordInfo(std::string keyword, SymbolKind::T_SymbolKind symbolKind)
		{
			m_keyword = keyword;
			m_symbolKind = symbolKind;
		}
	};

	// simple but ineffcient
	static const CP_KeywordInfo m_keywords[];

public:
	// test if @word is a keyword?
	// if true, store corresponding Symbol Kind in @result
	static bool isKeyword(std::string word, SymbolKind::T_SymbolKind *result)
	{
		for (int i = 0;;i++) {
			if (m_keywords[i] == NULL) {
				return false;
			}
			else if (((KeywordInfo *)m_keywords[i])->m_keyword.compare(word) == 0) {
				*result =  ((KeywordInfo *)m_keywords[i])->m_symbolKind;
				return true;
			}
		}
		
		return false;
	}
};

const CP_KeywordInfo KeywordTable::m_keywords[] = { 
	(CP_KeywordInfo)new KeywordInfo("DIV", SymbolKind::DIV),
	(CP_KeywordInfo)new KeywordInfo("MOD", SymbolKind::MOD),
	(CP_KeywordInfo)new KeywordInfo("OR", SymbolKind::OR),
	(CP_KeywordInfo)new KeywordInfo("OF", SymbolKind::OF),
	(CP_KeywordInfo)new KeywordInfo("THEN", SymbolKind::THEN),
	(CP_KeywordInfo)new KeywordInfo("DO", SymbolKind::DO),
	(CP_KeywordInfo)new KeywordInfo("END", SymbolKind::END),
	(CP_KeywordInfo)new KeywordInfo("ELSE", SymbolKind::ELSE),
	(CP_KeywordInfo)new KeywordInfo("ELSEIF", SymbolKind::ELSEIF),
	(CP_KeywordInfo)new KeywordInfo("IF", SymbolKind::IF),
	(CP_KeywordInfo)new KeywordInfo("WHILE", SymbolKind::WHILE),
	(CP_KeywordInfo)new KeywordInfo("ARRAY", SymbolKind::ARRAY),
	(CP_KeywordInfo)new KeywordInfo("RECORD", SymbolKind::RECORD),
	(CP_KeywordInfo)new KeywordInfo("CONST", SymbolKind::CONST),
	(CP_KeywordInfo)new KeywordInfo("TYPE", SymbolKind::TYPE),
	(CP_KeywordInfo)new KeywordInfo("VAR", SymbolKind::VAR),
	(CP_KeywordInfo)new KeywordInfo("PROCEDURE", SymbolKind::PROCEDURE),
	(CP_KeywordInfo)new KeywordInfo("BEGIN", SymbolKind::BEGIN),
	(CP_KeywordInfo)new KeywordInfo("MODULE", SymbolKind::MODULE),
	NULL };

#endif
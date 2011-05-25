#include <cstdio>
#include <cassert>
#include <string>
#include "scanner.h"
#include "KeywordTable.h"

using namespace std;

Scanner::Scanner(FILE *fp)
{
	m_fp = fp;
	m_currentSymbolKind == SymbolKind::ERROR;
	m_peekChar = '\0';
	m_number = 0;
	m_ident = "";
}

Scanner::~Scanner()
{
}

void Scanner::mark(string errMsg)
{
	fprintf(stderr, "Line %d, Column %d: %s\n", 
		m_sourceLocation.m_x, m_sourceLocation.m_y, 
		errMsg);
}

SymbolKind::T_SymbolKind Scanner::get()
{
	m_currentSymbolKind = this->getInternal();
	return m_currentSymbolKind;
}

SymbolKind::T_SymbolKind Scanner::getInternal()
{
	for (;;) {
		if (this->isWhiteSpace(m_peekChar)) {
			m_peekChar = this->peekChar();
		}
		
		switch (m_peekChar) {
		case '*':
			m_peekChar = this->peekChar();
			return SymbolKind::TIMES;
		case '&':
			m_peekChar = this->peekChar();
			return SymbolKind::AND;
		case '+':
			m_peekChar = this->peekChar();
			return SymbolKind::PLUS;
		case '-':
			m_peekChar = this->peekChar();
			return SymbolKind::MINUS;
		case '=':
			m_peekChar = this->peekChar();
			return SymbolKind::EQL;
		case '#':
			m_peekChar = this->peekChar();
			return SymbolKind::NEQ;
		case '<':
			m_peekChar = this->peekChar();
			if (m_peekChar == '=') {
				return SymbolKind::LEQ;
			}
			else {
				return SymbolKind::LSS;
			}
		case '>':
			m_peekChar = this->peekChar();
			if (m_peekChar == '=') {
				return SymbolKind::GEQ;
			}
			else {
				return SymbolKind::GTR;
			}
		case '.':
			m_peekChar = this->peekChar();
			return SymbolKind::PERIOD;
		case ',':
			m_peekChar = this->peekChar();
			return SymbolKind::COMMA;
		case ':':
			m_peekChar = this->peekChar();
			return SymbolKind::COLON;
		case ')':
			m_peekChar = this->peekChar();
			return SymbolKind::RPAREN;
		case ']':
			m_peekChar = this->peekChar();
			return SymbolKind::RBRAK;
		case '(':
			m_peekChar = this->peekChar();
			if (m_peekChar == '*') {
				if (!this->skipComment()) {
					return SymbolKind::ERROR;
				}
				else {
					return this->get();
				}
			}
			else {
				return SymbolKind::LPREN;
			}
		case '[':
			m_peekChar = this->peekChar();
			return SymbolKind::LBRAK;
		case '~':
			m_peekChar = this->peekChar();
			return SymbolKind::NOT;
		case ';':
			m_peekChar = this->peekChar();
			return SymbolKind::SEMICOLON;
		case EOF:
			return SymbolKind::END_OF_FILE;
		default:
			if (isalpha(m_peekChar)) {
				return this->getIdent();
			}
			else if (isdigit(m_peekChar)) {
				this->getNumber();
				return SymbolKind::NUMBER;
			}
			else {
				this->mark("illegal character");
				return SymbolKind::ERROR;
			}
		};
	}
}

bool Scanner::init()
{
	m_peekChar = this->peekChar();
	return true;
}

char Scanner::peekChar()
{
	// maybe a problem?
	char ch = (char)getc(m_fp);

	if (ch == '\n') {
		m_sourceLocation.m_x++;
	}
	else if (ch == '\r') {
		// nothing to do in case of "\n\r"
	}
	else {
		// TODO. how about handling '\t'?
		m_sourceLocation.m_y++;
	}

	return ch;
}

bool Scanner::isWhiteSpace(char ch)
{
	if ((ch == ' ') || (ch == '\t') || (ch == '\n') || (ch == '\r')) {
		return true;
	}
	else {
		return false;
	}
}

bool Scanner::skipComment()
{
	bool triggering = false;

	while ((m_peekChar = this->peekChar()) != EOF) {
		if (triggering) {
			if (m_peekChar == ')') {
				m_peekChar = this->peekChar();
				return true;
			}
			else {
				triggering = false;
			}
		}
		else if (m_peekChar == '*') {
			triggering = true;
		}
	}

	this->mark("comment not terminated");
	return false;
}

SymbolKind::T_SymbolKind Scanner::getIdent()
{
	string ident = "";
	ident += m_peekChar;

	while ((m_peekChar = this->peekChar()) != EOF) {
		if (isalpha(m_peekChar) || isdigit(m_peekChar)) {
			ident += m_peekChar;
		}
	}

	SymbolKind::T_SymbolKind sym = SymbolKind::ERROR;
	if (KeywordTable::isKeyword(ident, &sym)) {
		return sym;
	}
	else {
		sym = SymbolKind::IDENT;
		m_ident = ident;
		return sym;
	}
}

void Scanner::getNumber()
{
	string number = "";
	number += m_peekChar;

	while ((m_peekChar = this->peekChar()) != EOF) {
		if (isdigit(m_peekChar)) {
			number += m_peekChar;
		}
	}

	m_number = atoi(number.c_str());
}

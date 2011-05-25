// mapping oberon-0's terminal symbols

#ifndef SYMBOL_KINDS_H
#define SYMBOL_KINDS_H

class SymbolKind {
public:
	enum T_SymbolKind {
		ERROR = 0,
		TIMES, DIV, MOD, AND, PLUS, MINUS, OR,
		EQL, NEQ, LSS, GEQ, LEQ, GTR,
		PERIOD, COMMA, COLON, RPAREN, RBRAK,
		OF, THEN, DO,
		LPREN, LBRAK, NOT, BECOMES, NUMBER, IDENT,
		SEMICOLON, END, ELSE, ELSEIF,
		IF, WHILE,
		ARRAY, RECORD,
		CONST, TYPE, VAR, PROCEDURE, BEGIN, MODULE,
		END_OF_FILE,			
	};
};

#endif

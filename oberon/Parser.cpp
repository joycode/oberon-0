#include <cassert>
#include <cstdio>
#include <string>
#include "Parser.h"

using namespace std;

// TODO. ignoring memory leaks

Parser::Parser(void)
	: m_identValuesDict(), m_numberValuesDict()
{
	m_scanner = NULL;
}

Parser::~Parser(void)
{
	if (m_scanner != NULL) {
		delete m_scanner;
		m_scanner = NULL;
	}
}

ASTNode *Parser::parse(FILE *fp)
{
	m_scanner = new Scanner(fp);
	if (!m_scanner->init()) {
		return NULL;
	}

	this->getNextSymbol();

	ASTNode *module = NULL;
	if (!this->module(&module)) {
		m_scanner->mark("unknown");
		if (module != NULL) {
			delete module;
		}
		return NULL;
	}
	if (module == NULL) {
		assert(false);
		return NULL;
	}

	return module;
}

SymbolKind::T_SymbolKind Parser::getCurrentSymbol()
{
	assert(m_scanner != NULL);
	return m_scanner->getCurrentSymbolKind();
}

SymbolKind::T_SymbolKind Parser::getNextSymbol()
{
	assert(m_scanner != NULL);
	return m_scanner->get();
}

bool Parser::selector(ASTNode **result)
{
	assert(m_scanner != NULL);
	SymbolKind::T_SymbolKind sym_kind = this->getCurrentSymbol();
	if (sym_kind == SymbolKind::ERROR) {
		return false;
	}

	ASTNode **last_node = result;
	for (;;) {
		ASTNode **current_node = result;

		if (sym_kind == SymbolKind::ERROR) {
			return false;
		}
		else if (sym_kind == SymbolKind::PERIOD) {
			sym_kind = this->getNextSymbol();
			if (sym_kind != SymbolKind::IDENT) {
				// TODO. how about error handling?
				return false;
			}

			SymbolObject *sym_obj = new SymbolObject(SymbolKind::IDENT);
			string ident_value = m_scanner->getIdentValue();
			m_identValuesDict.insert(std::make_pair(sym_obj, ident_value));

			*current_node = new ASTNode(ASTNodeKind::SELECTOR);
			(*current_node)->addChild(new ASTNode(new SymbolObject(SymbolKind::PERIOD)));
			(*current_node)->addChild(new ASTNode(sym_obj));

			if (*last_node != NULL) {
				(*last_node)->addChild(*current_node);
			}
			*last_node = *current_node;

			sym_kind = this->getNextSymbol();

			continue;
		}
		else if (sym_kind == SymbolKind::LBRAK) {
			sym_kind = this->getNextSymbol();
			if ((sym_kind == SymbolKind::ERROR) || (sym_kind == SymbolKind::END_OF_FILE)) {
				// TODO. error handling
				return false;
			}

			ASTNode *exp = NULL;
			if (!this->expression(&exp)) {
				return false;
			}
			assert(exp != NULL);

			sym_kind = this->getCurrentSymbol();
			if (sym_kind != SymbolKind::RBRAK) {
				// TODO. error handling
				return false;
			}

			*current_node = new ASTNode(ASTNodeKind::SELECTOR);
			(*current_node)->addChild(new ASTNode(new SymbolObject(SymbolKind::LBRAK)));
			(*current_node)->addChild(exp);
			(*current_node)->addChild(new ASTNode(new SymbolObject(SymbolKind::RBRAK)));

			if (*last_node != NULL) {
				(*last_node)->addChild(*current_node);
			}
			*last_node = *current_node;

			sym_kind = this->getNextSymbol();

			continue;
		}
		else {
			return true;
		}
	}
}

bool Parser::factor(ASTNode **result)
{
	assert(m_scanner != NULL);
	SymbolKind::T_SymbolKind sym_kind = this->getCurrentSymbol();
	if (sym_kind == SymbolKind::ERROR) {
		return false;
	}

	*result = new ASTNode(ASTNodeKind::FACTOR);

	if (sym_kind == SymbolKind::ERROR) {
		return false;
	}
	else if (sym_kind == SymbolKind::IDENT) {
		sym_kind = this->getNextSymbol();
		if (sym_kind == SymbolKind::ERROR) {
			return false;
		}
		else if ((sym_kind == SymbolKind::PERIOD) || (sym_kind == SymbolKind::LBRAK)) {
			ASTNode *ast_selector = NULL;
			if (!this->selector(&ast_selector)) {
				return false;
			}

			if (ast_selector != NULL) {
				(*result)->addChild(ast_selector);
			}

			return true;
		}
		else {
			return true;
		}
	}
	else if (sym_kind == SymbolKind::NUMBER) {
		SymbolObject *sym_obj = new SymbolObject(SymbolKind::NUMBER);
		m_numberValuesDict.insert(std::make_pair(sym_obj, m_scanner->getNumberValue()));

		ASTNode *ast_number = new ASTNode(sym_obj);

		(*result)->addChild(ast_number);

		this->getNextSymbol();

		return true;
	}
	else if (sym_kind == SymbolKind::LPREN) {
		sym_kind = this->getNextSymbol();
		if ((sym_kind == SymbolKind::ERROR) || (sym_kind == SymbolKind::END_OF_FILE)) {
			// TODO. error handling
			return false;
		}

		ASTNode *ast_exp = NULL;
		if (!this->expression(&ast_exp)) {
			return false;
		}
		assert(ast_exp != NULL);

		sym_kind = this->getCurrentSymbol();
		if (sym_kind != SymbolKind::RPAREN) {
			// TODO. error handling
			return false;
		}

		(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::LBRAK)));
		(*result)->addChild(ast_exp);
		(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::RBRAK)));

		sym_kind = this->getNextSymbol();

		return true;
	}
	else if (sym_kind == SymbolKind::NOT) {
		sym_kind = this->getNextSymbol();
		if ((sym_kind == SymbolKind::ERROR) || (sym_kind == SymbolKind::END_OF_FILE)) {
			// TODO. error handling
			return false;
		}

		ASTNode *ast_factor = NULL;
		if (!this->factor(&ast_factor)) {
			return false;
		}
		assert(ast_factor != NULL);

		(*result)->addChild(ast_factor);

		return true;
	}
	else {
		return false;
	}
}
bool Parser::term(ASTNode **result)
{
	assert(m_scanner != NULL);
	SymbolKind::T_SymbolKind sym_kind = this->getCurrentSymbol();
	if (sym_kind == SymbolKind::ERROR) {
		return false;
	}

	*result = new ASTNode(ASTNodeKind::TERM);
	
	ASTNode *ast_factor = NULL;
	if (!this->factor(&ast_factor)) {
		return false;
	}
	assert(ast_factor != NULL);

	(*result)->addChild(ast_factor);

	sym_kind = this->getCurrentSymbol();
	if (sym_kind == SymbolKind::ERROR) {
		return false;
	}

	while ((sym_kind == SymbolKind::TIMES) || (sym_kind == SymbolKind::DIV) || 
		(sym_kind == SymbolKind::MOD) || (sym_kind == SymbolKind::AND)) {
			sym_kind = this->getNextSymbol();
			if ((sym_kind == SymbolKind::ERROR) || (sym_kind == SymbolKind::END_OF_FILE)) {
				// TODO. error handling
				return false;
			}

			if (!this->factor(&ast_factor)) {
				return false;
			}
			assert(ast_factor != NULL);

			(*result)->addChild(new ASTNode(new SymbolObject(sym_kind)));
			(*result)->addChild(ast_factor);

			sym_kind = this->getCurrentSymbol();
	}

	return true;
}
bool Parser::simpleExpression(ASTNode **result)
{
	assert(m_scanner != NULL);
	SymbolKind::T_SymbolKind sym_kind = this->getCurrentSymbol();
	if (sym_kind == SymbolKind::ERROR) {
		return false;
	}

	*result = new ASTNode(ASTNodeKind::SIMPLE_EXPRESSION);

	if ((sym_kind == SymbolKind::PLUS) || (sym_kind == SymbolKind::MINUS)) {
		(*result)->addChild(new ASTNode(new SymbolObject(sym_kind)));
		sym_kind = this->getNextSymbol();
		if ((sym_kind == SymbolKind::ERROR) || (sym_kind == SymbolKind::END_OF_FILE)) {
			// TODO. error handling
			return false;
		}
	}

	ASTNode *ast_term = NULL;
	if (!this->term(&ast_term)) {
		return false;
	}

	assert(ast_term != NULL);
	(*result)->addChild(ast_term);

	sym_kind = this->getCurrentSymbol();
	while ((sym_kind == SymbolKind::PLUS) || (sym_kind == SymbolKind::MINUS) || (sym_kind == SymbolKind::OR)) {
		(*result)->addChild(new ASTNode(new SymbolObject(sym_kind)));

		sym_kind = this->getNextSymbol();
		if (!this->term(&ast_term)) {
			return false;
		}

		assert(ast_term != NULL);
		(*result)->addChild(ast_term);

		sym_kind = this->getCurrentSymbol();
	}

	return true;
}
bool Parser::expression(ASTNode **result)
{
	assert(m_scanner != NULL);
	SymbolKind::T_SymbolKind sym_kind = this->getCurrentSymbol();
	if (sym_kind == SymbolKind::ERROR) {
		return false;
	}

	*result = new ASTNode(ASTNodeKind::EXPRESSION);

	ASTNode *ast_simple_exp = NULL;
	if (!this->simpleExpression(&ast_simple_exp)) {
		return false;
	}
	assert(ast_simple_exp != NULL);

	(*result)->addChild(ast_simple_exp);

	sym_kind = this->getCurrentSymbol();
	if (sym_kind == SymbolKind::ERROR) {
		return false;
	}
	else if ((sym_kind == SymbolKind::EQL) || (sym_kind == SymbolKind::NEQ) ||
		(sym_kind == SymbolKind::LSS) || (sym_kind == SymbolKind::LEQ) ||
		(sym_kind == SymbolKind::GTR) || (sym_kind == SymbolKind::GEQ)) {
			(*result)->addChild(new ASTNode(new SymbolObject(sym_kind)));

			sym_kind = this->getNextSymbol();
			if ((sym_kind == SymbolKind::ERROR) || (sym_kind == SymbolKind::END_OF_FILE)) {
				// TODO. error handling
				return false;
			}

			if (!this->simpleExpression(&ast_simple_exp)) {
				return false;
			}
			assert(ast_simple_exp != NULL);

			(*result)->addChild(ast_simple_exp);

			return true;
	}
	else {
		return true;
	}
}

bool Parser::assignment(ASTNode **result)
{
	assert(m_scanner != NULL);
	SymbolKind::T_SymbolKind sym_kind = this->getCurrentSymbol();
	if (sym_kind == SymbolKind::ERROR) {
		return false;
	}

	*result = new ASTNode(ASTNodeKind::ASSIGNMENT);
	if (sym_kind != SymbolKind::IDENT) {
		return false;
	}

	(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::IDENT)));

	ASTNode *ast_selector = NULL;
	if (!this->selector(&ast_selector)) {
		return false;
	}

	if (ast_selector != NULL) {
		(*result)->addChild(ast_selector);
	}

	if (sym_kind != SymbolKind::BECOMES) {
		return false;
	}

	(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::BECOMES)));

	sym_kind = this->getNextSymbol();
	if ((sym_kind == SymbolKind::ERROR) || (sym_kind == SymbolKind::END_OF_FILE)) {
		// TODO. error handling
		return false;
	}

	ASTNode *ast_exp = NULL;
	if (!this->expression(&ast_exp)) {
		return false;
	}
	assert(ast_exp != NULL);

	(*result)->addChild(ast_exp);

	return true;
}
bool Parser::actualParameters(ASTNode **result)
{
	assert(m_scanner != NULL);
	SymbolKind::T_SymbolKind sym_kind = this->getCurrentSymbol();

	if (sym_kind != SymbolKind::LPREN) {
		return false;
	}

	*result = new ASTNode(ASTNodeKind::ACTUAL_PARAMETERS);

	(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::LPREN)));

	sym_kind = this->getNextSymbol();
	if ((sym_kind == SymbolKind::ERROR) || (sym_kind == SymbolKind::END_OF_FILE)) {
		// TODO. error handling
		return false;
	}

	if (sym_kind != SymbolKind::RPAREN) {
		ASTNode *ast_exp = NULL;
		if (!this->expression(&ast_exp)) {
			return false;
		}
		assert(ast_exp != NULL);
		
		(*result)->addChild(ast_exp);

		while (sym_kind == SymbolKind::COMMA) {
			sym_kind = this->getNextSymbol();
			if ((sym_kind == SymbolKind::ERROR) || (sym_kind == SymbolKind::END_OF_FILE)) {
				// TODO. error handling
				return false;
			}

			if (!this->expression(&ast_exp)) {
				return false;
			}
			assert(ast_exp != NULL);

			(*result)->addChild(ast_exp);
		}
	}

	sym_kind = this->getCurrentSymbol();
	if (sym_kind != SymbolKind::RPAREN) {
		// TODO. error handling
		return false;
	}

	(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::RPAREN)));

	this->getNextSymbol();

	return true;
}
bool Parser::procedureCall(ASTNode **result)
{
	assert(m_scanner != NULL);
	SymbolKind::T_SymbolKind sym_kind = this->getCurrentSymbol();
	if (sym_kind != SymbolKind::IDENT) {
		return false;
	}

	*result = new ASTNode(ASTNodeKind::PROCEDURE_CALL);

	(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::IDENT)));

	sym_kind = this->getNextSymbol();
	if ((sym_kind == SymbolKind::PERIOD) || (sym_kind == SymbolKind::LBRAK)) {
		ASTNode *ast_selector = NULL;
		if (!this->selector(&ast_selector)) {
			return false;
		}
		
		if (ast_selector != NULL) {
			(*result)->addChild(ast_selector);
		}
	}

	if (sym_kind == SymbolKind::ERROR) {
		return false;
	}
	else if (sym_kind == SymbolKind::LPREN) {
		ASTNode *ast_params = NULL;
		if (!this->actualParameters(&ast_params)) {
			return false;
		}
		if (ast_params == NULL) {
			assert(false);
			return false;
		}

		(*result)->addChild(ast_params);

		return true;
	}
	else {
		return true;
	}
}
bool Parser::ifStatement(ASTNode **result)
{
	assert(m_scanner != NULL);
	SymbolKind::T_SymbolKind sym_kind = this->getCurrentSymbol();
	if (sym_kind != SymbolKind::IF) {
		return false;
	}

	*result = new ASTNode(ASTNodeKind::IF_STATEMENT);
	(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::IF)));

	sym_kind = this->getNextSymbol();

	ASTNode *ast_exp = NULL;
	if (!this->expression(&ast_exp)) {
		return false;
	}
	if (ast_exp == NULL) {
		assert(false);
		return false;
	}

	(*result)->addChild(ast_exp);

	sym_kind = this->getCurrentSymbol();
	if (sym_kind != SymbolKind::THEN) {
		return false;
	}

	(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::THEN)));

	ASTNode *ast_stmt_seq = NULL;

	sym_kind = this->getNextSymbol();
	if (!this->statementSequence(&ast_stmt_seq)) {
		return false;
	}

	if (ast_stmt_seq != NULL) {
		(*result)->addChild(ast_stmt_seq);
	}

	sym_kind = this->getCurrentSymbol();
	while (sym_kind == SymbolKind::ELSEIF) {
		(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::ELSEIF)));

		sym_kind = this->getNextSymbol();
		if (!this->expression(&ast_exp)) {
			return false;
		}
		if (ast_exp == NULL) {
			assert(false);
			return false;
		}
		(*result)->addChild(ast_exp);
		
		sym_kind = this->getCurrentSymbol();
		if (sym_kind != SymbolKind::THEN) {
			return false;
		}
		(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::THEN)));

		this->getNextSymbol();
		if (!this->statementSequence(&ast_stmt_seq)) {
			return false;
		}

		if (ast_stmt_seq != NULL) {
			(*result)->addChild(ast_stmt_seq);
		}

		sym_kind = this->getCurrentSymbol();
	}
	
	if (sym_kind == SymbolKind::ELSE) {
		(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::ELSE)));

		this->getNextSymbol();
		if (!this->statementSequence(&ast_stmt_seq)) {
			return false;
		}

		if (ast_stmt_seq != NULL) {
			(*result)->addChild(ast_stmt_seq);
		}

		sym_kind = this->getCurrentSymbol();
	}
	
	if (sym_kind != SymbolKind::END) {
		return false;
	}
	else {  
		(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::END)));  

		this->getNextSymbol();

		return true;
	}
}
bool Parser::whileStatement(ASTNode **result)
{
	assert(m_scanner != NULL);
	SymbolKind::T_SymbolKind sym_kind = this->getCurrentSymbol();
	if (sym_kind != SymbolKind::WHILE) {
		return false;
	}

	*result = new ASTNode(ASTNodeKind::WHILE_STATEMENT);
	(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::WHILE)));

	this->getNextSymbol();

	ASTNode *ast_exp = NULL;
	if (!this->expression(&ast_exp)) {
		return false;
	}
	if (ast_exp == NULL) {
		assert(false);
		return false;
	}
	(*result)->addChild(ast_exp);

	sym_kind = this->getCurrentSymbol();
	if (sym_kind != SymbolKind::DO) {
		return false;
	}
	(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::DO)));

	this->getNextSymbol();

	ASTNode *ast_stmt_seq = NULL;
	if (!this->statementSequence(&ast_stmt_seq)) {
		if (ast_stmt_seq != NULL) {
			delete ast_stmt_seq;
		}
		return false;
	}

	if (ast_stmt_seq != NULL) {
		(*result)->addChild(ast_stmt_seq);
	}

	sym_kind = this->getCurrentSymbol();
	if (sym_kind != SymbolKind::END) {
		return false;
	}
	else {
		(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::END)));

		this->getNextSymbol();

		return true;
	}
}
// caution: even return true, *result may still be NULL
bool Parser::statement(ASTNode **result)
{
	assert(m_scanner != NULL);
	SymbolKind::T_SymbolKind sym_kind = this->getCurrentSymbol();
	if (sym_kind == SymbolKind::ERROR) {
		return false;
	}

	if ((sym_kind == SymbolKind::IDENT) || (sym_kind == SymbolKind::IF) || (sym_kind == SymbolKind::WHILE)) {
		*result = new ASTNode(ASTNodeKind::STATEMENT);

		if (sym_kind == SymbolKind::IDENT) {
			sym_kind = this->getNextSymbol();
			if ((sym_kind == SymbolKind::PERIOD) || (sym_kind == SymbolKind::LBRAK)) {
				ASTNode *ast_selector = NULL;
				if (!this->selector(&ast_selector)) {
					if (ast_selector != NULL) {
						delete ast_selector;
					}
					return false;
				}
				if (ast_selector != NULL) {
					(*result)->addChild(ast_selector);
				}
			}

			if (sym_kind == SymbolKind::BECOMES) {
				(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::BECOMES)));

				this->getNextSymbol();

				ASTNode *ast_exp = NULL;
				if (!this->expression(&ast_exp)) {
					if (ast_exp != NULL) {
						delete ast_exp;
					}
					return false;
				}
				if (ast_exp == NULL) {
					assert(false);
					return false;
				}

				(*result)->addChild(ast_exp);

				return true;
			}
			else if (sym_kind == SymbolKind::LPREN) {
				ASTNode *ast_params = NULL;
				if (!this->actualParameters(&ast_params)) {
					if (ast_params != NULL) {
						delete ast_params;
					}
					return false;
				}
				if (ast_params == NULL) {
					assert(false);
					return false;
				}

				(*result)->addChild(ast_params);

				return true;
			}
			else {
				return true;
			}
		}
		else if (sym_kind == SymbolKind::IF) {
			ASTNode *ast_if = NULL;
			if (!this->ifStatement(&ast_if)) {
				if (ast_if != NULL) {
					delete ast_if;
				}
				return false;
			}
			if (ast_if == NULL) {
				assert(false);
				return false;
			}

			(*result)->addChild(ast_if);

			return true;
		}
		else if (sym_kind == SymbolKind::WHILE) {
			ASTNode *ast_while = NULL;
			if (!this->whileStatement(&ast_while)) {
				if (ast_while != NULL) {
					delete ast_while;
				}
				return false;
			}
			if (ast_while == NULL) {
				assert(false);
				return false;
			}

			(*result)->addChild(ast_while);

			return true;
		}
		else {
			assert(false);
			return false;
		}
	}
	else {
		return true;
	}
}
bool Parser::statementSequence(ASTNode **result)
{
	assert(m_scanner != NULL);
	SymbolKind::T_SymbolKind sym_kind = this->getCurrentSymbol();
	if (sym_kind == SymbolKind::ERROR) {
		return false;
	}

	*result = new ASTNode(ASTNodeKind::STATEMENT_SEQUENCE);

	ASTNode *ast_stmt = NULL;
	if (!this->statement(&ast_stmt)) {
		if (ast_stmt != NULL) {
			delete ast_stmt;
		}
		return false;
	}

	if (ast_stmt != NULL) {
		(*result)->addChild(ast_stmt);
	}

	sym_kind = this->getCurrentSymbol();

	while (sym_kind == SymbolKind::SEMICOLON) {
		(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::SEMICOLON)));

		this->getNextSymbol();

		if (!this->statement(&ast_stmt)) {
			if (ast_stmt != NULL) {
				delete ast_stmt;
			}
			return false;
		}

		if (ast_stmt != NULL) {
			(*result)->addChild(ast_stmt);
		}

		sym_kind = this->getCurrentSymbol();
	}

	return true;
}

bool Parser::identList(ASTNode **result)
{
	assert(m_scanner != NULL);
	SymbolKind::T_SymbolKind sym_kind = this->getCurrentSymbol();
	if (sym_kind != SymbolKind::IDENT) {
		return false;
	}

	*result = new ASTNode(ASTNodeKind::IDENT_LIST);
	(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::IDENT)));

	sym_kind = this->getNextSymbol();
	while (sym_kind == SymbolKind::COMMA) {
		(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::COMMA)));

		sym_kind = this->getNextSymbol();
		if (sym_kind != SymbolKind::IDENT) {
			return false;
		}

		(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::IDENT)));

		sym_kind = this->getNextSymbol();
	}

	return true;
}
bool Parser::arrayType(ASTNode **result)
{
	assert(m_scanner != NULL);
	SymbolKind::T_SymbolKind sym_kind = this->getCurrentSymbol();
	if (sym_kind != SymbolKind::ARRAY) {
		return false;
	}

	*result = new ASTNode(ASTNodeKind::ARRAY_TYPE);
	(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::ARRAY)));

	this->getNextSymbol();

	ASTNode *ast_exp = NULL;
	if (!this->expression(&ast_exp)) {
		if (ast_exp != NULL) {
			delete ast_exp;
		}
		return false;
	}
	if (ast_exp == NULL) {
		assert(false);
		return false;
	}
	(*result)->addChild(ast_exp);

	sym_kind = this->getCurrentSymbol();
	if (sym_kind != SymbolKind::OF) {
		return false;
	}
	(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::OF)));

	this->getNextSymbol();

	ASTNode *ast_type = NULL;
	if (!this->type(&ast_type)) {
		if (ast_type != NULL) {
			delete ast_type;
		}
		return false;
	}
	if (ast_type == NULL) {
		assert(false);
		return false;
	}

	(*result)->addChild(ast_type);

	return true;
}
// caution: even return true, *result may still be NULL
bool Parser::fieldList(ASTNode **result)
{
	assert(m_scanner != NULL);
	SymbolKind::T_SymbolKind sym_kind = this->getCurrentSymbol();
	if (sym_kind == SymbolKind::ERROR) {
		return false;
	}
	else if (sym_kind == SymbolKind::IDENT) {
		*result = new ASTNode(ASTNodeKind::FIELD_LIST);

		ASTNode *ast_ident_list = NULL;
		if (!this->identList(&ast_ident_list)) {
			if (ast_ident_list != NULL) {
				delete ast_ident_list;
			}
			return false;
		}
		if (ast_ident_list == NULL) {
			assert(false);
			return false;
		}

		(*result)->addChild(ast_ident_list);

		if (sym_kind != SymbolKind::COLON) {
			return false;
		}
		(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::COLON)));

		this->getNextSymbol();

		ASTNode *ast_type = NULL;
		if (!this->type(&ast_type)) {
			if (ast_type != NULL) {
				delete ast_type;
			}
			return false;
		}
		if (ast_type == NULL) {
			assert(false);
			return false;
		}

		(*result)->addChild(ast_type);

		return true;
	}
	else {
		return true;
	}
}
bool Parser::recordType(ASTNode **result)
{
	assert(m_scanner != NULL);
	SymbolKind::T_SymbolKind sym_kind = this->getCurrentSymbol();
	if (sym_kind != SymbolKind::RECORD) {
		return false;
	}

	*result = new ASTNode(ASTNodeKind::RECORD_TYPE);
	(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::RECORD)));

	this->getNextSymbol();

	ASTNode *ast_field_list = NULL;
	if (!this->fieldList(&ast_field_list)) {
		if (ast_field_list != NULL) {
			delete ast_field_list;
		}
		return false;
	}

	if (ast_field_list != NULL) {
		(*result)->addChild(ast_field_list);
	}

	sym_kind = this->getCurrentSymbol();
	while (sym_kind == SymbolKind::SEMICOLON) {
		(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::SEMICOLON)));

		this->getNextSymbol();

		if (!this->fieldList(&ast_field_list)) {
			if (ast_field_list != NULL) {
				delete ast_field_list;
			}
			return false;
		}

		if (ast_field_list != NULL) {
			(*result)->addChild(ast_field_list);
		}

		sym_kind = this->getCurrentSymbol();
	}

	sym_kind = this->getCurrentSymbol();
	if (sym_kind != SymbolKind::END) {
		return false;
	}

	(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::END)));

	return true;
}
bool Parser::type(ASTNode **result)
{
	assert(m_scanner != NULL);
	SymbolKind::T_SymbolKind sym_kind = this->getCurrentSymbol();
	if ((sym_kind == SymbolKind::IDENT) || (sym_kind == SymbolKind::ARRAY) || (sym_kind == SymbolKind::RECORD)) {
		*result = new ASTNode(ASTNodeKind::TYPE);

		if (sym_kind == SymbolKind::IDENT) {
			(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::IDENT)));

			this->getNextSymbol();

			return true;
		}
		else if (sym_kind == SymbolKind::ARRAY) {
			ASTNode *ast_array = NULL;
			if (!this->arrayType(&ast_array)) {
				if (ast_array != NULL) {
					delete ast_array;
				}
				return false;
			}
			if (ast_array == NULL) {
				assert(false);
				return false;
			}

			(*result)->addChild(ast_array);

			return true;
		}
		else if (sym_kind == SymbolKind::RECORD) {
			ASTNode *ast_record = NULL;
			if (!this->arrayType(&ast_record)) {
				if (ast_record != NULL) {
					delete ast_record;
				}
				return false;
			}
			if (ast_record == NULL) {
				assert(false);
				return false;
			}

			(*result)->addChild(ast_record);

			return true;
		}
		else {
			assert(false);
			return false;
		}
	}
	else {  
		return false;
	}
}
bool Parser::fpSection(ASTNode **result)
{
	assert(m_scanner != NULL);
	SymbolKind::T_SymbolKind sym_kind = this->getCurrentSymbol();

	if ((sym_kind == SymbolKind::VAR) || (sym_kind == SymbolKind::IDENT)) {
		if (sym_kind == SymbolKind::VAR) {
			(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::VAR)));

			sym_kind = this->getNextSymbol();
		}

		if (sym_kind == SymbolKind::IDENT) {
			ASTNode *ast_ident_list = NULL;
			if (!this->identList(&ast_ident_list)) {
				if (ast_ident_list != NULL) {
					delete ast_ident_list;
				}
				return false;
			}
			if (ast_ident_list == NULL) {
				assert(false);
				return false;
			}

			(*result)->addChild(ast_ident_list);

			if (sym_kind != SymbolKind::COLON) {
				return false;
			}

			ASTNode *ast_type = NULL;
			if (!this->type(&ast_type)) {
				if (ast_type != NULL) {
					delete ast_type;
				}
				return false;
			}
			if (ast_type == NULL) {
				assert(false);
				return false;
			}

			(*result)->addChild(ast_type);

			return true;
		}
		else {
			assert(false);
			return false;
		}
	}
	else {
		return false;
	}
}
bool Parser::formalParameters(ASTNode **result)
{
	assert(m_scanner != NULL);
	SymbolKind::T_SymbolKind sym_kind = this->getCurrentSymbol();
	if (sym_kind != SymbolKind::LPREN) {
		return false;
	}

	*result = new ASTNode(ASTNodeKind::FORMAL_PARAMETERS);
	(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::LPREN)));

	sym_kind = this->getNextSymbol();
	if ((sym_kind == SymbolKind::VAR) || (sym_kind == SymbolKind::IDENT)) {
		ASTNode *ast_fp_section = NULL;
		if (!this->fpSection(&ast_fp_section)) {
			if (ast_fp_section != NULL) {
				delete ast_fp_section;
			}
			return false;
		}
		if (ast_fp_section == NULL) {
			assert(false);
			return false;
		}

		(*result)->addChild(ast_fp_section);
	}

	sym_kind = this->getCurrentSymbol();
	while (sym_kind == SymbolKind::SEMICOLON) {
		(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::SEMICOLON)));

		this->getNextSymbol();

		ASTNode *ast_fp_section = NULL;
		if (!this->fpSection(&ast_fp_section)) {
			if (ast_fp_section != NULL) {
				delete ast_fp_section;
			}
			return false;
		}
		if (ast_fp_section == NULL) {
			assert(false);
			return false;
		}

		(*result)->addChild(ast_fp_section);

		sym_kind = this->getCurrentSymbol();
	}

	if (sym_kind != SymbolKind::RPAREN) {
		return false;
	}

	(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::RPAREN)));

	this->getNextSymbol();

	return true;
}
bool Parser::procedureHeading(ASTNode **result)
{
	assert(m_scanner != NULL);
	SymbolKind::T_SymbolKind sym_kind = this->getCurrentSymbol();
	if (sym_kind != SymbolKind::PROCEDURE) {
		return false;
	}

	*result = new ASTNode(ASTNodeKind::PROCEDURE_HEADING);
	(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::PROCEDURE)));

	sym_kind = this->getNextSymbol();
	if (sym_kind != SymbolKind::IDENT) {
		return false;
	}
	(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::IDENT)));

	sym_kind = this->getNextSymbol();
	if (sym_kind == SymbolKind::LPREN) {
		ASTNode *ast_params = NULL;
		if (!this->formalParameters(&ast_params)) {
			if (ast_params != NULL) {
				delete ast_params;
			}
			return false;
		}
		if (ast_params == NULL) {
			assert(false);
			return false;
		}
		(*result)->addChild(ast_params);

		return true;
	}
	else {  
		return true;
	}
}
bool Parser::procedureBody(ASTNode **result)
{
	assert(m_scanner != NULL);
	SymbolKind::T_SymbolKind sym_kind = this->getCurrentSymbol();
	if (sym_kind == SymbolKind::ERROR) {
		return false;
	}
	*result = new ASTNode(ASTNodeKind::PROCEDURE_BODY);

	ASTNode *ast_declarations = NULL;
	if (!this->declarations(&ast_declarations)) {
		if (ast_declarations != NULL) {
			delete ast_declarations;
		}
		return false;
	}

	if (ast_declarations != NULL) {
		(*result)->addChild(ast_declarations);
	}

	sym_kind = this->getCurrentSymbol();
	if (sym_kind == SymbolKind::BEGIN) {
		(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::BEGIN)));

		this->getNextSymbol();

		ASTNode *ast_stmt_seq = NULL;
		if (!this->statementSequence(&ast_stmt_seq)) {
			if (ast_stmt_seq != NULL) {
				delete ast_stmt_seq;
			}
			return false;
		}

		if (ast_stmt_seq != NULL) {
			(*result)->addChild(ast_stmt_seq);
		}
	}

	sym_kind = this->getCurrentSymbol();
	if (sym_kind != SymbolKind::END) {
		return false;
	}
	(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::END)));

	sym_kind = this->getNextSymbol();
	if (sym_kind != SymbolKind::IDENT) {
		return false;
	}
	(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::IDENT)));

	this->getNextSymbol();

	return true;
}
bool Parser::procedureDeclaration(ASTNode **result)
{
	assert(m_scanner != NULL);
	SymbolKind::T_SymbolKind sym_kind = this->getCurrentSymbol();
	if (sym_kind != SymbolKind::PROCEDURE) {
		return false;
	}
	*result = new ASTNode(ASTNodeKind::PROCEDURE_DECLARATION);

	ASTNode *ast_proc_head = NULL;
	if (!this->procedureHeading(&ast_proc_head)) {
		if (ast_proc_head != NULL) {
			delete ast_proc_head;
		}
		return false;
	}
	if (ast_proc_head == NULL) {
		assert(false);
		return false;
	}
	(*result)->addChild(ast_proc_head);

	sym_kind = this->getCurrentSymbol();
	if (sym_kind != SymbolKind::SEMICOLON) {
		return false;
	}
	(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::SEMICOLON)));

	this->getNextSymbol();

	ASTNode *ast_proc_body = NULL;
	if (!this->procedureBody(&ast_proc_body)) {
		if (ast_proc_body != NULL) {
			delete ast_proc_body;
		}
		return false;
	}
	if (ast_proc_body == NULL) {
		assert(false);
		return false;
	}
	(*result)->addChild(ast_proc_body);

	return true;
}
// caution: even return true, *result may still be NULL
bool Parser::declarations(ASTNode **result)
{
	assert(m_scanner != NULL);
	SymbolKind::T_SymbolKind sym_kind = this->getCurrentSymbol();

	if (sym_kind == SymbolKind::ERROR) {
		return false;
	}
	else if ((sym_kind == SymbolKind::CONST) || (sym_kind == SymbolKind::TYPE) ||
		(sym_kind == SymbolKind::VAR) || (sym_kind == SymbolKind::PROCEDURE)) {
			*result = new ASTNode(ASTNodeKind::DECLATIONS);

			if (sym_kind == SymbolKind::CONST) {
				(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::CONST)));

				sym_kind = this->getNextSymbol();
				while (sym_kind == SymbolKind::IDENT) {
					(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::IDENT)));

					sym_kind = this->getNextSymbol();
					if (sym_kind != SymbolKind::EQL) {
						return false;
					}
					(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::EQL)));

					ASTNode *ast_exp = NULL;
					if (!this->expression(&ast_exp)) {
						if (ast_exp != NULL) {
							delete ast_exp;
						}
						return false;
					}
					if (ast_exp == NULL) {
						assert(false);
						return false;
					}
					(*result)->addChild(ast_exp);

					sym_kind = this->getCurrentSymbol();
					if (sym_kind != SymbolKind::SEMICOLON) {
						return false;
					}  
					(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::SEMICOLON)));

					this->getNextSymbol();
				}
			}

			if (sym_kind == SymbolKind::TYPE) {
				(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::TYPE)));

				sym_kind = this->getNextSymbol();
				while (sym_kind == SymbolKind::IDENT) {
					(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::IDENT)));

					sym_kind = this->getNextSymbol();
					if (sym_kind != SymbolKind::EQL) {
						return false;
					}
					(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::EQL)));

					ASTNode *ast_type = NULL;
					if (!this->type(&ast_type)) {
						if (ast_type != NULL) {
							delete ast_type;
						}
						return false;
					}
					if (ast_type == NULL) {
						assert(false);
						return false;
					}
					(*result)->addChild(ast_type);

					sym_kind = this->getCurrentSymbol();
					if (sym_kind != SymbolKind::SEMICOLON) {
						return false;
					}
					(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::SEMICOLON)));

					this->getNextSymbol();
				}
			}

			if (sym_kind == SymbolKind::VAR) {
				(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::VAR)));

				sym_kind = this->getNextSymbol();
				while (sym_kind == SymbolKind::IDENT) {
					ASTNode *ast_ident_list = NULL;
					if (!this->identList(&ast_ident_list)) {
						if (ast_ident_list != NULL) {
							delete ast_ident_list;
						}
						return false;
					}
					if (ast_ident_list == NULL) {
						assert(false);
						return false;
					}
					(*result)->addChild(ast_ident_list);

					sym_kind = this->getCurrentSymbol();
					if (sym_kind != SymbolKind::COLON) {
						return false;
					}
					(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::COLON)));

					this->getNextSymbol();

					ASTNode *ast_type = NULL;
					if (!this->type(&ast_type)) {
						if (ast_type != NULL) {
							delete ast_type;
						}
						return false;
					}
					if (ast_type == NULL) {
						assert(false);
						return false;
					}
					(*result)->addChild(ast_type);

					sym_kind = this->getCurrentSymbol();
					if (sym_kind != SymbolKind::SEMICOLON) {
						return false;
					}
					(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::SEMICOLON)));

					sym_kind = this->getNextSymbol();
				}
			}

			while (sym_kind == SymbolKind::PROCEDURE) {
				ASTNode *ast_proc_decl = NULL;
				if (!this->procedureDeclaration(&ast_proc_decl)) {
					if (ast_proc_decl != NULL) {
						delete ast_proc_decl;
					}
					return false;
				}
				if (ast_proc_decl == NULL) {
					assert(false);
					return false;
				}
				(*result)->addChild(ast_proc_decl);

				sym_kind = this->getCurrentSymbol();
				if (sym_kind != SymbolKind::SEMICOLON) {
					return false;
				}
				(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::SEMICOLON)));

				sym_kind = this->getNextSymbol();
			}

			return true;
	}
	else {
		return true;
	}
}
bool Parser::module(ASTNode **result)
{
	assert(m_scanner != NULL);
	SymbolKind::T_SymbolKind sym_kind = this->getCurrentSymbol();
	if (sym_kind != SymbolKind::MODULE) {
		return false;
	}

	*result = new ASTNode(ASTNodeKind::MODULE);
	(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::MODULE)));

	sym_kind = this->getNextSymbol();
	if (sym_kind != SymbolKind::IDENT) {
		return false;
	}

	(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::IDENT)));

	sym_kind = this->getNextSymbol();
	if (sym_kind != SymbolKind::SEMICOLON) {
		return false;
	}
	(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::SEMICOLON)));


	sym_kind = this->getNextSymbol();
	ASTNode *ast_decls = NULL;
	if (!this->declarations(&ast_decls)) {
		if (ast_decls != NULL) {
			delete ast_decls;
		}
		return false;
	}

	if (ast_decls != NULL) {
		(*result)->addChild(ast_decls);
	}

	sym_kind = this->getCurrentSymbol();
	if (sym_kind == SymbolKind::BEGIN) {
		(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::BEGIN)));

		this->getNextSymbol();

		ASTNode *ast_stmt_seq = NULL;
		if (!this->statementSequence(&ast_stmt_seq)) {
			if (ast_stmt_seq != NULL) {
				delete ast_stmt_seq;
			}
			return false;
		}

		if (ast_stmt_seq != NULL) {
			(*result)->addChild(ast_stmt_seq);
		}
	}

	sym_kind = this->getCurrentSymbol();
	if (sym_kind != SymbolKind::END) {
		return false;
	}
	(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::END)));

	sym_kind = this->getNextSymbol();
	if (sym_kind != SymbolKind::IDENT) {
		return false;
	}
	(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::IDENT)));

	sym_kind = this->getNextSymbol();
	if (sym_kind != SymbolKind::PERIOD) {
		return false;
	}
	(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::PERIOD)));

	return true;
}

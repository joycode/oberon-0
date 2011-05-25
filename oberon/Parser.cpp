#include <cassert>
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

SymbolObject *Parser::parse(string filePath)
{
	assert(m_scanner != NULL);
	return NULL;
}

bool Parser::selector(ASTNode **result)
{
	assert(m_scanner != NULL);
	SymbolKind::T_SymbolKind sym_kind = m_scanner->getCurrentSymbolKind();
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
			sym_kind = m_scanner->get();
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

			sym_kind = m_scanner->get();

			continue;
		}
		else if (sym_kind == SymbolKind::LBRAK) {
			sym_kind = m_scanner->get();
			if ((sym_kind == SymbolKind::ERROR) || (sym_kind == SymbolKind::END_OF_FILE)) {
				// TODO. error handling
				return false;
			}

			ASTNode **exp = NULL;
			if (!this->expression(exp)) {
				return false;
			}
			assert(*exp != NULL);

			sym_kind = m_scanner->getCurrentSymbolKind();
			if (sym_kind != SymbolKind::RBRAK) {
				// TODO. error handling
				return false;
			}

			*current_node = new ASTNode(ASTNodeKind::SELECTOR);
			(*current_node)->addChild(new ASTNode(new SymbolObject(SymbolKind::LBRAK)));
			(*current_node)->addChild(*exp);
			(*current_node)->addChild(new ASTNode(new SymbolObject(SymbolKind::RBRAK)));

			if (*last_node != NULL) {
				(*last_node)->addChild(*current_node);
			}
			*last_node = *current_node;

			sym_kind = m_scanner->get();

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
	SymbolKind::T_SymbolKind sym_kind = m_scanner->getCurrentSymbolKind();
	if (sym_kind == SymbolKind::ERROR) {
		return false;
	}

	*result = new ASTNode(ASTNodeKind::FACTOR);

	if (sym_kind == SymbolKind::ERROR) {
		return false;
	}
	else if (sym_kind == SymbolKind::IDENT) {
		sym_kind = m_scanner->get();
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

		m_scanner->get();

		return true;
	}
	else if (sym_kind == SymbolKind::LBRAK) {
		sym_kind = m_scanner->get();
		if ((sym_kind == SymbolKind::ERROR) || (sym_kind == SymbolKind::END_OF_FILE)) {
			// TODO. error handling
			return false;
		}

		ASTNode *ast_exp = NULL;
		if (!this->expression(&ast_exp)) {
			return false;
		}
		assert(ast_exp != NULL);

		sym_kind = m_scanner->getCurrentSymbolKind();
		if (sym_kind != SymbolKind::RBRAK) {
			// TODO. error handling
			return false;
		}

		(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::LBRAK)));
		(*result)->addChild(ast_exp);
		(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::RBRAK)));

		sym_kind = m_scanner->get();

		return true;
	}
	else if (sym_kind == SymbolKind::NOT) {
		sym_kind = m_scanner->get();
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

	return false;
}
bool Parser::term(ASTNode **result)
{
	assert(m_scanner != NULL);

	*result = new ASTNode(ASTNodeKind::TERM);
	
	ASTNode *ast_factor = NULL;
	if (!this->factor(&ast_factor)) {
		return false;
	}
	assert(ast_factor != NULL);

	(*result)->addChild(ast_factor);

	SymbolKind::T_SymbolKind sym_kind = m_scanner->getCurrentSymbolKind();
	if (sym_kind == SymbolKind::ERROR) {
		return false;
	}
	else if ((sym_kind == SymbolKind::TIMES) || (sym_kind == SymbolKind::TIMES) || 
		(sym_kind == SymbolKind::TIMES) || (sym_kind == SymbolKind::TIMES)) {
			sym_kind = m_scanner->get();
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

			return true;
	}
	else {
		return true;
	}
}
bool Parser::simpleExpression(ASTNode **result)
{
	assert(m_scanner != NULL);
	SymbolKind::T_SymbolKind sym_kind = m_scanner->getCurrentSymbolKind();
	if (sym_kind == SymbolKind::ERROR) {
		return false;
	}

	*result = new ASTNode(ASTNodeKind::SIMPLE_EXPRESSION);

	if ((sym_kind == SymbolKind::PLUS) || (sym_kind == SymbolKind::MINUS)) {
		(*result)->addChild(new ASTNode(new SymbolObject(sym_kind)));
		sym_kind = m_scanner->get();
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

	if ((sym_kind == SymbolKind::PLUS) || (sym_kind == SymbolKind::MINUS) || (sym_kind == SymbolKind::OR)) {
		(*result)->addChild(new ASTNode(new SymbolObject(sym_kind)));

		sym_kind = m_scanner->get();
		if (!this->term(&ast_term)) {
			return false;
		}

		assert(ast_term != NULL);
		(*result)->addChild(ast_term);

		return true;
	}
	else {
		return true;
	}
}
bool Parser::expression(ASTNode **result)
{
	assert(m_scanner != NULL);
	SymbolKind::T_SymbolKind sym_kind = m_scanner->getCurrentSymbolKind();
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

	if (sym_kind == SymbolKind::ERROR) {
		return false;
	}
	else if ((sym_kind == SymbolKind::EQL) || (sym_kind == SymbolKind::NEQ) ||
		(sym_kind == SymbolKind::LSS) || (sym_kind == SymbolKind::LEQ) ||
		(sym_kind == SymbolKind::GTR) || (sym_kind == SymbolKind::GEQ)) {
			(*result)->addChild(new ASTNode(new SymbolObject(sym_kind)));

			sym_kind = m_scanner->get();
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
	SymbolKind::T_SymbolKind sym_kind = m_scanner->getCurrentSymbolKind();
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

	sym_kind = m_scanner->get();
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
	SymbolKind::T_SymbolKind sym_kind = m_scanner->getCurrentSymbolKind();

	if (sym_kind != SymbolKind::LPREN) {
		return false;
	}

	*result = new ASTNode(ASTNodeKind::ACTUAL_PARAMETERS);

	(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::LPREN)));

	sym_kind = m_scanner->get();
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
			sym_kind = m_scanner->get();
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

	if (sym_kind != SymbolKind::RPAREN) {
		// TODO. error handling
		return false;
	}

	(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::RPAREN)));

	m_scanner->get();

	return true;
}
bool Parser::procedureCall(ASTNode **result)
{
	assert(m_scanner != NULL);
	SymbolKind::T_SymbolKind sym_kind = m_scanner->getCurrentSymbolKind();
	if (sym_kind != SymbolKind::IDENT) {
		return false;
	}

	*result = new ASTNode(ASTNodeKind::PROCEDURE_CALL);

	(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::IDENT)));

	sym_kind = m_scanner->get();
	if ((sym_kind == SymbolKind::PERIOD) || (sym_kind == SymbolKind::LBRAK)) {
		ASTNode *ast_selector = NULL;
		if (!this->selector(&ast_selector)) {
			return false;
		}
		assert(ast_selector != NULL);

		(*result)->addChild(ast_selector);
	}

	if (sym_kind == SymbolKind::ERROR) {
		return false;
	}
	else if (sym_kind == SymbolKind::LPREN) {
		ASTNode *ast_params = NULL;
		if (!this->actualParameters(&ast_params)) {
			return false;
		}
		assert(ast_params != NULL);

		(*result)->addChild(ast_params);
	}
	else {
		return true;
	}
}
bool Parser::ifStatement(ASTNode **result)
{
	assert(m_scanner != NULL);
	SymbolKind::T_SymbolKind sym_kind = m_scanner->getCurrentSymbolKind();
	if (sym_kind != SymbolKind::IF) {
		return false;
	}

	*result = new ASTNode(ASTNodeKind::IF_STATEMENT);
	(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::IF)));

	sym_kind = m_scanner->get();

	ASTNode *ast_exp = NULL;
	if (!this->expression(&ast_exp)) {
		return false;
	}
	if (ast_exp == NULL) {
		assert(false);
		return false;
	}

	(*result)->addChild(ast_exp);

	sym_kind = m_scanner->getCurrentSymbolKind();
	if (sym_kind != SymbolKind::THEN) {
		return false;
	}

	(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::THEN)));

	ASTNode *ast_statement_seq = NULL;

	sym_kind = m_scanner->get();
	if (!this->statementSequence(&ast_statement_seq)) {
		return false;
	}
	assert(ast_statement_seq != NULL);

	sym_kind = m_scanner->getCurrentSymbolKind();
	while (sym_kind == SymbolKind::ELSEIF) {
		(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::ELSEIF)));

		sym_kind = m_scanner->get();
		if (!this->expression(&ast_exp)) {
			return false;
		}
		if (ast_exp == NULL) {
			assert(false);
			return false;
		}
		(*result)->addChild(ast_exp);
		
		sym_kind = m_scanner->getCurrentSymbolKind();
		if (sym_kind != SymbolKind::THEN) {
			return false;
		}
		(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::THEN)));

		m_scanner->get();
		if (!this->statementSequence(&ast_statement_seq)) {
			return false;
		}
		if (ast_statement_seq == NULL) {
			assert(false);
			return false;
		}
		(*result)->addChild(ast_statement_seq);

		sym_kind = m_scanner->getCurrentSymbolKind();
	}
	
	if (sym_kind == SymbolKind::ELSE) {
		(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::ELSE)));

		sym_kind = m_scanner->get();
		if (!this->expression(&ast_exp)) {
			return false;
		}
		if (ast_exp == NULL) {
			assert(false);
			return false;
		}
		(*result)->addChild(ast_exp);

		sym_kind = m_scanner->getCurrentSymbolKind();
		if (sym_kind != SymbolKind::THEN) {
			return false;
		}
		(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::THEN)));

		m_scanner->get();
		if (!this->statementSequence(&ast_statement_seq)) {
			return false;
		}
		if (ast_statement_seq == NULL) {
			assert(false);
			return false;
		}
		(*result)->addChild(ast_statement_seq);

		sym_kind = m_scanner->getCurrentSymbolKind();
	}
	
	if (sym_kind == SymbolKind::END) {
		(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::END)));

		m_scanner->get();

		return true;
	}
	else {  
		return false;
	}
}
bool Parser::whileStatement(ASTNode **result)
{
	assert(m_scanner != NULL);
	SymbolKind::T_SymbolKind sym_kind = m_scanner->getCurrentSymbolKind();
	if (sym_kind != SymbolKind::WHILE) {
		return false;
	}

	*result = new ASTNode(ASTNodeKind::WHILE_STATEMENT);
	(*result)->addChild(new ASTNode(new SymbolObject(SymbolKind::WHILE)));

	ASTNode *ast_exp = NULL;
	if (!this->expression(&ast_exp)) {
		return false;
	}
	if (ast_exp == NULL) {
		assert(false);
		return false;
	}

	sym_kind = m_scanner->getCurrentSymbolKind();

	return false;
}
bool Parser::statement(ASTNode **result)
{
	assert(m_scanner != NULL);
	return false;
}
bool Parser::statementSequence(ASTNode **result)
{
	assert(m_scanner != NULL);
	return false;
}

bool Parser::identList(ASTNode **result)
{
	assert(m_scanner != NULL);
	return false;
}
bool Parser::arrayType(ASTNode **result)
{
	assert(m_scanner != NULL);
	return false;
}
bool Parser::fieldList(ASTNode **result)
{
	assert(m_scanner != NULL);
	return false;
}
bool Parser::recordType(ASTNode **result)
{
	assert(m_scanner != NULL);
	return false;
}
bool Parser::type(ASTNode **result)
{
	assert(m_scanner != NULL);
	return false;
}
bool Parser::fpSection(ASTNode **result)
{
	assert(m_scanner != NULL);
	return false;
}
bool Parser::formalParameters(ASTNode **result)
{
	assert(m_scanner != NULL);
	return false;
}
bool Parser::procedureHeading(ASTNode **result)
{
	assert(m_scanner != NULL);
	return false;
}
bool Parser::procedureBody(ASTNode **result)
{
	assert(m_scanner != NULL);
	return false;
}
bool Parser::procedureDeclaration(ASTNode **result)
{
	assert(m_scanner != NULL);
	return false;
}
bool Parser::declarations(ASTNode **result)
{
	assert(m_scanner != NULL);
	return false;
}
bool Parser::module(ASTNode **result)
{
	assert(m_scanner != NULL);
	return false;
}

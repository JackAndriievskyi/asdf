#include "Avm.h"

#include <map>
#include <set>
#include <sstream>
#include <exception>

static const std::map<std::string, bool> instructions = {
	{"push",	true },
	{"pop",		false},
	{"dump",	false},
	{"assert",	true },
	{"add",		false},
	{"sub",		false},
	{"mul",		false},
	{"div",		false},
	{"mod",		false},
	{"print",	false},
	{"exit",	false}
};
const std::set<std::string> operands = {
	"int8",
	"int16",
	"int32",
	"float",
	"double"
};

//--------------------------
//--------------------------
class LexicalException : public std::exception
{
public:

	LexicalException(const std::string &, const size_t, const std::string &);
	LexicalException(const LexicalException&) = default;
	LexicalException & operator=(const LexicalException&) = default;

	~LexicalException() throw ();

	const char* what() const throw ();


private:

	std::string msg_;


};

LexicalException::LexicalException(const std::string & line, const size_t line_number, const std::string & token) {

	this->msg_ = "Lexical Error :\n\tLine "
		+ std::to_string(line_number)
		+ ": "
		+ line
		+ ": Expected unqualified-id\n"
		+ token;

}

LexicalException::~LexicalException() throw () {

}

const char* LexicalException::what(void) const throw () {

	return this->msg_.c_str();
}
//-----------------------
//-----------------------

Avm::Avm(std::istream & input, std::ostream & output, std::ostream & log) :
	_input(input),
	_output(output),
	_log(log)
{
	_input.read(nullptr, 0);
	_output.write(nullptr, 0);
	_log.write(nullptr, 0);

	if (_input.good() && _output.good() && _log.good())
		_avm_state = AvmState::GOOD;
	else
		_avm_state = AvmState::BAD;
}

Avm::~Avm() 
{}



void Avm::checkComment(const std::string & line, const size_t line_number, const std::string & token) {

	if (token.size() == 0)
		return;

	if (token[0] != ';')
		throw LexicalException(line, line_number, token); //THROW NOT COMMENT

}

void Avm::checkOperand(const std::string & line, const size_t line_number, const std::string & token) {

	if (token.find('(') == std::string::npos || token.find(')') == std::string::npos)
		throw LexicalException(line, line_number, token); // THROW WRONG OPERAND

	const std::string   operand_type(token.begin(), token.begin() + token.find('('));
	const std::string   operand_value(token.begin() + token.find('('), token.begin() + token.find(')') + 1);

	if (operand_type.size() + operand_value.size() != token.size() || operand_value.size() == 2)
		throw LexicalException(line, line_number, token); // THROW WRONG OPERAND

	if (operands.find(operand_type) == operands.end())
		throw LexicalException(line, line_number, token); // THROW WRONG OPERAND

	std::stringstream   tokenizer;
	tokenizer << std::string(operand_value.begin() + 1, operand_value.end() - 1);
	using type_name = char; //operand_value_name
	type_name	value_token;
	tokenizer >> value_token;

	//proverka znacheniya v value_token i ego tipa

	if (!tokenizer.good()) {
		std::string			should_be_empty;
		tokenizer >> should_be_empty;
		if (!should_be_empty.empty() && !tokenizer.eof())
			throw LexicalException(line, line_number, token); // THROW WRONG OPERAND
	}
	else
		throw LexicalException(line, line_number, token); // THROW WRONG OPERAND
}

void Avm::checkInstruction(const std::string & line, const size_t line_number, const std::string & token)
{
	if (instructions.find(token) == instructions.end())
		throw LexicalException(line, line_number, token); //THROW WRONG INSTRUCTION
}

void Avm::checkLine(const std::string & line, const size_t line_number)
{
	std::stringstream   tokenizer;
	tokenizer << line;
	std::string         token;
	tokenizer >> token;

	try {
		checkComment(line, line_number, token);
	}
	catch (std::exception &e) { //CATCH NOT COMMENT
		try {
			checkInstruction(line, line_number, token);
		}
		catch (std::exception &e) { //CATCH WRONG INSTRUCTION
			throw e;
		}

		if (token == "exit")
			_avm_state = AvmState::EXIT_INSTUCTION;

		const std::string instruction_name = std::move(token);
		tokenizer >> token;

		if ((instructions.find(instruction_name))->second) {
			try {
				checkComment(line, line_number, token);
				throw LexicalException(line, line_number, token); //THROW WRONG OPERANDS COUNT
			}
			catch (std::exception &e) { //CATCH NOT COMMENT
				try {
					checkOperand(line, line_number, token);
				}
				catch (std::exception &e){
					throw e;
				}
			}
			catch (std::exception &e) { //CATCH WRONG OPERANDS COUNT
				throw e;
			}

			tokenizer >> token;
		}

		try {
			checkComment(line, line_number, token);
		}
		catch (std::exception &e) { //CATCH NOT COMMENT
			throw LexicalException(line, line_number, token); //THROW WRONG OPERANDS COUNT
		}
	}
}

void Avm::check(void)
{
	if (_avm_state == AvmState::BAD)
		return;

	std::string         line;
	size_t              line_number;
	AvmState			state;

	state = AvmState::GOOD;
	line_number = 0;
	std::getline(this->_input, line);
	while (this->_input.good() && !this->exitInstruction()) { //dobavit' proverku po koncu texta
		try {
			checkLine(line, ++line_number);
		}
		catch (std::exception &e) {
			state = AvmState::BAD;
			std::cout << e.what() << std::endl; //this->_log.write
		}
		std::getline(this->_input, line);
	}
	if (!this->exitInstruction()) {
		state = AvmState::BAD;
		//this->_log.write NOEXITINSTRUCTION
	}
	else {
		while (this->_input.good()) {
			try {
				std::stringstream   tokenizer;
				tokenizer << line;
				std::string         token;
				tokenizer >> token;

				checkComment(line, line_number, token);
			}
			catch (std::exception &e) { // CATCH NOT COMMENT
				state = AvmState::BAD;
				//thit->_log.write INSTRUCTION OR SOME TEXT AFTER EXIT
			}
			std::getline(this->_input, line);
		}
	}

	_avm_state = state;
	this->_input.clear();
	this->_input.seekg(0); 
}



void Avm::parse(void)
{
	if (_avm_state == AvmState::BAD)
		return;


}



void Avm::result(void)
{
	if (_avm_state == AvmState::BAD)
		return;
}



bool Avm::good(void)
{
	return _avm_state == AvmState::GOOD;
}

bool Avm::bad(void)
{
	return _avm_state == AvmState::BAD;
}

bool Avm::exitInstruction(void)
{
	return _avm_state == AvmState::EXIT_INSTUCTION;
}

#pragma once

#include <iostream>

class Avm
{
public:

	enum AvmState
	{
		GOOD,
		BAD,
		EXIT_INSTUCTION
	};

	Avm(void) = delete;
	Avm(std::istream &, std::ostream & = std::cout, std::ostream & = std::cout);
	Avm(const Avm & rhs) = delete;
	Avm(const Avm && rrhs) = delete;
	Avm & operator=(const Avm &rhs) = delete;
	Avm & operator=(const Avm &&rhs) = delete;
	~Avm();

	void			check(void);
	void			parse(void);
	void			result(void);

	bool			good(void);
	bool			bad(void);

private:

	AvmState		_avm_state;
	bool			exitInstruction(void);

	std::istream	& _input;
	std::ostream	& _output;
	std::ostream	& _log;

	void			checkLine(const std::string &, const size_t);
	void			checkInstruction(const std::string &, const size_t, const std::string &);
	void			checkOperand(const std::string &, const size_t, const std::string &);
	void			checkComment(const std::string &, const size_t, const std::string &);
};
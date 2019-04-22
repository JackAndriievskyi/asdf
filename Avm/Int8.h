#pragma once
#include "IOperand.h"

class Int8 : public IOperand
{
public:
	using	type_name = int8_t;

	Int8();
	explicit Int8(type_name);
	Int8(const Int8 &);
	 
	virtual int getPrecision(void);
	virtual eOperandType getType(void) const;

	virtual IOperand const * operator+(IOperand const &);
	virtual IOperand const * operator-(IOperand const &);
	virtual IOperand const * operator*(IOperand const &);
	virtual IOperand const * operator/(IOperand const &);
	virtual IOperand const * operator%(IOperand const &);

	virtual std::string const & toString(void);

	~Int8();

private:

	type_name			val;
	const std::string	type_string;
};


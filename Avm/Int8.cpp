#include "Int8.h"



Int8::Int8()
{
}

Int8::Int8(char val) :

{
}

int Int8::getPrecision(void)
{
	return static_cast<int>(eOperandType::Int8);
}

eOperandType Int8::getType(void) const
{
	return eOperandType::Int8;
}


std::string const & Int8::toString(void)
{
	return type_string;
}

Int8::~Int8()
{
}

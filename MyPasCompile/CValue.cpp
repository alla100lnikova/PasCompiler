#include "CValue.h"

CValue::CValue(eValueType ValueType)
	:m_ValueType(ValueType)
{
}

CInteger::CInteger(int Value) : CValue(vtInteger)
	,m_Value(Value)
{
}

CReal::CReal(double Value): CValue(vtReal)
	 ,m_Value(Value)
{
}

CChar::CChar(char Value) : CValue(vtChar)
	,m_Value(Value)
{
}

CString::CString(string Value) : CValue(vtString)
	,m_Value(Value)
{
}

CBoolean::CBoolean(bool Value) : CValue(vtBoolean)
	, m_Value(Value)
{
}
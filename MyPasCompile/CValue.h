#pragma once
#include "Constants.h"

class CValue
{
protected:
	eValueType m_ValueType;
public:
	CValue(eValueType ValueType);
	eValueType GetValueType() { return m_ValueType; }
};

class CInteger : public CValue
{
private: 
	int m_Value;
public:
	CInteger(int Value);
	int GetValue() { return m_Value; }
};

class CReal : public CValue
{
private:
	double m_Value;
public:
	CReal(double Value);
	double GetValue() { return m_Value; }
};

class CChar : public CValue
{
private:
	char m_Value;
public:
	CChar(char Value);
	char GetValue() { return m_Value; }
};

class CString : public CValue
{
private:
	string m_Value;
public:
	CString(string Value);
	string GetValue() { return m_Value; }
};


class CBoolean : public CValue
{
private:
	bool m_Value;
public:
	CBoolean(bool Value);
	bool GetValue() { return m_Value; }
};

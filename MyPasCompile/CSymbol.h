#pragma once
#include "CValue.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
using namespace std;

class CSymbol
{
protected:
	string m_SymbolStr;
	eSymbolType SymbolType;

	eCharType GetCharType(char Char);
	char GetNextChar(CharPosition& CurrentPosition, vector<string>& ProgramText);
	void SetSymbolCode();
	bool IsSymbolTypeValid(char CurrentChar, bool IsString);
	string GetStrSymbol(CharPosition& CurrentPosition, vector<string>& ProgramText, bool IsString);

public:
	CSymbol();
	CSymbol(eSymbolType Type);
	virtual ~CSymbol() {};
	string GetSymbol() { return m_SymbolStr; }
	eSymbolType GetSymbolType() { return SymbolType; }
	eOperator SymbolCode;
	virtual CSymbol* ScanSymbol(CharPosition& CurrentPosition, vector<string>& ProgramText);
};

class CValueSymbol : public CSymbol
{
private:
	CValue* m_Value;
	void ScanStrSymbol(CharPosition& CurrentPosition, vector<string>& ProgramText);
	bool ScanDoubleConst(CharPosition& CurrentPosition, vector<string>& ProgramText, char& CurrentChar);
public:
	CValueSymbol();
	CValueSymbol(string BoolValue);
	~CValueSymbol() { delete m_Value; }
	CValue* GetValue(){ return m_Value; }
	CSymbol* ScanSymbol(CharPosition& CurrentPosition, vector<string>& ProgramText);
};

class CIdentSymbol : public CSymbol
{
public:
	CIdentSymbol();
	CSymbol* ScanSymbol(CharPosition& CurrentPosition, vector<string>& ProgramText);
};

class COperatorSymbol : public CSymbol
{
private:
	void ScanDoubleOperator(CharPosition& CurrentPosition, vector<string>& ProgramText);
	void SkipComments(CharPosition& CurrentPosition, vector<string>& ProgramText, char EndChar);
public:
	COperatorSymbol();
	CSymbol* ScanSymbol(CharPosition& CurrentPosition, vector<string>& ProgramText);
};
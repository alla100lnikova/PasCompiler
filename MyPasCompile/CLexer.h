#pragma once
#include "CSymbol.h"

#include <algorithm>
#include <string>
#include <vector>

using namespace std;

class CLexer
{
private:
	vector<string> m_ProgramText;
	CharPosition m_CurrentPosition;
	void Print(string Str, int Index, ofstream& Output);

public:
	CLexer(vector<string> ProgramText);
	CSymbol* GetNextSymbol();
	int GetCurrentStr() 
	{ 
		return m_CurrentPosition.LineNumber;
	}

	void Print();
};
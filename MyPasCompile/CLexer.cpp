#include "CLexer.h"
#include <iostream>
#include <Windows.h>


CLexer::CLexer(vector<string> ProgramText)
	:m_ProgramText(ProgramText),
	m_CurrentPosition { -1, -1 }
{
}

void CLexer::Print(CSymbol* Symbol)
{
	if (Symbol->GetSymbolType() == stValue)
	{
		CValueSymbol* Sym = dynamic_cast<CValueSymbol*>(Symbol);
		if (Sym == nullptr) return;
		CValue* Val = Sym->GetValue();
		
		if (Val == nullptr) return;
		switch (Val->GetValueType())
		{
			case vtInteger:
			{
				CInteger* V = static_cast<CInteger*> (Val);
				cout << V->GetValue() << "\n";
			}
			break;
			case vtReal:
			{
				CReal* V = static_cast<CReal*> (Val);
				cout << V->GetValue() << "\n";
			}
			break;
			default:
				cout << Symbol->GetSymbol() << " = " << Symbol->SymbolCode << "\n";
				break;
		}
	}
	else
	{
		cout << Symbol->GetSymbol() << " = " << Symbol->SymbolCode << "\n";
	}
}

void CLexer::Print(string Str, int Index)
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	cout << Index + 1 << " " << Str << "\n";
	for (auto Err : Constants::ErrorList)
	{
		if (Err.second.LineNumber == Index)
		{
			cout << "** ErrorCode: " << Err.first << " (LineNumber " << Err.second.LineNumber + 1 << ", CharNumber " << Err.second.CharNumber + 1 << ") **\n";
			cout << "** " << Constants::ErrorCodeTable.at(Err.first) << " **\n";
		}
	}
}

void CLexer::Print()
{
	int i = 0;
	for (auto Str : m_ProgramText)
	{
		Print(Str, i++);
	}

	for (auto Err : Constants::ErrorList)
	{
		if (Err.second.LineNumber == m_ProgramText.size())
		{
			cout << "** ErrorCode: " << Err.first << " (LineNumber " << Err.second.LineNumber + 1 << ", CharNumber " << Err.second.CharNumber + 1 << ") **\n";
			cout << "** " << Constants::ErrorCodeTable.at(Err.first) << " **\n";
		}
	}
}

CSymbol* CLexer::GetNextSymbol()
{
	CSymbol* NextSymbol = new CSymbol();
	NextSymbol = NextSymbol->ScanSymbol(m_CurrentPosition, m_ProgramText);
	while (NextSymbol && NextSymbol->SymbolCode == error)
	{
		delete NextSymbol;
		NextSymbol = new CSymbol();
		NextSymbol = NextSymbol->ScanSymbol(m_CurrentPosition, m_ProgramText);
	}
	return NextSymbol;
}
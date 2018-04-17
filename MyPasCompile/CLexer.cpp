#include "CLexer.h"
#include <iostream>
#include <Windows.h>
#include <fstream>

CLexer::CLexer(vector<string> ProgramText)
	:m_ProgramText(ProgramText),
	m_CurrentPosition { -1, -1 }
{
}

void CLexer::Print(string Str, int Index, ofstream& Output)
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	Output << Index + 1 << " " << Str << "\n";
	for (auto Err : Constants::ErrorList)
	{
		if (Err.second.LineNumber == Index)
		{
			Output << "** ErrorCode: " << Err.first << " (LineNumber " << Err.second.LineNumber + 1 << ", CharNumber " << Err.second.CharNumber + 1 << ") **\n";
			Output << "** " << Constants::ErrorCodeTable.at(Err.first) << " **\n";
		}
	}
}

void CLexer::Print()
{
	ofstream Output("Log.txt", ios::app);

	Output << "================================================================================================\n";

	int i = 0;
	for (auto Str : m_ProgramText)
	{
		Print(Str, i++, Output);
	}

	for (auto Err : Constants::ErrorList)
	{
		if (Err.second.LineNumber == m_ProgramText.size())
		{
			Output << "** ErrorCode: " << Err.first << " (LineNumber " << Err.second.LineNumber + 1 << ", CharNumber " << Err.second.CharNumber + 1 << ") **\n";
			Output << "** " << Constants::ErrorCodeTable.at(Err.first) << " **\n";
		}
	}
	Output << "================================================================================================\n";
	Output.close();
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
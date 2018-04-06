#include "CCompiler.h"

void CCompiler::Compile(vector<string> ProgramText)
{
	CLexer* Lexer = new CLexer(ProgramText);
	CParser Parser(Lexer, Lexer->GetNextSymbol());
	//vector<CSymbol*> Symbols;

	

	/*while (auto Symbol = Lexer->GetNextSymbol())
	{
		Symbols.push_back(Symbol);
	}

	for (auto Sym : Symbols)
	{
		delete[] Sym;
	}*/
	delete Lexer;
}
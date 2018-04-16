#include "CCompiler.h"

void CCompiler::Compile(vector<string> ProgramText)
{
	CLexer* Lexer = new CLexer(ProgramText);
	CParser Parser(Lexer, Lexer->GetNextSymbol());
	delete Lexer;
	Lexer = nullptr;
}
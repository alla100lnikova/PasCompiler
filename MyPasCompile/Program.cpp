#include "CCompiler.h"
#include <fstream>

void main()
{
	ifstream Fin("Program.txt");
	vector<string> ProgramText;
	string ProgramLine;
	CCompiler* Compiler = nullptr;

	while (!Fin.eof())
	{
		getline(Fin, ProgramLine);
		ProgramText.push_back(ProgramLine);
	}

	Compiler->Compile(ProgramText);
}
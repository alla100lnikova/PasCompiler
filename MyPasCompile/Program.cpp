#include "CCompiler.h"
#include <fstream>
#include <experimental\filesystem>
#include <Windows.h>

namespace fs = std::experimental::filesystem;

void main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	bool IsAutoTests = false;

	cout << "����� �� ����������? (1 - �� / 0 - ���)\n";
	cin >> IsAutoTests;

	if (IsAutoTests)
	{
		string Directory;
		cout << "������� ����� ����������:\n";
		cin >> Directory;

		int Count = 0;

		cout << "���������� " << Directory << "\n";
		for (auto& File : fs::directory_iterator(Directory))
		{

			ifstream Fin(File.path());
			cout << ++Count << ": " << File.path() << "\n";
			vector<string> ProgramText;
			string ProgramLine;
			CCompiler* Compiler = nullptr;

			while (!Fin.eof())
			{
				getline(Fin, ProgramLine);
				ProgramText.push_back(ProgramLine);
			}

			ofstream Output("Log.txt", ios::app);
			Output << "\n" << File.path() << "\n";
			Output.close();

			Compiler->Compile(ProgramText);
			Constants::ErrorList.clear();
		}

		cout << "����� ������: " << Count << "\n";
	}

	else
	{
		ifstream Fin("Program.txt");
		cout << "Program.txt" << "\n";
		vector<string> ProgramText;
		string ProgramLine;
		CCompiler* Compiler = nullptr;

		while (!Fin.eof())
		{
			getline(Fin, ProgramLine);
			ProgramText.push_back(ProgramLine);
		}
		Compiler->Compile(ProgramText);
		Constants::ErrorList.clear();
	}
}
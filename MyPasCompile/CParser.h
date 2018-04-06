#pragma once
#include "CLexer.h"
#include "CSemantic.h"
#include <vector>
using namespace std;

class CParser
{
private:
	CLexer* Lexer;
	CSemantic* Sem;
	CSymbol* m_pCurrentSymbol;
	bool IsSemicolonError;
	bool LexerGiveMeSymbolBistra() 
	{ 
		m_pCurrentSymbol = Lexer->GetNextSymbol(); 
		return m_pCurrentSymbol != nullptr; 
	}
	void SkipTo(vector<eOperator> SkipSym);
	void AddErrorAndSkip(int ErrorCode, vector<eOperator> SkipSym);
	bool Accept(eOperator Next, bool GetNext = true);

public:
	CParser(CLexer* Lex, CSymbol* CurSym) : Lexer(Lex), m_pCurrentSymbol(CurSym) { Sem = new CSemantic(); Program(); }
	~CParser() {}
	CSymbol* GetCurrentSymbol() { return m_pCurrentSymbol; }

//=======================================================================================
	//Программа
	bool Program();
	//Какой-то список после имени проги
	bool FileList();
	//Блок программы
	bool ProgramBlock();
	//Число без знака
	bool UnsignedNumber();
	//Знак
	bool Sign();
	bool SignedNumber();
	//Раздел описания типов
	bool TypesBlock();
	//Описание типа
	bool TypeDescription();
	//Тип
	CType* Type();
	//Записи
	CRecordType* RecordType();
	//Поля записи
	CRecordType* RecordFields(bool IsRightpar = false);
	//Фиксированная часть записи
	bool FixRecordPart(CRecordType* Rec);
	//Вариантная часть записи
	bool VariantRecordPart(CRecordType* Rec);
	bool Variant(CRecordType* Rec);
	bool VariantList(CRecordType* Rec);
	//Секция записи
	CType* RecordSection(vector<string>& Vars);
	//Список меток варианта
	vector<CValue*> VariantLableList();
	//Метка варианта
	bool VariantLable();
	//Раздел описания переменных
	bool VarBlock();
	//Описание однотипных переменных
	bool OneTypeVar();
	//Раздел операторов
	bool OperatorBlock();
	bool Operator();
	bool SimpleOperator();
	//Сложный оператор
	bool ComplexOperator();
	//Составной оператор
	bool CompositeOperator();
	//Оператор присваивания
	bool AssignOperator();
	CType* Variable(bool IsRecordField = false);
	CType* FullVariable(bool IsRecordField);
	//Переменная - поле записи
	CType* VariableRecordField();
	//Выражение
	bool Expression();
	//Операция отношения
	bool RelationshipOperation();
	bool SimpleExpression();
	bool AddOperation();
	//Слагаемое
	bool Term();
	bool MultOperation();
	//Множитель
	bool Multiplier();
	//Константа в выражении - число, переменная и т.п
	bool Constant();
	bool CycleOperator();
	bool ConditionOperator();
	//Оператор присоединения
	bool WithOperator();
	//Список переменных-записей для оператора присоединения
	bool RecordVarList();
};

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

	vector<bool> InWithOperator;
	vector<pair<int, CRecordType*> > WithRec;

	bool LexerGiveMeSymbolBistra() 
	{ 
		delete m_pCurrentSymbol;
		m_pCurrentSymbol = Lexer->GetNextSymbol(); 
		return m_pCurrentSymbol != nullptr; 
	}
	void SkipTo(vector<eOperator> SkipSym);
	void AddErrorAndSkip(int ErrorCode, vector<eOperator> SkipSym);
	bool Accept(eOperator Next, bool GetNext = true);

public:
	CParser(CLexer* Lex, CSymbol* CurSym) : Lexer(Lex), m_pCurrentSymbol(CurSym), InWithOperator(false) 
	{ 
		Sem = new CSemantic(); 
		Program(); 
	}
	~CParser() { delete Sem; delete m_pCurrentSymbol; }
	CSymbol* GetCurrentSymbol() { return m_pCurrentSymbol; }

//=======================================================================================
	//Программа
	bool Program();
	//Какой-то список после имени проги
	bool FileList();
	//Блок программы
	bool ProgramBlock();
	//Число без знака
	CType* UnsignedNumber();
	//Знак
	bool Sign();
	CType* SignedNumber();
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
	vector<CValue*> VariantLableList(CType* FlagType);
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
	CType* Variable(CRecordType* Rec = nullptr);
	CType* FullVariable(CRecordType* Rec = nullptr);
	//Переменная - поле записи
	CType* VariableRecordField(CRecordType* Rec = nullptr);
	//Выражение
	CType* Expression();
	//Операция отношения
	bool RelationshipOperation();
	CType* SimpleExpression();
	bool AddOperation();
	bool AddBoolOperation();
	//Слагаемое
	CType* Term();
	bool MultOperation();
	bool MultIntOperation();
	bool MultBoolOperation();
	//Множитель
	CType* Multiplier();
	//Константа в выражении - число, переменная и т.п
	CType* Constant();
	bool CycleOperator();
	bool ConditionOperator();
	//Оператор присоединения
	bool WithOperator();
	//Список переменных-записей для оператора присоединения
	bool RecordVarList();
};

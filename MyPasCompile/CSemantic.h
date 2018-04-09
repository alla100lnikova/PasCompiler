#pragma once
#include "Constants.h"
#include "CIdent.h"

class CSemantic
{
private:
	vector<string> m_TempVar;

	CType* GetIdentType(CSymbol* Ident);

public:
	CSemantic();
	~CSemantic();
	map<string, CIdent*> IdentMap;

//	void SetOldSym(CSymbol* Old) { m_OldSym = Old; }
	void ClearTempVar() { m_TempVar.clear(); }

	//Добавление типов в мапу / выдача ошибки о повторном
	void CheckScalarType(CSymbol* Symbol, CType* BaseType, int StrNum);
	//Проставление типа переменных /добавление их всех в мапу / ошибка о неописанном типе / повторное описание
	void SetVarsType(CType* TypeName, int StrNum, bool IsRecordFields = false);
	void SetRecFieldType(CType* Type, vector<string>& Vars, CRecordType* Rec, int StrNum);
	//Проверка полей записи / запись ли переменная
	bool CheckRecordFields(CRecordType* Rec, CSymbol* Symbol, int StrNum);
	//Проверка выражения в if / while
	void CheckBoolExpType(CSymbol* Symbol, int StrNum);
	//Проверка вар. части
	void CheckVarRecordPartType(CRecordType* Rec, CRecordType* ChildRec, vector<CValue*> Val, int StrNum);
	//Поиск переменной в мапе
	CIdent* CheckDescription(CSymbol* Symbol, int StrNum, eUseType UseType);
	void AddFlagVarPart(CRecordType* Rec, CSymbol* Symdol, int StrNum);

	//Сбор переменных до типа
	void AddVar(CSymbol* Symbol, int StrNum);
	void AddCheckRecord(CSymbol* Symbol, int StrNum);
	void SetProgName(CSymbol* Symbol, int StrNum);
	CType* GetType(CSymbol* Symbol);
	CType* GetBaseType(CType* Type);
	CType* Cast(CType* One, CType* Two, bool IsAssign = false);
	CType* GetTypeFromMap(string Name) { return (CType*) IdentMap[Name]; }
};
#pragma once
#include "Constants.h"
#include "CIdent.h"
#include "CScope.h"

class CSemantic
{
private:
	vector<string> m_TempVar;
	CScope* m_CurrentScope;
	CScope* m_BaseScope;

private:
	CType* GetIdentType(CSymbol* Ident);

public:
	CSemantic();
	~CSemantic();

	void ClearTempVar() { m_TempVar.clear(); }

	//���������� ����� � ���� / ������ ������ � ���������
	void AddCustomType(string Symbol, CType* BaseType, int StrNum);
	//������������ ���� ���������� /���������� �� ���� � ���� / ������ � ����������� ���� / ��������� ��������
	void SetVarsType(CType* TypeName, int StrNum);
	void SetRecFieldType(CType* Type, vector<string>& Vars, CRecordType* Rec, int StrNum);
	//�������� ����� ������ / ������ �� ����������
	CType* CheckRecordFields(CRecordType* Rec, CSymbol* Symbol, int StrNum);
	CType* CheckWithRecords(vector<pair<int, CRecordType*> >& WithRec, CSymbol* Name, int StrNum);
	//�������� ���. �����
	void CheckVarRecordPartType(CRecordType* Rec, CRecordType* ChildRec, vector<CValue*> Val, int StrNum);
	//����� ���������� � ����
	CIdent* CheckDescription(CSymbol* Symbol, int StrNum, eUseType UseType);
	void AddFlagVarPart(CRecordType* Rec, CSymbol* Symdol, int StrNum);

	//���� ���������� �� ����
	void AddVar(CSymbol* Symbol, int StrNum);
	void SetProgName(CSymbol* Symbol, int StrNum);
	CType* GetType(CSymbol* Symbol);
	CType* GetBaseType(CType* Type);
	CType* Cast(CType* One, CType* Two, bool IsAssign = false);
	CType* GetTypeFromMap(string Name) { return (CType*) m_CurrentScope->GetIdent(Name); }

	eValueType GetValType(CType* Type);
};
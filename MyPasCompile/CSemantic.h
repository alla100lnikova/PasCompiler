#pragma once
#include "Constants.h"
#include "CIdent.h"

class CSemantic
{
private:
	vector<string> m_TempVar;

	CType* GetIdentType(CSymbol* Ident);

	bool AddOp(eOperator op)
	{
		return op == plussy || minussy == op;
	}

	bool MultOperation(eOperator op)
	{
		return op == star
			|| op == slash
			|| op == divsy
			|| op == modsy;
	}

	bool RelationshipOperation(eOperator op)
	{
		return op == equalsy
			|| op == greater
			|| op == greaterequal
			|| op == later
			|| op == laterequal
			|| op == latergreater;
	}

	bool BoolOperation(eOperator op)
	{
		return  op == andsy || op == orsy || op == notsy;
	}

	bool IsConstant(eOperator op)
	{
		return op == intc
			|| op == floatc
			|| op == nilsy
			|| op == charc
			|| op == stringc;
	}

public:
	CSemantic();
	~CSemantic();
	map<string, CIdent*> IdentMap;

//	void SetOldSym(CSymbol* Old) { m_OldSym = Old; }
	void ClearTempVar() { m_TempVar.clear(); }

	//���������� ����� � ���� / ������ ������ � ���������
	void CheckScalarType(CSymbol* Symbol, CType* BaseType, int StrNum);
	//������������ ���� ���������� /���������� �� ���� � ���� / ������ � ����������� ���� / ��������� ��������
	void SetVarsType(CSymbol* TypeName, int StrNum, bool IsRecordFields = false);
	void SetRecFieldType(CType* Type, vector<string> Vars, CRecordType* Rec, int StrNum);
	//�������� ������������ ������������� �������� �������� ����������
	void CheckAssignType(CSymbol* Symbol, int StrNum);
	//�������� + -
	void CheckAddOperandType(CSymbol* Symbol, int StrNum);
	void CheckSubOper(CSymbol* Symbol, int StrNum); // �� ������ ���� ��������� � ������! ���� ������� � �������?
	//�������� * / � ������� �� 0
	void CheckMultOperandType(CSymbol* Symbol, int StrNum);
	//�������� div, mod
	void CheckDivModOperandType(CSymbol* Symbol, int StrNum);
	//�������� �������� ���������
	void CheckRelatOperandType(CSymbol* Symbol, int StrNum);
	//�������� ����� ������ / ������ �� ����������
	bool CheckRecordFields(CRecordType* Rec, CSymbol* Symbol, int StrNum);
	//�������� ��������� � if / while
	void CheckBoolExpType(CSymbol* Symbol, int StrNum);
	//�������� ���. �����
	void CheckVarRecordPartType(CRecordType* Rec, CRecordType* ChildRec, vector<CValue*> Val, int StrNum);
	//�������� ����� ��������� ??
	void CheckExpressionSize(CSymbol* Symbol, int StrNum);
	//����� ���������� � ����
	CIdent* CheckDescription(CSymbol* Symbol, int StrNum, eUseType UseType);
	void AddFlagVarPart(CRecordType* Rec, CSymbol* Symdol, int StrNum);

	//��������� ���� ���������, ���� ��������� ����
	void AddOperand(CSymbol* Symbol);
	//���� ���������� �� ����
	void AddVar(CSymbol* Symbol, int StrNum);
	void AddCheckRecord(CSymbol* Symbol, int StrNum);
	void SetCurrentRecord(CSymbol* Symbol, bool IsCheckField = false);
	void SetProgName(CSymbol* Symbol, int StrNum);
	CType* GetType(CSymbol* Symbol);
	CType* GetBaseType(CType* Type);
	CType* Cast(CType* One, CType* Two, bool IsAssign = false);
	CType* GetTypeFromMap(string Name) { return (CType*) IdentMap[Name]; }
};
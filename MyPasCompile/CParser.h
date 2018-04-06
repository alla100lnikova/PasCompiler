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
	//���������
	bool Program();
	//�����-�� ������ ����� ����� �����
	bool FileList();
	//���� ���������
	bool ProgramBlock();
	//����� ��� �����
	bool UnsignedNumber();
	//����
	bool Sign();
	bool SignedNumber();
	//������ �������� �����
	bool TypesBlock();
	//�������� ����
	bool TypeDescription();
	//���
	CType* Type();
	//������
	CRecordType* RecordType();
	//���� ������
	CRecordType* RecordFields(bool IsRightpar = false);
	//������������� ����� ������
	bool FixRecordPart(CRecordType* Rec);
	//���������� ����� ������
	bool VariantRecordPart(CRecordType* Rec);
	bool Variant(CRecordType* Rec);
	bool VariantList(CRecordType* Rec);
	//������ ������
	CType* RecordSection(vector<string>& Vars);
	//������ ����� ��������
	vector<CValue*> VariantLableList();
	//����� ��������
	bool VariantLable();
	//������ �������� ����������
	bool VarBlock();
	//�������� ���������� ����������
	bool OneTypeVar();
	//������ ����������
	bool OperatorBlock();
	bool Operator();
	bool SimpleOperator();
	//������� ��������
	bool ComplexOperator();
	//��������� ��������
	bool CompositeOperator();
	//�������� ������������
	bool AssignOperator();
	CType* Variable(bool IsRecordField = false);
	CType* FullVariable(bool IsRecordField);
	//���������� - ���� ������
	CType* VariableRecordField();
	//���������
	bool Expression();
	//�������� ���������
	bool RelationshipOperation();
	bool SimpleExpression();
	bool AddOperation();
	//���������
	bool Term();
	bool MultOperation();
	//���������
	bool Multiplier();
	//��������� � ��������� - �����, ���������� � �.�
	bool Constant();
	bool CycleOperator();
	bool ConditionOperator();
	//�������� �������������
	bool WithOperator();
	//������ ����������-������� ��� ��������� �������������
	bool RecordVarList();
};

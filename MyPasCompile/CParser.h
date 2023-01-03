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

	bool GetLexerSymbol()
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
	}
	~CParser() 
	{ 
		delete Sem; 
		delete m_pCurrentSymbol; 
	}
	CSymbol* GetCurrentSymbol() { return m_pCurrentSymbol; }

	//=======================================================================================
	//���������
	void Program();
	//�����-�� ������ ����� ����� �����
	bool FileList();
	//���� ���������
	void ProgramBlock();
	//����� ��� �����
	CType* UnsignedNumber();
	//����
	bool Sign();
	CType* SignedNumber();
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
	vector<CValue*> VariantLableList(CType* FlagType);
	//����� ��������
	bool VariantLable();
	//������ �������� ����������
	bool VarBlock();
	//�������� ���������� ����������
	bool OneTypeVar();
	//������ ����������
	void OperatorBlock();
	void Operator();
	void SimpleOperator();
	//������� ��������
	void ComplexOperator();
	//��������� ��������
	void CompositeOperator();
	//�������� ������������
	void AssignOperator();
	CType* Variable(CRecordType* Rec = nullptr);
	CType* FullVariable(CRecordType* Rec = nullptr);
	//���������� - ���� ������
	CType* VariableRecordField(CRecordType* Rec = nullptr);
	//���������
	CType* Expression();
	//�������� ���������
	bool RelationshipOperation();
	CType* SimpleExpression();
	bool AddOperation();
	bool AddBoolOperation();
	//���������
	CType* Term();
	bool MultOperation();
	bool MultIntOperation();
	bool MultBoolOperation();
	//���������
	CType* Multiplier();
	//��������� � ��������� - �����, ���������� � �.�
	CType* Constant();
	void CycleOperator();
	void ConditionOperator();
	//�������� �������������
	void WithOperator();
	//������ ����������-������� ��� ��������� �������������
	bool RecordVarList();
};

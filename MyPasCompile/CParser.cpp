#include "CParser.h"

void CParser::AddErrorAndSkip(int ErrorCode, vector<eOperator> SkipSym)
{
	if (ErrorCode != 0) Constants::AddError(ErrorCode, { Lexer->GetCurrentStr(), 0 });
	if (SkipSym.size() != 0) SkipTo(SkipSym);
}

bool CParser::Accept(eOperator Next, bool GetNext /* = true */)
{
	if (GetNext) LexerGiveMeSymbolBistra();
	return m_pCurrentSymbol && m_pCurrentSymbol->SymbolCode == Next;
}

void CParser::SkipTo(vector<eOperator> SkipSym)
{
	while (m_pCurrentSymbol && find(SkipSym.begin(), SkipSym.end(), m_pCurrentSymbol->SymbolCode) == SkipSym.end())
	{
		LexerGiveMeSymbolBistra();
	}
}

bool CParser::Program()
{
	if (Accept(programsy, false))
	{
		LexerGiveMeSymbolBistra();
		if (!Accept(ident, false))
		{
			AddErrorAndSkip(2, {});
			LexerGiveMeSymbolBistra();
		}
		else
		{
			Sem->SetProgName(m_pCurrentSymbol, Lexer->GetCurrentStr());
		}
		if (LexerGiveMeSymbolBistra() && FileList()) LexerGiveMeSymbolBistra();
	}

	if (ProgramBlock() && !Accept(point, false))
	{
		AddErrorAndSkip(61, {});
	}
	Lexer->Print();

	return true;
}

bool CParser::FileList()
{
	if (Accept(semicolon, false)) return true;
	if (Accept(leftpar, false))
	{
		while (Accept(ident))
		{
			Sem->SetProgName(m_pCurrentSymbol, Lexer->GetCurrentStr());
			if (Accept(comma)) continue;
			else
			{
				if (!Accept(rightpar, false))
					AddErrorAndSkip(20, { rightpar, semicolon, typesy, varsy, beginsy, ifsy, whilesy, withsy, endsy });
				break;
			}
		}

		if (Accept(rightpar, false))
		{
			if (Accept(semicolon)) return true;
			else
			{
				AddErrorAndSkip(14, { typesy, varsy, beginsy, ifsy, whilesy, withsy, endsy });
				return false;
			}
		}
		else
		{
			AddErrorAndSkip(4, { semicolon, typesy, varsy, beginsy, ifsy, whilesy, withsy, endsy });
			return false;
		}
	}

	AddErrorAndSkip(14, { semicolon, typesy, varsy, beginsy, ifsy, whilesy, withsy, endsy });
	return false;
}

bool CParser::ProgramBlock()
{
	if (m_pCurrentSymbol)
	{
		switch (m_pCurrentSymbol->SymbolCode)
		{
		case typesy:
			if (!TypesBlock())
			{
				AddErrorAndSkip(18, {});
			}
			break;
		case varsy:
			if (!VarBlock()) AddErrorAndSkip(18, {});
			break;
		case beginsy:
			if (LexerGiveMeSymbolBistra()) OperatorBlock();
			else AddErrorAndSkip(13, {});
			break;
		default:
			AddErrorAndSkip(17, {});
			OperatorBlock();
			break;
		}
	}
	return true;
}

bool CParser::TypesBlock()
{
	while (LexerGiveMeSymbolBistra()
		&& (!Accept(endsy, false)
			&& !Accept(varsy, false)
			&& !Accept(beginsy, false)
			&& !Accept(ifsy, false)
			&& !Accept(whilesy, false)
			&& !Accept(withsy, false)))
	{
		if (!TypeDescription()) break;
		if (!Accept(semicolon))
		{
			AddErrorAndSkip(14, { semicolon, varsy, beginsy, ifsy, whilesy, withsy, endsy });
		}
	}

	if (m_pCurrentSymbol)
	{
		switch (m_pCurrentSymbol->SymbolCode)
		{
		case varsy:
			if (!VarBlock())
			{
				AddErrorAndSkip(17, {});
				return false;
			}
			break;
		case beginsy:
			if (LexerGiveMeSymbolBistra()) OperatorBlock();
			else
			{
				AddErrorAndSkip(17, {});
				return false;
			}
			break;
		default:
			AddErrorAndSkip(17, {});
			OperatorBlock();
			break;
		}
	}
	else
	{
		AddErrorAndSkip(17, {});
		return false;
	}
	return true;
}

bool CParser::TypeDescription()
{
	if (Accept(ident, false))
	{
		Sem->CheckScalarType(m_pCurrentSymbol, Lexer->GetCurrentStr());
		if (Accept(equalsy))
		{
			if (LexerGiveMeSymbolBistra()) return Type() != nullptr;
			else
			{
				AddErrorAndSkip(18, {});
				return false;
			}
		}
		else
		{
			AddErrorAndSkip(16, { varsy, beginsy, ifsy, whilesy, withsy });
			return false;
		}
	}
	else
	{
		AddErrorAndSkip(2, { varsy, beginsy, ifsy, whilesy, withsy });
		return false;
	}
	return true;
}

CType* CParser::Type()
{
	switch (m_pCurrentSymbol->SymbolCode)
	{
	case ident:
		return static_cast<CType*>(Sem->CheckDescription(m_pCurrentSymbol, Lexer->GetCurrentStr(), utType));
	case recordsy:
		return RecordType();
	default:
		AddErrorAndSkip(2, { varsy, beginsy, ifsy, whilesy, withsy, endsy });
		return nullptr;
	}
}

CRecordType* CParser::RecordType()
{
	CRecordType* Rec = nullptr;
	if (LexerGiveMeSymbolBistra() && !(Rec = RecordFields())) return nullptr;
	if (!Accept(endsy, false))
	{
		AddErrorAndSkip(13, { varsy, beginsy, ifsy, whilesy, withsy, endsy });
		return nullptr;
	}
	return Rec;
}

CRecordType* CParser::RecordFields(bool IsRightpar)
{
	CRecordType* Rec = new CRecordType();
	switch (m_pCurrentSymbol->SymbolCode)
	{
	case ident:
		if (FixRecordPart(Rec))
		{
			if (Accept(casesy, false))
			{
				if (VariantRecordPart(Rec)) return Rec;
				else return nullptr;
			}
			return Rec;
		}
		else
			if (IsRightpar && Accept(rightpar, false)) return Rec;
			else
			{
				AddErrorAndSkip(18, { varsy, beginsy, ifsy, whilesy, withsy });
				return nullptr;
			}
	case casesy:
		if (LexerGiveMeSymbolBistra())
		{
			if (VariantRecordPart(Rec)) return Rec;
			else return nullptr;
		}
		else
		{
			AddErrorAndSkip(18, {});
			return nullptr;
		}
	default:
		AddErrorAndSkip(18, { varsy, beginsy, ifsy, whilesy, withsy });
		return nullptr;
	}
}

bool CParser::FixRecordPart(CRecordType* Rec)
{
	CType* Type;
	vector<string> Vars;
	while (Type = RecordSection(Vars))
	{
		Sem->SetRecFieldType(Type, Vars, Rec, Lexer->GetCurrentStr());
		if (Accept(semicolon)) LexerGiveMeSymbolBistra();
		else
		{
			if (Accept(endsy, false)) LexerGiveMeSymbolBistra();
			else
			{
				AddErrorAndSkip(14, {});
				if (m_pCurrentSymbol)
				{
					AddErrorAndSkip(0, { varsy, beginsy, ifsy, whilesy, withsy });
				}
				return false;
			}
		}
	}

	if (Accept(casesy, false) || Accept(endsy, false)) return true;
	return false;
}

CType* CParser::RecordSection(vector<string>& Vars)
{
	while (Accept(ident, false))
	{
		Vars.push_back(m_pCurrentSymbol->GetSymbol());
		if (Accept(comma)) LexerGiveMeSymbolBistra();
		else
		{
			if (Accept(colon, false))
			{
				if (LexerGiveMeSymbolBistra()) return Type();
				else
				{
					AddErrorAndSkip(2, { varsy, beginsy, ifsy, whilesy, withsy });
					return nullptr;
				}
			}
			else
			{
				AddErrorAndSkip(5, { varsy, beginsy, ifsy, whilesy, withsy });
				return nullptr;
			}
		}
	}
	return nullptr;
}

bool CParser::VariantRecordPart(CRecordType* Rec)
{
	if (Accept(ident))
	{
		Rec->FlagField.first = m_pCurrentSymbol->GetSymbol();
		if (Accept(colon))
			if (Accept(ident))
			{
				Sem->AddFlagVarPart(Rec, m_pCurrentSymbol, Lexer->GetCurrentStr());
				if (Accept(ofsy))
					return VariantList(Rec);
				else
				{
					AddErrorAndSkip(8, { varsy, beginsy, ifsy, whilesy, withsy });
					return false;
				}
			}
			else
			{
				AddErrorAndSkip(2, { varsy, beginsy, ifsy, whilesy, withsy });
				return false;
			}
		else
		{
			if (Accept(ofsy, false))
			{
				return VariantList(Rec);
			}
			else
			{
				AddErrorAndSkip(8, { varsy, beginsy, ifsy, whilesy, withsy });
				return false;
			}
		}
	}
	else
	{
		AddErrorAndSkip(2, { varsy, beginsy, ifsy, whilesy, withsy });
		return false;
	}
	return false;
}

bool CParser::VariantList(CRecordType* Rec)
{
	while (Variant(Rec)
		&& Accept(semicolon)
		&& !Accept(endsy, false)
		&& !Accept(varsy, false)
		&& !Accept(beginsy, false));
	if (Accept(endsy, false))
	{
		return true;
	}
	if (!Accept(semicolon, false))
	{
		AddErrorAndSkip(14, { varsy, beginsy, ifsy, whilesy, withsy });
	}
	else if (Accept(endsy)) return true;
	else AddErrorAndSkip(13, { varsy, beginsy, ifsy, whilesy, withsy });

	return false;
}

bool CParser::Variant(CRecordType* Rec)
{
	vector<CValue*> Val;
	Val = VariantLableList();
	if (Val.size() != 0)
		if (Accept(leftpar))
		{
			CRecordType* ChildRec;
			if (LexerGiveMeSymbolBistra() && (ChildRec = RecordFields(true)))
			{
				Sem->CheckVarRecordPartType(Rec, ChildRec, Val, Lexer->GetCurrentStr());
				return true;
			}
			else
			{
				if (!Accept(rightpar, false))
					AddErrorAndSkip(4, { varsy, beginsy, ifsy, whilesy, withsy });
				return false;
			}
		}
		else
		{
			AddErrorAndSkip(9, { varsy, beginsy, ifsy, whilesy, withsy });
			return false;
		}
	else
	{
		if (!Accept(endsy, false)) AddErrorAndSkip(0, { varsy, beginsy, ifsy, whilesy, withsy });
		return false;
	}
}

vector<CValue*> CParser::VariantLableList()
{
	vector<CValue*> Op;
	while (LexerGiveMeSymbolBistra()
		&& !Accept(colon, false)
		&& !Accept(endsy, false)
		&& !Accept(varsy, false)
		&& !Accept(beginsy, false))
	{
		if (!m_pCurrentSymbol || !VariantLable()) break;
		else
		{
			CValueSymbol* CurSym = static_cast<CValueSymbol*> (m_pCurrentSymbol);
			Op.push_back(CurSym->GetValue());
		}
		if (!Accept(comma)) break;
	}

	if (Accept(colon, false)) return Op;
	else if (!Accept(endsy, false)) AddErrorAndSkip(5, { varsy, beginsy, ifsy, whilesy, withsy });

	Op.clear();
	return Op;
}

bool CParser::VariantLable()
{
	if ((UnsignedNumber()
		|| Accept(charc, false)
		|| Accept(stringc, false)
		|| SignedNumber()))
		return true;

	if (!Accept(endsy, false)) AddErrorAndSkip(18, { varsy, beginsy, ifsy, whilesy, withsy });
	return false;
}

CType* CParser::UnsignedNumber()
{
	if(m_pCurrentSymbol->SymbolCode == intc || m_pCurrentSymbol->SymbolCode == floatc)
		return Sem->GetType(m_pCurrentSymbol);
	return nullptr;
}

bool CParser::Sign()
{
	return Accept(plussy, false) || Accept(minussy, false);
}

CType* CParser::SignedNumber()
{
	if (Sign() && LexerGiveMeSymbolBistra()) return UnsignedNumber();
	return nullptr;
}

bool CParser::VarBlock()
{
	while (LexerGiveMeSymbolBistra()
		&& (!Accept(endsy, false)
			&& !Accept(beginsy, false)
			&& !Accept(ifsy, false)
			&& !Accept(whilesy, false)
			&& !Accept(withsy, false)))
	{
		if (!OneTypeVar())
			AddErrorAndSkip(0, { semicolon, beginsy, ifsy, whilesy, withsy, endsy });
	}
	if (m_pCurrentSymbol)
	{
		switch (m_pCurrentSymbol->SymbolCode)
		{
		case beginsy:
			if (LexerGiveMeSymbolBistra()) OperatorBlock();
			else
			{
				AddErrorAndSkip(61, {});
				return false;
			}
			break;
		default:
			AddErrorAndSkip(17, { beginsy, ifsy, whilesy, withsy, endsy });
			OperatorBlock();
			break;
		}
	}
	else
	{
		AddErrorAndSkip(17, {});
		return false;
	}
	return true;
}

bool CParser::OneTypeVar()
{
	while (!Accept(colon, false) && Accept(ident, false))
	{
		Sem->AddVar(m_pCurrentSymbol, Lexer->GetCurrentStr());
		if (Accept(comma)) LexerGiveMeSymbolBistra();
		else break;
	}

	if (Accept(colon, false))
	{
		if (Accept(ident))
		{
			Sem->SetVarsType(m_pCurrentSymbol, Lexer->GetCurrentStr());
			if (Accept(semicolon)) return true;
			else AddErrorAndSkip(14, { beginsy, ifsy, whilesy, withsy, endsy });
		}
		else AddErrorAndSkip(2, { beginsy, ifsy, whilesy, withsy, endsy });

	}
	else AddErrorAndSkip(5, { beginsy, ifsy, whilesy, withsy, endsy });
	return false;
}

bool CParser::OperatorBlock()
{
	return CompositeOperator();
}

bool CParser::CompositeOperator()
{
	while (true)
	{
		IsSemicolonError = false;
		Operator();
		if (Accept(endsy, false)) break;
		if (!IsSemicolonError && !Accept(semicolon, false))
		{
			AddErrorAndSkip(14, {});
			IsSemicolonError = true;
		}
		if (m_pCurrentSymbol == nullptr) break;
		if (!IsSemicolonError) LexerGiveMeSymbolBistra();
	}

	if (Accept(endsy, false))
	{
		if (Accept(semicolon)) return true;
		else
		{
			if (Accept(elsesy, false)) return true;
			if (Accept(point, false)) return true;

			IsSemicolonError = true;
			AddErrorAndSkip(14, { beginsy, ifsy, whilesy, withsy, endsy });
			return false;
		}
	}
	else
	{
		AddErrorAndSkip(13, { beginsy, ifsy, whilesy, withsy, endsy });
		return false;
	}
}

bool CParser::ComplexOperator()
{
	switch (m_pCurrentSymbol->SymbolCode)
	{
	case beginsy:
		LexerGiveMeSymbolBistra();
		return CompositeOperator();
	case ifsy: return ConditionOperator();
	case whilesy: return CycleOperator();
	case withsy: return WithOperator();
	case elsesy:
	{
		AddErrorAndSkip(56, { beginsy, ifsy, whilesy, withsy, endsy });
		return false;
	}
	case endsy:
	{
		return false;
	}
	default:
		AddErrorAndSkip(6, { semicolon, beginsy, ifsy, whilesy, withsy, endsy });
		return false;
	}
}

bool CParser::ConditionOperator()
{
	CType* Type;
	if ((Type = Expression()) && !Accept(elsesy, false))
	{
		if ((Type = Sem->GetBaseType(Type)) && Type->TypeName != "boolean")
			AddErrorAndSkip(144, {});
	}
	else
	{
		AddErrorAndSkip(0, { beginsy, ifsy, elsesy, thensy, whilesy, withsy, endsy });
	}

	if (Accept(thensy, false))
		if (LexerGiveMeSymbolBistra() && Operator())
			if (Accept(elsesy, false))
			{
				LexerGiveMeSymbolBistra();
				return Operator();
			}
			else return true;
		else return false;
	else
	{
		AddErrorAndSkip(52, { beginsy, ifsy, elsesy, whilesy, withsy, endsy });
		if (Accept(elsesy, false))
		{
			LexerGiveMeSymbolBistra();
			return Operator();
		}
		return false;
	}
}

bool CParser::CycleOperator()
{
	CType* Type;
	if (Type = Expression())
	{
		if ((Type = Sem->GetBaseType(Type)) && Type->TypeName != "boolean")
			AddErrorAndSkip(144, {});
	}
	else
	{
		AddErrorAndSkip(0, { beginsy, ifsy, dosy, whilesy, withsy, endsy });
	}

	if (Accept(dosy, false))
	{
		LexerGiveMeSymbolBistra();
		return Operator();
	}
	else
	{
		AddErrorAndSkip(54, {});
		return Operator();
	}
}

bool CParser::WithOperator()
{
	if (!RecordVarList())
		AddErrorAndSkip(0, { beginsy, ifsy, dosy, whilesy, withsy, endsy });

	if (Accept(dosy, false))
	{
		LexerGiveMeSymbolBistra();
		return Operator();
	}
	else
	{
		AddErrorAndSkip(54, {});
		return Operator();
	}
}

bool CParser::RecordVarList()
{
	while (LexerGiveMeSymbolBistra() && Variable() && Accept(comma, false));

	if (!Accept(comma, false) && !Accept(dosy, false))
	{
		AddErrorAndSkip(20, {});
		return false;
	}
	return true;
}

bool CParser::Operator()
{
	if (Accept(ident, false)) return SimpleOperator();
	else return ComplexOperator();
}

bool CParser::SimpleOperator()
{
	return AssignOperator();
}

bool CParser::AssignOperator()
{
	CType* TypeVar;
	if (TypeVar = Variable())
		if (Accept(assign, false))
		{
			CType* TypeExp = Expression();
			if (!Sem->Cast(TypeVar, TypeExp, true))
				AddErrorAndSkip(145, {});
			return TypeExp != nullptr;
		}
		else AddErrorAndSkip(51, { beginsy, ifsy, whilesy, withsy, endsy });
	else AddErrorAndSkip(2, { beginsy, ifsy, whilesy, withsy, endsy });
	return false;
}

bool CParser::MultOperation()
{
	return Accept(star, false)
		|| Accept(slash, false);
}

bool CParser::MultIntOperation()
{
	return Accept(divsy, false)
		|| Accept(modsy, false);
}

bool CParser::MultBoolOperation()
{
	return Accept(andsy, false);
}

bool CParser::RelationshipOperation()
{
	return Accept(equalsy, false)
		|| Accept(greater, false)
		|| Accept(greaterequal, false)
		|| Accept(later, false)
		|| Accept(laterequal, false)
		|| Accept(insy, false)
		|| Accept(latergreater, false);
}

bool CParser::AddOperation()
{
	return Sign();
}

bool CParser::AddBoolOperation()
{
	return Accept(orsy, false);
}

CType* CParser::Constant()
{
	if(m_pCurrentSymbol->SymbolCode != ident)
		return Sem->GetType(m_pCurrentSymbol);
	return nullptr;
}

CType* CParser::FullVariable(bool IsRecordField)
{
	CType* Type = nullptr;
	if (Accept(ident, false))
	{
		//а если это поле записи, то нада хранить стартовую, иначе в определениях не найдется
		if (!IsRecordField)
			if (CVar* Var = static_cast<CVar*> (Sem->CheckDescription(m_pCurrentSymbol, Lexer->GetCurrentStr(), utVar)))
				Type = Var->GetType();
			else Type = nullptr;
			//if(IsRecordField) Sem->CheckRecordFields((CRecordType*) Type, m_pCurrentSymbol, Lexer->GetCurrentStr());
			return Type;
	}
	return nullptr;
}

CType* CParser::Variable(bool IsRecordField)
{
	CType* Type;
	if (Type = FullVariable(IsRecordField))
	{
		if (Accept(point))
		{
			Type = VariableRecordField();
		}
		return Type;
	}
	else return nullptr;
}

CType* CParser::VariableRecordField()
{
	CType* Type = nullptr;
	while (Accept(point, false) && LexerGiveMeSymbolBistra() && (Type = Variable(true)));
	return Type;
}

CType* CParser::Term()
{
	CType* Type = nullptr;
	while (LexerGiveMeSymbolBistra() && (Type = Multiplier()))
	{
		string BaseTypeName = Sem->GetBaseType(Type)->TypeName;
		if (MultBoolOperation())
		{
			if (BaseTypeName != "boolean") AddErrorAndSkip(210, {});
			continue;
		}
		if (MultIntOperation())
		{
			if (BaseTypeName != "integer") AddErrorAndSkip(212, {});
			continue;
		}
		if (MultOperation())
		{
			if (BaseTypeName == "char" 
				|| BaseTypeName == "boolean" 
				|| BaseTypeName == "string" 
				|| BaseTypeName == "") 
				AddErrorAndSkip(213, {});
			continue;
		}
		break;
	}
	return Type;
}

CType* CParser::Multiplier()
{
	CType* Type;
	if (Accept(leftpar, false))
	{
		if ((Type = Expression()) && Accept(rightpar, false))
		{
			LexerGiveMeSymbolBistra();
			return Type;
		}
		else
		{
			if (Accept(rightpar, false)) return Type;
			AddErrorAndSkip(4, { semicolon, beginsy, ifsy, whilesy, withsy, endsy });
			return false;
		}
	}
	if (Accept(notsy, false)) return Multiplier();
	if (Type = Constant())
	{
		LexerGiveMeSymbolBistra();
		return Type;
	}
	if (Accept(ident, false)) return Variable();

	return nullptr;
}

CType* CParser::Expression()
{
	CType* OneType;
	if (OneType = SimpleExpression())
		if (RelationshipOperation())
		{
			OneType = Sem->Cast(OneType, SimpleExpression());
			if (!OneType) AddErrorAndSkip(186, {});
			return OneType;
		}
		else return OneType;
	else return nullptr;
}

CType* CParser::SimpleExpression()
{
	CType* Type;
	bool IsSign = false;
	if (IsSign = Sign()) LexerGiveMeSymbolBistra();
	if (Type = Term())
	{
		Type = Sem->GetBaseType(Type);
		if (IsSign && Type && Type->TypeName != "real" && Type->TypeName != "integer")
			AddErrorAndSkip(184, {});

		bool IsBoolOp;
		while (AddOperation() ||(IsBoolOp = AddBoolOperation()))
		{
			Type = Sem->Cast(Type, Term());
			if (IsBoolOp && Type && Type->TypeName != "boolean")
			{
				AddErrorAndSkip(210, {});
				continue;
			}

			if (!Type)
			{
				AddErrorAndSkip(211, {});
			}			

			//if (!Type)
			//{
			//	AddErrorAndSkip(14, { semicolon, beginsy, ifsy, whilesy, withsy, endsy });
			//	return nullptr;
			//}
		}
		return Type;
	}
	return nullptr;
}
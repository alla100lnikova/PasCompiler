#include "CSymbol.h"
#include <algorithm>
#include <vector>
#include <string>
#include "CLexer.h"
#include <io.h>
using namespace std;

//CSymbol
CSymbol::CSymbol()
{
}

CSymbol::CSymbol(eSymbolType Type)
	:SymbolType(Type)
{
}

void CSymbol::SetSymbolCode()
{
	int CountLetter = m_SymbolStr.length(), i = 0;
	if (CountLetter > 1 && CountLetter < 9)
	{
		i = Constants::IdentNumberInKeywordTable[CountLetter - 1];
		while ((i < Constants::IdentNumberInKeywordTable[CountLetter])
			&& m_SymbolStr != Constants::KeyWordTable[i].Word) i++;

		SymbolCode = Constants::KeyWordTable[i].Code;
	}
	else
		SymbolCode = ident;
}

eCharType CSymbol::GetCharType(char Char)
{
	if (Char == '\'')
	{
		return ctApostrophe;
	}

	if (Char >= '0' && Char <= '9')
	{
		return ctNumber;
	}

	if (Char >= 'a' && Char <= 'z' || Char >= 'A' && Char <= 'Z')
	{
		return ctChar;
	}

	return ctOperator;
}

char CSymbol::GetNextChar(CharPosition& CurrentPosition, vector<string>& ProgramText)
{
	int size = ProgramText.size();
	if (CurrentPosition.CharNumber == -1)
	{
		CurrentPosition.LineNumber++;
	}

	if (CurrentPosition.LineNumber < size)
	{
		int sizeStr = ProgramText[CurrentPosition.LineNumber].size();
		if (++CurrentPosition.CharNumber >= sizeStr)
		{
			CurrentPosition.CharNumber = -1;
			return '\n'; 
		}
		return ProgramText.at(CurrentPosition.LineNumber).at(CurrentPosition.CharNumber);
	}
	else return '\n';
}

bool CSymbol::IsSymbolTypeValid(char CurrentChar, bool IsString)
{
	eCharType Type = GetCharType(CurrentChar);
	if (SymbolType == stValue)
	{
		if (IsString) return Type != ctApostrophe;
		else return Type == ctNumber;
	}
	
	return Type == ctChar || Type == ctNumber || CurrentChar == '_';
}

string CSymbol::GetStrSymbol(CharPosition& CurrentPosition, vector<string>& ProgramText, bool IsString)
{
	string Result;
	char CurrentChar = ProgramText.at(CurrentPosition.LineNumber).at(CurrentPosition.CharNumber);
	do
	{
		Result += CurrentChar;
	} while (IsSymbolTypeValid(CurrentChar = GetNextChar(CurrentPosition, ProgramText), IsString) && CurrentPosition.CharNumber != -1);

	if (Result[0] == '\'' && CurrentChar != '\'') Constants::AddError( 75, {CurrentPosition.LineNumber, (int)ProgramText[CurrentPosition.LineNumber - 1].length()} );

	transform(Result.begin(), Result.end(), Result.begin(), ::tolower);
	return Result;
}

CSymbol* CSymbol::ScanSymbol(CharPosition& CurrentPosition, vector<string>& ProgramText)
{
	int size = ProgramText.size();
	if (CurrentPosition.LineNumber >= size)
	{
		return nullptr;
	}

	CSymbol NextSymbol;
	char CurrentChar = CurrentPosition.CharNumber == -1 
		? (CurrentChar = GetNextChar(CurrentPosition, ProgramText)) 
		: ProgramText.at(CurrentPosition.LineNumber).at(CurrentPosition.CharNumber);

	if (CurrentPosition.LineNumber >= size)
	{
		return nullptr;
	}

	while (CurrentChar == ' ' || CurrentChar == '\n' || CurrentChar == '\t') CurrentChar = GetNextChar(CurrentPosition, ProgramText);

	switch (GetCharType(CurrentChar))
	{
		case ctApostrophe:
		case ctNumber:
		{
			CValueSymbol* ValueSymbol = new CValueSymbol();
			return ValueSymbol->ScanSymbol(CurrentPosition, ProgramText);
		}
		case ctChar:
		{
			CIdentSymbol* IdentSymbol = new CIdentSymbol();
			return IdentSymbol->ScanSymbol(CurrentPosition, ProgramText);
		}
		case ctOperator:
		{
			COperatorSymbol* Operator = new COperatorSymbol();
			return Operator->ScanSymbol(CurrentPosition, ProgramText);
		}
		break;
	}

	return this;
}

//CCValueSymbol
CValueSymbol::CValueSymbol() : CSymbol(stValue)
{
}

CValueSymbol::CValueSymbol(string BoolValue) : CSymbol(stValue)
{
	SymbolCode = boolc;
	m_SymbolStr = BoolValue;
	m_Value = new CBoolean(BoolValue == "true");
}

void CValueSymbol::ScanStrSymbol(CharPosition& CurrentPosition, vector<string>& ProgramText)
{
	m_SymbolStr = GetStrSymbol(CurrentPosition, ProgramText, true);
	m_SymbolStr = m_SymbolStr.substr(1, m_SymbolStr.length() - 1);
	
	if (m_SymbolStr.size() == 1)
	{
		m_Value = new CChar(m_SymbolStr[0]);
		SymbolCode = charc;
	}
	else
	{
		if (m_SymbolStr.size() < MAXLINE)
		{
			m_Value = new CString(m_SymbolStr);
			SymbolCode = stringc;
		}
		else
		{
			SymbolCode = error;
			Constants::AddError( 76, CurrentPosition );
		}
	}
	GetNextChar(CurrentPosition, ProgramText);
}

bool CValueSymbol::ScanDoubleConst(CharPosition& CurrentPosition, vector<string>& ProgramText, char& CurrentChar)
{
	m_SymbolStr +=  GetStrSymbol(CurrentPosition, ProgramText, false);

	CurrentChar = ProgramText.at(CurrentPosition.LineNumber).at(CurrentPosition.CharNumber);
	if (CurrentChar == 'E' || CurrentChar == 'e')
	{
		m_SymbolStr += CurrentChar;
		CurrentChar = GetNextChar(CurrentPosition, ProgramText);
		m_SymbolStr += CurrentChar;
		eCharType Type = GetCharType(CurrentChar);
		if (CurrentChar == '+' || CurrentChar == '-' || Type == ctNumber)
		{
			if (Type != ctNumber)
			{
				CurrentChar = GetNextChar(CurrentPosition, ProgramText);
				m_SymbolStr += CurrentChar;
				if (GetCharType(CurrentChar) != ctNumber)
				{
					Constants::AddError( 201, CurrentPosition );
					SymbolCode = error;
					return false;
				}
			}
			CurrentChar = GetNextChar(CurrentPosition, ProgramText);
			if((Type = GetCharType(CurrentChar)) == ctNumber)
				m_SymbolStr += GetStrSymbol(CurrentPosition, ProgramText, false);
		}
		else 
		{
			Constants::AddError( 201, CurrentPosition );
			SymbolCode = error;
			return false;
		}
	}
	return true;
}

CSymbol* CValueSymbol::ScanSymbol(CharPosition& CurrentPosition, vector<string>& ProgramText)
{
	int size = ProgramText.size();
	if (CurrentPosition.LineNumber >= size)
	{
		return nullptr;
	}

	char CurrentChar = ProgramText.at(CurrentPosition.LineNumber).at(CurrentPosition.CharNumber);
	if (CurrentChar == '\'')
	{
		ScanStrSymbol(CurrentPosition, ProgramText);
	}
	else
	{
		m_SymbolStr += GetStrSymbol(CurrentPosition, ProgramText, false);
		CurrentChar = CurrentPosition.CharNumber != -1 
			? ProgramText.at(CurrentPosition.LineNumber).at(CurrentPosition.CharNumber)
			: '\n';

		int size = ProgramText.size();
		if (CurrentPosition.LineNumber >= size)
		{
			return nullptr;
		}

		if (CurrentChar == '\n' || (CurrentChar != '.' && CurrentChar != 'E' && CurrentChar != 'e'))
		{
			int Value = atoi(m_SymbolStr.c_str());
			if (Value > MAXINTPAS)
			{
				Constants::AddError( 203, CurrentPosition );
				SymbolCode = intc;
			}
			else
			{
				m_Value = new CInteger(Value);
				SymbolCode = intc;
			}
		}
		else
		{
			if (ScanDoubleConst(CurrentPosition, ProgramText, CurrentChar))
			{
				double Value = atof(m_SymbolStr.c_str());
				if (Value > MAXREALPAS)
				{
					Constants::AddError(207, CurrentPosition);
					SymbolCode = floatc;
				}
				else
				{
					if (Value < MINREALPAS)
					{
						Constants::AddError(206, CurrentPosition);
						SymbolCode = floatc;
					}
					else
					{
						m_Value = new CReal(Value);
						SymbolCode = floatc;
					}
				}
			}
		}
	}

	return this;
}

//CIdentSymbol
CIdentSymbol::CIdentSymbol() : CSymbol(stIdent)
{
}

CSymbol* CIdentSymbol::ScanSymbol(CharPosition& CurrentPosition, vector<string>& ProgramText)
{
	m_SymbolStr = GetStrSymbol(CurrentPosition, ProgramText, true);
	if (m_SymbolStr == "true" || m_SymbolStr == "false")
	{
		return new CValueSymbol(m_SymbolStr);
	}

	SetSymbolCode();
	if (SymbolCode != ident) 
	{
		SymbolType = stKeyWord;
	}

	return this;
}

//COperatorSymbol
COperatorSymbol::COperatorSymbol() : CSymbol(stKeyWord)
{
}

CSymbol* COperatorSymbol::ScanSymbol(CharPosition& CurrentPosition, vector<string>& ProgramText)
{
	char CurrentChar = CurrentChar = CurrentPosition.CharNumber == -1
		? (CurrentChar = GetNextChar(CurrentPosition, ProgramText))
		: ProgramText.at(CurrentPosition.LineNumber).at(CurrentPosition.CharNumber);

	int size = ProgramText.size();
	if (CurrentPosition.LineNumber >= size)
	{
		return nullptr;
	}

	m_SymbolStr = CurrentChar;

	switch (CurrentChar)
	{
	case ',':
	{
		SymbolCode = comma;
	}
	break;
	case ';':
	{
		SymbolCode = semicolon;
	}
	break;
	case '/':
	{
		SymbolCode = slash;
	}
	break;
	case '=':
	{
		SymbolCode = equalsy;
	}
	break;
	case '+':
	{
		SymbolCode = plussy;
	}
	break;
	case '-':
	{
		SymbolCode = minussy;
	}
	break;
	case '*':
	{
		SymbolCode = star;
	}
	break;
	case '{':
	{
		SkipComments(CurrentPosition, ProgramText, '}');
		SymbolCode = error;
	}
	break;
	case '[':
	{
		SymbolCode = lbracket;
	}
	break;
	case ']':
	{
		SymbolCode = rbracket;
	}
	break;
	case ')':
	{
		SymbolCode = rightpar;
	}
	break;

	//могут оказаться 2ми
	default:
		ScanDoubleOperator(CurrentPosition, ProgramText);
		break;
	}
	GetNextChar(CurrentPosition, ProgramText);

	return this;
}

void COperatorSymbol::SkipComments(CharPosition& CurrentPosition, vector<string>& ProgramText, char EndChar)
{
	char CurrentChar = ProgramText.at(CurrentPosition.LineNumber).at(CurrentPosition.CharNumber);
	int size = ProgramText.size();
	while (CurrentPosition.LineNumber < size)
	{
		while (CurrentPosition.CharNumber != -1)
		{
			if (CurrentChar != EndChar)
			{
				CurrentChar = GetNextChar(CurrentPosition, ProgramText);
			}
			else
			{
				if (EndChar == '}') break;
				else
				{
					CurrentChar = GetNextChar(CurrentPosition, ProgramText);
					if (CurrentChar == ')') break;
				}
			}
		}
		if (CurrentPosition.CharNumber != -1) break;
		else
		{
			CurrentPosition.CharNumber = 0;
			CurrentPosition.LineNumber++;
		}
	}

	if (CurrentPosition.LineNumber >= size)
	{
		Constants::AddError( 86, { CurrentPosition.LineNumber - 1, (int)ProgramText[CurrentPosition.LineNumber - 1].length()} );
		SymbolCode = error;
	}
}

void COperatorSymbol::ScanDoubleOperator(CharPosition& CurrentPosition, vector<string>& ProgramText)
{
	char CurrentChar = ProgramText.at(CurrentPosition.LineNumber).at(CurrentPosition.CharNumber);
	CharPosition LastPosition = CurrentPosition;
	char NextChar = GetNextChar(CurrentPosition, ProgramText);
	bool IsDoubleOperator = false;

	switch (CurrentChar)
	{
	case '(':
	{
		if (IsDoubleOperator = NextChar == '*')
		{
			SkipComments(CurrentPosition, ProgramText, '*');
			SymbolCode = error;
		}
		else
		{
			SymbolCode = leftpar;
		}
	}
	break;
	case '>':
	{
		if (IsDoubleOperator = NextChar == '=')
		{
			SymbolCode = greaterequal;
		}
		else
		{
			SymbolCode = greater;
		}
	}
	break;
	case '<':
	{
		if (IsDoubleOperator = NextChar == '=')
		{
			SymbolCode = laterequal;
		}
		else
		{
			if (IsDoubleOperator = NextChar == '>')
			{
				SymbolCode = latergreater;
			}
			else
			{
				SymbolCode = later;
			}
		}
	}
	break;
	case '.':
	{
		if (IsDoubleOperator = NextChar == '.')
		{
			SymbolCode = twopoints;
		}
		else
		{
			SymbolCode = point;
		}
	}
	break;
	case ':':
	{
		if (IsDoubleOperator = NextChar == '=')
		{
			SymbolCode = assign;
		}
		else
		{
			SymbolCode = colon;
		}
	}
	break;
	default:
		Constants::AddError( 6, LastPosition );
		SymbolCode = error;
		break;
	}

	if (IsDoubleOperator)
	{
		m_SymbolStr += NextChar;
	}
	else
	{
		CurrentPosition = LastPosition;
	}
}

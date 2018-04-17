#pragma once
#pragma comment(linker, "/STACK:6500000")
#ifndef _CONSTS_
#define _CONSTS_

#include <string>
#include <map>
#include <vector>
using namespace std;

#define MAX_ERROR_NUMBER 20
#define MAXLINE 256
#define MAXINTPAS 2147483646
#define MAXREALPAS 1.7e38
#define MINREALPAS 2.9e-39

struct CharPosition
{
	int LineNumber;
	int CharNumber;
};

enum eValueType
{
	vtBoolean = 1,
	vtInteger = 2,
	vtReal = 3,
	vtChar = 4,
	vtString = 5
};

enum eUseType
{
	utProg = 1,
	utType = 2,
	utVar = 3
};

enum eCustomType
{
	tScalar,
	tRecord
};

enum eCharType
{
	ctChar = 0,
	ctOperator = 1,
	ctNumber = 2,
	ctApostrophe = 3
};

enum eOperator
{
	fake = -2,
	error = -1,

	star = 21,         /* * */
	slash = 60,        /* / */
	equalsy = 16,        /* = */
	comma = 20,        /* , */
	semicolon = 14,    /* ; */
	colon = 5,         /* : */
	point = 61,        /* . */
	arrow = 62,        /* ^ */
	leftpar = 9,       /* ( */
	rightpar = 4,      /* ) */
	lbracket = 11,     /* [ */
	rbracket = 12,     /* ] */
	flpar = 63,        /* { */
	frpar = 64,        /* } */
	later = 65,        /* < */
	greater = 66,      /* > */
	laterequal = 67,   /* <= */
	greaterequal = 68, /* => */
	latergreater = 69, /* <> */
	plussy = 70,         /* + */
	minussy = 71,        /* - */
	lcomment = 72,     /* (* */
	rcomment = 73,     /* )* */
	assign = 51,       /* := */
	twopoints = 74,    /* .. */

	ident = 2,         /* identifier */
	floatc = 82,       /* real constant */
	intc = 15,         /* integer constant */
	charc = 83,        /* character constant */
	stringc = 84,      /* string constant */
	boolc = 85,

	endoffile = 253,   /* end of file */
	eolint = 88,       /* end of sequence of integers */

					   /* list of keywords*/
	dosy = 54,
	ifsy = 56,
	insy = 22,
	ofsy = 8,
	orsy = 23,         /* | */
	tosy = 55,

	/* keywords of length 2 */

	andsy = 24,        /* & */
	divsy = 25,
	endsy = 13,
	forsy = 26,
	modsy = 27,
	nilsy = 28,
	notsy = 29,
	setsy = 29,
	varsy = 30,

	/* keywords of length 3 */

	casesy = 31,
	elsesy = 32,
	filesy = 57,
	gotosy = 33,
	thensy = 52,
	typesy = 34,
	withsy = 37,

	/* keywords of length 4 */

	arraysy = 38,
	beginsy = 17,
	constsy = 39,
	labelsy = 40,
	untilsy = 53,
	whilesy = 41,

	/* keywords of length 5 */

	downtosy = 55,
	packedsy = 42,
	recordsy = 43,
	repeatsy = 44,

	/* keywords of length 6 */

	programsy = 3,

	/* keywords of length 7 */

	functionsy = 77,

	/* keywords of length 8 */

	proceduresy = 80
};

enum eSymbolType
{
	stValue = 0,
	stKeyWord = 1,
	stIdent = 2
};

struct KeyWords
{
	string Word;
	eOperator Code;
};

#endif

class Constants
{
public:
	static int IdentNumberInKeywordTable[];
	static map<int, string> ErrorCodeTable;
	static vector<std::pair<int, CharPosition>> ErrorList;
	static KeyWords KeyWordTable[];
	static void AddError(int ErrorCode, CharPosition CurrentPosition);
};
#pragma once

#include <memory.h>
/////or <string.h>


typedef unsigned char BYTE;
typedef unsigned long DWORD;


//32 ������ ������. �������� ����������� 64bit, �� �� ������������� �� ������ ������. ������ �������� ������� �� ��������� UNSIGNED DWORD
//������������ ����� � ������ ��������� : �����, � ��������� ������, �����������
// ����������� ��� ������� �������� ��� ��� �������� ��, ����������� ��������� �������� � ��������� ��. ��� ������� ���� �������� �������������� ��������
//����� � 2^32 ������ ������� �����
//�������� ������������ ��� ���������� 32������ ��������� + 1 �� ������������.
//��������� ��Header mask
//��������� ��������� �� ������� ��������� ����� � �� � ������� �� ������� ����������� ������������� ����������
#define UD_WHEADER_SIGN_OF_BASE				0x80	//���� ��������. �������� �����������. �� ������ ������ ��������� ���������� ��������. �� ����� �������� ���, ��� ������� �������
#define UD_WHEADER_INFINITY					0x40	//������� ������������� (1 - �������������, 0 - �������� ��������) � ������ ����� ��������
#define UD_WHEADER_DEFINEDMASK				0x30	//����� �������������������� (1,2,3 - �������� ��������, 0 - �������������� ��������)
#define UD_WHEADER_DEFINED					0x30	//������ ��������
#define UD_WHEADER_LIMIT_HI					0x20	//������ ��������
#define UD_WHEADER_LIMIT_LOW				0x10	//������ ��������
#define UD_WHEADER_PRECISSION				0x03	//�������� n DWORD + 1 = ���������� DWORD �������� = : 0x0 2, 0x1 3, 0x2 9, 0x3 63. ��������� DWORD ��� ���������� �������� ��� �������� �� ����� ������ 
													// 9, 27+, 126+, 558+ ���������� ������.
#define UD_WHEADER_RESERVED					0x04	// 
#define UD_WHEADER_UNKNOWN					0x08	//������������� ��������, �������� ������������� - �������������

//����������� �������� (���� ����) ���������� ����� ����� �������� ������������ �����
// ������ ����� �� ����� ���� ����� ����, ���� ����� �� ����� ����. �������� � ������� Y1Y2Y3Y4Yn^DW  Y1!=0 Yn - ������ �����������. 
// ��������� ����� ������ - ��� ����������� ����� ������������. �������� �������� ����� ���������� DWORD �������� - 1 DWORD. ��� ������������ �������� � ������ �� ��������, � ���������� � �����
// ���� �� ����� �������������������� ����������� ����� 0 (�� ����������� �����), �� ���������� ��� ���������� ��� � ������� �������� �����. ������, 1/10 >>= 0x19999999  == BIN 0.0001 1001 1001 .... � ����� ������ ���������� ����������� ����� ����� ����� 4 (������ ����������)

#define UD_OPERATIONS_FLAG_REAL					0x01	//�������� � �������� ������
#define UD_OPERATIONS_FLAG_COMPLEX				0x02	//�������� � ����������� ������

/////////////////// Compare results
#define UD_CMP_RESULT_RE_LESS			0x00000001	//������
#define UD_CMP_RESULT_RE_GREATER		0x00000002	//������
#define UD_CMP_RESULT_RE_EQUAL			0x00000003	//�����
#define UD_CMP_RESULT_RE_MASK			0x00000007	
#define UD_CMP_RESULT_RE_CLOSE_LE		0x00000010	//������ ������
#define UD_CMP_RESULT_RE_CLOSE_GR		0x00000020	//������ ������
#define UD_CMP_RESULT_RE_CLOSE_EQ		0x00000030	//������ �����
#define UD_CMP_RESULT_RE_CLOSE_MASK		0x00000070	

#define UD_CMP_RESULT_IM_LESS			0x00000100	//������
#define UD_CMP_RESULT_IM_GREATER		0x00000200	//������
#define UD_CMP_RESULT_IM_EQUAL			0x00000300	//�����
#define UD_CMP_RESULT_IM_MASK			0x00000700	
#define UD_CMP_RESULT_IM_CLOSE_LE		0x00001000	//������ ������
#define UD_CMP_RESULT_IM_CLOSE_GR		0x00002000	//������ ������
#define UD_CMP_RESULT_IM_CLOSE_EQ		0x00003000	//������ �����
#define UD_CMP_RESULT_IM_CLOSE_MASK		0x00007000	

#define UD_CMP_RESULT_MASK				0x0000FFFF	//����������
#define UD_CMP_RESULT_UNDEF_MASK		0xFFFF0000	//������������

#define UD_CMP_RESULT_UNDEF_LEFT		0x10000000 //�� ��������� ��� �� ��������������� ����� (������) �������
#define UD_CMP_RESULT_UNDEF_RIGHT		0x20000000 //�� ��������� ��� �� ��������������� ������ (������) �������

#define UD_CMP_RESULT_RE_UNDEF			0x00010000	//�� ���������������
#define UD_CMP_RESULT_RE_INF			0x00020000	// �������������
#define UD_CMP_RESULT_RE_UNDEF_MASK		0x000F0000	

#define UD_CMP_RESULT_IM_UNDEF			0x00100000	//�� ���������������
#define UD_CMP_RESULT_IM_INF			0x00200000	// �������������
#define UD_CMP_RESULT_IM_UNDEF_MASK		0x00F00000	



////////////mantissa size
#define UD_WHEADER_MAX_PRECISSION_LEVEL	 0x3
#define UD_PREC_COUNT_LEV0	0x02
#define UD_PREC_COUNT_LEV1	0x03
#define UD_PREC_COUNT_LEV2	0x09
#define UD_PREC_COUNT_LEV3	0x3F

#define UD_ROUND_UP_BIT		0x80000000


typedef struct _UNIDIGITAL_HEADER
{
	BYTE Flags_Precission;
	char Exponent;
} UNIDIGITAL_HEADER, *PUNIDIGITAL_HEADER;

typedef struct _UNIDIGITAL
{
	UNIDIGITAL_HEADER ReHeader;
	UNIDIGITAL_HEADER ImHeader;
	DWORD Value[1];
} UNIDIGITAL, *PUNIDIGITAL;

typedef struct _UNIDIGITALR0
{
	UNIDIGITAL_HEADER ReHeader;
	UNIDIGITAL_HEADER ImHeader;
	DWORD Value[UD_PREC_COUNT_LEV0];
} UNIDIGITALR0, *PUNIDIGITALR0;


typedef struct _UNIDIGITALC0
{
	UNIDIGITAL_HEADER ReHeader;
	UNIDIGITAL_HEADER ImHeader;
	DWORD ValueR[UD_PREC_COUNT_LEV0];
	DWORD ValueC[UD_PREC_COUNT_LEV0];
} UNIDIGITALC0, *PUNIDIGITALC0;


typedef struct _UNIDIGITALR1
{
	UNIDIGITAL_HEADER ReHeader;
	UNIDIGITAL_HEADER ImHeader;
	DWORD Value[UD_PREC_COUNT_LEV1];
} UNIDIGITALR1, *PUNIDIGITALR1;


typedef struct _UNIDIGITALC1
{
	UNIDIGITAL_HEADER ReHeader;
	UNIDIGITAL_HEADER ImHeader;
	DWORD ValueR[UD_PREC_COUNT_LEV1];
	DWORD ValueC[UD_PREC_COUNT_LEV1];
} UNIDIGITALC1, *PUNIDIGITALC1;


typedef struct _UNIDIGITALR2
{
	UNIDIGITAL_HEADER ReHeader;
	UNIDIGITAL_HEADER ImHeader;
	DWORD Value[UD_PREC_COUNT_LEV2];
} UNIDIGITALR2, * PUNIDIGITALR2;


typedef struct _UNIDIGITALC2
{
	UNIDIGITAL_HEADER ReHeader;
	UNIDIGITAL_HEADER ImHeader;
	DWORD ValueR[UD_PREC_COUNT_LEV2];
	DWORD ValueC[UD_PREC_COUNT_LEV2];
} UNIDIGITALC2, * PUNIDIGITALC2;


typedef struct _UNIDIGITALR3
{
	UNIDIGITAL_HEADER ReHeader;
	UNIDIGITAL_HEADER ImHeader;
	DWORD Value[UD_PREC_COUNT_LEV3];
} UNIDIGITALR3, * PUNIDIGITALR3;


typedef struct _UNIDIGITALC3
{
	UNIDIGITAL_HEADER ReHeader;
	UNIDIGITAL_HEADER ImHeader;
	DWORD ValueR[UD_PREC_COUNT_LEV3];
	DWORD ValueC[UD_PREC_COUNT_LEV3];
} UNIDIGITALC3, * PUNIDIGITALC3;


#define UD_LOFLAG_OK			0x0000
#define UD_LOFLAG_NAN			0x0001
#define UD_LOFLAG_OVER			0x0002
#define UD_LOFLAG_INVALID_DATA	0x0004

#define UD_LOFLAG_MINUS			0x0010
#define UD_LOFLAG_PLUS			0x0020
#define UD_LOFLAG_COMPLEX		0x0030

#define UD_LOFLAG_MINUS_NAN		(UD_LOFLAG_NAN|UD_LOFLAG_MINUS)
#define UD_LOFLAG_PLUS_NAN		(UD_LOFLAG_NAN|UD_LOFLAG_PLUS)
#define UD_LOFLAG_MINUS_OVER	(UD_LOFLAG_OVER|UD_LOFLAG_MINUS)
#define UD_LOFLAG_PLUS_OVER		(UD_LOFLAG_OVER|UD_LOFLAG_PLUS)


class CUniDigital
{
private:
	DWORD m_dwOALenght;
	PUNIDIGITAL m_First;
	PUNIDIGITAL m_Second;
	PUNIDIGITAL m_Result;
	PUNIDIGITAL m_Slave;
	PUNIDIGITAL m_Temp;
	static DWORD udLastOperationFlags;
	//���� ��������� �� ������ �� ��������. �������� �������� �� ������
public:
	DWORD ADDUNSIGNED(PUNIDIGITAL in_pUD_Result, const PUNIDIGITAL in_pUD_1, const PUNIDIGITAL in_pUD_2, DWORD in_Flags);
	DWORD ADD_LL(PUNIDIGITAL_HEADER pUD_ResultH, DWORD* pdwResult, const PUNIDIGITAL_HEADER pUD_H, const DWORD* pdwUD);

	//���� �������� ���������� �� ������ �� ��������. ��������� � ���� ���������� ������� �� �������� �� ������ ���������� 
	DWORD SUBUNSIGNED(PUNIDIGITAL in_pUD_Result, const PUNIDIGITAL in_pUD_1, const PUNIDIGITAL in_pUD_2, DWORD in_Flags);
	DWORD SUB_LL(PUNIDIGITAL_HEADER pUD_ResultH, DWORD* pdwResult, const PUNIDIGITAL_HEADER pUD_H, const DWORD* pdwUD);


	CUniDigital();
	~CUniDigital();

	static DWORD InitUD(PUNIDIGITAL in_pUD, DWORD in_RePrecission, DWORD in_ImPrecission);

	static PUNIDIGITAL InitValue(PUNIDIGITAL in_pUD_Result);
	static PUNIDIGITAL InitRealValue(PUNIDIGITAL in_pUD_Result);
	static PUNIDIGITAL InitComplexValue(PUNIDIGITAL in_pUD_Result);
	static void InitValue(PUNIDIGITAL_HEADER pUD_H, DWORD* pdwValue);


	DWORD SetValue(PUNIDIGITAL in_pUD_Result, const PUNIDIGITAL in_pUD_Src);
	DWORD SetRealValue(PUNIDIGITAL in_pUD_Result, const PUNIDIGITAL in_pUD_Src);
	DWORD SetComplexValue(PUNIDIGITAL in_pUD_Result, const PUNIDIGITAL in_pUD_Src);

	DWORD CmpValue(const PUNIDIGITAL in_pUD_1, const PUNIDIGITAL in_pUD_2);
	DWORD CmpValue(const PUNIDIGITAL in_pUD_1, const PUNIDIGITAL in_pUD_2, DWORD in_Flags);
	DWORD CmpValueUnsigned(const PUNIDIGITAL in_pUD_1, const PUNIDIGITAL in_pUD_2, DWORD in_Flags);

	static DWORD GetLenght(const PUNIDIGITAL in_pUD);
	static DWORD GetLenght(DWORD in_RePrecissionLevel);
	static DWORD GetLenght(DWORD in_RePrecissionLevel, DWORD in_ImPrecissionLevel);

	DWORD GetError() { DWORD ret = CUniDigital::udLastOperationFlags;  CUniDigital::udLastOperationFlags = 0; return ret; }
	
	static DWORD OutputValue(const PUNIDIGITAL in_pUD);


	static DWORD ToDWORD(const PUNIDIGITAL in_pUD);
	static double ToDouble(const PUNIDIGITAL in_pUD);
	static long double ToLongDouble(const PUNIDIGITAL in_pUD);
	static float ToFloat(const PUNIDIGITAL in_pUD);
	static __int64 To__int64(const PUNIDIGITAL in_pUD);

	const static float fltDischarge;
	const static double dblDischarge;
	const static long double ldblDischarge;
	const static long double ldblDischarge2;
	const static long double ldblDischarge3;
	const static __int64 int64Discharge;

	DWORD ADD(PUNIDIGITAL in_pUD_Result, const PUNIDIGITAL in_pUD_1, const PUNIDIGITAL in_pUD_2);
	DWORD SUB(PUNIDIGITAL in_pUD_Result, const PUNIDIGITAL in_pUD_1, const PUNIDIGITAL in_pUD_2);
	DWORD MUL(PUNIDIGITAL in_pUD_Result, const PUNIDIGITAL in_pUD_1, const PUNIDIGITAL in_pUD_2);
	DWORD DIV(PUNIDIGITAL in_pUD_Result, const PUNIDIGITAL in_pUD_1, const PUNIDIGITAL in_pUD_2);

};





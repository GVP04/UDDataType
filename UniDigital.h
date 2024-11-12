#pragma once
//32 ������ ������. �������� ����������� 64bit, �� �� ������������� �� ������ ������. ������ �������� ������� �� ��������� UNSIGNED DWORD
//������������ ����� � ������ ��������� : �����, � ��������� ������, �����������
//����� � 2^32 ������ ������� �����
//�������� ������������ ��� ���������� 32������ ��������� 
//��������� ��Header mask
//��������� ��������� �� ������� ��������� ����� � �� � ������� �� ������� ����������� ������������� ����������
#define UD_WHEADER_SIGN_OF_BASE				0x80	//���� ��������. �������� �����������. �� ������ ������ ��������� ���������� ��������. �� ����� �������� ���, ��� ������� �������
#define UD_WHEADER_INFINITY					0x40	//������� ������������� (1 - �������������, 0 - �������� ��������) � ������ ����� ��������
#define UD_WHEADER_DEFINEDMASK				0x30	//����� �������������������� (1,2,3 - �������� ��������, 0 - �������������� ��������)
#define UD_WHEADER_DEFINED					0x30	//������ ��������
#define UD_WHEADER_LIMIT_HI					0x20	//������ ��������
#define UD_WHEADER_LIMIT_LOW				0x10	//������ ��������
#define UD_WHEADER_PRECISSION				0x03	//�������� = ���������� DWORD �������� = : 0x0 2, 0x1 4, 0x2 15, 0x3 63. ��������� DWORD ��� ���������� �������� ��� �������� �� ����� ������ 
													// 9, 27+, 126+, 558+ ���������� ������.
#define UD_WHEADER_UNKNOWN					0x08	//������������� ��������, �������� ������������� - �������������
#define UD_WHEADER_RESERVED					0x04	//������. ����� ���� ����������� ��� ���� ���������� ����������� �����, ����� ��� ����� ������������ ��������� ����� ������������ ��� �������������� ������/������ ������������ �����

//����������� �������� (���� ����) ���������� ����� ����� �������� ������������ �����
//��� ������ �������� ������������ ����� (�� ����������) ����� ����������� �������� ����� �� �������, � ��������� ����������� ����� ������������ ��� �������������� ������

#define UD_OPERATIONS_FLAG_REAL					0x01	//�������� � �������� ������
#define UD_OPERATIONS_FLAG_COMPLEX				0x02	//�������� � ����������� ������

#define UD_CMP_EQUAL	0x0	//�����
#define UD_CMP_MORE		0x1	//������
#define UD_CMP_LESS		0x2	//������
#define UD_CMP_UNKNOWN	0x4	//������������

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


class CUniDigital
{
private:
	static DWORD m_dwOALenght;
	static PUNIDIGITAL m_First;
	static PUNIDIGITAL m_Second;
	static PUNIDIGITAL m_Result;
	static PUNIDIGITAL m_Slave;
	//���� ��������� �� ������ �� ��������. �������� �������� �� ������
	static DWORD ADDUNSIGNED(PUNIDIGITAL in_pUD_Result, PUNIDIGITAL in_pUD_1, PUNIDIGITAL in_pUD_2, DWORD in_Flags);
	//���� �������� ���������� �� ������ �� ��������. ��������� � ���� ���������� ������� �� �������� �� ������ ���������� 
	static DWORD SUBUNSIGNED(PUNIDIGITAL in_pUD_Result, PUNIDIGITAL in_pUD_1, PUNIDIGITAL in_pUD_2, DWORD in_Flags);
public:
	CUniDigital();
	~CUniDigital();
	static DWORD GetLenght(PUNIDIGITAL in_pUD);
	static DWORD GetLenght(DWORD in_RePrecission);
	static DWORD GetLenght(DWORD in_RePrecission, DWORD in_ImPrecission);
	static DWORD InitUD(PUNIDIGITAL in_pUD, DWORD in_RePrecission, DWORD in_ImPrecission);
	static DWORD ADD(PUNIDIGITAL in_pUD_Result, PUNIDIGITAL in_pUD_1, PUNIDIGITAL in_pUD_2);
	static DWORD SUB(PUNIDIGITAL in_pUD_Result, PUNIDIGITAL in_pUD_1, PUNIDIGITAL in_pUD_2);
	static DWORD MUL(PUNIDIGITAL in_pUD_Result, PUNIDIGITAL in_pUD_1, PUNIDIGITAL in_pUD_2);
	static DWORD DIV(PUNIDIGITAL in_pUD_Result, PUNIDIGITAL in_pUD_1, PUNIDIGITAL in_pUD_2);
	static DWORD SetValue(PUNIDIGITAL in_pUD_Result, PUNIDIGITAL in_pUD_Src);
	static DWORD SetRealValue(PUNIDIGITAL in_pUD_Result, PUNIDIGITAL in_pUD_Src);
	static DWORD SetComplexValue(PUNIDIGITAL in_pUD_Result, PUNIDIGITAL in_pUD_Src);
	static DWORD CmpValue(PUNIDIGITAL in_pUD_1, PUNIDIGITAL in_pUD_2);
	static DWORD CmpValue(PUNIDIGITAL in_pUD_1, PUNIDIGITAL in_pUD_2, DWORD in_Flags);
	static DWORD CmpValueUnsigned(PUNIDIGITAL in_pUD_1, PUNIDIGITAL in_pUD_2, DWORD in_Flags);
};





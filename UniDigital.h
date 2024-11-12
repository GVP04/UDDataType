#pragma once
//32 битная версия. Возможна модификация 64bit, но не целесообразна на данный момент. Базово мантисса состоит из элементов UNSIGNED DWORD
//представляет числа в единой структуре : целые, с плавающей точкой, комплексные
//Число в 2^32 ричной системе счета
//Точность определяется как количество 32битных элементов 
//структура ххHeader mask
//структура расчитана из условия базовости числа в ИИ в отличии от базовых предпосылок человеческого интеллекта
#define UD_WHEADER_SIGN_OF_BASE				0x80	//знак мантиссы. Вынесено сознательно. На первый взгляд усложняет простейшие операции. По факту упрощает все, что немного сложнее
#define UD_WHEADER_INFINITY					0x40	//признак бесконечности (1 - бесконечность, 0 - конечное значение) с учетом знака мантиссы
#define UD_WHEADER_DEFINEDMASK				0x30	//маска инициализированности (1,2,3 - содержит значение, 0 - неопределенное значение)
#define UD_WHEADER_DEFINED					0x30	//точное значение
#define UD_WHEADER_LIMIT_HI					0x20	//больше значения
#define UD_WHEADER_LIMIT_LOW				0x10	//меньше значения
#define UD_WHEADER_PRECISSION				0x03	//точность = количество DWORD мантиссы = : 0x0 2, 0x1 4, 0x2 15, 0x3 63. Последний DWORD для сохраниния точности при переходе на новый разряд 
													// 9, 27+, 126+, 558+ десятичных знаков.
#define UD_WHEADER_UNKNOWN					0x08	//невычисляемое значение, например бесконечность - бесконечность
#define UD_WHEADER_RESERVED					0x04	//резерв. Может быть использован как флаг отсутствия комплексной части, тогда два байта комплексного заголовка можно использовать для дополнительных данных/флагов вещественной чисел

//комплексная мантисса (если есть) начинается сразу после мантиссы вещественной части
//для таблиц хранения вещественной чисел (не комлексных) часть комплексной мантиссы можно не хранить, а заголовок комплексной части использовать для дополнительных флагов

#define UD_OPERATIONS_FLAG_REAL					0x01	//Операция с реальной частью
#define UD_OPERATIONS_FLAG_COMPLEX				0x02	//Операция с комплексной частью

#define UD_CMP_EQUAL	0x0	//Равны
#define UD_CMP_MORE		0x1	//больше
#define UD_CMP_LESS		0x2	//Меньше
#define UD_CMP_UNKNOWN	0x4	//Неопределено

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
	//знак слагаемых не влияет на операцию. Операция сложения по модулю
	static DWORD ADDUNSIGNED(PUNIDIGITAL in_pUD_Result, PUNIDIGITAL in_pUD_1, PUNIDIGITAL in_pUD_2, DWORD in_Flags);
	//знак входящих параметров не влияет на операцию. Результат и знак результата зависит от величины по модулю параметров 
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





#pragma once

#include <memory.h>
/////or <string.h>


typedef unsigned char BYTE;
typedef unsigned long DWORD;


//32 битная версия. Возможна модификация 64bit, но не целесообразна на данный момент. Базово мантисса состоит из элементов UNSIGNED DWORD
// разработана как младшее усеченное развитие базового числового типа для сильного ИИ, учитывающий специфику хранения в нейронной БД. 
//Число в 0x100 ричной системе счета
//Точность определяется как 16битный элемент + 8 бит на переполнение.

// структура для прямого сравнения без преобразований дает быстрый результат [SEF][Mantissa0][Mantissa1][Mantissa2] 
// необходимо только учитывать очередность байтов

#define BFP08_WHEADER_SIGN_OF_BASE				0x80	// знак мантиссы. Вынесено сознательно. На первый взгляд усложняет простейшие операции. По факту упрощает все, что немного сложнее
														// бит BFP08_WHEADER_SIGN_OF_BASE == 1 -> положительное число !!!! обратная логика
#define BFP08_WHEADER_SIGN_OF_BASE_POSITIVE		0x80	
#define BFP08_WHEADER_SIGN_OF_BASE_NEGATIVE		0x00	

#define BFP08_WHEADER_SIGN_OF_EXP_POSITIVE		0x40	// 
#define BFP08_WHEADER_SIGN_OF_EXP_NEGATIVE		0x00	// 
#define BFP08_WHEADER_SIGN_OF_EXP				0x40	// знак экспоненты. Вынесено сознательно. На первый взгляд усложняет простейшие операции. По факту упрощает все, что немного сложнее
														// бит BFP08_WHEADER_SIGN_OF_EXP == 1 -> положительная экспонента  !!!! обратная логика
														// экспонента == 0 == 0x40
														// значение числа >>>> 0 == 0x00
														// exp == -3 >>  0x40 - 3 == 0x3C
#define BFP08_WHEADER_EXP_VALUE_MASK			0x7F	// маска значения экспоненты	
//#define BFP08_WHEADER_EXP_UV_MASK				0x3F	// маска значения экспоненты без знака
#define BFP08_WHEADER_INFINITY					0x7D	// признак бесконечности (3D - бесконечность) с учетом знака BFP08_WHEADER_SIGN_OF_BASE
#define BFP08_WHEADER_SOMETHIG					0x7E	// признак чего-то (3E - что-то), тогда Mantissa[0] - описывает тип чего-то, Mantissa[1] и Mantissa[2] значение чего-то
#define BFP08_WHEADER_CLOSE_TO_0				0x00	// значение числа стремится к нулю 
#define BFP08_WHEADER_UNKNOWN					0x7F	// невычисляемое значение, например бесконечность - бесконечность, / 0 

#define BFP08_MANTISSA_MAX				0xFF	// 
#define BFP08_MANTISSA_HALF				0x80	// 

#define BFP08_MANTISSA_MOVE_1				0x08	// 	Смещение в битах при преобразовании 
#define BFP08_MANTISSA_MOVE_2				0x10	// 	Смещение в битах при преобразовании  	
#define BFP08_MANTISSA_MOVE_3				0x18	// 	Смещение в битах при преобразовании  	


#define BFP08_WHEADER_0							(BFP08_WHEADER_SIGN_OF_BASE | BFP08_WHEADER_SIGN_OF_EXP)
// НЕ РЕАЛИЗОВАНО ---- дополнительная проверка если мантисса не равна нулю и BFP08_WHEADER_CLOSE_TO_MASK == BFP08_WHEADER_CLOSE_TO_0  то это неопределенность
// НЕ РЕАЛИЗОВАНО ---- BFP08_WHEADER_SOMETHIG

// первый байт не может быть равен нулю, если число не равно нулю. Хранение в формате Y1Y2Y3Yn^0x100  Y1!=0 Y3 - разряд преполнения. 
// последний байт данных - это переходящий байт переполнения. Реальная точность больше равна 2^16+. При переполнении точность и данные не теряются, а приводятся к норме

#define BFP08_LOFLAG_OK				0x0000
#define BFP08_LOFLAG_NAN			0x0001
#define BFP08_LOFLAG_OVER			0x0002
#define BFP08_LOFLAG_INVALID_DATA	0x0004

#define BFP08_LOFLAG_MINUS			0x0010
#define BFP08_LOFLAG_PLUS			0x0020
#define BFP08_LOFLAG_COMPLEX		0x0030

#define BFP08_LOFLAG_MINUS_NAN		(BFP08_LOFLAG_NAN |BFP08_LOFLAG_MINUS)
#define BFP08_LOFLAG_PLUS_NAN		(BFP08_LOFLAG_NAN |BFP08_LOFLAG_PLUS)
#define BFP08_LOFLAG_MINUS_OVER		(BFP08_LOFLAG_OVER|BFP08_LOFLAG_MINUS)
#define BFP08_LOFLAG_PLUS_OVER		(BFP08_LOFLAG_OVER|BFP08_LOFLAG_PLUS)





typedef struct BFP08
{
	BFP08()
	{
		SEF = BFP08_WHEADER_SIGN_OF_BASE_POSITIVE | BFP08_WHEADER_SIGN_OF_EXP_POSITIVE;
		Mantissa[0] = Mantissa[1] = Mantissa[2] = 0;
	}
	BFP08(int inValue)
	{
		SEF = 0;
		if (inValue >= 0) SEF = BFP08_WHEADER_SIGN_OF_BASE_POSITIVE | BFP08_WHEADER_SIGN_OF_EXP_POSITIVE;
		else
		{
			inValue = -inValue + 1;
			SEF = BFP08_WHEADER_SIGN_OF_BASE_NEGATIVE | BFP08_WHEADER_SIGN_OF_EXP_POSITIVE;
		}

		if ((inValue & 0xFF000000))
		{
			SEF += 3;
			if ((inValue & 0x00000080)) inValue++;
			Mantissa[2] = (unsigned char)(inValue >> BFP08_MANTISSA_MOVE_3);
			Mantissa[1] = (unsigned char)(inValue >> BFP08_MANTISSA_MOVE_2);
			Mantissa[0] = (unsigned char)(inValue >> BFP08_MANTISSA_MOVE_1) ;
		}
		else if ((inValue & 0xFF0000))
		{
			SEF += 2;
			Mantissa[2] = (unsigned char)(inValue >> BFP08_MANTISSA_MOVE_2);
			Mantissa[1] = (unsigned char)(inValue >> BFP08_MANTISSA_MOVE_1);
			Mantissa[0] = (unsigned char)inValue;
		}
		else if ((inValue & 0xFF00))
		{
			SEF += 1;
			Mantissa[2] = (unsigned char)(inValue >> BFP08_MANTISSA_MOVE_1);
			Mantissa[1] = (unsigned char)inValue;
			Mantissa[0] = 0;
		}
		else
		{
			Mantissa[2] = (unsigned char)inValue;
			Mantissa[1] = 0;
			Mantissa[0] = 0;
		}
	}


	BFP08(double inValue)
	{
		__int64 i_inValue = (*((__int64*)(&inValue)));

		__int64 mant = (i_inValue & 0x000FFFFFFFFFFFFF) | 0x0010000000000000;
		__int64 exp = ((i_inValue >> 52) & 0x7FF) - 0x3FF;

		unsigned char tmpSEF;

		if (exp >= 0)
		{
			tmpSEF = (exp >> 3);
			if (tmpSEF < BFP08_WHEADER_INFINITY - BFP08_WHEADER_SIGN_OF_EXP_POSITIVE) tmpSEF += BFP08_WHEADER_SIGN_OF_EXP_POSITIVE;
			else tmpSEF = BFP08_WHEADER_INFINITY;
		}
		else
		{
			tmpSEF = ((-exp - 1) >> 3);
			if (tmpSEF < BFP08_WHEADER_SIGN_OF_EXP_POSITIVE) tmpSEF = BFP08_WHEADER_SIGN_OF_EXP_POSITIVE - tmpSEF - 1;
			else tmpSEF = BFP08_WHEADER_CLOSE_TO_0;
		}

		if (!(i_inValue & 0x8000000000000000)) tmpSEF |= BFP08_WHEADER_SIGN_OF_BASE_POSITIVE;

		mant >>= (35 - ((exp) & 0x7));
		if ((mant & 0x1)) mant++;
		mant >>= 1;

		Mantissa[2] = (unsigned char)(mant >> BFP08_MANTISSA_MOVE_2);
		Mantissa[1] = (unsigned char)(mant >> BFP08_MANTISSA_MOVE_1);
		Mantissa[0] = (unsigned char)mant;

		SEF = tmpSEF;
	}

	BFP08 operator + (BFP08& other)
	{
		BFP08 ret;

		if ((SEF & BFP08_WHEADER_UNKNOWN) != BFP08_WHEADER_UNKNOWN && (other.SEF & BFP08_WHEADER_UNKNOWN) != BFP08_WHEADER_UNKNOWN)
		{
			if (((SEF ^ other.SEF) & BFP08_WHEADER_SIGN_OF_BASE)) //разные знаки
			{
				if ((SEF & BFP08_WHEADER_SIGN_OF_BASE) == BFP08_WHEADER_SIGN_OF_BASE_POSITIVE) //this >= 0
				{
					ret = other;
					ret.SEF &= BFP08_WHEADER_EXP_VALUE_MASK;
					ret = *this - ret;
				}
				else
				{
					ret = *this;
					ret.SEF &= BFP08_WHEADER_EXP_VALUE_MASK;
					ret = other - ret;
				}
			}
			else
			{
				if ((SEF & BFP08_WHEADER_INFINITY) == BFP08_WHEADER_INFINITY)		ret = *this;
				else
					if ((other.SEF & BFP08_WHEADER_INFINITY) == BFP08_WHEADER_INFINITY)		ret = other;
					else
					{//ADD  by DWORD  (byte order !!!!)
						__int64 iS1;
						int dltExp = other.SEF - SEF;

						if (!dltExp)
						{
							//(((__int64)other.Mantissa[2]) << BFP08_MANTISSA_MOVE_2) + (((__int64)other.Mantissa[1]) << BFP08_MANTISSA_MOVE_1) + ((__int64)other.Mantissa[0])
							//(((__int64)Mantissa[2]) << BFP08_MANTISSA_MOVE_2) + (((__int64)Mantissa[1]) << BFP08_MANTISSA_MOVE_1) + (__int64)Mantissa[0]

							iS1 = (((__int64)other.Mantissa[2]) << BFP08_MANTISSA_MOVE_2) + (((__int64)other.Mantissa[1]) << BFP08_MANTISSA_MOVE_1) + ((__int64)other.Mantissa[0]) + (((__int64)Mantissa[2]) << BFP08_MANTISSA_MOVE_2) + (((__int64)Mantissa[1]) << BFP08_MANTISSA_MOVE_1) + (__int64)Mantissa[0];
							ret.SEF = SEF;
						}
						else if (dltExp > 0)
						{
							iS1 = ((((__int64)Mantissa[2]) << BFP08_MANTISSA_MOVE_2) + (((__int64)Mantissa[1]) << BFP08_MANTISSA_MOVE_1) + (__int64)Mantissa[0]) >> ((dltExp << 3) - 1);
							if (iS1 & 0x1) iS1 += 2;
							iS1 = (iS1 >>= 1) + (*((DWORD*)&other) & 0x00FFFFFF);
							ret.SEF = other.SEF;
						}
						else
						{
							iS1 = (*((DWORD*)&other) & 0x00FFFFFF) >> (((-dltExp) << 3) - 1);
							if (iS1 & 0x1) iS1 += 2;
							iS1 = (iS1 >>= 1) + (((__int64)Mantissa[2]) << BFP08_MANTISSA_MOVE_2) + (((__int64)Mantissa[1]) << BFP08_MANTISSA_MOVE_1) + (__int64)Mantissa[0];
							ret.SEF = SEF;
						}


						if ((iS1 & 0xFF000000)) //переполнение
						{
							iS1 += 0x7F;
							iS1 >>= BFP08_MANTISSA_MOVE_1;

							if ((ret.SEF & BFP08_WHEADER_SIGN_OF_EXP))
							{//Exp >= 0
								if ((ret.SEF & BFP08_WHEADER_EXP_VALUE_MASK) < BFP08_WHEADER_INFINITY) ret.SEF++;
							}
							else
							{//Exp <0
								ret.SEF++;
							}
						}
						ret.Mantissa[2] = (unsigned char)(iS1 >> BFP08_MANTISSA_MOVE_2);
						ret.Mantissa[1] = (unsigned char)(iS1 >> BFP08_MANTISSA_MOVE_1);
						ret.Mantissa[0] = (unsigned char)iS1;
					}
			}
		}
		else  ret.SEF |= BFP08_WHEADER_UNKNOWN;

		return ret;
	}

	BFP08 operator - (BFP08& other)
	{
		BFP08 ret;
		return ret;
	}

	BFP08 operator * (BFP08& other)
	{
		BFP08 ret;

		//CBFP08::InitValue(in_pUD_Result);

		if ((SEF & BFP08_WHEADER_UNKNOWN) != BFP08_WHEADER_UNKNOWN && (other.SEF & BFP08_WHEADER_UNKNOWN) != BFP08_WHEADER_UNKNOWN)
		{
			if ((SEF & BFP08_WHEADER_INFINITY) == BFP08_WHEADER_INFINITY)
			{
				ret =  *this;
				ret.SEF ^= (other.SEF & BFP08_WHEADER_SIGN_OF_BASE);
				if (other.Mantissa[2] == 0) ret.SEF |= BFP08_WHEADER_UNKNOWN;
			}
			else if ((other.SEF & BFP08_WHEADER_INFINITY) == BFP08_WHEADER_INFINITY)
			{
				ret = other;
				ret.SEF ^= (SEF & BFP08_WHEADER_SIGN_OF_BASE);
				if (Mantissa[2] == 0) ret.SEF |= BFP08_WHEADER_UNKNOWN;
			}
			else
				//if (in_pUD_1.Mantissa[2] == 0 || other.Mantissa[2] == 0) InitValue(in_pUD_Result);
				//else
			{	//MUL
				__int64 iS = (((__int64)Mantissa[2]) << BFP08_MANTISSA_MOVE_2) + (((__int64)Mantissa[1]) << BFP08_MANTISSA_MOVE_1) + (__int64)Mantissa[0];
				__int64 iRes = ((iS * ((__int64)other.Mantissa[2]) << BFP08_MANTISSA_MOVE_2)) + ((iS * ((__int64)other.Mantissa[1]) << BFP08_MANTISSA_MOVE_1)) + iS * (__int64)other.Mantissa[0];

				//__int64 iRes = ((__int64)(*((DWORD*)&in_pUD_1) & 0x00FFFFFF)) * ((__int64)(*((DWORD*)&other) & 0x00FFFFFF));
				int newExp = (SEF & BFP08_WHEADER_EXP_VALUE_MASK) + (other.SEF & BFP08_WHEADER_EXP_VALUE_MASK) - 0x40;

				// 0xFFFFFF * 0xFFFFFF =  0x0000 FFFF FE00 0001
				if ((iRes & 0xFF0000000000))		iRes += 0x800000;
				else		iRes += 0x8000;

				if ((iRes & 0xFF0000000000)) // 0xFFFFFF * 0xFFFFFF =  0x0000 FFFF FE00 0001
				{
					newExp += 1;
					iRes >>= BFP08_MANTISSA_MOVE_3;
				}
				else if ((iRes & 0xFF00000000))
				{
					iRes >>= BFP08_MANTISSA_MOVE_2;
				}

				if (newExp <= BFP08_WHEADER_CLOSE_TO_0) //стремится к нулю со стороны знака мантиссы
					ret.SEF = ((SEF ^ other.SEF) & BFP08_WHEADER_SIGN_OF_BASE);
				else if (newExp >= BFP08_WHEADER_INFINITY)
					ret.SEF = ((SEF ^ other.SEF) & BFP08_WHEADER_SIGN_OF_BASE) | BFP08_WHEADER_INFINITY;
				else
				{
					//*((DWORD*)in_pUD_Result) = iRes;
					ret.Mantissa[2] = (unsigned char)(iRes >> BFP08_MANTISSA_MOVE_2);
					ret.Mantissa[1] = (unsigned char)(iRes >> BFP08_MANTISSA_MOVE_1);
					ret.Mantissa[0] = (unsigned char)iRes;
					ret.SEF = (newExp & BFP08_WHEADER_EXP_VALUE_MASK) | ((SEF ^ other.SEF) & BFP08_WHEADER_SIGN_OF_BASE);
				}
			}
		}
		else  ret.SEF = BFP08_WHEADER_UNKNOWN;

		return ret;
	}



public:
	unsigned char Mantissa[3];
	unsigned char SEF; //Mantissa Sign & Exponent & Flags
} *PBFP08;





class CBFP08
{
private:
	static DWORD udLastOperationFlags;
	static PBFP08 RollRight(PBFP08 in_pBFP08_Result, int nTimes);

public:
	//знак слагаемых не влияет на операцию. Операция сложения по модулю

	CBFP08();
	~CBFP08();

	static PBFP08 InitValue(PBFP08 in_pBFP08_Result);

	static PBFP08 SetValue(PBFP08 in_pBFP08_Result, const BFP08 &in_pBFP08_Src);

	static DWORD CmpValue(const BFP08 &in_pBFP08_1, const BFP08 &in_pBFP08_2);


	static DWORD GetError() { DWORD ret = CBFP08::udLastOperationFlags;  CBFP08::udLastOperationFlags = 0; return ret; }
	
	static DWORD OutputValue(const PBFP08 in_pUD);


	static DWORD ToDWORD(const BFP08 &in_pUD);
	static double ToDouble(const BFP08 &in_pUD);
	static long double ToLongDouble(const BFP08 &in_pUD);
	static __int64 To__int64(const BFP08 &in_pUD);


	static PBFP08 ADD(PBFP08 in_pBFP08_Result, const BFP08 &in_pBFP08_1, const BFP08 &in_pBFP08_2);
	static PBFP08 SUB(PBFP08 in_pBFP08_Result, const BFP08 &in_pBFP08_1, const BFP08 &in_pBFP08_2);
	static PBFP08 MUL(PBFP08 in_pBFP08_Result, const BFP08 &in_pBFP08_1, const BFP08 &in_pBFP08_2);
	static PBFP08 DIV(PBFP08 in_pBFP08_Result, const BFP08 &in_pBFP08_1, const BFP08 &in_pBFP08_2);

};





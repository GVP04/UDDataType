#pragma once

#include <memory.h>
/////or <string.h>

//32 битная версия. Возможна модификация 64bit, но не целесообразна на данный момент. Базово мантисса состоит из элементов UNSIGNED DWORD
// разработана как младшее усеченное развитие базового числового типа для сильного ИИ, учитывающий специфику хранения в нейронной БД. 
//Число в 0x100 ричной системе счета
//Точность определяется как 16битный элемент + 8 бит на переполнение.

// структура для прямого сравнения без преобразований дает быстрый результат [SEF][Mantissa0][Mantissa1][Mantissa2] 
// необходимо только учитывать очередность байтов

#define BFP32_WHEADER_SIGN_OF_BASE				0x80	// знак мантиссы. Вынесено сознательно. На первый взгляд усложняет простейшие операции. По факту упрощает все, что немного сложнее
														// бит BFP32_WHEADER_SIGN_OF_BASE == 1 -> положительное число !!!! обратная логика
#define BFP32_WHEADER_SIGN_OF_BASE_POSITIVE		0x80	
#define BFP32_WHEADER_SIGN_OF_BASE_NEGATIVE		0x00	

#define BFP32_WHEADER_SIGN_OF_EXP_POSITIVE		0x40	// 
#define BFP32_WHEADER_SIGN_OF_EXP_NEGATIVE		0x00	// 
#define BFP32_WHEADER_SIGN_OF_EXP				0x40	// знак экспоненты. Вынесено сознательно. На первый взгляд усложняет простейшие операции. По факту упрощает все, что немного сложнее
														// бит BFP32_WHEADER_SIGN_OF_EXP == 1 -> положительная экспонента  !!!! обратная логика
														// экспонента == 0 == 0x40
														// значение числа >>>> 0 == 0x00
														// exp == -3 >>  0x40 - 3 == 0x3C
#define BFP32_WHEADER_EXP_VALUE_MASK			0x7F	// маска значения экспоненты	
//#define BFP32_WHEADER_EXP_UV_MASK				0x3F	// маска значения экспоненты без знака
#define BFP32_WHEADER_INFINITY					0x7D	// признак бесконечности (3D - бесконечность) с учетом знака BFP32_WHEADER_SIGN_OF_BASE
#define BFP32_WHEADER_SOMETHIG					0x7E	// признак чего-то (3E - что-то), тогда Mantissa[0] - описывает тип чего-то, Mantissa[1] и Mantissa[2] значение чего-то
#define BFP32_WHEADER_CLOSE_TO_0				0x00	// значение числа стремится к нулю 
#define BFP32_WHEADER_UNKNOWN					0x7F	// невычисляемое значение, например бесконечность - бесконечность, / 0 

#define BFP32_MANTISSA_MAX				0xFFFFFFFF	// 
#define BFP32_MANTISSA_HALF				0x80000000	// 


#define BFP32_MANTISSA_MOVE_1				0x20	// 	Смещение в битах при преобразовании 
#define BFP32_MANTISSA_MOVE_2				0x40	// 	Смещение в битах при преобразовании  	
#define BFP32_MANTISSA_MOVE_3				0x60	// 	Смещение в битах при преобразовании  	


#define BFP32_WHEADER_0							(BFP32_WHEADER_SIGN_OF_BASE | BFP32_WHEADER_SIGN_OF_EXP)
// НЕ РЕАЛИЗОВАНО ---- дополнительная проверка если мантисса не равна нулю и BFP32_WHEADER_CLOSE_TO_MASK == BFP32_WHEADER_CLOSE_TO_0  то это неопределенность
// НЕ РЕАЛИЗОВАНО ---- BFP32_WHEADER_SOMETHIG

// первый байт не может быть равен нулю, если число не равно нулю. Хранение в формате Y1Y2Y3Yn^0x100  Y1!=0 Y3 - разряд преполнения. 
// последний байт данных - это переходящий байт переполнения. Реальная точность больше равна 2^16+. При переполнении точность и данные не теряются, а приводятся к норме

#define BFP32_LOFLAG_OK				0x0000
#define BFP32_LOFLAG_NAN			0x0001
#define BFP32_LOFLAG_OVER			0x0002
#define BFP32_LOFLAG_INVALID_DATA	0x0004

#define BFP32_LOFLAG_MINUS			0x0010
#define BFP32_LOFLAG_PLUS			0x0020
#define BFP32_LOFLAG_COMPLEX		0x0030

#define BFP32_LOFLAG_MINUS_NAN		(BFP32_LOFLAG_NAN |BFP32_LOFLAG_MINUS)
#define BFP32_LOFLAG_PLUS_NAN		(BFP32_LOFLAG_NAN |BFP32_LOFLAG_PLUS)
#define BFP32_LOFLAG_MINUS_OVER		(BFP32_LOFLAG_OVER|BFP32_LOFLAG_MINUS)
#define BFP32_LOFLAG_PLUS_OVER		(BFP32_LOFLAG_OVER|BFP32_LOFLAG_PLUS)





typedef struct BFP32
{
	BFP32()
	{
		SEF = BFP32_WHEADER_SIGN_OF_BASE_POSITIVE | BFP32_WHEADER_SIGN_OF_EXP_POSITIVE;
		Mantissa[0] = Mantissa[1] = Mantissa[2] = 0;
	}
	BFP32(int inValue)
	{
		SEF = 0;
		if (inValue >= 0) SEF = BFP32_WHEADER_SIGN_OF_BASE_POSITIVE | BFP32_WHEADER_SIGN_OF_EXP_POSITIVE;
		else
		{
			inValue = -inValue + 1;
			SEF = BFP32_WHEADER_SIGN_OF_BASE_NEGATIVE | BFP32_WHEADER_SIGN_OF_EXP_POSITIVE;
		}

		if ((inValue & 0xFF000000))
		{
			SEF += 3;
			if ((inValue & 0x00000080)) inValue++;
			Mantissa[2] = (unsigned int)(inValue >> BFP32_MANTISSA_MOVE_3);
			Mantissa[1] = (unsigned int)(inValue >> BFP32_MANTISSA_MOVE_2);
			Mantissa[0] = (unsigned int)(inValue >> BFP32_MANTISSA_MOVE_1) ;
		}
		else if ((inValue & 0xFF0000))
		{
			SEF += 2;
			Mantissa[2] = (unsigned int)(inValue >> BFP32_MANTISSA_MOVE_2);
			Mantissa[1] = (unsigned int)(inValue >> BFP32_MANTISSA_MOVE_1);
			Mantissa[0] = (unsigned int)inValue;
		}
		else if ((inValue & 0xFF00))
		{
			SEF += 1;
			Mantissa[2] = (unsigned int)(inValue >> BFP32_MANTISSA_MOVE_1);
			Mantissa[1] = (unsigned int)inValue;
			Mantissa[0] = 0;
		}
		else
		{
			Mantissa[2] = (unsigned int)inValue;
			Mantissa[1] = 0;
			Mantissa[0] = 0;
		}
	}


	BFP32(double inValue)
	{
		__int64 i_inValue = (*((__int64*)(&inValue)));

		__int64 mant = (i_inValue & 0x000FFFFFFFFFFFFF) | 0x0010000000000000;
		__int64 exp = ((i_inValue >> 52) & 0x7FF) - 0x3FF;

		unsigned short tmpSEF;

		if (exp >= 0)
			tmpSEF = (exp >> 5) + BFP32_WHEADER_SIGN_OF_EXP_POSITIVE;
		else
			tmpSEF = BFP32_WHEADER_SIGN_OF_EXP_POSITIVE - ((-exp-1) >> 5) - 1;

		//проверку на переполнение

		if (!(i_inValue & 0x8000000000000000)) tmpSEF |= BFP32_WHEADER_SIGN_OF_BASE_POSITIVE;

		int scrExp = exp & 0x1F;
		if (scrExp)
		{
			Mantissa[2] = (unsigned int)(mant >> (52 - scrExp));
			if (scrExp > 20)
			{
				Mantissa[1] = (unsigned int)(mant << (scrExp - 20));
				Mantissa[0] = 0;
			}
			else
			{
				Mantissa[1] = (unsigned int)(mant >> (20 - scrExp));
				Mantissa[0] = (unsigned int)(mant << (12 + scrExp));
			}
		}
		else
		{
			Mantissa[2] = (unsigned int)(mant >> 52);
			Mantissa[1] = (unsigned int)(mant >> 20 );
			Mantissa[0] = (unsigned int)(mant << 12);
		}
		SEF = tmpSEF;
	}

	BFP32(long double inValue)
	{
		__int64 *i_inValue = (__int64*)(&inValue);


		__int64 mant = (i_inValue[0] & 0x000FFFFFFFFFFFFF) | 0x0010000000000000;
		__int64 exp = ((i_inValue[0] >> 52) & 0x7FF) - 0x3FF;

		unsigned short tmpSEF;

		if (exp >= 0)
			tmpSEF = (exp >> 5) + BFP32_WHEADER_SIGN_OF_EXP_POSITIVE;
		else
			tmpSEF = BFP32_WHEADER_SIGN_OF_EXP_POSITIVE - ((-exp - 1) >> 5) - 1;

		//проверку на переполнение

		if (!(i_inValue[0] & 0x8000000000000000)) tmpSEF |= BFP32_WHEADER_SIGN_OF_BASE_POSITIVE;

		int scrExp = exp & 0x1F;
		if (scrExp)
		{
			Mantissa[2] = (unsigned int)(mant >> (52 - scrExp));
			if (scrExp > 20)
			{
				Mantissa[1] = (unsigned int)(mant << (scrExp - 20)) + (unsigned int)(i_inValue[1] >> (84 - scrExp));
				Mantissa[0] = (unsigned int)(i_inValue[1] >> (116 - scrExp));
			}
			else
			{
				Mantissa[1] = (unsigned int)(mant >> (20 - scrExp));
				Mantissa[0] = (unsigned int)(mant << (12 + scrExp)) + (unsigned int)(i_inValue[1] >> (20));
			}
		}
		else
		{
			Mantissa[2] = (unsigned int)(mant >> 52);
			Mantissa[1] = (unsigned int)(mant >> 20);
			Mantissa[0] = (unsigned int)(mant << 12) + (unsigned int)(i_inValue[1] >> 52);
		}
		SEF = tmpSEF;
	}

	BFP32 operator + (BFP32& other)
	{
		BFP32 ret;

		if ((SEF & BFP32_WHEADER_UNKNOWN) != BFP32_WHEADER_UNKNOWN && (other.SEF & BFP32_WHEADER_UNKNOWN) != BFP32_WHEADER_UNKNOWN)
		{
			if (((SEF ^ other.SEF) & BFP32_WHEADER_SIGN_OF_BASE)) //разные знаки
			{
				if ((SEF & BFP32_WHEADER_SIGN_OF_BASE) == BFP32_WHEADER_SIGN_OF_BASE_POSITIVE) //this >= 0
				{
					ret = other;
					ret.SEF &= BFP32_WHEADER_EXP_VALUE_MASK;
					ret = *this - ret;
				}
				else
				{
					ret = *this;
					ret.SEF &= BFP32_WHEADER_EXP_VALUE_MASK;
					ret = other - ret;
				}
			}
			else
			{
				if ((SEF & BFP32_WHEADER_INFINITY) == BFP32_WHEADER_INFINITY)		ret = *this;
				else
					if ((other.SEF & BFP32_WHEADER_INFINITY) == BFP32_WHEADER_INFINITY)		ret = other;
					else
					{//ADD  by DWORD  (byte order !!!!)
						unsigned __int64 iS1;
						int dltExp = other.SEF - SEF;

						if (!dltExp)
						{
							//(((__int64)other.Mantissa[2]) << BFP32_MANTISSA_MOVE_2) + (((__int64)other.Mantissa[1]) << BFP32_MANTISSA_MOVE_1) + ((__int64)other.Mantissa[0])
							//(((__int64)Mantissa[2]) << BFP32_MANTISSA_MOVE_2) + (((__int64)Mantissa[1]) << BFP32_MANTISSA_MOVE_1) + (__int64)Mantissa[0]

							iS1 = (((unsigned __int64)other.Mantissa[2]) << BFP32_MANTISSA_MOVE_2) + (((__int64)other.Mantissa[1]) << BFP32_MANTISSA_MOVE_1) + ((__int64)other.Mantissa[0]) + (((__int64)Mantissa[2]) << BFP32_MANTISSA_MOVE_2) + (((__int64)Mantissa[1]) << BFP32_MANTISSA_MOVE_1) + (__int64)Mantissa[0];
							ret.SEF = SEF;
						}
						else if (dltExp > 0)
						{
							iS1 = ((((unsigned __int64)Mantissa[2]) << BFP32_MANTISSA_MOVE_2) + (((__int64)Mantissa[1]) << BFP32_MANTISSA_MOVE_1) + (__int64)Mantissa[0]) >> ((dltExp << 3) - 1);
							if (iS1 & 0x1) iS1 += 2;
							iS1 = (iS1 >>= 1) + (*((unsigned __int64*)&other) & 0x00FFFFFF);
							ret.SEF = other.SEF;
						}
						else
						{
							iS1 = (*((unsigned __int64*)&other) & 0x00FFFFFF) >> (((-dltExp) << 3) - 1);
							if (iS1 & 0x1) iS1 += 2;
							iS1 = (iS1 >>= 1) + (((unsigned __int64)Mantissa[2]) << BFP32_MANTISSA_MOVE_2) + (((unsigned __int64)Mantissa[1]) << BFP32_MANTISSA_MOVE_1) + (unsigned __int64)Mantissa[0];
							ret.SEF = SEF;
						}


						if ((iS1 & 0xFF000000)) //переполнение
						{
							iS1 += 0x7F;
							iS1 >>= BFP32_MANTISSA_MOVE_1;

							if ((ret.SEF & BFP32_WHEADER_SIGN_OF_EXP))
							{//Exp >= 0
								if ((ret.SEF & BFP32_WHEADER_EXP_VALUE_MASK) < BFP32_WHEADER_INFINITY) ret.SEF++;
							}
							else
							{//Exp <0
								ret.SEF++;
							}
						}
						ret.Mantissa[2] = (unsigned int)(iS1 >> BFP32_MANTISSA_MOVE_2);
						ret.Mantissa[1] = (unsigned int)(iS1 >> BFP32_MANTISSA_MOVE_1);
						ret.Mantissa[0] = (unsigned int)iS1;
					}
			}
		}
		else  ret.SEF |= BFP32_WHEADER_UNKNOWN;

		return ret;
	}

	BFP32 operator - (BFP32& other)
	{
		BFP32 ret;
		return ret;
	}

	BFP32 operator * (BFP32& other)
	{
		BFP32 ret;

		//CBFP32::InitValue(in_pUD_Result);

		if ((SEF & BFP32_WHEADER_UNKNOWN) != BFP32_WHEADER_UNKNOWN && (other.SEF & BFP32_WHEADER_UNKNOWN) != BFP32_WHEADER_UNKNOWN)
		{
			if ((SEF & BFP32_WHEADER_INFINITY) == BFP32_WHEADER_INFINITY)
			{
				ret =  *this;
				ret.SEF ^= (other.SEF & BFP32_WHEADER_SIGN_OF_BASE);
				if (other.Mantissa[2] == 0) ret.SEF |= BFP32_WHEADER_UNKNOWN;
			}
			else if ((other.SEF & BFP32_WHEADER_INFINITY) == BFP32_WHEADER_INFINITY)
			{
				ret = other;
				ret.SEF ^= (SEF & BFP32_WHEADER_SIGN_OF_BASE);
				if (Mantissa[2] == 0) ret.SEF |= BFP32_WHEADER_UNKNOWN;
			}
			else
				//if (in_pUD_1.Mantissa[2] == 0 || other.Mantissa[2] == 0) InitValue(in_pUD_Result);
				//else
			{	//MUL
				__int64 iS = (((__int64)Mantissa[2]) << BFP32_MANTISSA_MOVE_2) + (((__int64)Mantissa[1]) << BFP32_MANTISSA_MOVE_1) + (__int64)Mantissa[0];
				__int64 iRes = ((iS * ((__int64)other.Mantissa[2]) << BFP32_MANTISSA_MOVE_2)) + ((iS * ((__int64)other.Mantissa[1]) << BFP32_MANTISSA_MOVE_1)) + iS * (__int64)other.Mantissa[0];

				//__int64 iRes = ((__int64)(*((DWORD*)&in_pUD_1) & 0x00FFFFFF)) * ((__int64)(*((DWORD*)&other) & 0x00FFFFFF));
				int newExp = (SEF & BFP32_WHEADER_EXP_VALUE_MASK) + (other.SEF & BFP32_WHEADER_EXP_VALUE_MASK) - 0x40;

				// 0xFFFFFF * 0xFFFFFF =  0x0000 FFFF FE00 0001
				if ((iRes & 0xFF0000000000))		iRes += 0x800000;
				else		iRes += 0x8000;

				if ((iRes & 0xFF0000000000)) // 0xFFFFFF * 0xFFFFFF =  0x0000 FFFF FE00 0001
				{
					newExp += 1;
					iRes >>= BFP32_MANTISSA_MOVE_3;
				}
				else if ((iRes & 0xFF00000000))
				{
					iRes >>= BFP32_MANTISSA_MOVE_2;
				}

				if (newExp <= BFP32_WHEADER_CLOSE_TO_0) //стремится к нулю со стороны знака мантиссы
					ret.SEF = ((SEF ^ other.SEF) & BFP32_WHEADER_SIGN_OF_BASE);
				else if (newExp >= BFP32_WHEADER_INFINITY)
					ret.SEF = ((SEF ^ other.SEF) & BFP32_WHEADER_SIGN_OF_BASE) | BFP32_WHEADER_INFINITY;
				else
				{
					//*((DWORD*)in_pUD_Result) = iRes;
					ret.Mantissa[2] = (unsigned int)(iRes >> BFP32_MANTISSA_MOVE_2);
					ret.Mantissa[1] = (unsigned int)(iRes >> BFP32_MANTISSA_MOVE_1);
					ret.Mantissa[0] = (unsigned int)iRes;
					ret.SEF = (newExp & BFP32_WHEADER_EXP_VALUE_MASK) | ((SEF ^ other.SEF) & BFP32_WHEADER_SIGN_OF_BASE);
				}
			}
		}
		else  ret.SEF = BFP32_WHEADER_UNKNOWN;

		return ret;
	}



public:
	unsigned int Mantissa[3];
	unsigned int SEF; //Mantissa Sign & Exponent & Flags
} *PBFP32;





class CBFP32
{
private:
	static unsigned int udLastOperationFlags;
	static PBFP32 RollRight(PBFP32 in_pBFP32_Result, int nTimes);

public:
	//знак слагаемых не влияет на операцию. Операция сложения по модулю

	CBFP32();
	~CBFP32();

	static PBFP32 InitValue(PBFP32 in_pBFP32_Result);

	static PBFP32 SetValue(PBFP32 in_pBFP32_Result, const BFP32 &in_pBFP32_Src);

	static unsigned int CmpValue(const BFP32 &in_pBFP32_1, const BFP32 &in_pBFP32_2);


	static unsigned __int64 GetError() { unsigned __int64 ret = CBFP32::udLastOperationFlags;  CBFP32::udLastOperationFlags = 0; return ret; }
	
	static unsigned __int64 OutputValue(const PBFP32 in_pUD);


	static unsigned int Tounsignedint(const BFP32 &in_pUD);
	static double ToDouble(const BFP32 &in_pUD);
	static long double ToLongDouble(const BFP32 &in_pUD);
	static __int64 To__int64(const BFP32 &in_pUD);


	static PBFP32 ADD(PBFP32 in_pBFP32_Result, const BFP32 &in_pBFP32_1, const BFP32 &in_pBFP32_2);
	static PBFP32 SUB(PBFP32 in_pBFP32_Result, const BFP32 &in_pBFP32_1, const BFP32 &in_pBFP32_2);
	static PBFP32 MUL(PBFP32 in_pBFP32_Result, const BFP32 &in_pBFP32_1, const BFP32 &in_pBFP32_2);
	static PBFP32 DIV(PBFP32 in_pBFP32_Result, const BFP32 &in_pBFP32_1, const BFP32 &in_pBFP32_2);

};





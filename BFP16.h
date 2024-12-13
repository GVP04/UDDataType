#pragma once

#include <memory.h>
/////or <string.h>

// структура для прямого сравнения без преобразований дает быстрый результат [SEF][Mantissa2][Mantissa1][Mantissa0] 
// необходимо только учитывать очередность байтов

#define BFP16_WHEADER_SIGN_OF_BASE				0x80	// знак мантиссы. Вынесено сознательно. На первый взгляд усложняет простейшие операции. По факту упрощает все, что немного сложнее
														// бит BFP16_WHEADER_SIGN_OF_BASE == 1 -> положительное число !!!! обратная логика
#define BFP16_WHEADER_SIGN_OF_BASE_POSITIVE		0x80	
#define BFP16_WHEADER_SIGN_OF_BASE_NEGATIVE		0x00	

#define BFP16_WHEADER_SIGN_OF_EXP_POSITIVE		0x40	// 
#define BFP16_WHEADER_SIGN_OF_EXP_NEGATIVE		0x00	// 
#define BFP16_WHEADER_SIGN_OF_EXP				0x40	// знак экспоненты. Вынесено сознательно. На первый взгляд усложняет простейшие операции. По факту упрощает все, что немного сложнее
														// бит BFP16_WHEADER_SIGN_OF_EXP == 1 -> положительная экспонента  !!!! обратная логика
														// экспонента == 0 == 0x40
														// значение числа >>>> 0 == 0x0000
														// exp == -3 >>  0x4000 - 3 == 0x3C00
#define BFP16_WHEADER_EXP_VALUE_MASK			0x7F	// маска значения экспоненты	
//#define BFP16_WHEADER_EXP_UV_MASK				0x3F	// маска значения экспоненты без знака
#define BFP16_WHEADER_INFINITY					0x7D	// признак бесконечности (3D - бесконечность) с учетом знака BFP16_WHEADER_SIGN_OF_BASE
#define BFP16_WHEADER_SOMETHIG					0x7E	// признак чего-то (3E - что-то), тогда Mantissa[0] - описывает тип чего-то, Mantissa[1] и Mantissa[2] значение чего-то
#define BFP16_WHEADER_CLOSE_TO_0				0x00	// значение числа стремится к нулю 
#define BFP16_WHEADER_UNKNOWN					0x7F	// невычисляемое значение, например бесконечность - бесконечность, / 0 

#define BFP16_WHEADER_USERDATA					0xFF	// зарезервировано

#define BFP16_MANTISSA_MAX				0xFFFF	// 	
#define BFP16_MANTISSA_HALF				0x8000	// 	

#define BFP16_MANTISSA_MOVE_1				0x10	// 	Смещение в битах при преобразовании 
#define BFP16_MANTISSA_MOVE_2				0x20	// 	Смещение в битах при преобразовании  	
#define BFP16_MANTISSA_MOVE_3				0x30	// 	Смещение в битах при преобразовании  	

#define BFP16_WHEADER_0							(BFP16_WHEADER_SIGN_OF_BASE | BFP16_WHEADER_SIGN_OF_EXP)
// НЕ РЕАЛИЗОВАНО ---- дополнительная проверка если мантисса не равна нулю и BFP16_WHEADER_CLOSE_TO_MASK == BFP16_WHEADER_CLOSE_TO_0  то это неопределенность
// НЕ РЕАЛИЗОВАНО ---- BFP16_WHEADER_SOMETHIG

// первый байт не может быть равен нулю, если число не равно нулю. Хранение в формате Y1Y2Y3Yn^0x100  Y1!=0 Y3 - разряд преполнения. 
// последний байт данных - это переходящий байт переполнения. Реальная точность больше равна 2^16+. При переполнении точность и данные не теряются, а приводятся к норме

#define BFP16_LOFLAG_OK				0x0000
#define BFP16_LOFLAG_NAN			0x0001
#define BFP16_LOFLAG_OVER			0x0002
#define BFP16_LOFLAG_INVALID_DATA	0x0004

#define BFP16_LOFLAG_MINUS			0x0010
#define BFP16_LOFLAG_PLUS			0x0020
#define BFP16_LOFLAG_COMPLEX		0x0030

#define BFP16_LOFLAG_MINUS_NAN		(BFP16_LOFLAG_NAN |BFP16_LOFLAG_MINUS)
#define BFP16_LOFLAG_PLUS_NAN		(BFP16_LOFLAG_NAN |BFP16_LOFLAG_PLUS)
#define BFP16_LOFLAG_MINUS_OVER		(BFP16_LOFLAG_OVER|BFP16_LOFLAG_MINUS)
#define BFP16_LOFLAG_PLUS_OVER		(BFP16_LOFLAG_OVER|BFP16_LOFLAG_PLUS)

typedef struct BFP16
{
public:
	unsigned short Mantissa[3];
	unsigned char UserData; //UserData
	unsigned char SEF; //Mantissa Sign & Exponent & Flags

public:

	BFP16()
	{
		SEF = BFP16_WHEADER_SIGN_OF_BASE_POSITIVE | BFP16_WHEADER_SIGN_OF_EXP_POSITIVE;
		Mantissa[0] = Mantissa[1] = Mantissa[2] = 0;
	}
	BFP16(int inValue)
	{
		SEF = 0;
		if (inValue >= 0) SEF = BFP16_WHEADER_SIGN_OF_BASE_POSITIVE | BFP16_WHEADER_SIGN_OF_EXP_POSITIVE;
		else
		{
			inValue = -inValue + 1;
			SEF = BFP16_WHEADER_SIGN_OF_BASE_NEGATIVE | BFP16_WHEADER_SIGN_OF_EXP_POSITIVE;
		}

		if ((inValue & 0xFF000000))
		{
			SEF += 3;
			if ((inValue & 0x00000080)) inValue++;
			Mantissa[2] = (unsigned short)(inValue >> BFP16_MANTISSA_MOVE_3);
			Mantissa[1] = (unsigned short)(inValue >> BFP16_MANTISSA_MOVE_2);
			Mantissa[0] = (unsigned short)(inValue >> BFP16_MANTISSA_MOVE_1) ;
		}
		else if ((inValue & 0xFF0000))
		{
			SEF += 2;
			Mantissa[2] = (unsigned short)(inValue >> BFP16_MANTISSA_MOVE_2);
			Mantissa[1] = (unsigned short)(inValue >> BFP16_MANTISSA_MOVE_1);
			Mantissa[0] = (unsigned short)inValue;
		}
		else if ((inValue & 0xFF00))
		{
			SEF += 1;
			Mantissa[2] = (unsigned short)(inValue >> BFP16_MANTISSA_MOVE_1);
			Mantissa[1] = (unsigned short)inValue;
			Mantissa[0] = 0;
		}
		else
		{
			Mantissa[2] = (unsigned short)inValue;
			Mantissa[1] = 0;
			Mantissa[0] = 0;
		}
	}


	BFP16(double inValue)
	{
		__int64 i_inValue = (*((__int64*)(&inValue)));

		__int64 mant = (i_inValue & 0x000FFFFFFFFFFFFF) | 0x0010000000000000;
		__int64 exp = ((i_inValue >> 52) & 0x7FF) - 0x3FF;

		unsigned char tmpSEF;

		if (exp >= 0)
		{
			tmpSEF = (exp >> 4);
			if (tmpSEF < BFP16_WHEADER_INFINITY - BFP16_WHEADER_SIGN_OF_EXP_POSITIVE) tmpSEF += BFP16_WHEADER_SIGN_OF_EXP_POSITIVE;
			else tmpSEF = BFP16_WHEADER_INFINITY;
		}
		else
		{
			tmpSEF = ((-exp - 1) >> 4);
			if (tmpSEF < BFP16_WHEADER_SIGN_OF_EXP_POSITIVE) tmpSEF = BFP16_WHEADER_SIGN_OF_EXP_POSITIVE - tmpSEF - 1;
			else tmpSEF = BFP16_WHEADER_CLOSE_TO_0;
		}

		if (!(i_inValue & 0x8000000000000000)) tmpSEF |= BFP16_WHEADER_SIGN_OF_BASE_POSITIVE;

		mant >>= (19 - (exp & 0xF));
		if ((mant & 0x1)) mant++;
		mant >>= 1;

		Mantissa[2] = (unsigned short)(mant >> BFP16_MANTISSA_MOVE_2);
		Mantissa[1] = (unsigned short)(mant >> BFP16_MANTISSA_MOVE_1);
		Mantissa[0] = (unsigned short)mant;

		SEF = tmpSEF;
	}

	BFP16 operator + (BFP16& other)
	{
		BFP16 ret;

		if ((SEF & BFP16_WHEADER_UNKNOWN) != BFP16_WHEADER_UNKNOWN && (other.SEF & BFP16_WHEADER_UNKNOWN) != BFP16_WHEADER_UNKNOWN)
		{
			if (((SEF ^ other.SEF) & BFP16_WHEADER_SIGN_OF_BASE)) //разные знаки
			{
				if ((SEF & BFP16_WHEADER_SIGN_OF_BASE) == BFP16_WHEADER_SIGN_OF_BASE_POSITIVE) //this >= 0
				{
					ret = other;
					ret.SEF &= BFP16_WHEADER_EXP_VALUE_MASK;
					ret = *this - ret;
				}
				else
				{
					ret = *this;
					ret.SEF &= BFP16_WHEADER_EXP_VALUE_MASK;
					ret = other - ret;
				}
			}
			else
			{
				if ((SEF & BFP16_WHEADER_INFINITY) == BFP16_WHEADER_INFINITY)		ret = *this;
				else
					if ((other.SEF & BFP16_WHEADER_INFINITY) == BFP16_WHEADER_INFINITY)		ret = other;
					else
					{//ADD  by DWORD  (byte order !!!!)
						__int64 iS1;
						int dltExp = other.SEF - SEF;

						if (!dltExp)
						{
							//(((__int64)other.Mantissa[2]) << BFP16_MANTISSA_MOVE_2) + (((__int64)other.Mantissa[1]) << BFP16_MANTISSA_MOVE_1) + ((__int64)other.Mantissa[0])
							//(((__int64)Mantissa[2]) << BFP16_MANTISSA_MOVE_2) + (((__int64)Mantissa[1]) << BFP16_MANTISSA_MOVE_1) + (__int64)Mantissa[0]

							iS1 = (((__int64)other.Mantissa[2]) << BFP16_MANTISSA_MOVE_2) + (((__int64)other.Mantissa[1]) << BFP16_MANTISSA_MOVE_1) + ((__int64)other.Mantissa[0]) + (((__int64)Mantissa[2]) << BFP16_MANTISSA_MOVE_2) + (((__int64)Mantissa[1]) << BFP16_MANTISSA_MOVE_1) + (__int64)Mantissa[0];
							ret.SEF = SEF;
						}
						else if (dltExp > 0)
						{
							iS1 = ((((__int64)Mantissa[2]) << BFP16_MANTISSA_MOVE_2) + (((__int64)Mantissa[1]) << BFP16_MANTISSA_MOVE_1) + (__int64)Mantissa[0]) >> ((dltExp << 3) - 1);
							if (iS1 & 0x1) iS1 += 2;
							iS1 = (iS1 >>= 1) + (*((__int64*)&other) & 0x00FFFFFFFFFFFF);
							ret.SEF = other.SEF;
						}
						else
						{
							iS1 = (*((__int64*)&other) & 0x00FFFFFFFFFFFF) >> (((-dltExp) << 3) - 1);
							if (iS1 & 0x1) iS1 += 2;
							iS1 = (iS1 >>= 1) + (((__int64)Mantissa[2]) << BFP16_MANTISSA_MOVE_2) + (((__int64)Mantissa[1]) << BFP16_MANTISSA_MOVE_1) + (__int64)Mantissa[0];
							ret.SEF = SEF;
						}


						if ((iS1 & 0xFFFF000000000000)) //переполнение
						{
							iS1 += 0x7FFF;
							iS1 >>= BFP16_MANTISSA_MOVE_1;

							if ((ret.SEF & BFP16_WHEADER_SIGN_OF_EXP))
							{//Exp >= 0
								if ((ret.SEF & BFP16_WHEADER_EXP_VALUE_MASK) < BFP16_WHEADER_INFINITY) ret.SEF++;
							}
							else
							{//Exp <0
								ret.SEF++;
							}
						}
						ret.Mantissa[2] = (unsigned short)(iS1 >> BFP16_MANTISSA_MOVE_2);
						ret.Mantissa[1] = (unsigned short)(iS1 >> BFP16_MANTISSA_MOVE_1);
						ret.Mantissa[0] = (unsigned short)iS1;
					}
			}
		}
		else  ret.SEF |= BFP16_WHEADER_UNKNOWN;

		return ret;
	}

	BFP16 operator - (BFP16& other)
	{
		BFP16 ret;
		return ret;
	}

	BFP16 operator * (BFP16& other)
	{
		BFP16 ret;

		if ((SEF & BFP16_WHEADER_UNKNOWN) != BFP16_WHEADER_UNKNOWN && (other.SEF & BFP16_WHEADER_UNKNOWN) != BFP16_WHEADER_UNKNOWN)
		{
			if ((SEF & BFP16_WHEADER_INFINITY) == BFP16_WHEADER_INFINITY)
			{
				ret = *this;
				ret.SEF ^= (other.SEF & BFP16_WHEADER_SIGN_OF_BASE);
				if (other.Mantissa[2] == 0) ret.SEF |= BFP16_WHEADER_UNKNOWN;
			}
			else if ((other.SEF & BFP16_WHEADER_INFINITY) == BFP16_WHEADER_INFINITY)
			{
				ret = other;
				ret.SEF ^= (SEF & BFP16_WHEADER_SIGN_OF_BASE);
				if (Mantissa[2] == 0) ret.SEF |= BFP16_WHEADER_UNKNOWN;
			}
			else
			{	//MUL
				unsigned __int64 iS = (((__int64)Mantissa[2]) << BFP16_MANTISSA_MOVE_2) + (((__int64)Mantissa[1]) << BFP16_MANTISSA_MOVE_1) + (__int64)Mantissa[0];
				unsigned __int64 iS0 = iS * ((__int64)other.Mantissa[0]);
				unsigned __int64 iS1 = iS * ((__int64)other.Mantissa[1]);

				unsigned __int64 iRes = iS * ((__int64)other.Mantissa[2]) + (iS1 >> BFP16_MANTISSA_MOVE_1) + (iS0 >> BFP16_MANTISSA_MOVE_2);
				iRes += (((iS0 >> BFP16_MANTISSA_MOVE_1) & 0xFFFF) + (iS1 & 0xFFFF)) >> BFP16_MANTISSA_MOVE_1;

				int newExp = (SEF & BFP16_WHEADER_EXP_VALUE_MASK) + (other.SEF & BFP16_WHEADER_EXP_VALUE_MASK) - 0x40;

				if ((iRes & 0xFF00000000000000))
				{
					iRes += 0x8000;
					newExp += 1;
					iRes >>= BFP16_MANTISSA_MOVE_1;
				}

				if (newExp <= BFP16_WHEADER_CLOSE_TO_0) //стремится к нулю со стороны знака мантиссы
					ret.SEF = ((SEF ^ other.SEF) & BFP16_WHEADER_SIGN_OF_BASE);
				else if (newExp >= BFP16_WHEADER_INFINITY)
					ret.SEF = ((SEF ^ other.SEF) & BFP16_WHEADER_SIGN_OF_BASE) | BFP16_WHEADER_INFINITY;
				else
				{
					ret.Mantissa[2] = (unsigned short)(iRes >> BFP16_MANTISSA_MOVE_2);
					ret.Mantissa[1] = (unsigned short)(iRes >> BFP16_MANTISSA_MOVE_1);
					ret.Mantissa[0] = (unsigned short)iRes;
					ret.SEF = (newExp & BFP16_WHEADER_EXP_VALUE_MASK) | ((SEF ^ other.SEF) & BFP16_WHEADER_SIGN_OF_BASE);
				}
			}
		}
		return ret;
	}


} *PBFP16;





class CBFP16
{
private:
	static unsigned int udLastOperationFlags;
	static PBFP16 RollRight(PBFP16 in_pBFP16_Result, int nTimes);

public:
	//знак слагаемых не влияет на операцию. Операция сложения по модулю

	CBFP16();
	~CBFP16();

	static PBFP16 InitValue(PBFP16 in_pBFP16_Result);

	static PBFP16 SetValue(PBFP16 in_pBFP16_Result, const BFP16 &in_pBFP16_Src);

	static unsigned int  CmpValue(const BFP16 &in_pBFP16_1, const BFP16 &in_pBFP16_2);


	static unsigned int  GetError() { unsigned int  ret = CBFP16::udLastOperationFlags;  CBFP16::udLastOperationFlags = 0; return ret; }
	
	static unsigned int  OutputValue(const PBFP16 in_pUD);


	static unsigned int  Tounsignedint(const BFP16 &in_pUD);
	static double ToDouble(const BFP16 &in_pUD);
	static long double ToLongDouble(const BFP16 &in_pUD);
	static float ToFloat(const BFP16 &in_pUD);
	static __int64 To__int64(const BFP16 &in_pUD);


	static PBFP16 ADD(PBFP16 in_pBFP16_Result, const BFP16 &in_pBFP16_1, const BFP16 &in_pBFP16_2);
	static PBFP16 SUB(PBFP16 in_pBFP16_Result, const BFP16 &in_pBFP16_1, const BFP16 &in_pBFP16_2);
	static PBFP16 MUL(PBFP16 in_pBFP16_Result, const BFP16 &in_pBFP16_1, const BFP16 &in_pBFP16_2);
	static PBFP16 DIV(PBFP16 in_pBFP16_Result, const BFP16 &in_pBFP16_1, const BFP16 &in_pBFP16_2);

	static double DIVDBL(const double in_D_1, const double in_D_2);
};





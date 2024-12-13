
#include "BFP16.h"


unsigned int  CBFP16::udLastOperationFlags;

CBFP16::CBFP16()
{
}


CBFP16::~CBFP16()
{
}

PBFP16 CBFP16::InitValue(PBFP16 in_pUD_Result)
{
	if (in_pUD_Result)
	{
		in_pUD_Result->Mantissa[0] = 0;
		in_pUD_Result->Mantissa[1] = 0;
		in_pUD_Result->Mantissa[2] = 0;
		in_pUD_Result->SEF = BFP16_WHEADER_SIGN_OF_BASE_POSITIVE | BFP16_WHEADER_SIGN_OF_EXP_POSITIVE;
	}

	return in_pUD_Result;
}

PBFP16 CBFP16::RollRight(PBFP16 in_pBFP16_Result, int nTimes)
{ // прямой разбор через IF для скорости
	if (nTimes == 1)
	{
		if ((in_pBFP16_Result->Mantissa[0] & BFP16_MANTISSA_HALF)) //переполнение и инкремент при сдвиге
		{
			if ((in_pBFP16_Result->Mantissa[1] == BFP16_MANTISSA_MAX))
			{
				in_pBFP16_Result->Mantissa[0] = 0;
				if ((in_pBFP16_Result->Mantissa[2] == BFP16_MANTISSA_MAX))
				{
					in_pBFP16_Result->Mantissa[1] = 0;
					in_pBFP16_Result->Mantissa[2] = 1;
				}
				else
				{
					in_pBFP16_Result->Mantissa[1] = in_pBFP16_Result->Mantissa[2] + 1;
					in_pBFP16_Result->Mantissa[2] = 0;
				}
			}
			else
			{
				in_pBFP16_Result->Mantissa[0] = in_pBFP16_Result->Mantissa[1] + 1;
				in_pBFP16_Result->Mantissa[1] = in_pBFP16_Result->Mantissa[2];
				in_pBFP16_Result->Mantissa[2] = 0;
			}
		}
		else
		{
			in_pBFP16_Result->Mantissa[0] = in_pBFP16_Result->Mantissa[1];
			in_pBFP16_Result->Mantissa[1] = in_pBFP16_Result->Mantissa[2];
			in_pBFP16_Result->Mantissa[2] = 0;
		}
	}
	else if (nTimes == 2)
	{
		if ((in_pBFP16_Result->Mantissa[1] & BFP16_MANTISSA_HALF)) //переполнение и инкремент при сдвиге
		{
			if ((in_pBFP16_Result->Mantissa[2] == BFP16_MANTISSA_MAX))
			{
					in_pBFP16_Result->Mantissa[0] = 0;
					in_pBFP16_Result->Mantissa[1] = 1;
			}
			else
			{
				in_pBFP16_Result->Mantissa[0] = in_pBFP16_Result->Mantissa[2] + 1;
				in_pBFP16_Result->Mantissa[1] = 0;
			}
		}
		else
		{
			in_pBFP16_Result->Mantissa[0] = in_pBFP16_Result->Mantissa[2];
			in_pBFP16_Result->Mantissa[1] = 0;
		}
		in_pBFP16_Result->Mantissa[2] = 0;
	}
	else if (nTimes == 3)
	{
		if ((in_pBFP16_Result->Mantissa[2] & BFP16_MANTISSA_HALF)) //переполнение и инкремент при сдвиге
			in_pBFP16_Result->Mantissa[0] = 1;
		else
			in_pBFP16_Result->Mantissa[0] = 0;

		in_pBFP16_Result->Mantissa[1] = 0;
		in_pBFP16_Result->Mantissa[2] = 0;

	}
	else if (nTimes > 3)
	{
		in_pBFP16_Result->Mantissa[0] = 0;
		in_pBFP16_Result->Mantissa[1] = 0;
		in_pBFP16_Result->Mantissa[2] = 0;
	}

	return in_pBFP16_Result;
}

PBFP16 CBFP16::ADD(PBFP16 in_pUD_Result, const BFP16 &in_pUD_1, const BFP16 &in_pUD_2)
{
	if (in_pUD_Result)
	{
		if ( (in_pUD_1.SEF & BFP16_WHEADER_UNKNOWN) != BFP16_WHEADER_UNKNOWN && (in_pUD_2.SEF & BFP16_WHEADER_UNKNOWN) != BFP16_WHEADER_UNKNOWN	)
		{
			if (((in_pUD_1.SEF ^ in_pUD_2.SEF) & BFP16_WHEADER_SIGN_OF_BASE)) //разные знаки
			{
				if ((in_pUD_1.SEF & BFP16_WHEADER_SIGN_OF_BASE)== BFP16_WHEADER_SIGN_OF_BASE_POSITIVE) //in_pUD_1 >= 0
				{
					*in_pUD_Result = in_pUD_2;
					in_pUD_Result->SEF &= BFP16_WHEADER_EXP_VALUE_MASK;
					SUB(in_pUD_Result, in_pUD_1, *in_pUD_Result);
				}
				else
				{
					*in_pUD_Result = in_pUD_1;
					in_pUD_Result->SEF &= BFP16_WHEADER_EXP_VALUE_MASK;
					SUB(in_pUD_Result, in_pUD_2, *in_pUD_Result);
				}
			}
			else
			{
				if ((in_pUD_1.SEF & BFP16_WHEADER_INFINITY) == BFP16_WHEADER_INFINITY)			CBFP16::SetValue(in_pUD_Result, in_pUD_1);
				else  if ((in_pUD_2.SEF & BFP16_WHEADER_INFINITY) == BFP16_WHEADER_INFINITY)		CBFP16::SetValue(in_pUD_Result, in_pUD_2);
				else
				{//ADD  by DWORD  (byte order !!!!)
					__int64 iS1;
					unsigned char SEF;
					int dltExp = in_pUD_2.SEF - in_pUD_1.SEF;

					if (!dltExp)
					{
						iS1 = ((((__int64)in_pUD_1.Mantissa[2]) << BFP16_MANTISSA_MOVE_2) + (((__int64)in_pUD_1.Mantissa[1]) << BFP16_MANTISSA_MOVE_1) + (__int64)in_pUD_1.Mantissa[0]) + ((((__int64)in_pUD_2.Mantissa[2]) << BFP16_MANTISSA_MOVE_2) + (((__int64)in_pUD_2.Mantissa[1]) << BFP16_MANTISSA_MOVE_1) + (__int64)in_pUD_2.Mantissa[0]);
						SEF = in_pUD_1.SEF;
					}
					else
						if (dltExp > 0)
						{
							iS1 = ((((__int64)in_pUD_1.Mantissa[2]) << BFP16_MANTISSA_MOVE_2) + (((__int64)in_pUD_1.Mantissa[1]) << BFP16_MANTISSA_MOVE_1) + (__int64)in_pUD_1.Mantissa[0]) >> ((dltExp << 7) - 1);
							if (iS1 & 0x1) iS1 += 2;
							iS1 = (iS1 >>= 1) + ((((__int64)in_pUD_2.Mantissa[2]) << BFP16_MANTISSA_MOVE_2) + (((__int64)in_pUD_2.Mantissa[1]) << BFP16_MANTISSA_MOVE_1) + (__int64)in_pUD_2.Mantissa[0]);
							SEF = in_pUD_2.SEF;
						}
						else
						{
							iS1 = ((((__int64)in_pUD_2.Mantissa[2]) << BFP16_MANTISSA_MOVE_2) + (((__int64)in_pUD_2.Mantissa[1]) << BFP16_MANTISSA_MOVE_1) + (__int64)in_pUD_2.Mantissa[0]) >> (((-dltExp) << 7) - 1);
							if (iS1 & 0x1) iS1 += 2;
							iS1 = (iS1 >>= 1) + (((__int64)in_pUD_1.Mantissa[2]) << BFP16_MANTISSA_MOVE_2) + (((__int64)in_pUD_1.Mantissa[1]) << BFP16_MANTISSA_MOVE_1) + (__int64)in_pUD_1.Mantissa[0];
							SEF = in_pUD_1.SEF;
						}

					if ((iS1 & 0xFF000000000000)) //переполнение
					{
						iS1 += 0x7FFF;
						iS1 >>= BFP16_MANTISSA_MOVE_1;

						if ((SEF & BFP16_WHEADER_SIGN_OF_EXP))
						{//Exp >= 0
							if ((SEF & BFP16_WHEADER_EXP_VALUE_MASK) < BFP16_WHEADER_INFINITY) SEF++;
						}
						else
						{//Exp <0
							SEF++;
						}
					}
					in_pUD_Result->Mantissa[2] = (unsigned short)(iS1 >> BFP16_MANTISSA_MOVE_2);
					in_pUD_Result->Mantissa[1] = (unsigned short)(iS1 >> BFP16_MANTISSA_MOVE_1);
					in_pUD_Result->Mantissa[0] = (unsigned short)iS1;
					in_pUD_Result->SEF = SEF;
				}
			}
		}
		else  in_pUD_Result->SEF |= BFP16_WHEADER_UNKNOWN;
	}

	return in_pUD_Result;
}

PBFP16 CBFP16::SUB(PBFP16 in_pUD_Result, const BFP16 &in_pUD_1, const BFP16 &in_pUD_2)
{
	if (in_pUD_Result)
	{
		if ((in_pUD_1.SEF & BFP16_WHEADER_UNKNOWN) != BFP16_WHEADER_UNKNOWN	&& (in_pUD_2.SEF & BFP16_WHEADER_UNKNOWN) != BFP16_WHEADER_UNKNOWN)
		{
			if (((in_pUD_1.SEF ^ in_pUD_2.SEF) & BFP16_WHEADER_SIGN_OF_BASE)) //разные знаки
			{
				if ((in_pUD_1.SEF & BFP16_WHEADER_SIGN_OF_BASE) == BFP16_WHEADER_SIGN_OF_BASE_POSITIVE) //in_pUD_1 >= 0
				{
					*in_pUD_Result = in_pUD_2;
					in_pUD_Result->SEF &= BFP16_WHEADER_EXP_VALUE_MASK;
					ADD(in_pUD_Result, in_pUD_1, *in_pUD_Result);
				}
				else
				{
					*in_pUD_Result = in_pUD_1;
					in_pUD_Result->SEF &= BFP16_WHEADER_EXP_VALUE_MASK;
					ADD(in_pUD_Result, in_pUD_2, *in_pUD_Result);
				}
			}
			else
			{
				if ((in_pUD_1.SEF & BFP16_WHEADER_INFINITY) == BFP16_WHEADER_INFINITY)			*in_pUD_Result = in_pUD_1;
				else
					if ((in_pUD_2.SEF & BFP16_WHEADER_INFINITY) == BFP16_WHEADER_INFINITY)		*in_pUD_Result = in_pUD_2;
					else
					{	//SUB byte by byte
						BFP16 tmp_1;
						BFP16 tmp_2;

						if (*((unsigned __int64*)&in_pUD_1) >= *((unsigned __int64*)&in_pUD_2))
						{
							in_pUD_Result->SEF = in_pUD_1.SEF;
							tmp_1 = in_pUD_1;
							tmp_2 = in_pUD_2;
							RollRight(&tmp_2, in_pUD_1.SEF - in_pUD_2.SEF);
						}
						else
						{
							in_pUD_Result->SEF = in_pUD_2.SEF;
							in_pUD_Result->SEF ^= BFP16_WHEADER_SIGN_OF_BASE;
							tmp_1 = in_pUD_2;
							tmp_2 = in_pUD_1;
							RollRight(&tmp_1, in_pUD_2.SEF - in_pUD_1.SEF);
						}


						__int64 tmp;
						tmp = 0x10000 + tmp_1.Mantissa[0] - tmp_2.Mantissa[0];
						in_pUD_Result->Mantissa[0] = (unsigned short)tmp;

						if ((tmp & 0x10000))	tmp = 0x10000 + tmp_1.Mantissa[1] - tmp_2.Mantissa[1];
						else				tmp = 0x10000 + tmp_1.Mantissa[1] - tmp_2.Mantissa[1] - 1;

						in_pUD_Result->Mantissa[1] = (unsigned short)tmp;

						if ((tmp & 0x10000))	tmp = 0x10000 + tmp_1.Mantissa[2] - tmp_2.Mantissa[2];
						else				tmp = 0x10000 + tmp_1.Mantissa[2] - tmp_2.Mantissa[2] - 1;

						in_pUD_Result->Mantissa[2] = (unsigned short)tmp;

						if (in_pUD_Result->Mantissa[2] == 0)
						{
							if (in_pUD_Result->Mantissa[1] == 0)
							{
								if (in_pUD_Result->Mantissa[0] == 0)
								{// результат == 0
									in_pUD_Result->SEF = BFP16_WHEADER_SIGN_OF_BASE_POSITIVE | BFP16_WHEADER_SIGN_OF_EXP_POSITIVE;
								}
								else
								{
									in_pUD_Result->Mantissa[2] = in_pUD_Result->Mantissa[0];
									in_pUD_Result->Mantissa[0] = 0;

									if ((in_pUD_Result->SEF & BFP16_WHEADER_SIGN_OF_EXP))
									{//Exp >= 0
										in_pUD_Result->SEF-= 2;
									}
									else
									{//Exp <0
										if ((in_pUD_Result->SEF & BFP16_WHEADER_EXP_VALUE_MASK) > 3) in_pUD_Result->SEF -= 2;
										else
										{
											in_pUD_Result->SEF &= BFP16_WHEADER_SIGN_OF_BASE;
											in_pUD_Result->Mantissa[2] = 0;
										}
									}
								}
							}
							else
							{
								in_pUD_Result->Mantissa[2] = in_pUD_Result->Mantissa[1];
								in_pUD_Result->Mantissa[1] = in_pUD_Result->Mantissa[0];
								in_pUD_Result->Mantissa[0] = 0;
								if ((in_pUD_Result->SEF & BFP16_WHEADER_SIGN_OF_EXP))
								{//Exp >= 0
									in_pUD_Result->SEF--;
								}
								else
								{//Exp <0
									if ((in_pUD_Result->SEF & BFP16_WHEADER_EXP_VALUE_MASK) > 2) in_pUD_Result->SEF--;
									else
										InitValue(in_pUD_Result);
								}
							}
						}
					}
			}
		}
		else  in_pUD_Result->SEF |= BFP16_WHEADER_UNKNOWN;
	}

	return in_pUD_Result;
}


PBFP16 CBFP16::MUL(PBFP16 in_pUD_Result, const BFP16 &in_pUD_1, const BFP16 &in_pUD_2)
{
	if (in_pUD_Result)
	{
		//CBFP16::InitValue(in_pUD_Result);

		if ((in_pUD_1.SEF & BFP16_WHEADER_UNKNOWN) != BFP16_WHEADER_UNKNOWN	&& (in_pUD_2.SEF & BFP16_WHEADER_UNKNOWN) != BFP16_WHEADER_UNKNOWN	)
		{
			if ((in_pUD_1.SEF & BFP16_WHEADER_INFINITY) == BFP16_WHEADER_INFINITY)
			{
				CBFP16::SetValue(in_pUD_Result, in_pUD_1);
				in_pUD_Result->SEF ^= (in_pUD_2.SEF & BFP16_WHEADER_SIGN_OF_BASE);
				if (in_pUD_2.Mantissa[2] == 0) in_pUD_Result->SEF |= BFP16_WHEADER_UNKNOWN;
			}
			else if ((in_pUD_2.SEF & BFP16_WHEADER_INFINITY) == BFP16_WHEADER_INFINITY)
			{
				CBFP16::SetValue(in_pUD_Result, in_pUD_2);
				in_pUD_Result->SEF ^= (in_pUD_1.SEF & BFP16_WHEADER_SIGN_OF_BASE);
				if (in_pUD_1.Mantissa[2] == 0) in_pUD_Result->SEF |= BFP16_WHEADER_UNKNOWN;
			}
			else
				//if (in_pUD_1.Mantissa[2] == 0 || in_pUD_2.Mantissa[2] == 0) InitValue(in_pUD_Result);
				//else
			{	//MUL
				unsigned __int64 iS = (((__int64)in_pUD_1.Mantissa[2]) << BFP16_MANTISSA_MOVE_2) + (((__int64)in_pUD_1.Mantissa[1]) << BFP16_MANTISSA_MOVE_1) + (__int64)in_pUD_1.Mantissa[0];
				unsigned __int64 iS0 = iS * ((__int64)in_pUD_2.Mantissa[0]);
				unsigned __int64 iS1 = iS * ((__int64)in_pUD_2.Mantissa[1]);
				//unsigned __int64 iS2 = iS * ((__int64)in_pUD_2.Mantissa[2]);

				unsigned __int64 iRes = iS * ((__int64)in_pUD_2.Mantissa[2]) + (iS1 >> BFP16_MANTISSA_MOVE_1) + (iS0 >> BFP16_MANTISSA_MOVE_2) ;
				iRes +=(((iS0 >> BFP16_MANTISSA_MOVE_1)& 0xFFFF) + (iS1 & 0xFFFF))>> BFP16_MANTISSA_MOVE_1;
				
				int newExp = (in_pUD_1.SEF & BFP16_WHEADER_EXP_VALUE_MASK) + (in_pUD_2.SEF & BFP16_WHEADER_EXP_VALUE_MASK) - 0x40;

				if ((iRes & 0xFF00000000000000))	
				{
					iRes += 0x8000;
					newExp += 1;
					iRes >>= BFP16_MANTISSA_MOVE_1;
				}
	
				if (newExp <= BFP16_WHEADER_CLOSE_TO_0) //стремится к нулю со стороны знака мантиссы
					in_pUD_Result->SEF = ((~(in_pUD_1.SEF ^ in_pUD_2.SEF)) & BFP16_WHEADER_SIGN_OF_BASE);
				else if (newExp >= BFP16_WHEADER_INFINITY)
					in_pUD_Result->SEF = ((~(in_pUD_1.SEF ^ in_pUD_2.SEF)) & BFP16_WHEADER_SIGN_OF_BASE) | BFP16_WHEADER_INFINITY;
				else
				{
					in_pUD_Result->Mantissa[2] = (unsigned short)(iRes >> BFP16_MANTISSA_MOVE_2);
					in_pUD_Result->Mantissa[1] = (unsigned short)(iRes >> BFP16_MANTISSA_MOVE_1);
					in_pUD_Result->Mantissa[0] = (unsigned short)iRes;
					in_pUD_Result->SEF = (newExp & BFP16_WHEADER_EXP_VALUE_MASK) | ((~(in_pUD_1.SEF ^ in_pUD_2.SEF)) & BFP16_WHEADER_SIGN_OF_BASE);
				}
			}
		}
		else  in_pUD_Result->SEF |= BFP16_WHEADER_UNKNOWN;
	}

	return in_pUD_Result;
}


PBFP16 CBFP16::DIV(PBFP16 in_pUD_Result, const BFP16 &in_pUD_1, const BFP16 &in_pUD_2)
{
	if (in_pUD_Result)
	{
		if ((in_pUD_1.SEF & BFP16_WHEADER_UNKNOWN) != BFP16_WHEADER_UNKNOWN && (in_pUD_2.SEF & BFP16_WHEADER_UNKNOWN) != BFP16_WHEADER_UNKNOWN)
		{
			if (in_pUD_2.Mantissa[2] == 0) in_pUD_Result->SEF |= BFP16_WHEADER_UNKNOWN;
			else if ((in_pUD_1.SEF & BFP16_WHEADER_INFINITY) == BFP16_WHEADER_INFINITY)
			{
				CBFP16::SetValue(in_pUD_Result, in_pUD_1);
				in_pUD_Result->SEF ^= (in_pUD_2.SEF & BFP16_WHEADER_SIGN_OF_BASE);
				if ((in_pUD_2.SEF & BFP16_WHEADER_INFINITY) == BFP16_WHEADER_INFINITY) in_pUD_Result->SEF |= BFP16_WHEADER_UNKNOWN;
			}
			else if ((in_pUD_2.SEF & BFP16_WHEADER_INFINITY) == BFP16_WHEADER_INFINITY)
			{
				if (in_pUD_1.Mantissa[2] != 0)
				{
					in_pUD_Result->SEF = BFP16_WHEADER_CLOSE_TO_0;
					in_pUD_Result->SEF |= (in_pUD_2.SEF & BFP16_WHEADER_SIGN_OF_BASE);
				}
			}
			else if (in_pUD_1.Mantissa[2] == 0) InitValue(in_pUD_Result);
			else
			{	//DIV
				//for(;;){;}     t=3000 
				//x = i - j      t=4700   - 3000 = 1700
				//x = i + j      t=4600   - 3000 = 1600
				//x = i * j      t=4600   - 3000 = 1600
				//x = i / j      t=14000  - 3000 = 11000
				//x = i - x * j  t=5700   - 3000 = 2700 ????
				//x = i % j      t=14100  - 3000 = 11100

				int newExp = ((int)(in_pUD_1.SEF & BFP16_WHEADER_EXP_VALUE_MASK)) - ((int)(in_pUD_2.SEF & BFP16_WHEADER_EXP_VALUE_MASK)) + (int)0x40;
				__int64 iS1 = (((__int64)in_pUD_1.Mantissa[2]) << BFP16_MANTISSA_MOVE_2) + (((__int64)in_pUD_1.Mantissa[1]) << BFP16_MANTISSA_MOVE_1) + in_pUD_1.Mantissa[0];
				__int64 iS2 = (((__int64)in_pUD_2.Mantissa[2]) << BFP16_MANTISSA_MOVE_2) + (((__int64)in_pUD_2.Mantissa[1]) << BFP16_MANTISSA_MOVE_1) + in_pUD_2.Mantissa[0];
				__int64 iRes = iS1 / iS2;
				__int64 rem = (iS1 - iS2 * iRes); // так быстрее. T(%) > T(/) >= 4 * T(*)  = 7 * T(+-)

				if (iRes == 0) newExp--;

				while (rem && !(iRes & 0xFFFF00000000))
				{
					rem <<= BFP16_MANTISSA_MOVE_1;
					iRes <<= BFP16_MANTISSA_MOVE_1;
					if (rem >= iS2)
					{
						__int64 iTmp = rem / iS2;
						rem = (rem - iS2 * iTmp);
						iRes += iTmp;
					}
				} 

				if ((rem << BFP16_MANTISSA_MOVE_1) >= iS2) iRes++;

				if (newExp <= BFP16_WHEADER_CLOSE_TO_0) //стремится к нулю со стороны знака мантиссы
					in_pUD_Result->SEF = ((~(in_pUD_1.SEF ^ in_pUD_2.SEF)) & BFP16_WHEADER_SIGN_OF_BASE);
				else if (newExp >= BFP16_WHEADER_INFINITY)
					in_pUD_Result->SEF = ((~(in_pUD_1.SEF ^ in_pUD_2.SEF)) & BFP16_WHEADER_SIGN_OF_BASE) | BFP16_WHEADER_INFINITY;
				else
				{
					in_pUD_Result->Mantissa[2] = (unsigned short)(iRes >> BFP16_MANTISSA_MOVE_2);
					in_pUD_Result->Mantissa[1] = (unsigned short)(iRes >> BFP16_MANTISSA_MOVE_1);
					in_pUD_Result->Mantissa[0] = (unsigned short)iRes;
					in_pUD_Result->SEF = (newExp & BFP16_WHEADER_EXP_VALUE_MASK) | ((~(in_pUD_1.SEF ^ in_pUD_2.SEF)) & BFP16_WHEADER_SIGN_OF_BASE);
				}
			}
		}
		else  in_pUD_Result->SEF |= BFP16_WHEADER_UNKNOWN;
	}

	return in_pUD_Result;
}

double CBFP16::DIVDBL(const double in_D_1, const double in_D_2)
{
	unsigned __int64 ret;

	unsigned __int64 i_inValue1 = (*((__int64*)(&in_D_1)));
	unsigned __int64 i_inValue2 = (*((__int64*)(&in_D_2)));

//	__int64 exp1 = ((i_inValue1 >> 52) & 0x7FF) - 0x3FF;
//	__int64 exp2 = ((i_inValue1 >> 52) & 0x7FF) - 0x3FF;
//	__int64 newExp = exp1 - exp2;

	__int64 newExp = ((i_inValue1 & 0x7FF0000000000000)<<1) - ((i_inValue2 & 0x7FF0000000000000)<<1); //+0x3FF0000000000000;
	//проверку на переполнение
	//провека на NAN ????

	unsigned __int64 mant1 = (i_inValue1 & 0x000FFFFFFFFFFFFF) | 0x0010000000000000;
	unsigned __int64 mant2 = (i_inValue2 & 0x000FFFFFFFFFFFFF) | 0x0010000000000000;


	if (i_inValue2)
	{
		if (i_inValue1 == 0) ret = 0;
		else
		{	//DIV
			unsigned __int64 rem; 

			if (mant1 < mant2)
			{
				newExp -= 0x0020000000000000;
				ret = 0;
				rem = mant1;
			}
			else
			{
				ret = 1;
				rem = mant1 - mant2; 
			}

			while (rem && !(ret & 0x001FFC0000000000))
			{
				rem <<= 0x0B;
				ret <<= 0x0B;
				if (rem >= 0x0010000000000000)
				{
					__int64 iTmp = rem / mant2;// так быстрее. T(%) > T(/) >= 4 * T(*)  = 7 * T(+-)
					rem = (rem - mant2 * iTmp);// так быстрее. T(%) > T(/) >= 4 * T(*)  = 7 * T(+-)
					ret += iTmp;
				}
			}

			while (rem && !(ret & 0x001FE00000000000))
			{
				rem <<= 0x8;
				ret <<= 0x8;
				if (rem >= 0x0010000000000000)
				{
					__int64 iTmp = rem / mant2;
					rem = (rem - mant2 * iTmp);
					ret += iTmp;
				}
			}
/**/
			while (rem && !(ret & 0x001E000000000000))
			{
				rem <<= 0x4;
				ret <<= 0x4;
				if (rem >= 0x0010000000000000)
				{
					__int64 iTmp = rem / mant2;
					rem = (rem - mant2 * iTmp);
					ret += iTmp;
				}
			}

			while (rem && !(ret & 0x0018000000000000))
			{
				rem <<= 0x2;
				ret <<= 0x2;
				if (rem >= 0x0010000000000000)
				{
					__int64 iTmp = rem / mant2;
					rem = (rem - mant2 * iTmp);
					ret += iTmp;
				}
			}


			while (rem && !(ret & 0x0010000000000000))
			{
				rem <<= 0x1;
				ret <<= 0x1;
				if (rem >= mant2)
				{
					rem = rem - mant2;
					ret ++;
				}
			}

			/*
			//	0x10,				0x0F,				0x0E,			0x0D,				0x0C,				0x0B,			0x0A,				0x09,				0x08,				0x07,			0x06,				0x05,				0x04,				0x03,				0x02,				0x01,
			//,0x001FFFE000000000,0x001FFFC000000000,0x001FFF8000000000,0x001FFF0000000000,0x001FFE0000000000,0x001FFC0000000000,0x001FF80000000000,0x001FF00000000000,0x001FE00000000000,0x001FC00000000000,0x001F800000000000,0x001F000000000000,0x001E000000000000,0x001C000000000000,0x0018000000000000,0x0010000000000000
			unsigned __int64 iMove[5] = { 0x0B,					0x8,				0x4,				0x2,				0x1};
			unsigned __int64 iMask[5] = { 0x001FFC0000000000,  0x001FE00000000000,  0x001E000000000000, 0x0018000000000000, 0x0010000000000000 };
			int i = 0;

			while (i < 5)
			{
				while (rem && !(ret & iMask[i]))
				{
					rem <<= iMove[i];
					ret <<= iMove[i];
					if (rem >= mant2)
					{
						__int64 iTmp = rem / mant2;
						rem = (rem - mant2 * iTmp);
						ret += iTmp;
					}
				}
				i++;
			}
			*/


			if ((rem << 1) >= mant2) ret++;

			ret &= 0x000FFFFFFFFFFFFF;
			//проверка newExp на переполнение
			//провека на NAN ???? {}
			ret |=  (((newExp >> 1)+ 0x3FF0000000000000)   & 0x7FF0000000000000);

			ret |= ((i_inValue1 ^ i_inValue2) & 0x8000000000000000); //знак числа
		}
	}
	else  ret = 0x7ff0000000000000 | ((i_inValue1 ^ i_inValue2) & 0x8000000000000000); //Деление на ноль это бесконечность. Деление на ноль возникает как алгоритмическая ошибка. Лучше купировать ее бесконечностью, которую легче найти и отладить без исключений

	return *((double *) &ret);
}



PBFP16 CBFP16::SetValue(PBFP16 in_pUD_Result, const BFP16 &in_pUD_Src)
{
	if (in_pUD_Result) *in_pUD_Result = in_pUD_Src;

	return in_pUD_Result;
}

unsigned int CBFP16::CmpValue(const BFP16 &in_pUD_1, const BFP16 &in_pUD_2)
{
	unsigned int ret = 0;


	return ret;
}


unsigned int CBFP16::Tounsignedint(const BFP16 &in_pUD)
{
	//!!!!!!!!should be annother lowlevel fast method
	//! Этот вариант только для проверки И понимания
	unsigned int ret = 0;
	CBFP16::udLastOperationFlags = 0;

	if ((in_pUD.SEF & BFP16_WHEADER_SIGN_OF_BASE) == BFP16_WHEADER_SIGN_OF_BASE_POSITIVE)
	{
		int tmpExpValue = (in_pUD.SEF & BFP16_WHEADER_EXP_VALUE_MASK) - 0x40;

		if ((in_pUD.SEF & BFP16_WHEADER_SIGN_OF_EXP) == BFP16_WHEADER_SIGN_OF_EXP_NEGATIVE)		CBFP16::udLastOperationFlags = BFP16_LOFLAG_NAN;
		else if (tmpExpValue > 3)	CBFP16::udLastOperationFlags = BFP16_LOFLAG_OVER;
		else
		{
			if (tmpExpValue >= 2)
			{
				ret = (((__int64)in_pUD.Mantissa[2]) << BFP16_MANTISSA_MOVE_2) + (((__int64)in_pUD.Mantissa[1]) << BFP16_MANTISSA_MOVE_1) + in_pUD.Mantissa[0];
				if (tmpExpValue == 2) ret <<= BFP16_MANTISSA_MOVE_1;
			}
			else if (tmpExpValue == 1)
			{
				ret = (((__int64)in_pUD.Mantissa[2]) << BFP16_MANTISSA_MOVE_1) + in_pUD.Mantissa[1];
				if ((in_pUD.Mantissa[0] & 0x80)) ret++; //желательно установить флаг потери точности
			}
			else //if (tmpExpValue == 0)
			{
				ret = in_pUD.Mantissa[2];
				if ((in_pUD.Mantissa[1] & 0x80)) ret++;//желательно установить флаг потери точности
				else
					if (in_pUD.Mantissa[1] == 0x7F && (in_pUD.Mantissa[0] & 0x80)) ret++;//желательно установить флаг потери точности
			}
		}
	}
	else CBFP16::udLastOperationFlags = BFP16_LOFLAG_INVALID_DATA;

	return ret;
}

double CBFP16::ToDouble(const BFP16 &in_pUD)
{
	unsigned __int64 ret;
	if ((in_pUD.SEF & BFP16_WHEADER_EXP_VALUE_MASK) == BFP16_WHEADER_INFINITY) ret = 0x7ff0000000000000;
	else if ((in_pUD.SEF & BFP16_WHEADER_EXP_VALUE_MASK) == BFP16_WHEADER_SOMETHIG) ret = 0; ///реализация чего-либо
	else if ((in_pUD.SEF & BFP16_WHEADER_EXP_VALUE_MASK) == BFP16_WHEADER_CLOSE_TO_0) ret = 0x7fffffffffffffff;
	else if ((in_pUD.SEF & BFP16_WHEADER_EXP_VALUE_MASK) == BFP16_WHEADER_UNKNOWN) ret = 0x7ff0000000000000; //не нашел в double аналог
	else
	{
		ret = (((__int64)in_pUD.Mantissa[2]) << BFP16_MANTISSA_MOVE_2) + (((__int64)in_pUD.Mantissa[1]) << BFP16_MANTISSA_MOVE_1) + in_pUD.Mantissa[0];
		__int64 newExp = ((in_pUD.SEF & BFP16_WHEADER_EXP_VALUE_MASK) - 0x40) * 0x10 + 0x14;

		while (ret && !(ret & 0x0010000000000000))
		{
			ret <<= 0x1;
			newExp--;
		}

		ret &= 0x000FFFFFFFFFFFFF;
		ret |= (((newExp + 1023) << 52) & 0x7FF0000000000000);
	}

	if (!(in_pUD.SEF & BFP16_WHEADER_SIGN_OF_BASE)) 
		ret |= 0x8000000000000000; //знак числа

	return *((double*)&ret);
}

long double CBFP16::ToLongDouble(const BFP16 &in_pUD)
{
	return ToDouble(  in_pUD);
}

float CBFP16::ToFloat(const BFP16 &in_pUD)
{
	float ret = 0.0;

	if ((in_pUD.SEF & BFP16_WHEADER_SIGN_OF_BASE) == BFP16_WHEADER_SIGN_OF_BASE_NEGATIVE)
		ret = (float)((((__int64)in_pUD.Mantissa[2]) << BFP16_MANTISSA_MOVE_2) + (((__int64)in_pUD.Mantissa[1]) << BFP16_MANTISSA_MOVE_1) + in_pUD.Mantissa[0]);
	else
		ret = -((float)(((((__int64)in_pUD.Mantissa[2]) << BFP16_MANTISSA_MOVE_2) + (((__int64)in_pUD.Mantissa[1]) << BFP16_MANTISSA_MOVE_1) + in_pUD.Mantissa[0])));

	int n = (in_pUD.SEF & BFP16_WHEADER_EXP_VALUE_MASK) - BFP16_WHEADER_SIGN_OF_EXP_POSITIVE - 2;

	if (n >= 0)
		while (n-- > 0) ret *= 256.0;// 0x100;
	else
		while (n++ < 0) ret /= 256.0;// 0x100;


	return ret;
}

__int64 CBFP16::To__int64(const BFP16 &in_pUD)
{
	//!!!!!!!!should be annother lowlevel fast method
	//! Этот вариант только для проверки И понимания
	__int64 ret = 0;

	if ((in_pUD.SEF & BFP16_WHEADER_SIGN_OF_BASE) == BFP16_WHEADER_SIGN_OF_BASE_NEGATIVE)
		ret = ((((__int64)in_pUD.Mantissa[2]) << BFP16_MANTISSA_MOVE_2) + (((__int64)in_pUD.Mantissa[1]) << BFP16_MANTISSA_MOVE_1) + in_pUD.Mantissa[0]);
	else
		ret = -((__int64)((((__int64)in_pUD.Mantissa[2]) << BFP16_MANTISSA_MOVE_2) + (((__int64)in_pUD.Mantissa[1]) << BFP16_MANTISSA_MOVE_1) + in_pUD.Mantissa[0]));


	int n = (in_pUD.SEF & BFP16_WHEADER_EXP_VALUE_MASK) - BFP16_WHEADER_SIGN_OF_EXP_POSITIVE - 2;

	if (n >= 0)
		while (n-- > 0) ret <<= BFP16_MANTISSA_MOVE_1;// 0x100;
	else
		while (n++ < 0) ret >>= BFP16_MANTISSA_MOVE_1;// 0x100;


	return ret;

}

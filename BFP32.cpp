
#include "BFP32.h"


unsigned int CBFP32::udLastOperationFlags;

CBFP32::CBFP32()
{
}


CBFP32::~CBFP32()
{
}

PBFP32 CBFP32::InitValue(PBFP32 in_pUD_Result)
{
	if (in_pUD_Result)
	{
		in_pUD_Result->Mantissa[0] = 0;
		in_pUD_Result->Mantissa[1] = 0;
		in_pUD_Result->Mantissa[2] = 0;
		in_pUD_Result->SEF = BFP32_WHEADER_SIGN_OF_BASE_POSITIVE | BFP32_WHEADER_SIGN_OF_EXP_POSITIVE;
	}

	return in_pUD_Result;
}


PBFP32 CBFP32::RollRight(PBFP32 in_pBFP32_Result, int nTimes)
{
	if (nTimes == 1)
	{
		if ((in_pBFP32_Result->Mantissa[0] & BFP32_MANTISSA_HALF)) //переполнение и инкремент при сдвиге
		{
			if ((in_pBFP32_Result->Mantissa[1] == BFP32_MANTISSA_MAX))
			{
				in_pBFP32_Result->Mantissa[0] = 0;
				if ((in_pBFP32_Result->Mantissa[2] == BFP32_MANTISSA_MAX))
				{
					in_pBFP32_Result->Mantissa[1] = 0;
					in_pBFP32_Result->Mantissa[2] = 1;
				}
				else
				{
					in_pBFP32_Result->Mantissa[1] = in_pBFP32_Result->Mantissa[2] + 1;
					in_pBFP32_Result->Mantissa[2] = 0;
				}
			}
			else
			{
				in_pBFP32_Result->Mantissa[0] = in_pBFP32_Result->Mantissa[1] + 1;
				in_pBFP32_Result->Mantissa[1] = in_pBFP32_Result->Mantissa[2];
				in_pBFP32_Result->Mantissa[2] = 0;
			}
		}
		else
		{
			in_pBFP32_Result->Mantissa[0] = in_pBFP32_Result->Mantissa[1];
			in_pBFP32_Result->Mantissa[1] = in_pBFP32_Result->Mantissa[2];
			in_pBFP32_Result->Mantissa[2] = 0;
		}
	}
	else if (nTimes == 2)
	{
		if ((in_pBFP32_Result->Mantissa[1] & BFP32_MANTISSA_HALF)) //переполнение и инкремент при сдвиге
		{
			if ((in_pBFP32_Result->Mantissa[2] == BFP32_MANTISSA_MAX))
			{
				in_pBFP32_Result->Mantissa[0] = 0;
				in_pBFP32_Result->Mantissa[1] = 1;
			}
			else
			{
				in_pBFP32_Result->Mantissa[0] = in_pBFP32_Result->Mantissa[2] + 1;
				in_pBFP32_Result->Mantissa[1] = 0;
			}
		}
		else
		{
			in_pBFP32_Result->Mantissa[0] = in_pBFP32_Result->Mantissa[2];
			in_pBFP32_Result->Mantissa[1] = 0;
		}
		in_pBFP32_Result->Mantissa[2] = 0;
	}
	else if (nTimes == 3)
	{
		if ((in_pBFP32_Result->Mantissa[2] & BFP32_MANTISSA_HALF)) //переполнение и инкремент при сдвиге
			in_pBFP32_Result->Mantissa[0] = 1;
		else
			in_pBFP32_Result->Mantissa[0] = 0;

		in_pBFP32_Result->Mantissa[1] = 0;
		in_pBFP32_Result->Mantissa[2] = 0;

	}
	else if (nTimes > 3)
	{
		in_pBFP32_Result->Mantissa[0] = 0;
		in_pBFP32_Result->Mantissa[1] = 0;
		in_pBFP32_Result->Mantissa[2] = 0;
	}


	return in_pBFP32_Result;
}

PBFP32 CBFP32::ADD(PBFP32 in_pUD_Result, const BFP32 &in_pUD_1, const BFP32 &in_pUD_2)
{
	if (in_pUD_Result)
	{
		if ( (in_pUD_1.SEF & BFP32_WHEADER_UNKNOWN) != BFP32_WHEADER_UNKNOWN && (in_pUD_2.SEF & BFP32_WHEADER_UNKNOWN) != BFP32_WHEADER_UNKNOWN	)
		{
			if (((in_pUD_1.SEF ^ in_pUD_2.SEF) & BFP32_WHEADER_SIGN_OF_BASE)) //разные знаки
			{
				if ((in_pUD_1.SEF & BFP32_WHEADER_SIGN_OF_BASE)== BFP32_WHEADER_SIGN_OF_BASE_POSITIVE) //in_pUD_1 >= 0
				{
					*in_pUD_Result = in_pUD_2;
					in_pUD_Result->SEF &= BFP32_WHEADER_EXP_VALUE_MASK;
					SUB(in_pUD_Result, in_pUD_1, *in_pUD_Result);
				}
				else
				{
					*in_pUD_Result = in_pUD_1;
					in_pUD_Result->SEF &= BFP32_WHEADER_EXP_VALUE_MASK;
					SUB(in_pUD_Result, in_pUD_2, *in_pUD_Result);
				}
			}
			else
			{
				if ((in_pUD_1.SEF & BFP32_WHEADER_INFINITY) == BFP32_WHEADER_INFINITY)			CBFP32::SetValue(in_pUD_Result, in_pUD_1);
				else
					if ((in_pUD_2.SEF & BFP32_WHEADER_INFINITY) == BFP32_WHEADER_INFINITY)		CBFP32::SetValue(in_pUD_Result, in_pUD_2);
					else
					{//ADD  by DWORD  (byte order !!!!)
						__int64 iS1;
						unsigned short SEF;
						int dltExp = in_pUD_2.SEF - in_pUD_1.SEF;


						if (!dltExp)
						{
							iS1 = ((((__int64)in_pUD_1.Mantissa[2]) << BFP32_MANTISSA_MOVE_2) + (((__int64)in_pUD_1.Mantissa[1]) << BFP32_MANTISSA_MOVE_1) + (__int64)in_pUD_1.Mantissa[0]) + ((((__int64)in_pUD_2.Mantissa[2]) << BFP32_MANTISSA_MOVE_2) + (((__int64)in_pUD_2.Mantissa[1]) << BFP32_MANTISSA_MOVE_1) + (__int64)in_pUD_2.Mantissa[0]);
							SEF = in_pUD_1.SEF;
						}
						else
						if (dltExp > 0)
						{
							iS1 = ((((__int64)in_pUD_1.Mantissa[2]) << BFP32_MANTISSA_MOVE_2) + (((__int64)in_pUD_1.Mantissa[1]) << BFP32_MANTISSA_MOVE_1) + (__int64)in_pUD_1.Mantissa[0]) >> ((dltExp << 3) - 1);
							if (iS1 & 0x1) iS1 += 2;
							iS1 = (iS1 >>= 1) + ((((__int64)in_pUD_2.Mantissa[2]) << BFP32_MANTISSA_MOVE_2) + (((__int64)in_pUD_2.Mantissa[1]) << BFP32_MANTISSA_MOVE_1) + (__int64)in_pUD_2.Mantissa[0]);
							SEF = in_pUD_2.SEF;
						}
						else
						{
							iS1 = ((((__int64)in_pUD_2.Mantissa[2]) << BFP32_MANTISSA_MOVE_2) + (((__int64)in_pUD_2.Mantissa[1]) << BFP32_MANTISSA_MOVE_1) + (__int64)in_pUD_2.Mantissa[0]) >> (((- dltExp) << 3) - 1);
							if (iS1 & 0x1) iS1 += 2;
							iS1 = (iS1 >>= 1) + (((__int64)in_pUD_1.Mantissa[2]) << BFP32_MANTISSA_MOVE_2) + (((__int64)in_pUD_1.Mantissa[1]) << BFP32_MANTISSA_MOVE_1) + (__int64)in_pUD_1.Mantissa[0];
							SEF = in_pUD_1.SEF;
						}

						//iS1 += iS2;

						if ((iS1 & 0xFF000000)) //переполнение
						{
							iS1 += 0x7F;
							iS1 >>= BFP32_MANTISSA_MOVE_1;

							if ((SEF & BFP32_WHEADER_SIGN_OF_EXP))
							{//Exp >= 0
								if ((SEF & BFP32_WHEADER_EXP_VALUE_MASK) < BFP32_WHEADER_INFINITY) SEF++;
							}
							else
							{//Exp <0
								SEF++;
							}
						}
   
						in_pUD_Result->Mantissa[2] = (unsigned short)(iS1 >> BFP32_MANTISSA_MOVE_2);
						in_pUD_Result->Mantissa[1] = (unsigned short)(iS1 >> BFP32_MANTISSA_MOVE_1);
						in_pUD_Result->Mantissa[0] = (unsigned short)iS1;
						in_pUD_Result->SEF = SEF;

					}
					//{	//ADD  byte by byte
					//	BFP32 tmp_1 = *in_pUD_1;
					//	BFP32 tmp_2 = *in_pUD_2;

					//	if (in_pUD_1.SEF > in_pUD_2.SEF)
					//	{
					//		RollRight(&tmp_2, in_pUD_1.SEF - in_pUD_2.SEF);
					//		in_pUD_Result->SEF = tmp_1.SEF;
					//	}
					//	else
					//		if (in_pUD_1.SEF < in_pUD_2.SEF)
					//		{
					//			RollRight(&tmp_1, in_pUD_2.SEF - in_pUD_1.SEF);
					//			in_pUD_Result->SEF = tmp_2.SEF;
					//		}
					//		else 
					//			in_pUD_Result->SEF = tmp_1.SEF;

					//	__int64 tmp;
					//	tmp = tmp_1.Mantissa[0] + tmp_2.Mantissa[0];
					//	in_pUD_Result->Mantissa[0] = tmp;

					//	if ((tmp & 0x100))	tmp = tmp_1.Mantissa[1] + tmp_2.Mantissa[1] + 1;
					//	else				tmp = tmp_1.Mantissa[1] + tmp_2.Mantissa[1];

					//	in_pUD_Result->Mantissa[1] = tmp;

					//	if ((tmp & 0x100))	tmp = tmp_1.Mantissa[2] + tmp_2.Mantissa[2] + 1;
					//	else				tmp = tmp_1.Mantissa[2] + tmp_2.Mantissa[2];

					//	in_pUD_Result->Mantissa[2] = tmp;

					//	if ((tmp & 0x100)) //переполнение
					//	{
					//		RollRight(in_pUD_Result, 1);
					//		in_pUD_Result->Mantissa[2] = 1;

					//		if ((in_pUD_Result->SEF & BFP32_WHEADER_SIGN_OF_EXP))
					//		{//Exp >= 0
					//			if ((in_pUD_Result->SEF & BFP32_WHEADER_EXP_VALUE_MASK) < BFP32_WHEADER_INFINITY) in_pUD_Result->SEF++;
					//		}
					//		else
					//		{//Exp <0
					//			in_pUD_Result->SEF++;
					//		}
					//	}
					//}
			}
		}
		else  in_pUD_Result->SEF |= BFP32_WHEADER_UNKNOWN;
	}

	return in_pUD_Result;
}

PBFP32 CBFP32::SUB(PBFP32 in_pUD_Result, const BFP32 &in_pUD_1, const BFP32 &in_pUD_2)
{
	if (in_pUD_Result)
	{
		if ((in_pUD_1.SEF & BFP32_WHEADER_UNKNOWN) != BFP32_WHEADER_UNKNOWN	&& (in_pUD_2.SEF & BFP32_WHEADER_UNKNOWN) != BFP32_WHEADER_UNKNOWN)
		{
			if (((in_pUD_1.SEF ^ in_pUD_2.SEF) & BFP32_WHEADER_SIGN_OF_BASE)) //разные знаки
			{
				if ((in_pUD_1.SEF & BFP32_WHEADER_SIGN_OF_BASE) == BFP32_WHEADER_SIGN_OF_BASE_POSITIVE) //in_pUD_1 >= 0
				{
					*in_pUD_Result = in_pUD_2;
					in_pUD_Result->SEF &= BFP32_WHEADER_EXP_VALUE_MASK;
					ADD(in_pUD_Result, in_pUD_1, *in_pUD_Result);
				}
				else
				{
					*in_pUD_Result = in_pUD_1;
					in_pUD_Result->SEF &= BFP32_WHEADER_EXP_VALUE_MASK;
					ADD(in_pUD_Result, in_pUD_2, *in_pUD_Result);
				}
			}
			else
			{
				if ((in_pUD_1.SEF & BFP32_WHEADER_INFINITY) == BFP32_WHEADER_INFINITY)			*in_pUD_Result = in_pUD_1;
				else
					if ((in_pUD_2.SEF & BFP32_WHEADER_INFINITY) == BFP32_WHEADER_INFINITY)		*in_pUD_Result = in_pUD_2;
					else
					{	//SUB byte by byte
						BFP32 tmp_1;
						BFP32 tmp_2;

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
							in_pUD_Result->SEF ^= BFP32_WHEADER_SIGN_OF_BASE;
							tmp_1 = in_pUD_2;
							tmp_2 = in_pUD_1;
							RollRight(&tmp_1, in_pUD_2.SEF - in_pUD_1.SEF);
						}


						__int64 tmp;
						tmp = 0x100 + tmp_1.Mantissa[0] - tmp_2.Mantissa[0];
						in_pUD_Result->Mantissa[0] = (unsigned short)tmp;

						if ((tmp & 0x100))	tmp = 0x100 + tmp_1.Mantissa[1] - tmp_2.Mantissa[1];
						else				tmp = 0x100 + tmp_1.Mantissa[1] - tmp_2.Mantissa[1] - 1;

						in_pUD_Result->Mantissa[1] = (unsigned short)tmp;

						if ((tmp & 0x100))	tmp = 0x100 + tmp_1.Mantissa[2] - tmp_2.Mantissa[2];
						else				tmp = 0x100 + tmp_1.Mantissa[2] - tmp_2.Mantissa[2] - 1;

						in_pUD_Result->Mantissa[2] = (unsigned short)tmp;

						if (in_pUD_Result->Mantissa[2] == 0)
						{
							if (in_pUD_Result->Mantissa[1] == 0)
							{
								if (in_pUD_Result->Mantissa[0] == 0)
								{// результат == 0
									in_pUD_Result->SEF = BFP32_WHEADER_SIGN_OF_BASE_POSITIVE | BFP32_WHEADER_SIGN_OF_EXP_POSITIVE;
								}
								else
								{
									in_pUD_Result->Mantissa[2] = in_pUD_Result->Mantissa[0];
									in_pUD_Result->Mantissa[0] = 0;

									if ((in_pUD_Result->SEF & BFP32_WHEADER_SIGN_OF_EXP))
									{//Exp >= 0
										in_pUD_Result->SEF-= 2;
									}
									else
									{//Exp <0
										if ((in_pUD_Result->SEF & BFP32_WHEADER_EXP_VALUE_MASK) > 3) in_pUD_Result->SEF -= 2;
										else
										{
											in_pUD_Result->SEF &= BFP32_WHEADER_SIGN_OF_BASE;
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
								if ((in_pUD_Result->SEF & BFP32_WHEADER_SIGN_OF_EXP))
								{//Exp >= 0
									in_pUD_Result->SEF--;
								}
								else
								{//Exp <0
									if ((in_pUD_Result->SEF & BFP32_WHEADER_EXP_VALUE_MASK) > 2) in_pUD_Result->SEF--;
									else
										InitValue(in_pUD_Result);
								}
							}
						}
					}
			}
		}
		else  in_pUD_Result->SEF |= BFP32_WHEADER_UNKNOWN;
	}

	return in_pUD_Result;
}


PBFP32 CBFP32::MUL(PBFP32 in_pUD_Result, const BFP32 &in_pUD_1, const BFP32 &in_pUD_2)
{
	if (in_pUD_Result)
	{
		//CBFP32::InitValue(in_pUD_Result);

		if ((in_pUD_1.SEF & BFP32_WHEADER_UNKNOWN) != BFP32_WHEADER_UNKNOWN	&& (in_pUD_2.SEF & BFP32_WHEADER_UNKNOWN) != BFP32_WHEADER_UNKNOWN	)
		{
			if ((in_pUD_1.SEF & BFP32_WHEADER_INFINITY) == BFP32_WHEADER_INFINITY)
			{
				CBFP32::SetValue(in_pUD_Result, in_pUD_1);
				in_pUD_Result->SEF ^= (in_pUD_2.SEF & BFP32_WHEADER_SIGN_OF_BASE);
				if (in_pUD_2.Mantissa[2] == 0) in_pUD_Result->SEF |= BFP32_WHEADER_UNKNOWN;
			}
			else if ((in_pUD_2.SEF & BFP32_WHEADER_INFINITY) == BFP32_WHEADER_INFINITY)
			{
				CBFP32::SetValue(in_pUD_Result, in_pUD_2);
				in_pUD_Result->SEF ^= (in_pUD_1.SEF & BFP32_WHEADER_SIGN_OF_BASE);
				if (in_pUD_1.Mantissa[2] == 0) in_pUD_Result->SEF |= BFP32_WHEADER_UNKNOWN;
			}
			else
				//if (in_pUD_1.Mantissa[2] == 0 || in_pUD_2.Mantissa[2] == 0) InitValue(in_pUD_Result);
				//else
			{	//MUL
				__int64 iS = (((__int64)in_pUD_1.Mantissa[2]) << BFP32_MANTISSA_MOVE_2) + (((__int64)in_pUD_1.Mantissa[1]) << BFP32_MANTISSA_MOVE_1) + (__int64)in_pUD_1.Mantissa[0];
				__int64 iRes = ((iS * ((__int64)in_pUD_2.Mantissa[2]) << BFP32_MANTISSA_MOVE_2)) + ((iS * ((__int64)in_pUD_2.Mantissa[1]) << BFP32_MANTISSA_MOVE_1)) + iS * (__int64)in_pUD_2.Mantissa[0];
				//__int64 iRes = ((__int64)(*((DWORD*)&in_pUD_1) & 0x00FFFFFF)) * ((__int64)(*((DWORD*)&in_pUD_2) & 0x00FFFFFF)); //slowly

				int newExp = (in_pUD_1.SEF & BFP32_WHEADER_EXP_VALUE_MASK) + (in_pUD_2.SEF & BFP32_WHEADER_EXP_VALUE_MASK) - 0x40;

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
					in_pUD_Result->SEF = ((~(in_pUD_1.SEF ^ in_pUD_2.SEF)) & BFP32_WHEADER_SIGN_OF_BASE);
				else if (newExp >= BFP32_WHEADER_INFINITY)
					in_pUD_Result->SEF = ((~(in_pUD_1.SEF ^ in_pUD_2.SEF)) & BFP32_WHEADER_SIGN_OF_BASE) | BFP32_WHEADER_INFINITY;
				else
				{
					in_pUD_Result->Mantissa[2] = (unsigned short)(iRes >> BFP32_MANTISSA_MOVE_2);
					in_pUD_Result->Mantissa[1] = (unsigned short)(iRes >> BFP32_MANTISSA_MOVE_1);
					in_pUD_Result->Mantissa[0] = (unsigned short)iRes;
					in_pUD_Result->SEF = (newExp & BFP32_WHEADER_EXP_VALUE_MASK) | ((~(in_pUD_1.SEF ^ in_pUD_2.SEF)) & BFP32_WHEADER_SIGN_OF_BASE);
				}
			}
		}
		else  in_pUD_Result->SEF |= BFP32_WHEADER_UNKNOWN;
	}

	return in_pUD_Result;
}
PBFP32 CBFP32::DIV(PBFP32 in_pUD_Result, const BFP32 &in_pUD_1, const BFP32 &in_pUD_2)
{
	if (in_pUD_Result)
	{
		CBFP32::InitValue(in_pUD_Result);

		if ((in_pUD_1.SEF & BFP32_WHEADER_UNKNOWN) != BFP32_WHEADER_UNKNOWN && (in_pUD_2.SEF & BFP32_WHEADER_UNKNOWN) != BFP32_WHEADER_UNKNOWN)
		{
			if (in_pUD_2.Mantissa[2] == 0) in_pUD_Result->SEF |= BFP32_WHEADER_UNKNOWN;
			else if ((in_pUD_1.SEF & BFP32_WHEADER_INFINITY) == BFP32_WHEADER_INFINITY)
			{
				CBFP32::SetValue(in_pUD_Result, in_pUD_1);
				in_pUD_Result->SEF ^= (in_pUD_2.SEF & BFP32_WHEADER_SIGN_OF_BASE);
				if ((in_pUD_2.SEF & BFP32_WHEADER_INFINITY) == BFP32_WHEADER_INFINITY) in_pUD_Result->SEF |= BFP32_WHEADER_UNKNOWN;
			}
			else if ((in_pUD_2.SEF & BFP32_WHEADER_INFINITY) == BFP32_WHEADER_INFINITY)
			{
				if (in_pUD_1.Mantissa[2] != 0)
				{
					in_pUD_Result->SEF = BFP32_WHEADER_CLOSE_TO_0;
					in_pUD_Result->SEF |= (in_pUD_2.SEF & BFP32_WHEADER_SIGN_OF_BASE);
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

				int newExp = (in_pUD_1.SEF & BFP32_WHEADER_EXP_VALUE_MASK) - (in_pUD_2.SEF & BFP32_WHEADER_EXP_VALUE_MASK) - 0x40;
				__int64 iS1 = (((__int64)in_pUD_1.Mantissa[2]) << BFP32_MANTISSA_MOVE_2) + (((__int64)in_pUD_1.Mantissa[1]) << BFP32_MANTISSA_MOVE_1) + in_pUD_1.Mantissa[0];
				__int64 iS2 = (((__int64)in_pUD_2.Mantissa[2]) << BFP32_MANTISSA_MOVE_2) + (((__int64)in_pUD_2.Mantissa[1]) << BFP32_MANTISSA_MOVE_1) + in_pUD_2.Mantissa[0];
				__int64 iRes = 0;
				__int64 rem;

				iRes = iS1 / iS2;
				rem = iS1 - iS2 * iRes; // так быстрее. T(%) > T(/) >= 4 * T(*)  = 7 * T(+-)

				while (rem && !(iRes & 0xFF0000));
				{
					__int64 iTmp = iS1 / iS2;
					rem = iS1 - iS2 * iTmp; // так быстрее. T(%) > T(/) >= 4 * T(*)  = 7 * T(+-)
					iRes <<= BFP32_MANTISSA_MOVE_1;
					iRes += iTmp;
					newExp--;
				} while (rem && !(iRes & 0xFF0000));


				if (newExp <= BFP32_WHEADER_CLOSE_TO_0) //стремится к нулю со стороны знака мантиссы
					in_pUD_Result->SEF = ((~(in_pUD_1.SEF ^ in_pUD_2.SEF)) & BFP32_WHEADER_SIGN_OF_BASE);
				else if (newExp >= BFP32_WHEADER_INFINITY)
					in_pUD_Result->SEF = ((~(in_pUD_1.SEF ^ in_pUD_2.SEF)) & BFP32_WHEADER_SIGN_OF_BASE) | BFP32_WHEADER_INFINITY;
				else
				{
					in_pUD_Result->Mantissa[2] = (unsigned short)(iRes >> BFP32_MANTISSA_MOVE_2);
					in_pUD_Result->Mantissa[1] = (unsigned short)(iRes >> BFP32_MANTISSA_MOVE_1);
					in_pUD_Result->Mantissa[0] = (unsigned short)iRes;
					in_pUD_Result->SEF = (newExp & BFP32_WHEADER_EXP_VALUE_MASK) | ((~(in_pUD_1.SEF ^ in_pUD_2.SEF)) & BFP32_WHEADER_SIGN_OF_BASE);
				}
			}
		}
		else  in_pUD_Result->SEF |= BFP32_WHEADER_UNKNOWN;
	}
	return in_pUD_Result;
}


PBFP32 CBFP32::SetValue(PBFP32 in_pUD_Result, const BFP32 &in_pUD_Src)
{
	if (in_pUD_Result) *in_pUD_Result = in_pUD_Src;

	return in_pUD_Result;
}

unsigned int CBFP32::CmpValue(const BFP32 &in_pUD_1, const BFP32 &in_pUD_2)
{
	unsigned int ret = 0;


	return ret;
}


unsigned int CBFP32::Tounsignedint(const BFP32 &in_pUD)
{
	//!!!!!!!!should be annother lowlevel fast method
	//! Этот вариант только для проверки И понимания
	unsigned __int64 ret = 0;
	CBFP32::udLastOperationFlags = 0;

	if ((in_pUD.SEF & BFP32_WHEADER_SIGN_OF_BASE) == BFP32_WHEADER_SIGN_OF_BASE_POSITIVE)
	{
		int tmpExpValue = (in_pUD.SEF & BFP32_WHEADER_EXP_VALUE_MASK) - 0x40;

		if ((in_pUD.SEF & BFP32_WHEADER_SIGN_OF_EXP) == BFP32_WHEADER_SIGN_OF_EXP_NEGATIVE)		CBFP32::udLastOperationFlags = BFP32_LOFLAG_NAN;
		else if (tmpExpValue > 3)	CBFP32::udLastOperationFlags = BFP32_LOFLAG_OVER;
		else
		{
			if (tmpExpValue >= 2)
			{
				ret = (((unsigned __int64)in_pUD.Mantissa[2]) << BFP32_MANTISSA_MOVE_2) + (((unsigned __int64)in_pUD.Mantissa[1]) << BFP32_MANTISSA_MOVE_1) + in_pUD.Mantissa[0];
				if (tmpExpValue == 2) ret <<= BFP32_MANTISSA_MOVE_1;
			}
			else if (tmpExpValue == 1)
			{
				ret = (((unsigned __int64)in_pUD.Mantissa[2]) << BFP32_MANTISSA_MOVE_1) + in_pUD.Mantissa[1];
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
	else CBFP32::udLastOperationFlags = BFP32_LOFLAG_INVALID_DATA;

	return ret;
}

double CBFP32::ToDouble(const BFP32 &in_pUD)
{
	unsigned __int64 ret;
	if ((in_pUD.SEF & BFP32_WHEADER_EXP_VALUE_MASK) == BFP32_WHEADER_INFINITY) ret = 0x7ff0000000000000;
	else if ((in_pUD.SEF & BFP32_WHEADER_EXP_VALUE_MASK) == BFP32_WHEADER_SOMETHIG) ret = 0; ///реализация чего-либо
	else if ((in_pUD.SEF & BFP32_WHEADER_EXP_VALUE_MASK) == BFP32_WHEADER_CLOSE_TO_0) ret = 0x7fffffffffffffff;
	else if ((in_pUD.SEF & BFP32_WHEADER_EXP_VALUE_MASK) == BFP32_WHEADER_UNKNOWN) ret = 0x7ff0000000000000; //не нашел в double аналог
	else if (!in_pUD.Mantissa[2]) ret = 0; //==0
	else
	{
		ret = (((__int64)in_pUD.Mantissa[2]) << BFP32_MANTISSA_MOVE_1) + in_pUD.Mantissa[1];
		__int64 newExp = ((in_pUD.SEF & BFP32_WHEADER_EXP_VALUE_MASK) - 0x40) * 0x20 + 0x14;

		if (!(ret & 0xFFFFFF0000000000))
		{
			ret = (((__int64)in_pUD.Mantissa[2]) << (BFP32_MANTISSA_MOVE_1 + 24)) + (((__int64)in_pUD.Mantissa[1]) << 24) + (in_pUD.Mantissa[0] >> 8);
			if ((in_pUD.Mantissa[0] & 0x00000080)) ret++;
			newExp -= 24;
		}
		else if (!(ret & 0xFFF0000000000000))
		{
			ret = (((__int64)in_pUD.Mantissa[2]) << (BFP32_MANTISSA_MOVE_1 + 12)) + (((__int64)in_pUD.Mantissa[1]) << 12) + (in_pUD.Mantissa[0] >> 20);
			if ((in_pUD.Mantissa[0] & 0x00080000)) ret++;
			newExp -= 12;
		}
		else if ((in_pUD.Mantissa[0] & 0x80000000))
			if (ret == 0xFFFFFFFFFFFFFFFF)
			{
				ret == 0x8000000000000000;
				newExp++;
			}
			else ret++;

		unsigned __int64 lastBit = 0;

		while (ret && (ret & 0xFFE0000000000000))
		{
			lastBit = ret & 0x1;
			ret >>= 0x1;
			newExp++;
		}

		if (lastBit && ((++ret) & 0xFFE0000000000000))
		{
			ret >>= 0x1;
			newExp++;
		}

		ret &= 0x000FFFFFFFFFFFFF;
		ret |= (((newExp + 1023) << 52) & 0x7FF0000000000000);
	}

	if (!(in_pUD.SEF & BFP32_WHEADER_SIGN_OF_BASE))
		ret |= 0x8000000000000000; //знак числа

	return *((double*)&ret);
}

long double CBFP32::ToLongDouble(const BFP32& in_pUD)
{
	unsigned __int64 ret[2] = { 0, 0};

	if ((in_pUD.SEF & BFP32_WHEADER_EXP_VALUE_MASK) == BFP32_WHEADER_INFINITY) ret[0] = 0x7ff0000000000000;
	else if ((in_pUD.SEF & BFP32_WHEADER_EXP_VALUE_MASK) == BFP32_WHEADER_SOMETHIG) ret[0] = 0; ///реализация чего-либо
	else if ((in_pUD.SEF & BFP32_WHEADER_EXP_VALUE_MASK) == BFP32_WHEADER_CLOSE_TO_0) ret[0] = 0x7fffffffffffffff;
	else if ((in_pUD.SEF & BFP32_WHEADER_EXP_VALUE_MASK) == BFP32_WHEADER_UNKNOWN) ret[0] = 0x7ff0000000000000; //не нашел в double аналог
	else if (!in_pUD.Mantissa[2]) ret[0] = 0; //==0
	else
	{
		ret[0] = ((__int64)in_pUD.Mantissa[2]) << 20;
		int nMove = 20;

		if ((!(ret[0] & 0xFFFFFF0000000000)))
		{
			nMove += 12;
			ret[0] <<= 12;
		}

		if ((!(ret[0] & 0xFFFF000000000000)))
		{
			nMove += 4;
			ret[0] <<= 4;
		}

		if ((!(ret[0] & 0x0010000000000000)))
		{
			nMove++;
			ret[0] <<= 1;
		}

		if (nMove == 32)
		{
			ret[0] += ((__int64)in_pUD.Mantissa[1]);
			ret[1] = (((__int64)in_pUD.Mantissa[0]) << 32);
		}
		else if (nMove > 32)
		{
			ret[0] += (((__int64)in_pUD.Mantissa[1]) << (nMove - 32)) + (((__int64)in_pUD.Mantissa[0]) >> (64 - nMove));
			ret[1] = (((__int64)in_pUD.Mantissa[0]) << (64 - nMove));
		}
		else
		{
			ret[0] += (((__int64)in_pUD.Mantissa[1]) >> (32 - nMove));
			ret[1] = (((__int64)in_pUD.Mantissa[1]) << (32 + nMove)) + (((__int64)in_pUD.Mantissa[0]) << (64 - nMove));
		}

		__int64 newExp = ((in_pUD.SEF & BFP32_WHEADER_EXP_VALUE_MASK) - 0x40) * 0x20 - nMove + 0x34;

		ret[0] &= 0x000FFFFFFFFFFFFF;

		ret[0] |= (((newExp + 0x3FF) << 52) & 0x7FF0000000000000);
	}

	if (!(in_pUD.SEF & BFP32_WHEADER_SIGN_OF_BASE))		ret[0] |= 0x8000000000000000; //знак числа

	return *((long double*)ret);
}

__int64 CBFP32::To__int64(const BFP32 &in_pUD)
{
	//!!!!!!!!should be annother lowlevel fast method
	//! Этот вариант только для проверки И понимания
	__int64 ret = 0;

	if ((in_pUD.SEF & BFP32_WHEADER_SIGN_OF_BASE) == BFP32_WHEADER_SIGN_OF_BASE_NEGATIVE)
		ret = ((((unsigned __int64)in_pUD.Mantissa[2]) << BFP32_MANTISSA_MOVE_2) + (((unsigned __int64)in_pUD.Mantissa[1]) << BFP32_MANTISSA_MOVE_1) + in_pUD.Mantissa[0]);
	else
		ret = -((__int64)((((unsigned __int64)in_pUD.Mantissa[2]) << BFP32_MANTISSA_MOVE_2) + (((unsigned __int64)in_pUD.Mantissa[1]) << BFP32_MANTISSA_MOVE_1) + in_pUD.Mantissa[0]));


	int n = (in_pUD.SEF & BFP32_WHEADER_EXP_VALUE_MASK) - BFP32_WHEADER_SIGN_OF_EXP_POSITIVE - 2;

	if (n >= 0)
		while (n-- > 0) ret <<= BFP32_MANTISSA_MOVE_1;// 0x100;
	else
		while (n++ < 0) ret >>= BFP32_MANTISSA_MOVE_1;// 0x100;


	return ret;

}

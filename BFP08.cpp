
#include "BFP08.h"


DWORD CBFP08::udLastOperationFlags;

CBFP08::CBFP08()
{
}


CBFP08::~CBFP08()
{
}

PBFP08 CBFP08::InitValue(PBFP08 in_pUD_Result)
{
	if (in_pUD_Result)
	{
		in_pUD_Result->Mantissa[0] = 0;
		in_pUD_Result->Mantissa[1] = 0;
		in_pUD_Result->Mantissa[2] = 0;
		in_pUD_Result->SEF = BFP08_WHEADER_SIGN_OF_BASE_POSITIVE | BFP08_WHEADER_SIGN_OF_EXP_POSITIVE;
	}

	return in_pUD_Result;
}


PBFP08 CBFP08::RollRight(PBFP08 in_pBFP08_Result, int nTimes)
{
	if (nTimes == 1)
	{
		if ((in_pBFP08_Result->Mantissa[0] & BFP08_MANTISSA_HALF)) //переполнение и инкремент при сдвиге
		{
			if ((in_pBFP08_Result->Mantissa[1] == BFP08_MANTISSA_MAX))
			{
				in_pBFP08_Result->Mantissa[0] = 0;
				if ((in_pBFP08_Result->Mantissa[2] == BFP08_MANTISSA_MAX))
				{
					in_pBFP08_Result->Mantissa[1] = 0;
					in_pBFP08_Result->Mantissa[2] = 1;
				}
				else
				{
					in_pBFP08_Result->Mantissa[1] = in_pBFP08_Result->Mantissa[2] + 1;
					in_pBFP08_Result->Mantissa[2] = 0;
				}
			}
			else
			{
				in_pBFP08_Result->Mantissa[0] = in_pBFP08_Result->Mantissa[1] + 1;
				in_pBFP08_Result->Mantissa[1] = in_pBFP08_Result->Mantissa[2];
				in_pBFP08_Result->Mantissa[2] = 0;
			}
		}
		else
		{
			in_pBFP08_Result->Mantissa[0] = in_pBFP08_Result->Mantissa[1];
			in_pBFP08_Result->Mantissa[1] = in_pBFP08_Result->Mantissa[2];
			in_pBFP08_Result->Mantissa[2] = 0;
		}
	}
	else if (nTimes == 2)
	{
		if ((in_pBFP08_Result->Mantissa[1] & BFP08_MANTISSA_HALF)) //переполнение и инкремент при сдвиге
		{
			if ((in_pBFP08_Result->Mantissa[2] == BFP08_MANTISSA_MAX))
			{
				in_pBFP08_Result->Mantissa[0] = 0;
				in_pBFP08_Result->Mantissa[1] = 1;
			}
			else
			{
				in_pBFP08_Result->Mantissa[0] = in_pBFP08_Result->Mantissa[2] + 1;
				in_pBFP08_Result->Mantissa[1] = 0;
			}
		}
		else
		{
			in_pBFP08_Result->Mantissa[0] = in_pBFP08_Result->Mantissa[2];
			in_pBFP08_Result->Mantissa[1] = 0;
		}
		in_pBFP08_Result->Mantissa[2] = 0;
	}
	else if (nTimes == 3)
	{
		if ((in_pBFP08_Result->Mantissa[2] & BFP08_MANTISSA_HALF)) //переполнение и инкремент при сдвиге
			in_pBFP08_Result->Mantissa[0] = 1;
		else
			in_pBFP08_Result->Mantissa[0] = 0;

		in_pBFP08_Result->Mantissa[1] = 0;
		in_pBFP08_Result->Mantissa[2] = 0;

	}
	else if (nTimes > 3)
	{
		in_pBFP08_Result->Mantissa[0] = 0;
		in_pBFP08_Result->Mantissa[1] = 0;
		in_pBFP08_Result->Mantissa[2] = 0;
	}

	return in_pBFP08_Result;
}

//#define BFP08_WHEADER_INFINITY				0x3F	//признак бесконечности (3F - бесконечность) с учетом знака BFP08_WHEADER_SIGN_OF_BASE
//#define BFP08_WHEADER_INFINITY_MASK			0x7F	//признак бесконечности (3F - бесконечность) с учетом знака BFP08_WHEADER_SIGN_OF_BASE
//#define BFP08_WHEADER_UNKNOWN					0x7F	//невычисляемое значение, например бесконечность - бесконечность


PBFP08 CBFP08::ADD(PBFP08 in_pUD_Result, const BFP08 &in_pUD_1, const BFP08 &in_pUD_2)
{
	if (in_pUD_Result)
	{
		if ( (in_pUD_1.SEF & BFP08_WHEADER_UNKNOWN) != BFP08_WHEADER_UNKNOWN && (in_pUD_2.SEF & BFP08_WHEADER_UNKNOWN) != BFP08_WHEADER_UNKNOWN	)
		{
			if (((in_pUD_1.SEF ^ in_pUD_2.SEF) & BFP08_WHEADER_SIGN_OF_BASE)) //разные знаки
			{
				if ((in_pUD_1.SEF & BFP08_WHEADER_SIGN_OF_BASE)== BFP08_WHEADER_SIGN_OF_BASE_POSITIVE) //in_pUD_1 >= 0
				{
					*in_pUD_Result = in_pUD_2;
					in_pUD_Result->SEF &= BFP08_WHEADER_EXP_VALUE_MASK;
					SUB(in_pUD_Result, in_pUD_1, *in_pUD_Result);
				}
				else
				{
					*in_pUD_Result = in_pUD_1;
					in_pUD_Result->SEF &= BFP08_WHEADER_EXP_VALUE_MASK;
					SUB(in_pUD_Result, in_pUD_2, *in_pUD_Result);
				}
			}
			else
			{
				if ((in_pUD_1.SEF & BFP08_WHEADER_INFINITY) == BFP08_WHEADER_INFINITY)			CBFP08::SetValue(in_pUD_Result, in_pUD_1);
				else
					if ((in_pUD_2.SEF & BFP08_WHEADER_INFINITY) == BFP08_WHEADER_INFINITY)		CBFP08::SetValue(in_pUD_Result, in_pUD_2);
					else
					{//ADD  by DWORD  (byte order !!!!)
						__int64 iS1;
						unsigned char SEF;
						int dltExp = in_pUD_2.SEF - in_pUD_1.SEF;


						if (!dltExp)
						{
							iS1 = ((((__int64)in_pUD_1.Mantissa[2]) << BFP08_MANTISSA_MOVE_2) + (((__int64)in_pUD_1.Mantissa[1]) << BFP08_MANTISSA_MOVE_1) + (__int64)in_pUD_1.Mantissa[0]) + ((((__int64)in_pUD_2.Mantissa[2]) << BFP08_MANTISSA_MOVE_2) + (((__int64)in_pUD_2.Mantissa[1]) << BFP08_MANTISSA_MOVE_1) + (__int64)in_pUD_2.Mantissa[0]);
							SEF = in_pUD_1.SEF;
						}
						else
						if (dltExp > 0)
						{
							iS1 = ((((__int64)in_pUD_1.Mantissa[2]) << BFP08_MANTISSA_MOVE_2) + (((__int64)in_pUD_1.Mantissa[1]) << BFP08_MANTISSA_MOVE_1) + (__int64)in_pUD_1.Mantissa[0]) >> ((dltExp << 3) - 1);
							if (iS1 & 0x1) iS1 += 2;
							iS1 = (iS1 >>= 1) + ((((__int64)in_pUD_2.Mantissa[2]) << BFP08_MANTISSA_MOVE_2) + (((__int64)in_pUD_2.Mantissa[1]) << BFP08_MANTISSA_MOVE_1) + (__int64)in_pUD_2.Mantissa[0]);
							SEF = in_pUD_2.SEF;
						}
						else
						{
							iS1 = ((((__int64)in_pUD_2.Mantissa[2]) << BFP08_MANTISSA_MOVE_2) + (((__int64)in_pUD_2.Mantissa[1]) << BFP08_MANTISSA_MOVE_1) + (__int64)in_pUD_2.Mantissa[0]) >> (((- dltExp) << 3) - 1);
							if (iS1 & 0x1) iS1 += 2;
							iS1 = (iS1 >>= 1) + (((__int64)in_pUD_1.Mantissa[2]) << BFP08_MANTISSA_MOVE_2) + (((__int64)in_pUD_1.Mantissa[1]) << BFP08_MANTISSA_MOVE_1) + (__int64)in_pUD_1.Mantissa[0];
							SEF = in_pUD_1.SEF;
						}

						//iS1 += iS2;

						if ((iS1 & 0xFF000000)) //переполнение
						{
							iS1 += 0x7F;
							iS1 >>= BFP08_MANTISSA_MOVE_1;

							if ((SEF & BFP08_WHEADER_SIGN_OF_EXP))
							{//Exp >= 0
								if ((SEF & BFP08_WHEADER_EXP_VALUE_MASK) < BFP08_WHEADER_INFINITY) SEF++;
							}
							else
							{//Exp <0
								SEF++;
							}
						}
   
						in_pUD_Result->Mantissa[2] = (unsigned char)(iS1 >> BFP08_MANTISSA_MOVE_2);
						in_pUD_Result->Mantissa[1] = (unsigned char)(iS1 >> BFP08_MANTISSA_MOVE_1);
						in_pUD_Result->Mantissa[0] = (unsigned char)iS1;
						in_pUD_Result->SEF = SEF;

					}
					//{	//ADD  byte by byte
					//	BFP08 tmp_1 = *in_pUD_1;
					//	BFP08 tmp_2 = *in_pUD_2;

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

					//		if ((in_pUD_Result->SEF & BFP08_WHEADER_SIGN_OF_EXP))
					//		{//Exp >= 0
					//			if ((in_pUD_Result->SEF & BFP08_WHEADER_EXP_VALUE_MASK) < BFP08_WHEADER_INFINITY) in_pUD_Result->SEF++;
					//		}
					//		else
					//		{//Exp <0
					//			in_pUD_Result->SEF++;
					//		}
					//	}
					//}
			}
		}
		else  in_pUD_Result->SEF |= BFP08_WHEADER_UNKNOWN;
	}

	return in_pUD_Result;
}

PBFP08 CBFP08::SUB(PBFP08 in_pUD_Result, const BFP08 &in_pUD_1, const BFP08 &in_pUD_2)
{
	if (in_pUD_Result)
	{
		if ((in_pUD_1.SEF & BFP08_WHEADER_UNKNOWN) != BFP08_WHEADER_UNKNOWN	&& (in_pUD_2.SEF & BFP08_WHEADER_UNKNOWN) != BFP08_WHEADER_UNKNOWN)
		{
			if (((in_pUD_1.SEF ^ in_pUD_2.SEF) & BFP08_WHEADER_SIGN_OF_BASE)) //разные знаки
			{
				if ((in_pUD_1.SEF & BFP08_WHEADER_SIGN_OF_BASE) == BFP08_WHEADER_SIGN_OF_BASE_POSITIVE) //in_pUD_1 >= 0
				{
					*in_pUD_Result = in_pUD_2;
					in_pUD_Result->SEF &= BFP08_WHEADER_EXP_VALUE_MASK;
					ADD(in_pUD_Result, in_pUD_1, *in_pUD_Result);
				}
				else
				{
					*in_pUD_Result = in_pUD_1;
					in_pUD_Result->SEF &= BFP08_WHEADER_EXP_VALUE_MASK;
					ADD(in_pUD_Result, in_pUD_2, *in_pUD_Result);
				}
			}
			else
			{
				if ((in_pUD_1.SEF & BFP08_WHEADER_INFINITY) == BFP08_WHEADER_INFINITY)			*in_pUD_Result = in_pUD_1;
				else
					if ((in_pUD_2.SEF & BFP08_WHEADER_INFINITY) == BFP08_WHEADER_INFINITY)		*in_pUD_Result = in_pUD_2;
					else
					{	//SUB byte by byte
						BFP08 tmp_1;
						BFP08 tmp_2;

						if (*((DWORD*)&in_pUD_1) >= *((DWORD*)&in_pUD_2))
						{
							in_pUD_Result->SEF = in_pUD_1.SEF;
							tmp_1 = in_pUD_1;
							tmp_2 = in_pUD_2;
							RollRight(&tmp_2, in_pUD_1.SEF - in_pUD_2.SEF);
						}
						else
						{
							in_pUD_Result->SEF = in_pUD_2.SEF;
							in_pUD_Result->SEF ^= BFP08_WHEADER_SIGN_OF_BASE;
							tmp_1 = in_pUD_2;
							tmp_2 = in_pUD_1;
							RollRight(&tmp_1, in_pUD_2.SEF - in_pUD_1.SEF);
						}


						__int64 tmp;
						tmp = 0x100 + tmp_1.Mantissa[0] - tmp_2.Mantissa[0];
						in_pUD_Result->Mantissa[0] = (unsigned char)tmp;

						if ((tmp & 0x100))	tmp = 0x100 + tmp_1.Mantissa[1] - tmp_2.Mantissa[1];
						else				tmp = 0x100 + tmp_1.Mantissa[1] - tmp_2.Mantissa[1] - 1;

						in_pUD_Result->Mantissa[1] = (unsigned char)tmp;

						if ((tmp & 0x100))	tmp = 0x100 + tmp_1.Mantissa[2] - tmp_2.Mantissa[2];
						else				tmp = 0x100 + tmp_1.Mantissa[2] - tmp_2.Mantissa[2] - 1;

						in_pUD_Result->Mantissa[2] = (unsigned char)tmp;

						if (in_pUD_Result->Mantissa[2] == 0)
						{
							if (in_pUD_Result->Mantissa[1] == 0)
							{
								if (in_pUD_Result->Mantissa[0] == 0)
								{// результат == 0
									in_pUD_Result->SEF = BFP08_WHEADER_SIGN_OF_BASE_POSITIVE | BFP08_WHEADER_SIGN_OF_EXP_POSITIVE;
								}
								else
								{
									in_pUD_Result->Mantissa[2] = in_pUD_Result->Mantissa[0];
									in_pUD_Result->Mantissa[0] = 0;

									if ((in_pUD_Result->SEF & BFP08_WHEADER_SIGN_OF_EXP))
									{//Exp >= 0
										in_pUD_Result->SEF-= 2;
									}
									else
									{//Exp <0
										if ((in_pUD_Result->SEF & BFP08_WHEADER_EXP_VALUE_MASK) > 3) in_pUD_Result->SEF -= 2;
										else
										{
											in_pUD_Result->SEF &= BFP08_WHEADER_SIGN_OF_BASE;
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
								if ((in_pUD_Result->SEF & BFP08_WHEADER_SIGN_OF_EXP))
								{//Exp >= 0
									in_pUD_Result->SEF--;
								}
								else
								{//Exp <0
									if ((in_pUD_Result->SEF & BFP08_WHEADER_EXP_VALUE_MASK) > 2) in_pUD_Result->SEF--;
									else
										InitValue(in_pUD_Result);
								}
							}
						}
					}
			}
		}
		else  in_pUD_Result->SEF |= BFP08_WHEADER_UNKNOWN;
	}

	return in_pUD_Result;
}


PBFP08 CBFP08::MUL(PBFP08 in_pUD_Result, const BFP08 &in_pUD_1, const BFP08 &in_pUD_2)
{
	if (in_pUD_Result)
	{
		//CBFP08::InitValue(in_pUD_Result);

		if ((in_pUD_1.SEF & BFP08_WHEADER_UNKNOWN) != BFP08_WHEADER_UNKNOWN	&& (in_pUD_2.SEF & BFP08_WHEADER_UNKNOWN) != BFP08_WHEADER_UNKNOWN	)
		{
			if ((in_pUD_1.SEF & BFP08_WHEADER_INFINITY) == BFP08_WHEADER_INFINITY)
			{
				CBFP08::SetValue(in_pUD_Result, in_pUD_1);
				in_pUD_Result->SEF ^= (in_pUD_2.SEF & BFP08_WHEADER_SIGN_OF_BASE);
				if (in_pUD_2.Mantissa[2] == 0) in_pUD_Result->SEF |= BFP08_WHEADER_UNKNOWN;
			}
			else if ((in_pUD_2.SEF & BFP08_WHEADER_INFINITY) == BFP08_WHEADER_INFINITY)
			{
				CBFP08::SetValue(in_pUD_Result, in_pUD_2);
				in_pUD_Result->SEF ^= (in_pUD_1.SEF & BFP08_WHEADER_SIGN_OF_BASE);
				if (in_pUD_1.Mantissa[2] == 0) in_pUD_Result->SEF |= BFP08_WHEADER_UNKNOWN;
			}
			else
				//if (in_pUD_1.Mantissa[2] == 0 || in_pUD_2.Mantissa[2] == 0) InitValue(in_pUD_Result);
				//else
			{	//MUL
				__int64 iS = (((__int64)in_pUD_1.Mantissa[2]) << BFP08_MANTISSA_MOVE_2) + (((__int64)in_pUD_1.Mantissa[1]) << BFP08_MANTISSA_MOVE_1) + (__int64)in_pUD_1.Mantissa[0];
				__int64 iRes = ((iS * ((__int64)in_pUD_2.Mantissa[2]) << BFP08_MANTISSA_MOVE_2)) + ((iS * ((__int64)in_pUD_2.Mantissa[1]) << BFP08_MANTISSA_MOVE_1)) + iS * (__int64)in_pUD_2.Mantissa[0];
				//__int64 iRes = ((__int64)(*((DWORD*)&in_pUD_1) & 0x00FFFFFF)) * ((__int64)(*((DWORD*)&in_pUD_2) & 0x00FFFFFF)); //slowly

				int newExp = (in_pUD_1.SEF & BFP08_WHEADER_EXP_VALUE_MASK) + (in_pUD_2.SEF & BFP08_WHEADER_EXP_VALUE_MASK) - 0x40;

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
					in_pUD_Result->SEF = ((~(in_pUD_1.SEF ^ in_pUD_2.SEF)) & BFP08_WHEADER_SIGN_OF_BASE);
				else if (newExp >= BFP08_WHEADER_INFINITY)
					in_pUD_Result->SEF = ((~(in_pUD_1.SEF ^ in_pUD_2.SEF)) & BFP08_WHEADER_SIGN_OF_BASE) | BFP08_WHEADER_INFINITY;
				else
				{
					in_pUD_Result->Mantissa[2] = (unsigned char)(iRes >> BFP08_MANTISSA_MOVE_2);
					in_pUD_Result->Mantissa[1] = (unsigned char)(iRes >> BFP08_MANTISSA_MOVE_1);
					in_pUD_Result->Mantissa[0] = (unsigned char)iRes;
					in_pUD_Result->SEF = (newExp & BFP08_WHEADER_EXP_VALUE_MASK) | ((~(in_pUD_1.SEF ^ in_pUD_2.SEF)) & BFP08_WHEADER_SIGN_OF_BASE);
				}
			}
		}
		else  in_pUD_Result->SEF |= BFP08_WHEADER_UNKNOWN;
	}

	return in_pUD_Result;
}
PBFP08 CBFP08::DIV(PBFP08 in_pUD_Result, const BFP08 &in_pUD_1, const BFP08 &in_pUD_2)
{
	if (in_pUD_Result)
	{
		CBFP08::InitValue(in_pUD_Result);

		if ((in_pUD_1.SEF & BFP08_WHEADER_UNKNOWN) != BFP08_WHEADER_UNKNOWN && (in_pUD_2.SEF & BFP08_WHEADER_UNKNOWN) != BFP08_WHEADER_UNKNOWN)
		{
			if (in_pUD_2.Mantissa[2] == 0) in_pUD_Result->SEF |= BFP08_WHEADER_UNKNOWN;
			else if ((in_pUD_1.SEF & BFP08_WHEADER_INFINITY) == BFP08_WHEADER_INFINITY)
			{
				CBFP08::SetValue(in_pUD_Result, in_pUD_1);
				in_pUD_Result->SEF ^= (in_pUD_2.SEF & BFP08_WHEADER_SIGN_OF_BASE);
				if ((in_pUD_2.SEF & BFP08_WHEADER_INFINITY) == BFP08_WHEADER_INFINITY) in_pUD_Result->SEF |= BFP08_WHEADER_UNKNOWN;
			}
			else if ((in_pUD_2.SEF & BFP08_WHEADER_INFINITY) == BFP08_WHEADER_INFINITY)
			{
				if (in_pUD_1.Mantissa[2] != 0)
				{
					in_pUD_Result->SEF = BFP08_WHEADER_CLOSE_TO_0;
					in_pUD_Result->SEF |= (in_pUD_2.SEF & BFP08_WHEADER_SIGN_OF_BASE);
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

				int newExp = (in_pUD_1.SEF & BFP08_WHEADER_EXP_VALUE_MASK) - (in_pUD_2.SEF & BFP08_WHEADER_EXP_VALUE_MASK) - 0x40;
				__int64 iS1 = (((__int64)in_pUD_1.Mantissa[2]) << BFP08_MANTISSA_MOVE_2) + (((__int64)in_pUD_1.Mantissa[1]) << BFP08_MANTISSA_MOVE_1) + in_pUD_1.Mantissa[0];
				__int64 iS2 = (((__int64)in_pUD_2.Mantissa[2]) << BFP08_MANTISSA_MOVE_2) + (((__int64)in_pUD_2.Mantissa[1]) << BFP08_MANTISSA_MOVE_1) + in_pUD_2.Mantissa[0];
				__int64 iRes = 0;
				__int64 rem;

				iRes = iS1 / iS2;
				rem = iS1 - iS2 * iRes; // так быстрее. T(%) > T(/) >= 4 * T(*)  = 7 * T(+-)

				while (rem && !(iRes & 0xFF0000));
				{
					__int64 iTmp = iS1 / iS2;
					rem = iS1 - iS2 * iTmp; // так быстрее. T(%) > T(/) >= 4 * T(*)  = 7 * T(+-)
					iRes <<= BFP08_MANTISSA_MOVE_1;
					iRes += iTmp;
					newExp--;
				} while (rem && !(iRes & 0xFF0000));


				if (newExp <= BFP08_WHEADER_CLOSE_TO_0) //стремится к нулю со стороны знака мантиссы
					in_pUD_Result->SEF = ((~(in_pUD_1.SEF ^ in_pUD_2.SEF)) & BFP08_WHEADER_SIGN_OF_BASE);
				else if (newExp >= BFP08_WHEADER_INFINITY)
					in_pUD_Result->SEF = ((~(in_pUD_1.SEF ^ in_pUD_2.SEF)) & BFP08_WHEADER_SIGN_OF_BASE) | BFP08_WHEADER_INFINITY;
				else
				{
					in_pUD_Result->Mantissa[2] = (unsigned char)(iRes >> BFP08_MANTISSA_MOVE_2);
					in_pUD_Result->Mantissa[1] = (unsigned char)(iRes >> BFP08_MANTISSA_MOVE_1);
					in_pUD_Result->Mantissa[0] = (unsigned char)iRes;
					in_pUD_Result->SEF = (newExp & BFP08_WHEADER_EXP_VALUE_MASK) | ((~(in_pUD_1.SEF ^ in_pUD_2.SEF)) & BFP08_WHEADER_SIGN_OF_BASE);
				}
			}
		}
		else  in_pUD_Result->SEF |= BFP08_WHEADER_UNKNOWN;
	}
	return in_pUD_Result;
}


PBFP08 CBFP08::SetValue(PBFP08 in_pUD_Result, const BFP08 &in_pUD_Src)
{
	if (in_pUD_Result) *in_pUD_Result = in_pUD_Src;

	return in_pUD_Result;
}

DWORD CBFP08::CmpValue(const BFP08 &in_pUD_1, const BFP08 &in_pUD_2)
{
	DWORD ret = 0;


	return ret;
}


DWORD CBFP08::ToDWORD(const BFP08 &in_pUD)
{
	//!!!!!!!!should be annother lowlevel fast method
	//! Этот вариант только для проверки И понимания
	DWORD ret = 0;
	CBFP08::udLastOperationFlags = 0;

	if ((in_pUD.SEF & BFP08_WHEADER_SIGN_OF_BASE) == BFP08_WHEADER_SIGN_OF_BASE_POSITIVE)
	{
		int tmpExpValue = (in_pUD.SEF & BFP08_WHEADER_EXP_VALUE_MASK) - 0x40;

		if ((in_pUD.SEF & BFP08_WHEADER_SIGN_OF_EXP) == BFP08_WHEADER_SIGN_OF_EXP_NEGATIVE)		CBFP08::udLastOperationFlags = BFP08_LOFLAG_NAN;
		else if (tmpExpValue > 3)	CBFP08::udLastOperationFlags = BFP08_LOFLAG_OVER;
		else
		{
			if (tmpExpValue >= 2)
			{
				ret = (((__int64)in_pUD.Mantissa[2]) << BFP08_MANTISSA_MOVE_2) + (((__int64)in_pUD.Mantissa[1]) << BFP08_MANTISSA_MOVE_1) + in_pUD.Mantissa[0];
				if (tmpExpValue == 2) ret <<= BFP08_MANTISSA_MOVE_1;
			}
			else if (tmpExpValue == 1)
			{
				ret = (((__int64)in_pUD.Mantissa[2]) << BFP08_MANTISSA_MOVE_1) + in_pUD.Mantissa[1];
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
	else CBFP08::udLastOperationFlags = BFP08_LOFLAG_INVALID_DATA;

	return ret;
}

double CBFP08::ToDouble(const BFP08 &in_pUD)
{
	unsigned __int64 ret;
	if ((in_pUD.SEF & BFP08_WHEADER_EXP_VALUE_MASK) == BFP08_WHEADER_INFINITY) ret = 0x7ff0000000000000;
	else if ((in_pUD.SEF & BFP08_WHEADER_EXP_VALUE_MASK) == BFP08_WHEADER_SOMETHIG) ret = 0; ///реализация чего-либо
	else if ((in_pUD.SEF & BFP08_WHEADER_EXP_VALUE_MASK) == BFP08_WHEADER_CLOSE_TO_0) ret = 0x7fffffffffffffff;
	else if ((in_pUD.SEF & BFP08_WHEADER_EXP_VALUE_MASK) == BFP08_WHEADER_UNKNOWN) ret = 0x7ff0000000000000; //не нашел в double аналог
	else
	{
		ret = (((__int64)in_pUD.Mantissa[2]) << BFP08_MANTISSA_MOVE_2) + (((__int64)in_pUD.Mantissa[1]) << BFP08_MANTISSA_MOVE_1) + in_pUD.Mantissa[0];
		__int64 newExp = ((in_pUD.SEF & BFP08_WHEADER_EXP_VALUE_MASK) - 0x40) * 0x8 + 0x24;

		while (ret && !(ret & 0x0010000000000000))
		{
			ret <<= 0x1;
			newExp--;
		}

		ret &= 0x000FFFFFFFFFFFFF;
		ret |= (((newExp + 1023) << 52) & 0x7FF0000000000000);
	}

	if (!(in_pUD.SEF & BFP08_WHEADER_SIGN_OF_BASE))
		ret |= 0x8000000000000000; //знак числа

	return *((double*)&ret);
}

long double CBFP08::ToLongDouble(const BFP08 &in_pUD)
{
	return ToDouble(  in_pUD);
}

__int64 CBFP08::To__int64(const BFP08 &in_pUD)
{
	//!!!!!!!!should be annother lowlevel fast method
	//! Этот вариант только для проверки И понимания
	__int64 ret = 0;

	if ((in_pUD.SEF & BFP08_WHEADER_SIGN_OF_BASE) == BFP08_WHEADER_SIGN_OF_BASE_NEGATIVE)
		ret = ((((__int64)in_pUD.Mantissa[2]) << BFP08_MANTISSA_MOVE_2) + (((__int64)in_pUD.Mantissa[1]) << BFP08_MANTISSA_MOVE_1) + in_pUD.Mantissa[0]);
	else
		ret = -((__int64)((((__int64)in_pUD.Mantissa[2]) << BFP08_MANTISSA_MOVE_2) + (((__int64)in_pUD.Mantissa[1]) << BFP08_MANTISSA_MOVE_1) + in_pUD.Mantissa[0]));


	int n = (in_pUD.SEF & BFP08_WHEADER_EXP_VALUE_MASK) - BFP08_WHEADER_SIGN_OF_EXP_POSITIVE - 2;

	if (n >= 0)
		while (n < 2 && n-- > 0 && ret) ret <<= BFP08_MANTISSA_MOVE_1;// 0x100;
	else
		while (n++ < 0 && ret) ret >>= BFP08_MANTISSA_MOVE_1;// 0x100;


	return ret;

}

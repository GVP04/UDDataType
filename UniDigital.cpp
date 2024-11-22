
#include "UniDigital.h"


DWORD CUniDigital::udLastOperationFlags;
const  float CUniDigital::fltDischarge = 4294967296.0;
const  double CUniDigital::dblDischarge = 4294967296.0;
const  long double CUniDigital::ldblDischarge = 4294967296.0;
const  long double CUniDigital::ldblDischarge2 = 4294967296.0 * 4294967296.0;
const  long double CUniDigital::ldblDischarge3 = 4294967296.0 * 4294967296.0 * 4294967296.0;
const __int64 CUniDigital::int64Discharge = 4294967296;


//длинна мантиссы в байтах в зависимости от значения Header.Flags_Precission
const DWORD UD_PrecissionLenght_BYTE[]  = { UD_PREC_COUNT_LEV0 * 4, UD_PREC_COUNT_LEV1 * 4, UD_PREC_COUNT_LEV2 * 4, UD_PREC_COUNT_LEV3 * 4 };
const DWORD UD_PrecissionLenght_WORD[]  = { UD_PREC_COUNT_LEV0 * 2, UD_PREC_COUNT_LEV1 * 2, UD_PREC_COUNT_LEV2 * 2, UD_PREC_COUNT_LEV3 * 2 };
const DWORD UD_PrecissionLenght_DWORD[] = { UD_PREC_COUNT_LEV0, UD_PREC_COUNT_LEV1, UD_PREC_COUNT_LEV2, UD_PREC_COUNT_LEV3 };


CUniDigital::CUniDigital()
{
	DWORD maxMantissa = UD_PrecissionLenght_BYTE[UD_WHEADER_PRECISSION] * 2 + 16;
	m_First = (PUNIDIGITAL)new BYTE [maxMantissa ];
	memset(m_First,0, maxMantissa);
	m_Second = (PUNIDIGITAL)new BYTE[maxMantissa ];
	memset(m_Second, 0, maxMantissa );
	m_Result = (PUNIDIGITAL)new BYTE[maxMantissa * 2];
	memset(m_Result, 0, maxMantissa * 2);
	m_Slave = (PUNIDIGITAL)new BYTE[maxMantissa * 2];
	memset(m_Slave, 0, maxMantissa * 2);
	m_Temp = (PUNIDIGITAL)new BYTE[maxMantissa * 2];
	memset(m_Temp, 0, maxMantissa * 2);
}


CUniDigital::~CUniDigital()
{
	if (m_First) delete [] m_First;
	if (m_Second) delete [] m_Second;
	if (m_Result) delete [] m_Result;
	if (m_Slave) delete [] m_Slave;
	if (m_Temp) delete [] m_Temp;

	m_First = NULL;
	m_Second = NULL;
	m_Result = NULL;
	m_Slave = NULL;
	m_Temp = NULL;
}


DWORD CUniDigital::GetLenght(PUNIDIGITAL in_pUD)
{
	DWORD ret = 0;
	if (in_pUD)
	{
		ret = 2 * sizeof(UNIDIGITAL_HEADER);
		if ((in_pUD->ReHeader.Flags_Precission & UD_WHEADER_DEFINED))
			ret += UD_PrecissionLenght_BYTE[ in_pUD->ReHeader.Flags_Precission & UD_WHEADER_PRECISSION];
		else 
			ret += UD_PrecissionLenght_BYTE[ 0 ];

		if ((in_pUD->ImHeader.Flags_Precission & UD_WHEADER_DEFINED))
			ret += UD_PrecissionLenght_BYTE[in_pUD->ImHeader.Flags_Precission & UD_WHEADER_PRECISSION];
		else
			ret += UD_PrecissionLenght_BYTE[0];
	}
	else CUniDigital::udLastOperationFlags = UD_LOFLAG_INVALID_DATA;

	return ret;
}

DWORD CUniDigital::GetLenght(DWORD in_RePrecissionLevel)
{
	DWORD ret = 2 * sizeof(UNIDIGITAL_HEADER);
	if (in_RePrecissionLevel <= UD_WHEADER_MAX_PRECISSION_LEVEL) ret += UD_PrecissionLenght_BYTE[in_RePrecissionLevel & UD_WHEADER_PRECISSION];
	else ret += UD_PrecissionLenght_BYTE[0];

	return	ret;
}

DWORD CUniDigital::GetLenght(DWORD in_RePrecissionLevel, DWORD in_ImPrecissionLevel)
{
	DWORD ret = 2 * sizeof(UNIDIGITAL_HEADER);
	if (in_RePrecissionLevel <= UD_WHEADER_MAX_PRECISSION_LEVEL) ret += UD_PrecissionLenght_BYTE[in_RePrecissionLevel & UD_WHEADER_PRECISSION];
	else ret += UD_PrecissionLenght_BYTE[0];
	if (in_ImPrecissionLevel <= UD_WHEADER_MAX_PRECISSION_LEVEL) ret += UD_PrecissionLenght_BYTE[in_ImPrecissionLevel & UD_WHEADER_PRECISSION];

	return ret;
}


DWORD CUniDigital::ADD_LL(PUNIDIGITAL_HEADER pUD_ResultH, DWORD *pdwResult, const PUNIDIGITAL_HEADER pUD_H, const DWORD* pdwUD)
{
	if (pUD_ResultH && pdwResult && pUD_H && pdwUD)
	{
#ifdef _DEBUG
		PUNIDIGITALR2 m_resVal = (PUNIDIGITALR2)m_Result;
		PUNIDIGITALR2 ResView = (PUNIDIGITALR2)pUD_ResultH;
		PUNIDIGITALR2 s00View = (PUNIDIGITALR2)pUD_H;
#endif
		register int iDeltaExp = pUD_ResultH->Exponent - pUD_H->Exponent;
		register int iPosSrc = UD_PrecissionLenght_DWORD[(pUD_H->Flags_Precission & UD_WHEADER_PRECISSION)] - 1;
		register int iPosDst = UD_PrecissionLenght_DWORD[(pUD_ResultH->Flags_Precission & UD_WHEADER_PRECISSION)] - 1;

		if (iDeltaExp < 0)
		{
			register __int64 overload = 0;
			int iPosDstTmp = iPosDst;
			int iPosSrcTmp = iPosDst + iDeltaExp;
			if (iPosSrcTmp >= 0)
				overload = pdwResult[iPosSrcTmp + 1] & UD_ROUND_UP_BIT;

			while (iPosSrcTmp >= 0)
			{
				register __int64 tmpValue = pdwResult[iPosSrcTmp--];
				 if (overload) tmpValue++;

				overload = tmpValue & int64Discharge;
				pdwResult[iPosDstTmp--] = static_cast<DWORD>(tmpValue);
			}
			if (overload) pdwResult[iPosDstTmp--] = 1;

			while (iPosDstTmp >= 0)	pdwResult[iPosDstTmp--] = 0;

			pUD_ResultH->Exponent = pUD_H->Exponent;
			iDeltaExp = 0;
		}

		if (iDeltaExp <= iPosDst) //за пределами точности не работаем
		{
			if (iDeltaExp + iPosSrc > iPosDst) iPosSrc -= iDeltaExp + iPosSrc - iPosDst;
			else
			if (iDeltaExp + iPosSrc < iPosDst) iPosDst = iDeltaExp + iPosSrc;

			register __int64 overload = 0;

			while (iPosDst >= 0 && iPosSrc >= 0)
			{
				register __int64 tmpValue = (__int64)pdwResult[iPosDst] + (__int64)pdwUD[iPosSrc--];
				if (overload) tmpValue++;

				overload = tmpValue & int64Discharge;
				pdwResult[iPosDst--] = static_cast<DWORD>(tmpValue);
			}

			while (overload && iPosDst >= 0)
			{
				register __int64 tmpValue = (__int64)pdwResult[iPosDst];
				if (overload) tmpValue++;

				overload = tmpValue & int64Discharge;
				pdwResult[iPosDst--] = static_cast<DWORD>(tmpValue);
			}

			if (overload)
			{
				iPosDst = UD_PrecissionLenght_DWORD[(pUD_ResultH->Flags_Precission & UD_WHEADER_PRECISSION)] - 1;
				while (iPosDst-- > 0) 
					pdwResult[iPosDst + 1] = pdwResult[iPosDst];

				pdwResult[0] = 1;
				if (pUD_ResultH->Exponent < 127) pUD_ResultH->Exponent++;
				else pUD_ResultH->Flags_Precission |= UD_WHEADER_LIMIT_HI;
			}
		}

	}
	else CUniDigital::udLastOperationFlags = UD_LOFLAG_INVALID_DATA;

	return 0;
}


DWORD CUniDigital::SUB_LL(PUNIDIGITAL_HEADER pUD_ResultH, DWORD* pdwResult, const PUNIDIGITAL_HEADER pUD_H, const DWORD* pdwUD)
{
	if (pUD_ResultH && pdwResult && pUD_H && pdwUD)
	{
#ifdef _DEBUG
		PUNIDIGITALR2 m_resVal = (PUNIDIGITALR2)m_Result;
		PUNIDIGITALR2 ResView = (PUNIDIGITALR2)pUD_ResultH;
		PUNIDIGITALR2 s00View = (PUNIDIGITALR2)pUD_H;
#endif
		register int iDeltaExp = pUD_ResultH->Exponent - pUD_H->Exponent;
		register int iPosSrc = UD_PrecissionLenght_DWORD[(pUD_H->Flags_Precission & UD_WHEADER_PRECISSION)] - 1;
		register int iPosDst = UD_PrecissionLenght_DWORD[(pUD_ResultH->Flags_Precission & UD_WHEADER_PRECISSION)] - 1;

		if (iDeltaExp < 0)
		{
			register __int64 overload = 0;
			int iPosDstTmp = iPosDst;
			int iPosSrcTmp = iPosDst + iDeltaExp;
			if (iPosSrcTmp >= 0)
				overload = pdwResult[iPosSrcTmp + 1] & UD_ROUND_UP_BIT;

			while (iPosSrcTmp >= 0)
			{
				register __int64 tmpValue = pdwResult[iPosSrcTmp--];
				if (overload) tmpValue++;

				overload = tmpValue & int64Discharge;
				pdwResult[iPosDstTmp--] = static_cast<DWORD>(tmpValue);
			}
			if (overload) pdwResult[iPosDstTmp--] = 1;

			while (iPosDstTmp >= 0)	pdwResult[iPosDstTmp--] = 0;

			pUD_ResultH->Exponent = pUD_H->Exponent;
			iDeltaExp = 0;
		}

		if (iDeltaExp <= iPosDst) //за пределами точности не работаем
		{
			if (iDeltaExp + iPosSrc > iPosDst) iPosSrc -= iDeltaExp + iPosSrc - iPosDst;
			else
				if (iDeltaExp + iPosSrc < iPosDst) iPosDst = iDeltaExp + iPosSrc;

			register __int64 overload = 0;

			while (iPosDst >= 0 && iPosSrc >= 0)
			{
				register __int64 tmpValue = (__int64)pdwResult[iPosDst] - (__int64)pdwUD[iPosSrc--];
				if (overload) tmpValue--;

				overload = tmpValue & int64Discharge;
				pdwResult[iPosDst--] = static_cast<DWORD>(tmpValue);
			}

			while (overload && iPosDst >= 0)
			{
				register __int64 tmpValue = (__int64)pdwResult[iPosDst];
				if (overload) tmpValue--;

				overload = tmpValue & int64Discharge;
				pdwResult[iPosDst--] = static_cast<DWORD>(tmpValue);
			}

			iPosDst = UD_PrecissionLenght_DWORD[(pUD_ResultH->Flags_Precission & UD_WHEADER_PRECISSION)] - 1;
			int i;

			if (overload)
			{
				pUD_ResultH->Flags_Precission ^= UD_WHEADER_SIGN_OF_BASE;

				for ( i = 0; i < iPosDst; i++)
					pdwResult[i] = 0 - pdwResult[i];
			}

			for (i = 0; i < iPosDst && pdwResult[i] == 0; i++);

			if (i == iPosDst)
			{
				pUD_ResultH->Exponent = 0;
				pdwResult[i] &= (UD_WHEADER_SIGN_OF_BASE ^ 0xFF);
			}
			else
				if (i)
				{
					int j;
					if (pUD_ResultH->Exponent >= -127 + i) pUD_ResultH->Exponent -= i;
					else
					{
						pUD_ResultH->Exponent = -127;
						pUD_ResultH->Flags_Precission |= UD_WHEADER_LIMIT_LOW;
					}


					for(j = 0; i < iPosDst; i++, j++)
						pdwResult[j] = pdwResult[i];

					while( j < iPosDst)
						pdwResult[j++] = 0;
				}
		}

	}
	else CUniDigital::udLastOperationFlags = UD_LOFLAG_INVALID_DATA;

	return 0;
}

DWORD CUniDigital::ADDUNSIGNED(PUNIDIGITAL in_pUD_Result, const PUNIDIGITAL in_pUD_1, const PUNIDIGITAL in_pUD_2, DWORD in_Flags)
{
	//Может быть оптимизировано в тч в части x + x = 2 * x = x << 1, или за пределами точности не вычислять
	
	if (in_pUD_Result && in_pUD_1 && in_pUD_2)
	{

		PUNIDIGITAL_HEADER h1 = NULL, h2 = NULL, hR = NULL;
		DWORD *D1 = NULL, *D2 = NULL, *DR = NULL;
		if ((in_Flags & UD_OPERATIONS_FLAG_REAL))
		{
			if (*(D1 = in_pUD_1->Value) == 0)	SetRealValue(in_pUD_Result, in_pUD_2);
			else if (*(D2 = in_pUD_2->Value) == 0) SetRealValue(in_pUD_Result, in_pUD_1);
			else
			{
				h1 = &in_pUD_1->ReHeader;
				h2 = &in_pUD_2->ReHeader;
				hR = &in_pUD_Result->ReHeader;
				DR = in_pUD_Result->Value;
			}
		}
		else
		{
			if (*(D1 = in_pUD_1->Value + UD_PrecissionLenght_DWORD[((h1 = &in_pUD_1->ImHeader)->Flags_Precission & UD_WHEADER_PRECISSION)]) == 0) SetComplexValue(in_pUD_Result, in_pUD_2);
			else if (*(D2 = in_pUD_2->Value + UD_PrecissionLenght_DWORD[((h2 = &in_pUD_2->ImHeader)->Flags_Precission & UD_WHEADER_PRECISSION)]) == 0)	SetComplexValue(in_pUD_Result, in_pUD_1);
			else
			{
				hR = &in_pUD_Result->ImHeader;
				DR = in_pUD_Result->Value + UD_PrecissionLenght_DWORD[(hR->Flags_Precission & UD_WHEADER_PRECISSION)];
			}
		}

		if ( 
			hR && h1 && h2 &&
			(hR->Flags_Precission & UD_WHEADER_DEFINED)
			&& (h1->Flags_Precission & UD_WHEADER_DEFINED)
			&& (h2->Flags_Precission & UD_WHEADER_DEFINED)
			)
		{
			if (DR)
			{
				if (D1 == DR)
				{
					if (D2 == DR)
					{
						if ((in_Flags & UD_OPERATIONS_FLAG_REAL)) 
							SetRealValue(m_Temp, in_pUD_Result);
						else
							SetComplexValue(m_Temp, in_pUD_Result);

						h2 = &m_Temp->ReHeader;
						D2 = m_Temp->Value;
					}
					ADD_LL(hR, DR, h2, D2);
				}
				else
				{
					if (D2 != DR)
					{
						if ((in_Flags & UD_OPERATIONS_FLAG_REAL))
							SetRealValue(in_pUD_Result, in_pUD_1);
						else
							SetComplexValue(in_pUD_Result, in_pUD_1);

						ADD_LL(hR, DR, h2, D2);
					}
					else
						ADD_LL(hR, DR, h1, D1);
				}
			}
		}
		else CUniDigital::udLastOperationFlags = UD_LOFLAG_INVALID_DATA;
	}

	return 0;
}

DWORD CUniDigital::SUBUNSIGNED(PUNIDIGITAL in_pUD_Result, PUNIDIGITAL in_pUD_1, PUNIDIGITAL in_pUD_2, DWORD in_Flags)
{
	// Считается, что оба in_pUD_ не отрицательны. Перестановкой и очисткой UD_WHEADER_SIGN_OF_BASE занимается вызывающая функция

	DWORD bNeedChangeSign = 0;

	if (in_pUD_Result && in_pUD_1 && in_pUD_2)
	{
		PUNIDIGITAL_HEADER h1 = NULL, h2 = NULL, hR = NULL;
		DWORD *D1 = NULL, *D2 = NULL, *DR = NULL;
		if ((in_Flags & UD_OPERATIONS_FLAG_REAL))
		{
			if (*(D1 = in_pUD_1->Value) == 0)
			{
				SetRealValue(in_pUD_Result, in_pUD_2);
				in_pUD_Result->ReHeader.Flags_Precission |= UD_WHEADER_SIGN_OF_BASE;
			}
			else if (*(D2 = in_pUD_2->Value) == 0) SetRealValue(in_pUD_Result, in_pUD_1);
			else
			{
				h1 = &in_pUD_1->ReHeader;
				h2 = &in_pUD_2->ReHeader;
				hR = &in_pUD_Result->ReHeader;
				DR = in_pUD_Result->Value;
			}
		}
		else
		{
			if (*(D1 = in_pUD_1->Value + UD_PrecissionLenght_DWORD[((h1 = &in_pUD_1->ImHeader)->Flags_Precission & UD_WHEADER_PRECISSION)]) == 0)
			{
				SetComplexValue(in_pUD_Result, in_pUD_2);
				in_pUD_Result->ImHeader.Flags_Precission |= UD_WHEADER_SIGN_OF_BASE;
			}
			else if (*(D2 = in_pUD_2->Value + UD_PrecissionLenght_DWORD[((h2 = &in_pUD_2->ImHeader)->Flags_Precission & UD_WHEADER_PRECISSION)]) == 0)	SetComplexValue(in_pUD_Result, in_pUD_1);
			else
			{
				hR = &in_pUD_Result->ImHeader;
				DR = in_pUD_Result->Value + UD_PrecissionLenght_DWORD[(hR->Flags_Precission & UD_WHEADER_PRECISSION)];
			}
		}

		if (
			hR && h1 && h2 &&
			(hR->Flags_Precission & UD_WHEADER_DEFINED)
			&& (h1->Flags_Precission & UD_WHEADER_DEFINED)
			&& (h2->Flags_Precission & UD_WHEADER_DEFINED)
			)
		{
			if (DR)
			{
				if (D1 == DR)
				{
					if (D2 == DR)
					{ // res = 0 = x - x
						InitValue(hR, DR);
					}
					else
						SUB_LL(hR, DR, h2, D2);
				}
				else
				{
					if (D2 != DR)
					{
						if ((in_Flags & UD_OPERATIONS_FLAG_REAL))
							SetRealValue(in_pUD_Result, in_pUD_1);
						else
							SetComplexValue(in_pUD_Result, in_pUD_1);

						SUB_LL(hR, DR, h2, D2);
					}
					else
						SUB_LL(hR, DR, h1, D1);
				}
			}
		}
		else CUniDigital::udLastOperationFlags = UD_LOFLAG_INVALID_DATA;
	}

	return 0;
}


void CUniDigital::InitValue(PUNIDIGITAL_HEADER pUD_H, DWORD* pdwValue)
{
	memset(pdwValue, 0, UD_PrecissionLenght_BYTE[pUD_H->Flags_Precission & UD_WHEADER_PRECISSION]);

	pUD_H->Exponent = 0;
	pUD_H->Flags_Precission &= UD_WHEADER_PRECISSION;
	pUD_H->Flags_Precission |= UD_WHEADER_DEFINED;
}

PUNIDIGITAL CUniDigital::InitValue(PUNIDIGITAL in_pUD_Result)
{
	if (in_pUD_Result)
	{
		if ((in_pUD_Result->ImHeader.Flags_Precission & UD_WHEADER_DEFINED))
		{
			memset(in_pUD_Result->Value, 0, UD_PrecissionLenght_BYTE[in_pUD_Result->ReHeader.Flags_Precission & UD_WHEADER_PRECISSION] + UD_PrecissionLenght_BYTE[in_pUD_Result->ImHeader.Flags_Precission & UD_WHEADER_PRECISSION]);
			in_pUD_Result->ImHeader.Exponent = 0;
			in_pUD_Result->ImHeader.Flags_Precission &= UD_WHEADER_PRECISSION;
			in_pUD_Result->ImHeader.Flags_Precission |= UD_WHEADER_DEFINED;
		}
		else
			memset(in_pUD_Result->Value, 0, UD_PrecissionLenght_BYTE[in_pUD_Result->ReHeader.Flags_Precission & UD_WHEADER_PRECISSION]);

		in_pUD_Result->ReHeader.Exponent = 0;
		in_pUD_Result->ReHeader.Flags_Precission &= UD_WHEADER_PRECISSION;
		in_pUD_Result->ReHeader.Flags_Precission |= UD_WHEADER_DEFINED;
	}

	return in_pUD_Result;
}

PUNIDIGITAL CUniDigital::InitRealValue(PUNIDIGITAL in_pUD_Result)
{
	if (in_pUD_Result)
	{
		memset(in_pUD_Result->Value, 0, UD_PrecissionLenght_BYTE[in_pUD_Result->ReHeader.Flags_Precission & UD_WHEADER_PRECISSION]);

		in_pUD_Result->ReHeader.Exponent = 0;
		in_pUD_Result->ReHeader.Flags_Precission &= UD_WHEADER_PRECISSION;
		in_pUD_Result->ReHeader.Flags_Precission |= UD_WHEADER_DEFINED;
	}

	return in_pUD_Result;
}

PUNIDIGITAL CUniDigital::InitComplexValue(PUNIDIGITAL in_pUD_Result)
{
	if (in_pUD_Result)
	{
		memset(in_pUD_Result->Value, 0, UD_PrecissionLenght_BYTE[in_pUD_Result->ReHeader.Flags_Precission & UD_WHEADER_PRECISSION] + UD_PrecissionLenght_BYTE[in_pUD_Result->ImHeader.Flags_Precission & UD_WHEADER_PRECISSION]);
		in_pUD_Result->ImHeader.Exponent = 0;
		in_pUD_Result->ImHeader.Flags_Precission &= UD_WHEADER_PRECISSION;
		in_pUD_Result->ImHeader.Flags_Precission |= UD_WHEADER_DEFINED;
	}

	return in_pUD_Result;
}


DWORD CUniDigital::ADD(PUNIDIGITAL in_pUD_Result, const PUNIDIGITAL in_pUD_1, const PUNIDIGITAL in_pUD_2)
{
	DWORD ret = 1;
	if (in_pUD_Result && in_pUD_1 && in_pUD_2)
	{
		if (!(in_pUD_1->ReHeader.Flags_Precission&UD_WHEADER_DEFINED))
			SetRealValue(in_pUD_Result, in_pUD_1);
		else if (!(in_pUD_2->ReHeader.Flags_Precission&UD_WHEADER_DEFINED))
			SetRealValue(in_pUD_Result, in_pUD_2);
		else if ((in_pUD_1->ReHeader.Flags_Precission&UD_WHEADER_UNKNOWN))
			SetRealValue(in_pUD_Result, in_pUD_1);
		else if ((in_pUD_2->ReHeader.Flags_Precission&UD_WHEADER_UNKNOWN))
			SetRealValue(in_pUD_Result, in_pUD_2);
		else if ((in_pUD_1->ReHeader.Flags_Precission&UD_WHEADER_INFINITY) || (in_pUD_2->ReHeader.Flags_Precission&UD_WHEADER_INFINITY))
		{
			if ((in_pUD_1->ReHeader.Flags_Precission&UD_WHEADER_INFINITY) && (in_pUD_2->ReHeader.Flags_Precission&UD_WHEADER_INFINITY))
			{
				if (((in_pUD_1->ReHeader.Flags_Precission ^ in_pUD_2->ReHeader.Flags_Precission)&UD_WHEADER_SIGN_OF_BASE))
				{ // разные знаки аргументов
					SetRealValue(in_pUD_Result, in_pUD_1);
					in_pUD_Result->ReHeader.Flags_Precission = (in_pUD_Result->ReHeader.Flags_Precission & (UD_WHEADER_UNKNOWN ^ 0xFFFFFFFF));
				}
				else
				{ //одинаковые знаки аргументов. Неопределенное значение
					SetRealValue(in_pUD_Result, in_pUD_1);
					in_pUD_Result->ReHeader.Flags_Precission = (in_pUD_Result->ReHeader.Flags_Precission & (UD_WHEADER_SIGN_OF_BASE ^ 0xFFFFFFFF)) | (in_pUD_1->ReHeader.Flags_Precission & UD_WHEADER_SIGN_OF_BASE);
				}
			}
			else
				if ((in_pUD_1->ReHeader.Flags_Precission&UD_WHEADER_INFINITY))
					SetRealValue(in_pUD_Result, in_pUD_1);
				else
					SetRealValue(in_pUD_Result, in_pUD_2);
		}
		else
		{
			if (((in_pUD_1->ReHeader.Flags_Precission ^ in_pUD_2->ReHeader.Flags_Precission)&UD_WHEADER_SIGN_OF_BASE))
			{ // разные знаки слагаемых
				if ((in_pUD_1->ReHeader.Flags_Precission & UD_WHEADER_SIGN_OF_BASE))
					ret = SUBUNSIGNED( in_pUD_Result,  in_pUD_2,  in_pUD_1, UD_OPERATIONS_FLAG_REAL);
				else
					ret = SUBUNSIGNED( in_pUD_Result,  in_pUD_1,  in_pUD_2, UD_OPERATIONS_FLAG_REAL);
			}
			else
			{ //одинаковые знаки слагаемых
				ret = ADDUNSIGNED(in_pUD_Result, in_pUD_1, in_pUD_2, UD_OPERATIONS_FLAG_REAL);
				in_pUD_Result->ReHeader.Flags_Precission = (in_pUD_Result->ReHeader.Flags_Precission & (UD_WHEADER_SIGN_OF_BASE ^ 0xFFFFFFFF)) | (in_pUD_1->ReHeader.Flags_Precission & UD_WHEADER_SIGN_OF_BASE);
			}
		}
		//для комплексной части
		if (!(in_pUD_1->ImHeader.Flags_Precission&UD_WHEADER_DEFINED) && !(in_pUD_2->ImHeader.Flags_Precission&UD_WHEADER_DEFINED))
			SetComplexValue(in_pUD_Result, in_pUD_1);
		else
		if (!(in_pUD_1->ImHeader.Flags_Precission&UD_WHEADER_DEFINED))
			SetComplexValue(in_pUD_Result, in_pUD_2);
		else if (!(in_pUD_2->ImHeader.Flags_Precission&UD_WHEADER_DEFINED))
			SetComplexValue(in_pUD_Result, in_pUD_1);
		else if ((in_pUD_1->ImHeader.Flags_Precission&UD_WHEADER_UNKNOWN))
			SetComplexValue(in_pUD_Result, in_pUD_1);
		else if ((in_pUD_2->ImHeader.Flags_Precission&UD_WHEADER_UNKNOWN))
			SetComplexValue(in_pUD_Result, in_pUD_2);
		else if ((in_pUD_1->ImHeader.Flags_Precission&UD_WHEADER_INFINITY) || (in_pUD_2->ImHeader.Flags_Precission&UD_WHEADER_INFINITY))
		{
			if ((in_pUD_1->ImHeader.Flags_Precission&UD_WHEADER_INFINITY) && (in_pUD_2->ImHeader.Flags_Precission&UD_WHEADER_INFINITY))
			{
				if (((in_pUD_1->ImHeader.Flags_Precission ^ in_pUD_2->ImHeader.Flags_Precission)&UD_WHEADER_SIGN_OF_BASE))
				{ // разные знаки аргументов
					SetComplexValue(in_pUD_Result, in_pUD_1);
					in_pUD_Result->ImHeader.Flags_Precission = (in_pUD_Result->ImHeader.Flags_Precission & (UD_WHEADER_UNKNOWN ^ 0xFFFFFFFF));
				}
				else
				{ //одинаковые знаки аргументов. Неопределенное значение
					SetComplexValue(in_pUD_Result, in_pUD_1);
					in_pUD_Result->ImHeader.Flags_Precission = (in_pUD_Result->ImHeader.Flags_Precission & (UD_WHEADER_SIGN_OF_BASE ^ 0xFFFFFFFF)) | (in_pUD_1->ImHeader.Flags_Precission & UD_WHEADER_SIGN_OF_BASE);
				}
			}
			else
				if ((in_pUD_1->ImHeader.Flags_Precission&UD_WHEADER_INFINITY))
					SetComplexValue(in_pUD_Result, in_pUD_1);
				else
					SetComplexValue(in_pUD_Result, in_pUD_2);
		}
		else
		{
			if (((in_pUD_1->ImHeader.Flags_Precission ^ in_pUD_2->ImHeader.Flags_Precission)&UD_WHEADER_SIGN_OF_BASE))
			{ // разные знаки слагаемых
				if ((in_pUD_1->ImHeader.Flags_Precission & UD_WHEADER_SIGN_OF_BASE))
					ret = SUBUNSIGNED(in_pUD_Result, in_pUD_2, in_pUD_1, UD_OPERATIONS_FLAG_COMPLEX);
				else
					ret = SUBUNSIGNED(in_pUD_Result, in_pUD_1, in_pUD_2, UD_OPERATIONS_FLAG_COMPLEX);
			}
			else
			{ //одинаковые знаки слагаемых
				ret = ADDUNSIGNED(in_pUD_Result, in_pUD_1, in_pUD_2, UD_OPERATIONS_FLAG_COMPLEX);
				in_pUD_Result->ImHeader.Flags_Precission = (in_pUD_Result->ImHeader.Flags_Precission & (UD_WHEADER_SIGN_OF_BASE ^ 0xFFFFFFFF)) | (in_pUD_1->ImHeader.Flags_Precission & UD_WHEADER_SIGN_OF_BASE);
			}
		}
	}

	return ret;
}


DWORD CUniDigital::SUB(PUNIDIGITAL in_pUD_Result, const PUNIDIGITAL in_pUD_1, const PUNIDIGITAL in_pUD_2)
{
	DWORD ret = 1;
	if (in_pUD_Result && in_pUD_1 && in_pUD_2)
	{
		if (!(in_pUD_1->ReHeader.Flags_Precission&UD_WHEADER_DEFINED))
			SetRealValue(in_pUD_Result, in_pUD_1);
		else if (!(in_pUD_2->ReHeader.Flags_Precission&UD_WHEADER_DEFINED))
			SetRealValue(in_pUD_Result, in_pUD_2);
		else if ((in_pUD_1->ReHeader.Flags_Precission&UD_WHEADER_UNKNOWN))
			SetRealValue(in_pUD_Result, in_pUD_1);
		else if ((in_pUD_2->ReHeader.Flags_Precission&UD_WHEADER_UNKNOWN))
			SetRealValue(in_pUD_Result, in_pUD_2);
		else if ((in_pUD_1->ReHeader.Flags_Precission&UD_WHEADER_INFINITY) || (in_pUD_2->ReHeader.Flags_Precission&UD_WHEADER_INFINITY))
		{
			if ((in_pUD_1->ReHeader.Flags_Precission&UD_WHEADER_INFINITY) && (in_pUD_2->ReHeader.Flags_Precission&UD_WHEADER_INFINITY))
			{
				if (((in_pUD_1->ReHeader.Flags_Precission ^ in_pUD_2->ReHeader.Flags_Precission)&UD_WHEADER_SIGN_OF_BASE))
				{ // разные знаки аргументов
					SetRealValue(in_pUD_Result, in_pUD_1);
					in_pUD_Result->ReHeader.Flags_Precission = (in_pUD_Result->ReHeader.Flags_Precission & (UD_WHEADER_SIGN_OF_BASE ^ 0xFFFFFFFF)) | (in_pUD_1->ReHeader.Flags_Precission & UD_WHEADER_SIGN_OF_BASE);
				}
				else
				{ //одинаковые знаки аргументов. Неопределенное значение
					SetRealValue(in_pUD_Result, in_pUD_1);
					in_pUD_Result->ReHeader.Flags_Precission = (in_pUD_Result->ReHeader.Flags_Precission & (UD_WHEADER_UNKNOWN ^ 0xFFFFFFFF));
				}
			}
			else
				if ((in_pUD_1->ReHeader.Flags_Precission&UD_WHEADER_INFINITY))
					SetRealValue( in_pUD_Result, in_pUD_1);
				else
					SetRealValue( in_pUD_Result, in_pUD_2);
		}
		else
		{
			if (((in_pUD_1->ReHeader.Flags_Precission ^ in_pUD_2->ReHeader.Flags_Precission)&UD_WHEADER_SIGN_OF_BASE))
			{ // разные знаки аргументов
				ret = ADDUNSIGNED(in_pUD_Result, in_pUD_1, in_pUD_2, UD_OPERATIONS_FLAG_REAL);
				in_pUD_Result->ReHeader.Flags_Precission = (in_pUD_Result->ReHeader.Flags_Precission & (UD_WHEADER_SIGN_OF_BASE ^ 0xFFFFFFFF)) | (in_pUD_1->ReHeader.Flags_Precission & UD_WHEADER_SIGN_OF_BASE);
			}
			else
			{ //одинаковые знаки аргументов
				if ((in_pUD_1->ReHeader.Flags_Precission & UD_WHEADER_SIGN_OF_BASE))
					ret = SUBUNSIGNED(in_pUD_Result, in_pUD_2, in_pUD_1, UD_OPERATIONS_FLAG_REAL);
				else
					ret = SUBUNSIGNED(in_pUD_Result, in_pUD_1, in_pUD_2, UD_OPERATIONS_FLAG_REAL);
			}
		}

		//для комплексной части
		if (!(in_pUD_1->ImHeader.Flags_Precission&UD_WHEADER_DEFINED) && !(in_pUD_2->ImHeader.Flags_Precission&UD_WHEADER_DEFINED))
			SetComplexValue(in_pUD_Result, in_pUD_1);
		else if (!(in_pUD_1->ImHeader.Flags_Precission&UD_WHEADER_DEFINED))
			SetComplexValue(in_pUD_Result, in_pUD_2);
		else if (!(in_pUD_2->ImHeader.Flags_Precission&UD_WHEADER_DEFINED))
			SetComplexValue(in_pUD_Result, in_pUD_1);
		else if ((in_pUD_1->ImHeader.Flags_Precission&UD_WHEADER_UNKNOWN))
			SetComplexValue(in_pUD_Result, in_pUD_1);
		else if ((in_pUD_2->ImHeader.Flags_Precission&UD_WHEADER_UNKNOWN))
			SetComplexValue(in_pUD_Result, in_pUD_2);
		else if ((in_pUD_1->ImHeader.Flags_Precission&UD_WHEADER_INFINITY) || (in_pUD_2->ImHeader.Flags_Precission&UD_WHEADER_INFINITY))
		{
			if ((in_pUD_1->ImHeader.Flags_Precission&UD_WHEADER_INFINITY) && (in_pUD_2->ImHeader.Flags_Precission&UD_WHEADER_INFINITY))
			{
				if (((in_pUD_1->ImHeader.Flags_Precission ^ in_pUD_2->ImHeader.Flags_Precission)&UD_WHEADER_SIGN_OF_BASE))
				{ // разные знаки аргументов
					SetComplexValue(in_pUD_Result, in_pUD_1);
					in_pUD_Result->ImHeader.Flags_Precission = (in_pUD_Result->ImHeader.Flags_Precission & (UD_WHEADER_SIGN_OF_BASE ^ 0xFFFFFFFF)) | (in_pUD_1->ImHeader.Flags_Precission & UD_WHEADER_SIGN_OF_BASE);
				}
				else
				{ //одинаковые знаки аргументов. Неопределенное значение
					SetComplexValue(in_pUD_Result, in_pUD_1);
					in_pUD_Result->ImHeader.Flags_Precission = (in_pUD_Result->ImHeader.Flags_Precission & (UD_WHEADER_UNKNOWN ^ 0xFFFFFFFF));
				}
			}
			else
				if ((in_pUD_1->ImHeader.Flags_Precission&UD_WHEADER_INFINITY))
					SetComplexValue(in_pUD_Result, in_pUD_1);
				else
					SetComplexValue(in_pUD_Result, in_pUD_2);
		}
		else
		{
			if (((in_pUD_1->ImHeader.Flags_Precission ^ in_pUD_2->ImHeader.Flags_Precission)&UD_WHEADER_SIGN_OF_BASE))
			{ // разные знаки аргументов
				ret = ADDUNSIGNED(in_pUD_Result, in_pUD_1, in_pUD_2, UD_OPERATIONS_FLAG_COMPLEX);
				in_pUD_Result->ImHeader.Flags_Precission = (in_pUD_Result->ImHeader.Flags_Precission & (UD_WHEADER_SIGN_OF_BASE ^ 0xFFFFFFFF)) | (in_pUD_1->ImHeader.Flags_Precission & UD_WHEADER_SIGN_OF_BASE);
			}
			else
			{ //одинаковые знаки аргументов
				if ((in_pUD_1->ImHeader.Flags_Precission & UD_WHEADER_SIGN_OF_BASE))
					ret = SUBUNSIGNED(in_pUD_Result, in_pUD_2, in_pUD_1, UD_OPERATIONS_FLAG_COMPLEX);
				else
					ret = SUBUNSIGNED(in_pUD_Result, in_pUD_1, in_pUD_2, UD_OPERATIONS_FLAG_COMPLEX);
			}
		}
	}
	return ret;
}


DWORD CUniDigital::MUL(PUNIDIGITAL in_pUD_Result, const PUNIDIGITAL in_pUD_1, const PUNIDIGITAL in_pUD_2)
{
	DWORD ret = 0;

	return ret;
}
DWORD CUniDigital::DIV(PUNIDIGITAL in_pUD_Result, const PUNIDIGITAL in_pUD_1, const PUNIDIGITAL in_pUD_2)
{
	DWORD ret = 0;

	return ret;
}


DWORD CUniDigital::SetValue(PUNIDIGITAL in_pUD_Result, const PUNIDIGITAL in_pUD_Src)
{
	DWORD ret = 0;

	ret = SetRealValue( in_pUD_Result, in_pUD_Src);
	ret += SetComplexValue( in_pUD_Result, in_pUD_Src);

	return ret;
}

DWORD CUniDigital::SetRealValue(PUNIDIGITAL in_pUD_Result, const PUNIDIGITAL in_pUD_Src)
{
	DWORD ret = 0;

	if (in_pUD_Result && in_pUD_Src && in_pUD_Result != in_pUD_Src && (in_pUD_Src->ReHeader.Flags_Precission & UD_WHEADER_DEFINED))
	{
		int realPrecSrc = UD_PrecissionLenght_DWORD[(in_pUD_Src->ReHeader.Flags_Precission & UD_WHEADER_PRECISSION)];
		int realPrecDst = UD_PrecissionLenght_DWORD[(in_pUD_Result->ReHeader.Flags_Precission & UD_WHEADER_PRECISSION)];

		in_pUD_Result->ReHeader = in_pUD_Src->ReHeader;

		if (realPrecDst >= realPrecSrc || !(in_pUD_Src->Value[realPrecDst] & UD_ROUND_UP_BIT))
		{
			//обнуление хвоста
			while (--realPrecDst >= realPrecSrc)
				in_pUD_Result->Value[realPrecDst] = 0;
			//копирование данных
			while (realPrecDst >= 0)
				in_pUD_Result->Value[realPrecDst] = in_pUD_Src->Value[realPrecDst--];
		}
		else
		{
			register __int64 overload = 1;
			register int iPosDst = realPrecDst;
			//
			while (--realPrecDst >= 0 && overload)
			{
				register __int64 tmpValue = in_pUD_Src->Value[realPrecDst] + 1;

				overload = tmpValue & int64Discharge;
				in_pUD_Result->Value[realPrecDst] = static_cast<DWORD>(tmpValue);
			}

			if (overload) // необходимо сдвинуть данные и увеличить экпоненту
			{
				//копирование данных
				while (--realPrecDst > 0)
					in_pUD_Result->Value[realPrecDst] = in_pUD_Result->Value[realPrecDst - 1];

				in_pUD_Result->Value[realPrecDst] = 1;

				if (in_pUD_Result->ReHeader.Exponent < 127) in_pUD_Result->ReHeader.Exponent++;
				else in_pUD_Result->ReHeader.Flags_Precission |= UD_WHEADER_LIMIT_HI;
			}
		}
	}
	return ret;
}

DWORD CUniDigital::SetComplexValue(PUNIDIGITAL in_pUD_Result, const PUNIDIGITAL in_pUD_Src)
{
	DWORD ret = 0;

	if (in_pUD_Result && in_pUD_Src && in_pUD_Result != in_pUD_Src && (in_pUD_Src->ReHeader.Flags_Precission & UD_WHEADER_DEFINED) && (in_pUD_Src->ImHeader.Flags_Precission & UD_WHEADER_DEFINED))
	{
		DWORD* pValueSrc = in_pUD_Src->Value + UD_PrecissionLenght_DWORD[(in_pUD_Src->ReHeader.Flags_Precission & UD_WHEADER_PRECISSION)];
		DWORD* pValueDst = in_pUD_Result->Value + UD_PrecissionLenght_DWORD[(in_pUD_Result->ReHeader.Flags_Precission & UD_WHEADER_PRECISSION)];

		int realPrecSrc = UD_PrecissionLenght_DWORD[(in_pUD_Src->ImHeader.Flags_Precission & UD_WHEADER_PRECISSION)];
		int realPrecDst = UD_PrecissionLenght_DWORD[(in_pUD_Result->ImHeader.Flags_Precission & UD_WHEADER_PRECISSION)];

		in_pUD_Result->ImHeader = in_pUD_Src->ImHeader;

		if (realPrecDst >= realPrecSrc || !(pValueSrc[realPrecDst] & UD_ROUND_UP_BIT))
		{
			//обнуление хвоста
			while (--realPrecDst >= realPrecSrc)
				pValueDst[realPrecDst] = 0;
			//копирование данных
			while (realPrecDst >= 0)
				pValueDst[realPrecDst] = pValueSrc[realPrecDst--];
		}
		else
		{
			register __int64 overload = 1;
			register int iPosDst = realPrecDst;
			//
			while (--realPrecDst >= 0 && overload)
			{
				register __int64 tmpValue = pValueSrc[realPrecDst] + 1;

				overload = tmpValue & int64Discharge;
				pValueDst[realPrecDst] = static_cast<DWORD>(tmpValue);
			}

			if (overload) // необходимо сдвинуть данные и увеличить экпоненту
			{
				//копирование данных
				while (--realPrecDst > 0)
					pValueDst[realPrecDst] = pValueDst[realPrecDst - 1];

				pValueDst[realPrecDst] = 1;

				if (in_pUD_Result->ImHeader.Exponent < 127) in_pUD_Result->ImHeader.Exponent++;
				else in_pUD_Result->ImHeader.Flags_Precission |= UD_WHEADER_LIMIT_HI;
			}
		}
	}
	return ret;
}

DWORD CUniDigital::InitUD(PUNIDIGITAL in_pUD, DWORD in_RePrecission, DWORD in_ImPrecission)
{
	DWORD ret = 0;
	if (in_pUD)
	{
		DWORD precR = 0, precC = 0;

		if (in_RePrecission > UD_PREC_COUNT_LEV0 - 1) precR++;
		if (in_RePrecission > UD_PREC_COUNT_LEV1 - 1) precR++;
		if (in_RePrecission > UD_PREC_COUNT_LEV2 - 1) precR++;


		if ( in_ImPrecission == 0) precC = UD_WHEADER_MAX_PRECISSION_LEVEL + 1;
		else
		{
			if (in_ImPrecission > UD_PREC_COUNT_LEV0 - 1) precC++;
			if (in_ImPrecission > UD_PREC_COUNT_LEV1 - 1) precC++;
			if (in_ImPrecission > UD_PREC_COUNT_LEV2 - 1) precC++;
		}

		ret = GetLenght(precR, precC);
		memset(in_pUD, 0, ret);

		in_pUD->ReHeader.Flags_Precission = (precR & UD_WHEADER_PRECISSION) | UD_WHEADER_DEFINED;

		if (in_ImPrecission > 0)
			in_pUD->ImHeader.Flags_Precission = (precC & UD_WHEADER_PRECISSION) | UD_WHEADER_DEFINED;

	}

	return ret;
}


#define UD_CMP_RESULT_RE_LESS			0x00000001	//Меньше
#define UD_CMP_RESULT_RE_GREATER		0x00000002	//больше
#define UD_CMP_RESULT_RE_EQUAL			0x00000003	//Равны
#define UD_CMP_RESULT_RE_MASK			0x00000007	
#define UD_CMP_RESULT_RE_CLOSE_LE		0x00000010	//Близко меньше
#define UD_CMP_RESULT_RE_CLOSE_GR		0x00000020	//Близко больше
#define UD_CMP_RESULT_RE_CLOSE_EQ		0x00000030	//Близко равны
#define UD_CMP_RESULT_RE_CLOSE_MASK		0x00000070	

#define UD_CMP_RESULT_IM_LESS			0x00000100	//Меньше
#define UD_CMP_RESULT_IM_GREATER		0x00000200	//больше
#define UD_CMP_RESULT_IM_EQUAL			0x00000300	//Равны
#define UD_CMP_RESULT_IM_MASK			0x00000700	
#define UD_CMP_RESULT_IM_CLOSE_LE		0x00001000	//Близко меньше
#define UD_CMP_RESULT_IM_CLOSE_GR		0x00002000	//Близко больше
#define UD_CMP_RESULT_IM_CLOSE_EQ		0x00003000	//Близко равны
#define UD_CMP_RESULT_IM_CLOSE_MASK		0x00007000	

#define UD_CMP_RESULT_MASK				0x0000FFFF	//Определено
#define UD_CMP_RESULT_UNDEF_MASK		0xFFFF0000	//Неопределено

#define UD_CMP_RESULT_UNDEF_LEFT		0x10000000 //не определен или не инициализирован левый (первый) операнд
#define UD_CMP_RESULT_UNDEF_RIGHT		0x20000000 //не определен или не инициализирован правый (второй) операнд

#define UD_CMP_RESULT_RE_UNDEF			0x00010000	//Не инициализироано
#define UD_CMP_RESULT_RE_INF			0x00020000	// бесконечность
#define UD_CMP_RESULT_RE_UNDEF_MASK		0x000F0000	

#define UD_CMP_RESULT_IM_UNDEF			0x00100000	//Не инициализироано
#define UD_CMP_RESULT_IM_INF			0x00200000	// бесконечность
#define UD_CMP_RESULT_IM_UNDEF_MASK		0x00F00000	


DWORD CUniDigital::CmpValueUnsigned(const PUNIDIGITAL in_pUD_1, const PUNIDIGITAL in_pUD_2, DWORD in_Flags)
{
	//все что определено знаком обрабатывается выше
	int ret = 0;

	if (!in_pUD_1) ret |= UD_CMP_RESULT_UNDEF_LEFT;
	if (!in_pUD_2) ret |= UD_CMP_RESULT_UNDEF_RIGHT;

	if (!ret)
	{
		if (!(in_pUD_1->ReHeader.Flags_Precission & UD_WHEADER_DEFINED)) ret |= UD_CMP_RESULT_UNDEF_LEFT;
		if (!(in_pUD_2->ReHeader.Flags_Precission & UD_WHEADER_DEFINED)) ret |= UD_CMP_RESULT_UNDEF_RIGHT;

		if (!ret && (in_Flags & UD_OPERATIONS_FLAG_COMPLEX))
		{
			if (!(in_pUD_1->ImHeader.Flags_Precission & UD_WHEADER_DEFINED)) ret |= UD_CMP_RESULT_UNDEF_LEFT;
			if (!(in_pUD_2->ImHeader.Flags_Precission & UD_WHEADER_DEFINED)) ret |= UD_CMP_RESULT_UNDEF_RIGHT;

			if (!ret)
			{
				//infinity first
				if ((in_pUD_1->ImHeader.Flags_Precission & UD_WHEADER_INFINITY))
				{
					if ((in_pUD_2->ImHeader.Flags_Precission & UD_WHEADER_INFINITY)) ret |= UD_CMP_RESULT_IM_INF;
					else ret |= UD_CMP_RESULT_IM_GREATER;
				}
				else
					if ((in_pUD_2->ImHeader.Flags_Precission & UD_WHEADER_INFINITY)) ret |= UD_CMP_RESULT_IM_LESS;
					else //not infinity
					{
						//Exponent first
						if (in_pUD_1->ImHeader.Exponent < in_pUD_2->ImHeader.Exponent) ret |= UD_CMP_RESULT_IM_LESS;
						else
							if (in_pUD_1->ImHeader.Exponent > in_pUD_2->ImHeader.Exponent) ret |= UD_CMP_RESULT_IM_GREATER;
							else
							{
								DWORD* pValue1 = in_pUD_1->Value + UD_PrecissionLenght_DWORD[(in_pUD_1->ReHeader.Flags_Precission & UD_WHEADER_PRECISSION)] - 1;
								DWORD* pValue2 = in_pUD_2->Value + UD_PrecissionLenght_DWORD[(in_pUD_2->ReHeader.Flags_Precission & UD_WHEADER_PRECISSION)] - 1;

								int realPrec1 = UD_PrecissionLenght_DWORD[(in_pUD_1->ImHeader.Flags_Precission & UD_WHEADER_PRECISSION)] - 1;
								int realPrec2 = UD_PrecissionLenght_DWORD[(in_pUD_2->ImHeader.Flags_Precission & UD_WHEADER_PRECISSION)] - 1;
								int idx = 0;

								while (idx < realPrec1 && idx < realPrec2 && pValue1[idx] == pValue2[idx]) idx++;

								if (idx < realPrec1 && idx < realPrec2) // !=
								{
									if (pValue1[idx] > pValue2[idx]) ret |= UD_CMP_RESULT_IM_GREATER;
									else ret |= UD_CMP_RESULT_IM_LESS;
								}
								else
								{
									// == или очень близко
									ret |= UD_CMP_RESULT_IM_EQUAL;

									if (idx > realPrec1 && idx > realPrec2)
									{
										if ((in_pUD_1->ReHeader.Flags_Precission & UD_WHEADER_LIMIT_HI))
										{
											if ((in_pUD_2->ReHeader.Flags_Precission & UD_WHEADER_LIMIT_HI)) ret |= UD_CMP_RESULT_IM_CLOSE_EQ;
											else
												if ((in_pUD_2->ReHeader.Flags_Precission & UD_WHEADER_LIMIT_LOW)) ret |= UD_CMP_RESULT_IM_CLOSE_GR;
										}
										else
											if ((in_pUD_1->ReHeader.Flags_Precission & UD_WHEADER_LIMIT_LOW))
											{
												if ((in_pUD_2->ReHeader.Flags_Precission & UD_WHEADER_LIMIT_HI)) ret |= UD_CMP_RESULT_IM_CLOSE_LE;
												else
													if ((in_pUD_2->ReHeader.Flags_Precission & UD_WHEADER_LIMIT_LOW)) ret |= UD_CMP_RESULT_IM_CLOSE_EQ;
											}
											else
											{
												if ((in_pUD_2->ReHeader.Flags_Precission & UD_WHEADER_LIMIT_HI)) ret |= UD_CMP_RESULT_IM_CLOSE_GR;
												else
													if ((in_pUD_2->ReHeader.Flags_Precission & UD_WHEADER_LIMIT_LOW)) ret |= UD_CMP_RESULT_IM_CLOSE_LE;
											}
									}
									else
									{
										if (idx < realPrec1)
										{
											while (idx < realPrec1 && pValue1[idx] == 0) idx++;

											if (idx < realPrec1) ret |= UD_CMP_RESULT_IM_CLOSE_GR;
										}
										else
										{
											while (idx < realPrec2 && pValue2[idx] == 0) idx++;

											if (idx < realPrec2) ret |= UD_CMP_RESULT_IM_CLOSE_LE;
										}
									}
								}
							}
					}
			}
		}
		else
		{
			if (!(in_pUD_1->ReHeader.Flags_Precission & UD_WHEADER_DEFINED)) ret |= UD_CMP_RESULT_UNDEF_LEFT;
			if (!(in_pUD_2->ReHeader.Flags_Precission & UD_WHEADER_DEFINED)) ret |= UD_CMP_RESULT_UNDEF_RIGHT;

			if (!ret)
			{
				//infinity first
				if ((in_pUD_1->ReHeader.Flags_Precission & UD_WHEADER_INFINITY))
				{
					if ((in_pUD_2->ReHeader.Flags_Precission & UD_WHEADER_INFINITY)) ret |= UD_CMP_RESULT_RE_INF;
					else ret |= UD_CMP_RESULT_RE_GREATER;
				}
				else
					if ((in_pUD_2->ReHeader.Flags_Precission & UD_WHEADER_INFINITY)) ret |= UD_CMP_RESULT_RE_LESS;
					else //not infinity
					{
						//Exponent first
						if (in_pUD_1->ReHeader.Exponent < in_pUD_2->ReHeader.Exponent) ret |= UD_CMP_RESULT_RE_LESS;
						else
							if (in_pUD_1->ReHeader.Exponent > in_pUD_2->ReHeader.Exponent) ret |= UD_CMP_RESULT_RE_GREATER;
							else
							{
								DWORD* pValue1 = in_pUD_1->Value - 1;
								DWORD* pValue2 = in_pUD_2->Value - 1;

								int realPrec1 = UD_PrecissionLenght_DWORD[(in_pUD_1->ReHeader.Flags_Precission & UD_WHEADER_PRECISSION)] - 1;
								int realPrec2 = UD_PrecissionLenght_DWORD[(in_pUD_2->ReHeader.Flags_Precission & UD_WHEADER_PRECISSION)] - 1;
								int idx = 0;

								while (idx < realPrec1 && idx < realPrec2 && pValue1[idx] == pValue2[idx]) idx++;

								if (idx < realPrec1 && idx < realPrec2) // !=
								{
									if (pValue1[idx] > pValue2[idx]) ret |= UD_CMP_RESULT_RE_GREATER;
									else ret |= UD_CMP_RESULT_RE_LESS;
								}
								else
								{
									// == или очень близко
									ret |= UD_CMP_RESULT_RE_EQUAL;

									if (idx > realPrec1 && idx > realPrec2)
									{
										if ((in_pUD_1->ReHeader.Flags_Precission & UD_WHEADER_LIMIT_HI))
										{
											if ((in_pUD_2->ReHeader.Flags_Precission & UD_WHEADER_LIMIT_HI)) ret |= UD_CMP_RESULT_RE_CLOSE_EQ;
											else
												if ((in_pUD_2->ReHeader.Flags_Precission & UD_WHEADER_LIMIT_LOW)) ret |= UD_CMP_RESULT_RE_CLOSE_GR;
										}
										else
											if ((in_pUD_1->ReHeader.Flags_Precission & UD_WHEADER_LIMIT_LOW))
											{
												if ((in_pUD_2->ReHeader.Flags_Precission & UD_WHEADER_LIMIT_HI)) ret |= UD_CMP_RESULT_RE_CLOSE_LE;
												else
													if ((in_pUD_2->ReHeader.Flags_Precission & UD_WHEADER_LIMIT_LOW)) ret |= UD_CMP_RESULT_RE_CLOSE_EQ;
											}
											else
											{
												if ((in_pUD_2->ReHeader.Flags_Precission & UD_WHEADER_LIMIT_HI)) ret |= UD_CMP_RESULT_RE_CLOSE_GR;
												else
													if ((in_pUD_2->ReHeader.Flags_Precission & UD_WHEADER_LIMIT_LOW)) ret |= UD_CMP_RESULT_RE_CLOSE_LE;
											}
									}
									else
									{
										if (idx < realPrec1)
										{
											while (idx < realPrec1 && pValue1[idx] == 0) idx++;

											if (idx < realPrec1) ret |= UD_CMP_RESULT_RE_CLOSE_GR;
										}
										else
										{
											while (idx < realPrec2 && pValue2[idx] == 0) idx++;

											if (idx < realPrec2) ret |= UD_CMP_RESULT_RE_CLOSE_LE;
										}
									}
								}
							}
					}
			}
		}
	}

	return ret;
}


DWORD CUniDigital::CmpValue(const PUNIDIGITAL in_pUD_1, const PUNIDIGITAL in_pUD_2)
{
	DWORD ret = 0;

	return ret;
}

DWORD CUniDigital::CmpValue(const PUNIDIGITAL in_pUD_1, const PUNIDIGITAL in_pUD_2, DWORD in_Flags)
{
	DWORD ret = 0;

	return ret;
}


DWORD CUniDigital::ToDWORD(const PUNIDIGITAL in_pUD)
{
	//!!!!!!!!should be annother lowlevel fast method
	//! Этот вариант только для проверки И понимания
	DWORD ret = 0;
	CUniDigital::udLastOperationFlags = 0;
	if (in_pUD)
	{
		if ( in_pUD->ReHeader.Exponent == 0)
		{
			if ((in_pUD->ReHeader.Flags_Precission & UD_WHEADER_SIGN_OF_BASE)) //отрицательное число
				CUniDigital::udLastOperationFlags = UD_LOFLAG_MINUS_OVER;
			else 
				ret = *(in_pUD->Value);
		}
		else
			CUniDigital::udLastOperationFlags =  ((in_pUD->ReHeader.Exponent > 0) ? UD_LOFLAG_OVER : UD_LOFLAG_NAN) | ((*(in_pUD->Value) & UD_ROUND_UP_BIT) ? UD_LOFLAG_MINUS : UD_LOFLAG_PLUS);
	}
	else CUniDigital::udLastOperationFlags = UD_LOFLAG_INVALID_DATA;

	return ret;
}

double CUniDigital::ToDouble(const PUNIDIGITAL in_pUD)
{
	double ret = 0.0;
	//!!!!!!!!should be annother lowlevel fast method
	//! Этот вариант только для проверки И понимания
	
	if (in_pUD)
	{
		ret = (double)*(in_pUD->Value);
		if ( in_pUD->Value[1] )
			ret += (double)(in_pUD->Value[1]) / dblDischarge;

		if ((in_pUD->ReHeader.Flags_Precission & UD_WHEADER_SIGN_OF_BASE))
			ret = -ret;
	

		int n = in_pUD->ReHeader.Exponent;

		if (n >= 0)
			while( n-- > 0) ret *= dblDischarge;
		else 
			while (n++ < 0) ret /= dblDischarge;

	}
	else CUniDigital::udLastOperationFlags = UD_LOFLAG_INVALID_DATA;

	return ret;
}

long double CUniDigital::ToLongDouble(const PUNIDIGITAL in_pUD)
{
	long double ret = 0.0;
	//!!!!!!!!should be annother lowlevel fast method
	//! Этот вариант только для проверки И понимания

	if (in_pUD)
	{
		ret = (long double)*(in_pUD->Value);
		if (in_pUD->Value[1])
			ret += (long double)(in_pUD->Value[1]) / ldblDischarge;

		if (UD_PrecissionLenght_DWORD[(in_pUD->ReHeader.Flags_Precission & UD_WHEADER_PRECISSION)] > 2 && in_pUD->Value[2])
			ret += (long double)(in_pUD->Value[2]) / ldblDischarge2;

		if (UD_PrecissionLenght_DWORD[(in_pUD->ReHeader.Flags_Precission & UD_WHEADER_PRECISSION)] > 3 && in_pUD->Value[3])
			ret += (long double)(in_pUD->Value[3]) / ldblDischarge3;

		int n = in_pUD->ReHeader.Exponent;
		while (n-- > 0) ret *= ldblDischarge;
		while (n++ < 0) ret /= ldblDischarge;
	}
	else CUniDigital::udLastOperationFlags = UD_LOFLAG_INVALID_DATA;

	return ret;
}

float CUniDigital::ToFloat(const PUNIDIGITAL in_pUD)
{
	float ret = 0.0;
	//!!!!!!!!should be annother lowlevel fast method
	//! Этот вариант только для проверки И понимания

	if (in_pUD)
	{
		ret = (float)*(in_pUD->Value);
		if (in_pUD->Value[1])
			ret += (float)(in_pUD->Value[1]) / fltDischarge;


		int n = in_pUD->ReHeader.Exponent;

		while (n-- > 0) ret *= fltDischarge;
		while (n++ < 0) ret /= fltDischarge;

	}
	else CUniDigital::udLastOperationFlags = UD_LOFLAG_INVALID_DATA;

	return ret;
}

__int64 CUniDigital::To__int64(const PUNIDIGITAL in_pUD)
{
	//!!!!!!!!should be annother lowlevel fast method
	//! Этот вариант только для проверки И понимания
	__int64 ret = 0;
	if (in_pUD)
	{
		if ((in_pUD->ReHeader.Exponent & 0176) == 0) //нет переполнения (кроме значений со значимым старшим битом в мантиссе)
		{
			if ((in_pUD->ReHeader.Exponent & 01) == 0)
			{
				ret = *(in_pUD->Value);
				if ((in_pUD->ReHeader.Flags_Precission & UD_WHEADER_SIGN_OF_BASE))
					ret = -ret;
			}
			else
			{
				if ((*(in_pUD->Value) & UD_ROUND_UP_BIT))
				{
					if ((in_pUD->ReHeader.Flags_Precission & UD_WHEADER_SIGN_OF_BASE))
						CUniDigital::udLastOperationFlags = UD_LOFLAG_MINUS_OVER;
					else
						CUniDigital::udLastOperationFlags = UD_LOFLAG_PLUS_OVER;
				}
				else
				{
					ret = *(in_pUD->Value) * int64Discharge + in_pUD->Value[1];
					if ((in_pUD->ReHeader.Flags_Precission & UD_WHEADER_SIGN_OF_BASE))
						ret = -ret;
				}
			}
		}
		else
		{
			if ((in_pUD->ReHeader.Flags_Precission & UD_WHEADER_SIGN_OF_BASE))
				CUniDigital::udLastOperationFlags = UD_LOFLAG_MINUS_OVER;
			else
				CUniDigital::udLastOperationFlags = UD_LOFLAG_PLUS_OVER;
		}
	}
	else CUniDigital::udLastOperationFlags = UD_LOFLAG_INVALID_DATA;

	return ret;

}

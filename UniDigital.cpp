#include "stdafx.h"
#include "UniDigital.h"


PUNIDIGITAL CUniDigital::m_First;
PUNIDIGITAL CUniDigital::m_Second;
PUNIDIGITAL CUniDigital::m_Result;
PUNIDIGITAL CUniDigital::m_Slave;
DWORD CUniDigital::m_dwOALenght;

//длинна мантиссы в байтах в зависимости от значения Header.Flags_Precission
const DWORD UD_PrecissionLenght_BYTE[]  = { 0x08, 0x10, 0x3C, 0xFC };
const DWORD UD_PrecissionLenght_WORD[]  = { 0x04, 0x08, 0x1E, 0x7E };
const DWORD UD_PrecissionLenght_DWORD[] = { 0x02, 0x04, 0x0f, 0x3F };


CUniDigital::CUniDigital()
{
	m_dwOALenght = 0x100 * sizeof(DWORD) + 256;

	DWORD maxMantissa = UD_PrecissionLenght_BYTE[UD_WHEADER_PRECISSION] * 2;
	m_First = (PUNIDIGITAL)new BYTE [maxMantissa ];
	ZeroMemory(m_First, maxMantissa);
	m_Second = (PUNIDIGITAL)new BYTE[maxMantissa ];
	ZeroMemory(m_Second, maxMantissa );
	m_Result = (PUNIDIGITAL)new BYTE[m_dwOALenght];
	ZeroMemory(m_Result, m_dwOALenght);
	m_Slave = (PUNIDIGITAL)new BYTE[m_dwOALenght];
	ZeroMemory(m_Slave, m_dwOALenght);
}


CUniDigital::~CUniDigital()
{
	if (m_First) delete [] m_First;
	if (m_Second) delete [] m_Second;
	if (m_Result) delete [] m_Result;
	if (m_Slave) delete [] m_Slave;

	m_First = NULL;
	m_Second = NULL;
	m_Result = NULL;
	m_Slave = NULL;
}


DWORD CUniDigital::GetLenght(PUNIDIGITAL in_pUD)
{
	DWORD ret = sizeof(UNIDIGITAL);
	if (in_pUD)
	{
		if ((in_pUD->ReHeader.Flags_Precission & UD_WHEADER_DEFINED))
			ret = sizeof(UNIDIGITAL_HEADER) + UD_PrecissionLenght_BYTE[ in_pUD->ReHeader.Flags_Precission & UD_WHEADER_PRECISSION];

		if ((in_pUD->ImHeader.Flags_Precission & UD_WHEADER_DEFINED))
			ret += UD_PrecissionLenght_BYTE[in_pUD->ImHeader.Flags_Precission & UD_WHEADER_PRECISSION];
	}
	return ret;
}

DWORD CUniDigital::GetLenght(DWORD in_RePrecission)
{
	return	sizeof(UNIDIGITAL_HEADER) + UD_PrecissionLenght_BYTE[in_RePrecission & UD_WHEADER_PRECISSION];
}

DWORD CUniDigital::GetLenght(DWORD in_RePrecission, DWORD in_ImPrecission)
{
	return	sizeof(UNIDIGITAL_HEADER) + UD_PrecissionLenght_BYTE[in_RePrecission & UD_WHEADER_PRECISSION] + UD_PrecissionLenght_BYTE[in_ImPrecission & UD_WHEADER_PRECISSION];
}

DWORD CUniDigital::ADDUNSIGNED(PUNIDIGITAL in_pUD_Result, PUNIDIGITAL in_pUD_1, PUNIDIGITAL in_pUD_2, DWORD in_Flags)
{ //быстродействие не цель. Быстрые расчеты можно сделать в специализированных навыках
	//необходимо доработать. Нет проверки на переполнение. Нечеткая фиксация точки нулевой экспоненты. Вообще, криво все.
	//не устанавливается признак смещения после округления
	//вообще не тестировалось комплексно. Нет времени. Надо все проверить и оптимизировать


	PUNIDIGITAL_HEADER h1, h2, hR;
	DWORD *D1, *D2, *DR = NULL;
	if ((in_Flags & UD_OPERATIONS_FLAG_REAL))
	{
		D1 = in_pUD_1->Value;
		D2 = in_pUD_2->Value;
		if (*D1 == 0)		SetRealValue(in_pUD_Result, in_pUD_2); 
		else if (*D2 == 0)	SetRealValue(in_pUD_Result, in_pUD_1); 
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
		D1 = in_pUD_1->Value + UD_PrecissionLenght_DWORD[(h1->Flags_Precission & UD_WHEADER_PRECISSION)];
		D2 = in_pUD_2->Value + UD_PrecissionLenght_DWORD[(h2->Flags_Precission & UD_WHEADER_PRECISSION)];
		if (*D1 == 0)		SetComplexValue(in_pUD_Result, in_pUD_2);
		else if (*D2 == 0)	SetComplexValue(in_pUD_Result, in_pUD_1);
		else
		{
			h1 = &in_pUD_1->ImHeader;
			h2 = &in_pUD_2->ImHeader;
			hR = &in_pUD_Result->ImHeader;
			DR = in_pUD_Result->Value + UD_PrecissionLenght_DWORD[(hR->Flags_Precission & UD_WHEADER_PRECISSION)];
		}
	}

	if (DR)
	{
		ZeroMemory(m_Result, m_dwOALenght);
		DWORD *dDest1 = m_Result->Value + (128 - h1->Exponent);
		CopyMemory(dDest1, D1, UD_PrecissionLenght_BYTE[h1->Flags_Precission & UD_WHEADER_PRECISSION]);

		DWORD *pDestADD = (m_Result->Value + (128 - h2->Exponent) + UD_PrecissionLenght_DWORD[(h2->Flags_Precission & UD_WHEADER_PRECISSION)]) - 1;
		DWORD *pSrcADD = (D2 + UD_PrecissionLenght_DWORD[(h2->Flags_Precission & UD_WHEADER_PRECISSION)]) - 1;
		int nStep = UD_PrecissionLenght_DWORD[(h2->Flags_Precission & UD_WHEADER_PRECISSION)];

		__int64 tmpValue;
		DWORD Overload = 0;
		while (nStep--)
		{
			tmpValue = *(pSrcADD--) + *pDestADD + Overload;
			if ((tmpValue & 0xFFFFFFFF00000000)) Overload = 1; else Overload = 0;
			*(pDestADD--) = (tmpValue & 0x00000000FFFFFFFF);
		}

		while (Overload && pDestADD > m_Result->Value)
		{
			tmpValue = *pDestADD + Overload;
			if ((tmpValue & 0xFFFFFFFF00000000)) Overload = 1; else Overload = 0;
			*(pDestADD--) = (tmpValue & 0x00000000FFFFFFFF);
		}

		dDest1 = m_Result->Value;
		nStep = 0;
		while (!*dDest1 && nStep++ < 0x100) dDest1++;

		if (nStep < 0x100)
		{//ненулевое значение
			hR->Exponent = 128 - nStep;
			if (*(dDest1 + UD_PrecissionLenght_DWORD[(hR->Flags_Precission & UD_WHEADER_PRECISSION)]) > 0x7FFFFFFE)
			{
				Overload = 1;
				DWORD *pDestTMP = dDest1 + UD_PrecissionLenght_DWORD[(hR->Flags_Precission & UD_WHEADER_PRECISSION)] - 1;
				while (Overload && pDestTMP >= dDest1)
				{
					tmpValue = *pDestTMP + Overload;
					if (tmpValue == 0) Overload = 1; else Overload = 0;
					*(pDestTMP--) = (DWORD)tmpValue;
				}
				if (Overload)
				{
					*(--dDest1) = 0x1;
					hR->Exponent++;
				}
			}
			CopyMemory(DR, dDest1, UD_PrecissionLenght_BYTE[hR->Flags_Precission & UD_WHEADER_PRECISSION]);
		}
		else
		{// == 0
			hR->Exponent = 0;
			ZeroMemory(DR, UD_PrecissionLenght_BYTE[(hR->Flags_Precission & UD_WHEADER_PRECISSION)]);
			hR->Flags_Precission |= UD_WHEADER_DEFINED;
			hR->Flags_Precission &= (0xFFFFFFFF ^ (UD_WHEADER_INFINITY | UD_WHEADER_UNKNOWN));
		}
	}
	return 0;
}

DWORD CUniDigital::SUBUNSIGNED(PUNIDIGITAL in_pUD_Result, PUNIDIGITAL in_pUD_1, PUNIDIGITAL in_pUD_2, DWORD in_Flags)
{	//быстродействие не цель. Быстрые расчеты можно сделать в специализированных навыках
	//необходимо доработать. Нет проверки на переполнение. Нечеткая фиксация точки нулевой экспоненты. Вообще, криво все.
	//не устанавливается признак смещения после округления
	//вообще не тестировалось комплексно. Нет времени. Надо все проверить и оптимизировать

	DWORD bNeedChangeSign = 0;
	PUNIDIGITAL_HEADER h1, h2, hR;
	DWORD *D1, *D2, *DR = NULL;
	if ((in_Flags & UD_OPERATIONS_FLAG_REAL))
	{
		h1 = &in_pUD_1->ReHeader;
		h2 = &in_pUD_2->ReHeader;
		D1 = in_pUD_1->Value;
		D2 = in_pUD_2->Value;
		if (*D2 == 0)	SetRealValue(in_pUD_Result, in_pUD_1);
		else if (*D1 == 0) { SetRealValue(in_pUD_Result, in_pUD_2);  bNeedChangeSign = 1; }
		else
		{
			hR = &in_pUD_Result->ReHeader;
			DR = in_pUD_Result->Value;
		}
	}
	else
	{
		h1 = &in_pUD_1->ImHeader;
		h2 = &in_pUD_2->ImHeader;
		D1 = in_pUD_1->Value + UD_PrecissionLenght_DWORD[(h1->Flags_Precission & UD_WHEADER_PRECISSION)];
		D2 = in_pUD_2->Value + UD_PrecissionLenght_DWORD[(h2->Flags_Precission & UD_WHEADER_PRECISSION)];
		if (*D2 == 0)	SetComplexValue(in_pUD_Result, in_pUD_1);
		else if (*D1 == 0) { SetComplexValue(in_pUD_Result, in_pUD_2); bNeedChangeSign = 1; }
		else
		{
			hR = &in_pUD_Result->ImHeader;
			DR = in_pUD_Result->Value + UD_PrecissionLenght_DWORD[(hR->Flags_Precission & UD_WHEADER_PRECISSION)];
		}
	}

	if (DR)
	{
		switch (CUniDigital::CmpValueUnsigned(in_pUD_1, in_pUD_2, in_Flags))
		{
		case UD_CMP_EQUAL:
			hR->Exponent = 0;
			ZeroMemory(DR, UD_PrecissionLenght_BYTE[(hR->Flags_Precission & UD_WHEADER_PRECISSION)]);
			hR->Flags_Precission |= UD_WHEADER_DEFINED;
			hR->Flags_Precission &= (0xFFFFFFFF ^ (UD_WHEADER_INFINITY | UD_WHEADER_UNKNOWN));
			DR = NULL;
			break;
		case UD_CMP_MORE:
		case UD_CMP_LESS:
		case UD_CMP_UNKNOWN:
			hR->Exponent = 0;
			ZeroMemory(DR, UD_PrecissionLenght_BYTE[(hR->Flags_Precission & UD_WHEADER_PRECISSION)]);
			hR->Flags_Precission |= (UD_WHEADER_DEFINED | UD_WHEADER_UNKNOWN);
			DR = NULL;
			break;
		}



		if (DR)
		{




			ZeroMemory(m_Result, m_dwOALenght);
			DWORD *dDest1 = m_Result->Value + (128 - h1->Exponent);
			CopyMemory(dDest1, D1, UD_PrecissionLenght_BYTE[h1->Flags_Precission & UD_WHEADER_PRECISSION]);

			DWORD *pDestADD = (m_Result->Value + (128 - h2->Exponent) + UD_PrecissionLenght_DWORD[(h2->Flags_Precission & UD_WHEADER_PRECISSION)]) - 1;
			DWORD *pSrcADD = (D2 + UD_PrecissionLenght_DWORD[(h2->Flags_Precission & UD_WHEADER_PRECISSION)]) - 1;
			int nStep = UD_PrecissionLenght_DWORD[(h2->Flags_Precission & UD_WHEADER_PRECISSION)];

			__int64 tmpValue;
			DWORD Overload = 0;
			while (nStep--)
			{
				tmpValue = *(pSrcADD--) + *pDestADD + Overload;
				if ((tmpValue & 0xFFFFFFFF00000000)) Overload = 1; else Overload = 0;
				*(pDestADD--) = (tmpValue & 0x00000000FFFFFFFF);
			}

			while (Overload && pDestADD > m_Result->Value)
			{
				tmpValue = *pDestADD + Overload;
				if ((tmpValue & 0xFFFFFFFF00000000)) Overload = 1; else Overload = 0;
				*(pDestADD--) = (tmpValue & 0x00000000FFFFFFFF);
			}

			dDest1 = m_Result->Value;
			nStep = 0;
			while (!*dDest1 && nStep++ < 0x100) dDest1++;

			if (nStep < 0x100)
			{//ненулевое значение
				hR->Exponent = 128 - nStep;
				if (*(dDest1 + UD_PrecissionLenght_DWORD[(hR->Flags_Precission & UD_WHEADER_PRECISSION)]) > 0x7FFFFFFE)
				{
					Overload = 1;
					DWORD *pDestTMP = dDest1 + UD_PrecissionLenght_DWORD[(hR->Flags_Precission & UD_WHEADER_PRECISSION)] - 1;
					while (Overload && pDestTMP >= dDest1)
					{
						tmpValue = *pDestTMP + Overload;
						if (tmpValue == 0) Overload = 1; else Overload = 0;
						*(pDestTMP--) = (DWORD)tmpValue;
					}
					if (Overload)
					{
						*(--dDest1) = 0x1;
						hR->Exponent++;
					}
				}
				CopyMemory(DR, dDest1, UD_PrecissionLenght_BYTE[hR->Flags_Precission & UD_WHEADER_PRECISSION]);
			}
			else
			{// == 0
				hR->Exponent = 0;
				ZeroMemory(DR, UD_PrecissionLenght_BYTE[(hR->Flags_Precission & UD_WHEADER_PRECISSION)]);
				hR->Flags_Precission |= UD_WHEADER_DEFINED;
				hR->Flags_Precission &= (0xFFFFFFFF ^ (UD_WHEADER_INFINITY | UD_WHEADER_UNKNOWN));
			}
		}
	}

	if (bNeedChangeSign) in_pUD_Result->ImHeader.Flags_Precission |= UD_WHEADER_SIGN_OF_BASE;

	return 0;
}

DWORD CUniDigital::ADD(PUNIDIGITAL in_pUD_Result, PUNIDIGITAL in_pUD_1, PUNIDIGITAL in_pUD_2)
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
		if (!(in_pUD_1->ImHeader.Flags_Precission&UD_WHEADER_DEFINED) && !(in_pUD_1->ImHeader.Flags_Precission&UD_WHEADER_DEFINED))
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


DWORD CUniDigital::SUB(PUNIDIGITAL in_pUD_Result, PUNIDIGITAL in_pUD_1, PUNIDIGITAL in_pUD_2)
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
DWORD CUniDigital::MUL(PUNIDIGITAL in_pUD_Result, PUNIDIGITAL in_pUD_1, PUNIDIGITAL in_pUD_2)
{
	DWORD ret = 0;

	return ret;
}
DWORD CUniDigital::DIV(PUNIDIGITAL in_pUD_Result, PUNIDIGITAL in_pUD_1, PUNIDIGITAL in_pUD_2)
{
	DWORD ret = 0;

	return ret;
}
DWORD CUniDigital::SetValue(PUNIDIGITAL in_pUD_Result, PUNIDIGITAL in_pUD_Src)
{
	DWORD ret = 0;

	return ret;
}

DWORD CUniDigital::SetRealValue(PUNIDIGITAL in_pUD_Result, PUNIDIGITAL in_pUD_Src)
{
	DWORD ret = 0;

	return ret;
}

DWORD CUniDigital::SetComplexValue(PUNIDIGITAL in_pUD_Result, PUNIDIGITAL in_pUD_Src)
{
	DWORD ret = 0;

	return ret;
}

DWORD CUniDigital::InitUD(PUNIDIGITAL in_pUD, DWORD in_RePrecission, DWORD in_ImPrecission)
{
	DWORD ret = 0;
	if (in_pUD)
	{
		ret = GetLenght(in_RePrecission, in_ImPrecission);
		ZeroMemory(in_pUD, ret);
		in_pUD->ReHeader.Flags_Precission = (in_RePrecission & UD_WHEADER_PRECISSION) | UD_WHEADER_DEFINED;
		in_pUD->ImHeader.Flags_Precission = (in_ImPrecission & UD_WHEADER_PRECISSION) | UD_WHEADER_DEFINED;
	}

	return ret;
}

DWORD CUniDigital::CmpValueUnsigned(PUNIDIGITAL in_pUD_1, PUNIDIGITAL in_pUD_2, DWORD in_Flags)
{

	return 0;
}
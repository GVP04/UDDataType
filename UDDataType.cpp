// UDDataType.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Windows.h>
#include <iostream>
#include <sysinfoapi.h>
#include "UniDigital.h"
#include "BFP08.h"
#include "BFP16.h"
#include "BFP32.h"


void CheckToFromDouble()
{
    //test in out double
    double gg_mul[4] = { 0.999 ,-0.999 ,1.001 ,-1.001 };
    double gg_ini[4] = { 1.0 ,-1.0 ,1.0 ,-1.0 , };
    double gggg = 1.0;
    double ggggLast = 1.0;
    int max_I = 1000000;

    for (int k = 0; k < 3; k++)
	{
        int i, j;

		for (j = 0; j < 4; j++)
		{
			gggg = gg_ini[j];
            double dRes = gggg;
            double dResLast;
			BFP08 CBFP08_1 = BFP08(gggg);
			BFP16 CBFP16_1 = BFP16(gggg);
			BFP32 CBFP32_1 = BFP32(gggg);
            switch (k)
            {
            case 0:      std::cout << " CBFP08\t";               break;
            case 1:      std::cout << " CBFP16\t";               break;
            case 2:      std::cout << " CBFP32\t";               break;
            }

			for (i = 1; i < max_I; i++)
			{
                dResLast = dRes;
                switch (k)
				{
				case 0:
					CBFP08_1 = BFP08(gggg);
					dRes = CBFP08::ToDouble(CBFP08_1);
					break;
				case 1:
					CBFP16_1 = BFP16(gggg);
					dRes = CBFP16::ToDouble(CBFP16_1);
					break;
				case 2:
					CBFP32_1 = BFP32(gggg);
					dRes = CBFP32::ToDouble(CBFP32_1);
					break;
				}

				if (gggg > 0)
				{
					if (gggg < dRes / 1.01)     i += max_I;
					if (gggg > dRes * 1.01)     i += max_I;
				}
				else
				{
					if (gggg > dRes / 1.01)     i += max_I;
					if (gggg < dRes * 1.01)     i += max_I;
				}
				if (i >= max_I)
				{
					char tmps[5000];
					sprintf_s(tmps, 5000, "%15.15g", gggg);
					std::cout << "j=" << j << "\t" << " i=" << i - max_I << "\t" << " gggg=" << tmps << "\t";
					sprintf_s(tmps, 5000, "%15.15g", dRes);
					std::cout << " dRes=" << tmps << "\t";
                    sprintf_s(tmps, 5000, "%15.15g", ggggLast);
                    std::cout << " ggggLast=" << tmps << "\t";
                    sprintf_s(tmps, 5000, "%15.15g", dResLast);
                    std::cout << " dResLast=" << tmps << "\n";
                    i++;
				}
                else
                {
				    ggggLast = gggg;
				    gggg *= gg_mul[j];
                }
			}
			if (i == max_I)	std::cout << "j=" << j << "\t" << " i=" << i << "\t" << " Ok" << "\n";
		}
	}
}

void CheckToFromLongDouble()
{
    //test in out double
    long double gg_mul[4] = { 0.999 ,-0.999 ,1.001 ,-1.001 };
    long double gg_ini[4] = { 1.0 ,-1.0 ,1.0 ,-1.0 , };
    long double gggg = 1.0;
    long double ggggLast = 1.0;
    int max_I = 1000000;
        int i, j;

		for (j = 0; j < 4; j++)
		{
			gggg = gg_ini[j];
            double dRes = gggg;
            double dResLast;
			BFP32 CBFP32_1 = BFP32(gggg);

			for (i = 1; i < max_I; i++)
			{
                dResLast = dRes;
				CBFP32_1 = BFP32(gggg);
				dRes = CBFP32::ToDouble(CBFP32_1);

				if (gggg > 0)
				{
					if (gggg < dRes / 1.0001)     i += max_I;
					if (gggg > dRes * 1.0001)     i += max_I;
				}
				else
				{
					if (gggg > dRes / 1.0001)     i += max_I;
					if (gggg < dRes * 1.0001)     i += max_I;
				}
				if (i >= max_I)
				{
					char tmps[5000];
					sprintf_s(tmps, 5000, "%15.15g", gggg);
					std::cout << "j=" << j << "\t" << " i=" << i - max_I << "\t" << " gggg=" << tmps << "\t";
					sprintf_s(tmps, 5000, "%15.15g", dRes);
					std::cout << " dRes=" << tmps << "\t";
                    sprintf_s(tmps, 5000, "%15.15g", ggggLast);
                    std::cout << " ggggLast=" << tmps << "\t";
                    sprintf_s(tmps, 5000, "%15.15g", dResLast);
                    std::cout << " dResLast=" << tmps << "\n";
                    i++;
				}
                else
                {
				    ggggLast = gggg;
				    gggg *= gg_mul[j];
                }
			}
			if (i == max_I)	std::cout << "j=" << j << "\t" << " i=" << i << "\t" << " Ok" << "\n";
		}

}
 
int main()
{
    int i, j, k;
    CUniDigital ud;
    CBFP08 ccCBFP08;

    char* arrtp = new char[110000];

    PUNIDIGITAL s1 = (PUNIDIGITAL)arrtp;
    PUNIDIGITAL s2 = (PUNIDIGITAL)(arrtp + 10000);
    PUNIDIGITAL Result = (PUNIDIGITAL)(arrtp + 20000);
    char* tmps = arrtp + 100000;

    CUniDigital::InitUD(s1, 3, 0);
    CUniDigital::InitUD(s2, 3, 0);
    CUniDigital::InitUD(Result, 2, 0);


    //CheckToFromDouble();
    CheckToFromLongDouble();
 

    double d1 = 1.0;
    double d2 = 1.0000001;
    double dRes = 10000000.0;

    int tic_strt, tic_end;

    tic_strt = GetTickCount();

    for( j = 0; j < 100000; j++)
        for (i = 0; i < 10000; i++)
            dRes /= d2;
            //dRes += d2;

    tic_end = GetTickCount();

    sprintf_s(tmps, 5000, "%15.15g", dRes);

    std::cout << tmps << "\t" << tic_end - tic_strt << "\n";


    ////////////////////////////////////////////////////////////////////
    //double _d1 = 1.0;
    //double _d2 = 1.0000001;
    //double _dRes = 10000000.0;

    //tic_strt = GetTickCount();

    //for (j = 0; j < 100000; j++)
    //{
    //    for (i = 0; i < 10000; i++)
    //    {
    //         _dRes = CBFP16::DIVDBL(_dRes, _d2);
    //    }
    //}

    //tic_end = GetTickCount();

    //sprintf_s(tmps, 5000, "%15.15g", dRes);

    //std::cout << tmps << "\t" << tic_end - tic_strt << "\n";


    //sprintf_s(tmps, 5000, "%15.15g", _dRes);

    //std::cout << tmps << "\t" << tic_end - tic_strt  << "\n";

    ////////////////////////////////////////////////////////////////////////////////

    BFP16 CBFP16_1 = BFP16(10000000.0);
    BFP16 CBFP16_2 = BFP16(1.0000001);
    BFP16 CBFP16_3 = BFP16(2);


    d1 = 1.0;
    d2 = 1.0000001;
    dRes = 10000000.0;



    double _dRes = CBFP16::ToDouble(CBFP16_1);

    tic_strt = GetTickCount();
 
    for (j = 0; j < 100000; j++)
    {
        for (i = 0; i < 10000; i++)
        {
            //CBFP08::ADD(&CBFP08_1, CBFP08_1, CBFP08_2);
            //CBFP08::MUL(&CBFP08_1, CBFP08_1, CBFP08_2);
            //CBFP16::MUL(&CBFP16_1, CBFP16_1, CBFP16_2);
            CBFP16::DIV(&CBFP16_1, CBFP16_1, CBFP16_2);
         }
    }

    tic_end = GetTickCount();
    _dRes = CBFP16::ToDouble(CBFP16_1);

    sprintf_s(tmps, 5000, "%15.15g", _dRes);
    std::cout << tmps << "\t" << tic_end - tic_strt << "\t" << CBFP16_1.Mantissa << "\n";

    delete[] arrtp;
}



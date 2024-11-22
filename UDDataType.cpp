// UDDataType.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Windows.h>
#include <iostream>
#include <sysinfoapi.h>
#include "UniDigital.h"


int main()
{
     CUniDigital ud;

    char* arrtp = new char[110000];

    PUNIDIGITAL s1 = (PUNIDIGITAL)arrtp;
    PUNIDIGITAL s2 = (PUNIDIGITAL)(arrtp + 10000);
    PUNIDIGITAL Result = (PUNIDIGITAL)(arrtp + 20000);
    char* tmps = arrtp + 100000;

    CUniDigital::InitUD(s1, 3, 0);
    CUniDigital::InitUD(s2, 3, 0);
    CUniDigital::InitUD(Result, 2, 0);

    *(Result->Value) = 0;
    Result->ReHeader.Exponent = 0;
    s1->Value[0] = 290;
    s1->Value[1] = 0x19999999;// 1/10 == 0x199999999......
    s1->Value[2] = 0x99999999;// 
    *(s2->Value) = 7;



    //ud.SUBUNSIGNED(&Result->ReHeader, Result->Value, &s2->ReHeader, s2->Value);
    //sprintf_s(tmps, 5000, "%15.15g", CUniDigital::ToDouble(Result));

    //std::cout << tmps << "\t" << "\n";


    int i, j;


    double d1 = 0.0;
    double d2 = 290.1;
    double dRes = 0.0;

    int ticD_strt, ticD_end, ticU_strt, ticU_end;

    ticD_strt = GetTickCount();

    for( j = 0; j < 10; j++)
        for (i = 0; i < 10000; i++)
            dRes += d2;

    ticD_end = GetTickCount();

    sprintf_s(tmps, 5000, "%15.15g", dRes);

    std::cout << tmps << "\t" << ticD_end - ticD_strt << "\n";

    ticU_strt = GetTickCount();

    for ( j = 0; j < 10; j++)
        for (i = 0; i < 10000; i++)
        {
             //ud.ADD_LL(&Result->ReHeader, Result->Value, &s1->ReHeader, s1->Value);
            //    ud.SUB_LL(&Result->ReHeader, Result->Value, &s2->ReHeader, s2->Value);

            ud.ADD(Result, Result, s1);


        }
    

    ticU_end = GetTickCount();

    sprintf_s(tmps, 5000, "%15.15g", CUniDigital::ToDouble(Result));

    std::cout << tmps << "\t" << ticU_end - ticU_strt << "\n";


/*

    for (i = 0; i < 100; i++)
    {
        *(s1->Value) = *(Result->Value) + i;
        std::cout << *Result->Value << "_" << Result->Value[1] << "\t" << *s1->Value << "\t = \t";

        ud.ADDUNSIGNED(&Result->ReHeader, Result->Value, &s1->ReHeader, s1->Value);

        std::cout << *Result->Value << "\n";
    }

*/


    delete[] arrtp;
}



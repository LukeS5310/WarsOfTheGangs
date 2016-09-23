/*
*	Created:			03.08.2013
*	Author:				Y_Less
*	Description:		Тест скорости выполнения функций
*/

#if defined _tests_included
	#endinput
#endif
#include<a_samp>
#define _tests_included
#pragma library tests

#define CODE_1 printf("%d", 42);
#define CODE_2 new str[4]; format(str, sizeof (str), "%d", 42); print(str);
#define CODE_3 print("42");
#define ITERATIONS (10000)

main()
{
    new
        t0,
        t1,
        t2,
        t3,
        i;
    t0 = GetTickCount();
    for (i = 0; i < ITERATIONS; i++)
    {
        CODE_1
    }
    t1 = GetTickCount();
    for (i = 0; i < ITERATIONS; i++)
    {
        CODE_2
    }
    t2 = GetTickCount();
    for (i = 0; i < ITERATIONS; i++)
    {
        CODE_3
    }
    t3 = GetTickCount();
    printf("Time 1: %04d, time 2: %04d, time 3: %04d", t1 - t0, t2 - t1, t3 - t2);
}

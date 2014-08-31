// float_double.cpp : Defines the entry point for the console application.
//

#include <cstdio>
#include <Windows.h>
#include <cmath>
#include <float.h>

#define ARR_SIZE 1000000
#define NUM_ITER 1000

#define INIT_TIME() LARGE_INTEGER startTime, endTime, freqTime; \
					QueryPerformanceFrequency(&freqTime);
#define START_TIME() QueryPerformanceCounter(&startTime)
#define END_TIME() QueryPerformanceCounter(&endTime)
#define PRINT_TIME(msg) { printf(msg); printf("%d micro sec\n", (endTime.QuadPart - startTime.QuadPart) * 1000000/freqTime.QuadPart); }

int main(int argc, char* argv[])
{
	INIT_TIME();

	printf("float VS double\n");
	printf("sizeof(float): %d\n", sizeof(float));
	printf("sizeof(double): %d\n", sizeof(double));


	//
	// set precission to single:
	//

	// Show original x87 control word and do calculation.
	unsigned int control_word_x87;
    control_word_x87 = __control87_2(0, 0, &control_word_x87, 0);
    printf( "Original: 0x%.4x\n", control_word_x87 );

    // Set precision to 24 bits
    control_word_x87 = __control87_2(_PC_24, MCW_PC, &control_word_x87, 0);
    printf( "24-bit:   0x%.4x\n", control_word_x87 );

	//
	// test float:
	//
	float *floatArray = (float *)malloc(ARR_SIZE * sizeof(float));

	START_TIME();
	for (int i = 0; i < ARR_SIZE; ++i)
	{
		floatArray[i] = (float)(i*i)/100.0f;
	}
	
	for (int i = 0; i < ARR_SIZE; ++i)
	{
		float temp = 0.0f;
		for (int j = 0; j < NUM_ITER; ++j)
		{
			temp += floatArray[j]*2.0f;
			temp += sqrtf(floatArray[j]*0.1f);
		}
		temp = sqrtf(temp);
		floatArray[i] = temp;
	}
	END_TIME();
	free(floatArray);

	PRINT_TIME("processing float: ");

	//
	// test double:
	//

	// Restore default precision-control bits and recalculate.
    control_word_x87 = __control87_2( _PC_64, MCW_PC, 
                                     &control_word_x87, 0 );
    printf( "Default:  0x%.4x\n", control_word_x87 );

	double *doubleArray = (double *)malloc(ARR_SIZE * sizeof(double));

	START_TIME();
	for (int i = 0; i < ARR_SIZE; ++i)
	{
		doubleArray[i] = (double)(i*i)/100.0;
	}
	
	for (int i = 0; i < ARR_SIZE; ++i)
	{
		double temp = 0.0;
		for (int j = 0; j < NUM_ITER; ++j)
		{
			temp += doubleArray[j]*2.0;
			temp += sqrt(doubleArray[j]*0.1);
		}
		temp = sqrt(temp);
		doubleArray[i] = temp;
	}
	END_TIME();

	free(doubleArray);

	PRINT_TIME("processing double: ");

	return 0;
}


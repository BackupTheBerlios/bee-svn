//=    & Sample series of data which can be integers or reals.              =
//=      There are 11 values in this file. &                                =
//=     0.0                                                                 =
//=     2.9                                                                 =
//=     8.9                                                                 =
//=     1.5                                                                 =
//=     0.5                                                                 =
//=     3.1                                                                 =
//=     9.0                                                                 =
//=     4.0                                                                 =
//=    11.5                                                                 =
//=     9.0                                                                 =
//=     0.0                                                                 =
//=-------------------------------------------------------------------------=
//= Example output (for above "in.dat", BUCKET_SIZE = 2.0, NUM_BUCKET = 5)  =
//=                                                                         =
//=   ------------------------------------------------ hist.c -----         =
//=     Frequency for 5 buckets of size 2.000000 (for 11 samples)           =
//=        0.000000000 <= X <    2.000000000 =      4 -- 36.3636 %          =
//=        2.000000000 <= X <    4.000000000 =      2 -- 18.1818 %          =
//=        4.000000000 <= X <    6.000000000 =      1 --  9.0909 %          =
//=        6.000000000 <= X <    8.000000000 =      0 --  0.0000 %          =
//=        8.000000000 <= X <   10.000000000 =      3 -- 27.2727 %          =
#include <stdio.h>                 // Needed for printf() and feof()
#include <stdlib.h>                // Needed for exit(), atof(), and qsort()
#include <string.h>                // Needed for strcmp()

#define MAX_SIZE      1000000L     // Maximum size of time series data array
int BUCKET_SIZE=0 ;     // Bucket size
int NUM_BUCKET =0  ;     // Number of buckets

double     X[MAX_SIZE];            // Time series read from "in.dat"
long int   N;                      // Number of values in "in.dat"

void load_X_array(void);                    // Load X array
int  comp(const void *p, const void *q);    // Compare p and q for qsort()

void main(int argc, char* argv[])
{
    int i, j;                        // Loop counters
    int count;                       // Frequency counter for this bucket
    int total_count;                 // Total count of all buckets
    int start;                       // Start index for histogram loop

    if( argc< 3 ) { printf("%s bucket_size  num_buckets\n", argv[0]); exit(2); }
    BUCKET_SIZE = atoi(argv[1] );
    NUM_BUCKET  = atoi(argv[2] ); 


    load_X_array();
    qsort(X, N, sizeof(double), comp);
    // Do for all the buckets
    total_count = 0;
    start = 0;
    for (i=0; i<NUM_BUCKET; i++)
    {
        // Scan X[] for current bucket size
        count = 0;
        for(j=start; j<N; j++)
        {
            // Count as in this bucket
            if (X[j] < ((i+1) * (double) BUCKET_SIZE))
            {
                count++;
                total_count++;
            }

            // For this case need to start couting for next bucket
            if (X[j] >= ((i+1) * (double) BUCKET_SIZE))
            {
                start = j;
                break;
            }

            // For this case we are all done
            if (j == (N-1))
            {
                start = N;
                break;
            }
        }

        // Output count and percentage for current bucket size
        //printf("  %14.9f <= X < %14.9f = %6ld -- %f %% \n", (i * (double) BUCKET_SIZE), ((i + 1) * (double) BUCKET_SIZE), count, 100.0 * ((double) count / N));
    printf("%f %6ld \n", ((i + 1) * (double) BUCKET_SIZE), count);
    }

    // Check if any were not counted and output count and percentage
    if (total_count < N)
    {
        printf("-------------------------------------------------------------\n");
        printf("  Not in range = %6d -- %f %% \n",
                (N - total_count), 100.0 * ((double) (N - total_count) / N));
    }
}






void load_X_array(void)
{
    char      temp_string[1024];     // Temporary string variable

    // Read all values into X
    N = 0;
    while(1)
    {
        scanf("%s", temp_string);
        if (feof(stdin)) goto end;

        // This handles a comment bounded by "&" symbols
        while (strcmp(temp_string, "&") == 0)
        {
            do
            {
                scanf("%s", temp_string);
                if (feof(stdin)) goto end;
            } while (strcmp(temp_string, "&") != 0);
            scanf("%s", temp_string);
            if (feof(stdin)) goto end;
        }

        // Enter value in array and increment array index
        X[N] = atof(temp_string);
        N++;

        // Check if MAX_SIZE data values exceeded
        if (N >= MAX_SIZE)
        {
            printf("*** ERROR - greater than %ld data values \n", MAX_SIZE);
            exit(1);
        }
    }

    // End-of-file escape
end:

    return;
}

//===========================================================================
//=  Function to compare two aruguments in an array of double               =
//=   - Needed by qsort()                                                   =
//===========================================================================
int  comp(const void *p, const void *q)
{
    double *ptr1 = (double *)(p);
    double *ptr2 = (double *)(q);

    if (*ptr1 < *ptr2) return -1;
    else if (*ptr1 == *ptr2) return 0;
    else return 1;
}

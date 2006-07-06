#include <vector>
#include <algorithm>
using namespace std ;



class HistogramGenerator {
    public:
        HistogramGenerator() ;
        ~HistogramGenerator() ;
        double exponetial( double x ) ;

} ;



double
HistogramGenerator::exponential(double x)
{
  double z;                     // Uniform random number (0 < z < 1)
  double exp_value;             // exponential value to be returned

  // Pull a uniform random number (0 < z < 1)
  do
  {
    z = rand_val(0);
  }
  while ((z == 0) || (z == 1));

  // Compute exponential random variable using inversion method
  exp_value = -x * log(z);

  return(exp_value);
}


int
histogram( vector<double> vec, int num_buckets, int bucket_size  )
{
    sort(vec.begin(), vec.end()); // Sort the vector

    // Do for all the buckets
    total_count = 0;
    start = 0;
    for (i=0; i<num_buckets; i++)
    {
        // Scan vec[] for current bucket size
        count = 0;
        for(k=start; k<vec.size(); k++)
        {
            // Count as in this bucket
            if (vec[k] < ((i+1) * (double) bucket_size))
            {
                count++;
                total_count++;
            }

            // For this case need to start couting for next bucket
            if (vec[k] >= ((i+1) * (double) bucket_size))
            {
                start = k;
                break;
            }

            // For this case we are all done
            if (k == (N-1))
            {
                start = N;
                break;
            }
        }

        // Output count and percentage for current bucket size
        printf("  %14.9f <= vec < %14.9f = %6ld -- %f %% \n",
                (i * (double) bucket_size), ((i + 1) * (double) bucket_size),
                count, 100.0 * ((double) count / N));
    }
}




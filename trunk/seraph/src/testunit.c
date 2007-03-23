#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
int maxi( int i1, int i2 )
{
        return ( i1 > i2 ) ? i1 : i2;
}

void test_maxi( void )
{
        CU_ASSERT( maxi( 0, 2 ) == 2 );
        CU_ASSERT( maxi( 0, -2 ) == 0 );
        CU_ASSERT( maxi( 2, 2 ) == 2 );
        CU_ASSERT( maxi( 1, 2 ) == 1 );
}

int init_suite1( void )
{
        return 0;
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int clean_suite1( void )
{
        return 0;
}



int main(  )
{
        CU_pSuite pSuite = NULL;

        /* initialize the CUnit test registry */
        if( CUE_SUCCESS != CU_initialize_registry(  ) )
                return CU_get_error(  );

        /* add a suite to the registry */
        pSuite = CU_add_suite( "Suite_1", init_suite1, clean_suite1 );
        if( NULL == pSuite ) {
                CU_cleanup_registry(  );
                return CU_get_error(  );
        }

        /* add the tests to the suite */
        /* NOTE - ORDER IS IMPORTANT - MUST TEST fread() AFTER fprintf() */
        if( ( NULL == CU_add_test( pSuite, "test of maxi()", test_maxi ) ) ) {
                CU_cleanup_registry(  );
                return CU_get_error(  );
        }

        /* Run all tests using the CUnit Basic interface */
        CU_basic_set_mode( CU_BRM_VERBOSE );
        CU_basic_run_tests(  );
        CU_cleanup_registry(  );
        return CU_get_error(  );
}

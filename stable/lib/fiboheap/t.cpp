#include <cstdio>
#include <vector>

using namespace std ;

typedef struct nod_s {
    int     key ;
    int     rank ;
    int     lost ;
    struct  nod_s*   pred ;
    struct  nod_s*   minkey ;
    vector<struct nod_s*> succ ;

    nod_s() {
        lost = 0 ;
        rank = 0 ;
        pred = 0 ;
    };
} node_t ;

typedef struct heap_s {
    int bucket[16] ={0}; // enough for a heap holding 2048 elements

} heap_t ;


/**
 * Merges the three rooted at i and j, into a single
 * tree.
 * Restore Property 2: no two root nodes have the same
 * rank.
 */
int
link(nod_t* i, nod_t* j)
{
    //
    if( j->key <= i->key )
    {
        i->pred = j //make j the predecessor of i
        j->rank++ ;
        j->succ.push_back(i) ;
        return 0 ;
    }
    j->pred = i ;   //make i the predecessor of j
    i->rank++ ;
    i->succ.push_back(j) ;
    return 0 ;
}


/**
 * Cut node i from its predecesor and makes it a root.
 * Restore Property 1: Each nonroot node has lost at most
 * one successor after becoming a nonroot node .
 */
int
cut(nod_t* i)
{
    vector<struct nod_s*>::iterator it ;
    for( it =i->pred->succ.begin() ;
         it!=i->pred->succ.end() ;
         ++it
       )
    {
        if( it == i )
        {
            i->pred->succ.erase(it) ;
            i->pred->rank--;
            // restore Property 1.
            if( i->pred->pred ) // the node is not root
            {
                i->pred->lost++
                if( 2 == i->pred->lost ) // the node lost 2 succ
                {
                    cut(i->pred) ;
                }
            }
            break ;
        }
    }
    i->pred = 0 ;
    i->lost = 0 ;
    return 0 ;
}

find_min() {}
insert() {}
decrease_key() {}
delete_min() {}


int main()
{
    return 0;
}


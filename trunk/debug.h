#ifndef DEBUG_H
#define DEBUG_H

#include <stdlib.h>
#include <iostream>


#ifndef RING
      #define RING  if( getenv("DEBUG") !=NULL) cout<<__FILE__<<":\t"<<__LINE__<<"\t"<<__PRETTY_FUNCTION__<<endl;
#endif

#endif /* DEBUG_H */

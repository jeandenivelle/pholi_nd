
#include "logic/beliefstate.h"
#include "logic/uniquenamestack.h"

#include "quantifiers.h"
#include "propositional.h"

namespace calc
{

   namespace pretty
   {

      
      template< typename F >
      void print( std::ostream& out,
                  const logic::beliefstate& blfs,
                  logic::pretty::uniquenamestack& names,
                  const disjunction<F> & disj )
      {

      }

      template< typename F > 
      void print( std::ostream& out, 
                  const logic::beliefstate& blfs,
                  logic::pretty::uniquenamestack& names, 
                  const forall<F> & all )
      {
         logic::pretty::print( out, blfs, names,
         [&all]( size_t i ) { return all. vars. at(i); }, all. vars. size( )); 
         print( out, blfs, names, all. body ); 
      }

   }

   
}



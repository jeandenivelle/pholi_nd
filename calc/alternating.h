
#ifndef CALC_ALTERNATING_
#define CALC_ALTERNATING_

#include "logic/term.h"
#include "logic/context.h"

#include "propositional.h"
#include "quantifiers.h"

#include "polarity.h"

namespace calc
{

   template< typename F >
   using anf = conjunction< forall< disjunction< exists<F>>>> ;

   logic::selector kleening( logic::selector sel, polarity pol );


   anf< logic::term > flatten( anf< logic::term > conj );


   void
   flatten( std::vector< logic::vartype > & ctxt,
            polarity pol,
            const logic::term& fm,
            conjunction< forall< logic::term >> & conj );

   void 
   flatten( std::vector< logic::vartype > & ctxt, 
            polarity pol, 
            const logic::term& fm,
            disjunction< exists< logic::term >> & disj );


   void
   flatten_prop( std::vector< logic::vartype > & ctxt,
                 polarity pol,
                 const logic::term& fm,
                 conjunction< forall< logic::term >> & conj );

   void 
   flatten_prop( std::vector< logic::vartype > & ctxt,
                 polarity pol, 
                 const logic::term& fm,
                 disjunction< exists< logic::term >> & disj );

}

#endif


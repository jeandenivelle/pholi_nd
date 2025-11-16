
#ifndef CALC_ALTERNATING_
#define CALC_ALTERNATING_

#include <vector>

#include "logic/term.h"

#include "propositional.h"
#include "quantifiers.h"

namespace calc
{
   logic::selector quantof( logic::selector op );
      // Returns the quantifier that belongs to op.
      // op_kleene_or  -> op_kleene_exists
      // op_kleene_and -> op_kleene_forall

   logic::selector alternation( logic::selector op );
      // Returns the alternation of op.
      // op_kleene_or -> op_kleene_and
      // op_kleene_and -> op_kleene_or


   template< typename F >
   using anf = conjunction< forall< disjunction< exists<F>>>> ;

   anf< std::pair< logic::term, logic::term >>
   pairkleene( const anf< logic::term > & fm );
      // First is original formula, second is topkleene.

   void flatten( anf< std::pair< logic::term, logic::term >> & fm );

   bool isalternating( const logic::term& f,
                       logic::selector op, unsigned int rank );
      // The alternation rank of f. 

#if 0
   logic::term
   restrict_alternation( transformer& trans, logic::beliefstate& blfs,
             logic::context& ctxt, logic::term f,
             logic::selector op, unsigned int maxrank );
      // op must be a Kleene operator. 
#endif

}

#endif



#ifndef CALC_ALTERNATING_
#define CALC_ALTERNATING_

#include "logic/term.h"
#include "logic/context.h"

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

   anf< logic::term > flatten( const anf< logic::term > & conj );


   void flatten( anf< logic::term > & conj,
                 std::vector< logic::vartype > & ctxt,
                 const disjunction< exists< logic::term >> & disj );


   // Flatten only conj and forall:
  
   void flatten( anf< logic::term > & conj,
                 std::vector< logic::vartype > & ctxt,
                 const logic::term& fm );

   
   void flatten( disjunction< exists< logic::term >> & disj,
                 std::vector< logic::vartype > & ctxt, 
                 const logic::term& tm );

   
#if 0
   logic::term
   restrict_alternation( transformer& trans, logic::beliefstate& blfs,
             logic::context& ctxt, logic::term f,
             logic::selector op, unsigned int maxrank );
      // op must be a Kleene operator. 
#endif

}

#endif


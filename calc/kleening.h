
#ifndef CALC_KLEENING_
#define CALC_KLEENING_

#include "polarity.h"

#include "logic/term.h"
#include "logic/context.h"

namespace calc
{

   bool canbe_pi( const logic::term& fm );
   bool canbe_pi_prop( const logic::term& fm );
      // True if main Kleene operator can be nontrivial kleene-forall
      // or conjunction.

   bool canbe_sigma( const logic::term& fm );
   bool canbe_sigma_prop( const logic::term& fm );
      // True if main Kleene operator can be nontrivial kleene-exists or
      // disjunction.
 
 
   logic::selector kleenop( logic::selector op );
      // Get the kleening of op, for a monotone operator.

   logic::term apply( const logic::term& f, polarity pol );
      // If pol is positive, we return f.
      // If pol is negative, we return either not(f),
      // or try to remove a negation from f.

   logic::term apply_prop( const logic::term& f, polarity pol ); 
      // Return prop(f) or not( prop(f)).

   logic::term kleene_top( const logic::term& f, polarity pol );
   logic::term kleene_top_prop( const logic::term& f, polarity pol );

}

#endif


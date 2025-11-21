
#ifndef CALC_KLEENING_
#define CALC_KLEENING_

#include "polarity.h"

#include "logic/term.h"
#include "logic/context.h"

namespace calc
{

   logic::term apply( const logic::term& f, polarity pol );
      // If pol is positive, we return f.
      // If pol is negative, we return either not(f),
      // or try to remove a negation from f.

   logic::term apply_prop( const logic::term& f, polarity pol ); 
      // Return prop(f) or not( prop(f)).


}

#endif


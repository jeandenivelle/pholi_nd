
// Written by Hans de Nivelle, July/August 2025.

#ifndef CALC_PROOFCHECKING_
#define CALC_PROOFCHECKING_

#include <string_view>

#include "errorstack.h"
#include "sequent.h"
#include "proofterm.h"

namespace calc
{
   errorstack::builder
   errorheader( const sequent& seq, std::string_view rule );

   void 
   betapi( const logic::beliefstate& blfs, 
           forall< disjunction< exists< logic::term >>> & tm );

   forall< disjunction< exists< logic::term >>>
   lift( forall< disjunction< exists< logic::term >>> cls, size_t dist );

   void checkproof( const logic::beliefstate& blfs,
                    proofterm& prf, sequent& seq, errorstack& err );
      // In case of failure, we vent our frustration into err, and 
      // return nothing. As with type checking,
      // we may try to recover from these errors, and check
      // other parts of the proof. 
      // The proofterm is not const, because we resolve overloads.
}

#endif


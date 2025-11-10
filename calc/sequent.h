
#ifndef CALC_SEQUENT_
#define CALC_SEQUENT_

#include <map>
#include <vector>
#include <optional>
#include <string>
#include <string_view>

#include "errorstack.h"
#include "indexedstack.h"
#include "logic/beliefstate.h"
#include "logic/context.h"
#include "propositional.h"
#include "quantifiers.h"

namespace calc
{


   struct formula
   {
      std::string name;    // Our name (or empty string if we don't have one).

      std::vector< size_t > hides;
         // Formulas that we are hiding.

      bool hidden; 
         // True if we by ourselves are hidden.

      size_t level;   // Size of context that we depend on.
                      // 0 means that we are not using anything from ctxt.

      forall< disjunction< exists< logic::term >>> form;

      std::string comment;

      formula( size_t level, 
               forall< disjunction< exists< logic::term >>> && form )
         : level( level ),
           form( std::move( form ))
      { }  

      formula( formula&& ) noexcept = default;
      formula& operator = ( formula&& ) noexcept = default;

      void ugly( std::ostream& out ) const;  
   };
   
   
   // Sequent is not a complete class. It is more like a view
   // into a beliefstate.

   struct sequent
   {
      logic::context ctxt;
      indexedstack< std::string, size_t > db;
         // db is needed because we typecheck terms during 
         // proofchecking.

      std::map< size_t, logic::term > defs;
         // If a position in ctxt is a definition,
         // its value is here.

      std::vector< formula > forms;
         // The formulas, see above. 

      std::map< std::string, std::vector< size_t >> names;
         // Quick lookup of names.
      
      sequent( ) noexcept = default;
      sequent( sequent&& ) noexcept = default;
      sequent& operator = ( sequent&& ) noexcept = default;

      size_t nrvars( ) const { return ctxt. size( ); } 
#if 0
      void restore( size_t s ); 
#endif
      void ugly( std::ostream& out ) const;      
#if 0
      void pretty( std::ostream& out, bool showblocked = false ) const;
#endif

      // The number returned can be used for restoring the context:

      size_t assume( const std::string& name, const logic::type& tp );
      size_t define( const std::string& name, const logic::term& val,
                     const logic::type& tp );

      void assume( forall< disjunction< exists< logic::term >>> && form );
   };

   inline std::ostream& operator << ( std::ostream& out, const sequent& seq )
   {
      // seq. pretty( out );
      return out;
   }
}

#endif



// Written by Hans de Nivelle, November 2025.

#ifndef CALC_SEQUENT_
#define CALC_SEQUENT_

#include <map>
#include <vector>
#include <string>

#include "errorstack.h"
#include "indexedstack.h"
#include "logic/beliefstate.h"
#include "logic/context.h"
#include "propositional.h"
#include "quantifiers.h"

namespace calc
{
 
   // Sequent is not a complete class. It is more like a view
   // into a beliefstate.

   struct sequent
   {

      // An assumption level. We don't give names to
      // individual formulas, only to assumption levels.

      struct level
      {
         std::string name;   

         std::vector< forall< disjunction< exists< logic::term >>>> rpn; 
         size_t contextsize;   
            // When level is created.

         level( std::string name, size_t contextsize )
            : name( std::move( name )), contextsize( contextsize )
         { }

      };

       
      logic::context ctxt;
      indexedstack< std::string, size_t > db;
         // db is needed because we typecheck terms during 
         // proofchecking. 'db' stands for De Bruijn. 

      std::map< size_t, logic::term > defs;
         // If a position in ctxt is a definition, its value is here.

      std::vector< level > lev;
         // Stack of stacks of derived formulas. 

      sequent( ) noexcept { } 
      sequent( sequent&& ) noexcept = default;
      sequent& operator = ( sequent&& ) noexcept = default;

      size_t contextsize( ) const { return ctxt. size( ); } 
      size_t nrlevels( ) const { return lev. size( ); }

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

      void addlevel( std::string name );
      void poplevel( );

      const level& lastlevel( ) const { return lev. back( ); }
      level& lastlevel( ) { return lev. back( ); }

      void push( forall< disjunction< exists< logic::term >>> form );
      void pop( );

      forall< disjunction< exists< logic::term >>> & 
      get( size_t ind );

      const forall< disjunction< exists< logic::term >>> & 
      get( size_t ind ) const;

   };

   inline std::ostream& operator << ( std::ostream& out, const sequent& seq )
   {
      throw std::runtime_error( "<< not implemented" );
      seq. ugly( out );
      return out;
   }
}

#endif


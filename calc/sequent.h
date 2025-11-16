
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

      struct lemma 
      {
         forall< disjunction< exists< logic::term >>> cls;
         size_t nrvars;   // In context.
      };

      struct level
      {
         std::vector< forall< disjunction< exists< logic::term >>>> rpn; 
         size_t nrvars;   // In context.

         std::vector< std::string > names; 
            // So that they can be backtracked.

         level( size_t nrvars )
            : nrvars( nrvars )
         { }

      };

       
      logic::context ctxt;
      indexedstack< std::string, size_t > db;
         // db is needed because we typecheck terms during 
         // proofchecking. 'db' stands for De Bruijn. 

      std::map< size_t, logic::term > defs;
         // If a position in ctxt is a definition,
         // its value is here.

      std::vector< level > lev;
         // Stack of stacks of derived formulas. 

      std::map< std::string, std::vector< lemma >> lemmas;
         // Last one is the current one.

      sequent( ) noexcept { } 
      sequent( sequent&& ) noexcept = default;
      sequent& operator = ( sequent&& ) noexcept = default;

      size_t nrvars( ) const { return ctxt. size( ); } 
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

      void addlevel( );
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


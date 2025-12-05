
#ifndef CALC_SIMPLIFIER_
#define CALC_SIMPLIFIER_

#include <iostream>

#include "logic/term.h"
#include "propositional.h"
#include "quantifiers.h"
#include "prefix.h"

namespace calc
{

   struct simplifier
   {
      using clause = disjunction< exists< logic::term >> ;

      conjunction< clause > cnf;
         // We accept existentially quantified terms, because one
         // could have simplifications of form
         // !A + exists( x1, ... xn, F ),  A => exists( x1, ... xn, F ).

      simplifier( ) noexcept = default;
      simplifier( simplifier&& ) = default;
      simplifier& operator = ( simplifier&& ) = default;
 
      uint64_t res_simplify( );
         // Do a resolution simplification.
         // We look for pairs A1 \/ R1,  A2 \/ R2 where
         // A1,A2 are in conflict, and R1 subsumes R2.
         // In that case, we remove A2. 
 
      uint64_t eq_simplify( ); 
         // Do a paramodulation simplification.
         // We look for pairs t1 == t2 \/ R1, A2[t1] \/ R2,
         // where R1 subsumes R2. In that case,
         // we replace t1 by t2. 
         // We use KBO for directing the equality, so it can be in the
         // other direction too.

      uint64_t fully_simplify( );
         // Keep on calling res_simplify and eq_simplify until no
         // more simplifications are possible.

      void remove_repeated( ); 
         // Remove repeated literals, and obviously false literals.

      void remove_redundant( );
         // Remove disjunctions subsumed by disj. 
      // size_t size( ) const { return set. size( ); } 
  
      void print( std::ostream& out ) const;
   };

   std::pair< prefix, const logic::term* > 
   decompose( const logic::term& tm );
      // Assumes ANF.

   bool inconflict( const logic::term& tm1, const logic::term& tm2 );
   bool subsumes( const logic::term& tm1, const logic::term& tm2 );

   bool subsumes( const exists< logic::term > & ex1,
                  const exists< logic::term > & ex2 );
      // Very incomplete!  
   
   bool trivially_true( const logic::term& tm );
      // Catches a few trivial cases:
      //    TRUE, ( t = t ), # t1 = t2, ## t, and # FALSE, # TRUE.
               
   void simplify( disjunction< exists< logic::term >> & cls );
      // Remove redundant literals, and direct equalities using
      // KBO.
  
   bool 
   subsumes( const simplifier::clause & cls1,
             simplifier::clause::const_iterator skip1,
             const simplifier::clause & cls2,
             simplifier::clause::const_iterator skip2 );

      // True if cls1 \ skip1 is a subset of cls2 \ skip2.
      // If you want the full clause, use end( ).

   // True if it happened:
 
   bool 
   rewrite( const simplifier::clause & from, simplifier::clause & into );

   bool
   resolve( const simplifier::clause & from, simplifier::clause & into );

}

#endif



#ifndef CALC_SIMPLIFIER_
#define CALC_SIMPLIFIER_

#include <iostream>

#include "logic/term.h"
#include "propositional.h"
#include "quantifiers.h"
#include "polarity.h"

namespace calc
{

   struct simplifier
   {
      class truth
      {
         uint8_t val;

         explicit truth( uint8_t val ) 
            : val( val )
         { }

      public:
         static truth F( ) { return truth(4); }
         static truth E( ) { return truth(2); }
         static truth T( ) { return truth(1); }

         bool subset( truth t ) const
            { return ! ( val & ~t. val ); }

         truth& operator |= ( const truth& t ) 
            { val |= t. val; return *this; }

         truth& operator &= ( const truth& t )
            { val &= t. val; return *this; }
        
         void print( std::ostream& out ) const;
      };


      conjunction< disjunction< exists< logic::term >>> cnf;
         // We accept existentially quantified terms, because one
         // could have simplifications of form
         // !A + exists( x1, ... xn, F ),  A => exists( x1, ... xn, F ).

      simplifier( ) noexcept = default;
 
      uint64_t res_simplify( );
         // Do a resolution simplification.
         // We look for pairs A1 \/ R1,  A2 \/ R2 where
         // A1,A2 are in conflict, and R1 is a subset of R2.
         // In that case, we remove A2. 
 
      uint64_t eq_simplify( ); 
         // Do a paramodulation simplification.
         // We look for pairs t1 == t2 \/ R1, A2[t1] \/ R2,
         // where R1 is a subset of R2. In that case,
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

   inline simplifier::truth 
   operator & ( simplifier::truth t1, simplifier::truth t2 ) 
      { return t1 &= t2; }

   inline simplifier::truth  
   operator | ( simplifier::truth t1, simplifier::truth t2 ) 
      { return t1 |= t2; }
 
   bool inconflict( polarity pol1, const logic::term& tm1,
                    polarity pol2, const logic::term& tm2 );

#if 0
   bool inconflict( const logic::term& tm1, const logic::term& tm2 );

   bool contains( const logic::term& lit, const clause& cls, 
                  clause::const_iterator skip );

   bool subset( const clause& cls1, 
                clause::const_iterator skip1,
                const clause& cls2, 
                   clause::const_iterator skip2 );
      // True if cls1 \ skip1 is a subset of cls2 \ skip2.

   bool certainly( short int pol, const logic::term& tm );
      // If pol > 0, certainly true.
      // If pol < 0, certainly false.

   void remove_repeated( clause& cls );
         // Remove repeated literals 
         // and literals that are certainly false. 

   bool istautology( const clause& cls );
      // True if we contain a literal that is certainly true. 
      // I do not expect that tautologies will occur in meaningful proofs.

   logic::term disjunction( const clause& cls );
#endif

}


#endif


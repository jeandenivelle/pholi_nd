
// Written by Hans de Nivelle, November 2025.

#ifndef CALC_OUTERMOST_
#define CALC_OUTERMOST_

#include "logic/outermost.h"
#include "propositional.h"
#include "quantifiers.h"

namespace calc
{

#if 0
   template< logic::replacement R > 
   exists< logic::term > 
   outermost( const R& repl, existential< logic::term > ex, size_t vardepth )
   {
      if( fm. has_value( ))
         fm. value( ) = logic::outermost( r, std::move( fm. value( )), 0 ); 
   }
#endif

   template< logic::replacement R, typename F > 
   exists<F> 
   outermost( R& repl, exists<F> ex, size_t vardepth )
   {
      ex. body = outermost( repl, std::move( ex. body ),
                            vardepth + ex. vars. size( )); 
      return ex;
   }

   template< logic::replacement R, typename F >
   disjunction<F> outermost( R& repl, disjunction<F> disj, size_t vardepth )
   {
      for( auto& f : disj )
         f = outermost( repl, std::move(f), vardepth );
      return disj;
   }

   template< logic::replacement R, typename F > 
   forall<F> outermost( R& repl, forall<F> all, size_t vardepth )
   {
      all. body = outermost( repl, std::move( all. body ), 
                             vardepth + all. vars. size( ));
      return all;
   }
 
}

#endif


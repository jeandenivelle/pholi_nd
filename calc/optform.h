
#ifndef CALC_OPTFORM_
#define CALC_OPTFORM_

#include <optional>
#include <string_view>
#include <iostream>

#include "sequent.h"
#include "errorstack.h"
#include "logic/outermost.h"

// An optional formula. 

namespace calc
{
   // Shall I make this the main data structure returned by the
   // proof checker?

   struct optform
   {
      std::optional< logic::term > fm;

      std::string_view rule; 
      sequent& seq;
      errorstack& err; 

      optform( const std::optional< logic::term > & fm,
               std::string_view rule,
               sequent& seq, errorstack& err )
         : fm( fm ),
           rule( rule ),
           seq( seq ), err( err ) 
      { }

      optform( std::optional< logic::term > && fm,
               std::string_view rule,
               sequent& seq, errorstack& err )
         : fm( std::move( fm )),
           rule( rule ),
           seq( seq ), err( err )
      { } 
           
      void musthave( logic::selector op ); 
      void replacebysub( size_t ind );
      void getuniquesub( );
      void aritymustbe( size_t i );  // Works for Kleene operators only.
      void nrvarsmustbe( size_t i ); // Works for quantifiers only.
         // Both are exact.

      void fake( );  // Generate a message that fm was faked.
                     // Don't reset, so that we can return it.

      operator bool( ) const { return fm. has_value( ); }
      const logic::term& value( ) const { return fm. value( ); }
      logic::term& value( ) { return fm. value( ); }

      void print( std::ostream& out ) const;
      void pretty( std::ostream& out ) const;
   };
}

#endif



#ifndef CALC_QUANTIFIERS_
#define CALC_QUANTIFIERS_

#include <iostream>
#include <vector>

namespace calc
{

   // We are only a dumb data structure.
   // We have no algorithms.

   template< typename F > struct forall
   {
      std::vector< logic::vartype > vars;
      F body; 

      forall( ) noexcept = default;
      forall( F&& body ) noexcept : body( std::move( body )) { }
      forall( const F& body ) : body( body ) { }

      size_t nrvars( ) const { return vars. size( ); }

      void print( std::ostream& out ) const
      {
         out << "forall(";
         for( auto p = vars. begin( ); p != vars. end( ); ++ p )
         {
            if( p != vars. begin( ))
               out << ", ";
            else
               out << ' ';
            out << *p;
         }
         out << " ): " << body;
      } 
   };


   template< typename F > struct exists
   {
      std::vector< logic::vartype > vars;
      F body;

      exists( ) noexcept = default;
      exists( F&& body ) noexcept : body( std::move( body )) { }
      explicit exists( const F& body ) : body( body ) { }

      size_t nrvars( ) const { return vars. size( ); }

      void print( std::ostream& out ) const
      {
         out << "exists(";
         for( auto p = vars. begin( ); p != vars. end( ); ++ p )
         {
            if( p != vars. begin( ))
               out << ", ";
            else
               out << ' ';
            out << *p;
         }
         out << " ): " << body;
      }
   };

}

#endif


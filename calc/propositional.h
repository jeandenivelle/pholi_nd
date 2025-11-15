
#ifndef CALC_PROPOSITIONAL_
#define CALC_PROPOSITIONAL_

#include <iostream>

namespace calc
{

   
   template< typename F > struct disjunction
   {
      std::vector<F> disj;

      disjunction( ) noexcept = default;
      disjunction( std::initializer_list<F> init ) 
         : disj( init ) 
      { }

      void append( F&& f )
         { disj. push_back( std::move(f)); }
      
      void append( const F& f )
         { disj. push_back(f); } 
 
      using iterator = std::vector<F>  :: iterator;
      using const_iterator = std::vector<F> :: const_iterator;

      iterator begin( ) { return disj. begin( ); }
      const_iterator begin( ) const { return disj. begin( ); }

      iterator end( ) { return disj. end( ); }
      const_iterator end( ) const { return disj. end( ); }

      size_t size( ) const { return disj. size( ); }
      const F& at( size_t i ) const { return disj. at(i); }

      void print( std::ostream& out ) const
      {
         out << '{';
         for( auto p = disj. begin( ); p != disj. end( ); ++ p )
         {
            if( p != disj. begin( ))
               out << "; ";
            else 
               out << ' ';
            out << *p; 
         }
         out << " }";
      }

   };


   template< typename F > struct conjunction
   {
      std::vector<F> conj;

      conjunction( ) noexcept = default;

      void append( F&& f )
         { conj. push_back( std::move(f)); }

      void append( const F& f )
         { conj. push_back(f); }

      using iterator = std::vector<F>  :: iterator;
      using const_iterator = std::vector<F> :: const_iterator;

      iterator begin( ) { return conj. begin( ); }
      const_iterator begin( ) const { return conj. begin( ); }

      iterator end( ) { return conj. end( ); }
      const_iterator end( ) const { return conj. end( ); }

      size_t size( ) const { return conj. size( ); }
      const F& at( size_t i ) const { return conj. at(i); }

      void print( std::ostream& out ) const
      {
         out << '{';
         for( auto p = conj. begin( ); p != conj. end( ); ++ p )
         {
            if( p != conj. begin( ))
               out << "; ";
            else
               out << ' ';
            out << *p;
         }
         out << " }";
      }

   };

}

#endif


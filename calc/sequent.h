
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
 
   struct sequent
   {

      // An assumption level. We don't give names to
      // individual formulas, only to assumption levels.

      struct level
      {
         std::string name;   

         std::vector< forall< disjunction< exists< logic::term >>>> stack; 
         size_t contextsize;   
            // At the moment when the level is created.

         level( std::string name, size_t contextsize )
            : name( std::move( name )), contextsize( contextsize )
         { }

         void push( forall< disjunction< exists< logic::term >>> form )
            { stack. push_back( std::move( form )); }

         // We use Python style indexing. That means that -1 is the last
         // element.

         const forall< disjunction< exists< logic::term >>> & 
         at( ssize_t ind ) const; 

         forall< disjunction< exists< logic::term >>> &
            at( ssize_t ind );

         void erase( ssize_t ind );

         using iterator = 
         std::vector< forall< disjunction< exists< logic::term >>>> 
                 :: iterator;

         using const_iterator =
         std::vector< forall< disjunction< exists< logic::term >>>>
                 :: const_iterator;

         iterator begin( ) { return stack. begin( ); }
         iterator end( ) { return stack. end( ); }
            // Be careful with those, because the order is from
            // bottom of stack to top of stack. They are useful
            // for copying or processing.

         void clear( ) 
            { stack. clear( ); }    
         // Forget about everything. Used to think that it was so easy.

         size_t size( ) const { return stack. size( ); } 

         bool inrange( ssize_t ind ) const;
            // True if ind can be used as an index.

         iterator find( ssize_t ind );
         const_iterator find( ssize_t ind ) const; 
      };

       
      logic::context ctxt;
      indexedstack< std::string, size_t > db;
         // db is needed because we typecheck terms during 
         // proofchecking. 'db' stands for De Bruijn. 
         // size_t looks from the beginning!

      std::map< size_t, logic::term > defs;
         // If a position in ctxt is a definition, its value is here.

      std::vector< level > lev;
         // Stack of stacks of derived formulas. 

      sequent( ) noexcept { } 
      sequent( sequent&& ) noexcept = default;
      sequent& operator = ( sequent&& ) noexcept = default;

      size_t size( ) const { return lev. size( ); }

      void ugly( std::ostream& out ) const;      
#if 0
      void pretty( std::ostream& out, bool showblocked = false ) const;
#endif

      // The number returned can be used for restoring the context:

      size_t assume( const std::string& name, const logic::type& tp );

      size_t define( const std::string& name, const logic::term& val,
                     const logic::type& tp );

      void push_back( std::string name );
      void pop_back( );
         // Add or remove a level.

      const level& back( ) const;
      level& back( );

      const level& at( size_t ind ) const { return lev. at( ind ); }
      level& at( size_t ind ) { return lev. at( ind ); }
   };


   inline std::ostream& operator << ( std::ostream& out, const sequent& seq )
   {
      throw std::runtime_error( "<< not implemented" );
      seq. ugly( out );
      return out;
   }
}

#endif


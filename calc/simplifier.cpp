
#include "simplifier.h"
#include "logic/cmp.h"
#include "logic/replacements.h"


std::pair< calc::prefix, const logic::term* > 
calc::decompose( const logic::term& tm )
{
   if( tm. sel( ) == logic::op_prop )
   {
      return std::pair( prefix::F( ) | prefix::T( ), 
                        & tm. view_unary( ). sub( ) );   
   }

   if( tm. sel( ) == logic::op_not )
   {
      const auto& sub = tm. view_unary( ). sub( );
      if( sub. sel( ) == logic::op_prop )
         return std::pair( prefix::E( ), & sub. view_unary( ). sub( ));
      else
         return std::pair( prefix::F( ), & sub );
   }

   return std::pair( prefix::T( ), & tm );
}


#if 0

uint64_t calc::clauseset::res_simplify( )
{
   uint64_t counter = 0;

   for( auto from = set. begin( ); from != set. end( ); ++ from )
   {
      for( auto into = set. begin( ); into != set. end( ); ++ into ) 
      {
         if( into != from )
         {
            for( auto p = from -> begin( ); p != from -> end( ); ++ p )
            {
               auto in = into -> begin( ); 
               while( in != into -> end( ))
               {
                  if( inconflict( *p, *in ) && subset( *from, p, *into, in )) 
                  {
                     std::cout << *p << " conflicts " << *in << "\n";
                     in = into -> erase( in );
                     ++ counter; 
                  }
                  else 
                     ++ in;
               }
            }
         } 
      }
   }

   return counter;
}


uint64_t calc::clauseset::eq_simplify( )
{
   uint64_t counter = 0;

   for( auto from = set. begin( ); from != set. end( ); ++ from )
   {
      for( auto into = set. begin( ); into != set. end( ); ++ into )
      {
         if( into != from )
         {
            for( auto eq = from -> begin( ); eq != from -> end( ); ++ eq )
            {
               if( eq -> sel( ) == logic::op_equals )
               {
                  auto rewr = logic::rewriterule( 
                     eq -> view_binary( ). sub1( ), 
                     eq -> view_binary( ). sub2( ));
                  
                  std::cout << rewr << "\n";
                  auto cmp = kbo( rewr. from, rewr. to );

                  if( !is_eq( cmp ))
                  {
                     if( is_lt( cmp ))
                        rewr. swap( );
                     std::cout << "directed: " << rewr << "\n";

                     for( auto in = into -> begin( ); 
                          in != into -> end( ); ++ in ) 
                     {
                        // We first check for subset.
                        // If yes, we try to rewrite, and count
                        // how often it happened.

                        if( subset( *from, eq, *into, in ))
                        {
                           rewr. counter = 0;
                           *in = outermost( rewr, *in, 0 );
                           counter += rewr. counter; 
                        }
                     }
                  }
               }
            }
         }
      }
   }

   return counter;
}

uint64_t 
calc::clauseset::fully_simplify( )
{
   remove_repeated( );
   remove_redundant( );
   uint64_t counter = 0;
restart:
   auto s = res_simplify( ) + eq_simplify( );
   if(s) 
   {
      counter += s;
      goto restart;
   }
   remove_repeated( );
   remove_redundant( );

   return counter;
}

void calc::clauseset::remove_repeated( )
{
   for( auto& cl : set )
      calc::remove_repeated( cl ); 
}

void calc::clauseset::remove_redundant( )
{
   // First remove tautologies:

   auto it = set. begin( );
   while( it != set. end( ))
   {
      if( istautology( *it ))
         it = set. erase( it );
      else
         ++ it;
   }

   // Then subsumed:

   for( auto it = set. begin( ); it != set. end( ); ++ it )
   {
      auto p = set. begin( );
      while( p != set. end( ))
      {
         if( p != it && subset( *it, it -> end( ), *p, p -> end( )))
            p = set. erase(p);
         else
            ++ p;
      }
   }
}


logic::term
calc::clauseset::getformula( ) const
{
   auto res = logic::term( logic::op_kleene_and,
                           std::initializer_list< logic::term > ( ));
   auto kl = res. view_kleene( );
   for( const auto& cl : set )
      kl. push_back( disjunction( cl )); 
   return res;
}

#endif

void
calc::simplifier::print( std::ostream& out ) const
{
   out << "Simplifier:\n";
   for( const auto& disj : cnf )
      out << "   " << disj << "\n";
   out << "\n";
}


bool
calc::inconflict( const logic::term& tm1, const logic::term& tm2 )
{
   std::cout << "inconflict ";
   std::cout << tm1 << "   " << tm2 << "\n";

   auto dec1 = decompose( tm1 );
   auto dec2 = decompose( tm2 );

   if( ( dec1. first & dec2. first ). isempty( ))
   {
      if( logic::equal( *dec1. second, *dec2. second ))
         return true; 
   }

   return false;
}


bool
calc::subsumes( const logic::term& tm1, const logic::term& tm2 )
{
   std::cout << "subsumes ";
   std::cout << tm1 << "   " << tm2 << "\n";

   auto dec1 = decompose( tm1 );
   auto dec2 = decompose( tm2 );

   if( dec1. first. subset( dec2. first ))
   {
      if( logic::equal( *dec1. second, *dec2. second ))
         return true;
   }

   return false;
}


bool
calc::subsumes( const exists< logic::term > & ex1, 
                const exists< logic::term > & ex2 )
{
   std::cout << "subsumes ";
   std::cout << ex1 << "   " << ex2 << "\n";

   if( ex1. vars. size( ) != ex2. vars. size( ))
      return false;

   for( size_t i = 0; i != ex1. vars. size( ); ++ i )
   {
      if( !equal( ex1. vars[i]. tp, ex2. vars[i]. tp ))
         return false;
   }

   return subsumes( ex1. body, ex2. body );
}


void calc::simplify( disjunction< exists< logic::term >> & cls )
{
   auto copy = disjunction< exists< logic::term >> ( );
   



}

#if 0
#if 0

bool 
calc::contains( const logic::term& lit, 
                const clause& cls, clause::const_iterator skip )
{
   for( auto q = cls. begin( ); q != cls. end( ); ++ q )
   {
      if( q != skip && equal( lit, *q ))
         return true;
   } 
   
   return false;
}

bool 
calc::subset( const clause& cls1, clause::const_iterator skip1,
              const clause& cls2, clause::const_iterator skip2 )
{
   for( auto p1 = cls1. begin( ); p1 != cls1. end( ); ++ p1 )
   {
      if( p1 != skip1 && !contains( *p1, cls2, skip2 ))
         return false;
   }
   return true; 
}

bool
calc::certainly( short int pol, const logic::term& tm )
{
   if( tm. sel( ) == logic::op_not )
   {
      auto un = tm. view_unary( );
      return certainly( -pol, un. sub( ));
   }

   if( pol > 0 && tm. sel( ) == logic::op_prop )
   {
      auto un = tm. view_unary( ); 
      if( un. sub( ). sel( ) == logic::op_equals ||
          un. sub( ). sel( ) == logic::op_prop )
      {
         return true;
      }
   }

   if( pol > 0 && tm. sel( ) == logic::op_equals ) 
   {
      auto eq = tm. view_binary( );
      if( equal( eq. sub1( ), eq. sub2( )))
         return true;
   }

   return false;
}

void calc::remove_repeated( clause& cls ) 
{
   auto it = cls. begin( );
   while( it != cls. end( ))
   {
      bool skip = false;

      if( certainly( -1, *it ))
         skip = true;

      for( auto p = cls. begin( ); p != it && !skip; ++ p )
      {
         if( equal( *p, *it ))
            skip = true;
      }

      if( skip )
         it = cls. erase( it );
      else
         ++ it;
   }
}

bool
calc::istautology( const clause& cls )
{
   for( const auto& lit : cls ) 
   {
      if( certainly( 1, lit ))
         return true;
   }
   return false;
}

logic::term
calc::disjunction( const clause& cls )
{
   logic::term res = logic::term( logic::op_kleene_or, 
                                  std::initializer_list< logic::term > ( )); 
   auto kl = res. view_kleene( );
   for( const auto& lit : cls )
      kl. push_back( lit ); 
   return res;
}
#endif

#endif


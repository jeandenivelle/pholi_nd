
#include "simplifier.h"
#include "outermost.h"

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
   std::cout << tm1 << "   " << tm2 << " ?\n";

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


bool calc::trivially_true( const logic::term& tm )
{
   if( tm. sel( ) == logic::op_true )
      return true;

   if( tm. sel( ) == logic::op_equals )
   {
      auto bin = tm. view_binary( );
      if( equal( bin. sub1( ), bin. sub2( )))
         return true;
   }

   if( tm. sel( ) == logic::op_prop )
   {
      const auto& sub = tm. view_unary( ). sub( ); 
      
      if( sub. sel( ) == logic::op_equals ||
          sub. sel( ) == logic::op_prop ||
          sub. sel( ) == logic::op_false ||
          sub. sel( ) == logic::op_true )
      {
         return true;
      }
   }

   return false;
}


void calc::simplify( disjunction< exists< logic::term >> & cls )
{
   auto into = cls. begin( ); 
   while( into != cls. end( ))
   {
      if( into -> vars. size( ) == 0 && trivially_true( into -> body ))
      {
         cls = disjunction( { exists( logic::term( logic::op_true )) } );
         return;
      }

      if( into -> body. sel( ) == logic::op_not )
      {
         // We allow variables, because exists ... FALSE 
         // is still FALSE.

         const auto& sub = into -> body. view_unary( ). sub( );
         if( trivially_true( sub ))
         {
            into = cls. erase( into );
            goto skipped;
         }
      }

      if( into -> body. sel( ) == logic::op_equals )
      {
         // Check if equality must be flipped:

         auto bin = into -> body. view_binary( );
         if( is_lt( logic::kbo( bin. sub1( ), bin. sub2( ))) )
         {
            into -> body = logic::term( logic::op_equals,
                            bin. sub2( ), bin. sub1( ));
         } 
      }
 
      for( auto from = cls. begin( ); from != cls. end( ); ++ from )
      {
         if( from != into && subsumes( *from, *into ))
         {
            into = cls. erase( into ); 
            goto skipped;
         }
      }

      ++ into;

   skipped:
      ;
   }
}


bool 
calc::subsumes( const exists< logic::term > & lit,
                const simplifier::clause & cls,
                simplifier::clause::const_iterator skip )
{
   for( auto q = cls. begin( ); q != cls. end( ); ++ q )
   {
      if( q != skip && subsumes( lit, *q ))
         return true;
   } 
   
   return false;
}

bool 
calc::subsumes( const simplifier::clause& cls1, 
                simplifier::clause::const_iterator skip1,
                const simplifier::clause& cls2, 
                simplifier::clause::const_iterator skip2 )
{
   for( auto p1 = cls1. begin( ); p1 != cls1. end( ); ++ p1 )
   {
      if( p1 != skip1 && !subsumes( *p1, cls2, skip2 ))
         return false;
   }
   return true; 
}

bool 
calc::resolve( const simplifier::clause& from,
               simplifier::clause& into )
{
   for( auto p = from. begin( ); p != from. end( ); ++ p )
      for( auto q = into. begin( ); q != into. end( ); ++ q )
      { 
         if( p -> vars. size( ) == 0 && 
             q -> vars. size( ) == 0 &&
             inconflict( p -> body, q -> body ))
         {
            std::cout << "in conflict " << *p << " " << *q << "\n";
            if( subsumes( from, p, into, q )) 
            {
               std::cout << "will simplify\n";
               into. erase( q ); 
               return true; 
            }
         }
      }

   return false;
}


bool 
calc::rewrite( const simplifier::clause& from,
               simplifier::clause& into )
{
   for( auto p = from. begin( ); p != from. end( ); ++ p )
   {
      if( p -> vars. size( ) == 0 &&
          p -> body. sel( ) == logic::op_equals )
      {
         auto bin = p -> body. view_binary( );
         auto rewr = logic::rewriterule( bin. sub1( ), bin. sub2( ));
         std::cout << rewr << "\n";

         for( auto q = into. begin( ); q != into. end( ); ++ q )
         { 
            *q = outermost( rewr, std::move(*q), 0 );
            if( rewr. counter )
            {
               std::cout << "rewrote " << *q << "\n";
               if( subsumes( from, p, into, q ))
               {
                  std::cout << "will simplify\n";
                  return true; 
               }
            }
         }
      }
   }

   return false;
}


#if 0

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


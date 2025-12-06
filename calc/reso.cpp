
#include "reso.h"
#include "outermost.h"

#include "logic/cmp.h"
#include "logic/replacements.h"


std::pair< calc::prefix, const logic::term* > 
calc::reso::decompose( const logic::term& tm )
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


#endif


bool
calc::reso::inconflict( const logic::term& tm1, const logic::term& tm2 )
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
calc::reso::subsumes( const logic::term& tm1, const logic::term& tm2 )
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
calc::reso::subsumes( const literal& lit1, const literal& lit2 )
{
   std::cout << "subsumes ";
   std::cout << lit1 << "   " << lit2 << "\n";

   if( lit1. vars. size( ) != lit2. vars. size( ))
      return false;

   for( size_t i = 0; i != lit1. vars. size( ); ++ i )
   {
      if( !equal( lit1. vars[i]. tp, lit2. vars[i]. tp ))
         return false;
   }

   return subsumes( lit1. body, lit2. body );
}


bool calc::reso::trivially_true( const logic::term& tm )
{
   // Truth constant:

   if( tm. sel( ) == logic::op_true )
      return true;

   // equality t == t:

   if( tm. sel( ) == logic::op_equals )
   {
      auto bin = tm. view_binary( );
      if( equal( bin. sub1( ), bin. sub2( )))
         return true;
   }

   // # ( t1 == t2 ), # # t, # FALSE, # TRUE:

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


void calc::reso::simplify( clause& cls )
{
   auto p1 = cls. begin( );
   auto p2 = cls. begin( );
      // We copy from p1 to p2:

   while( p1 != cls. end( ))
   {
      std::cout << "looking at " << *p1 << "\n";

      // If *p1 is trivially true, we replace the clause by 
      // { TRUE }:

      if( p1 -> vars. size( ) == 0 && trivially_true( p1 -> body ))
      {
         *cls. begin( ) = exists( logic::term( logic::op_true ));
         while( cls. size( ) > 1 )
            cls. remove_last( );
         return;
      }

      if( p1 -> body. sel( ) == logic::op_not )
      {
         // We allow variables, because exists( x1, ... xn; FALSE )
         // is still FALSE.

         const auto& sub = p1 -> body. view_unary( ). sub( );
         if( trivially_true( sub ))
            goto skip;
      }

      // Check if a later literal subsumes:

      for( auto s = p1 + 1; s != cls. end( ); ++ s )
      {
         if( subsumes( *p1, *s ))
            goto skip;
      }

      // Check if an earlier literal subsumes:

      for( auto s = cls. begin( ); s != p2; ++ s )
      {
         if( subsumes( *p1, *s ))
            goto skip;
      }

      std::cout << "keeping " << *p1 << "\n";

      // Check if *p1 is an equality that needs to be flipped.
      // We also flip inside exists, even when it is useless:

      if( p1 -> body. sel( ) == logic::op_equals )
      {
         auto bin = p1 -> body. view_binary( );
         if( is_lt( logic::kbo( bin. sub1( ), bin. sub2( ))) )
         {
            p1 -> body = logic::term( logic::op_equals,
                                      bin. sub2( ), bin. sub1( ));
         } 
      }

      if( p1 != p2 )
         *p2 = std::move( *p1 ); 
      ++ p2;

   skip:
      ++ p1; 
   }

   while( p2 != cls. end( ))
      cls. remove_last( );
}


bool 
calc::reso::subsumes( const literal& lit, const clause& cls,
                      clause::const_iterator skip )
{
   for( auto q = cls. begin( ); q != cls. end( ); ++ q )
   {
      if( q != skip && subsumes( lit, *q ))
         return true;
   } 
   
   return false;
}

bool 
calc::reso::subsumes( const clause& cls1, clause::const_iterator skip1,
                      const clause& cls2, clause::const_iterator skip2 )
{
   for( auto p1 = cls1. begin( ); p1 != cls1. end( ); ++ p1 )
   {
      if( p1 != skip1 && !subsumes( *p1, cls2, skip2 ))
         return false;
   }
   return true; 
}

bool 
calc::reso::resolve( const clause& from, clause& into )
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
calc::reso::rewrite( const clause& from, clause& into )
{
   for( auto p = from. begin( ); p != from. end( ); ++ p )
   {
      // exists( V, t1 == t2 ) can be used only when V is empty:

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
                  return true; 
               }
            }
         }
      }
   }

   return false;
}


void calc::reso::simplify( conjunction< clause > & simp )
{
   for( auto& cls : simp )
      simplify( cls );

   bool fixedpoint = false;
   while( !fixedpoint )
   {
      fixedpoint = true;
      for( auto from = simp. cbegin( ); from != simp. cend( ); ++ from )
      {
         for( auto into = simp. begin( ); into != simp. end( ); ++ into )
         {
            if( from != into )
            {
               if( rewrite( *from, *into ))
               { 
                  simplify( *into );
                  std::cout << "rewrote " << *into << "\n";
                  fixedpoint = false;
               }

               if( resolve( *from, *into ))
               {
                  simplify( *into );
                  std::cout << "resolved " << *into << "\n";
                  fixedpoint = false;
               }
            }
         }
      }
   }

}



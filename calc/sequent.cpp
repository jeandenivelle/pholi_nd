
#include "sequent.h"
#include "logic/pretty.h"


auto calc::sequent::level::get( size_t ind ) const 
   -> const forall< disjunction< exists< logic::term >>> & 
{
   throw std::logic_error( "const is used" );
   if( ind >= stack. size( ))
      throw std::logic_error( "sequent::level::at: index too high" ); 

   return stack. at( stack. size( ) - ind - 1 );
}

auto calc::sequent::level::get( size_t ind ) 
   -> forall< disjunction< exists< logic::term >>> &
{
   if( ind >= stack. size( ))
      throw std::logic_error( "sequent::level::at: index too high" );

   return stack. at( stack. size( ) - ind - 1 );
}


void calc::sequent::level::pop( )
{
   if( stack. size( ) == 0 )
      throw std::logic_error( "pop: no formula to pop" );

   stack. pop_back( );
}

size_t
calc::sequent::assume( const std::string& name,
                       const logic::type& tp )
{
   size_t nr = ctxt. size( );
   ctxt. append( name, tp );
   db. push( name, nr );
   return nr;
}

size_t 
calc::sequent::define( const std::string& name,
                       const logic::term& val, 
                       const logic::type& tp )
{
   size_t nr = assume( name, tp );
   defs. insert( std::pair( nr, val ));
   return nr;
}

void calc::sequent::push_back( std::string name )
{
   lev. push_back( level( std::move( name ),  ctxt. size( )));
}

void calc::sequent::pop_back( )
{
   if( lev. size( ) == 0 )
      throw std::logic_error( "pop_back( ) : nothing to pop" );

   lev. pop_back( );
}

const calc::sequent::level& calc::sequent::back( ) const 
{
   if( lev. size( ) == 0 )
      throw std::logic_error( "back: there are no levels" );

   return lev. back( );
}

calc::sequent::level& calc::sequent::back( )
{  
   if( lev. size( ) == 0 )
      throw std::logic_error( "back: there are no levels" );

   return lev. back( );
}



#if 0

logic::exact
calc::sequent::getexactname( size_t i ) const
{
   if( i >= size( )) throw std::logic_error( "sequent::getexactname" ); 
   switch( steps[i]. sel( ))
   {
   case seq_belief:
      return steps[i]. view_belief( ). name( ); 


   }
   std::cout << steps[i]. sel( ) << "\n";
   throw std::logic_error( "cannot get exact name" );
}

#if 0
void
calc::sequent::addformula( std::string_view namebase,
                           const logic::term& f,
                           unsigned int par1, unsigned int par2,
                           unsigned int br, unsigned int nr )
{
   if( namebase. empty( ))
      throw std::logic_error( "new formula: namebase cannot be empty" );

   auto name = get_fresh_ident( namebase );
   auto ex = blfs. append( logic::belief( logic::bel_form, name, f ));
   bool anf = isinanf(f); 
   size_t rank = 0;
   if( anf )
   {
      rank = alternation_rank(f);
   }
   ext. push_back( extension( ext_initial, false, "", 
                              ex, f, anf, rank, par1, par2, br, nr ));
}

#endif


void calc::sequent::restore( size_t ss )
{
   // If anything got blocked, we unblock it:

   while( ss < steps. size( )) 
   {
      switch( steps. back( ). sel( ))
      {
      case seq_belief:
         {
            auto bl = steps. back( ). view_belief( );
            blfs. backtrack( bl. name( ));
         }
         break;
      case seq_blocking:
         {
            size_t nr = steps. back( ). view_blocking( ). nr( );
            steps[ nr ]. view_belief( ). visible( ) = true;
         }
         break;
      }
      steps. pop_back( );
   }
}

#endif

void calc::sequent::ugly( std::ostream& out ) const
{
   out << "Sequent:\n";
   out << ctxt;
   out << "\n";
   out << "Definitions:\n";
   for( const auto& def : defs )
      out << "   #" << def. first << " := " << def. second << "\n";
  
   out << "\n"; 
   out << "Levels:\n";
   for( const auto& l : lev ) 
   {
      out << "   level " << l. name << ", ";
      out << "contextsize = " << l. contextsize << ":\n";
      for( size_t i = 0; i != l. stack. size( ); ++ i )
         out << "      " << i << " : " << l. stack[i] << "\n";
   } 
}

#if 0

void calc::sequent::pretty( std::ostream& out, bool showblocked ) const
{
   out << "Sequent:\n";
   for( const auto& e : steps )
   {
      switch( e. sel( ))
      {
      case seq_belief:
         {
            auto bl = e. view_belief( ); 
            if( showblocked || bl. visible( ))
            { 
               out << "   " << blfs. at( bl. name( ) ). ident( );
               logic::pretty::print( out, blfs, blfs. at( bl. name( ) )); 
            } 
         }
         break; 
      default: 
         std::cout << e. sel( ) << "\n";
         throw std::runtime_error( "doesnt know how to be pretty" );
      }
   }
}

#endif


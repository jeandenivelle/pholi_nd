
#include "optform.h"
#include "printing.h"

#include "removelets.h"
#include "alternating.h"
#include "projection.h"

#include "logic/replacements.h"


void calc::optform::musthave( logic::selector op )
{
   if( !fm. has_value( ))
      return; 
   
   if( fm. value( ). sel( ) == op )
      return;

   errorstack::builder bld;
   bld << "wrong operator for " << rule << " : ";
   bld << "main operator must be " << printing::viewpretty( op );
   bld << ", but the formula is: ";
   pretty( bld );
   err. push( std::move( bld ));

   fm. reset( ); 
}


void calc::optform::replacebysub( size_t i ) 
{
   if( !fm. has_value( ))
      return; 

   if( !fm. value( ). option_is_kleene( ))
      throw std::logic_error( "replacebysub: Not a Kleene operator" );

   auto kl = fm. value( ). view_kleene( );

   if( i >= kl. size( ))
   {
      auto bld = printing::makeheader( seq, rule );
      bld << "need subform nr " << i << ", but there are only ";
      bld << kl. size( ) << " subforms in: ";
      pretty( bld );
      err. push( std::move( bld ));
      fm. reset( );
      return;
   }
   
   fm = kl. extr_sub(i);
}


void calc::optform::getuniquesub( ) 
{
   if( !fm. has_value( ))
      return;

   if( !fm. value( ). option_is_kleene( ))
      throw std::logic_error( "getuniquesub: Not a Kleene operator" );

   auto kl = fm. value( ). view_kleene( );

   if( kl. size( ) != 1 )
   {
      auto bld = printing::makeheader( seq, rule );
      bld << "formula must have arity one, but it is: ";
      pretty( bld );
      err. push( std::move( bld ));

      fm. reset( );
      return;
   }

   fm = kl. extr_sub(0);
}

void calc::optform::aritymustbe( size_t i )
{  
   if( !fm. has_value( ))
      return;

   if( !fm. value( ). option_is_kleene( ))
      throw std::logic_error( "aritymustbe: Not a Kleene operator" );

   auto kl = fm. value( ). view_kleene( );

   if( kl. size( ) != i )
   {
      auto bld = printing::makeheader( seq, rule );
      bld << "formula must have arity " << i << ", but it is : ";
      pretty( bld );
      err. push( std::move( bld ));
   
      fm. reset( );
   }
}

void calc::optform::nrvarsmustbe( size_t i )
{ 
   if( !fm. has_value( ))
      return;

   if( !fm. value( ). option_is_quant( ))
      throw std::logic_error( "nrvarsmustbe: Not a quantifier" );

   auto kl = fm. value( ). view_quant( );

   if( kl. size( ) != i )
   {
      auto bld = printing::makeheader( seq, rule );
      bld << "quantifier must have " << i << " variables, but it is : ";
      pretty( bld );
      err. push( std::move( bld ));
  
      fm. reset( );
   }
}


#if 0
void calc::optform::normalize( )
{
   if( !fm. has_value( ))
      return;

   logic::betareduction beta;
   projection proj( seq.blfs );

   do
   {
      beta. counter = 0;
      rewr_outermost( beta );
   }
   while( beta. counter );
}
#endif

void calc::optform::fake( )
{
   if( !fm. has_value( ))
      return; 

   auto bld = printing::makeheader( seq, rule );
   bld << "faking proof of:  ";
   pretty( bld );
   err. push( std::move( bld ));
}

void calc::optform::print( std::ostream& out ) const
{
   if( fm. has_value( ))
      out << rule << " : " << fm. value( );
   else
      out << rule << " : " << "(no formula)";
   out << "\n";
}

void calc::optform::pretty( std::ostream& out ) const
{
#if 0
   if( fm. has_value( ))
      printing::pretty( out, seq, fm. value( )); 
   else
      out << "(no formula)";
#endif
   out << "(pretty printing not implemented)"; 
   out << "\n";
}



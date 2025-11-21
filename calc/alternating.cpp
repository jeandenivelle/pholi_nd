
#include "alternating.h"
#include "kleening.h"

namespace
{

   void 
   print( std::ostream& out, const std::vector< logic::vartype > & ctxt )
   {
      out << "[";  
      for( auto p = ctxt. begin( ); p != ctxt. end( ); ++ p )
      {
         if( p == ctxt. begin( ))
            out << " ";
         else
            out << ", ";
         out << *p;
      }
      out << " ] :   ";
   }


   void appendvars( std::vector< logic::vartype > & ctxt,
                    const logic::term& fm )
   {
      auto quant = fm. view_quant( );
      for( size_t i = 0; i != quant. size( ); ++ i )
         ctxt. push_back( quant. var(i));
   }


   void
   restore( std::vector< logic::vartype > & ctxt, size_t s )
   {


   }

}


logic::selector 
calc::kleening( logic::selector sel, polarity pol )
{
   using namespace logic;

   if( pol == pol_pos )
   {
      switch( sel )
      {
      case op_false:
      case op_or:
      case op_lazy_or:
         return op_kleene_or;

      case op_true:
      case op_and:
      case op_lazy_and:
         return op_kleene_and;

      }
   }

   if( pol == pol_neg )
   {
      switch( sel )
      {
      case op_false:
      case op_or:
      case op_lazy_or:
         return op_kleene_and;
         
      case op_true:
      case op_and:
      case op_lazy_and:
         return op_kleene_or;

      }
   }
   throw std::logic_error( "Kleening: operator not implemented" );
}

calc::anf< logic::term >
calc::flatten( anf< logic::term > conj )
{
   // We do two passes: In the first, we try to flatten 
   // and/forall. 
   // In the second, we flatten or/exists.

   anf< logic::term > conj2; 
   for( auto& cl : conj )
   {
      if( cl. body. size( ) == 1 && cl. body. at(0). nrvars( ) == 0 )
      {
         std::vector< logic::vartype > ctxt = std::move( cl. vars );
         conjunction< forall< logic::term >> res;
         flatten( ctxt, pol_pos, cl. body. at(0). body, res );

         for( auto& c : res )
         {
            conj2. append( forall( c. vars, disjunction( { exists( c. body ) } )));
         }
      }
   }
   return conj2;
}



void 
calc::flatten( std::vector< logic::vartype > & ctxt,
               polarity pol,
               const logic::term& fm,
               conjunction< forall< logic::term >> & conj )
{
   std::cout << "flatten-conj " << pol << " :  " << fm << " " << "\n";

   using namespace logic;

   switch( fm. sel( ))
   {
   case op_exact:
   case op_debruijn:
   case op_unchecked:
      conj. append( forall( ctxt, apply( fm, pol ) ));
      return;

   case op_false:
   case op_true:
      if( kleening( fm. sel( ), pol ) == op_kleene_or )
      {
         conj. append( forall( ctxt, term( op_false )));
         return;
      }
      else
         return;

   case op_error:
      return;

   case op_and:
   case op_or:
   case op_lazy_and:
   case op_lazy_or:
      {
         if( kleening( fm. sel( ), pol ) == op_kleene_and )
         {
            auto bin = fm. view_binary( );
            flatten( ctxt, pol, bin. sub1( ), conj );
            flatten( ctxt, pol, bin. sub2( ), conj );
         }
         else
            conj. append( forall( ctxt, apply( fm, pol ) ));
         return;
      }

   case op_not:
      flatten( ctxt, -pol, fm. view_unary( ). sub( ), conj );
      return;

   case op_prop:
      flatten_prop( ctxt, pol, fm. view_unary( ). sub( ), conj );
      return;


#if 0
   if( kln. sel( ) == logic::op_exists &&
       kln. view_quant( ). size( ) == 1 )
   {
      flatten( conj, ctxt, kln. view_quant( ). body( ));
      return; 
   }

   if( kln. sel( ) == logic::op_kleene_or &&
       kln. view_kleene( ). size( ) == 1 )
   {
      flatten( conj, ctxt, kln. view_kleene( ). sub(0));
      return;
   }

   if( kln. sel( ) == logic:: op_kleene_and )
      throw std::logic_error( "kleene_and" );

   if( kln. sel( ) == logic::op_kleene_forall )
      throw std::logic_error( "kleene_forall" );

   auto ex = disjunction( { exists( fm ) } );
  
   conj. append( forall( ctxt, std::move( ex )));
#endif
   }

   std::cout << "flatten-conj " << pol << " :  " << fm. sel( ) << "\n";
   throw std::logic_error( "operator not implemented" );
}


void
calc::flatten( std::vector< logic::vartype > & ctxt, 
               polarity pol,
               const logic::term& fm,
               disjunction< exists< logic::term >> & disj )
{
   std::cout << "flatten-disj " << pol << " :  " << fm << " " << "\n";

   using namespace logic;

   switch( fm. sel( ))
   {
   case op_exact:
   case op_debruijn:
   case op_unchecked:
      disj. append( exists( ctxt, apply( fm, pol ) ));
      return;

   case op_false:
   case op_true:
      if( kleening( fm. sel( ), pol ) == op_kleene_and )
      {
         disj. append( exists( ctxt, term( op_true )));
         return;
      }
      else 
         return;

   case op_error:
      return;

   case op_not:
      flatten( ctxt, -pol, fm. view_unary( ). sub( ), disj );
      return;

   case op_prop:
      flatten_prop( ctxt, pol, fm. view_unary( ). sub( ), disj );
      return;

   case op_and:
   case op_or:
   case op_lazy_and:
   case op_lazy_or:
      {
         if( kleening( fm. sel( ), pol ) == op_kleene_or )
         {
            auto bin = fm. view_binary( );
            flatten( ctxt, pol, bin. sub1( ), disj );
            flatten( ctxt, pol, bin. sub2( ), disj );
         }
         else
            disj. append( exists( ctxt, apply( fm, pol ) ));
         return;
      }

   }

   std::cout << "flatten-disj " << pol << " :  " << fm. sel( ) << "\n";
   throw std::logic_error( "operator not implemented" );
}


void
calc::flatten_prop( std::vector< logic::vartype > & ctxt,
                    polarity pol,
                    const logic::term& fm,
                    conjunction< forall< logic::term >> & conj )
{
   std::cout << "flatten-conj-prop " << pol << " :  " << fm << " " << "\n";

   using namespace logic;

   switch( fm. sel( ))
   {

   case op_not:
      flatten_prop( ctxt, pol, fm. view_unary( ). sub( ), conj );
      return;

   case op_forall:
   case op_exists:
      {
         if( pol == pol_neg )
         {
            conj. append( forall( ctxt, apply_prop( fm, pol )));
            return;
         }


      }

   }

   std::cout << "flatten-conj-prop " << pol << " :  " << fm. sel( ) << "\n";
   throw std::logic_error( "operator not implemented" );
}


void
calc::flatten_prop( std::vector< logic::vartype > & ctxt,
                    polarity pol,
                    const logic::term& fm,
                    disjunction< exists< logic::term >> & disj )
{
   std::cout << "flatten-disj-prop " << pol << " :  " << fm << " " << "\n";

   using namespace logic;

   switch( fm. sel( ))
   {

   case op_not:
      flatten_prop( ctxt, pol, fm. view_unary( ). sub( ), disj );
      return;

   case op_and:
   case op_or:
   case op_implies:
   case op_equiv:
      {

         auto bin = fm. view_binary( ); 
         flatten_prop( ctxt, pol, bin. sub1( ), disj );
         flatten_prop( ctxt, pol, bin. sub2( ), disj );
         return;
      }

   case op_forall:
   case op_exists:
      {
         size_t s = ctxt. size( ); 
         appendvars( ctxt, fm );
         flatten_prop( ctxt, pol, fm. view_quant( ). body( ), disj );
         return;
      }

   case op_apply:
      {
         disj. append( exists( ctxt, apply_prop( fm, pol )));
         return;
      }

   }

   std::cout << "flatten-disj-prop " << pol << " :  " << fm. sel( ) << "\n";
   throw std::logic_error( "operator not implemented" );
}



#if 0

logic::term 
calc::alternating( const logic::term& f, logic::selector op, 
                   unsigned int rank ) 
{
   if constexpr( false )
   {
      std::cout << "alternating " << op << "/" << rank << " :  " << f << "\n\n";
   }

   if( rank == 0 )
      return f;
   else
   {
      std::vector< logic::term > args;
      logic::context ctxt; 
      flatten( ctxt, f, op, rank, args );
      return logic::term( op, args. begin( ), args. end( ));
   }
}

#endif

#if 0

void 
calc::flatten( logic::context& ctxt, const logic::term& frm, 
               logic::selector op, unsigned int rank,
               std::vector< logic::term > & into )
{
   if constexpr( false )
   {
      std::cout << "flatten " << op << "/" << rank << " : ";
      std::cout << frm << "\n\n";
   }

   auto kln = kleene_top( frm, pol_pos );

   if( kln. sel( ) == op )
   {
      auto nary = kln. view_kleene( );
      for( size_t i = 0; i != nary. size( ); ++ i )
         flatten( ctxt, nary. sub(i), op, rank, into );
      return;
   }

   if( kln. sel( ) == quantof( op ))
   {
      auto ex = kln. view_quant( );
      size_t csize = ctxt. size( );
      for( size_t i = 0; i != ex. size( ); ++ i )
         ctxt. append( ex. var(i). pref, ex. var(i). tp );  
      flatten( ctxt, ex. body( ), op, rank, into );
      ctxt. restore( csize );  
      return; 
   }

   into. push_back( quantify( quantof( op ), ctxt,
                    alternating( frm, alternation( op ), rank - 1 )));
}
#endif

#if 0

size_t
calc::alternation_rank( const logic::term& f )
{
   size_t rank = 0;

   if( f. sel( ) == logic::op_kleene_or )
   {
      auto kl = f. view_kleene( );
      for( size_t i = 0; i != kl. size( ); ++ i )
      {
         const auto* p = &kl. sub(i);

         // If it is a Kleene exists, we replace p by the body:

         if( p -> sel( ) == logic::op_kleene_exists )
            p = &( p -> view_quant( ). body( ));

         if( p -> sel( ) == logic::op_kleene_and )
         {
            size_t rk = alternation_rank( *p );
            if( rk > rank )
               rank = rk; 
         }
      }
      return rank + 1;
   }

#if 0
   if( isliteral(f))
      return 0;

   size_t inc = ischange( op, f. sel( ));
      // What we will be increasing here.
 
   switch( f. sel( ))
   {
   case logic::op_kleene_and:
   case logic::op_kleene_or:
      {
         size_t max = 1;
         auto prop = f. view_kleene( );
         for( size_t i = 0; i != prop. size( ); ++ i )
         {
            size_t m = alternation_rank( prop. sub(i), f. sel( )); 
            if( m > max )
               max = m;
         }
        
         return max + inc; 
      }
 
   case logic::op_kleene_forall:
   case logic::op_kleene_exists:
      {
         auto quant = f. view_quant( );

         size_t sub = alternation_rank( quant. body( ), f. sel( ));
         if( sub == 0 ) 
            sub = 1;

         return sub + inc; 
      }
   default:
      throw std::logic_error( "alternation rank : should be unreachable" );

   }
#endif
   throw std::logic_error( "rank: not in ANF" );
}
#endif

#if 0

logic::term
calc::restrict_alternation( transformer& trans, logic::beliefstate& blfs,
                logic::context& ctxt, logic::term f,
                logic::selector op, unsigned int maxrank )
{
   if constexpr( false )
   {
      std::cout << "restrict alternation : " << f << "\n";
      std::cout << "   " << op << "/" << maxrank << "\n";
   }

   if( isliteral(f))
      return f;

   // If we are not a literal, then the rank is >= 1.

   bool dec = ischange( op, f. sel( ));
      // True if we are going to decrease.

   if( maxrank == 0 || ( dec && maxrank == 1 ))
   {
      auto pr = norm_debruijns(f);

      auto restr = restriction( ctxt, pr. first );
      logic::exact pred = trans. newpredsym( blfs, "p", restr );
      trans. push( std::move( restr ), pred, pr. second,
                   pol_neg, step_rank );
      return application( logic::term( logic::op_exact, pred ), pr. first );
   }

   // We check if there is a level increase:

    if( dec ) 
      -- maxrank;

   switch( f. sel( ))
   {
   case logic::op_kleene_and:
   case logic::op_kleene_or:
      {
         auto prop = f. view_kleene( );
         for( size_t i = 0; i != prop. size( ); ++ i )
         {
            prop. update_sub( i,
               restrict_alternation( trans, blfs, ctxt, prop. extr_sub(i),
                                     f. sel( ), maxrank ));
         }
         return f;
      }
  
   case logic::op_kleene_forall:
   case logic::op_kleene_exists:
      {
         auto q = f. view_quant( ); 
         size_t ss = ctxt. size( );
         for( size_t i = 0; i != q. size( ); ++ i )
            ctxt. append( q. var(i). pref, q. var(i). tp );

         q. update_body( 
                restrict_alternation( trans, blfs, ctxt, q. extr_body( ), 
                                      f. sel( ), maxrank ));

         ctxt. restore(ss);
         return f;
      }
   }

   throw std::runtime_error( "splitalt: should be not reachable" );
}

#endif

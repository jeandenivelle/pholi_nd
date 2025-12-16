
#include "proofchecking.h"

#include "logic/pretty.h"
#include "logic/replacements.h"
#include "logic/counters.h"
#include "logic/structural.h"
#include "logic/cmp.h"
#include "logic/counters.h"

#include "expander.h"
#include "localexpander.h"
#include "outermost.h"
#include "traverse.h"
#include "alternating.h"
#include "atp.h"

#include "printing.h"

#include "logic/termoperators.h"

auto 
calc::lift( forall< disjunction< exists< logic::term >>> cls, size_t dist )
   -> forall< disjunction< exists< logic::term >>>
{
   std::cout << "lifting " << cls << " over distance " << dist << "\n";
   if( dist > 0 )
   {
      auto lift = logic::lifter( dist );
      return outermost( lift, std::move( cls ), 0 );
   }
   else
      return cls; 
}

// This is a very bad case of a bloated implementation, 
// which should be looked at from the programming language point of view. 

bool calc::istautology( const logic::term& disj ) 
{
   if( disj. sel( ) != logic::op_kleene_or )
      throw std::logic_error( "calc::istautology : Not a Kleene disjunction" );

   auto kl = disj. view_kleene( );

   // We just do a few simple checks:

   for( size_t i = 0; i != kl. size( ); ++ i )
   {
      if( kl. sub(i). sel( ) == logic::op_equals )
      {
         auto eq = kl. sub(i). view_binary( );
         if( equal( eq. sub1( ), eq. sub2( )) )
            return true;
      }
   }

   // This is of course entirely ridiculous, but it is an exercise
   // in coding too:

   for( size_t i = 0; i != kl. size( ); ++ i )
   {
      if( kl. sub(i). sel( ) == logic::op_not )
      {
         const auto& sub = kl. sub(i). view_unary( ). sub( );
         if( sub. sel( ) == logic::op_prop || 
             sub. sel( ) == logic::op_equals )
         {
            for( size_t j = 0; j != kl. size( ); ++ j )
            {
               if( i != j && equal( sub, kl. sub(j)) )
                  return true;
            }
         }

         if( sub. sel( ) == logic::op_prop )
         {
            const auto& subsub = sub. view_unary( ). sub( );

            bool saw_subsub = false;
            bool saw_notsubsub = false;
 
            for( size_t j = 0; j != kl. size( ); ++ j )
            {
               if( equal( kl. sub(j), subsub ))
                  saw_subsub = true;

               if( kl. sub(j). sel( ) == logic::op_not &&
                   equal( kl. sub(j). view_unary( ). sub( ), subsub ))
               {
                  saw_notsubsub = true;
               }
            }

            if( saw_subsub && saw_notsubsub ) 
               return true;
         }
      }
   }

   return false; 
}


void
calc::checkproof( const logic::beliefstate& blfs,
                  proofterm& prf, sequent& seq, errorstack& err )
{
   switch( prf. sel( ))
   {

#if 0
   case prf_ident:
      {
         auto id = prf. view_ident( ). ident( );
         const auto& f = seq. blfs. getformulas( id ); 
         if( f. empty( ))
         {
            errorstack::builder bld;
            bld << "unknown identifier " << id << " was used in a proof\n";
            bld << seq << "\n";
            err. push( std::move( bld ));
            return { };
         }

         if( f. size( ) > 1 )
         {
            errorstack::builder bld; 
            bld << "ambiguous identifier " << id << " was used in a proof\n";
            bld << seq << "\n";
            err. push( std::move( bld ));
            return { };
         }

         return seq. getformula( f. front( ), err );
      }
#endif

   case prf_flatten: 
      {
         auto fm = std::move( seq. back( ). get(0));
         seq. back( ). pop( );
         std::cout << "flattening " << fm << "\n";

         anf< logic::term > conj;
         conj. append( std::move(fm));
         conj = flatten( std::move( conj ));

         std::cout << conj << "\n\n";
         for( auto& c : conj )
            seq. back( ). push( std::move(c) );

         return;
      }

   case prf_orexistselim:
      {
         auto elim = prf. view_orexistselim( ); 
         auto mainform = std::move( seq. back( ). get(0));
            // Should be a universally quantified disjunction,
            // without variables.

         seq. back( ). pop( );

         if( mainform. vars. size( ))
         {
            std::cout << "orexistselim\n";
            throw std::runtime_error( "there are universal variables" );
         }

         const dnf< logic::term > disj = std::move( mainform. body );
            // It is a disjunction of existential formulas. 

         dnf< logic::term > result;
            // This will be our result.

         size_t ss = seq. ctxt. size( ); 
         size_t nrlevels = seq. size( );

         if( disj. size( ) < elim. size( ))
         {
            std::cout << "CRASH IS IMMINENT\n";
            throw std::runtime_error( "disjunction is too small" );
         }

         for( size_t i = 0; i != elim. size( ); ++ i )
         {
            const auto& sub = disj. at(i);
 
            // Assume the existential variables:
 
            for( size_t v = 0; v != sub. vars. size( ); ++ v )
               seq. assume( sub. vars[v]. pref, sub. vars[v]. tp );

            // Create a new assumption level:

            seq. push_back( elim. name( ));

            seq. back( ). push( forall( disjunction{ exists( sub. body ) } ));
            auto subproof = elim. extr_branch(i);
            checkproof( blfs, subproof, seq, err );
            elim. update_branch( i, std::move( subproof ));

            std::cout << "==============================\n";
            std::cout << "disjunction is " << disj << "\n";
            std::cout << "number options = " << disj. size( ) << "\n";
            std::cout << "choice was: " << i << "\n";

#if 0
            // Was part of testing. Should be completely removed later:

            seq. assume( "hhhh", logic::type( logic::type_form ));
            seq. assume( "ssss", logic::type( logic::type_obj ));

            seq. ugly( std::cout );  
            std::cout << "\n";
#endif

            // We use the last formula. If there are no formulas, 
            // it is an error:

            if( seq. back( ). size( ) == 0 )
            {
               throw std::runtime_error( "orexistselim: No result" );
            }

            auto concl = std::move( seq. back( ). get(0));
               // Conclusion of our current assumption.

            if( concl. vars. size( ))
            {
               std::cout << concl << "\n";
               throw std::runtime_error( "orexistselim: universal variables in conclusion" );
            }

            // concl is now a forall without variables, 
            // containing a disjunction:

#if 0
            // This was used for testing.

            concl. body = disjunction( 
               {
                  exists( logic::forall( {{ "A", logic::type( logic::type_form ) }}, apply( 5_db, { 3_db, 4_db } ))), 
                  exists( {{ "X", logic::type( logic::type_form ) },
                           { "Y", logic::type( logic::type_obj ) }},
                          apply( "f"_unchecked, { 0_db, 1_db, 2_db, 5_db, 6_db, 7_db } ))
               } );

            std::cout << "concl = " << concl << "\n";
            std::cout << "ss = " << ss << "\n";
#endif

            // concl. body( ) is a disjunction of existentially
            // quantified formulas. For each disjunct separately,
            // we determine its free variables, and 
            // add existential quantifiers for them.

            for( size_t i = 0; i != concl. body. size( ); ++ i )
            {
               // We construct a substitution that normalizes
               // the free variables in concl. body. at(i).

               // In order to do that, we first collect 
               // the free variables of concl. body. at(i) : 
 
               logic::debruijn_counter vars;
               traverse( vars, concl. body. at(i), 0 );

               // We don't care about all free variables, only about the 
               // ones that we assumed by ourselves. 
               // We go through our assumptions, check if they occur
               // in vars. We create a normalizing subsitution for those. 

               auto norm = logic::normalizer( seq. ctxt. size( ) - ss );

               for( size_t v = 0; v + ss < seq. ctxt. size( ); ++ v )
               {
                  if( vars. contains(v))
                     norm. append(v);
               }
 
               // apply norm on the body:

               concl. body. at(i) = 
                  outermost( norm, std::move( concl. body. at(i)), 0 );

               std::vector< logic::vartype > quant;

               // These are the assumptions that we are about to drop:

               for( size_t v = seq. ctxt. size( ) - ss; v -- ; )
               {
                  if( vars. contains(v))
                     quant. push_back( { seq. ctxt. getname(v),
                                         seq. ctxt. gettype(v) } );                          
               }

               for( auto& q : concl. body. at(i). vars )
                  quant. push_back( std::move(q));

               concl. body. at(i). vars = std::move( quant );
            }

            if( seq. size( ) != nrlevels + 1 )
               throw std::logic_error( "something went wrong with the levels" );

            seq. pop_back( );
            seq. ctxt. restore( ss );

            // concl still is a forall without variables:

            for( auto& d : concl. body )
               result. append( std::move(d));
         }

         // If there are more disjunctions than cases in the proof,
         // we copy the missing disjuncts unchanged:

         if( elim. size( ) < disj. size( ))
         {
            std::cout << elim. size( ) << " " << disj. size( ) << "\n";

            for( size_t i = elim. size( ); i < disj. size( ); ++ i )
               result. append( std::move( disj. at(i)) );
         }

         atp::simplify( result );

         seq. back( ). push( forall( std::move( result )));

         return; 
      }

   case prf_propcut:
      {
         auto cut = prf. view_propcut( );
 
         auto fm = cut. extr_fm( );  
         size_t ss = seq. ctxt. size( );
         fm = replace_debruijn( seq. db, fm );
 
         auto tp = checkandresolve( blfs, err, seq. ctxt, fm );
         if( !tp. has_value( ) || tp. value( ). sel( ) != logic::type_form )
         {
            std::cout << "wrong type!\n";
            throw std::logic_error( "no type" ); 
         }

         cut. update_fm( fm );
         fm = logic::term( logic::op_prop, fm );

         seq. back( ). push( forall( disjunction{ 
                exists( logic::term( logic::op_not, fm )), exists(fm) } ));

         return;
      }

   case prf_chain:
      {
         auto ch = prf. view_chain( );

         for( size_t i = 0; i != ch. size( ); ++ i )
         {
            auto step = ch. extr_step(i);
            checkproof( blfs, step, seq, err );
            ch. update_step( i, std::move( step ));
         } 

         return; 
      }

#if 0
   case prf_expand:
      {
         auto exp = prf. view_expand( ); 

         auto parent = proofcheck( exp. parent( ), seq, err ); 
         if( !parent. has_value( ))
            return { };

         auto nm = optform( std::move( parent ), "expand", seq, err );

         expander def( exp. ident( ), exp. occ( ), seq. blfs, err );
            // We are using unchecked identifier exp. ident( ).
            // The expander will look only at exact overloads. 
            // This guarantees type safety.

         std::cout << def << "\n"; 

         nm. normalize( );
         // nm. make_anf2( );
         std::cout << "expand returns " << nm << "\n";
         return nm. value( );
      }
#endif

   case prf_expandlocal:
      {
         auto var = prf. view_expandlocal( ). var( );

         // We have an identifier, but we need a De Bruijn index:

         size_t ind = seq. ctxt. size( );
         {  
            auto p = seq. db. find( var );
            if( p == seq. db. end( ))
            {
               throw std::logic_error( "did not find the variable" );
            }

            ind = p -> second;
         }
         std::cout << "index = " << ind << "\n";

         auto def = seq. defs. find( ind );
         if( def == seq. defs. end( ))
         {
            throw std::logic_error( "variable has no definition" );
         }

         if( seq. back( ). contextsize != seq. ctxt. size( ))
            throw std::logic_error( "size of context does not fit to level" );

         auto exp = localexpander( seq. ctxt. size( ) - ind - 1,  
                                   def -> second, 
                                   prf. view_expandlocal( ). occ( ));

         auto fm = std::move( seq. back( ). get(0));
         seq. back( ). pop( );

         fm = outermost( exp, std::move(fm), 0 );
         seq. back( ). push( std::move(fm));
         return;
      }

   case prf_lift:
      {
         auto lft = prf. view_lift( );

         // This is not terribly efficient, but I think
         // that the number of levels in a proof is logarithmic in its
         // size.

         seq. ugly( std::cout );

         for( size_t lev = 0; lev != seq. size( ); ++ lev )
         {
            if( seq. at( lev ). name == lft. level( ))
            { 
               if( lft. ind( ) < seq. at( lev ). stack. size( ))
               {
                  auto fm = seq. at( lev ). stack[ lft. ind( ) ];
                  seq. back( ). push( lift( std::move(fm),
                                      seq. ctxt. size( ) -
                                      seq. at( lev ). contextsize ));

                  return;  // succesful return. 
               }
               else
                  std::cout << "too big\n";
            }
         }
         throw std::logic_error( " not found " );
      }

#if 0
   case prf_define: 
      {
         auto def = prf. view_define( );

         // We first need to typecheck the value:

         auto val = def. val( );
         size_t errsize = err. size( );
         logic::context ctxt;
         auto tp = checkandresolve( seq. blfs, err, ctxt, val );

         if( err. size( ) > errsize )
         {
            err. addheader( errsize, "during type checking of inproof definition" );
            throw std::logic_error( "do something" );
         } 

         if( !tp. has_value( ))
            throw std::logic_error( "should be unreachable" );

         size_t seqsize = seq. size( );
         seq. define( def. name( ), val, tp. value( ));
         
         auto res = proofcheck( def. parent( ), seq, err );
         if( !res. has_value( ))
         {
            seq. restore( seqsize ); 
            return res; 
         }

         logic::rewriterule rewr( 
            logic::term( logic::op_exact, seq. getexactname( seqsize )), 
            val );

         res. value( ) = outermost( rewr, std::move( res. value( )), 0 );
         seq. restore( seqsize );
         return res;
      }

   case prf_cut:
      {
         auto cut = prf. view_cut( );

         // We evaluate the first parent: 

         auto first = proofcheck( cut. first( ), seq, err );
         if( !first. has_value( ))
            return { };

         size_t seqsize = seq. size( );
         seq. assume( cut. name( ), first. value( ));

         auto res = proofcheck( cut. second( ), seq, err );
         seq. restore( seqsize );
         return res;
      }
   
   case prf_forallintro:
      {
         auto intro = prf. view_forallintro( );

         std::vector< logic::exact > exactnames;
            // The names that seq will give to the assumptions.
            // We need them, so that we can subtitute them away later.

         auto seqsize = seq. size( );
         for( size_t i = 0; i != intro. size( ); ++ i )
         {
            logic::exact name = 
               seq. assume( intro.var(i).pref, intro.var(i).tp );

            exactnames. push_back( name );
         }
 
         auto res = optform( proofcheck( intro. parent( ), seq, err ), 
                             "forall-intro", seq, err );

         if( !res )
            return { };

         res. musthave( logic::op_kleene_and );
        
         logic::introsubst subst;
         for( const auto& e : exactnames )
            subst. bind(e);

         res. rewr_outermost( subst );

         std::vector< logic::vartype > vars; 
         for( size_t i = 0; i != intro. size( ); ++ i ) 
            vars. push_back( intro. var(i));
 
         res. quantify( vars );
         seq. restore( seqsize );

         return res. value( ); 
      }
#endif

   case prf_forallelim:
      {
         auto elim = prf. view_forallelim( );
         auto mainform = std::move( seq. back( ). get(0));
            // Should be universally quantified.

         seq. back( ). pop( );

         if( mainform. vars. size( ) < elim. size( ))
         {
            throw std::runtime_error( "forallelim: Too many values" );
            // We could just eliminate a few. 
         }
 
         size_t errstart = err. size( );
         logic::fullsubst subst;

         size_t ss = seq. ctxt. size( );

         bool alltypescorrect = true;

         for( size_t i = 0; i != elim. size( ); ++ i )
         {
            auto inst = elim. extr_value(i);
            auto tp = checkandresolve( blfs, err, seq. ctxt, inst );

            if( ss != seq. ctxt. size( ))
               throw std::logic_error( "size changed, should not happen" );

            if( ! tp. has_value( ) ||  
                !equal( tp. value( ), mainform. vars[i]. tp ))
            {
               alltypescorrect = false; 
            }

            elim. update_value( i, inst ); 
            subst. append( std::move( inst ));

#if 0
            logic::context ctxt;
            auto tm = elim. value(i);
            auto tp = checkandresolve( seq. blfs, err, ctxt, tm );

            if( tp. has_value( )) 
            {
               if( logic::equal( tp. value( ), q. var(i). tp ))
               {
                  subst. push( std::move( tm ));
               }
               else
               {
                  auto bld = errorstack::builder( ); 
                  bld << "true type of instance ";
                  printing::pretty( bld, seq, tm );
                  bld << " is wrong\n"; 
                  bld << "It is "; 
                  printing::pretty( bld, seq, tp. value( ));
                  bld << ", but must be ";
                  printing::pretty( bld, seq, q. var(i). tp ); 
                  err. push( std::move( bld ));
               }
            }
#endif
         }

         if( !alltypescorrect )
         {
            throw std::runtime_error( "we cannot do forallinst, types wrong" );
         }

         std::cout << subst << "\n";
         mainform = outermost( subst, std::move( mainform ), 0 ); 
 
         // We do not remove the outermost forall, because the
         // implementation requires its presence. We remove the variables
         // that were instantiated. It is possible that some remain. 

         mainform. vars. erase( mainform. vars. begin( ),
                                mainform. vars. begin( ) + elim. size( ));

         std::cout << "after instantiation: " << mainform << "\n";
         seq. back( ). push( std::move( mainform ));
         return;  
      }
#if 0
   case prf_andintro:
      {
         auto intro = prf. view_andintro( ); 

         auto result = 
            logic::term( logic::op_kleene_and,
                         std::initializer_list< logic::term > ( ));

         for( size_t i = 0; i != intro. size( ); ++ i )
         {
            auto fm = proofcheck( intro. parent(i), seq, err );

            // There is no point in continuing I think:

            if( !fm. has_value( ))
               return fm; 

            auto conj = optform( std::move( fm ), "andintro", seq, err );
            conj. musthave( logic::op_kleene_and );
            if( !conj )
               return { };

            auto kl = conj. value( ). view_kleene( );

            for( size_t i = 0; i != kl. size( ); ++ i )
               result. view_kleene( ). push_back( kl. sub(i));
         }

         return result;
      }

   case prf_select:
      {
         auto sel = prf. view_select( );
         auto fm = proofcheck( sel. parent( ), seq, err );
         if( !fm. has_value( ))
            return fm;

         auto conj = optform( std::move( fm ), "select", seq, err );
         conj. musthave( logic::op_kleene_and );
         if( !conj ) 
            return { };

         auto errsize = err. size( );

         auto kl = conj. value( ). view_kleene( );
         auto result = logic::term( 
              logic::op_kleene_and,
              std::initializer_list< logic::term > ( )); 

         for( size_t i = 0; i != sel. size( ); ++ i )
         {
            if( sel. nr(i) >= kl. size( ))
            {
               auto bld = errorstack::builder( );
               bld << "selected subterm " << sel. nr(i);
               bld << " >= " << kl. size( ); 
               err. push( std::move( bld )); 
            }
            else
               result. view_kleene( ). push_back( kl. sub( sel. nr(i) ));
         } 

         if( err. size( ) > errsize )
         {
            auto bld = printing::makeheader( seq, "select" );
            err. addheader( errsize, std::move( bld ));
         }

         return result;
      }

#endif
 
   case prf_simplify:
      {
         std::vector< forall< disjunction< exists< logic::term >>>> ignored;
         conjunction< atp::clause > simp; 

         auto& last = seq. back( );
         for( auto& f : last )
         {
            if( f. vars. size( ))
               ignored. push_back( std::move(f));
            else
               simp. append( std::move( f. body ));
         }
 
#if 0 
         std::cout << "before simplification: " << simp << "\n";
         std::cout << "ignoring\n";
         for( const auto& ig : ignored )
            std::cout << "   " << ig << "\n"; 
#endif
         atp::simplify( simp );

#if 0
         std::cout << "after simplification: " << simp << "\n";
#endif

         seq. back( ). clear( );
         for( auto& ig : ignored )
            seq. back( ). push( std::move(ig) );

         for( auto& s : simp )
            seq. back( ). push( forall( std::move(s)) );

         return;
      }

   case prf_fake:
      {
         auto res = prf. view_fake( ). goal( );
         auto cls = disjunction( { exists( std::move(res) ) } ); 
         seq. back( ). push( forall( std::move( cls ))); 
         return;
      }

   case prf_nop:
      {
         return;   // Truly nothing was done. 
      }

   case prf_show:
      {
         auto show = prf. view_show( ); 
         for( short unsigned int i = 0; i < 70; ++ i )
            std::cout << '-';
         std::cout << "\n"; 
         std::cout << "proof state " << show. comment( ) << ":\n";
         seq. ugly( std::cout ); 
         for( short unsigned int i = 0; i < 70; ++ i )
            std::cout << '-'; 
         std::cout << "\n\n"; 
         return;
      } 

   }

   std::cout << prf. sel( ) << "\n";
   throw std::logic_error( "dont know how to check proof term" );
}



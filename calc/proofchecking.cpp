
#include "proofchecking.h"

#include "logic/pretty.h"
#include "logic/replacements.h"
#include "logic/structural.h"
#include "logic/cmp.h"
#include "logic/inspections.h"

#include "expander.h"

#include "printing.h"


// This is a very bad case of bloated implementation, 
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

   // this is of course entirely ridiculous, but it is an exercise
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

   case prf_clausify:
      {
         auto cl = prf. view_clausify( ); 
         auto fm = checkproof( cl. parent( ), seq, err );
         if( !fm. has_value( ))
            return { };

         auto res = optform( std::move( fm ), "clausify", seq, err );
         res. make_anf2( );
         return res. value( );
      }
#endif

   case prf_orexistselim:
      {
         auto elim = prf. view_orexistselim( ); 
         seq. ugly( std::cout );
         auto mainform = std::move( seq. get(0));
            // Should be a universally quantified disjunction,
            // without variables.
 
         seq. pop( );

         std::cout << "mainform = " << mainform << "\n";
         if( mainform. vars. size( ))
         {
            throw std::runtime_error( "there are universal variables" );

         }

         const disjunction< exists< logic::term >> disj = 
            std::move( mainform. body );

         size_t nrvars = seq. nrvars( );
         size_t nrlevels = seq. nrlevels( );

         for( const auto& sub : disj ) 
         {
            std::cout << "sub = " << sub << "\n"; 
 
            // Assume the existential variables:
 
            for( size_t i = 0; i != sub. nrvars( ); ++ i )
            {
               seq. assume( sub. vars[i]. pref, sub. vars[i]. tp );
            }

            // Create a new assumption level:

            seq. addlevel( );

            seq. push( forall( disjunction{ exists( sub. body ) } ));

            std::cout << "============================\n";
            seq. ugly( std::cout );  
            std::cout << "\n";
#if 0
         bool success = true;

            {
               auto sub = optform( kl. sub(i), "or-elim", seq, err );
               sub. make_anf2( );
               seq. assume( elim. name(i), sub. value( ));
            }

            auto res = optform( proofcheck( elim. branch(i), seq, err ),
                                "option in or-elim", seq, err );
              
            res. musthave( logic::op_kleene_and );
            res. getuniquesub( );
            res. musthave( logic::op_kleene_or );

            if( !res )
               success = false; 
            else
            {
               auto disj = res. value( ). view_kleene( );
               for( size_t i = 0; i != disj. size( ); ++ i )
                  intro. push_back( disj. sub(i));  
            }

            seq. restore( seqsize );
#endif
            while( seq. nrlevels( ) > nrlevels )
               seq. poplevel( );
         }
#if 0
         if( !success )
            return { };

         return disjunction( intro );
#endif
         throw std::runtime_error( "keine ahnung" );
      }

   case prf_propcut:
      {
         auto cut = prf. view_propcut( );
 
         auto fm = cut. extr_fm( );  
         size_t s = seq. nrvars( );
         fm = replace_debruijn( seq. db, fm );
 
         auto tp = checkandresolve( blfs, err, seq. ctxt, fm );
         if( !tp. has_value( ) || tp. value( ). sel( ) != logic::type_form )
         {
            std::cout << "wrong type!\n";
            throw std::logic_error( "no type" ); 
         }

         cut. update_fm( fm );
         seq. push( forall( disjunction{ 
                exists( logic::term( logic::op_not, fm )), exists(fm) } ));
 
         return;
      }

   case prf_chain:
      {
         auto ch = prf. view_chain( );
         size_t nrvars = seq. nrvars( );
         // size_t nrforms = seq. nrforms( );

         for( size_t i = 0; i != ch. size( ); ++ i )
         {
            auto step = ch. extr_step(i);
            checkproof( blfs, step, seq, err );
            ch. update_step( i, std::move( step ));
         } 
         std::cout << "crashing:\n"; 
         seq. ugly( std::cout );
         throw std::runtime_error( "still no clue what to do of course" );
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

         nm. rewr_outermost( def ); 
         nm. normalize( );
         // nm. make_anf2( );
         std::cout << "expand returns " << nm << "\n";
         return nm. value( );
      }

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
   
   case prf_existselim:
      {
         std::vector< logic::vartype > assumptions;
         std::vector< logic::exact > exactnames;
            // The names that seq will give to the assumptions.
            // Note that the sequent data structure is rotten.

         while( exists. value( ). sel( ) == logic::op_kleene_exists )
         {
            auto ex = exists. value( ). view_quant( );

            exists. value( ) = ex. body( );
         }

         for( const auto& a : assumptions ) 
         {
            logic::exact name = seq. assume( a.pref, a.tp );
            exactnames. push_back( name );
         }

         {
            logic::fullsubst namesubst;
               // We need to substitute global names in place of
               // of the DeBruijn indices.

            for( const auto& e : exactnames )
               namesubst. push( logic::term( logic::op_exact, e ));

            exists. rewr_outermost( namesubst );
         }

         exists. make_anf2( );

         seq. assume( elim. name( ), exists. value( ));

         auto res = optform( proofcheck( elim. intro( ), seq, err ),
                             "generic result of exists-elim", seq, err );

         // We require that the result is a singleton conjunction:

         res. musthave( logic::op_kleene_and );
         res. getuniquesub( );
         res. musthave( logic::op_kleene_or );
         if( !res )
         {
            seq. restore( seqsize ); 
            return { };
         }

         logic::exactcounter eigenvars( false );
         for( size_t i = seqsize; i + 1 < seq. size( ); ++ i )
            eigenvars. addtodomain( seq. getexactname(i));

         count( eigenvars, res. value( ), 0 );

         logic::introsubst intro;

         std::vector< logic::vartype > usedassumptions;
            // The ones that are used in the result. They need
            // to be assumed.
         
         for( size_t i = 0; i != exactnames. size( ); ++ i )
         {
            if( eigenvars. at( exactnames[i] ))
            {
               intro. bind( exactnames[i] ); 
               usedassumptions. push_back( assumptions[i] );
            }
         }
         seq. restore( seqsize );
         
         // If no eigenvariable occurs in res, we can return res
         // unchanged:

         if( intro. size( ))
         {
            res. rewr_outermost( intro );
            res. quantify( usedassumptions ); 
         }
         return logic::term( logic::op_kleene_and, { res. value( ) } );
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

   case prf_forallelim:
      {
         auto elim = prf. view_forallelim( );
         auto anf = proofcheck( elim. parent( ), seq, err );
         if( !anf. has_value( ))
            return anf;

         auto forall = optform( std::move( anf ), "forall-elim", seq, err );
         forall. musthave( logic::op_kleene_and );
         forall. replacebysub( elim. nrforall( ));
         forall. musthave( logic::op_kleene_forall );
         forall. nrvarsmustbe( elim. size( )); 

         if( !forall ) return { };

         size_t errstart = err. size( );
         logic::fullsubst subst;

         auto q = forall. value( ). view_quant( ); 
         for( size_t i = 0; i != elim. size( ); ++ i )
         {
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
         }
        
         if( subst. size( ) < elim. size( ))
         {
            auto header = printing::makeheader( seq, "forall-elim" );
            err. addheader( errstart, std::move( header )); 
            return { };
         } 
         auto res = outermost( subst, std::move( q. body( )), 0 ); 
         res = logic::term( logic::op_kleene_and, { res } );
         return res; 
      }

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
 
   case prf_simplify:
      {
         auto simp = prf. view_simplify( );

         auto form = proofcheck( simp. parent( ), seq, err );

         if( !form. has_value( ))
            return form;

         clauseset set;
         set. insert( form. value( ));
         set. fully_simplify( );
         return set. getformula( );
      }

   case prf_fake:
      {
         auto res = optform( prf. view_fake( ). goal( ), "fake", seq, err );
         res. make_anf2( );
         res. fake( ); 
         return res. value( ); 
      }

   case prf_show:
      {
         auto show = prf. view_show( ); 
         auto res = proofcheck( show. prf( ), seq, err );
         for( short unsigned int i = 0; i < 70; ++ i )
            std::cout << '-';
         std::cout << "\n"; 
         std::cout << "this is " << show. comment( ) << ":\n";
         std::cout << seq << "\n";
         if( res. has_value( ))
         {
            std::cout << "Deduced subformula:\n";
            std::cout << "   ";
            printing::pretty( std::cout, seq, res. value( ));
         }
         else
            std::cout << "(proof failed)\n";
         std::cout << "\n\n";
         return res;
      } 
#endif
   }

   std::cout << prf. sel( ) << "\n";
   throw std::logic_error( "dont know how to check proof term" );
}



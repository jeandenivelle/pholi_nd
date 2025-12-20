

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





#include "localexpander.h"
#include "logic/replacements.h"


logic::term
calc::localexpander::operator( ) ( logic::term tm, size_t vardepth, bool& change )
{
   std::cout << "THERE ARE UNSOLVED ISSUES WITH LIFTING\n";

   if( tm. sel( ) == logic::op_debruijn )
   {
      size_t ind = tm. view_debruijn( ). index( );
      if( var + vardepth == ind )
      {
         std::cout << "that's an occurrence\n";
         if( i ++ == repl )
         {
            std::cout << "we replace it\n";
            change = true;
            return lift( value, vardepth );
         }
      }
   }

   return tm;
}


void calc::localexpander::print( std::ostream& out ) const
{
   out << "Local Expander " << i << "/" << repl;
   out << " :   #" << var << " := " << value;
}



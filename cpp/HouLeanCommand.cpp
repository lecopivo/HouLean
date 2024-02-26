/*
 * simple example of adding a 'date' command to Houdini
 */
#include <UT/UT_DSOVersion.h>
#include <CMD/CMD_Manager.h>
#include <CMD/CMD_Args.h>
#include <time.h>
/*
 * cmd_date()
  
 * callback function for the new 'date' command
 */
static void
cmd_date( CMD_Args &args )
{
    char        *time_string;
    time_t       time_struct;
    int          i;
    // the 's' option left pads with some spaces,
    // just for an example
    if( args.found('s') )
    {
        int num_spaces = args.iargp( 's' );
        for( i = num_spaces; i>0; i-- )
            args.out() << " ";
    }
    // call the standard C function 'time'.
    // see 'man time' for details
    time_struct = time(0);
    time_string = ctime( &time_struct );
    // printout the date to the args out stream
    args.out() << time_string;
}
/*
 * this function gets called once during Houdini initialization
 */
void
CMDextendLibrary( CMD_Manager *cman )
{
    // install the date command into the command manager
    //    name = "date", options_string = "s:",
    //    callback = cmd_date
    cman->installCommand("date", "s:", cmd_date);
}

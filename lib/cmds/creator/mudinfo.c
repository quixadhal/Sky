/**
 * Display various information about the mud's status.
 *
 * File taken from the VikingMUD lib.  Written by Kniggit 930525
 *
 * Hamlet added some stuff, May 1997.
 * Hamlet got annoyed and reworked the code, Jul 1997.
 * Sandoz got annoyed and made all this a couple of times faster, Sept 2002.
 */

inherit COMMAND_BASE;

#ifndef __COMPILER__
#define __COMPILER__     "probably gcc"
#define __OPTIMIZATION__ "bugged driver?"
#endif

#define WIDTH 15
#define PAD(x,y)      sprintf("%-*s: %s\n", WIDTH, x, ""+y )

/** @ignore yes */
int cmd() {
    mapping map;
    int up;

    up = uptime();
    map = rusage();

    write( PAD("Mud Name", sprintf("%s  (%s %i)", mud_name(),
               query_host_name(), __PORT__ ) )+
           PAD("Driver", __VERSION__ )+
           PAD("Architecture", __ARCH__ )+
           PAD("Compiler", __COMPILER__+"  ("+__OPTIMIZATION__+")")+
           PAD("Driver Uptime", time_string( up, 2 ) )+
           PAD("Next Reboot In", time_string(
               SHUTDOWN_H->query_shutdown() * 60, 2 ) )+
           PAD("Avg CPU Usage", sprintf("%4.2f%%",
               ( ( map["utime"] + map["stime"] ) / 10.0 ) / up ) )+
           PAD("Load Average", query_load_average() )+
           PAD("No. of Users", sizeof(users()) )+
           PAD("Named Livings", sizeof(named_livings()) )+
           PAD("Objects", sizeof(objects()) )+
           PAD("Call Outs", sizeof(call_out_info())+" pending.")+
           PAD("Memory Usage", memory_info() ) );

    return 1;

} /* cmd() */

/** @ignore yes */
mixed query_patterns() { return ({"", (: cmd() :) }); }

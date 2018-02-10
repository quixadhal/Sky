/**
 * Contains some simul_efuns.
 * @author Pinkfish
 */

/**
 * This method returns out the call stack traces.  It is usually
 * called in the error handling routines in the master object
 * but it can be used elsewhere for debugging purposes.  You
 * can see the results of this function in /log/runtime and
 * /log/catch.
 * @return call stack trace
 */
string back_trace() {
    string ret, *progs, *funcs;
    mixed obs;
    int i, sz;

    progs = map( call_stack(0), (: ( $1[<1] == '>' ? $1 : $1[0..<3] ) :) );
    obs = call_stack(1);
    funcs = call_stack(2);

    i = sz = sizeof(progs);

    ret = "Time: "+ctime(time())+"\n";

    while( i-- > 1 ) {
        mixed name;

        if( objectp(obs[i]) ) {
            if( !stringp( name = obs[i]->query_name() ) ) {
                if( sizeof( name ) && stringp( name[0] ) )
                    name = name[0];
                else name = 0;
            }
            obs[i] = file_name( obs[i] );
        } else {
            obs[i] = "null";
        }

        if( obs[i] == progs[i] )
            ret += sprintf("(%i) '%s' in %s\n",
                sz - i, funcs[i], progs[i] );
        else
            ret += sprintf("(%i) '%s' in %s (%s)\n",
                sz - i, funcs[i], obs[i], progs[i] );

        if( name )
            ret += sprintf("Object name: %O\n", name );

    }

    return ret;

} /* back_trace() */

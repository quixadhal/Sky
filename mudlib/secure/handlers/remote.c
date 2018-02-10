/* Remote Editor
 * Shaydz 1/3/02
 * Small bits of code, and idea from Descartes of Borg RCP
 */

#include <access.h>
#include <network.h>

#define PORT_RCP 7320;

inherit SERVER;

#include <remote.h>

private string Password;
private mapping Connections;
private string Directory, Home;
private int connected;

string eventWriteFile(int fd, string file, string str) {
    file = replace(file,"^M");
    file = replace(file,"\r");
    if (!master()->valid_write(file,Connections[fd]["username"] , "write_file"))
        return "50 Permission denied to write: " + file;
    if( file_size(file) > -1 && !rm(file) )
        return "50 Failed to write file: " + file;
    if( !write_file(file, str) ) return "50 Failed to write file: " + file;
    else {
        log_file("/secure/log/redit.log", "%s: %s uploaded %s\n",
             ctime(time()), Connections[fd]["username"],file);
        return "110 File " + file + " written.";
    }
} /* eventWriteFile() */

string eventReadFile(int fd, string file) {
    string tmp;

    if (!master()->valid_read(file,Connections[fd]["username"] , "read_file"))
        return "50 Permission denied to read: " + file;

    tmp = read_file(file);
    if( !(tmp = read_file(file)) ) return "";
    log_file("/secure/log/redit.log", "%s: %s downloaded %s\n",
             ctime(time()), Connections[fd]["username"],file);

    return tmp;

} /* eventReadFile() */

string eventCommand(int fd, string cmd, string arg) {
    string *files;
    object ob;
    string tmp;

    switch(cmd) {
    case "ls":

        if( !arg || arg == "" ) arg = Directory;
        if (!master()->valid_read(arg,Connections[fd]["username"] , "get_dir"))
            return 0;
        switch(file_size(arg)) {
        case -2:
            if( arg[<1] != '/' ) arg += "/";
            files = ({ arg });
            files += map(get_dir(arg),
              (: (file_size($(arg) + $1) == -2) ? $1 + "/" : $1 :));
            return implode(files, " ");
        case -1:
            return 0;
        default:
            return arg;
        }
    case "mkdir":
        arg = Directory+arg;
        if (!master()->valid_write(arg,Connections[fd]["username"] , "mkdir"))
            return "50 Permission denied to mkdir: " + arg;
        if( file_size(arg) != -1 ) return 0;
        if( !mkdir(arg) ) return 0;
        else return "Directory created.";
    case "cd":
        arg = Directory+arg;
        if( !arg || arg == "" ) Directory = Home;
        else if( !((int)master()->valid_read(arg, Connections[fd]["username"], "cd")) )
            return 0;
        else Directory = arg;
        return Directory;
    case "rm":
        arg = Directory+arg;
        if (!master()->valid_write(arg,Connections[fd]["username"] , "rm"))
            return "50 Permission denied to rm " + arg;
        if( !rm(arg) ) return 0;
        else return arg + " deleted.";
    case "rmdir":
        arg = Directory+arg;
        if (!master()->valid_write(arg,Connections[fd]["username"] , "rmdir"))
            return "50 Permission denied to rmdir " + arg;
        if( !rmdir(arg) ) return 0;
        else return arg + " deleted.";
    case "mv":
        files = explode(arg, " ");
        if( sizeof(files) != 2 ) return 0;
        files[0] = Directory+files[0];
        files[1] = Directory+ files[1];
        if (!master()->valid_read(files[0],Connections[fd]["username"] , "read_file"))
            return 0;
        if (!master()->valid_write(files[1],Connections[fd]["username"] , "write_file"))
            return 0;
        if( !rename(files[0], files[1]) ) return 0;
        else return files[0] + " renamed to " + files[1] + ".";
    case "update":
        arg = Directory+ arg;
        if( arg[<2..] == ".c" ) arg = arg[0..<3];
        if( ob = find_object(arg) ) {
            if( !((int)ob->eventDestruct()) )
                return "Failed to destruct original object.";
        }
        tmp = catch(call_other(arg, "???"));
        if( tmp )
            return "Error in loading object: " + replace_string(tmp, "\n", "\t");
        else return arg + ": successfully loaded.";
    }
    return 0;
} /* eventCommand() */

void create() {
    server::create();
    Connections = ([]);
    connected = 0;
    call_out( (: Setup :), 1);
} /* create() */

void Setup() {
    seteuid(geteuid(TO));
    if( eventCreateSocket(7320) < 0 ) {
        if( this_object() ) dest_me();
        return;
    }
} /* Setup() */

void eventSocketClosed(int fd) {
    map_delete(Connections, fd);
    connected=0;
} /* eventSocketClosed() */

void eventRead(int fd, string str) {
    if( !str ) {
        eventWrite(fd, "50 Invalid command.\n", 1);
        if( Connections[fd] ){
            map_delete(Connections, fd);
            connected = 0;
        }
        return;
    }
    eventProcess(fd, str);
} /* eventRead() */

private void eventProcess(int fd, string str) {
    string tmp, cmd, arg, file, val;
    int x;

    if( Connections[fd] && Connections[fd]["in edit"] > 0 ) {
        int len;

        Connections[fd]["buffer"] += str;
        if( !(len = strlen(Connections[fd]["buffer"])) ) return;
        if( len == Connections[fd]["in edit"] ) {
            tmp =  eventWriteFile(fd, Connections[fd]["file"],
              Connections[fd]["buffer"]);
            eventWrite(fd, tmp + "\n");
            Connections[fd]["in edit"] = 0;
            Connections[fd]["file"] = "";
            Connections[fd]["buffer"] = "";
            return;
        }
        else if( len > Connections[fd]["in edit"] ) {
            string tmp1, tmp2;

            tmp1 =Connections[fd]["buffer"][0..(Connections[fd]["in edit"]-1)];
            tmp2 =Connections[fd]["buffer"][Connections[fd]["in edit"]..];
            Connections[fd]["buffer"] = "";
            eventProcess(fd, tmp1);
            eventProcess(fd, tmp2);
            return;
        }
        return;
    }
    if( !Connections[fd] ) Connections[fd] = ([ "buffer" : "" ]);
    if( (Connections[fd]["buffer"] += str) == "" ) return;
    if( (x = strsrch(Connections[fd]["buffer"], "\n")) == -1 ) return;
    str = Connections[fd]["buffer"][0..(x-1)];
    if( x != strlen(Connections[fd]["buffer"]) - 1 )
        Connections[fd]["buffer"] = Connections[fd]["buffer"][(x+1)..];
    else Connections[fd]["buffer"] = "";
    if( sscanf(str, "%s %s", cmd, arg) != 2) {
        cmd = replace_string(str, "\r", "");
        arg = "";
    }
    else arg = replace_string(arg, "\r", "");
    if( !Connections[fd]["username"] ) {
        string username, *user, password;

        if( cmd != "login" ) {
            eventWrite(fd, "50 Must login with user name and password.\n", 1);
            map_delete(Connections, fd);
            connected =0;
            return;
        }
        if( sscanf(arg, "%s %s", username, password) != 2 ) {
            eventWrite(fd, "50 Login failed.\n", 1);
            map_delete(Connections, fd);
            connected=0;
            return;
        }
        user = explode(username," ")-({"login"});
        username = implode(user,"");
        if( !creatorp(username) ) {
            eventWrite(fd, "50 Login failed.\n", 1);
            map_delete(Connections, fd);
            connected=0;
            return;
        }
        if (!LOGIN_OBJ->test_password(username, password)) {
            log_file("remote", "Failed attempt to login as " + username
              + "\n");
            eventWrite(fd, "50 Login failed.\n", 1);
            connected=0;
            map_delete(Connections, fd);
            return;
        }
        Directory = "/w/"+username+"/";
        Connections[fd]["username"] = username;
        connected =1;
        event(users(),"inform",CAP(username)+" connected to redit","ftp");
        eventWrite(fd, "60 Connection to " + mud_name() + ".\n");
    }
    else if( cmd == "100" ) {
        int sz;

        if( sscanf(arg, "%d %s", sz, Connections[fd]["file"]) != 2 )
            eventWrite(fd, "50 Bad file send command.\n");
        else {
            if( !sz ) eventWrite(fd, "110 No changes sent or written.\n");
            else Connections[fd]["in edit"] = sz;
        }
    }
    else switch( cmd ) {
    case "edit":
        file = eventReadFile(fd, arg);

        if(sizeof(file)){
            if( file[<1] != '\n' ) file += "\n";
            if(file[0..1] == "50"){
                eventWrite(fd, file);
                break;
            }
        }
        if(!strlen(file)){
            eventWrite(fd, "50 File doesn't seem to exist.");
            break;
        }
        tmp = sprintf( "%-14s", "100 " + strlen(file));
        eventWrite(fd, tmp[0..15]);
        eventWrite(fd, file);

        break;
    case "ls":
        val = eventCommand(fd, cmd, arg);
        if( val ) eventWrite(fd, "500 " + val + "\n");
        else eventWrite(fd, "50 " +cmd+ " " +arg+ ": Permission denied.\n");
        break;
    case "update":
        val = eventCommand(fd, cmd, arg);
        if( val ) eventWrite(fd, "510 " + val + "\n");
        else eventWrite(fd, "50 Update attempt went off into nowhere.\n");
        break;
    default:
        val = eventCommand(fd, cmd, arg);
        if( val ) eventWrite(fd, "400 " + val + "\n");
        else eventWrite(fd, "50 "+cmd+" "+arg+": Command not supported.\n");
        break;
    }
    eventProcess(fd, "");
} /* eventProcess() */

string stats(){
    return sprintf("%O",Connections);
} /* stats() */

void clean_up(){
    return ;
} /* clean_up() */

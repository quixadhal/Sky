
inherit COMMAND_BASE;


int cmd( string str ) {
    string s;
    
    if(!TP) {
        return 1;
    }
    s = lower_case(str);
    s = replace(s,({"a","�","b","�","c","�","d","�","e","�","h","H","i","�","j","J",
                        "l","|","n","�","o","�","p","�","q","�","r","�","s","�","u","�",
                        "v","V","x","�","y","�"}));
    tell_object(TP,str +" in asciitext = "+s+"\n");
    return 1;
    
} /* cmd() */

int back( string str ) {
    string s;
    
    if(!TP) {
        return 1;
    }
    s = replace(str,({"�","a","�","b","�","c","�","d","�","e","H","h","�","i","J","j",
                        "|","l","�","n","�","o","�","p","�","q","�","r","�","s","�","u",
                        "V","v","�","x","�","y"}));
    tell_object(TP,str +" in plaintext = "+s+"\n");
    return 1;
    
} /* back() */

mixed *query_patterns() {
    return ({
      "<string>", (: cmd($4[0]) :),
      "back <string>", (: back($4[0]) :),

    });
} /* query_patterns() */

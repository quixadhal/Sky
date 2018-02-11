
inherit ERRORS_BASE;

#include <http.h>
#include <log.h>
#include <board.h>

class bugs {
  int bug_num;
  int bugs;
  string file;
  int changed;
  string status;
  int last_use;
}

#define BUG_NUM ((class bugs)www_globvars[user])->bug_num
#define BUGS ((class bugs)www_globvars[user])->bugs
#define FILE ((class bugs)www_globvars[user])->file
#define CHANGED ((class bugs)www_globvars[user])->changed
#define STATUS ((class bugs)www_globvars[user])->status
#define LAST_USE ((class bugs)www_globvars[user])->last_use

#define MAX_IDLE_TIME 60*60

private nosave mapping www_globvars;

protected string get_bug(int i, string user);
protected string menu(string user);
protected string startup(mapping args, string user);
protected void delete_bug(int n, string user, string status);
protected string do_exit(string user);
protected string do_idea(string user);
protected string do_reply(string user);
protected string do_comment(string user);

protected void standard_response(string type, string user);
private void www_forward_bug(string recipient, string user);
protected int send_mail(class http_request req);
protected string store_comment(class http_request req);
protected string do_index(string user);

protected void create() {
  seteuid(getuid());
  www_globvars = ([]);
  call_out("check_connections", 5 * 60);
}

private varargs string create_header(string title, string host) {
  return "<HTML>\n<HEAD>\n"
    "<TITLE>" + title + "</TITLE>\n" +
    (host ? "<META http-equiv=\"Refresh\" content=\"3600;URL=http://" + host +
     "/secure/errors.c?type=timeout\">" : "") +
    "</HEAD>\n<BODY bgcolor=\"#ffffff\" text=\"#000030\" link=\"#4a529c\" "
    "vlink=\"#b57339\">\n<FONT face=\"arial,helvetica\">\n"
    "</A>\n<H2>Errors</H2>\n"
    "<H3><I>Online Errors Tool.</I></H3>\n<BR>\n";
} /* create_header() */

private string create_footer(class http_request req) {
  return "</BODY>\n</HTML>\n";
}

private string remove_control_chars(string str) {
  int i;

  for (i = 0; i < strlen(str); i++) {
    /* This will also change the hard space back tyo a space. */
    if ((str[i] < ' '  && str[i] != '\n' && str[i] != '\r') ||
        str[i] == 160 ) {
      str[i] = ' ';
    }
  }
  return str;
} /* remove_control_chars() */

string htmlify_no_address(string str) {
  return replace(str, ({
    "&", "&amp;",
      "<", "&lt;",
      ">", "&gt;",
      "\n", "<BR>\n",
      /* " ", "&nbsp;", */
      }) );
}

/*
 * Makes all the '<' turn into $gt; and the '>' the same.  Turn
 * http addresses into real addresses...
 */
string htmlify(string  str) {
   string *bits;
   string start;
   string end;
   string extra;
   int i;

   str =  htmlify_no_address(str);
   if (strsrch(str, "http:") > 0) {
      bits = explode("#" + str, "http:");
      bits[0] = bits[0][1..];
      for (i = 1; i < sizeof(bits); i++) {
         if (sscanf(bits[i], "%s&nbsp;%s", start, end) == 2) {
            end = "&nbsp;" + end;
            if (sscanf(start, "%s<BR>%s", start, extra) == 2) {
                end = "<BR>" + extra + end;
            }
         } else if (sscanf(bits[i], "%s<BR>%s", start, end) == 2) {
            end = "<BR>" + end;
         } else {
            start = bits[i];
            end = "";
         }
         if (start[<1] == '.') {
            start = start[0..<2];
            end = "." + end;
         }
         bits[i] = "<A href=\"http:" + start + "\">http:" + start + "</A>" +
                     end;
      }
      str = implode(bits, "");
   }

   return str;
}

private void www_forward_bug(string recipient, string user) {
  string bug;
  mixed res;

  // Handle boards a little differently.
  if(member_array(recipient, BOARD_HAND->query_boards()) != -1) {
    res = get_row(user, BUG_NUM);

    if (stringp(res)) {
      return;
    }
    bug = res["Report"];

    if (res["Runtime"] && (res["Runtime"] != "")) {
      bug += "\n[RUNTIME]\n" + res["Runtime"];
    }

    if(!BOARD_HAND->add_message(bug, user, "Bug report", recipient))
      log_file("TEMP", "Failed to post to board.\n");
    return;
  }

  if( !::forward_bug( user, BUG_NUM, recipient ) ) {
      return;
  }
  BUGS--;

  if (BUG_NUM > BUGS)
    BUG_NUM--;
  CHANGED = 1;
}

string www_request(string, mapping args, class http_request req) {
  string ret, user;

  if(!req->user)
    return "Error!";

  user = req->user;

  if(!master()->valid_seteuid(this_object(), user))
    return create_header("Error handler") + "Can't set euid!" +
      create_footer(req);

  seteuid(user);

  if(!creatorp(user))
    return create_header("Error handler") + "You are not a creator!" +
      create_footer(req);

  if(!www_globvars[user]) {
    www_globvars[user] = new(class bugs);
    ret = startup(args, user);
    if(ret) {
      map_delete(www_globvars, user);
      return create_header("Error handler") + ret + create_footer(req);
    }
  }

  LAST_USE = time();

  switch(args["type"]) {
  case "mail":
    if (send_mail(req)) {
      delete_bug(BUG_NUM, user, upper_case(req->body->data["status"]));
    }
    ret = get_bug(BUG_NUM, user);
    break;
  case "comment":
    ret = store_comment(req);
    ret += get_bug(BUG_NUM, user);
    break;
  case "timeout":
    unguarded((: rm, WWW_TEMP_MAIL_LIST :));
    unguarded((: rm, WWW_TEMP_FORWARD_LIST :));
    finish_errors(user);
    map_delete(www_globvars, user);
    return create_header("Error handler: Timeout") +
      "<P>Timeout (1 hour), your changes have been discarded.\n"
      "<P><A href=\"errors.c\">Return to main menu</A>" +
      create_footer(req);
    break;
  default:
    switch(args["command"]) {
    case "Forward":
      if(!args["recipient"]) {
        ret = "You must either give the name of a creator, a board, or a "
          "directory.<BR>\n";
      } else {
        string recipient;
        recipient = args["recipient"];

        if( creatorp(recipient) ) {
           recipient = "/w/"+recipient;
        } else if (recipient == "unknown") {
          recipient = "/log";
        } else if(member_array(recipient, BOARD_HAND->query_boards()) != -1) {
          ;
        } else if (unguarded((: file_size, recipient :)) == -2) {
          ;
        } else {
          ret = "Dir does not exist, or you haven't got read access.<BR>\n";
          recipient = "";
        }

        if(recipient != "") {
          ret = "Bug forwarded.<BR>\n";
          www_forward_bug(recipient, user);
          ret += get_bug(BUG_NUM, user);
        }
      }
      break;
    case "Fixed":
      standard_response("d", user);
      delete_bug(BUG_NUM, user, "FIXED");
      ret = get_bug(BUG_NUM, user);
      break;
    case "No Error":
      standard_response("n", user);
      delete_bug(BUG_NUM, user, "DENIED");
      ret = get_bug(BUG_NUM, user);
      break;
    case "Thanks":
      standard_response("t", user);
      delete_bug(BUG_NUM, user, "CONSIDERING");
      ret = get_bug(BUG_NUM, user);
      break;
    case "Reply":
      return create_header("Error handler: Reply", req->hostname) + do_reply(user) +
        create_footer(req);
      break;
    case "Previous":
      BUG_NUM -= 1;
      ret = get_bug(BUG_NUM, user);
      break;
    case "Next":
      BUG_NUM += 1;
      ret = get_bug(BUG_NUM, user);
      break;
    case "Index":
      return create_header("Error handler", req->hostname) + do_index(user) + menu(user) +
        create_footer(req);
      break;
    case "Goto":
      BUG_NUM = to_int(args["number"]);;
      ret = get_bug(BUG_NUM, user);
      break;
    case "Save":
      return create_header("Error handler: Finished") + do_exit(user) +
        "<P><A href=\"errors.c\">return to main menu</A></P>" +
        create_footer(req);
      break;
    case "Done":
    case "No Save":
      unguarded((: rm, WWW_TEMP_MAIL_LIST :));
      unguarded((: rm, WWW_TEMP_FORWARD_LIST :));
      finish_errors(user);
      map_delete(www_globvars, user);
      return create_header("Error handler: Finished") +
        "<P>Finished <A href=\"errors.c\">return to main menu</A>" +
        create_footer(req);
      break;
    case "Idea":
      ret = do_idea(user);
      ret += get_bug(BUG_NUM, user);
      break;
    case "Comment":
      return create_header("Error handler: Comment", req->hostname) + do_comment(user) +
        create_footer(req);
      break;
    case "Help":
      ret = "<P>Explanation of the buttons and status:\n"
        "<TABLE>\n<TR>\n"
        "<TH align=\"left\">Status:\n"
        "<TD>The numbers between [] give the total range of reports, the "
        "number between () is the current report.\n"
        "<TR>\n"
        "<TH align=\"left\">Navigation:\n"
        "<TD>These buttons navigate through the reports. The \"Index\" button "
        "gives an index of all the select reports.\n"
        "<TR>\n"
        "<TH align=\"left\">Delete:\n"
        "<TD>The \"Fixed\" button marks a report as fixed and sends the "
        "reporter mail about this.\n"
        "<TR><TH>\n"
        "<TD>The \"No Error\" button marks a report as denied and "
        "sends the reporter mail about this.\n"
        "<TR><TH>\n"
        "<TD>The \"Thanks\" button marks a report as being "
        "considered by you and sends the reporter a \"thank you\" mail about "
        "this.\n"
        "<TR><TH>\n"
        "<TD>The \"Reply\" button allows you to type your own mail "
        "reply to a report and to mark it as fixed, denied, fixing or "
        "considering.\n"
        "<TR>\n"
        "<TH align=\"left\">Misc:\n"
        "<TD>The \"Forward\" button forwards a bug report to the person or "
        "directory set in the text entry field next to it.\n"
        "<TR><TH>\n"
        "<TD>The \"Idea\" button turns a BUG report into an IDEA "
        "report.\n"
        "<TR><TH>\n"
        "<TD>The \"Comment\" button allows you to attach a comment "
        "to a bug report, useful if you're forwarding a report or if you "
        "have some ideas about it but can't immediately fix or implement it.\n"
        "<TR><TH>\n"
        "<TD>The \"Help\" button gets you this help.\n"
        "<TR>\n"
        "<TH align=\"left\">Finish:\n"
        "<TD>The \"Save\" button saves the changes you made in this session, "
        "the \"No Save\" button discards them. You need to always select one "
        "of these buttons when you're done, otherwise you can't start a new "
        "session.\n"
        "</TABLE>\n";
      break;
    default:
      ret = get_bug(BUG_NUM, user);
    }
  }
  return create_header("Error handler", req->hostname) + ret + menu(user) + create_footer(req);
}

protected string menu(string user) {
  string ret;

  ret = "<HR>\n<FORM action=\"errors.c\" method=\"GET\">\n"
    "<TABLE>\n";

  if (STATUS) {
    ret += "<TR>\n<TH align=\"left\">Status:</TH>\n<TD>";
    if (BUGS > 0)
      ret += sprintf("[1-%d] (%d)", BUGS, BUG_NUM);
    else
      ret += "[No bugs]";

    ret += "</TD>\n";

    if (BUGS > 1) {
      ret += "<TR>\n<TH align=\"left\">Navigation:</TH>\n<TD>";
      if((BUG_NUM) > 1)
        ret += "<INPUT type=\"submit\" name=\"command\" value=\"Previous\">\n";
      ret += "<INPUT type=\"submit\" name=\"command\" value=\"Index\">\n";

      if((BUG_NUM) < (BUGS))
        ret += "<INPUT type=\"submit\" name=\"command\" value=\"Next\">\n";

      ret += "</TD>\n";
    }

    if (member_array(STATUS, ({ "DENIED", "FIXED" })) == -1) {
      ret += "<TR>\n<TH align=\"left\">Delete:</TH>\n<TD>"
        "<INPUT type=\"submit\" name=\"command\" value=\"Fixed\">\n"
        "<INPUT type=\"submit\" name=\"command\" value=\"No Error\">\n"
        "<INPUT type=\"submit\" name=\"command\" value=\"Thanks\">\n"
        "<INPUT type=\"submit\" name=\"command\" value=\"Reply\">\n"
        "</TD>\n"
        "<TR>\n<th align=\"left\">Misc:</TH>\n<TD>"
        "<INPUT type=\"submit\" name=\"command\" value=\"Forward\">\n"
        "<INPUT type=\"text\" name=\"recipient\" value=\"Noone\" size=13>\n"
        "<INPUT type=\"submit\" name=\"command\" value=\"Idea\">\n"
        "<INPUT type=\"submit\" name=\"command\" value=\"Comment\">\n"
        "<INPUT type=\"submit\" name=\"command\" value=\"Help\">\n"
        "</TD>\n";
    } else {
      ret += "<TR>\n<th align=\"left\">Misc:</TH>\n"
        "<TD><INPUT type=\"submit\" name=\"help\" value=\"Help\"></TD>\n";
    }
  }
  ret += "<TR>\n<TH align=\"left\">Finish:</TH>\n<TD>"
    "<INPUT type=\"submit\" name=\"command\" value=\"Save\">\n"
    "<INPUT type=\"submit\" name=\"command\" value=\"No Save\">\n"
    "</TD>\n";
  ret += "</TABLE>\n</FORM>\n";
  STATUS = 0;
  return ret;
}

protected string startup(mapping args, string user) {
  string str, ret, temp, query, type, filename, contains, excludes, status;
  string *types, fixer, reporter;
  mixed numrows;

  str = args["dir"];

  // No directory so give menu.
  if (!strlen(str)) {
    return "<FORM method=\"GET\" action=\"errors.c\">\n"
      "<TABLE>\n"
      "<TR>\n"
      "<TH align=\"left\">Enter directory:</TH>\n"
      "<TD><INPUT type=\"text\" name=\"dir\"> \n"
      "<INPUT type=\"checkbox\" name=\"recurse\" value=\"yes\"> Subdirs?\n"
      "<INPUT type=\"submit\" value=\"Enter\"></TD>\n"
      "<TR>\n"
      "<TH align=\"left\">Select report status:</TH>\n"
      "<TD><INPUT type=\"checkbox\" name=\"status\" value=\"All\"> All\n"
      "<INPUT type=\"checkbox\" name=\"open\" value=\"yes\" checked>"
      " Open\n"
      "<INPUT type=\"checkbox\" name=\"fixing\" value=\"yes\" checked> Fixing\n"
      "<INPUT type=\"checkbox\" name=\"considering\" value=\"yes\" checked>"
      " Considering\n"
      "<INPUT type=\"checkbox\" name=\"fixed\" value=\"yes\"> Fixed\n"
      "<INPUT type=\"checkbox\" name=\"denied\" value=\"yes\"> Denied\n"
      "</TD>\n"
      "<TR>\n"
      "<TH align=\"left\">Select report type:</TH>\n"
      "<TD><INPUT type=\"checkbox\" name=\"type\" value=\"All\" checked> All\n"
      "<INPUT type=\"checkbox\" name=\"bug\" value=\"yes\"> Bug\n"
      "<INPUT type=\"checkbox\" name=\"typo\" value=\"yes\"> Typo\n"
      "<INPUT type=\"checkbox\" name=\"idea\" value=\"yes\"> Idea\n"
      "</TD>\n"
      "<TR>\n"
      "<TH align=\"left\">Only show reports reported by:</TH>\n"
      "<TD><INPUT type=\"text\" name=\"reporter\"></TD>"
      "<TR>\n"
      "<TH align=\"left\">Only show reports fixed by:</TH>\n"
      "<TD><INPUT type=\"text\" name=\"fixer\"></TD>"
      "<TR>\n"
      "<TH align=\"left\">Only show reports about:</TH>\n"
      "<TD><INPUT type=\"text\" name=\"filename\"> "
      "(Example: /cmds/errors_base)</TD>\n"
      "<TR>\n"
      "<TH align=\"left\">Only show reports containing:</TH>\n"
      "<TD><INPUT type=\"text\" name=\"contains\"> \n"
      "<INPUT type=\"checkbox\" name=\"cregexp\" value=\"yes\"> Regexp?\n"
      "</TD>\n"
      "<TR>\n"
      "<TH align=\"left\">Don't show reports containing:</TH>\n"
      "<TD><INPUT type=\"text\" name=\"excludes\"> \n"
      "<INPUT type=\"checkbox\" name=\"eregexp\" value=\"yes\"> Regexp?\n"
      "</TD>\n"
      "</TABLE>\n"
      "</FORM>\n";
  }

  status = args["status"];
  reporter = args["reporter"];
  fixer = args["fixer"];
  filename = args["filename"];
  contains = args["contains"];
  excludes = args["excludes"];
  type = args["type"];

  if (str != "/" && str[<1] == '/') {
    str = str[0..<2];
  }
  FILE = str + "/" + ERROR_LOG;

  if ((temp = init_errors(user, "errors", 1))) {
    map_delete(www_globvars, user);
    return sprintf("Error connecting to the errors database: %s<BR>\n", temp);
  }
  query = "SELECT Id, EntryDate, Filename, Category, Type, Name, Reporter, "
    "Report, Runtime, Fixer, FixDate, Status, Directory FROM errors WHERE "
    "(Directory = '" + str + "'";

  if (args["recurse"]) {
    if (str == "/") {
      str = "";
    }
    query += " OR Directory LIKE '" + str + "/%') ";
  } else {
    query += ") ";
  }

  if (status != "All") {
    types = keys(args) & ({ "open", "fixed", "fixing", "considering",
                            "denied" });
    if (sizeof(types)) {
      status = implode(types, (: $1 + ", '" + upper_case($2) + "'" :), "");
      status = status[2..];
      query += "AND Status IN (" + status + ") ";
    }
  }

  if (type != "All") {
    types = keys(args) & ({ "bug", "idea", "typo" });
    if (sizeof(types)) {
      type = implode(types, (: $1 + ", '" + upper_case($2) + "'" :), "");
      type = type[2..];
      query += "AND Type IN (" + type + ") ";
    }
  }

  if (strlen(reporter)) {
    query += "AND Reporter = '" + reporter + "' ";
  }

  if (strlen(fixer)) {
    query += "AND Fixer = '" + fixer + "' ";
  }

  if (strlen(filename)) {
    if (filename[<2..] == ".c") {
      filename = filename[0..<3];
    }
    query += "AND Filename = '" + filename + "' ";
  }

  if (strlen(contains)) {
    if (args["cregexp"]) {
      query += "AND (Report REGEXP '" + contains + "' OR Runtime REGEXP '" +
        contains + "') ";
    } else {
      query += "AND (Report LIKE '%" + contains + "%' OR Runtime LIKE '%" +
        contains + "%') ";
    }
  }

  if (strlen(excludes)) {
    if (args["eregexp"]) {
      query += "AND (Report NOT REGEXP '" + excludes + "' AND Runtime NOT REGEXP '" +
        excludes + "') ";
    } else {
      query += "AND (Report NOT LIKE '%" + excludes + "%' AND Runtime NOT LIKE '%" +
        excludes + "%') ";
    }
  }
  query += "ORDER BY Directory, DirEntryDate;";
  //  tell_creator("ceres", query);
  numrows = sql_query(user, query);

  if (stringp(numrows)) {
    finish_errors(user);
    map_delete(www_globvars, user);
    return sprintf("Error fetching reports in %s : %s<BR>\n", str, numrows);
  }

  if (!numrows) {
    ret = "No reports found matching your criteria.<BR>\n";
    finish_errors(user);
    map_delete(www_globvars, user);
  } else {
    unguarded((: rm, WWW_TEMP_MAIL_LIST :));
    unguarded((: rm, WWW_TEMP_FORWARD_LIST :));
    BUGS = numrows;
    BUG_NUM = 1;
    ret = 0;
  }
  return ret;
}

protected string get_bug(int i, string user) {
  string bug, ret, title, filename, name, fixer, status, directory;
  mixed res, forwards, comments;
  int rtime, ftime;

  if (BUGS == 0 || i > BUGS)
    return "No more reports.<BR>\n";
  res = get_row(user, i);

  if (stringp(res)) {
    return sprintf("Error getting report %d : %s<BR>\n", i, res);
  }
  title = sprintf("%s %s %s", res["Category"], res["Type"], res["Name"]);
  filename = res["Filename"];
  name = res["Reporter"];
  bug = res["Report"];
  rtime = res["EntryDate"];
  fixer = res["Fixer"];
  ftime = res["FixDate"];
  status = STATUS = res["Status"];
  directory = res["Directory"];

  if (res["Runtime"] && (res["Runtime"] != "")) {
    bug += "\n[RUNTIME]\n" + res["Runtime"];
  }
  forwards = get_forwards(user, i);
  if (stringp(forwards)) {
    return sprintf("Error getting forward info for report %d : %s<BR>\n", i,
                   forwards);
  }
  comments = get_comments(user, i);
  if (stringp(comments)) {
    return sprintf("Error getting comments for report %d : %s<BR>\n", i,
                   comments);
  }
  ret = sprintf("<H3>%s</H3>\n", title) +
    "<TABLE>\n" +
    "<TR>\n<TH align=\"left\">Directory</TH>\n"
    "<TD>" + directory + "</TD>\n"
    "<TR>\n<TH align=\"left\">File</TH>\n"
    "<TD><A href=\"/secure/source_browser.c?file=" +
    replace_string(filename, "/", "%2F") + "\">"  + filename + "</A> "
    "<A href=\"edit.c?file="+ replace_string(filename, "/", "%2F") +
    ".c\">[Edit]</A></TD>";

  if (sizeof(forwards)) {
    string *fdirs = forwards["OldDirectory"], *fnames = forwards["Forwarder"];
    int size, *ftimes = forwards["ForwardDate"];

    size = sizeof(fdirs);
    for (i = 0; i < size; i++) {
      ret += "<TR>\n<TH align=\"left\">Forwarded by</TH>\n"
        "<TD><A href=\"/secure/finger.c?player="+
        fnames[i] + "\">" + capitalize(fnames[i]) + "</A></TD>\n";
      if (fdirs[i] != "") {
        ret += "<TR>\n<TH align=\"left\">from</TH>\n<TD>" + fdirs[i] +
          "</TD>\n";
      }
      ret += "<TR>\n<TH align=\"left\">at</TH>\n<TD>" + ctime(ftimes[i]) +
        "</TD>\n";
    }
  }
  if (strlen(fixer)) {
    ret += "<TR>\n<TH align=\"left\">Fixed by</TH>\n"
      "<TD><A href=\"/secure/finger.c?player=" + fixer + "\">" +
      capitalize(fixer) + "</A></TD>\n"
      "<TR>\n<TH align=\"left\">at</TH>\n<TD>" + ctime(ftime) + "</TD>\n";
  }

  ret += "<TR>\n<TH align=\"left\">Made by</TH>\n"
    "<TD><A href=\"/secure/finger.c?player=" + name + "\">" +
    capitalize(name) + "</A></TD>\n"
    "<TR>\n<TH align=\"left\">at</TH>\n<TD>" + ctime(rtime) + "</TD>\n"
    "<TR>\n<TH align=\"left\">Status</TH>\n<TD>" + status + "</TD></TABLE>\n";

  ret += "<p><PRE>" + sprintf("%-=80s", HTTPD->escape_html(bug)) + "</PRE>\n";

  if (sizeof(comments)) {
    string *ctexts = comments["Comment"], *cnames = comments["Commenter"];
    int size, *ctimes = comments["CommentDate"];

    size = sizeof(ctexts);
    for (i = 0; i < size; i++) {
      ret += "<TABLE>\n<TR>\n<TR>\n<TH align=\"left\">Comment by</TH>\n"
        "<TD><A href=\"/secure/finger.c?player=" + cnames[i] + "\">" +
        capitalize(cnames[i]) + "</A></TD>\n"
        "<TR>\n<TH align=\"left\">at</TH>\n<TD>" + ctime(ctimes[i]) +
        "</TD>\n</TABLE>\n"
        "<P><PRE>" + HTTPD->escape_html(ctexts[i]) + "</PRE>\n";
    }
  }
  return ret;
}

string do_reply(string user) {
  string bug, ret;
  mixed res;

  res = get_row(user, BUG_NUM);

  if (stringp(res)) {
    return sprintf("Error getting report %d : %s\n", BUG_NUM, res);
  }
  bug = res["Report"];
  ret = "<H3>Reply to a report</H3>\n"
    "<FORM method=\"POST\" action=\"errors.c?type=mail\">\n"
    "<TABLE>\n<TR>\n<TH align=\"left\">Set report status:</TH>\n"
    "<TD><SELECT name=\"status\">\n"
    "<OPTION value=\"Fixed\" selected>Fixed\n"
    "<OPTION value=\"Denied\">Denied\n"
    "<OPTION value=\"Fixing\">Fixing\n"
    "<OPTION value=\"Considering\">Considering\n"
    "</SELECT></td>\n"
    "</TABLE>\n"
    "<FONT face=monospace>\n"
    "<TEXTAREA name=\"body\" rows=\"20\" cols=\"70\" wrap=\"hard\">\n";
  ret += "> " + replace_string(HTTPD->escape_html(bug), "\n", "\n> ");
  ret += strip_colours(HTTPD->escape_html(PLAYER_H->query_signature(user)));
  ret += "</TEXTAREA><BR>\n</FONT>\n"
    "<INPUT type=\"submit\" value=\"Send\">\n"
    "</FORM>\n";
  return ret;
}

string do_comment(string user) {
  string ret;

  ret = get_bug(BUG_NUM, user) +
    "<HR>Enter your comment here:\n"
    "<FORM method=\"POST\" action=\"errors.c?type=comment\">\n"
    "<FONT face=monospace>\n"
    "<TEXTAREA name=\"body\" rows=\"20\" cols=\"70\" wrap=\"hard\">\n"
    "</TEXTAREA><BR>\n</FONT>\n"
    "<INPUT type=submit value=\"Send\">\n"
    "</FORM>\n";
  return ret;
}

protected void standard_response(string type, string user) {
  string name, bug;
  int rtime;
  mixed res;

  res = get_row(user, BUG_NUM);

  if (stringp(res)) {
    return;
  }
  name = res["Reporter"];
  bug = res["Report"];
  rtime = res["EntryDate"];

  switch(type) {
  case "t":
    bug = "Thank you for the following report, it has been noted.\n\n" +
      bug + "\n";
    break;
  case "n":
    if(res["Type"] == "IDEA")
      bug = "Thank you for the following report, it has been noted.\n\n" +
        bug + "\n";
    else
      bug = "The following report by you is not a bug.\n\n" + bug +
        "\nThank you for reporting it anyway though.\n";
    break;
  case "d":
  default:
    if(res["Type"] == "IDEA")
      bug = "The following idea has now been implemented.\n\n"+ bug +
        "\nThank you for suggesting it.\n";
    else
      bug = "The following bug has now been fixed.\n\n"+ bug +
        "\nThank you for reporting it.\n";
  }

  bug = bug + "\nThis was an automatic mail\n";

  unguarded((: write_file, WWW_TEMP_MAIL_LIST, name + " " + rtime + " " +
               bug + "END" :));
}

protected void delete_bug(int n, string user, string status) {
  if (!set_status(user, n, status)) {
    return;
  }
  BUGS--;

  if (BUG_NUM > BUGS)
    BUG_NUM--;
  CHANGED = 1;
}

protected string do_exit(string user) {
  string name, rtime, bug, ret, trash, filename;

  ret = "";
  filename = FILE;
  TCRE("shaydz",sprintf("filename:%O",filename));
  if (CHANGED && (unguarded((: master()->valid_write($(filename), $(user), "rm") :))||!file_exists(FILE))) {
    ret += "\n<UL>\n";
    trash = save_changes(user, user);
    if (trash) {
      ret += sprintf("<LI>Error saving (some) changes : %s<BR>\n", trash);
    } else {
      trash = read_file(WWW_TEMP_MAIL_LIST);

      if (trash) {
        while(sscanf(trash, "%s %s %sEND%s", name, rtime, bug, trash) == 4) {
          if( !rank(name) )
              ret += "<LI>No such user " + name + "<BR>\n";
          else {
             MAIL_H->do_mail_message(name, user, "Bug Report", "", bug, 0, 0);
             ret += "<LI>Sending mail to " + name + "<BR>\n";
          }
        }
      }
    }
    unguarded((: rm, WWW_TEMP_MAIL_LIST :));
    unguarded((: rm, WWW_TEMP_FORWARD_LIST :));
    ret += "</UL>\nDone.<BR>\n";
  } else if (CHANGED) {
    ret += "You don't have write permission to this directory.<BR>\n";
  } else {
    ret += "No changes need saving.<BR>\n";
  }
  finish_errors(user);
  map_delete(www_globvars, user);
  return ret;
}

protected string do_idea(string user) {
  string ret;
  mixed row;

  row = get_row(user, BUG_NUM);
  if (stringp(row)) {
    return sprintf("Error fetching bug report %d : %s<BR>\n", row);
  }
  if (row["Type"] == "IDEA") {
    ret = "This report is already an idea.<BR>\n";
  }
  else  {
    set_type(user, BUG_NUM, "IDEA");
    row["Type"] = "IDEA";
    CHANGED = 1;
    ret = "Report has been changed to an idea.<BR>\n";
  }
  return ret;
}

protected string do_index(string user) {
  string ret;
  int i, size;
  mixed res;

  ret = "<TABLE>\n<TR>\n"
    "<TH align=\"left\">Number</TH>\n"
    "<TH align=\"left\">Date</TH>\n"
    "<TH align=\"left\">Type</TH>\n"
    "<TH align=\"left\">Reporter</TH>\n"
    "<TH align=\"left\">Filename</TH>\n"
    "<TH align=\"left\">Directory</TH>\n";
  size = BUGS;
  if (size > 500) {
    return "Too many reports for index.<BR>\n";
  }
  for (i = 1; i <= size; i++) {
    res = get_row(user, i);

    if (stringp(res)) {
      return sprintf("Error getting report %d : %s<BR>\n", i, res);
    }
    ret += "<TR>\n<TD>" + i + "</TD>\n"
      "<TD>" + ctime(res["EntryDate"]) + "</TD>\n"
      "<TD><A href=\"errors.c?command=Goto&number=" + i + "\">" +
      sprintf("%s %s %s", res["Category"], res["Type"], res["Name"]) +
      "</A></TD>\n"
      "<TD>"+ res["Reporter"] + "</TD>\n<TD>" + res["Filename"] +
      "</TD>\n<TD>" + res["Directory"] + "</TD>\n";
  }
  ret += "</TABLE>\n";
  return ret;
}

protected int send_mail(class http_request req) {
  string user, bug;
  mixed res;

  user = req->user;
  res = get_row(user, BUG_NUM);

  if (stringp(res)) {
    return 0;
  }
  bug = remove_control_chars(strip_colours(req->body->data["body"]));
  if (!strlen(bug)) {
    return 0;
  }
  bug = sprintf("%-=78s", bug);
  unguarded((: write_file, WWW_TEMP_MAIL_LIST, res["Reporter"] + " " +
               res["EntryDate"] + " " + bug + "END" :));
  return 1;
}

protected string store_comment(class http_request req) {
  string user, comment, ret;

  user = req->user;
  comment = remove_control_chars(strip_colours(req->body->data["body"]));
  if (!strlen(comment)) {
    return "No comment given.<BR>\n";
  }
  comment = sprintf("%-=78s", comment);
  ret = comment_bug(user, BUG_NUM, user, comment);
  if (ret) {
    return sprintf("Error adding comment: %s<BR>\n", ret);
  }
  return "Comment added.<BR>\n";
}

void dest_me() {
  string user;

  if (sizeof(www_globvars)) {
    seteuid(getuid());
    foreach (user in keys(www_globvars)) {
      finish_errors(user);
      unguarded((: rm, WWW_TEMP_MAIL_LIST :));
      unguarded((: rm, WWW_TEMP_FORWARD_LIST :));
    }
  }
  destruct(this_object());
}

int clean_up(int) {
  if (sizeof(www_globvars)) {
    return 1;
  }
  dest_me();
}

protected void check_connections() {
  class bugs sess;
  string user, old_euid;

  if (sizeof(www_globvars)) {
    old_euid = geteuid();
    seteuid(getuid());
    foreach (user, sess in www_globvars) {
      if ((sess->last_use + MAX_IDLE_TIME) <= time()) {
        finish_errors(user);
        unguarded((: rm, WWW_TEMP_MAIL_LIST :));
        unguarded((: rm, WWW_TEMP_FORWARD_LIST :));
        map_delete(www_globvars, user);
      }
    }
    seteuid(old_euid);
  }
  call_out("check_connections", 5 * 60);
}

mixed *stats() {
  return ({
    ({ "users", sizeof(www_globvars) ? implode(keys(www_globvars), ", ") : "nobody" })
  });
}

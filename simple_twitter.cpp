#include "server_type.h"
#include "simulation.h"

using namespace std;

int main(int argc, char *argv[])
{
    try
    {
        detect_argv_num(argc);
        user_number(argv[1]);
        detect_logfile(argv[2]);
        initial_userlist(user_number(argv[1]), argv[1]);
    }
    catch (Exception_t &exception)
    {
        cout << exception.error_info;
        return 0;
    }
    unsigned int number = user_number(argv[1]);
    User_t *users = initial_userlist(number, argv[1]);
    unsigned int tag_total_count = 0;
    Tag_t *tags = initial_tag(users, number, &tag_total_count);
    // int commands_num = detect_logfile(argv[2]);
    ifstream logfile;
    string buffer;
    logfile.open(argv[2]);
    while (logfile)
    {
        getline(logfile, buffer);
        string command[5];
        int count = 0;
        string word;
        stringstream iss(buffer);
        while (iss >> word)
        {
            command[count] = word;
            count++;
        }
        if (command[0] == "trending")
        {
            cout << ">> trending" << endl;
            trending(tags, atoi(command[1].c_str()), &tag_total_count);
        }
        else if (command[1] == "visit")
        {
            cout << ">> visit" << endl;
            visit(command[0], command[2], users, number);
        }
        else if (command[1] == "follow")
        {
            cout << ">> follow" << endl;
            follow(command[0], command[2], users, number);
        }
        else if (command[1] == "unfollow")
        {
            cout << ">> unfollow" << endl;
            unfollow(command[0], command[2], users, number);
        }
        else if (command[1] == "refresh")
        {
            cout << ">> refresh" << endl;
            refresh(command[0], users, number);
        }
        else if (command[1] == "like")
        {
            cout << ">> like" << endl;
            like(command[0], command[2], atoi(command[3].c_str()), users, number, tags, &tag_total_count);
        }
        else if (command[1] == "unlike")
        {
            cout << ">> unlike" << endl;
            unlike(command[0], command[2], atoi(command[3].c_str()), users, number, tags, &tag_total_count);
        }
        else if (command[1] == "comment")
        {
            cout << ">> comment" << endl;
            string comment_text;
            getline(logfile, comment_text);
            comment(command[0], command[2], atoi(command[3].c_str()), users, comment_text, number, tags, &tag_total_count);
        }
        else if (command[1] == "uncomment")
        {
            cout << ">> uncomment" << endl;
            uncomment(command[0], command[2], atoi(command[3].c_str()), users, atoi(command[4].c_str()), number, tags, &tag_total_count);
        }
        else if (command[1] == "post")
        {
            cout << ">> post" << endl;
            string title;
            getline(logfile, title);
            string tag;
            unsigned int tag_count = 0;
            string tags_list[MAX_TAGS];
            getline(logfile, tag);
            while (*tag.c_str() == '#')
            {
                string tagname = tag.substr(1, strlen(tag.c_str()) - 2);
                tags_list[tag_count] = tagname;
                tag_count++;
                getline(logfile, tag);
            }
            string text = tag;
            post(command[0], title, tags_list, text, tag_count, users, number, tags, &tag_total_count);
        }
        else if (command[1] == "delete")
        {
            cout << ">> delete" << endl;
            unpost(command[0], atoi(command[2].c_str()), users, number, tags, &tag_total_count);
        }
    }
    return 0;
}

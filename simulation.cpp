/*
 * This is VE280 Project 2, SU2020.
 * Written by Ziqiao Ma and Zhuoer Zhu.
 * Latest Update: 5/29/2020.
 * All rights reserved.
 */

#include "simulation.h"

// TODO: Define your functions in this header file.

/* Helper Functions */

// Printing

void printUser(User_t user, const string relationship)
{
    cout << user.username << endl;
    cout << relationship << endl;
    cout << "Followers: " << user.num_followers
         << "\nFollowing: " << user.num_following << endl;
}

void printPost(Post_t post)
{
    cout << post.owner << endl;
    cout << post.title << endl;
    cout << post.text << endl;
    cout << "Tags: ";
    for (unsigned int i = 0; i < post.num_tags; ++i)
    {
        cout << post.tags[i] << " ";
    }
    cout << "\nLikes: " << post.num_likes << endl;
    if (post.num_comments > 0)
    {
        cout << "Comments:" << endl;
        for (unsigned int i = 0; i < post.num_comments; ++i)
        {
            cout << post.comments[i].user << ": "
                 << post.comments[i].text << endl;
        }
    }
    cout << "- - - - - - - - - - - - - - -" << endl;
}

void printTag(const Tag_t *tag, unsigned int rank)
{
    cout << rank << " " << tag->tag_content << ": " << tag->tag_score << endl;
}

void detect_argv_num(int argc)
{
    if (argc < 3)
    {
        ostringstream oStream;
        oStream << "Error: Wrong number of arguments!" << endl;
        oStream << "Usage: ./p2 <username> <logfile>" << endl;
        throw Exception_t(INVALID_ARGUMENT, oStream.str());
    };
}

unsigned int user_number(char *argv)
{
    unsigned int lines = 0;
    ifstream username;
    string buffer;
    username.open(argv);
    if (!username)
    {
        ostringstream oStream;
        oStream << "Error: Cannot open file " << argv << "!" << endl;
        throw Exception_t(FILE_MISSING, oStream.str());
    }
    getline(username, buffer);
    while (username)
    {
        getline(username, buffer);
        lines++;
    }
    username.close();
    if (lines > MAX_USERS)
    {
        ostringstream oStream;
        oStream << "Error: Too many users!" << endl;
        oStream << "Maximal number of users is " << MAX_USERS << "." << endl;
        throw Exception_t(CAPACITY_OVERFLOW, oStream.str());
    }
    return lines - 1;
}

int detect_logfile(char *argv)
{
    unsigned int commands = 0;
    ifstream logfile;
    string buffer;
    logfile.open(argv);
    if (!logfile)
    {
        ostringstream oStream;
        oStream << "Error: Cannot open file " << argv << "!" << endl;
        throw Exception_t(FILE_MISSING, oStream.str());
    }
    getline(logfile, buffer);
    while (logfile)
    {
        commands++;
        getline(logfile, buffer);
    }
    logfile.close();
    return commands;
}

Post_t *initial_posts(const string username, unsigned int count, char *users_name)
{
    ifstream usernames;
    string file;
    usernames.open(users_name);
    getline(usernames, file);
    usernames.close();
    Post_t *posts;
    posts = new Post_t[MAX_POSTS];
    if (count == 0)
    {
        return posts;
    }
    else
    {
        for (unsigned int i = 1; i < count + 1; i++)
        {
            (posts + i - 1)->owner = username;
            int tag_count = 0;
            ifstream post;
            string buffer;
            post.open(file + "/" + username + "/posts/" + to_string(i));
            if (!post)
            {
                ostringstream oStream;
                oStream << "Error: Cannot open file " << file << "/" << username << "/posts/" << to_string(i)
                        << "!" << endl;
                throw Exception_t(FILE_MISSING, oStream.str());
            }
            getline(post, buffer);
            (posts + i - 1)->title = buffer;
            getline(post, buffer);
            while (*buffer.c_str() == '#')
            {
                (posts + i - 1)->tags[tag_count] = buffer.substr(1, strlen(buffer.c_str()) - 2);
                tag_count++;
                getline(post, buffer);
            }
            (posts + i - 1)->num_tags = tag_count;
            if ((posts + i - 1)->num_tags > MAX_TAGS)
            {
                ostringstream oStream;
                oStream << "Error: Too many tags for post " << (posts + i - 1)->title << "!" << endl;
                oStream << "Maximal number of tags is " << MAX_TAGS << "." << endl;
                throw Exception_t(CAPACITY_OVERFLOW, oStream.str());
            }
            (posts + i - 1)->text = buffer;
            getline(post, buffer);
            (posts + i - 1)->num_likes = atoi(buffer.c_str());
            if ((posts + i - 1)->num_likes > MAX_LIKES)
            {
                ostringstream oStream;
                oStream << "Error: Too many likes for post " << (posts + i - 1)->title << "!" << endl;
                oStream << "Maximal number of likes is " << MAX_LIKES << "." << endl;
                throw Exception_t(CAPACITY_OVERFLOW, oStream.str());
            }
            if ((posts + i - 1)->num_likes > 0)
            {
                for (unsigned int j = 0; j < (posts + i - 1)->num_likes; j++)
                {
                    getline(post, buffer);
                    (posts + i - 1)->like_users[j] = buffer;
                }
            }
            getline(post, buffer);
            (posts + i - 1)->num_comments = atoi(buffer.c_str());
            if ((posts + i - 1)->num_comments > MAX_COMMENTS)
            {
                ostringstream oStream;
                oStream << "Error: Too many comments for post " << (posts + i - 1)->title << "!" << endl;
                oStream << "Maximal number of comments is " << MAX_COMMENTS << "." << endl;
                throw Exception_t(CAPACITY_OVERFLOW, oStream.str());
            }
            if ((posts + i - 1)->num_comments > 0)
            {
                for (unsigned int j = 0; j < (posts + i - 1)->num_comments; j++)
                {
                    getline(post, buffer);
                    (posts + i - 1)->comments[j].user = buffer;
                    getline(post, buffer);
                    (posts + i - 1)->comments[j].text = buffer;
                }
            }
            post.close();
        }
    }
    return posts;
}

User_t *initial_userlist(unsigned int lines, char *users_name)
{
    ifstream username;
    string file;
    username.open(users_name);
    getline(username, file);
    username.close();
    string buffer;
    username.open(users_name);
    User_t *list = new User_t[lines];
    int num = 0;
    string name;
    getline(username, name);
    while (username)
    {
        if (name != file)
        {
            (list + num)->username = name;
            num++;
        }
        getline(username, name);
    }
    username.close();
    for (int i = 0; i < num; i++)
    {
        ifstream user;
        user.open(file + "/" + (list + i)->username + "/user_info");
        if (!user)
        {
            ostringstream oStream;
            oStream << "Error: Cannot open file " << file << "/" << (list + num - 1)->username << "/user_info"
                    << "!" << endl;
            throw Exception_t(FILE_MISSING, oStream.str());
        }
        getline(user, buffer);
        (list + i)->num_posts = atoi(buffer.c_str());
        if ((list + i)->num_posts > MAX_POSTS)
        {
            ostringstream oStream;
            oStream << "Error: Too many posts for user " << (list + i)->username << "!" << endl;
            oStream << "Maximal number of posts is " << MAX_POSTS << "." << endl;
            throw Exception_t(CAPACITY_OVERFLOW, oStream.str());
        }
        (list + i)->posts = initial_posts((list + i)->username, (list + i)->num_posts, users_name);
        getline(user, buffer);
        (list + i)->num_following = atoi(buffer.c_str());
        if ((list + i)->num_following > MAX_FOLLOWING)
        {
            ostringstream oStream;
            oStream << "Error: Too many followings for user " << (list + i)->username << "!" << endl;
            oStream << "Maximal number of followings is " << MAX_FOLLOWING << "." << endl;
            throw Exception_t(CAPACITY_OVERFLOW, oStream.str());
        }
        if ((list + i)->num_following > 0)
        {
            for (unsigned int j = 0; j < (list + i)->num_following; j++)
            {
                getline(user, buffer);
                (list + i)->following[j] = buffer;
            }
        }
        getline(user, buffer);
        (list + i)->num_followers = atoi(buffer.c_str());
        if ((list + i)->num_followers > MAX_FOLLOWERS)
        {
            ostringstream oStream;
            oStream << "Error: Too many followers for user " << (list + i)->username << "!" << endl;
            oStream << "Maximal number of followers is " << MAX_FOLLOWERS << "." << endl;
            throw Exception_t(CAPACITY_OVERFLOW, oStream.str());
        }
        if ((list + i)->num_followers > 0)
        {
            for (unsigned int j = 0; j < (list + i)->num_followers; j++)
            {
                getline(user, buffer);
                (list + i)->follower[j] = buffer;
            }
        }
    }
    return list;
}

Tag_t *initial_tag(User_t *list, unsigned int lines, unsigned int *tag_count)
{
    unsigned int num = 0;
    int detect = -1;
    Tag_t *tag = new Tag_t[5000];
    // memset(tag, 0, 20 * sizeof(Tag_t));
    for (unsigned int i = 0; i < lines; i++)
    {
        if ((list + i)->num_posts == 0)
        {
            continue;
        }
        else
        {
            for (unsigned int j = 0; j < (list + i)->num_posts; j++)
            {
                for (unsigned int m = 0; m < (list + i)->posts[j].num_tags; m++)
                {
                    if (num == 0)
                    {
                        (tag + num)->tag_content = (list + i)->posts[j].tags[m];
                        (tag + num)->num_post = 1;
                        (tag + num)->num_like = (list + i)->posts[j].num_likes;
                        (tag + num)->num_comment = (list + i)->posts[j].num_comments;
                        num++;
                    }
                    else
                    {
                        for (unsigned int n = 0; n < num; n++)
                        {
                            if ((list + i)->posts[j].tags[m] == (tag + n)->tag_content)
                            {
                                detect = n;
                            }
                        }
                        if (detect >= 0)
                        {
                            (tag + detect)->num_post++;
                            (tag + detect)->num_like = (tag + detect)->num_like + (list + i)->posts[j].num_likes;
                            (tag + detect)->num_comment = (tag + detect)->num_comment + (list + i)->posts[j].num_comments;
                        }
                        else
                        {
                            (tag + num)->tag_content = (list + i)->posts[j].tags[m];
                            (tag + num)->num_post = 1;
                            (tag + num)->num_like = (list + i)->posts[j].num_likes;
                            (tag + num)->num_comment = (list + i)->posts[j].num_comments;
                            num++;
                        }
                    }
                }
            }
        }
    }
    for (unsigned int i = 0; i < num; i++)
    {
        (tag + i)->tag_score = 5 * (tag + i)->num_post + 3 * (tag + i)->num_comment + (tag + i)->num_like;
    }
    *tag_count = num;
    return tag;
}

User_t *get_user_by_name(const string username, User_t *list, unsigned int lines)
{
    User_t *user = NULL;
    for (unsigned int i = 0; i < lines; i++)
    {
        if ((list + i)->username == username)
        {
            user = (list + i);
            break;
        }
    }
    return user;
}

void visit(const string visitor_name, const string visited_name, User_t *list, unsigned int lines)
{
    User_t *visitor = get_user_by_name(visitor_name, list, lines);
    User_t *visited = get_user_by_name(visited_name, list, lines);
    string relationship;
    if (visitor == visited)
    {
        relationship = "";
    }
    else if (visited->num_followers == 0)
    {
        relationship = "stranger";
    }
    else
    {
        for (unsigned int i = 0; i < visited->num_followers; i++)
        {
            if (visitor->username == visited->follower[i])
            {
                if (visited->num_following == 0)
                {
                    relationship = "following";
                }
                else
                {
                    int detect = 0;
                    for (unsigned int j = 0; j < visited->num_following; j++)
                    {
                        if (visited->following[j] == visitor->username)
                        {
                            detect = 1;
                        }
                    }
                    if (detect == 1)
                    {
                        relationship = "friend";
                    }
                    else
                    {
                        relationship = "following";
                    }
                }
            }
            else
            {
                relationship = "stranger";
            }
        }
    }
    printUser(*visited, relationship);
}

bool compare(const Tag_t &a, const Tag_t &b)
{
    if (a.tag_score == b.tag_score)
    {
        return *a.tag_content.c_str() < *b.tag_content.c_str();
    }
    return (a.tag_score > b.tag_score);
}

int comparenew(const void *p1, const void *p2)
{
    Tag_t *s1 = (Tag_t *)p1;
    Tag_t *s2 = (Tag_t *)p2;
    return (s2->tag_score - s1->tag_score);
}

void trending(Tag_t *tag, unsigned int top, unsigned int *tag_count)
{
    int tags_number = 0;
    for (unsigned int i = 0; i < 50; i++)
    {
        if ((tag + i) != NULL)
        {
            tags_number++;
        }
    }
    sort(tag, tag + tags_number, compare);
    // qsort((void *)tag, tags_number, sizeof(Tag_t), comparenew);
    for (unsigned int i = 0; i < top; i++)
    {
        if (i < *tag_count)
        {
            printTag((tag + i), i + 1);
        }
    }
}

void refresh(string username, User_t *list, unsigned int lines)
{
    User_t *user = get_user_by_name(username, list, lines);
    if (user->num_posts > 0)
    {
        for (unsigned int i = 0; i < user->num_posts; i++)
        {
            printPost(user->posts[i]);
        }
    }
    for (unsigned int i = 0; i < user->num_following; i++)
    {
        User_t *following = get_user_by_name(user->following[i], list, lines);
        if (following->num_posts > 0)
        {
            for (unsigned int j = 0; j < following->num_posts; j++)
            {
                printPost(following->posts[j]);
            }
        }
    }
}

void follow(const string following_name, const string followed_name, User_t *list, unsigned int lines)
{
    User_t *following = get_user_by_name(following_name, list, lines);
    User_t *followed = get_user_by_name(followed_name, list, lines);
    int detect = 0;
    for (unsigned int i = 0; i < following->num_following; i++)
    {
        if (following->following[i] == followed_name)
        {
            detect = 1;
        }
    }
    if (detect == 0)
    {
        following->num_following++;
        following->following[following->num_following - 1] = followed_name;
        followed->num_followers++;
        followed->follower[followed->num_followers - 1] = following_name;
    }
}

void unfollow(const string unfollowing_name, const string unfollowed_name, User_t *list, unsigned int lines)
{
    User_t *unfollowing = get_user_by_name(unfollowing_name, list, lines);
    User_t *unfollowed = get_user_by_name(unfollowed_name, list, lines);
    int following_index = 0;
    for (unsigned int i = 0; i < unfollowing->num_following; i++)
    {
        if (unfollowing->following[i] == unfollowed_name)
        {
            following_index = i;
        }
    }
    for (unsigned int i = following_index; i < unfollowing->num_following - 1; i++)
    {
        unfollowing->following[i] = unfollowing->following[i + 1];
    }
    unfollowing->following[unfollowing->num_following - 1] = "";
    unfollowing->num_following--;
    int follower_index = 0;
    for (unsigned i = 0; i < unfollowed->num_followers; i++)
    {
        if (unfollowed->follower[i] == unfollowing_name)
        {
            follower_index = i;
        }
    }
    for (unsigned int i = follower_index; i < unfollowed->num_followers - 1; i++)
    {
        unfollowed->follower[i] = unfollowed->follower[i + 1];
    }
    unfollowed->follower[unfollowed->num_followers - 1] = "";
    unfollowed->num_followers--;
}

void like(const string like_name, const string liked_name, unsigned int post_num, User_t *list, unsigned int lines, Tag_t *taglist, unsigned int *tag_count)
{
    User_t *like = get_user_by_name(like_name, list, lines);
    User_t *liked = get_user_by_name(liked_name, list, lines);
    try
    {
        if (post_num > liked->num_posts)
        {
            ostringstream oStream;
            oStream << "Error: " << like->username << " cannot like post #" << post_num << " of " << liked->username << "!"
                    << endl;
            oStream << liked->username << " does not have post #" << post_num << "." << endl;
            throw Exception_t(INVALID_LOG, oStream.str());
        }
    }
    catch (Exception_t &exception)
    {
        cout << exception.error_info;
        return;
    }
    int has_user = 0;
    for (unsigned int i = 0; i < liked->posts[post_num - 1].num_likes; i++)
    {
        if (liked->posts[post_num - 1].like_users[i] == like->username)
        {
            has_user = 1;
        }
    }
    try
    {
        if (has_user == 1)
        {
            ostringstream oStream;
            oStream << "Error: " << like->username << " cannot like post #" << post_num << " of" << liked->username << "!"
                    << endl;
            oStream << like->username << " has already liked post #" << post_num << " of " << liked->username << "." << endl;
            throw Exception_t(INVALID_LOG, oStream.str());
        }
    }
    catch (Exception_t &exception)
    {
        cout << exception.error_info;
        return;
    }
    liked->posts[post_num - 1].num_likes++;
    liked->posts[post_num - 1].like_users[liked->posts[post_num - 1].num_likes - 1] = like_name;
    for (unsigned int k = 0; k < liked->posts[post_num - 1].num_tags; k++)
    {
        for (unsigned int i = 0; i < *tag_count + 1; i++)
        {
            if ((taglist + i)->tag_content == liked->posts[post_num - 1].tags[k])
            {
                (taglist + i)->num_like++;
                (taglist + i)->tag_score = (taglist + i)->tag_score + 1;
            }
        }
    }
}

void unlike(const string unlike_name, const string unliked_name, unsigned int post_num, User_t *list, unsigned int lines, Tag_t *taglist, unsigned int *tag_count)
{
    User_t *unlike = get_user_by_name(unlike_name, list, lines);
    User_t *unliked = get_user_by_name(unliked_name, list, lines);
    try
    {
        if (post_num > unliked->num_posts)
        {
            ostringstream oStream;
            oStream << "Error: " << unlike->username << " cannot unlike post #" << post_num << " of " << unliked->username << "!"
                    << endl;
            oStream << unliked->username << " does not have post #" << post_num << "." << endl;
            throw Exception_t(INVALID_LOG, oStream.str());
        }
    }
    catch (Exception_t &exception)
    {
        cout << exception.error_info;
        return;
    }
    int has_user = 0;
    for (unsigned int i = 0; i < unliked->posts[post_num - 1].num_likes; i++)
    {
        if (unliked->posts[post_num - 1].like_users[i] == unlike->username)
        {
            has_user = 1;
        }
    }
    try
    {
        if (has_user == 0)
        {
            ostringstream oStream;
            oStream << "Error: " << unlike->username << " cannot unlike post #" << post_num << " of " << unliked->username << "!"
                    << endl;
            oStream << unlike->username << " has not liked post #" << post_num << " of " << unliked->username << "." << endl;
            throw Exception_t(INVALID_LOG, oStream.str());
        }
    }
    catch (Exception_t &exception)
    {
        cout << exception.error_info;
        return;
    }
    int like_index = 0;
    for (unsigned int i = 0; i < unliked->posts[post_num - 1].num_likes; i++)
    {
        if (unliked->posts[post_num - 1].like_users[i] == unlike_name)
        {
            like_index = i;
        }
    }
    for (unsigned int i = like_index; i < unliked->posts[post_num - 1].num_likes - 1; i++)
    {
        unliked->posts[post_num - 1].like_users[i] = unliked->posts[post_num - 1].like_users[i + 1];
    }
    unliked->posts[post_num - 1].like_users[unliked->posts[post_num - 1].num_likes - 1] = "";
    unliked->posts[post_num - 1].num_likes--;
    for (unsigned int k = 0; k < unliked->posts[post_num - 1].num_tags; k++)
    {
        for (unsigned int i = 0; i < *tag_count + 1; i++)
        {
            if ((taglist + i)->tag_content == unliked->posts[post_num - 1].tags[k])
            {
                (taglist + i)->num_like--;
                (taglist + i)->tag_score = (taglist + i)->tag_score - 1;
            }
        }
    }
}

void comment(const string commenter_name, const string poster_name, unsigned int post_num, User_t *list, const string comment, unsigned int lines, Tag_t *taglist, unsigned int *tag_count)
{
    User_t *commenter = get_user_by_name(commenter_name, list, lines);
    User_t *poster = get_user_by_name(poster_name, list, lines);
    try
    {
        if (post_num > poster->num_posts)
        {
            ostringstream oStream;
            oStream << "Error: " << commenter->username << " cannot comment post #" << post_num << " of " << poster->username << "!"
                    << endl;
            oStream << poster->username << " does not have post #" << post_num << "." << endl;
            throw Exception_t(INVALID_LOG, oStream.str());
        }
    }
    catch (Exception_t &exception)
    {
        cout << exception.error_info;
        return;
    }
    for (unsigned int k = 0; k < poster->posts[post_num - 1].num_tags; k++)
    {
        for (unsigned int i = 0; i < *tag_count + 1; i++)
        {
            if ((taglist + i)->tag_content == poster->posts[post_num - 1].tags[k])
            {
                (taglist + i)->num_comment++;
                (taglist + i)->tag_score = (taglist + i)->tag_score + 3;
            }
        }
    }
    poster->posts[post_num - 1].num_comments++;
    poster->posts[post_num - 1].comments[poster->posts[post_num - 1].num_comments - 1].user = commenter_name;
    poster->posts[post_num - 1].comments[poster->posts[post_num - 1].num_comments - 1].text = comment;
}

void uncomment(const string uncommenter_name, const string poster_name, unsigned int post_num, User_t *list, unsigned int comment_num, unsigned int lines, Tag_t *taglist, unsigned int *tag_count)
{
    User_t *uncommenter = get_user_by_name(uncommenter_name, list, lines);
    User_t *poster = get_user_by_name(poster_name, list, lines);
    try
    {
        if (post_num > poster->num_posts)
        {
            ostringstream oStream;
            oStream << "Error: " << uncommenter->username << " cannot uncomment comment #" << comment_num
                    << " of post #" << post_num << " posted by " << poster->username << "!" << endl;
            oStream << poster->username << " does not have post #" << post_num << "." << endl;
            throw Exception_t(INVALID_LOG, oStream.str());
        }
    }
    catch (Exception_t &exception)
    {
        cout << exception.error_info;
        return;
    }
    try
    {
        if (comment_num > poster->posts[post_num - 1].num_comments)
        {
            ostringstream oStream;
            oStream << "Error: " << uncommenter->username << " cannot uncomment comment #" << comment_num
                    << " of post #" << post_num << " posted by " << poster->username << "!" << endl;
            oStream << "Post #" << post_num << " does not have comment #" << comment_num << "." << endl;
            throw Exception_t(INVALID_LOG, oStream.str());
        }
    }
    catch (Exception_t &exception)
    {
        cout << exception.error_info;
        return;
    }
    try
    {
        if (poster->posts[post_num - 1].comments[comment_num - 1].user != uncommenter->username)
        {
            ostringstream oStream;
            oStream << "Error: " << uncommenter->username << " cannot uncomment comment #" << comment_num
                    << " of post #" << post_num << " posted by " << poster->username << "!" << endl;
            oStream << uncommenter->username << " is not the owner of comment #" << comment_num << "." << endl;
        }
    }
    catch (Exception_t &exception)
    {
        cout << exception.error_info;
        return;
    }
    int index_comment = 0;
    for (unsigned int k = 0; k < poster->posts[post_num - 1].num_tags; k++)
    {
        for (unsigned int i = 0; i < *tag_count + 1; i++)
        {
            if ((taglist + i)->tag_content == poster->posts[post_num - 1].tags[k])
            {
                (taglist + i)->num_comment--;
                (taglist + i)->tag_score = (taglist + i)->tag_score - 3;
            }
        }
    }
    for (unsigned int i = 0; i < poster->posts[post_num].num_comments; i++)
    {
        if (poster->posts[post_num - 1].comments[i].user == uncommenter->username)
        {
            index_comment = i;
        }
    }
    for (unsigned int i = index_comment; i < poster->posts[post_num - 1].num_comments - 1; i++)
    {
        poster->posts[post_num - 1].comments[i].user = poster->posts[post_num - 1].comments[i + 1].user;
        poster->posts[post_num - 1].comments[i].text = poster->posts[post_num - 1].comments[i + 1].text;
    }
    poster->posts[post_num - 1].comments[poster->posts[post_num - 1].num_comments - 1].user = "";
    poster->posts[post_num - 1].comments[poster->posts[post_num - 1].num_comments - 1].text = "";
    poster->posts[post_num - 1].num_comments--;
}

void post(const string poster, const string title, const string tag[], const string text, unsigned int tagcount, User_t *list, unsigned int lines, Tag_t *taglist, unsigned int *tag_count)
{
    User_t *user = get_user_by_name(poster, list, lines);
    user->num_posts++;
    try
    {
        if (user->num_posts > MAX_POSTS)
        {
            ostringstream oStream;
            oStream << "Error: Too many posts for user " << user->username << "!" << endl;
            oStream << "Maximal number of posts is " << MAX_POSTS << "." << endl;
            throw Exception_t(CAPACITY_OVERFLOW, oStream.str());
        }
    }
    catch (Exception_t &exception)
    {
        return;
    }
    user->posts[user->num_posts - 1].owner = poster;
    user->posts[user->num_posts - 1].title = title;
    user->posts[user->num_posts - 1].num_tags = tagcount;
    for (unsigned int i = 0; i < tagcount; i++)
    {
        user->posts[user->num_posts - 1].tags[i] = tag[i];
    }
    user->posts[user->num_posts - 1].text = text;
    user->posts[user->num_posts - 1].num_likes = 0;
    user->posts[user->num_posts - 1].num_comments = 0;
    for (unsigned int k = 0; k < tagcount; k++)
    {
        int index = -1;
        for (unsigned int i = 0; i < *tag_count + 1; i++)
        {
            if ((taglist + i)->tag_content == tag[k])
            {
                index = i;
                (taglist + i)->num_post++;
                (taglist + i)->tag_score = (taglist + i)->tag_score + 5;
            }
        }
        if (index < 0)
        {
            *tag_count = *tag_count + 1;
            (taglist + *tag_count)->tag_content = tag[k];
            (taglist + *tag_count)->num_post = 1;
            (taglist + *tag_count)->num_like = 0;
            (taglist + *tag_count)->num_comment = 0;
            (taglist + *tag_count)->tag_score = 5;
        }
    }
}

void unpost(const string poster, unsigned int post_num, User_t *list, unsigned int lines, Tag_t *taglist, unsigned int *tag_count)
{
    User_t *user = get_user_by_name(poster, list, lines);
    try
    {
        if (post_num > user->num_posts)
        {
            ostringstream oStream;
            oStream << "Error: " << user->username << " cannot delete post #" << post_num << "!" << endl;
            oStream << user->username << " does not have post #" << post_num << "." << endl;
            throw Exception_t(INVALID_LOG, oStream.str());
        }
    }
    catch (Exception_t &exception)
    {
        cout << exception.error_info;
        return;
    }
    for (unsigned int k = 0; k < user->posts[post_num - 1].num_tags; k++)
    {
        for (unsigned int i = 0; i < *tag_count + 1; i++)
        {
            if ((taglist + i)->tag_content == user->posts[post_num - 1].tags[k])
            {
                (taglist + i)->num_post--;
                (taglist + i)->num_like = (taglist + i)->num_like - user->posts[post_num - 1].num_likes;
                (taglist + i)->num_comment = (taglist + i)->num_comment - user->posts[post_num - 1].num_comments;
                (taglist + i)->tag_score = 5 * (taglist + i)->num_post + 3 * (taglist + i)->num_comment + (taglist + i)->num_like;
            }
        }
    }
    for (unsigned int i = post_num - 1; i < user->num_posts - 1; i++)
    {
        user->posts[i] = user->posts[i + 1];
    }
    user->num_posts--;
}

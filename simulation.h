/*
 * This is VE280 Project 2, SU2020.
 * Written by Ziqiao Ma and Zhuoer Zhu.
 * Latest Update: 5/29/2020.
 * All rights reserved.
 */

#include <algorithm>
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <sstream>

#include "server_type.h"
using namespace std;

// TODO: Declare your functions in this header file.
// Hint: You may need these request handling functions.
void detect_argv_num(int argc);
unsigned int user_number(char *argv);
int detect_logfile(char *argv);
Post_t *initial_posts(const string username, unsigned int count, char *users_name);
User_t *initial_userlist(unsigned int lines, char *users_name);
Tag_t *initial_tag(User_t *list, unsigned int lines, unsigned int *tag_count);
User_t *get_user_by_name(const string username, User_t *list, unsigned int lines);
void visit(const string vistor_name, const string visited_name, User_t *list, unsigned int lines);
bool compare(const Tag_t &a, const Tag_t &b);
int comparenew(const void *p1, const void *p2);
void trending(Tag_t *tag, unsigned int top, unsigned int *tag_count);
void refresh(string username, User_t *list, unsigned int lines);
void follow(const string following_name, string followed_name, User_t *list, unsigned int lines);
void unfollow(const string unfollowing_name, const string unfollowed_name, User_t *list, unsigned int lines);
void like(const string like_name, const string liked_name, unsigned int post_num, User_t *list, unsigned int lines, Tag_t *taglist, unsigned int *tag_count);
void unlike(const string unlike_name, const string unliked_name, unsigned int post_num, User_t *list, unsigned int lines, Tag_t *taglist, unsigned int *tag_count);
void comment(const string commenter_name, const string poster_name, unsigned int post_num, User_t *list, const string comment, unsigned int lines, Tag_t *taglist, unsigned int *tag_count);
void uncomment(const string uncommenter_name, const string poster_name, unsigned int post_num, User_t *list, unsigned int comment_num, unsigned int lines, Tag_t *taglist, unsigned int *tag_count);
void post(const string poster, const string title, const string tag[], const string text, unsigned int tagcount, User_t *list, unsigned int lines, Tag_t *taglist, unsigned int *tag_count);
void unpost(const string poster, unsigned int post_num, User_t *list, unsigned int lines, Tag_t *taglist, unsigned int *tag_count);

/* Helper Functions */

// Printing
void printUser(User_t user, const string relationship);
void printPost(Post_t post);
void printTag(const Tag_t *tag, unsigned int rank);

#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAX_LENGTH 1024

int strequ(const char s1[], const char s2[]);
int strlen(const char *s);
int has_opt(const char opt[]);
void handle_d(char *d_name);
void handle_a(char *d_name);
void handle_i(char *f_name);
void handle_R(char *f_name);

int g_argc;
char **g_argv;

int main(int argc, char *argv[]) {
    g_argc=argc;
    g_argv=argv;
    char dir_buff[MAX_LENGTH];
    if (getcwd(dir_buff, sizeof(dir_buff))) {
        puts(dir_buff);
        DIR *dp;
        if ((dp = opendir(dir_buff)) != NULL) {
            struct dirent *entry;
            while ((entry=readdir(dp)) != NULL) {
                puts(entry->d_name);
            }
            puts("=================");
//            handle_d(dp);
            closedir(dp);
        }
    }
    return 0;
}

void handle_d(char *d_name){
    if (has_opt("-d")){
        handle_l(d_name);
    } else{
        handle_a(d_name);
    }
}

void handle_a(char *d_name){
    DIR *dp;
    struct dirent *entry;
    if ((dp=opendir(d_name))!=NULL){
        if (has_opt("-a")){
            while ((entry=readdir(dp)) != NULL) {
                handle_R(entry->d_name);
            }
        } else{
            while ((entry=readdir(dp)) != NULL){
                char *p = entry->d_name;
                if (*p!='.'){
                    handle_R(p);
                }
            }
        }
        closedir(dp);
    }
}

void handle_R(char *f_name){
    if (has_opt("-R")){
        char cwd_buff[MAX_LENGTH];
        if (getcwd(cwd_buff, sizeof(cwd_buff))){
            int len = strlen(cwd_buff);
            printf(".%s:", f_name+len);
        } else{
            puts(f_name);
        }

    } else{

    }
}

void handle_i(char *f_name){
    if (has_opt("-i")){

    }
}

int has_opt(const char opt[]) {
    for (int i = 1; i < g_argc; ++i) {
        if (strequ(opt, g_argv[i])){
            return 1;
        }
    }
    return 0;
}

int strequ(const char s1[], const char s2[]) {
    if (s1==NULL || s2==NULL){
        return -1;
    }
    while (*s1) {
        if (*s1 != *s2) {
            return 0;
        }
        ++s1;
        ++s2;
    }
    return *s1 == *s2;
}

int strlen(const char *s){
    int len=0;
    if (s){
        while (*s){
            ++s;
        }
    }
    return len;
}

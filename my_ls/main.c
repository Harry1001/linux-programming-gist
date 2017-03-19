#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>

#define MAX_LENGTH 1024

int strequ(const char s1[], const char s2[]);
int has_opt(const char opt[]);
void handle_d(char *d_name);
void handle_a(char *d_name);
void handle_il(char *f_name);
void handle_R(char *f_name);

int g_argc;
char **g_argv;

int main(int argc, char *argv[]) {
    g_argc=argc;
    g_argv=argv;
    char dir_buff[MAX_LENGTH];
    if (getcwd(dir_buff, sizeof(dir_buff))) {
        handle_d(dir_buff);
//        puts(dir_buff);
//        DIR *dp;
//        if ((dp = opendir(dir_buff)) != NULL) {
//            struct dirent *entry;
//            while ((entry=readdir(dp)) != NULL) {
//                puts(entry->d_name);
//            }
//            puts("=================");
//            closedir(dp);
//        }
    }
    return 0;
}

void handle_d(char *d_name){
    if (has_opt("-d")){
        handle_il(d_name);
    } else{
        handle_a(d_name);
    }
}

void handle_a(char *d_name){
    DIR *dp;
    struct dirent *entry;
    if ((dp=opendir(d_name))!=NULL){
        while((entry=readdir(dp)) != NULL){
            if (!strequ(entry->d_name, ".") && (!strequ(entry->d_name, ".."))){
                if (has_opt("-a")){
                    handle_R(entry->d_name);
                } else{
                    char *p = entry->d_name;
                    if (*p!='.'){
                        handle_R(p);
                    }
                }
            }
        }
        closedir(dp);
    }
}

void handle_R(char *f_name){
    handle_il(f_name);
    if (has_opt("-R")){
        //if directory, open it
        struct stat stat_buff;
        if (lstat(f_name, &stat_buff)==0){   //get stat success
            if (S_ISDIR(stat_buff.st_mode)){    //is directory
                handle_a(f_name);
            } else{ //not directory
                //do nothing
            }
        }
    } else{
        //do nothing
    }
}

void handle_il(char *f_name){
    if (has_opt("-i")){
        struct stat stat_buff;
        if (lstat(f_name, &stat_buff)==0){
            printf("%lu", stat_buff.st_ino);
        }
    }
    if (has_opt("-l")){
        char per_buff[11];
        char *p=per_buff;
        per_buff[0]='\0';
        per_buff[11]='\0';
        struct stat stat_buff;
        if (lstat(f_name, &stat_buff)==0){
            if (S_ISDIR(stat_buff.st_mode)){
                *p++ = 'd';
            } else{
                *p++ = '-';
            }
            if (stat_buff.st_mode & S_IRUSR){
                *p++ = 'r';
            } else{
                *p++ = '-';
            }
            if (stat_buff.st_mode & S_IWUSR){
                *p++ = 'w';
            } else{
                *p++ = '-';
            }
            if (stat_buff.st_mode & S_IXUSR){
                *p = 'x';
                if (stat_buff.st_mode & S_ISUID){
                    *p = 's';
                }
                ++p;
            } else{
                *p = '-';
                if (stat_buff.st_mode & S_ISUID){
                    *p = 'S';
                }
                ++p;
            }
            //group
            if (stat_buff.st_mode & S_IRGRP){
                *p++ = 'r';
            } else{
                *p++ = '-';
            }
            if (stat_buff.st_mode & S_IWGRP){
                *p++ = 'w';
            } else{
                *p++ = '-';
            }
            if (stat_buff.st_mode & S_IXGRP){
                *p = 'x';
                if (stat_buff.st_mode & S_ISGID){
                    *p = 's';
                }
                ++p;
            } else{
                *p = '-';
                if (stat_buff.st_mode & S_ISGID){
                    *p = 'S';
                }
                ++p;
            }
            //other
            if (stat_buff.st_mode & S_IROTH){
                *p++ = 'r';
            } else{
                *p++ = '-';
            }
            if (stat_buff.st_mode & S_IWOTH){
                *p++ = 'w';
            } else{
                *p++ = '-';
            }
            if (stat_buff.st_mode & S_IXOTH){
                *p = 'x';
                if (stat_buff.st_mode & S_ISVTX){
                    *p = 't';
                }
            } else{
                *p = '-';
                if (stat_buff.st_mode & S_ISVTX){
                    *p = 'T';
                }
            }
            //output permission
            printf(" %s", per_buff);

            printf(" %lu", stat_buff.st_nlink);
            struct passwd *pw = getpwuid(stat_buff.st_uid);
            struct group *gr = getgrgid(stat_buff.st_gid);
            if (pw != NULL){
                printf(" %s", pw->pw_name);
            } else{
                printf(" %u", stat_buff.st_uid);
            }
            if (gr !=NULL){
                printf(" %s", gr->gr_name);
            } else{
                printf(" %u", stat_buff.st_gid);
            }
            printf(" %ld", stat_buff.st_size);



            //todo
            printf("\n");
        }
    } else{
        printf("%s\t", f_name);
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


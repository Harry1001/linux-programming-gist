#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH 1024

int strequ(const char s1[], const char s2[]);
int has_opt(const char opt[]);
void handle_d();
void handle_a();
void handle_il(char *f_name);
void handle_R(char *d_name);

int g_argc;
char **g_argv;;
char cwd[MAX_LENGTH];

int main(int argc, char *argv[]) {
    g_argc=argc;
    g_argv=argv;
    cwd[0]='.';
    cwd[1]='\0';
    handle_d();
    return 0;
}

void handle_d(){
    if (has_opt("-d")){
        handle_il(cwd);
    } else{
        handle_a();
    }
}

void handle_a(){
    DIR *dp;
    struct dirent *entry;

    if (has_opt("-R")){
        printf("%s:\n", cwd);
    }
    if ((dp=opendir(cwd))!=NULL){
        if (has_opt("-a")){
            handle_il(".");
            handle_il("..");
        }
        while((entry=readdir(dp)) != NULL){
            if (!strequ(entry->d_name, ".") && (!strequ(entry->d_name, ".."))){

                if (has_opt("-a")){
                    handle_il(entry->d_name);
                } else{
                    char *p = entry->d_name;
                    if (*p!='.'){
                        handle_il(entry->d_name);
                    }
                }
            }
        }
        puts("\n");
        closedir(dp);

        if((dp=opendir(cwd))!=NULL){
            while((entry=readdir(dp)) != NULL){
                if (!strequ(entry->d_name, ".") && (!strequ(entry->d_name, ".."))){
                    size_t len = strlen(entry->d_name);
                    strcat(cwd,"/");
                    strcat(cwd, entry->d_name);
                    struct stat stat_buff;
                    if (lstat(cwd, &stat_buff)==0){   //get stat success
                        cwd[strlen(cwd)-len-1]='\0';
                        if (S_ISDIR(stat_buff.st_mode)){    //is directory
                            if (has_opt("-a")){
                                handle_R(entry->d_name);
                            } else{
                                if (*(entry->d_name) != '.'){
                                    handle_R(entry->d_name);
                                }
                            }
                        } else{ //not directory
                            //do nothing
                        }
                    } else{
                        cwd[strlen(cwd)-len-1]='\0';
                    }
                }
            }

            closedir(dp);
        }

    }
}

void handle_R(char *d_name){
    if (has_opt("-R")){
        size_t len = strlen(d_name);
        strcat(cwd, "/");
        strcat(cwd, d_name);
        handle_a();
        size_t total_len = strlen(cwd);
        cwd[total_len-len-1]='\0';
    } else{
        //do nothing
    }
}

void handle_il(char *f_name){
    if (has_opt("-i")){
        struct stat stat_buff;
        if (lstat(f_name, &stat_buff)==0){
            printf("%lu ", stat_buff.st_ino);
        }
    }
    if (has_opt("-l")){
        char per_buff[11];
        char *p=per_buff;
        per_buff[0]='\0';
        per_buff[10]='\0';
        struct stat stat_buff;
        if (lstat(f_name, &stat_buff)==0){
            if (S_ISDIR(stat_buff.st_mode)){
                *p++ = 'd';
            } else if (S_ISCHR(stat_buff.st_mode)){
                *p++ = 'c';
            } else if (S_ISBLK(stat_buff.st_mode)){
                *p++ = 'b';
            } else if (S_ISFIFO(stat_buff.st_mode)){
                *p++ = 'p';
            } else if (S_ISLNK(stat_buff.st_mode)){
                *p++ = 'l';
            } else if (S_ISSOCK(stat_buff.st_mode)){
                *p++ = 's';
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
            printf("%s ", per_buff);

            printf("%lu ", stat_buff.st_nlink);
            struct passwd *pw = getpwuid(stat_buff.st_uid);
            struct group *gr = getgrgid(stat_buff.st_gid);
            if (pw != NULL){
                printf("%s ", pw->pw_name);
            } else{
                printf("%u ", stat_buff.st_uid);
            }
            if (gr !=NULL){
                printf("%s ", gr->gr_name);
            } else{
                printf("%u ", stat_buff.st_gid);
            }
            printf("%ld ", stat_buff.st_size);
            printf("%d ", stat_buff.st_mtim);//todo error
            printf("%s\n", f_name);
        }
    } else{
        printf("%s  ", f_name);
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


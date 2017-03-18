#include <stdio.h>
#define MAX_LINE 1024

int is_blank(char c);

int main(int argc, char * argv[]) {
    if (argc != 2) {
        printf("Sorry, please input one filename\n");
        return -1;
    }
    FILE* fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf("Read file failed!\n");
        return -1;
    }
    char buff[MAX_LINE];
    unsigned int l_cnt,w_cnt,c_cnt;
    l_cnt=w_cnt=c_cnt=0;
    int was_blank=1;
    while (!feof(fp)) {
        fgets(buff, MAX_LINE, fp);
        char *p = buff;
        while (*p) {
            c_cnt++;
            if (is_blank(*p)) {
                was_blank=1;
                if (*p=='\n'){
                    l_cnt++;
                }
            } else {
                if (was_blank){
                    w_cnt++;
                    was_blank=0;
                }
            }
            p++;
        }
    }
    fclose(fp);
    printf(" %d %d %d %s\n", l_cnt, w_cnt, c_cnt, argv[1]);
    return 0;
}

int is_blank(char c) {
    if (c==' ') {
        return 1;
    } else {
        return c<=' ';
    }
}
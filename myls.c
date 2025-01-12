/*
 *myls.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <libgen.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>

void print_dir(DIR *maindir, bool alldirs);
void print_dir_stats(DIR *maindir, bool alldirs, char* dir_path);
void print_item_stats(char* file_name);
void print_stats(struct stat is);

int main(int argc, char *argv[]){
    bool show_hidden_files = false;
    bool list_file_info = false;
    struct stat item_stats;
    DIR *my_dir;

    int ch;
    while ((ch = getopt(argc, argv, "al")) != -1){
        switch (ch){
            case 'a':
                show_hidden_files = true;
                break;
            case 'l':
                list_file_info = true;

                break;
            default:
                return 0;
                break;
        }
    }

    if(optind == argc){
        my_dir = opendir(".");
        if(list_file_info){
            print_dir_stats(my_dir, show_hidden_files, ".");
        }
        else{
            print_dir(my_dir, show_hidden_files);
        }
        closedir(my_dir);
    }
    else{
        for(int i = optind; i < argc ;i++){
            if(stat(argv[i], &item_stats) == -1){
                perror("Error: couldn't see stats of item");
            }

            if(S_ISDIR(item_stats.st_mode)){
                if ((my_dir = opendir(argv[i])) == NULL){
                    perror("cannot open directory");
                    continue;
                }

                if(list_file_info){
                    if(optind + 1 < argc){
                        printf("%s:\n", argv[i]); ///prints header if multiple non option arguements
                    }

                    print_dir_stats(my_dir, show_hidden_files, argv[i]);

                    if(optind + 1 < argc){
                        printf("\n"); //adds space before next option
                    }

                    closedir(my_dir);
                }
                else{
                    if(optind + 1 < argc){
                        printf("%s:\n", argv[i]); //prints header if multiple non option arguements
                    }

                    print_dir(my_dir, show_hidden_files);

                    if(optind + 1 < argc){
                        printf("\n");  //adds space before next option
                    }
                }
            }

            else{
                if(list_file_info){
                    print_item_stats(argv[i]);
                }
                else{
                    printf("%s\n", argv[i]);

                }
            }
        } 
    }
}

/*
prints all directories that don't begin with a '.' 
if parameter 'alldirs' is false
*/
void print_dir(DIR *maindir, bool alldirs){
    struct dirent *item;
    while((item = readdir(maindir)) != NULL){
        char first = item->d_name[0];
        if ((first != '.') || alldirs){
            printf("%s        ", item->d_name);
        }
    }
    printf("\n");
}

/*
prints all directories in the -l format that don't begin with a '.'
unless the "alldirs" boolean is true, in which case print all directories in the -l format
*/
void print_dir_stats(DIR *maindir, bool alldirs, char* dir_path){
    struct dirent *item;
    struct stat item_stats;
    //printf("%s", dir_path);

    while((item = readdir(maindir)) != NULL){
        char first = item->d_name[0];
        if ((first != '.') || alldirs){

            char* item_path = malloc((strlen(dir_path) + strlen(item->d_name) + 2));    
            snprintf(item_path, strlen(dir_path) + strlen(item->d_name) + 2, "%s/%s", dir_path, item->d_name);

            if(stat(item_path, &item_stats) == -1){
                perror("Error: couldn't get directory stats");
            }

            free(item_path);

            print_stats(item_stats);

            printf(" %s", item->d_name);
            printf("\n"); 
        }
    }
}

/*
Prints the file stats of the file specified by file_name
*/
void print_item_stats(char* file_name){
    struct stat item_stats;

    if(stat(file_name, &item_stats) == -1){
        perror("Error: couldn't get file stats");
        return;
    }

    print_stats(item_stats);

    printf(" %s", file_name);
    printf("\n");
}


/*
Prints the stats of the given stat struct
*/
void print_stats(struct stat is){
    char time_string[50];
    struct passwd *password;
    struct group *group_name;
    struct tm *time_info;

    printf((S_ISDIR(is.st_mode)) ? "d" : "-");
    printf((is.st_mode & S_IRUSR) ? "r" : "-");
    printf((is.st_mode & S_IWUSR) ? "w" : "-");
    printf((is.st_mode & S_IXUSR) ? "x" : "-");
    printf((is.st_mode & S_IRGRP) ? "r" : "-");
    printf((is.st_mode & S_IWGRP) ? "w" : "-");
    printf((is.st_mode & S_IXGRP) ? "x" : "-");
    printf((is.st_mode & S_IROTH) ? "r" : "-");
    printf((is.st_mode & S_IWOTH) ? "w" : "-");
    printf((is.st_mode & S_IXOTH) ? "x" : "-");
    printf(" %ld", (is.st_nlink));
    password = getpwuid(is.st_uid);
    if (password == NULL){
        printf(" %d", is.st_uid);
    }
    else{
        printf(" %s", (password->pw_name));
    }
    group_name = getgrgid(is.st_gid);
    if (group_name == NULL){
        printf(" %d", is.st_gid);
    }
    else{
        printf(" %s", (group_name->gr_name));
    }
    printf(" %ld ", is.st_size);
    time_info = localtime(&(is.st_mtime));
    if(time_info != NULL){
        
        strftime(time_string, 13, "%b %d %H:%M", time_info);   //13 is the amount of space the longest possible time_info will need
        printf("%s", time_string);
    }
    else{
        return;
    }
}





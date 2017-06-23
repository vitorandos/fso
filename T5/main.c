#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

int lines_printed = 0;


void search_directory(const char *directory_path, const char *search_string){

    if (lines_printed == 4){
      return ;
    }

    DIR *physical_directory;
    struct dirent *entry;

    if (!(physical_directory = opendir(directory_path)))
        return;
    if (!(entry = readdir(physical_directory)))
        return;

    do {
        if (entry->d_type == DT_DIR) {
            char path[1024];
            int len = snprintf(path, sizeof(path)-1, "%s/%s", directory_path, entry->d_name);
            path[len] = 0;
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            search_directory(path, search_string);
        }
        else {
            if ((strstr(entry->d_name, search_string) != NULL) && lines_printed < 4){
              FILE *file;
              char file_path[100] = "";
              strcat(file_path, directory_path);
              strcat(file_path, "/");
              strcat(file_path, entry->d_name);

              file = fopen(file_path, "rb");
              if (!file){
                printf("Erro ao abrir arquivo.\n");
              }
              char line[50];
              int end = fread(line, 1, 30, file);
              line[end] = 0;
              printf("%d%s --\n%s\n", lines_printed + 1, file_path, line);
              lines_printed++;

              fclose(file);
            }
        }
    } while (entry = readdir(physical_directory));

    closedir(physical_directory);
}

int main(int argc, char const *argv[]) {
  char path[50] = ".";
  char search_string[50];
  char *end;
  int max_lines = 0;

  if (argc == 4){
    strcat(path, argv[1]);
    strcpy(search_string, argv[2]);
    max_lines = strtol(argv[3], &end, 10);
  } else {
    printf("Erro nos parametros de entrada.\n");
  }

  printf("%s\n", path);
  printf("%s\n", search_string);
  printf("%d\n", max_lines);

  const char *home_directory;

  if ((home_directory = getenv("HOME")) == NULL){
    home_directory = getpwuid(getuid())->pw_dir;
  }

  search_directory(path, search_string);
  /*
  char *directory_to_be_open = "./";
  int path_char_position = 0;

  for (; path[path_char_position] != '/'; ++path_char_position){
    strcat(directory_to_be_open, &path[path_char_position]);
  }*/


  return 0;
}

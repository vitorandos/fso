#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

int lines_printed = 0;

void search_directory(const char *directory_path, const char *search_string, int max_lines){

    if (lines_printed == max_lines){
      return ;
    }

    DIR *physical_directory;
    struct dirent *entry;

    if (!(physical_directory = opendir(directory_path)))
        return;
    if (!(entry = readdir(physical_directory)))
        return;

    // check files
    do {
      if (entry->d_type == DT_REG){
        if ((strstr(entry->d_name, search_string) != NULL) && lines_printed < max_lines){
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

    if (!(physical_directory = opendir(directory_path)))
        return;
    if (!(entry = readdir(physical_directory)))
        return;

    // check directories
    do {
      if (entry->d_type == DT_DIR) {
          char path[1024];
          int len = snprintf(path, sizeof(path)-1, "%s/%s", directory_path, entry->d_name);
          path[len] = 0;
          if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
              continue;
          search_directory(path, search_string, max_lines);
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
    return 0;
  }

  printf("Resultado de buscador -- \"%s\" na pasta %s\n", search_string, argv[1]);
  search_directory(path, search_string, max_lines);

  return 0;
}

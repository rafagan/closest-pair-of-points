/**
 * Nome completo: `
 * Registro acadêmico: 
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int calcFileSize(FILE* file) {
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    rewind(file);

    return size;
}

char* allocReadFileContent(char* path) {
    FILE* file = fopen(path, "r");
    unsigned int stringSize = calcFileSize(file);
    char* buffer = (char*) malloc(sizeof(char) * stringSize + 1);
    unsigned int readSize = fread(buffer, sizeof(char), stringSize, file);
    buffer[stringSize] = '\0';

    if(stringSize != readSize) {
        free(buffer);
        buffer = NULL;
    }

    fclose(file);
    return buffer;
}

char* proccessInputData(char* input) {
    int isFirstLine = 1;
    int isReadingX = 1;
    char buffer[50];
    double x, y;

    for(int i = 0; input[i] != '\0'; i++) {
        char c = input[i];
        if(!isdigit(c) && c != '.') {
            if(isFirstLine) {
                isFirstLine = 0;
                continue;
            }

            if(isReadingX) {
                x = strtod(buffer, NULL);
                isReadingX = 0;
            } else {
                y = strtod(buffer, NULL);
                isReadingX = 1;
            }

            strcpy(buffer, "");
        } else {
            if(!isFirstLine) {
                char tmp[2] = {input[i], '\0'};
                strcat(buffer, tmp);
            }
        }
    }

    return NULL;
}

int main(int argc, char **argv) {
    if(argc != 2) {
        puts("Invalid inputs. Usage: \n.$ /closest /path/to/genpoints/input.txt");
        return 1;
    }

    char* path = argv[1];
    char* buffer = allocReadFileContent(path);
    proccessInputData(buffer);
    free(buffer);

    return 0;
}
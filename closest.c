/**
 * Nome completo: `
 * Registro acadêmico: 
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef struct Point {
    double x;
    double y;
} Point;

typedef struct InputData {
    unsigned int count;
    Point** points;
} InputData;

typedef struct OutputData {
    double timeSecs;
    double distance;
    Point p1;
    Point p2;
} OutputData;

int calcFileSize(FILE* file) {
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    rewind(file);

    return size;
}

char* allocReadFileContent(const char* path) {
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

InputData* allocProcessInputData(const char* input) {
    int isFirstLine = 1;
    int isReadingX = 1;
    char buffer[50] = {};
    
    int pointCount;
    Point** points = NULL;
    Point* point = NULL;

    double x, y;

    for(unsigned int i = 0, j = 0; input[i] != '\0'; i++) {
        char c = input[i];
        if(!isdigit(c) && c != '.') {
            if(isFirstLine) {
                pointCount = atoi(buffer);
                points = malloc(sizeof(Point) * pointCount);
                isFirstLine = 0;
            } else {
                if(isReadingX) {
                    x = strtod(buffer, NULL);
                    isReadingX = 0;
                } else {
                    y = strtod(buffer, NULL);
                    isReadingX = 1;

                    point = malloc(sizeof(Point));
                    point->x = x;
                    point->y = y;
                    points[j++] = point;
                }
            }

            strcpy(buffer, "");
        } else {
            char tmp[2] = {input[i], '\0'};
            strcat(buffer, tmp);
        }
    }

    InputData* data = malloc(sizeof(InputData));
    data->count = pointCount;
    data->points = points;

    return data;
}

void freeInputData(InputData* inputData) {
    for(unsigned int i = 0; i < inputData->count; i++) {
        free(inputData->points[i]);
    }
    free(inputData);
}

void logInputData(const InputData* data) {
    puts("Entrada :");
    for(unsigned int i = 0; i < data->count; i++) {
        Point* point = data->points[i];
        printf("\t%d:(%f, %f)\n", i, point->x, point->y);
    }
}

OutputData findClosestPairOfPoints(const InputData* input) {
    Point p1;
    p1.x = 3.0;
    p1.y = 4.0;
    
    Point p2;
    p2.x = 5.0;
    p2.y = 6.0;

    OutputData output;
    output.timeSecs = 1.0;
    output.distance = 2.0;
    output.p1 = p1;
    output.p2 = p2;

    return output;
}

void logOutputData(const OutputData* data) {
    puts("Saída :");
    printf("\tTempo: %f\n", data->timeSecs);
    printf("\tDistância: %f\n", data->distance);
    printf("\tx1: %f\n", data->p1.x);
    printf("\ty1: %f\n", data->p1.y);
    printf("\tx2: %f\n", data->p2.x);
    printf("\ty2: %f\n", data->p2.y);
}

int main(int argc, char **argv) {
    if(argc != 2) {
        puts("Invalid inputs. Usage: \n.$ /closest /path/to/genpoints/input.txt");
        return 1;
    }

    char* path = argv[1];
    char* buffer = allocReadFileContent(path);
    InputData* inputData = allocProcessInputData(buffer);
    
    printf("%s\n", buffer);
    logInputData(inputData);

    OutputData outputData = findClosestPairOfPoints(inputData);
    logOutputData(&outputData);

    free(buffer);
    freeInputData(inputData);

    return 0;
}
/**
 * Nome completo: `
 * Registro acadêmico: 
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <time.h>

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
    inputData = NULL;
}

void logInputData(const InputData* data) {
    puts("Entrada :");
    for(unsigned int i = 0; i < data->count; i++) {
        Point* point = data->points[i];
        printf("\t%d:(%f, %f)\n", i, point->x, point->y);
    }
}

double calcEuclidianDistance(const Point* p1, const Point* p2) {
    return sqrt(pow(p1->x - p2->x, 2.0) + pow(p1->y - p2->y, 2.0));
}

double min2(double a, double b) {
    return a < b ? a : b;
}

double min3(double a, double b, double c) {
    return min2(min2(a, b), c);
}

int isEqual(double a, double b) {
    return fabs(a - b) < 0.000000001;
}

OutputData findClosestPairOfPointsNaive(const InputData* input) {
    Point* p1;
    Point* p2;
    double distance = DBL_MAX;

    clock_t startTime = clock();

    // Brute force time complexity: O(n^2)
    for(int i = 0; i < input->count; i++) {
        Point* _p1 = input->points[i];
        for(int j = i + 1; j < input->count; j++) {
            Point* _p2 = input->points[j];
            double d = calcEuclidianDistance(_p1, _p2);
            if(d < distance) {
                distance = d;
                p1 = _p1;
                p2 = _p2;
            }
        }
    }

    clock_t endTime = clock();

    OutputData output;
    output.timeSecs = (double)(endTime - startTime) / CLOCKS_PER_SEC;
    output.distance = distance;
    output.p1 = *p1;
    output.p2 = *p2;

    return output;
}

int sortPointsByXAscendingComparator(const void* a, const void* b) {
   const double x1 = (*((Point**)a))->x;
   const double x2 = (*(Point**)b)->x;
   return x1 - x2;
}

OutputData findClosestPairOfPointsRecursive(
    const Point** points, 
    unsigned int startIndex, 
    unsigned int endIndex
) {
    OutputData output;
    const unsigned int length = endIndex - startIndex + 1;

    if(length == 2) {
        output.p1 = *points[startIndex];
        output.p2 = *points[endIndex];
        output.distance = calcEuclidianDistance(&output.p1, &output.p2);
    } else if(length == 3) {
        const unsigned int first = startIndex;
        const unsigned int second = startIndex + 1;
        const unsigned int third = endIndex;
        
        const double d1 = calcEuclidianDistance(points[first], points[second]);
        const double d2 = calcEuclidianDistance(points[first], points[third]);
        const double d3 = calcEuclidianDistance(points[second], points[third]);
        output.distance = min3(d1, d2, d3);

        if(isEqual(output.distance, d1)) {
            output.p1 = *points[first];
            output.p2 = *points[second];
        } else if(isEqual(output.distance, d2)) {
            output.p1 = *points[first];
            output.p2 = *points[third];
        } else {
            output.p1 = *points[second];
            output.p2 = *points[third];
        }
    } else {
        // Algoritmo - Parte 1:
        // Dividir e conquistar para encontrar a menor distância em x entre dois pontos
        // deltaLeft, deltaRight, median
        // delta = min(deltaLeft, deltaRight)
        // Problema: Ao encontrar, não é garantido que seja menor distância na borda

        const unsigned int median = startIndex + length / 2;
        const unsigned int leftStart = startIndex;
        const unsigned int leftEnd = median - 1;
        const unsigned int rightStart = median;
        const unsigned int rightEnd = endIndex;

        const OutputData outputLeft = findClosestPairOfPointsRecursive(points, leftStart, leftEnd);
        const OutputData outputRight = findClosestPairOfPointsRecursive(points, rightStart, rightEnd);
        const double delta = min2(outputLeft.distance, outputRight.distance);

        if(isEqual(delta, outputLeft.distance)) {
            output.p1 = *points[leftStart];
            output.p2 = *points[leftEnd];
        } else {
            output.p1 = *points[rightStart];
            output.p2 = *points[rightEnd];
        }

        output.distance = delta;

        // Algoritmo - Parte 2:
        // Depois de encontrar a menor distância de delta em x, procuramos entre:
        //  [x - delta, x + delta] porque qualquer um maior que isso teria um x maior
        //  do que a menor distância em x
        //  Portanto, o range em x fará uma faixa que tem largura 2 * delta
        // Quadrado 2 * delta largura por delta altura
    }

    return output;
}

OutputData findClosestPairOfPoints(const InputData* input) {
    clock_t startTime = clock();

    const Point** sortedPoints = malloc(sizeof(Point) * input->count);
    memcpy(sortedPoints, input->points, sizeof(Point) * input->count);
    
    // C std qsort (quicksort): O(nlogn)
    qsort(sortedPoints, input->count, sizeof(Point*), sortPointsByXAscendingComparator);

    unsigned int startIndex = 0;
    unsigned int endIndex = input->count - 1;
    OutputData output = findClosestPairOfPointsRecursive(sortedPoints, 0, input->count - 1);
    
    free(sortedPoints);

    clock_t endTime = clock();
    output.timeSecs = (double)(endTime - startTime) / CLOCKS_PER_SEC;
    
    return output;
}

void logOutputData(const OutputData* data, int isVerbose) {
    if(isVerbose) {
        puts("Saída :");
        printf("\tTempo: %f\n", data->timeSecs);
        printf("\tDistância: %f\n", data->distance);
        printf("\tx1: %f\n", data->p1.x);
        printf("\ty1: %f\n", data->p1.y);
        printf("\tx2: %f\n", data->p2.x);
        printf("\ty2: %f\n", data->p2.y);
    } else {
        printf(
            "%.6f %.6f %.6f %.6f %.6f %.6f\n", 
            data->timeSecs,
            data->distance,
            data->p1.x,
            data->p1.y,
            data->p2.x,
            data->p2.y
        );
    }
}

int main(int argc, char **argv) {
    if(argc != 2) {
        puts("Invalid inputs. Usage: \n.$ /closest /path/to/genpoints/input.txt");
        return 1;
    }

    char* path = argv[1];
    char* buffer = allocReadFileContent(path);
    InputData* inputData = allocProcessInputData(buffer);
    
    // printf("%s\n", buffer);
    // logInputData(inputData);

    OutputData outputData1 = findClosestPairOfPointsNaive(inputData);
    logOutputData(&outputData1, 1);

    OutputData outputData2 = findClosestPairOfPoints(inputData);
    logOutputData(&outputData2, 1);

    free(buffer);
    freeInputData(inputData);

    return 0;
}
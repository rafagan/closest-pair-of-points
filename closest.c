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

InputData allocProcessInputData(const char* input) {
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

    InputData data;
    data.count = pointCount;
    data.points = points;

    return data;
}

void freeInputDataPoints(InputData inputData) {
    for(unsigned int i = 0; i < inputData.count; i++) {
        free(inputData.points[i]);
    }
}

void logInputData(InputData data) {
    puts("Entrada :");
    for(unsigned int i = 0; i < data.count; i++) {
        Point* point = data.points[i];
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

OutputData findClosestPairOfPointsNaive(InputData input) {
    Point* p1;
    Point* p2;
    double distance = DBL_MAX;

    clock_t startTime = clock();

    // Brute force time complexity: O(n^2)
    for(int i = 0; i < input.count; i++) {
        Point* _p1 = input.points[i];
        for(int j = i + 1; j < input.count; j++) {
            Point* _p2 = input.points[j];
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

int sortPointsByYAscendingComparator(const void* a, const void* b) {
   const double y1 = (*((Point**)a))->y;
   const double y2 = (*(Point**)b)->y;
   return y1 - y2;
}

InputData findPointsInStripeYAxisSorted(InputData input, const Point* midPoint, double delta) {
    InputData output;
    output.points = malloc(sizeof(Point) * input.count);
    output.count = 0;
    for(int i = 0; i < input.count; i++) {
        Point* p = input.points[i];
        if(p == midPoint) continue;
        if(fabs(p->x - midPoint->x) < delta) {
            output.points[output.count++] = p;
        }
    }

    output.points = (Point**) realloc(output.points, sizeof(Point) * output.count);
    qsort(output.points, output.count, sizeof(Point*), sortPointsByYAscendingComparator);
    
    return output;
}

OutputData allocFindClosestPairOfPointsRecursive(
    InputData input,
    unsigned int startIndex, 
    unsigned int endIndex
) {
    OutputData output;
    const unsigned int length = endIndex - startIndex + 1;

    if(length == 2) {
        output.p1 = *input.points[startIndex];
        output.p2 = *input.points[endIndex];
        output.distance = calcEuclidianDistance(&output.p1, &output.p2);
    } else if(length == 3) {
        const unsigned int first = startIndex;
        const unsigned int second = startIndex + 1;
        const unsigned int third = endIndex;
        
        const double d1 = calcEuclidianDistance(input.points[first], input.points[second]);
        const double d2 = calcEuclidianDistance(input.points[first], input.points[third]);
        const double d3 = calcEuclidianDistance(input.points[second], input.points[third]);
        output.distance = min3(d1, d2, d3);

        if(isEqual(output.distance, d1)) {
            output.p1 = *input.points[first];
            output.p2 = *input.points[second];
        } else if(isEqual(output.distance, d2)) {
            output.p1 = *input.points[first];
            output.p2 = *input.points[third];
        } else {
            output.p1 = *input.points[second];
            output.p2 = *input.points[third];
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

        const OutputData outputLeft = allocFindClosestPairOfPointsRecursive(input, leftStart, leftEnd);
        const OutputData outputRight = allocFindClosestPairOfPointsRecursive(input, rightStart, rightEnd);
        const double delta = min2(outputLeft.distance, outputRight.distance);

        if(isEqual(delta, outputLeft.distance)) {
            output = outputLeft;
        } else {
            output = outputRight;
        }

        // Algoritmo - Parte 2:
        // Depois de encontrar a menor distância de delta em x, procuramos entre:
        //  [x - delta, x + delta] porque qualquer um maior que isso teria uma distancia
        //  do que o delta encontrado
        //  Portanto, o range em x fará uma faixa que tem largura 2 * delta
        // Fazendo uma análise cuidadosa, é possível perceber que em uma área
        //  com largura 2delta e altura delta próxima dos botões existem no máximo 8 pontos
        //  que podem ter uma distância menor, representando os 8 quadrantes,
        //  4 à esquerda da borda e 4 à direita
        // Considerando que um dos 8 pontos é o nosso encontrado, restam 7 comparações
        // Isso faz com que essa parte do algoritmo seja O(1), e o todo seja O(nlogn)

        const Point* midPoint = input.points[median];
        InputData stripePointsData = findPointsInStripeYAxisSorted(
            input, midPoint, delta
        );

        for(int i = 0; i < stripePointsData.count; i++) {
            Point* p1 = stripePointsData.points[i];
            for(int j = i + 1; j < stripePointsData.count; j++) {
                Point* p2 = stripePointsData.points[j];
                if(p2->y - p1->y >= output.distance) break;
                double d = calcEuclidianDistance(p1, p2);
                if(d < output.distance) {
                    output.distance = d;
                    output.p1 = *p2;
                    output.p2 = *p2;
                }
            }
        }

        free(stripePointsData.points);
    }

    return output;
}

OutputData findClosestPairOfPoints(InputData input) {
    clock_t startTime = clock();

    InputData sortedInputData;
    sortedInputData.count = input.count;
    sortedInputData.points = malloc(sizeof(Point) * input.count);
    memcpy(sortedInputData.points, input.points, sizeof(Point) * input.count);
    
    // C std qsort (quicksort): O(nlogn)
    qsort(sortedInputData.points, input.count, sizeof(Point*), sortPointsByXAscendingComparator);

    unsigned int startIndex = 0;
    unsigned int endIndex = input.count - 1;
    OutputData output = allocFindClosestPairOfPointsRecursive(
        sortedInputData, 0, input.count - 1
    );
    
    free(sortedInputData.points);

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
    InputData inputData = allocProcessInputData(buffer);
    
    // printf("%s\n", buffer);
    // logInputData(inputData);

    // OutputData outputData1 = findClosestPairOfPointsNaive(inputData);
    // logOutputData(&outputData1, 0);

    OutputData outputData2 = findClosestPairOfPoints(inputData);
    logOutputData(&outputData2, 0);

    free(buffer);

    return 0;
}
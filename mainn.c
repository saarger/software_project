
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define int_max 2147483647

int global_delta_uk = int_max;

typedef struct {
    float* data;
    size_t size;
} Vector;


Vector* open_file(const char* filename, size_t* num_vectors) {
    Vector* vectors = NULL;
    FILE* f = fopen(filename, "r");
    if (f == NULL) {
        perror("Failed to open file");
        return NULL;
    }
    size_t bufsize = 256;
    char* buffer = malloc(bufsize * sizeof(char));
    if (buffer == NULL) {
        perror("Failed to allocate memory for buffer");
        fclose(f);
        return NULL;
    }
    size_t buflen = 0;
    *num_vectors = 0;
    int c;
    Vector vector = { .data = NULL, .size = 0 };
    while ((c = fgetc(f)) != EOF) {
        if (c == ',' || c == '\n') {
            buffer[buflen] = '\0';
            float value = strtof(buffer, NULL);
            vector.data = realloc(vector.data, (vector.size+1) * sizeof(float));
            if (vector.data == NULL) {
                perror("Failed to allocate memory for vector");
                fclose(f);
                free(buffer);
                return NULL;
            }
            vector.data[vector.size] = value;
            vector.size++;
            buflen = 0;
            if (c == '\n') {
                vectors = realloc(vectors, (*num_vectors+1) * sizeof(Vector));
                if (vectors == NULL) {
                    perror("Failed to allocate memory for vectors");
                    fclose(f);
                    free(buffer);
                    return NULL;
                }
                vectors[*num_vectors] = vector;
                (*num_vectors)++;
                vector = (Vector) { .data = NULL, .size = 0 };
            }
        } else {
            if (buflen == bufsize) {
                bufsize *= 2;
                buffer = realloc(buffer, bufsize * sizeof(char));
                if (buffer == NULL) {
                    perror("Failed to allocate memory for buffer");
                    fclose(f);
                    return NULL;
                }
            }
            buffer[buflen] = c;
            buflen++;
        }
    }
    free(buffer);
    fclose(f);
    return vectors;
}
void print_vector(Vector* vector) {
    printf("[");
    for (int i = 0; i < vector->size; i++) {
        printf("%f", vector->data[i]);
        if (i < vector->size-1) {
            printf(", ");
        }
    }
    printf("]\n");
}

void free_vectors(Vector* vectors, size_t num_vectors) {
    for (int i = 0; i < num_vectors; i++) {
        free(vectors[i].data);
    }
    free(vectors);
}

float d(Vector* p,Vector* q){
    int sum = 0;
    for (int i = 0; i < p->size; i++) {
        sum += (p->data[i] - q->data[i]) * (p->data[i] - q->data[i]);
    }
    return sqrt(sum);
}

Vector divideVector(Vector* p,int num){
    Vector div = { .data = NULL, .size = p->size };
    for (int i = 0; i < p->size; i++) {
        div.data = realloc(div.data, (div.size+1) * sizeof(float));
        if (div.data == NULL) {
            perror("Failed to allocate memory for vector");
            return div;
        }
        div.data[i] = p->data[i] / num;
    }

    return div;
}

Vector sumVector(Vector* p,Vector* q){
    Vector sum = { .data = NULL, .size = p->size };
    for (int i = 0; i < p->size; i++) {
        sum.data = realloc(sum.data, (sum.size+1) * sizeof(float));
        if (sum.data == NULL) {
            perror("Failed to allocate memory for vector");
            return sum;
        }
        sum.data[i] = p->data[i] + q->data[i];
    }

    return sum;
}

Vector* update_centroids(Vector* centroids,int* vector_cluster,int k,int num_vectors,Vector* vectors){
    Vector* new_centroids = malloc(k * sizeof(Vector));
    if (new_centroids == NULL) {
        perror("Failed to allocate memory for new_centroids");
        return "An Error Has Occurred";
    }
    int maxdeltaUk = 0;
    int deltauk = 0;
    for (int i = 0; i < k; i++) {
        Vector sum = { .data = calloc(vectors[0].size, sizeof(float)), .size = vectors[0].size };
        int count = 0;
        for (int j = 0; j < num_vectors; j++) {
            if (vector_cluster[j] == i){
                sum = sumVector(&vectors[j],&sum);
                count++;
            }
        }
        new_centroids[i] = divideVector(&sum,count);
        deltauk = d(&new_centroids[i],&centroids[i]);
        if (deltauk > maxdeltaUk){
            maxdeltaUk = deltauk;
        }
    }
    global_delta_uk = maxdeltaUk;
    free_vectors(centroids, k);
    return new_centroids;
}

Vector* Kmeans(int k,int iter,char *InputData){
    int epsilon = 0.001;
    size_t num_vectors;
    Vector* vectors = open_file("/Users/oit/Desktop/tests/input_1.txt", &num_vectors);
    if (vectors == NULL) {
        return "An Error Has Occurred";
    }
    if (k >= num_vectors && k <= 1){
        return "An Error Has Occurred";
    }
    Vector* centroids = malloc(k * sizeof(Vector));
    if (centroids == NULL) {
        perror("Failed to allocate memory for centroids");
        return "An Error Has Occurred";
    }
    for (int i = 0; i < k; ++i) {
        centroids[i] = vectors[i];
    }
    int i = 0;
    while (i < iter && global_delta_uk>epsilon){
        int* vector_cluster = malloc(num_vectors * sizeof(int));
        if (vector_cluster == NULL) {
            perror("Failed to allocate memory for vector_cluster");
            return "An Error Has Occurred";
        }
        for (int j = 0;j < num_vectors; j++) {
            int min = int_max;
            for (int l = 0;l<k;l++){
                float distance = d(&vectors[j],&centroids[l]);
                if (distance < min){
                    min = distance;
                    vector_cluster[j] = l;
                }
            }
        }
        centroids = update_centroids(centroids,vector_cluster,k,num_vectors,vectors);
        i++;
        free(vector_cluster);
    }
    return centroids;
}




int main() {
    size_t num_vectors;

    Vector *centroids = Kmeans(3, 600, "/Users/oit/Desktop/tests/input_1.txt");
    if (centroids == NULL) {
        return 1;
    }
    // Do something with the vectors
    for (size_t i = 0; i < num_vectors; i++) {
        printf("Vector %zu: [", i);
        for (size_t j = 0; j < centroids[i].size; j++) {
            printf("%f", centroids[i].data[j]);
            if (j < centroids[i].size - 1) {
                printf(", ");
            }
        }
        printf("]\n");
    }
        // Free memory
        for (size_t i = 0; i < num_vectors; i++) {
            free(centroids[i].data);
        }
        free(centroids);
        return 0;

    }


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define int_max 2147483647

double global_delta_uk = 1000000;
int num_vectors,size_vec;
typedef struct {
    double* data;
    size_t size;
} Vector;
double* sumVector(double* p, double* q);
double** Kmeans(int k,int iter,char *InputData);
double** read_file(char *input_filename);
double d(double* p, double* q);
double* divideVector(double* p, double num);
void assert(void* pointer);

double d(double* p, double* q){
    int sum=0;
    for(int i=0;i<size_vec;i++){
        sum += pow(p[i] - q[i],2);
    }
    return sqrt(sum);
}
void assert(void* pointer) {
    if (pointer == NULL) {
        fprintf(stderr, "%s\n", "An Error Has Occurred");
        exit(EXIT_FAILURE);
    }
}

double* divideVector(double* p, double num) {
    double* div = malloc(size_vec * sizeof(double));
    assert(div);

    for (int i = 0; i < size_vec; i++) {
        div[i] = p[i] / num;
    }

    return div;
}

double* sumVector(double* p, double* q){
    double* sum = calloc(size_vec,sizeof(double));
    assert(sum);
    for (int i = 0; i < size_vec; i++) {
        sum[i] = p[i] + q[i];
    }

    return sum;
}

double** update_centroids(double** centroids,int* vector_cluster,int k,double** vectors){
    double** new_centroids = (double **)calloc(k,sizeof(double*));
    assert(new_centroids);
    for (int i = 0 ; i < k ; i++)
    {
        new_centroids[i] = (double *)calloc(size_vec, sizeof(double));
        assert(new_centroids[i]);
    }
    double maxdeltaUk = 0;
    double deltauk = 0;
    for (int i = 0; i < k; i++) {
        double* sum = (double *)calloc(size_vec, sizeof(double));
        assert(sum);
        int count = 0;
        for (int j = 0; j < num_vectors; j++) {
            if (vector_cluster[j] == i){
                sum = sumVector(vectors[j],sum);
                count++;
            }
        }
        new_centroids[i] = divideVector(sum,count);
        deltauk = d(new_centroids[i],centroids[i]);
        if (deltauk > maxdeltaUk){
            maxdeltaUk = deltauk;
        }
    }
    global_delta_uk = maxdeltaUk;
    return new_centroids;
}


double** Kmeans(int k,int iter,char *InputData){
    double epsilon = 0.001;
    double** vectors = read_file(InputData);
    assert(vectors);

    if (k >= num_vectors && k <= 1){
        fprintf(stderr, "%s\n", "Invalid number of clusters!");
        exit(EXIT_FAILURE);
    }
    double** centroids = (double **)calloc(k,sizeof(double*));
    assert(centroids);
    for (int i = 0 ; i < k ; i++)
    {
        centroids[i] = (double *)calloc(size_vec, sizeof(double));
        assert(centroids[i]);
    }
    for (int i = 0; i < k; ++i) {
        centroids[i] = vectors[i];
    }
    int i = 0;
    while (i < iter && global_delta_uk>epsilon){
        int* vector_cluster = (int*)calloc(num_vectors , sizeof(int));
        assert(vector_cluster);
        for (int j = 0;j < num_vectors; j++) {
            int min = int_max;
            for (int l = 0;l<k;l++){
                double distance = d(vectors[j],centroids[l]);
                if (distance < min){
                    min = distance;
                    vector_cluster[j] = l;
                }
            }
        }
        centroids = update_centroids(centroids,vector_cluster,k,vectors);
        i++;
        free(vector_cluster);
    }
    return centroids;
}

double** read_file(char *input_filename)
{
    int index=0, line=0;
    double a;
    char b;
    double **data_points;
    FILE *ipf;

    /* count cordinate number in vector */

    ipf = fopen(input_filename, "r");
    assert(ipf);
    size_vec = 0;
    while ((fscanf(ipf,"%lf",&a) == 1)){
        b = fgetc(ipf);
        size_vec++;
        if( b != ','){
            break;
        }
    }
    fclose(ipf);

    /* count total number of vectors */

    ipf = fopen(input_filename, "r");
    assert(ipf);
    num_vectors = 0;
    while ((fscanf(ipf,"%lf%c", &a, &b) == 2)){
        if (b != ',')
        {
            num_vectors += 1;
        }
    }
/*     for(b = fgetc(ipf) ; b != EOF ; b = fgetc(ipf)){
        if(b == '\n'){
            num_vectors += 1;
        }
    } */
    fclose(ipf);

    /* main read section */

    data_points = (double **)calloc(num_vectors,sizeof(double*));
    assert(data_points);
    ipf = fopen(input_filename, "r");
    assert(ipf);
    for(line = 0 ; line < num_vectors ; line++)
    {
        data_points[line] = calloc(size_vec,sizeof(double));
        assert(data_points[line]);
        for(index = 0 ; index < size_vec ; index++)
        {
            fscanf(ipf,"%lf",&a);
            data_points[line][index] = a;
            fgetc(ipf);
        }
    }
    fclose(ipf);
    return data_points;
}


int main() {
    double** x = Kmeans(7,1000,"/Users/nadav/Downloads/tests/input_2.txt");

    for(int i=0;i<7;i++){
        for(int j=0;j<size_vec;j++){
            printf("%f",x[i][j]);
            printf(",");
        }
        printf("\n");
    }

}
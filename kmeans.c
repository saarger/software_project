
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define int_max 2147483647

double global_delta_uk = 1000000;
int num_vectors=0;
int size_vec = 1;
char c;
double vec;


double* sumVector(double* p, double* q);
double** Kmeans(int k,int iter);
double** read_file(char *input_filename);
double d(double* p, double* q);
double* divideVector(double* p, double num);
void assert(void* pointer);
int isInt(const char* str);
void printVectors(double** res,int k);


void printVectors(double** res,int k){
    int i;
    int j;
    for(i=0;i<k;i++){
        if(i != 0)
        {
            printf("\n");
        }
        for(j=0;j<size_vec;j++){
            printf("%.4f",res[i][j]);
            if(j != size_vec-1) {
                printf(",");
            }
        }
    }
    printf("\n");
}

int isInt(const char* str) {
    char* endptr;
    strtol(str, &endptr, 10);
    return (*str != '\0' && *endptr == '\0');
}

double d(double* p, double* q){
    double sum=0;
    int i;
    for(i=0;i<size_vec;i++){
        sum = sum + pow(p[i] - q[i],2);
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
    int i;
    double* div = malloc(size_vec * sizeof(double));
    assert(div);

    for (i = 0; i < size_vec; i++) {
        div[i] = p[i] / num;
    }

    return div;
}

double* sumVector(double* p, double* q){
    int i;
    double* sum = calloc(size_vec,sizeof(double));
    assert(sum);

    for (i = 0; i < size_vec; i++) {
        sum[i] = p[i] + q[i];
    }

    return sum;
}

double** update_centroids(double** centroids,int* vector_cluster,int k,double** vectors){
    int i;
    int j;
    int count;
    double maxdeltaUk;
    double deltauk;
    double** new_centroids = (double **)calloc(k,sizeof(double*));
    assert(new_centroids);

    for (i = 0 ; i < k ; i++)
    {
        new_centroids[i] = (double *)calloc(size_vec, sizeof(double));
        assert(new_centroids[i]);
    }
    maxdeltaUk = 0;
    deltauk = 0;
    for (i = 0; i < k; i++) {
        double* sum = (double *)calloc(size_vec, sizeof(double));
        assert(sum);
        count = 0;
        for (j = 0; j < num_vectors; j++) {
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

    free(centroids);
    global_delta_uk = maxdeltaUk;
    centroids = new_centroids;
    for (i = 0; i < k; i++) {
        free(new_centroids[i]);
    }
    free(new_centroids);
    return centroids;
}

double** load_vectors(double** vectors){
    int i;
    int j;
    for(i=0;i<num_vectors;

    i++){
        vectors[i] = calloc(size_vec,sizeof(double));
        assert(vectors[i]);

        for(j=0;j<size_vec;j++){
            scanf("%lf%c",&vec,&c);
            vectors[i][j] = vec;
        }
    }
    return vectors;
}

double** Kmeans(int k,int iter){
    double epsilon = 0.001;
    int j;
    int i;
    int l;
    double** centroids;
    double** vectors=NULL;
    vectors = calloc(num_vectors,sizeof(double*));
    assert(vectors);
    vectors = load_vectors(vectors);
    assert(vectors);


    if (k >= num_vectors && k <= 1){
        fprintf(stderr, "%s\n", "Invalid number of clusters!");
        exit(EXIT_FAILURE);
    }
    centroids = (double **)calloc(k,sizeof(double*));
    assert(centroids);
    for (i = 0 ; i < k ; i++)
    {
        centroids[i] = (double *)calloc(size_vec, sizeof(double));
        assert(centroids[i]);
    }
    for (i = 0; i < k; ++i) {
        centroids[i] = vectors[i];
    }
    i = 0;
    while (i < iter && global_delta_uk>epsilon){
        int* vector_cluster = (int*)calloc(num_vectors , sizeof(int));
        assert(vector_cluster);
        for (j = 0;j < num_vectors; j++) {
            double min = int_max;
            for (l = 0;l<k;l++){
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
    for (i = 0; i < num_vectors; i++) {
        free(vectors[i]);
    }
    free(vectors);
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


int main(int argc,char *argv[]) {
    int iter = 200;
    int k;
    int bool=1;
    double** res;

    if (argc < 2 || argc > 3)
    {
        assert(NULL);
    }
    if (isInt(argv[1]) != 1)
    {
        assert(NULL);
    }
    k = atoi(argv[1]);
    if (argc == 3)
    {
        if (isInt(argv[2]) == 1)
        {
            iter = atoi(argv[2]);
            if(iter <= 0 || iter >= 1000)
            {
                fprintf(stderr, "%s\n", "Invalid maximum iteration!");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            assert(NULL);
        }

    }

    /**count num vectors and size of vectors**/
    while (scanf("%lf%c", &vec, &c) == 2)
    {
        if (c == ',' && bool){
            size_vec++;
        }
        if (c == '\n') {

            num_vectors++;
            bool = 0;
        }
    }
    rewind(stdin);
    res = Kmeans(k,iter);
    printVectors(res,k);
    for (int i = 0; i < k; i++) {
        free(res[i]);
    }
    free(res);
    return 0;
}


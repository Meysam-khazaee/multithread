#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <openssl/sha.h>
#include <math.h>

#define ITERATION_NUMBER (int)pow(2,20)
#define THREAD_NUMBER 10

typedef struct {
    int thread_id;
    char ***last_sha1s;
    const char *input;
} thread_data_t;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// pthread_cond_t condition = PTHREAD_COND_INITIALIZER;

void *thread_function(void *arg) {
    thread_data_t *data = (thread_data_t *)arg;
    int group_mate = data->thread_id ^ 1;  // XOR with 1 for finding group mate    
    for (int counter = 0; counter < ITERATION_NUMBER; counter++) {
        char data_str[(sizeof(data->thread_id)/sizeof(int)) + (SHA_DIGEST_LENGTH * 2 + 1) + (sizeof(counter)/sizeof(int)) + strlen(data->input) + 1];
        
        if (counter == 0) {
            snprintf(data_str, sizeof(data_str), "%d%d%s", data->thread_id, counter, data->input);
        } 
        else if (strlen(data->last_sha1s[group_mate][counter - 1]) != 0) {
            pthread_mutex_lock(&mutex);
            snprintf(data_str, sizeof(data_str), "%d%s%d%s", data->thread_id, data->last_sha1s[group_mate][counter - 1], counter, data->input);
            pthread_mutex_unlock(&mutex);
        }
        else{
            //thread_cond_wait(&condition, &mutex);
            do {
                //printf(".");
            } while(strlen(data->last_sha1s[group_mate][counter - 1]) == 0);
            pthread_mutex_lock(&mutex);
            snprintf(data_str, sizeof(data_str), "%d%s%d%s", data->thread_id, data->last_sha1s[group_mate][counter - 1], counter, data->input);
            pthread_mutex_unlock(&mutex);
        }

        unsigned char sha1[SHA_DIGEST_LENGTH + 1];
        SHA1((unsigned char *)data_str, strlen(data_str), sha1);

        char hex_sha1[SHA_DIGEST_LENGTH * 2 + 1];
        for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
            snprintf(hex_sha1 + i * 2, 3, "%02x", sha1[i]);
        }

        strcpy(data->last_sha1s[data->thread_id][counter], hex_sha1);
    }
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}

char *new_hash_function(char *input, int k) {
    pthread_t threads[k];
    char*** last_sha1s = (char***)calloc(k,sizeof(char**));

    for (int i = 0; i < k; i++) {
        last_sha1s[i] = (char**)calloc(ITERATION_NUMBER, sizeof(char*));
        for (int j = 0; j < ITERATION_NUMBER; j++) {
            last_sha1s[i][j] = (char *)calloc((SHA_DIGEST_LENGTH * 2 + 1),sizeof(char));
        }
    }

    thread_data_t data[k];
    for (int i = 0; i < k; i++) {
        data[i].thread_id = i;
        data[i].last_sha1s = last_sha1s;
        data[i].input = input;
        pthread_create(&threads[i], NULL, thread_function, &data[i]);
    }

    for (int i = 0; i < k; i++) {
        pthread_join(threads[i], NULL);
    }

    char final_hash[k * SHA_DIGEST_LENGTH * 2 + 1];
    memset(final_hash, 0, sizeof(final_hash));
    for (int i = 0; i < k; i++) {
        strcat(final_hash, last_sha1s[i][ITERATION_NUMBER - 1]);
    }

    unsigned char overall_sha1[SHA_DIGEST_LENGTH + 1];
    SHA1((unsigned char *)final_hash, strlen(final_hash), overall_sha1);

    char hex_overall_sha1[SHA_DIGEST_LENGTH * 2 + 1];
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        snprintf(hex_overall_sha1 + i * 2, 3, "%02x", overall_sha1[i]);
    }

    for (int i = 0; i < k; i++) {
        for (int j = 0; j < ITERATION_NUMBER; j++) {
            if (NULL != last_sha1s[i][j]){
                free(last_sha1s[i][j]);
            }
        }
        free(last_sha1s[i]);
    }
    free(last_sha1s);

    return strdup(hex_overall_sha1);
}

int main() {
    double start_time = clock();
    char *hash_result = new_hash_function("meysam_khazaee", THREAD_NUMBER);

    double end_time = clock();
    double execution_time = (end_time - start_time) / CLOCKS_PER_SEC;

    printf("Hash Value: %s\n", hash_result);
    printf("Execution Time: %.18f seconds\n", execution_time);

    free(hash_result);
    return 0;
}
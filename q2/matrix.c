#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>


#define NUM_THREAD 4
#define ROW_A 2
#define COL_A 3
#define ROW_B 3
#define COL_B 2

// Global matrices (shared between threads)
int **A, **B, **Cs, **Cp;

// Function to fill matrix A with sample data from matrixA.txt and matrixB.txt
void initialize_matrix() {
    // Read matrix A from matrixA.txt in the format:
    // 1 2 3
    // 4 5 6
    // Use malloc to dynamically allocate A[ROW_A][COL_A]
    // Similarly, read matrix B from matrixB.txt:
    // 7 8
    // 9 10
    // 11 12
    // Use malloc to dynamically allocate B[ROW_B][COL_B]
    // TODO
    FILE *fa = fopen("matrixA.txt", "r");
    FILE *fb = fopen("matrixB.txt", "r");

    if (!fa || !fb) {
        printf("Error reading input files!\n");
        exit(1);
    }

    A = malloc(ROW_A * sizeof(int *));
    for (int i = 0; i < ROW_A; i++)
        A[i] = malloc(COL_A * sizeof(int));

    B = malloc(ROW_B * sizeof(int *));
    for (int i = 0; i < ROW_B; i++)
        B[i] = malloc(COL_B * sizeof(int));

    Cs = malloc(ROW_A * sizeof(int *));
    Cp = malloc(ROW_A * sizeof(int *));
    for (int i = 0; i < ROW_A; i++) {
        Cs[i] = malloc(COL_B * sizeof(int));
        Cp[i] = malloc(COL_B * sizeof(int));
    }

    for (int i = 0; i < ROW_A; i++) {
        for (int j = 0; j < COL_A; j++) {
            fscanf(fa, "%d", &A[i][j]);
        }
    }

    for (int i = 0; i < ROW_B; i++) {
        for (int j = 0; j < COL_B; j++) {
            fscanf(fb, "%d", &B[i][j]);
        }
    }

    fclose(fa);
    fclose(fb);
}

// Save data in a file
void save_matrix() {
    // Write the C matrix result into a file matrixC.txt as this format
    // 58    64
    // 139  154-
    // TODO
    FILE *file = fopen("matrixC.txt", "w");  // matrixC.txt dosyasını yazma modunda aç

    if (file == NULL) {
        printf("Error opening file for writing!\n");
        return;
    }

    // Cp matrisini dosyaya yaz
    for (int i = 0; i < ROW_A; i++) {
        for (int j = 0; j < COL_B; j++) {
            fprintf(file, "%d ", Cp[i][j]);  // Her elemanı yaz
        }
        fprintf(file, "\n");  // Her satır sonunda yeni satıra geç
    }

    fclose(file);  // Dosyayı kapat
    printf("Matrix C saved to matrixC.txt\n");
}
// Function to print a matrix
void print_matrix(int rows, int cols, int **matrix) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

// Serial function to compute dot product
void dot_serial() {
    // dot product logic here
    // TODO
    for (int i = 0; i < ROW_A; i++) {
        for (int j = 0; j < COL_B; j++) {
            Cs[i][j] = 0;
            for (int k = 0; k < COL_A; k++) {
                Cs[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

#include <unistd.h>
// Multithreaded dot product worker function
void *dot_worker(void *arg) {    
    //TODO 
    //TODO : compute one element Cp[r][c] = sum(A[r][k] * B[k][c])
    int *id = (int *)arg;
    int i = id[0];
    int j = id[1];

    Cp[i][j] = 0;
    for (int k = 0; k < COL_A; k++)
    {
        Cp[i][j] += A[i][k] * B[k][j];
    }
    sleep(5);
    pthread_exit(NULL);
}


// Timing helper function
double get_time_diff(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

int main() {
    struct timespec start, end;
    double ts, tp;

    // Initialize matrix A and B
    initialize_matrix();        // Read matrix from a file
    printf("Matrix A:\n");
    print_matrix(ROW_A, COL_A, A);
	printf("Matrix B:\n");
    print_matrix(ROW_B, COL_B, B);
    // Serial dot product
    clock_gettime(CLOCK_MONOTONIC, &start);
    dot_serial();
    clock_gettime(CLOCK_MONOTONIC, &end);
    ts = get_time_diff(start, end);
    printf("Serial dot product Matrix C:\n");
    print_matrix(ROW_A, COL_B, Cs);
    printf("Serial dot product time: %f seconds\n", ts);

    clock_gettime(CLOCK_MONOTONIC, &start);
    // **Multithreaded product**
    pthread_t dotProduct_threads[4];
    int id[4][2];
    // Create threads for transposing here using for loop
	// TODO
	// Create threads for transposing here using for loop
    for (int t = 0; t < 4; t++) {
        id[t][0] = t / 2;  // Row
        id[t][1] = t % 2;   // Column
        pthread_create(&dotProduct_threads[t], NULL, dot_worker, (void *)id[t]);
    }
    // Join threads here
    //TODO
    for (int t = 0; t < 4; t++) {
        pthread_join(dotProduct_threads[t], NULL);
    }
	clock_gettime(CLOCK_MONOTONIC, &end);
    tp = get_time_diff(start, end);
    printf("Multithreaded dot product Matrix C:\n");
    print_matrix(ROW_A, COL_B, Cp);
    printf("Multithreaded dot product time: %f seconds\n", tp);
    save_matrix();

    return 0;
}

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<pthread.h>
#include<time.h>
#include<unistd.h>

//global variables
int row1, column1, row2, column2;
int matrix1[1000][1000];
int matrix2[1000][1000];
int result[1000][1000];
int result2[1000][1000];
int i=0,j=0,k=0;
double timeElapsed1;
double timeElapsed2;

//a struct to send row and column to each thread in by elements
typedef struct arguments{
int indexR;
int indexC;
} args;

//declration of the functions used
void *multByRow(void *arg);
void *multByElement(void *arg);
void fileReader(char *filename);
void fileWriter(char *filename);

//main
int main()
{
char fileinput[255] ;
char *fileOutput = "outputMatrix.txt";

printf("Please enter the filename: ");
scanf("%s",fileinput);

//reading the matrices from input file
fileReader(fileinput);

//printing first matrix after being read
printf("Matrix1\n");
for(i=0; i<row1; i++)
{
for(j=0; j<column1; j++)
{
if(j == column1-1)
printf("%d\n", matrix1[i][j]);
else
printf("%d ", matrix1[i][j]);
}
}

//printing second matrix after being read
printf("Matrix2\n");
for(i=0; i<row2; i++)
{
for(j=0; j<column2; j++)
{
if(j == column2-1)
printf("%d\n", matrix2[i][j]);
else
printf("%d ", matrix2[i][j]);
}
}

//the matrix multiplication condition
if(row2 == column1)
{
//creating thread for every row
pthread_t th[row1];
int index[row1];
int threadCount2 = 0;

//start timer
clock_t begin1 = clock();

//for loop to create threads
for(i=0; i<row1; i++)
{
index[threadCount2]=i;
pthread_create(&th[threadCount2], NULL, &multByRow,(void *)&(index[threadCount2]));

threadCount2 ++;
}

//for loop to join threads
for(i=0; i<row1; i++)
pthread_join(th[i], NULL);

//end timer
clock_t end1 = clock();
timeElapsed1 = (double)(end1-begin1)/CLOCKS_PER_SEC;

//creating thread for every element
pthread_t th2[row1*column1];
int threadCount = 0;
args par[row1*column2];

//start timer
clock_t begin2 = clock();

//for loop to create threads
for(i=0; i<row1; i++)
{
for(j=0; j<column2; j++)
{
par[threadCount].indexR = i;
par[threadCount].indexC = j;

pthread_create(&th2[threadCount], NULL, &multByElement, (void *)&par[threadCount]);

threadCount ++;
}
}

//for loop to join threads
for(i=0; i<row1*column2; i++)
pthread_join(th2[i], NULL);

//end timer
clock_t end2 = clock();
timeElapsed2 = (double)(end2-begin2)/CLOCKS_PER_SEC;
}

//printing the matrix multiplied by row
printf("By row:\n");
for(i=0; i<row1; i++)
{
for(j=0; j<column2; j++)
{
if(j == column2-1)
printf("%d\n", result[i][j]);
else
printf("%d ", result[i][j]);
}
}

//printing the matrix multiplied by element
printf("By element:\n");
for(i=0; i<row1; i++)
{
for(j=0; j<column2; j++)
{
if(j == column2-1)
printf("%d\n", result2[i][j]);
else
printf("%d ", result2[i][j]);
}
}

//writing the results to output file
fileWriter(fileOutput);

return 0;
}

//matrix multiplication by row function
void *multByRow(void *arg)
{
int *r = arg;
int l, m;
for(l=0; l<column2; l++)
{
result[*r][l] = 0;

for(m=0; m<column1; m++)
{
result[*r][l] = result[*r][l] + (matrix1[*r][m]*matrix2[m][l]);
}
}
}

//matrix multiplication by element function
void *multByElement(void *arg)
{
args* par = arg;
int l;

for(l=0; l<column1; l++)
{
result2[par->indexR][par->indexC] = result2[par->indexR][par->indexC] + (matrix1[par->indexR][l]*matrix2[l][par->indexC]);
}

}

//reading input file function
void fileReader(char *filename)
{
FILE * fpointer;

fpointer = fopen(filename, "r");

//ensures that the file exists
if(fpointer == NULL)
{
printf("File not found!!\n");
exit(1);
}

//reading row and columns of first matrix from file
fscanf(fpointer, "%d", &row1);
fscanf(fpointer, "%d", &column1);
//reading elements of first matrix
for(i=0; i<row1; i++)
{
for(j=0; j<column1; j++)
{
fscanf(fpointer, "%d", &matrix1[i][j]);
}
}

//reading row and column of second matrix from file
fscanf(fpointer, "%d", &row2);
fscanf(fpointer, "%d", &column2);
//reading elements of second matrix
for(i=0; i<row2; i++)
{
for(j=0; j<column2; j++)
{
fscanf(fpointer, "%d", &matrix2[i][j]);
}
}

fclose(fpointer);

}

//writing to output file function
void fileWriter(char *filename)
{
FILE* fp;
fp = fopen(filename,"w");

if(row2 == column1)
{
fputs("Thread for each row:\n",fp);
for(i=0; i<row1; i++)
{
for(j=0; j<column2; j++)
{
if(j == column2-1)
fprintf(fp, "%d\n", result[i][j]);
else
fprintf(fp, "%d ", result[i][j]);
}
}
fprintf(fp,"Time elapsed: %f seconds\n",timeElapsed1);

fputs("\nThread for each element:\n",fp);
for(i=0; i<row1; i++)
{
for(j=0; j<column2; j++)
{
if(j == column2-1)
fprintf(fp, "%d\n", result2[i][j]);
else
fprintf(fp, "%d ", result2[i][j]);
}
}
fprintf(fp,"Time elapsed: %f seconds\n",timeElapsed2);
}
else {
fprintf(fp,"Matrices can't be multiplied, inner dimensions of matrices don't match!!\n");
}

fclose(fp);
}

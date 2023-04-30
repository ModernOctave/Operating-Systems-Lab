#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

//Semaphore variables
sem_t sem;
int currBit[2] = {0,0}; //Current bit being processed

struct Image
{
	int width;
	int height;
	int max;
	int ***data;
} typedef Image;

Image loadImage(char *filename)
{
	// Open the file
	FILE *fp = fopen(filename, "r");

	// Ensure format is P3
	char format[3];
	fscanf(fp, "%s", format);
	if (strcmp(format, "P3") != 0)
	{
		printf("Error: File is not in P3 format.");
		exit(1);
	}

	// Get the width and height
	int width, height;
	fscanf(fp, "%d %d", &width, &height);

	// Get the max value
	int max;
	fscanf(fp, "%d", &max);

	// Create matrix to store the image
	int ***data = malloc(height * sizeof(int **));
	for (int i = 0; i < height; i++)
	{
		data[i] = malloc(width * sizeof(int *));
		for (int j = 0; j < width; j++)
		{
			data[i][j] = malloc(3 * sizeof(int));
		}
	}

	// Read the image
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				fscanf(fp, "%d", &data[i][j][k]);
			}
		}
	}

	// Create the image struct
	Image image;
	image.width = width;
	image.height = height;
	image.max = max;
	image.data = data;

	return image;
}

void saveImage(Image image, char *filename)
{
	// Open the file
	FILE *fp = fopen(filename, "w");

	// Write the header
	fprintf(fp, "P3\n");
	fprintf(fp, "%d %d\n", image.width, image.height);
	fprintf(fp, "%d\n", image.max);

	// Write the image
	for (int i = 0; i < image.height; i++)
	{
		for (int j = 0; j < image.width; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				fprintf(fp, "%d ", image.data[i][j][k]);
			}
		}
		fprintf(fp, "\n");
	}
}

void* tsfmToGrayscale(void* img)
{
	Image* image = (Image*)img;
	// Convert to grayscale
	for (int i = 0; i < image->height; i++)
	{
		for (int j = 0; j < image->width; j++)
		{
			sem_post(&sem);

			int avg = (image->data[i][j][0] + image->data[i][j][1] + image->data[i][j][2]) / 3;
			image->data[i][j][0] = avg;
			image->data[i][j][1] = avg;
			image->data[i][j][2] = avg;

			currBit[0] = i+1;
			currBit[1] = j+1;

			sem_wait(&sem);


		}
	}
}

void* tsfmToBlur(void* img)
{

	Image* image = (Image*)img;

	int iterations = 5;

	int pixel_done = 0;
	for (int n = 0; n < iterations; n++)
	{
		// Do a 5x5 blur
		for (int i = 2; i < image->height-2; i++)
		{
			for (int j = 2; j < image->width-2; j++)
			{

				sem_post(&sem);
				if(j+2 <= currBit[1] || i+2 <= currBit[0])
				{
					// Apply a 5x5 blur
					for (int k = 0; k < 3; k++)
					{
						// Apply a 5x5 blur
						int avg = image->data[i-2][j-2][k]*1 + image->data[i-2][j-1][k]*4 + image->data[i-2][j][k]*6 + image->data[i-2][j+1][k]*4 + image->data[i-2][j+2][k]*1
							+ image->data[i-1][j-2][k]*4 + image->data[i-1][j-1][k]*16 + image->data[i-1][j][k]*24 + image->data[i-1][j+1][k]*16 + image->data[i-1][j+2][k]*4
							+ image->data[i][j-2][k]*6 + image->data[i][j-1][k]*24 + image->data[i][j][k]*36 + image->data[i][j+1][k]*24 + image->data[i][j+2][k]*6
							+ image->data[i+1][j-2][k]*4 + image->data[i+1][j-1][k]*16 + image->data[i+1][j][k]*24 + image->data[i+1][j+1][k]*16 + image->data[i+1][j+2][k]*4
							+ image->data[i+2][j-2][k]*1 + image->data[i+2][j-1][k]*4 + image->data[i+2][j][k]*6 + image->data[i+2][j+1][k]*4 + image->data[i+2][j+2][k]*1;
						avg = avg / 256;
						// Limit the value to 0-max
						if (avg > image->max)
						{
							avg = image->max;
						}
						if (avg < 0)
						{
							avg = 0;
						}
						image->data[i][j][k] = avg;
					}
				
					pixel_done = 1;
				}

				else
				{
					pixel_done = 0;
				}

				sem_wait(&sem);
				if(pixel_done == 0)
				{
					j = j-1;
				}
			}

			if(pixel_done == 0)
			{
				i = i-1;
			}
		}
	}
}

int main(int argc, char *argv[])
{
	// Ensure the correct number of arguments are passed
	if (argc != 2)
	{
		printf("Usage: ./cvt <filename>");
		exit(1);
	}



	// Load the image
	Image image = loadImage(argv[1]);

	//create 2 threads and pass them the 2 functions




	pthread_t thread1, thread2;

	clock_t start, end;

	// Start the timer
	start = clock();

	pthread_create(&thread1, NULL, tsfmToGrayscale, &image);

	pthread_create(&thread2, NULL, tsfmToBlur, &image);

	pthread_join(thread1, NULL);

	pthread_join(thread2, NULL);

	// Stop the timer
	end = clock();

	// Print the time

	double time_taken = ((double) (end - start)) / CLOCKS_PER_SEC;

	printf("Time taken: %f seconds\n", time_taken);

	// Save the image

	saveImage(image, "output.ppm");
	

	return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>


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

void tsfmToGrayscale(Image image, int fds[2])
{
	// Convert to grayscale

	//close the read end of the pipe
	close(fds[0]);


	for (int i = 0; i < image.height; i++)
	{
		for (int j = 0; j < image.width; j++)
		{
			int avg = (image.data[i][j][0] + image.data[i][j][1] + image.data[i][j][2]) / 3;
			image.data[i][j][0] = avg;
			image.data[i][j][1] = avg;
			image.data[i][j][2] = avg;


			//send this average value to the next process

			write(fds[1], &avg, sizeof(int));
			
		}
	}


	
}

Image tsfmToBlur(Image image, int iterations, int fds[2])
{


	//close the write end of the pipe
	close(fds[1]);

	int counter = 0;
	
	int height = image.height;
	int width = image.width;


	//Read all the values of the pixels from the pipe
	while(1){
		int avg;
		read(fds[0], &avg, sizeof(int));
		image.data[counter/width][counter%width][0] = avg;
		image.data[counter/width][counter%width][1] = avg;
		image.data[counter/width][counter%width][2] = avg;
		counter++;

		if(counter == height*width){
			break;
		}

	}

	for (int n = 0; n < iterations; n++)
	{
		// Do a 5x5 blur
		for (int i = 2; i < image.height-2; i++)
		{
			for (int j = 2; j < image.width-2; j++)
			{
				for (int k = 0; k < 3; k++)
				{
					// Apply a 5x5 blur
					int avg = image.data[i-2][j-2][k]*1 + image.data[i-2][j-1][k]*4 + image.data[i-2][j][k]*6 + image.data[i-2][j+1][k]*4 + image.data[i-2][j+2][k]*1
						+ image.data[i-1][j-2][k]*4 + image.data[i-1][j-1][k]*16 + image.data[i-1][j][k]*24 + image.data[i-1][j+1][k]*16 + image.data[i-1][j+2][k]*4
						+ image.data[i][j-2][k]*6 + image.data[i][j-1][k]*24 + image.data[i][j][k]*36 + image.data[i][j+1][k]*24 + image.data[i][j+2][k]*6
						+ image.data[i+1][j-2][k]*4 + image.data[i+1][j-1][k]*16 + image.data[i+1][j][k]*24 + image.data[i+1][j+1][k]*16 + image.data[i+1][j+2][k]*4
						+ image.data[i+2][j-2][k]*1 + image.data[i+2][j-1][k]*4 + image.data[i+2][j][k]*6 + image.data[i+2][j+1][k]*4 + image.data[i+2][j+2][k]*1;
					avg = avg / 256;
					// Limit the value to 0-max
					if (avg > image.max)
					{
						avg = image.max;
					}
					if (avg < 0)
					{
						avg = 0;
					}
					image.data[i][j][k] = avg;
				}
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

	key_t key = ftok("shmfile",65);

	int shmid = shmget(key, sizeof(int) * 3 * image.height * image.width ,0666|IPC_CREAT);

	int ***data = (int ***)shmat(shmid,(void*)0,0);


	int fds1[2];
	int descriptors1 = pipe(fds1);

	if(descriptors1 == -1)
	{
		printf("Error: Pipe 1 failed.");
		exit(1);
	}

	clock_t start, end;
	
	start = clock();

	//fork two processes to run the two transformations

	int pid1 = fork();

	if(pid1 == 0)
	{

		tsfmToGrayscale(image, fds1);
	}

	else
	{

		
		tsfmToBlur(image, 5, fds1);
	
		waitpid(pid1, NULL, 0);
		// Save the image
		saveImage(image, "output.ppm");

		end = clock();
		
		double time_taken = ((double)(end - start))/CLOCKS_PER_SEC;
		printf("Time taken: %f seconds\n", time_taken);
		
	}

	return 0;
}
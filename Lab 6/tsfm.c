#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>


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

Image tsfmToGrayscale(Image image)
{
	// Convert to grayscale
	for (int i = 0; i < image.height; i++)
	{
		for (int j = 0; j < image.width; j++)
		{
			int avg = image.data[i][j][0] * 0.114 + image.data[i][j][1] * 0.299 + image.data[i][j][2] * 0.587;
			image.data[i][j][0] = avg;
			image.data[i][j][1] = avg;
			image.data[i][j][2] = avg;
		}
	}
}

Image tsfmToBlur(Image image, int iterations)
{
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

	clock_t start, end;

	// Start the timer
	start = clock();

	// Convert to grayscale
	tsfmToGrayscale(image);

	// Do blur
	tsfmToBlur(image, 5);

	// Stop the timer
	end = clock();

	// Print the time

	double time_taken = ((double) (end - start)) / CLOCKS_PER_SEC;

	printf("Time taken: %f seconds\n", time_taken);

	// Save the image
	saveImage(image, "output.ppm");

	return 0;
}
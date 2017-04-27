//
//  filehandler.c
//  C-Ray
//
//  Created by Valtteri Koskivuori on 28/02/15.
//  Copyright (c) 2015 Valtteri Koskivuori. All rights reserved.
//

#include "includes.h"
#include "filehandler.h"

#include "camera.h"
#include "scene.h"
#include "renderer.h"

#define min(a,b) (((a) < (b)) ? (a) : (b))

//Prototypes for internal functions
int getFileSize(char *fileName);

void saveBmpFromArray(const char *filename, unsigned char *imgData, int currentFrame, int width, int height) {
	int i;
	int error;
	FILE *f;
	int filesize = 54 + 3 * width * height;
	
	unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
	unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
	unsigned char bmppadding[3] = {0,0,0};
	
	//Create header with filesize data
	bmpfileheader[2] = (unsigned char)(filesize    );
	bmpfileheader[3] = (unsigned char)(filesize>> 8);
	bmpfileheader[4] = (unsigned char)(filesize>>16);
	bmpfileheader[5] = (unsigned char)(filesize>>24);
	
	//create header width and height info
	bmpinfoheader[ 4] = (unsigned char)(width    );
	bmpinfoheader[ 5] = (unsigned char)(width>>8 );
	bmpinfoheader[ 6] = (unsigned char)(width>>16);
	bmpinfoheader[ 7] = (unsigned char)(width>>24);
	
	bmpinfoheader[ 8] = (unsigned char)(height    );
	bmpinfoheader[ 9] = (unsigned char)(height>>8 );
	bmpinfoheader[10] = (unsigned char)(height>>16);
	bmpinfoheader[11] = (unsigned char)(height>>24);
	
	f = fopen(filename,"wb");
	error = (unsigned int)fwrite(bmpfileheader,1,14,f);
	if (error != 14) {
		printf("Error writing BMP file header data\n");
	}
	error = (unsigned int)fwrite(bmpinfoheader,1,40,f);
	if (error != 40) {
		printf("Error writing BMP info header data\n");
	}
	
	for (i = 0; i < height; i++) {
		error = (unsigned int)fwrite(imgData+(width*(i)*3),3,width,f);
		if (error != width) {
			printf("Error writing image line to BMP\n");
		}
		error = (unsigned int)fwrite(bmppadding,1,(4-(width*3)%4)%4,f);
		if (error != (4-(width*3)%4)%4) {
			printf("Error writing BMP padding data\n");
		}
	}
	fclose(f);
}

void encodePNGFromArray(const char *filename, unsigned char *imgData, int width, int height) {
	unsigned error = lodepng_encode24_file(filename, imgData, width, height);
	if (error) printf("error %u: %s\n", error, lodepng_error_text(error));
}

void printFileSize(char *fileName) {
	//We determine the file size after saving, because the lodePNG library doesn't have a way to tell the compressed file size
	//This will work for all image formats
	long bytes, kilobytes, megabytes, gigabytes, terabytes; // <- Futureproofing?!
	bytes = getFileSize(fileName);
	if (fileName) free(fileName);
	kilobytes = bytes / 1000;
	megabytes = kilobytes / 1000;
	gigabytes = megabytes / 1000;
	terabytes = gigabytes / 1000;
	
	if (gigabytes > 1000) {
		printf("Wrote %ldTB to file.\n", terabytes);
	} else if (megabytes > 1000) {
		printf("Wrote %ldGB to file.\n", gigabytes);
	} else if (kilobytes > 1000) {
		printf("Wrote %ldMB to file.\n", megabytes);
	} else if (bytes > 1000) {
		printf("Wrote %ldKB to file.\n", kilobytes);
	} else {
		printf("Wrote %ldB to file.\n", bytes);
	}
	
}

void writeImage(const char *filePath, unsigned char *imgData, enum fileType type, int currentFrame, int width, int height) {
	//Save image data to a file
	int bufSize;
	if (currentFrame < 100) {
		bufSize = 26;
	} else if (currentFrame < 1000) {
		bufSize = 27;
	} else {
		bufSize = 28;
	}
	char *buf = (char*)calloc(bufSize, sizeof(char));
	
	if (type == bmp){
		sprintf(buf, "%srendered_%d.bmp", filePath, currentFrame);
		printf("Saving result in \"%s\"\n", buf);
		saveBmpFromArray(buf, imgData, currentFrame, width, height);
	} else  if (type == png){
		sprintf(buf, "%srendered_%d.png", filePath, currentFrame);
		printf("Saving result in \"%s\"\n", buf);
		encodePNGFromArray(buf, imgData, width, height);
	}
	printFileSize(buf);
}

int getFileSize(char *fileName) {
	FILE *file;
	file = fopen(fileName, "r");
	if (!file) return 0;
	fseek(file, 0L, SEEK_END);
	int size = (int)ftell(file);
	fclose(file);
	return size;
}

int writeTarga(struct renderer *renderer, char *fileName) {
	//We can just write the renderer to memory
	FILE *file;
	file = fopen(fileName, "wb");
	fwrite(&renderer, sizeof(renderer), 1, file);
	fclose(file);
	return 0;
}

struct renderer *readTarga(char *fileName) {
	FILE *file;
	file = fopen(fileName, "r");
	struct renderer *renderer = malloc(sizeof(renderer));
	fread(&renderer, sizeof(renderer), 1, file);
	return renderer;
}

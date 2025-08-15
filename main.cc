/*
 * This program is used to show case the results of parsing a simple .obj file. 
 * You can compile this by running `g++ main.cc`
 * Feel free to re-use this and the associated files for any purpose you desire…unless you're trying to stop me from using my own code.
 * If that's the case than you're forbidden from using this code.
 *
 * Enforced by the honor system.
 * */
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <string>

#include "./WavefrontImport.h"

WavefrontImport objImport;

int commandParse(int argc, char *argv[]){
	for(int i=0; i<argc; i++){
		if(!strncmp(argv[i], "--vertex", 8)){
			return 0;
		}
	}
	return -1;
}

void printHelp(int argc, char *argv[]){
	printf("======== Manual =========\n");
	printf("Usage %s --help\n", argv[0]);
       	printf("Usage %s <file.obj> [options]\n", argv[0]);

	printf("No Options: By providing no potions, this program will just dump all the data in a readable format.\n");
	printf("--color-padding : pad your result with bytes for color\n");
	printf("--vertex [format(optional)]: Displays only vertex data.\n");
	printf("\t--javascript : prints vertex data as a javascript array\n");
}

void printVertecies(int argc, char *argv[]){
	if(argc <= 3){
		for(int i=0; i<objImport.objCount; i++){
                	printf("Section %d:\n", i);
                	printf("\tVertex coords  : (%f, %f, %f)\n", objImport.obj[i].vertex[0], objImport.obj[i].vertex[1], objImport.obj[i].vertex[2]);
		}
		return;
	}

	int colorPadding=0;
	for(int i=0; i<argc; i++){
		if(!strncmp(argv[i], "--color-padding", 15)){
			colorPadding = 1;
			break;
		}
	}

	if(!strncmp(argv[3], "--javascript", 12)){
		printf("\n\n====JAVASCRIPT OUTPUT =====\n[");
		float r=0.5, g=0.2, b=0.7;
		for(int i=0; i<objImport.objCount; i++){
			if(colorPadding == 1){
	                        printf("[%f, %f, %f,  %f, %f, %f], ", objImport.obj[i].vertex[0], objImport.obj[i].vertex[1], objImport.obj[i].vertex[2], r, g, b);
			}else{
	                        printf("[%f, %f, %f], ", objImport.obj[i].vertex[0], objImport.obj[i].vertex[1], objImport.obj[i].vertex[2]);
			}
                }
		printf("];\n\n====JAVASCRIPT OUTPUT =====");
	}
}

int main(int argc, char *argv[]){
	if(argc < 2){
		printf("Usage %s --help\n", argv[0]);
		printf("Usage %s <file.obj> [options]\n", argv[0]);
		return 1;
	}
	
	if(!strncmp(argv[1], "--help", 6)){
		printHelp(argc, argv);
		return 0;
	}

	// Import Object
	if(!objImport.import(argv[1])){
		printf("Failed to import object.\n");
		return 1;
	}else{
		printf("Importation successful!\n");	
	}

	if(argc > 2){
		switch(commandParse(argc, argv)){
			case 0: // --vertex
				printVertecies(argc, argv);
				return 0;
			default:
				printf("Invalid command.\n");
				return 1;
		}
	}

	printf("Object Count : %ld\n", objImport.objCount);
	// Enumerate first entry using the built in struct
	printf("---------------\nObject Vertecies: \n");
	for(int i=0; i<objImport.objCount; i++){
		printf("Section %d:\n", i);
		printf("\tVertex coords  : (%f, %f, %f)\n", objImport.obj[i].vertex[0], objImport.obj[i].vertex[1], objImport.obj[i].vertex[2]);
		printf("\tTexture coords : (%f, %f)\n", objImport.obj[i].texture[0], objImport.obj[i].texture[1]);
		printf("\tNormal coords : (%f, %f, %f)\n", objImport.obj[i].normal[0], objImport.obj[i].normal[1], objImport.obj[i].normal[2]);
	}printf("------------------\n");
	
	// Enumerate the same data but directly from the array. This array is what you pass to openGL.
	printf("Direct Buffer Object Vertecies : \n");
	for(int i=0; i<8; i++){
		printf("%f ", objImport.glObjBuffer[i]);
	}printf("\n");
	return 0;
}

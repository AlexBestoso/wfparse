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

int main(){
	// Import Object
	WavefrontImport objImport;
	if(!objImport.import("cube.obj")){
		printf("Failed to import object.\n");
		return 1;
	}else{
		printf("Importation successful!\n");	
	}

	// Enumerate first entry using the built in struct
	printf("---------------\nObject Vertecies: \n");
	for(int i=0; i<1; i++){
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

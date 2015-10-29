
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include <X11/X.h>
#include <X11/Xlib.h>

#include <GL/glew.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "c3dlas/c3dlas.h"
#include "c3dlas/meshgen.h"
#include "text/text.h"

#include "utilities.h"
#include "shader.h"
#include "window.h"
#include "game.h"



XStuff xs;
GameState game;
InputState input;


int main(int argc, char* argv[]) {
	
	memset(&xs, 0, sizeof(XStuff));
	
	xs.targetMSAA = 4;
	xs.windowTitle = "EACSMB";
	
	initXWindow(&xs);
	
	initGame(&xs, &game);
	
	while(1) {
		processEvents(&xs, &input, -1);
		
		if(xs.ready)
			renderFrame(&xs, &game, &input);
		
		
		
		if(game.frameSpan < 1.0/60.0) {
			float sleeptime = (1000.0/60.0) - (game.frameSpan * 1000000);
			printf("sleeptime: %f\n", sleeptime /1000000);
			if(sleeptime > 0) usleep(sleeptime); // problem... something is wrong in the math
		}
	}
	
	
	
	
	return 0;
}




 
 
void glexit(char* msg) {
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		fprintf(stderr, "GL ERROR: %s: %s \n", msg, gluErrorString(err));
		exit(-1);
	}
}

char* glerr(char* msg) {
	char* errstr;
	GLenum err;
	
	err = glGetError();
	errstr = NULL; 
	
	if (err != GL_NO_ERROR) { 
		errstr = (char*)gluErrorString(err);
		fprintf(stderr, "GL ERROR: %s: %s \n", msg, errstr);
	}
	
	return errstr;
}



 
 

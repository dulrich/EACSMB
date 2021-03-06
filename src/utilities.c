
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <math.h>

#include <dirent.h>

#include <GL/glew.h>
#include <GL/glx.h>
#include <GL/glu.h>


#include "utilities.h"



void _glexit(char* msg, const char* file, int line, const char* func) {
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		fprintf(stderr, "GL ERROR at %s:%d (%s): %s: %s \n", file, line, func, msg, gluErrorString(err));
		exit(-1);
	}
}


char* _glerr(char* msg, const char* file, int line, const char* func) {
	char* errstr;
	GLenum err;
	
	err = glGetError();
	errstr = NULL;
	
	if (err != GL_NO_ERROR) {
		errstr = (char*)gluErrorString(err);
#ifndef NO_GL_GET_ERR_DEBUG
		fprintf(
			stderr,
				TERM_BOLD TERM_COLOR_RED "GL ERROR:" TERM_RESET TERM_COLOR_RED
				"GL ERROR at %s:%d (%s): %s: %s \n",
			file, line, func, msg, errstr);
#endif
	}
	
	return errstr;
}


void _khr_debug_callback( // i hate this stype of formatting, but this function has too many damn arguments
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar *message,
	GLvoid *userParam) {

	printf(TERM_BOLD TERM_COLOR_RED "GL ERROR:" TERM_RESET TERM_COLOR_RED " %s\n" TERM_RESET, message);
	
}

void initKHRDebug() {
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_LOW, 0, NULL, GL_FALSE);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
	
	glDebugMessageCallback(_khr_debug_callback , NULL);
	

}

char* readFile(char* path, int* srcLen) {
	
	int fsize;
	char* contents;
	FILE* f;
	
	
	f = fopen(path, "rb");
	if(!f) {
		fprintf(stderr, "Could not open file \"%s\"\n", path);
		return NULL;
	}
	
	fseek(f, 0, SEEK_END);
	fsize = ftell(f);
	rewind(f);
	
	contents = (char*)malloc(fsize + 2);
	
	fread(contents+1, sizeof(char), fsize, f);
	contents[0] = '\n';
	contents[fsize] = 0;
	
	fclose(f);
	
	if(srcLen) *srcLen = fsize + 1;
	
	return contents;
}




GLuint makeVAO(VAOConfig* details, int stride) {
	int i; // packed data is expected
	uintptr_t offset = 0;
	GLuint vao;
	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	for(i = 0; details[i].sz != 0; i++) {
		GLenum t;
		int ds;
		
		glEnableVertexAttribArray(i);
		
		t = details[i].type;
		if(t == GL_FLOAT) { // works only for my usage
			
			glVertexAttribPointer(i, details[i].sz, t, GL_FALSE, stride, (void*)offset);
		}
		else {
			glVertexAttribIPointer(i, details[i].sz, t, stride, (void*)offset);
		}
		glerr("vao init");
		
		if(t == GL_UNSIGNED_BYTE || t == GL_BYTE) ds = 1;
		else if(t == GL_UNSIGNED_SHORT || t == GL_SHORT) ds = 2;
		else ds = 4;
		
		offset += ds * details[i].sz;
	}
	
	return vao;
}



float fclamp(float val, float min, float max) {
	return fmin(max, fmax(min, val));
}

float fclampNorm(float val) {
	return fclamp(val, 0.0f, 1.0f);
}

int iclamp(int val, int min, int max) {
	return MIN(max, MAX(min, val));
}

int iclampNorm(int val) {
	return iclamp(val, 0, 1);
}


// length of the line, or length of the string if no \n found
size_t strlnlen(const char* s) {
	char* n;
	
	n = strchr(s, '\n');
	if(!n) return strlen(s);
	
	return n - s;
}

// strdup a line
char* strlndup(const char* s) {
	return strndup(s, strlnlen(s));
}

// line count;
int strlinecnt(const char* s) {
	int n;

	if(!*s) return 0;
	
	n = 1;
	while(*s) // just to make you cringe
		if(*s++ == '\n') 
			n++;
	
	return n;
}

char* pathJoin(const char* a, const char* b) {
	int alen, blen;
	char* o;
	
	
	alen = a ? strlen(a) : 0;
	blen = b ? strlen(b) : 0;
	
	o = malloc(alen + blen + 2);
	
	strcpy(o, a ? a : "");
	o[alen] = '/'; // TODO: fix the concat here
	strcpy(o + alen + 1, b ? b : "");
	o[alen + blen + 1] = 0; 
	
	return o;
}


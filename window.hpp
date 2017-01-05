#pragma once

#include <windows.h>		// Header File For Windows
#include <GL/gl.h>
#include <gl/glu.h>			// Header File For The GLu32 Library

/*
HDC			hDC=NULL;		// Private GDI Device Context
HGLRC		hRC=NULL;		// Permanent Rendering Context
HWND		hWnd=NULL;		// Holds Our Window Handle
HINSTANCE	hInstance;		// Holds The Instance Of The Application

extern bool	keys[256];			// Array Used For The Keyboard Routine
extern bool	active=true;		
extern bool	fullscreen=false;	// Fullscreen Flag Set To Fullscreen Mode By Default

extern int windowWidth=0;
extern int windowHeight=0;
*/

extern HDC			hDC;		// Private GDI Device Context
extern HGLRC		hRC;		// Permanent Rendering Context
extern HWND		hWnd;		// Holds Our Window Handle
extern HINSTANCE	hInstance;		// Holds The Instance Of The Application

extern bool	keys[256];			// Array Used For The Keyboard Routine
extern bool	active;		
extern bool	fullscreen;	// Fullscreen Flag Set To Fullscreen Mode By Default

extern int windowWidth;
extern int windowHeight;

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void configGL();

GLvoid resizeWindow(GLsizei width, GLsizei height);

void killWindow();

bool createWindow(const char* title, int width, int height, bool fullscreenflag);

LRESULT CALLBACK WndProc(	HWND	hWnd,UINT	uMsg,WPARAM	wParam,	LPARAM	lParam);
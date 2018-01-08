/*
==================================================================================
cWNDManager.cpp
==================================================================================
*/

#include "cWNDManager.h"
#include <ctime>
#include <iostream>
#include <windows.h>
#include "GL/glut.h"
#include "wglext.h"
#include "windowOGL.h"
#include "GameConstants.h"

cWNDManager* cWNDManager::pInstance = NULL;

typedef HGLRC(APIENTRYP PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC, HGLRC, const int*);
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;

/*
=================================================================================
Constructor
=================================================================================
*/
cWNDManager::cWNDManager()
{
	m_isRunning = false;
	m_winOGL = NULL;
	m_hinstance = NULL;
	m_lastTime = 0;
}
/*
=================================================================================
Singleton Design Pattern
=================================================================================
*/
cWNDManager* cWNDManager::getInstance()
{
	if (pInstance == NULL)
	{
		pInstance = new cWNDManager();
	}
	return cWNDManager::pInstance;
}

bool cWNDManager::createWND(int width, int height, int bpp)
{
	DWORD      dwExStyle;       // Window Extended Style
	DWORD      dwStyle;         // Window Style

	m_windowRect.left = (long)0; // Set Left Value To 0
	m_windowRect.right = (long)width; // Set Right Value To Requested Width
	m_windowRect.top = (long)0;  // Set Top Value To 0
	m_windowRect.bottom = (long)height;   // Set Bottom Value To Requested Height

	// fill out the window class structure
	m_windowClass.cbSize = sizeof(WNDCLASSEX);
	m_windowClass.style = CS_HREDRAW | CS_VREDRAW;
	m_windowClass.lpfnWndProc = (WNDPROC)cWNDManager::WndProc; //We set our static method as the event handler (WNDPROC)
	m_windowClass.cbClsExtra = 0;
	m_windowClass.cbWndExtra = 0;
	m_windowClass.hInstance = m_hinstance;
	m_windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);  // default icon
	m_windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);      // default arrow
	m_windowClass.hbrBackground = NULL;                             // don't need background
	m_windowClass.lpszMenuName = NULL;                             // no menu
	m_windowClass.lpszClassName = "winOGL";
	m_windowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);      // windows logo small icon

	// register the windows class
	if (!RegisterClassEx(&m_windowClass))
	{
		return false;
	}

	dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;   // Window Extended Style
	dwStyle = WS_OVERLAPPEDWINDOW;                    // Windows Style

	AdjustWindowRectEx(&m_windowRect, dwStyle, false, dwExStyle);     // Adjust Window To True Requested Size

	// class registered, so now create our window
	m_hwnd = CreateWindowEx(NULL,                                 // extended style
		"winOGL",                          // class name
		"Games Programming 3 Coursework - DWYLIE202",      // app name
		WS_OVERLAPPEDWINDOW,	// the window style
		CW_USEDEFAULT, // the starting x coordinate
		CW_USEDEFAULT, // the starting y coordinate
		m_windowRect.right - m_windowRect.left, // the pixel width of the window
		m_windowRect.bottom - m_windowRect.top, // the pixel height of the window
		NULL, // the parent window; NULL for desktop
		NULL, // the menu for the application; NULL for none
		m_hinstance, // the handle to the application instance
		this); // no values passed to the window

	// check if window creation failed (hwnd would equal NULL)
	if (!m_hwnd)
		return 0;

	m_hdc = GetDC(m_hwnd);

	ShowWindow(m_hwnd, SW_SHOW);          // display the window
	UpdateWindow(m_hwnd);                 // update the window

	m_lastTime = GetTickCount64() / 1000.0f; //Initialize the time
	return true;
}

void cWNDManager::destroyWND()
{
	ShowCursor(true);                       // Show Mouse Pointer
}

void cWNDManager::attachOGLWnd(windowOGL* OGLWindow)
{
	m_winOGL = OGLWindow;
}

bool cWNDManager::isWNDRunning()
{
	return m_isRunning;
}

void cWNDManager::processWNDEvents()
{
	MSG msg;

	//While there are messages in the queue, store them in msg
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		//Process the messages one-by-one
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void cWNDManager::setupPixelFormat(void)
{
	int pixelFormat;

	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),  // size
		1,                          // version
		PFD_SUPPORT_OPENGL |        // OpenGL window
		PFD_DRAW_TO_WINDOW |        // render to window
		PFD_DOUBLEBUFFER,           // support double-buffering
		PFD_TYPE_RGBA,              // color type
		32,                         // prefered color depth
		0, 0, 0, 0, 0, 0,           // color bits (ignored)
		0,                          // no alpha buffer
		0,                          // alpha bits (ignored)
		0,                          // no accumulation buffer
		0, 0, 0, 0,                 // accum bits (ignored)
		16,                         // depth buffer
		0,                          // no stencil buffer
		0,                          // no auxiliary buffers
		PFD_MAIN_PLANE,             // main layer
		0,                          // reserved
		0, 0, 0,                    // no layer, visible, damage masks
	};

	pixelFormat = ChoosePixelFormat(m_hdc, &pfd);
	SetPixelFormat(m_hdc, pixelFormat, &pfd);
}

LRESULT CALLBACK cWNDManager::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	cWNDManager* theWindow = NULL;
	switch (uMsg)
	{
	case WM_CREATE:         // window creation
	{
		pInstance->m_hdc = GetDC(hWnd);
		pInstance->setupPixelFormat();

		//Set the version that we want, in this case 3.0
		int attribs[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
			WGL_CONTEXT_MINOR_VERSION_ARB, 0,
			0 }; //zero indicates the end of the array

		//Create temporary context so we can get a pointer to the function
		HGLRC tmpContext = wglCreateContext(pInstance->m_hdc);
		//Make it current
		wglMakeCurrent(pInstance->m_hdc, tmpContext);

		//Get the function pointer
		wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

		//If this is NULL then OpenGL 3.0 is not supported
		if (!wglCreateContextAttribsARB)
		{
			std::cerr << "OpenGL 3.0 is not supported, falling back to GL 2.1" << std::endl;
			pInstance->m_hglrc = tmpContext;
		}
		else
		{
			// Create an OpenGL 3.0 context using the new function
			pInstance->m_hglrc = wglCreateContextAttribsARB(pInstance->m_hdc, 0, attribs);
			//Delete the temporary context
			wglDeleteContext(tmpContext);
		}

		//Make the GL3 context current
		wglMakeCurrent(pInstance->m_hdc, pInstance->m_hglrc);

		pInstance->m_isRunning = true; //Mark our window as running
	}
		break;
	case WM_DESTROY: // window destroy
	case WM_CLOSE: // windows is closing
		wglMakeCurrent(pInstance->m_hdc, NULL);
		wglDeleteContext(pInstance->m_hglrc);
		pInstance->m_isRunning = false; //Stop the main loop
		PostQuitMessage(0); //Send a WM_QUIT message
		return 0;
		break;
	case WM_SIZE:
	{
		int height = HIWORD(lParam);        // retrieve width and height
		int width = LOWORD(lParam);
		pInstance->getAttachedWND()->onResize(width, height); //Call the example's resize method
	}
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) //If the escape key was pressed
		{
			DestroyWindow(pInstance->m_hwnd); //Send a WM_DESTROY message
		}

		// Lab Code goes here

		if (wParam == 'w' || wParam == 'W') {

			if (!soundPlaying) {

				soundPlaying = true;
				PlaySound(TEXT("sound.wav"), NULL, SND_ASYNC);

			}
			else {

				PlaySound(NULL, NULL, SND_ASYNC);
				soundPlaying = false;

			}

		}

		if (wParam == 'd' || wParam == 'D') //If d or D was pressed 
		{
			drawMode = ++drawMode % 3;
			if (drawMode == 0) // fill mode 
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glEnable(GL_DEPTH_TEST);
				glEnable(GL_CULL_FACE);
			}
			else if (drawMode == 1) // wireframe mode 
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glDisable(GL_DEPTH_TEST);
				glDisable(GL_CULL_FACE);
			}
			else // point mode 
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
				glDisable(GL_DEPTH_TEST);
				glDisable(GL_CULL_FACE);
			}
			break;
		}
		if (wParam == VK_LEFT) //If the Left Arrow key was pressed 
		{
			//rotationAngle = rotationAngle + 5; //Increase rotation Angle 
			//glRotatef(rotationAngle, 0.0f, 1.0f, 0.0f);

			cameraRotationAngle += 5.0f;
			glRotatef(cameraRotationAngle, 0.0f, 1.0f, 0.0f);
		}
		if (wParam == VK_RIGHT) //If the Left Arrow key was pressed 
		{
			//cameraRotationAngle = cameraRotationAngle - 5; //Decrease rotation Angle 
			//glRotatef(-cameraRotationAngle, 0.0f, 1.0f, 0.0f);

			if (!soundPlaying) {
				soundPlaying = true;
			}
			else if (soundPlaying) {
				soundPlaying = false;
			}


		}

		if (wParam == VK_UP) {

			//theCamera.moveTheCameraForward(5.0f);

			/*glm::vec3 camera_Position = theCamera.getTheCameraPos();
			float newCamPos_x = camera_Position.x;
			float newCamPos_y = camera_Position.y;
			float newCamPos_z = camera_Position.z;
			glm::vec3 newCamPos = glm::vec3(newCamPos_x + 5.0f, newCamPos_y, newCamPos_z);*/

			//theCamera.setTheCameraPos(newCamPos);

			//cam_Position = glm::vec3(cam_Position.x + 5, cam_Position.y, cam_Position.z);

			//currentCamPos += glm::vec3(5.0f, 0.0f, 0.0f);
			if (!isOrtho) {
				isOrtho = true;
			}
			else {
				isOrtho = false;
			}
			

		}

		break;
	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

windowOGL*  cWNDManager::getAttachedWND()
{
	return m_winOGL;
}


float cWNDManager::getElapsedSeconds()
{
	float currentTime = float(GetTickCount64()) / 1000.0f;
	float seconds = float(currentTime - m_lastTime);
	m_lastTime = currentTime;
	return seconds;
}

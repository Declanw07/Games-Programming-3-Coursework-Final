#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#define GLX_GLXEXT_LEGACY //Must be declared so that our local glxext.h is picked up, rather than the system one


//#include <windows.h>
#include "GameConstants.h"
#include "windowOGL.h"
#include "cWNDManager.h"
//#include "cColours.h"
#include "cShapes.h"
#include "cPyramid.h"
#include "cSphere.h"
#include "cMaterial.h"
#include "cLight.h"
#include "cStarfield.h"
#include "cFontMgr.h"
#include "cCamera.h"





int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR cmdLine,
                   int cmdShow)
{

    // Set window parameters/dimensions
    const int windowWidth = 1024;
    const int windowHeight = 768;
    const int windowBPP = 16;



    // Application window
	static cWNDManager* pgmWNDMgr = cWNDManager::getInstance();

	// Font manager
	static cFontMgr* theFontMgr = cFontMgr::getInstance();

	// OpenGL window
	windowOGL theOGLWnd;

    // Attach openGL window to application window
	pgmWNDMgr->attachOGLWnd(&theOGLWnd);

	// Planet creation
	cSphere sun(4, 40, 40);
	cSphere earth(3,30,30);
	cSphere moon(1, 20, 20);

    // Create window
	if (!pgmWNDMgr->createWND(windowWidth, windowHeight, windowBPP))
    {
        //If it fails

        MessageBox(NULL, "Unable to create the OpenGL Window", "An error occurred", MB_ICONERROR | MB_OK);
		pgmWNDMgr->destroyWND(); //Reset the display and exit
        return 1;
    }

	if (!theOGLWnd.initOGL(windowWidth, windowHeight))
    {
        MessageBox(NULL, "Could not initialize the application", "An error occurred", MB_ICONERROR | MB_OK);
		pgmWNDMgr->destroyWND(); // Reset display and exit application
        return 1;
    }

	// Create textures for each planet/sphere
	cTexture earthTex;
	earthTex.createTexture("Images/Earth.png");
	cTexture moonTex;
	moonTex.createTexture("Images/Moon.png");
	cTexture sunTex;
	sunTex.createTexture("Images/Sun.png");
	cTexture starTexture;
	starTexture.createTexture("Images/star.png");

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//																															//
	//	The starfield object is leftover from the Lab this project was built from, if deleted then all objects					//
	//	within the scene will render green, I am unsure how to fix this issue so have just set the volume to 0 and the amount	//
	//	of sprites used for the starfield to 0 and this prevents the planets from rendering green.                              //
	//																															//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	cStarfield theStarField(starTexture.getTexture(), glm::vec3(0.0f, 0.0f, 0.0f));

	sun.initialise(sunTex.getTexture(), glm::vec3(0, 0, 40), glm::vec3(0, 0, 0));

	earth.initialise(earthTex.getTexture(), glm::vec3(10, 0, 20), glm::vec3(0, 0, 0));
	float earthRotSpeed = 3.0f;
	GLfloat earthOrbit = 0.0f;

	moon.initialise(moonTex.getTexture(), glm::vec3(10, 5, 8), glm::vec3(0, 0, 0));
	float moonRotSpeed = 5.0f;
	GLfloat moonOrbit = 0.0f;

 	// Create Materials for lights
	cMaterial sunMaterial(lightColour4(0.0f, 0.0f, 0.0f, 1.0f), lightColour4(1.0f, 1.0f, 1.0f, 1.0f), lightColour4(1.0f, 1.0f, 1.0f, 1.0f), lightColour4(0, 0, 0, 1.0f), 5.0f);
	cMaterial earthMaterial(lightColour4(0.2f, 0.2f, 0.2f, 1.0f), lightColour4(1.0f, 1.0f, 1.0f, 1.0f), lightColour4(1.0f, 1.0f, 1.0f, 1.0f), lightColour4(0, 0, 0, 1.0f), 50.0f);
	cMaterial moonMaterial(lightColour4(0.1f, 0.1f, 0.1f, 1.0f), lightColour4(1.0f, 1.0f, 1.0f, 1.0f), lightColour4(0.2f, 0.2f, 0.2f, 1.0f), lightColour4(0, 0, 0, 1.0f), 10.0f);
	// Create Light
	cLight sunLight(GL_LIGHT0, lightColour4(1.0f, 0.73f, 0.34f, 0.8f), lightColour4(1, 1, 1, 1), lightColour4(1, 1, 1, 1), glm::vec4(0, 0, 20, 1),
		glm::vec3(0.0, 0.0, 1.0), 0.0f, 180.0f, 1.0f, 0.0f, 0.0f);
	//Define Ambient light for scene
	GLfloat g_Ambient[] = { 0.2, 0.2, 0.2, 1.0 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, g_Ambient);

	// load game fonts
	// Load Fonts
	LPCSTR gameFonts[2] = { "Fonts/Helmet-Regular.ttf", "Fonts/Digital-7.ttf" };

	theFontMgr->addFont("Helmet", gameFonts[0], 24);
	theFontMgr->addFont("Font2", gameFonts[1], 24);

	//
	//	Perspective camera creation, camera position, lookat and up vector is set along with aspect ratio.
	//	Projection matrix is then set, parameters are 45.0f (fov), aspect ratio, near and far clipping planes.
	//
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	cCamera theCamera;
	theCamera.setTheCameraPos(glm::vec3(0.0f, 0.0f, 60.0f));
	theCamera.setTheCameraLookAt(glm::vec3(0.0f, 0.0f, 0.0f));
	theCamera.setTheCameraUpVector(glm::vec3(0.0f, 1.0f, 0.0f)); // pointing upwards in world space
	theCamera.setTheCameraAspectRatio(windowWidth, windowHeight);
	theCamera.setTheProjectionMatrix(45.0f, theCamera.getTheCameraAspectRatio(), 0.1f, 300.0f);
	theCamera.update();

	//
	//	Orthographic camera creation, camera position is then set along with the look at and the up vector.
	//	Camera projection matrix suitable for ortho view is set with parameters left, right, top, bottom then near and far clipping planes.
	//
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	cCamera theCameraOrtho;
	theCameraOrtho.setTheCameraPos(glm::vec3(0.0f, 0.0f, 60.0f));
	theCameraOrtho.setTheCameraLookAt(glm::vec3(0.0f, 0.0f, 0.0f));
	theCameraOrtho.setTheCameraUpVector(glm::vec3(0.0f, 1.0f, 0.0f));
	theCameraOrtho.setTheProjectionMatrix(0.0f, (float)windowWidth, (float)windowHeight, 0.0f, 0.1f, 100.0f);
	theCameraOrtho.update();

	//
	//	Rotation radius for camera is set, this is the distance from the point that the camera rotates around.
	//	The rotation angle is the current angle the camera is rotating around this point at.
	//
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	GLfloat cameraRotRadius = 60.0f;
	GLfloat cameraRotationAngle = 0.0f;

	// Bool to check if sound is being played as windows.h PlaySound() function only allows one sound to be played
	// and this is to avoid constant restarting of sound clip within the game loop.
	bool soundPlayed = false;

   // Main game loop, render frames until isRunning returns false
	while (pgmWNDMgr->isWNDRunning())
    {
		pgmWNDMgr->processWNDEvents(); // Process any window events


        // Fetch the delta time.
		float elapsedTime = pgmWNDMgr->getElapsedSeconds();

		// Checks to play sounds using PlaySound() from windows.h
		if (soundPlaying && !soundPlayed) {
			// If sound is toggled on and the sound isn't already playing then play audio.
			PlaySound(TEXT("sound.wav"), NULL, SND_ASYNC);
			soundPlayed = true;

		}
		else if (!soundPlaying && soundPlayed) {
			// If sound is toggled off and the sound isn't playing set the soundPlayed to false and stop audio.
			PlaySound(TEXT(NULL), NULL, SND_ASYNC);
			soundPlayed = false;
		}
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		theOGLWnd.initOGL(windowWidth,windowHeight);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		// Perform checks to see which camera is active, this checks if the camera is in orthographic mode or perspective.
		if (!isOrtho) {
			glLoadMatrixf((GLfloat*)&theCamera.getTheViewMatrix());
		}
		else if (isOrtho) {

			glLoadMatrixf((GLfloat*)&theCameraOrtho.getTheViewMatrix());
		}

		// Render the starfield to avoid issues with rendering planets turning green.
		glPushMatrix();
		theStarField.render(0.0f);	//

		// Prepare the sun to be rendered and then render the sun.
		sun.prepare(0.0f);
		sunMaterial.useMaterial();
		sunLight.lightOn();
		sun.render(sun.getRotAngle());
		
		// Set the correct rotation angle of the earth planet and then prepare it for rendering then render the planet
		glPushMatrix();
		earth.setRotAngle(earth.getRotAngle() + (earthRotSpeed*elapsedTime));
		earth.prepare(earth.getRotAngle());
		earthMaterial.useMaterial();
		earth.render(earth.getRotAngle());

		// Do the same to the moon as with earth.
		glPushMatrix();
		moon.setRotAngle(moon.getRotAngle() + (moonRotSpeed*elapsedTime));
		moon.prepare(rotationAngle);
		moonMaterial.useMaterial();
		moon.render(moon.getRotAngle());
		glPopMatrix();
		glPopMatrix();

		glPushMatrix();
		theOGLWnd.setOrtho2D(windowWidth, windowHeight);
		theFontMgr->getFont("Helmet")->printText("Games Programming 3 Coursework", FTPoint(10, 15, 0.0f));
		if (isOrtho) {
			theFontMgr->getFont("Font2")->printText("Camera Mode: Orthographic", FTPoint(10, 35, 0.0f));
		}
		else if (!isOrtho) {
			theFontMgr->getFont("Font2")->printText("Camera Mode: Perspective", FTPoint(10, 35, 0.0f));
		}

		if (soundPlaying) {
			theFontMgr->getFont("Font2")->printText("Sound is playing", FTPoint(10, 55, 0.0f));
		}
		else if (!soundPlaying) {
			theFontMgr->getFont("Font2")->printText("Sound is not playing", FTPoint(10, 55, 0.0f));
		}
		theFontMgr->getFont("Font2")->printText("Press right arrow to play sound", FTPoint(10, 75, 0.0f));
		theFontMgr->getFont("Font2")->printText("Press up arrow to change camera", FTPoint(10, 95, 0.0f));


		glPopMatrix();

		pgmWNDMgr->swapBuffers();

		// Set positions for perspective camera relative to the rotation radius around the center and the rotation angle.
		GLfloat posX = (glm::sin(glm::radians(cameraRotationAngle)) * cameraRotRadius);
		GLfloat posZ = (glm::cos(glm::radians(cameraRotationAngle)) * cameraRotRadius);

		theCamera.setTheCameraPos(glm::vec3(posX, 0.0f, posZ));

		// Check which camera is current and render using that camera
		if (!isOrtho) {

			theCamera.update();
		}
		else if (isOrtho) {

			theCameraOrtho.update();
		}
		
		// Perform camera rotation
		cameraRotationAngle -= (5.0f * elapsedTime);

	}

	// Free any allocated resources and destroy the application window
	theOGLWnd.shutdown();
	pgmWNDMgr->destroyWND();

    return 0;
}

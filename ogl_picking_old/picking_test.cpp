/** @file mesh_test.cpp
 *  @brief man code for <app name>
 *
 *	@author <author>
 *	@date <day-month> 2011
 */

#include "stdafx.h"
#include "utils.h"
#include "utils_shaders.h"
#include "utils_math.h"

#include "input.h"

#include "material_point.h"
#include "points_with_collision.h"

#pragma region globals
///////////////////////////////////////////////////////////////////////////////
// globals
Camera g_camera;

// shaders:
GLuint g_shaderLight;

// geometry to render
GLint g_meshDisplayList;

// time:
double g_appTime = 0.0;	// global app time in seconds

// menu:
#define MENU_EXIT 1
#define MENU_RESET 2
#define MENU_EULER 3
#define MENU_VERLET 4

#pragma endregion

///////////////////////////////////////////////////////////////////////////////
// balls:
InsideBoxArea g_box;
PointSetWithCollisions g_points(90);


///////////////////////////////////////////////////////////////////////////////
// data for mouse selection
MouseRayTestData g_rayTest;
unsigned int g_lastHitPointID;
unsigned int g_draggedPointID;
bool g_areWeDraggingPoint;

///////////////////////////////////////////////////////////////////////////////
// function prototypes
bool InitApp();
void InitSimulation();
void ResetSimulation();
void CleanUp();

// callbacks:
void ChangeSize(int w, int h);
void ProcessMenu(int option);

// keyboard:
void ProcessNormalKeys(unsigned char key, int x, int y);
void PressSpecialKey(int key, int x, int y);
void ReleaseSpecialKey(int key, int x, int y);

// mouse:
void ProcessMouse(int button, int state, int x, int y);
void ProcessMouseMotion(int x, int y);
void ProcessPassiveMouseMotion(int x, int y);

void Idle();
void UpdateScene(double deltaSeconds);
void HitTest();
void RenderScene();

///////////////////////////////////////////////////////////////////////////////
// entry point
int main(int argc, char **argv)
{
	// init GLUT
	glutInit(&argc, argv);
	// use depth buffer, double buffering and standard RGBA mode
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(200,200);
	// standard resolution:
	glutInitWindowSize(640, 480);
	// title:
	glutCreateWindow("<add title>");

	// set callbacks:
	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(ProcessNormalKeys);
	glutSpecialFunc(PressSpecialKey);
	glutSpecialUpFunc(ReleaseSpecialKey);
	glutMouseFunc(ProcessMouse);
	glutMotionFunc(ProcessMouseMotion);
	glutPassiveMotionFunc(ProcessPassiveMouseMotion);
	glutDisplayFunc(RenderScene);
	glutIdleFunc(Idle);
	glutReshapeFunc(ChangeSize);
	
	// init OpenGL extensions (like shader support, framebuffer object, etc)
	if (utInitOpenGL20() == false) return 1;

	// init whole application:
	if (InitApp() == false)
	{
		utLOG_ERROR("cannot init application...");
		return 1;
	}

	// start rendering and animation:
	glutMainLoop();

	// finish:
	CleanUp();

	return(0);
}

#pragma region Init & CleanUp
///////////////////////////////////////////////////////////////////////////////
bool InitApp() 
{
	// create GLUT menu:
	int menu = glutCreateMenu(ProcessMenu);
	glutAddMenuEntry("Reset Camera", MENU_RESET);
	glutAddMenuEntry("euler integration", MENU_EULER);
	glutAddMenuEntry("verlet integration", MENU_VERLET);
	glutAddMenuEntry("Exit", MENU_EXIT);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	glLineWidth(2.0f);

	g_meshDisplayList = glGenLists(1);

	glNewList(g_meshDisplayList,GL_COMPILE);
		glutSolidSphere(1.0f, 16, 10);
	glEndList();

	srand((unsigned int)time(NULL));

	if (utLoadAndBuildShader("data\\light.vert", "data\\light.frag", &g_shaderLight) == false)
		return false;

	g_camera.m_zoom = 10.0f;
	g_camera.m_angleY = 45.0f;

	g_areWeDraggingPoint = false;

	InitSimulation();

	return true;
}

void InitSimulation()
{
	g_box.m_min = Vec3d(-4.0, 0.0, -4.0);
	g_box.m_max = Vec3d(4.0, 4.0, 4.0);
	g_box.m_bounceFactor = 1.0;
	g_box.m_frictionFactor = 0.0;
	
	g_points.m_prohibitedArea = &g_box;
	g_points.m_epsilon = 0.99;

	ResetSimulation();
}

void ResetSimulation()
{
	// set random positions and velocities
	double px = -3.5, pz = -3.5;
	for (unsigned int i = 0; i < g_points.PointCount(); ++i)
	{
		g_points.Point(i)->m_pos.x = px;
		g_points.Point(i)->m_pos.y = 0.25;
		g_points.Point(i)->m_pos.z = pz;
		
		px += 0.75;
		if (px > 3.5) { px = -3.5; pz+=0.75; }

		g_points.Point(i)->m_vel.x = 0.0;
		g_points.Point(i)->m_vel.y = 0.0;
		g_points.Point(i)->m_vel.z = 0.0;

		g_points.Point(i)->m_mass = Rand(1.0, 2.5);
		g_points.Point(i)->m_radius = g_points.Point(i)->m_mass*0.1;//Rand(0.1, 0.3);

		g_points.Point(i)->Reset();
	}
}

///////////////////////////////////////////////////////////////////////////////
void CleanUp()
{
	glDeleteLists(g_meshDisplayList, 1);
	utDeleteAllUsedShaders();
}

#pragma endregion

#pragma region Input

///////////////////////////////////////////////////////////////////////////////
void ChangeSize(int w, int h)
{
	g_camera.ChangeViewportSize(w, h);
}

///////////////////////////////////////////////////////////////////////////////
void ProcessMenu(int option)
{
	if (option == MENU_EXIT)
	{
		exit(0);
	}
	else if (option == MENU_RESET)
	{
		g_camera.m_angleX = 0.0f;
		g_camera.m_angleY = 0.0f;
		g_camera.m_zoom = 10.0f;
	}
	else if (option == MENU_EULER)
	{
		MaterialPoint::s_algType = MaterialPoint::atEuler;
	}
	else if (option == MENU_VERLET)
	{
		MaterialPoint::s_algType = MaterialPoint::atVerlet;
	}
}

///////////////////////////////////////////////////////////////////////////////
void ProcessNormalKeys(unsigned char key, int x, int y) {

	if (key == 27) 
		exit(0);
	else if (key == ' ')
	{
		g_camera.m_angleX = 0.0f;
		g_camera.m_angleY = 0.0f;
		g_camera.m_zoom = 10.0f;
	}
	else if (key == 'r')
	{
		ResetSimulation();
	}
}

///////////////////////////////////////////////////////////////////////////////
void PressSpecialKey(int key, int x, int y) 
{
	g_camera.PressSpecialKey(key, x, y);
}


///////////////////////////////////////////////////////////////////////////////
void ReleaseSpecialKey(int key, int x, int y) 
{
	g_camera.ReleaseSpecialKey(key, x, y);
}

///////////////////////////////////////////////////////////////////////////////
void ProcessMouse(int button, int state, int x, int y)
{
	g_camera.ProcessMouse(button, state, x, y);

	// can we move any ball?
	if (g_camera.m_isLeftPressed && g_rayTest.m_hit)
	{
		g_draggedPointID = g_lastHitPointID;
		g_areWeDraggingPoint = true;
	}
	else
		g_areWeDraggingPoint = false;
}

///////////////////////////////////////////////////////////////////////////////
void ProcessMouseMotion(int x, int y)
{
	// if we are dragging any ball we cannot use mouse for scene rotation
	g_camera.ProcessMouseMotion(x, y, !g_areWeDraggingPoint);
}

///////////////////////////////////////////////////////////////////////////////
void ProcessPassiveMouseMotion(int x, int y)
{
	// called when no mouse btn are pressed and mouse moves


	g_camera.ProcessPassiveMouseMotion(x, y);
}

#pragma endregion

#pragma region Update & Render

///////////////////////////////////////////////////////////////////////////////
void Idle()
{
	static double lastDeltas[3] = { 0.0, 0.0, 0.0 };
	static const double REFRESH_TIME = 1.0/60.0;	// monitor with 60 Hz
	
	// in milisec
	int t = glutGet(GLUT_ELAPSED_TIME);
	double newTime = (double)t*0.001;

	double deltaTime = newTime - g_appTime;
	if (deltaTime > REFRESH_TIME) deltaTime = REFRESH_TIME;

	// average:
	deltaTime = (deltaTime + lastDeltas[0] + lastDeltas[1] + lastDeltas[2]) * 0.25;

	g_appTime = g_appTime+deltaTime;

	// call Update:
	UpdateScene(deltaTime);

	// render frame:
	RenderScene();

	// save delta:
	lastDeltas[0] = lastDeltas[1];
	lastDeltas[1] = lastDeltas[2];
	lastDeltas[2] = deltaTime;
}

///////////////////////////////////////////////////////////////////////////////
void UpdateScene(double deltaTime) 
{
	// drag point: use simple spring equation
	if (g_areWeDraggingPoint)
	{
		const double DRAG_K = 10.0;
		Vec3d force;
		Vec3d r = g_rayTest.m_point - g_points.Point(g_draggedPointID)->m_pos;
		force = DRAG_K*r;
		// add force and update:
		g_points.Point(g_draggedPointID)->PrepareMove(deltaTime, force);
		g_points.Point(g_draggedPointID)->UpdateMove();
	}

	g_points.Update(deltaTime);

	g_camera.Update(deltaTime);
}

///////////////////////////////////////////////////////////////////////////////
void HitTest()
{
	// calculate ray for mouse:
	g_rayTest.CalculateRay(g_camera);

	if (g_areWeDraggingPoint == false)
	{		
		// perform hit test with all point in the scene
		// not optimal - one can use some scene tree system to optimise it... 
		g_rayTest.m_hit = g_points.RayTest(g_rayTest.m_start, g_rayTest.m_end, &g_lastHitPointID, &g_rayTest.m_lastT);

		// now in:
		// m_hit - did we hit something?
		// m_lastT - param that can be used to calculate next ball position
		// note that can point to 2D position, so we can only move a point in TWO directions on a plane
		// that is parallel to camera's near plane
	}
}

///////////////////////////////////////////////////////////////////////////////
void DrawBalls()
{
	glUseProgram(g_shaderLight);

	glColor3f(0.5f, 0.5f, 0.7f);
	for (unsigned int i = 0; i < g_points.PointCount(); ++i)
	{
		if (g_rayTest.m_hit && i == g_lastHitPointID)
		{
			glColor3f(0.2f, 0.9f, 0.2f);
		}

		glPushMatrix();
			glTranslated(g_points.Point(i)->m_pos.x, g_points.Point(i)->m_pos.y, g_points.Point(i)->m_pos.z);
			glScaled(g_points.Point(i)->m_radius, g_points.Point(i)->m_radius, g_points.Point(i)->m_radius);
			glCallList(g_meshDisplayList);
		glPopMatrix();

		if (g_rayTest.m_hit && i == g_lastHitPointID)
		{
			glColor3f(0.5f, 0.5f, 0.7f);
		}
	}

	glUseProgram(0);
}

///////////////////////////////////////////////////////////////////////////////
void DrawBox()
{
	glColor4f(1.0f, 1.0f, 1.0f, 0.2f);
	utDrawTransparentBox((float)g_box.CenterX(), (float)g_box.CenterY(), (float)g_box.CenterZ(),
	  				     (float)g_box.SizeX(), (float)g_box.SizeY(), (float)g_box.SizeZ());
}

///////////////////////////////////////////////////////////////////////////////
void RenderScene() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//
	// setup camera:
	//
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	g_camera.SetSimpleView();

	//
	// collision/selection
	//
	HitTest();

	// render something
	DrawBalls();

	DrawBox();

	if (g_areWeDraggingPoint)
	{
		glDisable(GL_DEPTH_TEST);
		glColor3f(0.0f, 1.0f, 0.0f);
		glBegin(GL_LINES);
			glVertex3dv(g_points.Point(g_draggedPointID)->m_pos.m);
			glVertex3dv(g_rayTest.m_point.m);
		glEnd();
		glEnable(GL_DEPTH_TEST);
	}

	// draw text & fps:
	glColor3f(0.0f,1.0f,0.0f);
	utSetOrthographicProjection(g_camera.m_screenWidth, g_camera.m_screenHeight);
		utCalculateAndPrintFps(10, 35);
		utDrawText2D(10,55, "Esc - Quit");
		utDrawText2D(10,70, "R - Reset");
		utDrawText2D(10,85, "Space - Reset Cam");
		if (MaterialPoint::s_algType == MaterialPoint::atEuler)
			utDrawText2D(10,100, "Euler Integration");
		else
			utDrawText2D(10,100, "Verlet Integration");

		static char txt[32];
		sprintf(txt, "mouse %d %d", g_camera.m_mouseX, g_camera.m_mouseY);
		utDrawText2D(10, 115, txt);
	utResetPerspectiveProjection();

	glutSwapBuffers();
}

#pragma endregion

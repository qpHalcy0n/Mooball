// This is complete demoscene hackery //
// This will change //

#include <stdio.h>
#include <stdlib.h>

#include "quickrender.h"
#include "quickimage.h"
#include "quickcam.h"
#include "quick3ds.h"
#include "windowsvc.h"
#include "input.h"
#include "quickINI.h"
#include "guimgr.h"
#include "guiwindow.h"





// Global program constants //
bool		running = true;
bool		g_bFirstIter = true;
bool		g_bMSAA = false;
bool		g_bVSYNC = false;
bool		g_bDebugMode = false;



CLightManager*			g_lightManager = NULL;			// Global light mgr object
int						g_gbufferShader;
//int						g_lightShader;
int						g_compositeShader;
int						g_edgeDetectionShader;
int						g_edgeAntialiasShader;
CCameraObject			g_globalCam;
CTextureInterface*		g_pTexInterface = NULL;
CShaderInterface*		g_pShaderInterface = NULL;
C3DSModel				g_model;

//CGuiWindow*				g_pTestWindow = NULL;

// Global shader constants //
int				g_offscreenDepth;
int				g_normalDepthColor;
int				g_lightBuffer;
int				g_randomTexture;
int				g_edgeTexture;
int				g_groundTexture;

int				g_msaaColor;
int				g_msaaDepth;
int				g_resolveTexture;


// Initialize modelview and projection matrices //
bool			validate_device();



// add_random_light
// Spawns a random point light with random color and a random
// Position between 3 and -3 in all directions
static void add_random_light()
{
	CPointLight rnd;

	float x = (rand() % 1024 / 1024.0F) * 24.0F - 24.0F;
	float y = (rand() % 1024 / 1024.0F) * 24.0F - 24.0F;
	float z = (rand() % 1024 / 1024.0F) * 24.0F - 24.0F;

	vec3 lpos = {x, y, z};
	rnd.SetPosition(lpos);

	float r = rand() % 1024 / 1024.0F;
	float g = rand() % 1024 / 1024.0F;
	float b = rand() % 1024 / 1024.0F;

	int b1, b2, b3, b4, b5, b6;
	b1 = rand() % 54;
	b2 = rand() % 54;
	b3 = rand() % 54;
	b4 = rand() % 54;
	b5 = rand() % 54;
	b6 = rand() % 54;


	vec3 lcolor = {r, g, b};
	rnd.SetColor(lcolor);
	rnd.SetRadius(4.0F);


	g_lightManager->AddLight(&rnd);
}




//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Callback Routines
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-




// keyboard_callback(unsigned char, int, int)
// Invoked when a key is pressed and is signalled by GLUT
//
// key- ASCII value of the key pressed
// mx-  position in x of mouse cursor relative to the absolute position of the screen
// my-  position in y of mouse cursor relative to the absolute position of the screen
static void PollKeys()
{
	// ESC
	if(IsKeyDown(27))
		SendQuitMsg();

	if(IsKeyDown('W') || IsKeyDown('w'))
		g_globalCam.MoveForward(0.1F);
	if(IsKeyDown('S') || IsKeyDown('s'))
		g_globalCam.MoveBack(0.1F);
	if(IsKeyDown('A') || IsKeyDown('a'))
		g_globalCam.MoveLeft(0.1F);
	if(IsKeyDown('D') || IsKeyDown('d'))
		g_globalCam.MoveRight(0.1F);
	if(IsKeyDown('K') || IsKeyDown('k'))
		add_random_light();
	if(IsKeyDown('F') || IsKeyDown('f'))
		g_lightManager->ToggleFlashlight();
}



// mouse_callback(int, int)
// Invoked when a mouse button is pressed only and is signalled by GLUT
// mx-  position in x of mouse cursor relative to the absolute position of the screen
// my-  position in y of mouse cursor relative to the absolute position of the screen

//	HACK: I'll be moving the UI toggling code. -JWB

bool ZDown = false;
static void PollMouse()
{
	if(IsKeyDown(192))
		g_bDebugMode = !g_bDebugMode;

	if(g_bDebugMode)
		SetCursorVisibility(1);
	else
	{
		SetCursorVisibility(0);

		int winX, winY;
		winX = winY = 0;

		s_window_placement wp = GetWindowLocation();
		int dw = (wp.left + wp.right) / 2;
		int dh = (wp.top + wp.bottom) / 2;

		int mx, my;
		GetMousePosition(mx, my);

		g_globalCam.RotateByMouse(mx - dw, my - dh, dw, dh);

		SetMousePosition(dw, dh);
	}
}




//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Initialization routines
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-`


// init_cg(void)
// Initializes the CG runtime by creating contexts, compiling effects,
// and validating techniques. Reports to the debug window any errors encountered.
static void	init_cg()
{
    g_gbufferShader = g_pShaderInterface->AddShader("Media/Shaders/effect_normaldepth.vert", "Media/Shaders/effect_normaldepth.frag");
    g_compositeShader = g_pShaderInterface->AddShader("Media/Shaders/effect_lightcomposite.vert", "Media/Shaders/effect_lightcomposite.frag");
    g_edgeDetectionShader = g_pShaderInterface->AddShader("Media/Shaders/effect_edgedetect.vert", "Media/Shaders/effect_edgedetect.frag");
    g_edgeAntialiasShader = g_pShaderInterface->AddShader("Media/Shaders/effect_edgeantialias.vert", "Media/Shaders/effect_edgeantialias.frag");
}

// init_render_targets(void)
// This function obtains handles, loads textures, and sets up the various offscreen
// targets required for this demo.
// For this demo, there are 3 offscreen color targets that share an offscreen depth buffer.
// They are the normaldepth buffer, the primary ssao target, and the temp blur target.
static void init_render_targets()
{
	int dispWidth = GetScreenWidth();
	int dispHeight = GetScreenHeight();

	unsigned int msaaFlags = SAMPLE_NEAREST;

	g_normalDepthColor = g_pTexInterface->AddRenderTarget(dispWidth, dispHeight, PIXEL_FORMAT_RGBA32F, SAMPLE_TRILINEAR_ANISO);
	g_offscreenDepth   = g_pTexInterface->AddDepthStencilTarget(dispWidth, dispHeight, 32, 0);
	g_lightBuffer	   = g_pTexInterface->AddRenderTarget(dispWidth, dispHeight, PIXEL_FORMAT_RGBA8, SAMPLE_TRILINEAR_ANISO);

	g_msaaColor = INVALID_HANDLE;
	g_msaaDepth = INVALID_HANDLE;
	if(g_bMSAA)
	{
		msaaFlags |= (MSAA_8_SAMPLES | TEXTURE_WRITE_ONLY);
		g_msaaColor		   = g_pTexInterface->AddRenderTarget(dispWidth, dispHeight, PIXEL_FORMAT_RGBA8, msaaFlags);
		g_msaaDepth		   = g_pTexInterface->AddDepthStencilTarget(dispWidth, dispHeight, 32, 0, msaaFlags);
	}
	g_resolveTexture   = g_pTexInterface->AddRenderTarget(dispWidth, dispHeight, PIXEL_FORMAT_RGBA8, SAMPLE_NEAREST);
	g_edgeTexture	   = g_pTexInterface->AddRenderTarget(dispWidth, dispHeight, PIXEL_FORMAT_RGBA8, SAMPLE_NEAREST);



	// Generate and load random texture for SSAO //
	g_randomTexture = g_pTexInterface->AddTexture("Media/Textures/splash.dds", SAMPLE_BILINEAR | SAMPLE_CLAMP);
	g_groundTexture = g_pTexInterface->AddTexture("Media/Textures/brick-walk-texture.bmp", SAMPLE_TRILINEAR_ANISO | SAMPLE_WRAP);
	g_pTexInterface->SetGlobalAnisotropy(16.0F);

	// Gen global light //
	vec3 color = {0.5f, 0.5f, 0.5f};
	vec3 pos = {0.0f, 0.0f, 0.0f};
	CPointLight pl;
	pl.SetColor(color);
	pl.SetPosition(pos);
	pl.SetRadius(40.0f);
	g_lightManager->AddLight(&pl);
}

bool ValidateDevice()
{
	// Print hardware vendor //
	const char* vendor = (const char*)glGetString(GL_VENDOR);
	printf("Hardware Vendor: %s\n", vendor);

	// Print device class //
	const char* dev = (const char*)glGetString(GL_RENDERER);
	printf("Device: %s\n", dev);

	// Print OpenGL version string //
	const char* str = (const char*)glGetString(GL_VERSION);
	printf("OpenGL Version: %s\n", str);


	// Print GLSL Version string //
	const char* shaderver = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
	printf("GLSL Version: %s\n", shaderver);

	int vtf = 0;
	glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &vtf);
	if(vtf == 0)
	{
		printf("Max Texture Image Units: %d\n", vtf);
		return false;
	}

	return true;
}



//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Render Routines
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
static void RenderCornellBox(float dimension)
{
	glBegin(GL_QUADS);
		// Ceiling //
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glNormal3f(0.0f, -1.0f, 0.0f);
		glVertex3f(-dimension, dimension, -dimension);
		glVertex3f(dimension, dimension, -dimension);
		glVertex3f(dimension, dimension, dimension);
		glVertex3f(-dimension, dimension, dimension);

		// Back wall //
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-dimension, -dimension, -dimension);
		glVertex3f(dimension, -dimension, -dimension);
		glVertex3f(dimension, dimension, -dimension);
		glVertex3f(-dimension, dimension, -dimension);

		// Floor //
		glNormal3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-dimension, -dimension, dimension);
		glVertex3f(dimension, -dimension, dimension);
		glVertex3f(dimension, -dimension, -dimension);
		glVertex3f(-dimension, -dimension, -dimension);

		// Left wall //
		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
		glNormal3f(1.0f, 0.0f, 0.0f);
		glVertex3f(-dimension, -dimension, dimension);
		glVertex3f(-dimension, -dimension, -dimension);
		glVertex3f(-dimension, dimension, -dimension);
		glVertex3f(-dimension, dimension, dimension);

		// Right wall //
		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glVertex3f(dimension, -dimension, -dimension);
		glVertex3f(dimension, -dimension, dimension);
		glVertex3f(dimension, dimension, dimension);
		glVertex3f(dimension, dimension, -dimension);
	glEnd();
}


static void RenderGroundPlane()
{

	glBegin( GL_QUADS );
		glColor3f( 1.0F, 1.0F, 1.0F );
		glNormal3f( 0.0f, 1.0f, 0.0f );
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f( -99.0f, -10.0f, 99.0f );

		glTexCoord2f(20.0f, 0.0f);
		glVertex3f( 99.0f, -10.0f, 99.0f );

		glTexCoord2f(20.0f, 20.0f);
		glVertex3f( 99.0f, -10.0f, -99.0f );

		glTexCoord2f(0.0f, 20.0f);
		glVertex3f( -99.0f, -10.0f, -99.0f );
	glEnd();
}




static void RenderGeometry()
{
	g_pShaderInterface->SetStateMatrix("g_matWorldViewProj", MATRIX_MODELVIEW | MATRIX_PROJECTION);
	g_pShaderInterface->SetStateMatrix("g_matWorldView", MATRIX_MODELVIEW);
	RenderGroundPlane();
	RenderCornellBox(10.0f);

//	glMatrixMode(GL_MODELVIEW);
//	glPushMatrix();
//	glTranslatef( 0.0f, 1.0f, 0.0f );
//	glScalef(0.01F, 0.01F, 0.01F);

//	g_pShaderInterface->SetStateMatrix("g_matWorldViewProj", MATRIX_MODELVIEW | MATRIX_PROJECTION);
//	g_pShaderInterface->SetStateMatrix("g_matWorldView", MATRIX_MODELVIEW);

//	g_model.Render();
//	glPopMatrix();
}


static void RenderSceneGBuffer()
{
	// Setup GBuffer pass //
	// Enables ZTest, ZWrites, and disables culling //
	float farClip = g_globalCam.GetFarClip();

	g_pShaderInterface->BindShader( g_gbufferShader );
	g_pShaderInterface->SetFloat( "g_farPlane", &farClip, 1 );

	// Bind GBuffer //
	// There is a known ATi/OpenGL bug in which bound color target clears take n * clear (n = number of targets)
	// time, which is ridiculously long. Therefore we will just clear the depth buffer before they are bound
	g_pTexInterface->BindRenderTarget(&g_normalDepthColor, 1, g_offscreenDepth);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);


	// Render cornell box and enclosed boxes to GBuffer //
	RenderGeometry();

	g_pShaderInterface->UnbindShader();

	int def = DEFAULT_HANDLE;
	g_pTexInterface->BindRenderTarget(&def, 1, DEFAULT_HANDLE);
}





static void RenderLightBuffer()
{
	vec2 viewDims = {(float)GetScreenWidth(), (float)GetScreenHeight()};
	int rt = g_lightBuffer;
//	g_pTexInterface->BindRenderTarget(&rt, 1, DEFAULT_HANDLE);
	g_pTexInterface->BindRenderTarget(&rt, 1, g_offscreenDepth);


	// There is a known ATi/OpenGL bug in which bound color target clears take n * clear (n = number of targets)
	// time, which is ridiculously long. Therefore we will just clear the depth buffer before they are bound
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0, 0, 0, 0);

	// Enable additive blends //
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	g_lightManager->SortLights(g_globalCam);

	g_lightManager->RenderLights(g_normalDepthColor);


	// Disable blending and move on //
	glDisable(GL_BLEND);

	// If MSAA is selected and available, then render the edge buffer and smooth the
	// light buffer
	if(g_bMSAA)
	{
		int def = DEFAULT_HANDLE;
		g_pTexInterface->BindRenderTarget(&g_edgeTexture, 1, g_offscreenDepth);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		g_pShaderInterface->BindShader( g_edgeDetectionShader );
		g_pShaderInterface->SetFloat( "g_viewDims", viewDims, 2 );
		g_pShaderInterface->SetTexture( "edgeSampler", g_normalDepthColor );

		RenderTexturedFullscreenQuad();
		g_pShaderInterface->UnbindShader();

		// Then smooth the edges into the backbuffer //
		g_pTexInterface->BindRenderTarget(&def, 1, DEFAULT_HANDLE);
		g_pTexInterface->BindRenderTarget(&g_normalDepthColor, 1, g_offscreenDepth);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		g_pShaderInterface->BindShader( g_edgeAntialiasShader );
		g_pShaderInterface->SetTexture( "g_edgeBuffer", g_edgeTexture );
		g_pShaderInterface->SetTexture( "g_colorBuffer", g_lightBuffer );
		g_pShaderInterface->SetFloat( "g_viewDims", viewDims, 2 );

		RenderTexturedFullscreenQuad();

		g_pShaderInterface->UnbindShader();
	}

	int def = DEFAULT_HANDLE;
	g_pTexInterface->BindRenderTarget(&def, 1, DEFAULT_HANDLE);
}


static void renderDebug()
{
	g_pTexInterface->BindTexture(g_lightBuffer, 0);
	RenderTexturedFullscreenQuad();
}

float translationAmt = 0.0f;
void RenderScene()
{
    g_globalCam.Update();

	// Clear Depth and color buffers //
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	

	// Enabled texturing and depth testing //
	glEnable(GL_TEXTURE_2D);				// Enable Texturing
	glEnable(GL_DEPTH_TEST);				// Enable Depth tests
	glDepthFunc(GL_LEQUAL);					// LESSEQUAL depth test

	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	glFrontFace( GL_CCW );

	// Do a depth only pass //
	int rtColor = (g_bMSAA) ? g_msaaColor : DEFAULT_HANDLE;
	int rtDepth = (g_bMSAA) ? g_msaaDepth : DEFAULT_HANDLE;
	g_pTexInterface->BindRenderTarget(&rtColor, 1, rtDepth);
	glColorMask(0, 0, 0, 0);
	if(g_bMSAA)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	RenderGeometry( );
	int def = DEFAULT_HANDLE;

	glColorMask(1, 1, 1, 1);
	// Render the gbuffer to non-msaa targets //
	RenderSceneGBuffer();
	RenderLightBuffer();


	rtColor = (g_bMSAA) ? g_msaaColor : DEFAULT_HANDLE;
	rtDepth = (g_bMSAA) ? g_msaaDepth : DEFAULT_HANDLE;
	g_pTexInterface->BindRenderTarget(&rtColor, 1, rtDepth);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	rtColor = (g_bMSAA) ? g_normalDepthColor : g_lightBuffer;

	vec2 viewDims = {(float)GetScreenWidth(), (float)GetScreenHeight()};
	g_pShaderInterface->BindShader( g_compositeShader );
	g_pShaderInterface->SetFloat( "g_viewDims", viewDims, 2 );
	g_pShaderInterface->SetStateMatrix( "g_matWorldViewProj", MATRIX_MODELVIEW | MATRIX_PROJECTION );
	g_pShaderInterface->SetTexture( "g_lightBuffer", rtColor );
	g_pShaderInterface->SetTexture("g_diffuseTexture", g_groundTexture);

	int textured = 1;
	g_pShaderInterface->SetInt( "g_bIsTextured", &textured, 0 );

	glDepthFunc(GL_LEQUAL);
	RenderGeometry();

	g_pShaderInterface->UnbindShader();
	

	if(g_bMSAA)
	{
		g_pTexInterface->BindTexture(g_lightBuffer);
		RenderTexturedFullscreenQuad();

		// Rebind default backbuffer //
		g_pTexInterface->ResolveMSAATarget(g_resolveTexture);
		g_pTexInterface->BindRenderTarget(&def, 1, DEFAULT_HANDLE);

		glDisable(GL_DEPTH_TEST);
		g_pTexInterface->BindTexture(g_resolveTexture);        // resolve tex
		RenderTexturedFullscreenQuad();
	}


	g_pTexInterface->EvictTextures();

#ifdef RENDER_DEBUG
	glDepthFunc(GL_ALWAYS);
	renderDebug();
	glDepthFunc(GL_LEQUAL);
#endif

	glFlush();
}





int main(int argc, char** argv)
{

	int dw = GetScreenWidth();
	int dh = GetScreenHeight();

	vec3 icp = {0.0F, 4.0F, 8.0F};
	vec3 icl = {0.0F, 0.0F, 0.0F};
	vec3 icu = {0.0F, 1.0F, 0.0F};
	g_globalCam.SetCamera(icp, icl, icu);
	g_globalCam.SetPerspective(65.0F, dw / (float)dh, 0.1F, 1000.0F);



	// Query extension base //
	CreateAppWindow("Mooball", dw, dh, 32, false);
	initExtensions();
	InitKeys();

	srand(time(NULL));

	// Query Device capabilities //
	if(!ValidateDevice())
	{
		DestroyAppWindow();
		return 0;
	}
	g_pTexInterface = new CTextureInterface;
	g_pShaderInterface = new CShaderInterface;
	g_lightManager = new CLightManager;
	g_bMSAA = (quickINI::Instance()->getValueAsInt("msaa") > 0) && (g_pTexInterface->GetMaxMSAA() > 0);
	g_bVSYNC = quickINI::Instance()->getValueAsInt("vsync") > 0;


//	g_model.Load( "Media/Models/Pokeball.3ds" );


	// Initialize CG Runtime and shaders //
	init_cg();

	// Turn vsync on //
	if( EXT_VSYNC && g_bVSYNC )
	{
        #ifdef WIN32
            wglSwapIntervalEXT(1);
        #else
            glXSwapIntervalSGI(1);
        #endif
	}

	// Create offscreen targets and depth configurations //
	init_render_targets();

	// Added 8/4/10 - Keeps Mooball from hogging
	// the input focus while it's minimized.
//	HWND windowHandle = GetFocus();

	while(running)
	{
		if(QueryQuitMsg())
			running = false;

		else
		{
			if(!g_bDebugMode)
			{
				UpdateScene();
				PollKeys();
				PollMouse();
			}
			RenderScene();
			FlipBuffers();
		}

	}

	// Fall through to destruction //
	DestroyAppWindow();

	delete g_lightManager;
	delete g_pTexInterface;
	delete g_pShaderInterface;

	return 0;
}

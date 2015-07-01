/*____________________________________________________________________
|
| File: main.c
|
| Description: Main module in Demo1 program.
|
| Functions:  Program_Get_User_Preferences
|             Program_Init
|							 Init_Graphics
|								Set_Mouse_Cursor
|             Program_Run
|							 Init_Render_State
|             Program_Free
|             Program_Immediate_Key_Handler               
|
| (C) Copyright 2013 Abonvita Software LLC.
| Licensed under the GX Toolkit License, Version 1.0.
|___________________________________________________________________*/

#define _MAIN_
#define TREE_NUM 25
#define MAX_TIME 100
#define BUG_NUM 2
/*___________________
|
| Include Files
|__________________*/

#include <first_header.h>
#include "dp.h"
#include "..\Framework\win_support.h"
#include <rom8x8.h>
#include <time.h>
#include "main.h"
#include "terraingrid.h"
#include "clib.h"

void DrawAnimatedButterflay(
	gx3dObject *obj_butterfly,
	gx3dObjectLayer *leftwing,
	gx3dObjectLayer *rightwing,
	float wing_angle);


/*___________________
|
| Type definitions
|__________________*/

typedef struct {
  unsigned resolution;
  unsigned bitdepth;
} UserPreferences;


struct Keyframe {
	float time;           // in seconds
	gx3dVector position;  // in world coords   
	float heading;
};

typedef struct{
	bool bIshunt;
	gx3dVector pos;
	gx3dSphere sphere;
}BugCoord;

struct Animation {
	float length;
	int num_keys;
	Keyframe *keys; // array of keyframes
};

Keyframe keys_model1[] = 
{
		{ 0, { 0, -0.5, 3 }, 90 },
		{ 2, { 1, 0.1, 3 }, 90 },
		{ 3, { 1.25, 0.6, 3.35 }, 45 },
		{ 4, { 1.5, 1.2, 3.5 }, 0 },
		{ 5, { 1.25, 1.0, 3.75 }, -45 },
		{ 6, { 1, 0.5, 4 }, -90 },
		{ 10, { -1, 0.4, 4 }, -90 },
		{ 11, { -1.25, 0.3, 3.75 }, -145 },
		{ 12, { -1.5, -0.1, 3.5 }, -180 },
		{ 13, { -1.25, -0.4, 3.25 }, -225 },
		{ 15, { -1, -0.1, 3 }, -270 },
		{ 17, { 0, -0.5, 3 }, -270 }
};

Keyframe keys_model2[] =
{
	{ 0, { 5, -0.5, 3 }, 90 },
	{ 2, { 6, 0.1, 3 }, 90 },
	{ 3, {6.25, 0.6, 3.35 }, 45 },
	{ 4, { 6.5, 1.2, 3.5 }, 0 },
	{ 5, { 6.25, 1.0, 3.75 }, -45 },
	{ 6, { 6, 0.5, 4 }, -90 },
	{ 9, { 4, 0.4, 4 }, -90 },
	{ 10, { 4, -0.1, 3 }, -270 },
	{ 15, { 5, -0.5, 3 }, -270 }
};

Keyframe keys_model3[] =
{
	{ 0, { 5, -0.5, 3 }, 90 },
	{ 2, { 6, 0.1, 3 }, 45 },
	{ 3, { 6.25, 0.6, 3.35 }, 0 },
	{ 12, { 5, -0.5, 3 }, -270 }
};

Keyframe keys_model4[] =
{
	{ 0, { 5, -0.5, 3 }, 90 },
	{ 3, { 6.25, 0.6, 3.35 }, 0 },
	{ 5, { 6.25, 1.6, 3.75 }, -45 },
	{ 12, { 5, -0.5, 3 }, -270 }
};

Keyframe keys_model5[] =
{
	{ 0, { 5, -0.5, 3 }, 45 },
	{ 5, { 6.25, 0.6, 3.35 }, 0 },
	{ 5, { 6.25, 1.6, 3.75 }, -45 },
	{ 8, { 6.25, 1.6, 4.0}, -45 },
	{ 9, { 5, -0.5, 3 }, -315 }
};

Keyframe keys_model6[] =
{
	{ 0, { 8, -0.5, 4 }, 45 },
	{ 5, { 8.25, 0.6, 4.35 }, 0 },
	{ 5, { 8.25, 1.6, 4.75 }, -45 },
	{ 8, { 8.25, 1.6, 5.0 }, -45 },
	{ 9, { 8, -0.5, 4 }, -315 }
};

Keyframe keys_model7[] =
{
	{ 0, { -4, 1.5, 1 }, 45 },
	{ 8, { 4.25, 0.6, 1.35 }, 0 },
	{ 17, { -4, 1.5, 1 }, -315 }
};

Keyframe keys_model8[] =
{
	{ 0, { 4, -0.5, 1 }, 45 },
	{ 6, { 4.25, 0.6, 1.35 }, 0 },
	{ 8, { 8.25, 1.6, 5.0 }, -45 },
	{ 11, { 7, -0.5, 4 }, -90 },
	{ 17, { 4, -0.5, 1 }, -315 }
};

Keyframe keys_model9[] =
{
	{ 0, { 6, 0.5, 1 }, 45 },
	{ 2, { 5.25, 1.6, 1.35 }, 0 },
	{ 4, { 8, 0.5, 4 }, -90 },
	{ 6, { 6, 0.5, 1 }, -315 }
};

Keyframe keys_model10[] =
{
	{ 0, { -8, 0.5, 1 }, 45 },
	{ 7, { 8, 0.5, 4 }, -90 },
	{ 10, { -8, 0.5, 1 }, -315 }
};
Animation anim_model[] = { { 17, 12, keys_model1 }, { 15, 9, keys_model2 }, { 12, 4, keys_model3 }, { 12, 4, keys_model4 }, { 9, 5, keys_model5 },
{ 9, 5, keys_model6 }, { 17, 3, keys_model7 }, { 17, 5, keys_model8 }, { 6, 4, keys_model9 }, { 10, 3, keys_model10 } };

struct TrailData {
	gx3dVector world, view;
	float alpha;  // 0-1
	float wing_angle;
	float heading;
};

gx3dBox tree_bound[TREE_NUM];
gx3dBox animation_buttferfly[10];
BugCoord bug[TREE_NUM][BUG_NUM];
/*___________________
|
| Function Prototypes
|__________________*/

static int Init_Graphics (unsigned resolution, unsigned bitdepth, unsigned stencildepth, int *generate_keypress_events);
static void Set_Mouse_Cursor (void);
static void Init_Render_State (void);

/*___________________
|
| Constants
|__________________*/
#define SCREENSHOT_FILENAME "screenshots\\screen"
#define MAX_VRAM_PAGES  2
#define GRAPHICS_RESOLUTION  \
  (                          \
    gxRESOLUTION_640x480   | \
    gxRESOLUTION_800x600   | \
    gxRESOLUTION_1024x768  | \
    gxRESOLUTION_1152x864  | \
    gxRESOLUTION_1280x960  | \
    gxRESOLUTION_1400x1050 | \
    gxRESOLUTION_1440x1080 | \
    gxRESOLUTION_1600x1200 | \
    gxRESOLUTION_1152x720  | \
    gxRESOLUTION_1280x800  | \
    gxRESOLUTION_1440x900  | \
    gxRESOLUTION_1680x1050 | \
    gxRESOLUTION_1920x1200 | \
    gxRESOLUTION_2048x1280 | \
    gxRESOLUTION_1280x720  | \
    gxRESOLUTION_1600x900  | \
    gxRESOLUTION_1920x1080 | \
    gxRESOLUTION_2560x1600   \
  )
#define GRAPHICS_STENCILDEPTH 0
#define GRAPHICS_BITDEPTH (gxBITDEPTH_24 | gxBITDEPTH_32)

#define AUTO_TRACKING    1
#define NO_AUTO_TRACKING 0

/*____________________________________________________________________
|
| Function: Program_Get_User_Preferences
|
| Input: Called from TheWin::Init()
| Output: Allows program to popup dialog boxes, etc. to get any user
|   preferences such as screen resolution.  Returns preferences via a
|   pointer.  Returns true on success, else false to quit the program.
|___________________________________________________________________*/

int Program_Get_User_Preferences (void **preferences)
{
  static UserPreferences user_preferences;

  if (gxGetUserFormat (GRAPHICS_DRIVER, GRAPHICS_RESOLUTION, GRAPHICS_BITDEPTH, &user_preferences.resolution, &user_preferences.bitdepth)) {
    *preferences = (void *)&user_preferences;
    return (1);
  }
  else
    return (0);
}

/*____________________________________________________________________
|
| Function: Program_Init
|
| Input: Called from TheWin::Start_Program_Thread()
| Output: Starts graphics mode.  Returns # of user pages available if 
|       successful, else 0.
|___________________________________________________________________*/
 
int Program_Init (void *preferences, int *generate_keypress_events)
{
  UserPreferences *user_preferences = (UserPreferences *) preferences;
  int initialized = FALSE;

  if (user_preferences) 
    initialized = Init_Graphics (user_preferences->resolution, user_preferences->bitdepth, GRAPHICS_STENCILDEPTH, generate_keypress_events);
    
  return (initialized);
}

/*____________________________________________________________________
|
| Function: Init_Graphics
|
| Input: Called from Program_Init()
| Output: Starts graphics mode.  Returns # of user pages available if 
|       successful, else 0.
|___________________________________________________________________*/

static int Init_Graphics (unsigned resolution, unsigned bitdepth, unsigned stencildepth, int *generate_keypress_events)
{
  int num_pages;
  byte *font_data;
  unsigned font_size;

/*____________________________________________________________________
|
| Init globals
|___________________________________________________________________*/

  Pgm_num_pages   = 0;
  Pgm_system_font = NULL;

/*____________________________________________________________________
|
| Start graphics mode and event processing
|___________________________________________________________________*/

  font_data = font_data_rom8x8;
  font_size = sizeof(font_data_rom8x8);
                                                                      
  // Start graphics mode                                      
  num_pages = gxStartGraphics (resolution, bitdepth, stencildepth, MAX_VRAM_PAGES, GRAPHICS_DRIVER);
  if (num_pages == MAX_VRAM_PAGES) {
    // Init system, drawing fonts 
    Pgm_system_font  = gxLoadFontData (gxFONT_TYPE_GX, font_data, font_size);
    // Make system font the default drawing font 
    gxSetFont (Pgm_system_font);

    // Start event processing
    evStartEvents (evTYPE_MOUSE_LEFT_PRESS     | evTYPE_MOUSE_RIGHT_PRESS   |
                   evTYPE_MOUSE_LEFT_RELEASE   | evTYPE_MOUSE_RIGHT_RELEASE |
                   evTYPE_MOUSE_WHEEL_BACKWARD | evTYPE_MOUSE_WHEEL_FORWARD |
//                   evTYPE_KEY_PRESS | 
                   evTYPE_RAW_KEY_PRESS | evTYPE_RAW_KEY_RELEASE,       
                   AUTO_TRACKING, EVENT_DRIVER);
    *generate_keypress_events = FALSE;  // true if using evTYPE_KEY_PRESS in the above mask

		// Set a custom mouse cursor
		Set_Mouse_Cursor ();  
    // Hide the mouse cursor
    msHideMouse();

    // Set globals
    Pgm_num_pages = num_pages;
  }

  return (Pgm_num_pages);
}

/*____________________________________________________________________
|
| Function: Set_Mouse_Cursor
|
| Input: Called from Init_Graphics()
| Output: Sets custom mouse cursor.
|___________________________________________________________________*/
     
static void Set_Mouse_Cursor (void)
{
  gxColor fc, bc;

  // Set cursor to a medium sized red arrow
  fc.r = 255;
  fc.g = 0;
  fc.b = 0;
  fc.a = 0;
  bc.r = 1;
  bc.g = 1;
  bc.b = 1;
  bc.a = 0;
  msSetCursor (msCURSOR_MEDIUM_ARROW, fc, bc);
}

void CheckDrowObject (gx3dObject *obj, float x, float y, float z)
{
	gx3dSphere sphere;
	// make of the objects bounding sphere
	sphere = obj->bound_sphere;
	// adjust it for world space
	sphere.center.x += x;
	sphere.center.y += y;
	sphere.center.z += z;
	if (gx3d_Relation_Sphere_Frustum (&sphere) != gxRELATION_OUTSIDE)
		gx3d_DrawObject (obj, 0);
}

/*____________________________________________________________________
|
| Function: Program_Run
|
| Input: Called from Program_Thread()
| Output: Runs program in the current video mode.  Begins with mouse
|   hidden.
|___________________________________________________________________*/
                    
#define QUIT (NUM_MENU_OPTIONS-1)

void Program_Run (void)
{
  int quit;
  evEvent event;
  gx3dDriverInfo dinfo;
  gxColor color;
  char str[256];


  gx3dObject *obj_ground, *obj_tree, *obj_butterfly, *obj_butterfly1, *obj_sky, *obj_leave, 
	  *health_obj, *blood_obj, *obj_hangup, *obj_bug, *obj_hangbug, *obj_lost, *obj_win;
  gx3dMatrix m, m1, m2, m3, m4, m5;
  gx3dColor color3d_white = { 1, 1, 1, 0 };
  gx3dColor color3d_dim = { 0.2f, 0.2f, 0.2f };
  gx3dColor color3d_black = { 0, 0, 0, 0 };
  gx3dColor color3d_darkgray = { 0.3f, 0.3f, 0.3f, 0 };
  gx3dColor color3d_gray = { 0.5f, 0.5f, 0.5f, 0 };
  gx3dMaterialData material_default = {
    { 1, 1, 1, 1 }, // ambient color
    { 1, 1, 1, 1 }, // diffuse color
    { 1, 1, 1, 1 }, // specular color
    { 0, 0, 0, 0 }, // emissive color
    10              // specular sharpness (0=disabled, 0.01=sharp, 10=diffused)
  };		

/*____________________________________________________________________
|
| Print info about graphics driver to debug file.
|___________________________________________________________________*/

  gx3d_GetDriverInfo (&dinfo);
  debug_WriteFile ("_______________ Device Info ______________");
  sprintf (str, "max texture size: %dx%d", dinfo.max_texture_dx, dinfo.max_texture_dy);
  debug_WriteFile (str);
  sprintf (str, "max active lights: %d", dinfo.max_active_lights);
  debug_WriteFile (str);
  sprintf (str, "max user clip planes: %d", dinfo.max_user_clip_planes);
  debug_WriteFile (str);
  sprintf (str, "max simultaneous texture stages: %d", dinfo.max_simultaneous_texture_stages);
  debug_WriteFile (str);
  sprintf (str, "max texture stages: %d", dinfo.max_texture_stages);
  debug_WriteFile (str);
  sprintf (str, "max texture repeat: %d", dinfo.max_texture_repeat);
  debug_WriteFile (str);
  debug_WriteFile ("__________________________________________");

/*____________________________________________________________________
|
| Clear the screen and draw a 3D object
|___________________________________________________________________*/

	// Set 2d graphics state
	Pgm_screen.xleft   = 0;
	Pgm_screen.ytop    = 0;
	Pgm_screen.xright  = gxGetScreenWidth() - 1;
	Pgm_screen.ybottom = gxGetScreenHeight() - 1;
	gxSetWindow (&Pgm_screen);
	gxSetClip   (&Pgm_screen);
	gxSetClipping (FALSE);

	// Set the 3D viewport
	gx3d_SetViewport (&Pgm_screen);
	// Init other 3D stuff
	Init_Render_State ();
	// Load 3D models						
	gx3d_ReadLWO2File ("Objects\\mytreelimb.lwo", &obj_tree, gx3d_VERTEXFORMAT_TEXCOORDS, 0);
	gx3d_ReadLWO2File("Objects\\ground.lwo", &obj_ground, gx3d_VERTEXFORMAT_TEXCOORDS, 0);
	gx3d_ReadLWO2File("Objects\\butterfly1.lwo", &obj_butterfly1, gx3d_VERTEXFORMAT_TEXCOORDS, 0);
	gx3d_ReadLWO2File("Objects\\butterfly.lwo", &obj_butterfly, gx3d_VERTEXFORMAT_TEXCOORDS, 0);
	gx3d_ReadLWO2File("Objects\\sky.lwo", &obj_sky, gx3d_VERTEXFORMAT_TEXCOORDS, 0);
	gx3d_ReadLWO2File("Objects\\health_billboard.lwo", &health_obj, gx3d_VERTEXFORMAT_TEXCOORDS, 0);
	gx3d_ReadLWO2File("Objects\\blood_billboard.lwo", &blood_obj, gx3d_VERTEXFORMAT_TEXCOORDS, 0);
	gx3d_ReadLWO2File("Objects\\hangup.lwo", &obj_hangup, gx3d_VERTEXFORMAT_TEXCOORDS, 0);
	gx3d_ReadLWO2File("Objects\\bugex.lwo", &obj_bug, gx3d_VERTEXFORMAT_TEXCOORDS, 0);
	gx3d_ReadLWO2File("Objects\\bug_billboard.lwo", &obj_hangbug, gx3d_VERTEXFORMAT_TEXCOORDS, 0);
	gx3d_ReadLWO2File ("Objects\\lost.lwo", &obj_lost, gx3d_VERTEXFORMAT_TEXCOORDS, 0);
	gx3d_ReadLWO2File ("Objects\\win.lwo", &obj_win, gx3d_VERTEXFORMAT_TEXCOORDS, 0);

	gx3d_MakeDoubleSidedObject(obj_butterfly);
	gx3d_MakeDoubleSidedObject(health_obj);
	gx3d_MakeDoubleSidedObject(blood_obj);
	gx3d_MakeDoubleSidedObject(obj_hangup);
	//gx3d_MakeDoubleSidedObject(obj_bug);
	gx3d_MakeDoubleSidedObject (obj_hangbug);
	gx3d_MakeDoubleSidedObject (obj_lost);
	gx3d_MakeDoubleSidedObject (obj_win);

	gx3dObjectLayer *leftwing, *rightwing;
	leftwing = gx3d_GetObjectLayer(obj_butterfly, "left");
	rightwing = gx3d_GetObjectLayer(obj_butterfly, "right");

	gx3dObjectLayer *leftwing1, *rightwing1;
	leftwing1 = gx3d_GetObjectLayer(obj_butterfly1, "left");
	rightwing1 = gx3d_GetObjectLayer(obj_butterfly1, "right");

	snd_Init(22, 16, 2, 1, 1);
	snd_SetListenerDistanceFactorToFeet (snd_3D_APPLY_NOW);

	Sound s_flap, s_song, s_cheers,s_gameover;

	s_song = snd_LoadSound("wav\\birds.wav", snd_CONTROL_VOLUME, 0);
	s_flap = snd_LoadSound ("wav\\flap.wav", snd_CONTROL_3D, 0);
	s_cheers = snd_LoadSound ("wav\\cheers.wav", snd_CONTROL_3D, 0);
	s_gameover = snd_LoadSound ("wav\\gameover.wav", snd_CONTROL_VOLUME, 0);
	//create light

	gx3dLight dir_light;
	gx3dLightData light_data;
	light_data.light_type = gx3d_LIGHT_TYPE_DIRECTION;
	light_data.point.diffuse_color.r = 1;
	light_data.point.diffuse_color.g = 1;
	light_data.point.diffuse_color.b = 1;
	light_data.point.diffuse_color.a = 1;
	light_data.point.specular_color.r = 1;
	light_data.point.specular_color.g = 1;
	light_data.point.specular_color.b = 1;
	light_data.point.specular_color.a = 0;
	light_data.point.ambient_color.r = 1;
	light_data.point.ambient_color.g = 1;
	light_data.point.ambient_color.b = 1;
	light_data.point.ambient_color.a = 0.2f;
	light_data.point.src.x = 10;
	light_data.point.src.y = 10;
	light_data.point.src.z = 0;
	light_data.point.range = 100;
	light_data.point.constant_attenuation = 0.2f;
	light_data.point.linear_attenuation = 0.125f;
	light_data.point.quadratic_attenuation = 0;

	dir_light = gx3d_InitLight(&light_data);


/*____________________________________________________________________
|
| Draw something
|___________________________________________________________________*/

	gx3dVector bpos = { 0, 0.5f, -8 };
	float bheading = 0;  // rotate about y
	float bpitch = 0;    // rotate about x
	float broll = 0;     // rotate about z
	// perform the rotations in order: XZY

	float speed = 0;

	const float pitch_max = 45;
	const float roll_max = 35;
	const float movingspeed_max = 3;  // feet per second
	const float rotatespeed_max = 60; // degrees per second

	gx3dVector camera_from, camera_to, camera_up = { 0, 1, 0 };

	// Set view matrix
	gx3dVector from = { 0, 2, -3 }, to = { 0, 0, 4 }, up = { 0, 1, 0 }, dummy;
	gx3d_CameraSetPosition (&from, &to, &up, gx3d_CAMERA_ORIENTATION_LOOKTO_FIXED);
	gx3d_CameraSetViewMatrix ();

  // Set projection matrix
	float fov = 80; // degrees field of view
	float near_plane = 0.1f;
	float far_plane = 1000;
	gx3d_SetProjectionMatrix (fov, near_plane, far_plane);

	// Sets the 3D viewport clear color to black
	color.r = 0;
	color.g = 0;
	color.b = 0;
	color.a = 0;

	// Draw in wireframe
	gx3d_SetFillMode(gx3d_FILL_MODE_GOURAUD_SHADED); 
	//gx3d_SetBackfaceRemoval (false); // this turns off backface removal (which by default is on)

	float v = 0; // a variable to control rotation

	// Flush event queue
	evFlushEvents ();
	unsigned starttime = timeGetTime();
	unsigned starttime_default = starttime;
	int elapsedtime, curtime;

	bool forward = false;
	bool backward = false;
	bool right = false;
	bool left = false;
	bool flapping = false;

	snd_PlaySound(s_song, 1);
	snd_SetSoundVolume(s_song, 90);


	snd_SetSoundMode (s_flap, snd_3D_MODE_ORIGIN_RELATIVE, snd_3D_APPLY_NOW);
	snd_SetSoundPosition (s_flap, bpos.x, bpos.y, bpos.z, snd_3D_APPLY_NOW);
	snd_SetSoundMinDistance (s_flap, 10, snd_3D_APPLY_NOW);
	snd_SetSoundMaxDistance (s_flap, 50, snd_3D_APPLY_NOW);

	snd_PlaySound (s_flap, 1);
	snd_SetSoundVolume (s_flap, 100);

	for (int i = 0; i < TREE_NUM; i++)
	{
		for (int j = 0; j < BUG_NUM; j++)
		{
			bug[i][j].bIshunt = false;
			bug[i][j].pos.y = .75+j*0.5;
			bug[i][j].pos.x = 0;
			bug[i][j].pos.z = -0.25;

		}
	}

	int hunt_bug = 0;
	int play_time = 0;
	int ending_pictime = 0;
	bool game_over = false;
	
  // Render loop
  for (quit=FALSE; NOT quit; ) 
  {
	  // Compute elapsed time
	  curtime = timeGetTime();
	  elapsedtime = curtime - starttime;
	  starttime = curtime;
	  bool take_screenshot = false;
	 
	  while (evGetEvent(&event)) 
	  {
		  if (event.type == evTYPE_RAW_KEY_PRESS) 
		  {
			  if (event.keycode == evKY_ESC)
			  {
				  quit = TRUE;
			  }
				  
			  else if (event.keycode == 'w')
			  {
				  forward = true;
			  }
			  else if (event.keycode == 's')
				  backward = true;
			  else if (event.keycode == 'a')
				  left = true;
			  else if (event.keycode == 'd')
				  right = true;
			  else if (event.keycode == evKY_SPACE) 
			  {
				  flapping = true;
				  //          flapping_up_time = timeGetTime();
			  }
			  else if (event.keycode == 'o')
			  {
				  take_screenshot = true;
			  }
		  }
		  else if (event.type == evTYPE_RAW_KEY_RELEASE) 
		  {
			  if (event.keycode == 'w')
				  forward = false;
			  else if (event.keycode == 's')
				  backward = false;
			  else if (event.keycode == 'a')
				  left = false;
			  else if (event.keycode == 'd')
				  right = false;
			  else if (event.keycode == evKY_SPACE)
				  flapping = false;
		  }
	  }

	  // Clear viewport
	  gx3d_ClearViewport(gx3d_CLEAR_SURFACE | gx3d_CLEAR_ZBUFFER, color, gx3d_MAX_ZBUFFER_VALUE, 0);

	  //set fog
	  gx3d_SetFogColor (0, 0, 0);
	  gx3d_SetLinearPixelFog (35, 100);

	  // Start rendering in 3D
	  if (gx3d_BeginRender())
	  {

		  gx3d_CameraSetPosition(&camera_from, &camera_to, &camera_up, gx3d_CAMERA_ORIENTATION_LOOKTO_FIXED);
		  gx3d_CameraSetViewMatrix();

		  gx3dColor color3d = { 0.2f, 0.2f, 0.2f, 1 };
		  gx3d_SetAmbientLight(color3d);
		  gx3d_EnableLight(dir_light);

		  // Draw the ground object
		  gx3d_GetScaleMatrix(&m, 1.5f, 1.5f, 1.5f);
		  gx3d_GetTranslateMatrix(&m1, 0, 0, 0);
		  gx3d_MultiplyMatrix(&m, &m1, &m);
		  gx3d_SetObjectMatrix(obj_ground, &m);
		  CheckDrowObject(obj_ground, 0, 0, 0);

		  float x = -12, z = -10;
		  gx3d_EnableFog ();
		  for (int ii = 0; ii < TREE_NUM; ii++)
		  {
			  gx3d_SetAmbientLight(color3d);
			  gx3d_GetTranslateMatrix(&m, x, 0, z);
			  gx3d_SetObjectMatrix(obj_tree, &m);
			  CheckDrowObject(obj_tree, x, 0, z);
			  tree_bound[ii] = obj_tree->bound_box;
			  tree_bound[ii].min.x += x;
			  tree_bound[ii].min.y += 0;
			  tree_bound[ii].min.z += z;
			  tree_bound[ii].max.x += x;
			  tree_bound[ii].max.z += z ;
			  tree_bound[ii].max.y += 0;

			  for (int j = 0; j < BUG_NUM; j++)
			  {
				  
				  float bugx, bugy, bugz;
				  bugy = bug[ii][j].pos.y; 
				  bugx = bug[ii][j].pos.x+x;
				  bugz = bug[ii][j].pos.z + z;
				  //display bug
				  if (!bug[ii][j].bIshunt)
				  {
					  gx3d_GetTranslateMatrix (&m, bugx, bugy, bugz);
					  gx3d_SetObjectMatrix (obj_bug, &m);
					  CheckDrowObject (obj_bug, bugx, bugy, bugz);
					  
					  bug[ii][j].sphere = obj_bug->bound_sphere;
					  bug[ii][j].sphere.center.x += bugx;
					  bug[ii][j].sphere.center.y += bugy;
					  bug[ii][j].sphere.center.z += bugz;

				  }

			  }

			  x += 5;
			  if (ii % 5 == 0)
			  {
				  z += 3;
				  x = -12;
			  }
		  }

		  gx3d_DisableFog ();


		  gx3d_DisableLight(dir_light);
		  gx3d_SetAmbientLight(color3d_white);
		  gx3d_SetMaterial(&material_default);
		  // Draw the sky object
		  gx3d_GetTranslateMatrix(&m, 0, 0, 0);
		  gx3d_SetObjectMatrix(obj_sky, &m);
		  CheckDrowObject(obj_sky, 0, 0, 0);

		  static float wing_angle = 45;
		  static float wing_angle_incr = 10;
		  wing_angle += wing_angle_incr;
		  if (wing_angle > 50)
		  {
			  wing_angle_incr = -10;
		  }
		  else if (wing_angle < -75)
		  {
			  wing_angle_incr = 10;
		  }

		  //draw animatied butterfly
		  DrawAnimatedButterflay(obj_butterfly1, leftwing1, rightwing1, wing_angle);

		  static float prewing = wing_angle;
		  if (flapping)
		  {
			  prewing = wing_angle + 100;
		  }
		  else
		  {
			  prewing = wing_angle;
		  }

		  // Draw 'user' butterfly
		  gx3d_DisableZBuffer();
		  gx3d_GetScaleMatrix(&m1, 1.5f, 1.5f, 1.5f);
		  gx3d_GetRotateXMatrix(&m2, bpitch);
		  gx3d_GetRotateZMatrix(&m3, -broll);
		  gx3d_GetRotateYMatrix(&m4, bheading);
		  gx3d_GetTranslateMatrix(&m5, bpos.x, bpos.y, bpos.z);
		  gx3d_MultiplyMatrix(&m1, &m2, &m);
		  gx3d_MultiplyMatrix(&m, &m3, &m);
		  gx3d_MultiplyMatrix(&m, &m4, &m);
		  gx3d_MultiplyMatrix(&m, &m5, &m);
		  gx3d_SetObjectMatrix(obj_butterfly, &m);
		  gx3d_GetRotateZMatrix(&m, prewing);
		  gx3d_SetObjectLayerMatrix(obj_butterfly, leftwing, &m);
		  gx3d_GetRotateZMatrix(&m, -prewing);
		  gx3d_SetObjectLayerMatrix(obj_butterfly, rightwing, &m);
		  gx3d_SetAmbientLight(color3d_white);
		  CheckDrowObject(obj_butterfly, bpos.x, bpos.y, bpos.z);
		  gx3d_EnableZBuffer();
		  gx3d_DisableAlphaBlending();
		  gx3d_DisableAlphaTesting();
			
		  //update flapping sound position
		  gx3dVector Xsound1_position;
		  // build a matrix 
		  gx3d_MultiplyVectorMatrix (&bpos, &m, &Xsound1_position);
		  snd_SetSoundPosition (s_flap, Xsound1_position.x, Xsound1_position.y, Xsound1_position.z, snd_3D_APPLY_NOW);

		  //Draw health bar

		  // Set default view matrix
		  gx3dVector tfrom = { 0, 2, -3 }, tto = { 0, 0, 4 }, tup = { 0, 1, 0 };
		  gx3d_CameraSetPosition (&tfrom, &tto, &tup, gx3d_CAMERA_ORIENTATION_LOOKTO_FIXED);
		  gx3d_CameraSetViewMatrix ();

		  gx3d_DisableZBuffer ();
		  gx3d_EnableAlphaTesting (128);
		  gx3d_EnableAlphaBlending ();

		  float view_y = tto.y+1.8;
		  float view_x = tto.x-0.9; //tan (30 * 3.1415 / 180.0)*view_y;
		  
		  gx3d_GetTranslateMatrix(&m, view_x, view_y, 0);
		  gx3d_SetObjectMatrix (health_obj, &m);
		  gx3d_DrawObject(health_obj, 0);

		  //calculate blood
		  
			  
		  float percent = (float)(timeGetTime() - starttime_default) / (float)(MAX_TIME*1000.0);
			if (percent > 1.0 )
			{
				percent = 1.0;
				if (!game_over)
				{
					game_over = TRUE;
					ending_pictime = timeGetTime ();
				}

			}

			gx3d_GetScaleMatrix(&m, percent, 1, 1);
			gx3d_GetTranslateMatrix(&m1, view_x +0.48 - percent/2, view_y, 0);
			gx3d_MultiplyMatrix(&m, &m1, &m);
			gx3d_SetObjectMatrix(blood_obj, &m);
			gx3d_DrawObject(blood_obj, 0);
		  

			//draw hang up butterfly
			gx3d_GetTranslateMatrix(&m, view_x-0.78, view_y+0.1, 0);
			gx3d_SetObjectMatrix(obj_hangup, &m);
			gx3d_DrawObject(obj_hangup, 0);


			//draw hang up heart
			view_x -= 0.2;
			for (int i = 0; i < hunt_bug; i++)
			{
				gx3d_GetTranslateMatrix (&m, view_x, view_y+0.45, 0);
				gx3d_SetObjectMatrix (obj_hangbug, &m);
				gx3d_DrawObject (obj_hangbug, 0);
				view_x += 0.3;
			}
			gx3d_DisableAlphaTesting ();
			gx3d_DisableAlphaBlending ();
			gx3d_EnableZBuffer ();

			if (game_over)
			{
				//display
				if (hunt_bug >= 0)
				{
					gx3d_DisableZBuffer ();
					gx3d_EnableAlphaTesting (128);
					gx3d_EnableAlphaBlending ();

					gx3d_GetTranslateMatrix (&m, view_x, view_y, 0);
					gx3d_SetObjectMatrix (obj_win, &m);
					gx3d_DrawObject (obj_win, 0);


					gx3d_DisableAlphaTesting ();
					gx3d_DisableAlphaBlending ();
					gx3d_EnableZBuffer ();
				}
				else
				{
					gx3d_GetTranslateMatrix (&m, view_x, view_y, 0);
					gx3d_SetObjectMatrix (obj_lost, &m);
					gx3d_DrawObject (obj_lost, 0);
				}

				if (timeGetTime () - ending_pictime > 2000)
				{
					quit = TRUE;
					snd_StopSound (s_gameover);
				}

				if (!snd_IsPlaying (s_gameover))
				{
					snd_PlaySound (s_gameover, 1);
					snd_SetSoundVolume (s_gameover, 100);
				}
			}
			// Stop rendering
			gx3d_EndRender();

			// Page flip (so user can see it)
			gxFlipVisualActivePages(FALSE);
	  }

	  // Update the state of the butterfly
	  if (right && !left) 
	  {
		  bheading += (rotatespeed_max * (elapsedtime / 1000.0));
		  broll += (15 * (elapsedtime / 1000.0));
		  if (broll > roll_max)
			  broll = roll_max;
	  }
	  else if (left && !right) 
	  {
		  bheading -= (rotatespeed_max * (elapsedtime / 1000.0));
		  broll -= (15 * (elapsedtime / 1000.0));
		  if (broll < -roll_max)
			  broll = -roll_max;
	  }
	  else 
	  {
		  // equalize roll back to 0
		  if (broll > 0) 
		  {
			  broll -= (15 * (elapsedtime / 1000.0));
			  if (broll < 0)
				  broll = 0;
		  }
		  else if (broll < 0) 
		  {
			  broll += (15 * (elapsedtime / 1000.0));
			  if (broll > 0) 
				  broll = 0;
		  }
	  }

	  while (bheading > 360) bheading -= 360;
	  while (bheading < -360) bheading += 360;

	  gx3dVector previous_bpos = bpos;
	  
	  if (forward && !backward) 
	  {
		  gx3dVector dir = { 0, 0, 1 };
		  gx3d_GetRotateYMatrix(&m, bheading);
		  gx3d_MultiplyVectorMatrix(&dir, &m, &dir);
		  bpos.x += (dir.x * movingspeed_max * elapsedtime / 1000.0);
		  //bpos.y += (dir.y * movingspeed_max * elapsedtime / 10000.0);;
		  bpos.z += (dir.z * movingspeed_max * elapsedtime / 1000.0);;
	  }
	  else if (backward && !forward) 
	  {
		  gx3dVector dir = { 0, 0, 1 };
		  gx3d_GetRotateYMatrix(&m, bheading);
		  gx3d_MultiplyVectorMatrix(&dir, &m, &dir);
		  bpos.x -= (dir.x * movingspeed_max * elapsedtime / 1000.0);
		  //bpos.y -= (dir.y * movingspeed_max * elapsedtime / 10000.0);
		  bpos.z -= (dir.z * movingspeed_max * elapsedtime / 1000.0);
	  }
	  
	  static bool bisplay = false;
	  if (flapping)
	  {
		  gx3dVector dir = { 0, 1, 0 };
		  gx3d_GetRotateYMatrix(&m, bheading);
		  gx3d_MultiplyVectorMatrix(&dir, &m, &dir);
		  bpos.y += (dir.y * movingspeed_max * elapsedtime / 1000.0);
	  }
	  else
	  {
		  gx3dVector dir = { 0, 1, 0 };
		  gx3d_GetRotateYMatrix(&m, bheading);
		  gx3d_MultiplyVectorMatrix(&dir, &m, &dir);

		  if (bpos.y > 1)
		  {
			  bpos.y -= (dir.y * movingspeed_max * elapsedtime / 1000.0);
		  }
	  }

	  // compute camera parameters
	  camera_to = bpos;
	  camera_from.x = from.x;
	  camera_from.y = from.y;
	  camera_from.z = from.z;
	  gx3d_GetRotateYMatrix(&m, bheading);
	  gx3d_MultiplyVectorMatrix(&camera_from, &m, &camera_from);
	  camera_from.x += bpos.x;
	  camera_from.y += bpos.y;
	  camera_from.z += bpos.z;

	  gx3d_GetRotateZMatrix(&m1, -broll*2.5);
	  gx3d_GetRotateYMatrix(&m2, bheading);
	  gx3d_MultiplyMatrix(&m1, &m2, &m);
	  gx3dVector tup = { 0, 1, 0 };
	  gx3d_MultiplyVectorMatrix(&tup, &m, &camera_up);

    // Add one degree rotation per frame
		v += 1.0;
		if (v >= 360.0)               
			v = 0; 

	if (take_screenshot) {
		char str[32];
		char buff[50];
		static int screenshot_count = 0;
		strcpy(str, SCREENSHOT_FILENAME);
		strcat(str, itoa(screenshot_count++, buff, 10));
		strcat(str, ".bmp");
		gxWriteBMPFile(str);
	}

	for (int i = 0; i < 10; i++)
	{
		gxRelation rel;
		rel = gx3d_Relation_Point_Box (&bpos, &animation_buttferfly[i]);

		if (rel == gxRELATION_INSIDE ||
			rel == gxRELATION_INTERSECT)
		{
			bpos = previous_bpos;
			break;
		}
	}

	for (int i = 0; i < TREE_NUM; i++)
	{
		gxRelation rel;
		rel = gx3d_Relation_Point_Box(&bpos, &tree_bound[i]);


		if (rel == gxRELATION_INSIDE||
			rel == gxRELATION_INTERSECT)
		{
		
			bpos = previous_bpos;
			for (int j = 0; j < BUG_NUM; j++)
			{
				rel = gx3d_Relation_Point_Sphere(&bpos, &bug[i][j].sphere);
				if ((rel == gxRELATION_INSIDE ||
					rel == gxRELATION_INTERSECT) && !bug[i][j].bIshunt)
				{
					bug[i][j].bIshunt = true;
					hunt_bug++;

					snd_SetSoundMode (s_cheers, snd_3D_MODE_ORIGIN_RELATIVE, snd_3D_APPLY_NOW);
					snd_SetSoundPosition (s_cheers, bpos.x, bpos.y, bpos.z, snd_3D_APPLY_NOW);
					snd_SetSoundMinDistance (s_cheers, 10, snd_3D_APPLY_NOW);
					snd_SetSoundMaxDistance (s_cheers, 50, snd_3D_APPLY_NOW);

					//play cheer sound
					gx3dVector Xsound1_position;
					gx3d_MultiplyVectorMatrix (&bpos, &m, &Xsound1_position);
					snd_SetSoundPosition (s_cheers, Xsound1_position.x, Xsound1_position.y, Xsound1_position.z, snd_3D_APPLY_NOW);

					snd_PlaySound (s_cheers, 1);
					snd_SetSoundVolume (s_cheers, 100);
					play_time = timeGetTime ();
					
				}
			}
		}
	}

	if (timeGetTime()-play_time > 3000 && snd_IsPlaying (s_cheers))
	{
		snd_StopSound (s_cheers);
	}
  }

/*____________________________________________________________________
|
| Free dynamic data structures
|___________________________________________________________________*/

  if (obj_ground)
    gx3d_FreeObject (obj_ground);  

  if (obj_sky)
	  gx3d_FreeObject(obj_sky);
  if (obj_butterfly)
	  gx3d_FreeObject(obj_butterfly);
  if (obj_butterfly1)
	  gx3d_FreeObject(obj_butterfly1);
  if (obj_tree)
	  gx3d_FreeObject(obj_tree);

  snd_StopSound(s_song);
  snd_Free();
}

/*____________________________________________________________________
|
| Function: Init_Render_State
|
| Input: Called from Program_Run()
| Output: Initializes general 3D render state.
|___________________________________________________________________*/

static void Init_Render_State (void)
{
  // Enable zbuffering
  gx3d_EnableZBuffer ();

  // Enable lighting
  gx3d_EnableLighting ();

  // Set the default alpha blend factor
  gx3d_SetAlphaBlendFactor (gx3d_ALPHABLENDFACTOR_SRCALPHA, gx3d_ALPHABLENDFACTOR_INVSRCALPHA);

  // Init texture addressing mode - wrap in both u and v dimensions
  gx3d_SetTextureAddressingMode (0, gx3d_TEXTURE_DIMENSION_U | gx3d_TEXTURE_DIMENSION_V, gx3d_TEXTURE_ADDRESSMODE_WRAP);
  gx3d_SetTextureAddressingMode (1, gx3d_TEXTURE_DIMENSION_U | gx3d_TEXTURE_DIMENSION_V, gx3d_TEXTURE_ADDRESSMODE_WRAP);
  // Texture stage 0 default blend operator and arguments
  gx3d_SetTextureColorOp (0, gx3d_TEXTURE_COLOROP_MODULATE, gx3d_TEXTURE_ARG_TEXTURE, gx3d_TEXTURE_ARG_CURRENT);
  gx3d_SetTextureAlphaOp (0, gx3d_TEXTURE_ALPHAOP_SELECTARG1, gx3d_TEXTURE_ARG_TEXTURE, 0);
  // Texture stage 1 is off by default
  gx3d_SetTextureColorOp (1, gx3d_TEXTURE_COLOROP_DISABLE, 0, 0);
  gx3d_SetTextureAlphaOp (1, gx3d_TEXTURE_ALPHAOP_DISABLE, 0, 0);
    
  // Set default texture coordinates
  gx3d_SetTextureCoordinates (0, gx3d_TEXCOORD_SET0);
  gx3d_SetTextureCoordinates (1, gx3d_TEXCOORD_SET1);

  // Enable trilinear texture filtering
  gx3d_SetTextureFiltering (0, gx3d_TEXTURE_FILTERTYPE_TRILINEAR, 0);
  gx3d_SetTextureFiltering (1, gx3d_TEXTURE_FILTERTYPE_TRILINEAR, 0);
}

/*____________________________________________________________________
|
| Function: Program_Free
|
| Input: Called from TheWin::OnClose()
| Output: Exits graphics mode.
|___________________________________________________________________*/

void Program_Free (void)
{
  // Stop event processing 
  evStopEvents ();
  // Return to text mode 
  if (Pgm_system_font)
    gxFreeFont (Pgm_system_font);
  // Exit graphics mode (back to desktop mode)
  gxStopGraphics ();
}

void DrawAnimatedButterflay(
	gx3dObject *obj_butterfly, 
	gx3dObjectLayer *leftwing,
	gx3dObjectLayer *rightwing,
	float wing_angle)
{
	gx3dColor color3d_white = { 1, 1, 1, 0 };
	gx3dMatrix m, m1, m2, m3, m4, m5;

	int i;
	float curtime, timeindex, t, heading;
	gx3dVector v;
	curtime = (float)timeGetTime() / 1000;

	// Draw the animated object
	gx3d_EnableAlphaBlending();
	gx3d_EnableAlphaTesting(128);

	gx3d_DisableZBuffer();
	for (int i = 0; i < 10; i++)
	{
		wing_angle += 5;
		timeindex = curtime - ((int)(curtime / anim_model[i].length) * anim_model[i].length);
		// Search keys to find the two keys in between
		int j;
		for (j = 0; timeindex > anim_model[i].keys[j + 1].time; j++);
		// On last key?
		if (j == anim_model[i].num_keys - 1)
		{
			// Just use the last key
			v = anim_model[i].keys[j].position;
			heading = anim_model[i].keys[j].heading;
		}
		else
		{
			// Interpolate between 2 keys
			t = (timeindex - anim_model[i].keys[j].time) / (anim_model[i].keys[j + 1].time - anim_model[i].keys[j].time);
			gx3d_LerpVector (&(anim_model[i].keys[j].position), &(anim_model[i].keys[j + 1].position), t, &v);
			heading = gx3d_Lerp (anim_model[i].keys[j].heading, anim_model[i].keys[j + 1].heading, t);
		}

		gx3d_GetRotateYMatrix (&m1, heading);
		gx3d_GetTranslateMatrix (&m2, v.x, v.y, v.z);
		gx3d_MultiplyMatrix (&m1, &m2, &m);
		gx3d_SetObjectMatrix (obj_butterfly, &m);
		gx3d_GetRotateZMatrix (&m, wing_angle);
		gx3d_SetObjectLayerMatrix (obj_butterfly, leftwing, &m);
		gx3d_GetRotateZMatrix (&m, -wing_angle);
		gx3d_SetObjectLayerMatrix (obj_butterfly, rightwing, &m);
		gx3d_SetAmbientLight (color3d_white);
		CheckDrowObject (obj_butterfly, v.x, v.y, v.z);
		animation_buttferfly[i].min.x += v.x;
		animation_buttferfly[i].min.y += v.y;
		animation_buttferfly[i].min.z += v.z;
		animation_buttferfly[i].max.x += v.x;
		animation_buttferfly[i].max.y += v.y;
		animation_buttferfly[i].max.z += v.z;

	}

	gx3d_EnableZBuffer();
		
}


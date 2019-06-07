// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt
// Extended for VR by Leo Reyes, 2019

// Shaders by Marty McFly (used with permission from the author)
// https://github.com/martymcmodding/qUINT/tree/master/Shaders

/*

Trevor:
Steamvr_mat.txt
Driver: oculus
Display: 20B06R03EVHM

eyeLeft:
1.000000, 0.000000, 0.000000, -0.031750
0.000000, 1.000000, 0.000000, 0.000000
0.000000, 0.000000, 1.000000, 0.000000

eyeRight:
1.000000, 0.000000, 0.000000, 0.031750
0.000000, 1.000000, 0.000000, 0.000000
0.000000, 0.000000, 1.000000, 0.000000

projLeft:
0.929789, 0.000000, 0.015672, 0.000000
0.000000, 0.750974, 0.000000, 0.000000
0.000000, 0.000000, -1.010101, -0.505050
0.000000, 0.000000, -1.000000, 0.000000

projRight:
0.929789, 0.000000, -0.015672, 0.000000
0.000000, 0.750974, 0.000000, 0.000000
0.000000, 0.000000, -1.010101, -0.505050
0.000000, 0.000000, -1.000000, 0.000000

Raw data (Left eye):
Left: -1.058658, Right: 1.092368, Top: -1.331603, Bottom: 1.331603
Raw data (Right eye):
Left: -1.092368, Right: 1.058658, Top: -1.331603, Bottom: 1.331603


FullBody:

Driver: lighthouse
Display: LHR-3395DDAF

eyeLeft:
1.000000, 0.000000, 0.000000, -0.033050
0.000000, 1.000000, 0.000000, 0.000000
0.000000, 0.000000, 1.000000, 0.015000

eyeRight:
1.000000, 0.000000, 0.000000, 0.033050
0.000000, 1.000000, 0.000000, 0.000000
0.000000, 0.000000, 1.000000, 0.015000

projLeft:
0.756668, 0.000000, -0.056751, 0.000000
0.000000, 0.681272, -0.001673, 0.000000
0.000000, 0.000000, -1.010101, -0.505050
0.000000, 0.000000, -1.000000, 0.000000

projRight:
0.757176, 0.000000, 0.056972, 0.000000
0.000000, 0.681598, -0.005991, 0.000000
0.000000, 0.000000, -1.010101, -0.505050
0.000000, 0.000000, -1.000000, 0.000000

Raw data (Left eye):
Left: -1.396584, Right: 1.246583, Top: -1.470297, Bottom: 1.465387
Raw data (Right eye):
Left: -1.245455, Right: 1.395940, Top: -1.475930, Bottom: 1.458352


Chee:

Driver: aapvr
Display: Pimax 5K Plus

eyeLeft:
0.984808, 0.000000, 0.173648, -0.033236
0.000000, 1.000000, 0.000000, 0.000000
-0.173648, 0.000000, 0.984808, 0.000000

eyeRight:
0.984808, -0.000000, -0.173648, 0.033236
0.000000, 1.000000, -0.000000, 0.000000
0.173648, 0.000000, 0.984808, 0.000000

projLeft:
0.647594, 0.000000, -0.128239, 0.000000
0.000000, 0.787500, 0.000000, 0.000000
0.000000, 0.000000, -1.010101, -0.505050
0.000000, 0.000000, -1.000000, 0.000000

projRight:
0.647594, 0.000000, 0.128239, 0.000000
0.000000, 0.787500, 0.000000, 0.000000
0.000000, 0.000000, -1.010101, -0.505050
0.000000, 0.000000, -1.000000, 0.000000

Raw data (Left eye):
Left: -1.742203, Right: 1.346154, Top: -1.269841, Bottom: 1.269841
Raw data (Right eye):
Left: -1.346154, Right: 1.742203, Top: -1.269841, Bottom: 1.269841

*/

#include "common.h"
#include "DeviceResources.h"
#include "Direct3DDevice.h"
#include "Direct3DExecuteBuffer.h"
#include "Direct3DTexture.h"
#include "BackbufferSurface.h"
#include "ExecuteBufferDumper.h"

#include <ScreenGrab.h>
#include <wincodec.h>
#include <vector>

#include <headers/openvr.h>
#include "Matrices.h"

#define DBG_MAX_PRESENT_LOGS 0

FILE *g_HackFile = NULL;

const float DEFAULT_FOCAL_DIST = 0.5f;
//const float DEFAULT_FOCAL_DIST_STEAMVR = 0.6f;
const float DEFAULT_IPD = 6.5f;
const float DEFAULT_HUD_PARALLAX = 12.5f;
const float DEFAULT_TEXT_PARALLAX = 40.0f;
const float DEFAULT_FLOATING_GUI_PARALLAX = 34.5f;
const float DEFAULT_TECH_LIB_PARALLAX = 0.05f;
const float DEFAULT_FLOATING_OBJ_PARALLAX = 3.0f;
const float DEFAULT_GUI_ELEM_SCALE = 0.75f;
const float DEFAULT_GUI_ELEM_PZ_THRESHOLD = 0.0008f;
const float DEFAULT_ZOOM_OUT_SCALE = 0.8f;
const float DEFAULT_ASPECT_RATIO = 1.33f;
const float DEFAULT_CONCOURSE_SCALE = 0.4f;
const float DEFAULT_CONCOURSE_ASPECT_RATIO = 2.0f; // Default for non-SteamVR
const float DEFAULT_GLOBAL_SCALE = 1.75f;
//const float DEFAULT_GLOBAL_SCALE_STEAMVR = 1.4f;
const float DEFAULT_LENS_K1 = 2.0f;
const float DEFAULT_LENS_K2 = 0.22f;
const float DEFAULT_LENS_K3 = 0.0f;
const float DEFAULT_COCKPIT_PZ_THRESHOLD = 0.166f; // I used 0.13f for a long time until I jumped on a TIE-Interceptor
const int DEFAULT_SKYBOX_INDEX = 2;
const bool DEFAULT_INTERLEAVED_REPROJECTION = false;
const bool DEFAULT_BARREL_EFFECT_STATE = true;
const bool DEFAULT_BARREL_EFFECT_STATE_STEAMVR = false; // SteamVR provides its own lens correction, only enable it if the user really wants it
const float DEFAULT_BRIGHTNESS = 0.95f;
const float MAX_BRIGHTNESS = 1.0f;

const char *FOCAL_DIST_VRPARAM = "focal_dist";
const char *STEREOSCOPY_STRENGTH_VRPARAM = "IPD";
const char *SIZE_3D_WINDOW_VRPARAM = "3d_window_size";
const char *SIZE_3D_WINDOW_ZOOM_OUT_VRPARAM = "3d_window_zoom_out_size";
const char *CONCOURSE_WINDOW_SCALE_VRPARAM = "concourse_window_scale";
const char *COCKPIT_Z_THRESHOLD_VRPARAM = "cockpit_z_threshold";
const char *ASPECT_RATIO_VRPARAM = "3d_aspect_ratio";
const char *CONCOURSE_ASPECT_RATIO_VRPARAM = "concourse_aspect_ratio";
const char *K1_VRPARAM = "k1";
const char *K2_VRPARAM = "k2";
const char *K3_VRPARAM = "k3";
const char *HUD_PARALLAX_VRPARAM = "HUD_parallax";
const char *GUI_PARALLAX_VRPARAM = "GUI_parallax";
const char *GUI_OBJ_PARALLAX_VRPARAM = "GUI_target_parallax";
const char *TEXT_PARALLAX_VRPARAM = "Text_parallax";
const char *TECH_LIB_PARALLAX_VRPARAM = "Tech_Library_parallax";
const char *BRIGHTNESS_VRPARAM = "brightness";
const char *VR_MODE_VRPARAM = "VR_Mode"; // Select "None", "DirectSBS" or "SteamVR"
const char *VR_MODE_NONE_SVAL = "None";
const char *VR_MODE_DIRECT_SBS_SVAL = "DirectSBS";
const char *VR_MODE_STEAMVR_SVAL = "SteamVR";
const char *INTERLEAVED_REPROJ_VRPARAM = "SteamVR_Interleaved_Reprojection";
const char *BARREL_EFFECT_STATE_VRPARAM = "apply_lens_correction";

/*
typedef enum {
	VR_MODE_NONE,
	VR_MODE_DIRECT_SBS,
	VR_MODE_STEAMVR
} VRModeEnum;
VRModeEnum g_VRMode = VR_MODE_DIRECT_SBS;
*/

/* SteamVR HMD */
vr::IVRSystem *g_pHMD = NULL;
vr::IVRCompositor *g_pVRCompositor = NULL;
vr::IVRScreenshots *g_pVRScreenshots = NULL;
vr::TrackedDevicePose_t g_rTrackedDevicePose;
uint32_t g_steamVRWidth = 0, g_steamVRHeight = 0; // The resolution recommended by SteamVR is stored here
bool g_bSteamVREnabled = false; // The user sets this flag to true to request support for SteamVR.
bool g_bSteamVRInitialized = false; // The system will set this flag after SteamVR has been initialized
bool g_bUseSteamVR = false; // The system will set this flag if the user requested SteamVR and SteamVR was initialized properly
bool g_bInterleavedReprojection = DEFAULT_INTERLEAVED_REPROJECTION;
vr::HmdMatrix34_t g_EyeMatrixLeft, g_EyeMatrixRight;
//vr::HmdMatrix44_t g_EyeMatrixLeftInv, g_EyeMatrixRightInv;
Matrix4 g_EyeMatrixLeftInv, g_EyeMatrixRightInv;
//vr::HmdMatrix44_t g_projLeft, g_projRight;
Matrix4 g_projLeft, g_projRight;
Matrix4 g_fullMatrixLeft, g_fullMatrixRight;
VertexShaderMatrixCB g_VSMatrixCB;
void projectSteamVR(float X, float Y, float Z, vr::EVREye eye, float &x, float &y, float &z);

/* Vertices that will be used for the VertexBuffer. */
D3DTLVERTEX *g_OrigVerts = NULL, *g_LeftVerts = NULL, *g_RightVerts = NULL;
D3DTLVERTEX *g_3DVerts = NULL;
int g_iNumVertices = 0;

// Counter for calls to DrawIndexed() (This helps us know where were are in the rendering process)
// Gets reset everytime the backbuffer is presented and is increased only after BOTH the left and
// right images have been rendered.
int g_iDrawCounter = 0, g_iNoDrawBeforeIndex = 0, g_iNoDrawAfterIndex = -1;
// Similar to the above, but only get incremented after each Execute() is finished.
int g_iExecBufCounter = 0, g_iNoExecBeforeIndex = 0, g_iNoExecAfterIndex = -1;
int g_iSkyBoxExecIndex = DEFAULT_SKYBOX_INDEX; // This gives us the threshold for the Skybox
// g_iSkyBoxExecIndex is compared against g_iExecBufCounter to determine when the SkyBox is rendered
// This is important because in XWAU the SkyBox is *not* rendered at infinity and causes a lot of
// visual contention if not handled properly.

bool g_bFixSkyBox = true; // Fix the skybox (send it to infinity: use original vertices without parallax)
bool g_bSkipSkyBox = false;
bool g_bStartedGUI = false; // Set to false at the beginning of each frame. Set to true when the GUI has begun rendering.
bool g_bSkipGUI = false; // Skip non-skybox draw calls with disabled Z-Buffer
bool g_bSkipText = false; // Skips text draw calls
bool g_bSkipAfterTargetComp = false; // Skip all draw calls after the targetting computer has been drawn
bool g_bTargetCompDrawn = false; // Becomes true after the targetting computer has been drawn
unsigned int g_iFloatingGUIDrawnCounter = 0;
int g_iPresentCounter = 0, g_iNonZBufferCounter = 0, g_iSkipNonZBufferDrawIdx = -1;
// The following flag tells us when the main GUI elements (HUD, radars, etc) have been rendered
// It's reset to false every time the backbuffer is swapped.
//bool g_bGUIIsRendered = false;
float g_fZOverride = 0.05f; // 0 is Z-Far and 1 is ZNear in ZBuffer-Log coords. 0.05 is almost at ZFar
//float g_fZOverride = 0.95f;

// g_fZOverride is activated when it's greater than -0.9f, and it's used for bracket rendering so that 
// objects cover the brackets. In this way, we avoid visual contention from the brackets.
bool g_bCockpitPZHackEnabled = true;
bool g_bOverrideAspectRatio = false;
bool g_bEnableVR = true; // Enable/disable VR mode.

bool g_bDumpSpecificTex = false;
int g_iDumpSpecificTexIdx = 0;
bool g_bDisplayWidth = false;
extern bool g_bDumpDebug;

// This is the current resolution of the screen:
float g_fLensK1 = DEFAULT_LENS_K1;
float g_fLensK2 = DEFAULT_LENS_K2;
float g_fLensK3 = DEFAULT_LENS_K3;

// GUI elements seem to be in the range 0..0.0005, so 0.0008 sounds like a good threshold:
float g_fGUIElemPZThreshold = DEFAULT_GUI_ELEM_PZ_THRESHOLD;
float g_fGUIElemScale = DEFAULT_GUI_ELEM_SCALE;
float g_fGlobalScale = DEFAULT_GLOBAL_SCALE;
float g_fGlobalScaleZoomOut = DEFAULT_ZOOM_OUT_SCALE;
float g_fConcourseScale = DEFAULT_CONCOURSE_SCALE;
float g_fConcourseAspectRatio = DEFAULT_CONCOURSE_ASPECT_RATIO;
float g_fHUDParallax = DEFAULT_HUD_PARALLAX;   // The aiming HUD is rendered at this depth
float g_fTextParallax = DEFAULT_TEXT_PARALLAX; // All text gets rendered at this parallax
float g_fFloatingGUIParallax = DEFAULT_FLOATING_GUI_PARALLAX; // Floating GUI elements are rendered at this depth
float g_fFloatingGUIObjParallax = DEFAULT_FLOATING_OBJ_PARALLAX; // The targeted object must be rendered above the Floating GUI
float g_fTechLibraryParallax = DEFAULT_TECH_LIB_PARALLAX;
float g_fAspectRatio = DEFAULT_ASPECT_RATIO;
bool g_bZoomOut = false;
float g_fBrightness = DEFAULT_BRIGHTNESS;
float g_fGUIElemsScale = DEFAULT_GLOBAL_SCALE; // Used to reduce the size of all the GUI elements

VertexShaderCBuffer g_VSCBuffer;
PixelShaderCBuffer g_PSCBuffer;

float g_fCockpitPZThreshold = DEFAULT_COCKPIT_PZ_THRESHOLD; // The TIE-Interceptor needs this thresold!
float g_fBackupCockpitPZThreshold = g_fCockpitPZThreshold; // Backup of the cockpit threshold, used when toggling this effect on or off.

const float IPD_SCALE_FACTOR = 100.0f; // Transform centimeters to meters (IPD = 6.5 becomes 0.065)
const float GAME_SCALE_FACTOR = 60.0f; // Estimated empirically
const float GAME_SCALE_FACTOR_Z = 60.0f; // Estimated empirically
//const float GAME_SCALE_FACTOR = 2.0f; // Estimated empirically
//const float GAME_SCALE_FACTOR_Z = 2.0f; // Estimated empirically

										 //const float GAME_SCALE_FACTOR = 30.0f; // Estimated empirically
//const float GAME_SCALE_FACTOR = 1.0f; // Estimated empirically

// In reality, there should be a different factor per in-game resolution; but for now this should be enough
//const float C = 1.0f, Z_FAR = 50.0f;
//const float LOG_K = log(C*Z_FAR + 1.0f);

const float C = 1.0f, Z_FAR = 1.0f;
const float LOG_K = 1.0f;

float g_fIPD = DEFAULT_IPD / IPD_SCALE_FACTOR;
float g_fHalfIPD = g_fIPD / 2.0f;
float g_fFocalDist = DEFAULT_FOCAL_DIST;

/*
 * Control/Debug variables
 */
bool g_bDisableZBuffer = false;
bool g_bDisableBarrelEffect = false;
bool g_bStart3DCapture = false, g_bDo3DCapture = false, g_bSkipTexturelessGUI = false;
// g_bSkipTexturelessGUI skips draw calls that don't have a texture associated (lastTextureSelected == NULL)
bool g_bDumpGUI = false;
int g_iHUDTexDumpCounter = 0;
int g_iDumpGUICounter = 0, g_iHUDCounter = 0;
#undef INDEX_BUF_SAVE

//extern std::vector<uint32_t> Floating_GUI_CRCs; // Remove this later, it's only here for debugging purposes.
//extern uint32_t *HUD_CRCs; // Remove this later, it's only here for debugging purposes.

/* Reloads all the CRCs. */
bool ReloadCRCs();
bool isInVector(uint32_t crc, std::vector<uint32_t> &vector);
void DeleteStereoVertices();

/* Maps (-6, 6) to (-0.5, 0.5) using a sigmoid function */
float centeredSigmoid(float x) {
	return 1.0f / (1.0f + exp(-x)) - 0.5f;
}

/* Maps (-6, 6) to (-0.5, 0.5) using a sine function */
/*
const float PI = 3.141592f;
const float HALF_PI = PI / 2.0f;
float centered_sine(float x) {
	return sin(HALF_PI * (x / 6.0f)) * 0.5f;
}
*/

typedef struct HeadPosStruct {
	float x, y, z;
} HeadPos;

HeadPos g_HeadPosAnim = { 0 }, g_HeadPos = { 0 };
bool g_bLeftKeyDown, g_bRightKeyDown, g_bUpKeyDown, g_bDownKeyDown, g_bUpKeyDownShift, g_bDownKeyDownShift;
const float ANIM_INCR = 0.1f, MAX_LEAN_X = 0.015f, MAX_LEAN_Y = 0.015f, MAX_LEAN_Z = 0.03f;

void animTickX() {
	if (g_bRightKeyDown)
		g_HeadPosAnim.x += ANIM_INCR;
	else if (g_bLeftKeyDown)
		g_HeadPosAnim.x -= ANIM_INCR;
	else if (!g_bRightKeyDown && !g_bLeftKeyDown) {
		if (g_HeadPosAnim.x < 0.0001)
			g_HeadPosAnim.x += ANIM_INCR;
		if (g_HeadPosAnim.x > 0.0001)
			g_HeadPosAnim.x -= ANIM_INCR;
	}

	// Range clamping
	if (g_HeadPosAnim.x >  6.0f)  g_HeadPosAnim.x =  6.0f;
	if (g_HeadPosAnim.x < -6.0f)  g_HeadPosAnim.x = -6.0f;

	g_HeadPos.x = centeredSigmoid(g_HeadPosAnim.x) * MAX_LEAN_X;
}

void animTickY() {
	if (g_bDownKeyDown)
		g_HeadPosAnim.y += ANIM_INCR;
	else if (g_bUpKeyDown)
		g_HeadPosAnim.y -= ANIM_INCR;
	else if (!g_bDownKeyDown && !g_bUpKeyDown) {
		if (g_HeadPosAnim.y < 0.0001)
			g_HeadPosAnim.y += ANIM_INCR;
		if (g_HeadPosAnim.y > 0.0001)
			g_HeadPosAnim.y -= ANIM_INCR;
	}

	// Range clamping
	if (g_HeadPosAnim.y >  6.0f)  g_HeadPosAnim.y =  6.0f;
	if (g_HeadPosAnim.y < -6.0f)  g_HeadPosAnim.y = -6.0f;

	g_HeadPos.y = centeredSigmoid(g_HeadPosAnim.y) * MAX_LEAN_Y;
}

void animTickZ() {
	if (g_bDownKeyDownShift)
		g_HeadPosAnim.z -= ANIM_INCR;
	else if (g_bUpKeyDownShift)
		g_HeadPosAnim.z += ANIM_INCR;
	else if (!g_bDownKeyDownShift && !g_bUpKeyDownShift) {
		if (g_HeadPosAnim.z < 0.0001)
			g_HeadPosAnim.z += ANIM_INCR;
		if (g_HeadPosAnim.z > 0.0001)
			g_HeadPosAnim.z -= ANIM_INCR;
	}

	// Range clamping
	if (g_HeadPosAnim.z >  6.0f)  g_HeadPosAnim.z =  6.0f;
	if (g_HeadPosAnim.z < -6.0f)  g_HeadPosAnim.z = -6.0f;

	g_HeadPos.z = centeredSigmoid(g_HeadPosAnim.z) * MAX_LEAN_Z;
}

// NewIPD is in cms
void EvaluateIPD(float NewIPD) {
	/* if (NewIPD < 0.0f)
		NewIPD = 0.0f;
	if (NewIPD > 12.0f) {
		NewIPD = 12.0f;
	} */
	g_fIPD = NewIPD / IPD_SCALE_FACTOR;
	log_debug("[DBG] NewIPD: %0.3f, Actual g_fIPD: %0.6f", NewIPD, g_fIPD);
	g_fHalfIPD = g_fIPD / 2.0f;
	//log_debug("[DBG] New IPD: %f", IPD);
}

// Delta is in cms here
void IncreaseIPD(float Delta) {
	float NewIPD = g_fIPD * IPD_SCALE_FACTOR + Delta;
	EvaluateIPD(NewIPD);
}

void ToggleCockpitPZHack() {
	g_bCockpitPZHackEnabled = !g_bCockpitPZHackEnabled;
	//log_debug("[DBG] CockpitHackEnabled: %d", g_bCockpitPZHackEnabled);
	if (!g_bCockpitPZHackEnabled)
		g_fCockpitPZThreshold = 2.0f;
	else
		g_fCockpitPZThreshold = g_fBackupCockpitPZThreshold;
}

void ToggleZoomOutMode() {
	g_bZoomOut = !g_bZoomOut;
	g_fGUIElemsScale = g_bZoomOut ? g_fGlobalScaleZoomOut : g_fGlobalScale;
}

void IncreaseZOverride(float Delta) {
	g_fZOverride += Delta;
	//log_debug("[DBG] g_fZOverride: %f", g_fZOverride);
}

void IncreaseZoomOutScale(float Delta) {
	g_fGlobalScaleZoomOut += Delta;
	if (g_fGlobalScaleZoomOut < 0.2f)
		g_fGlobalScaleZoomOut = 0.2f;

	// Apply this change by modifying the global scale:
	g_fGUIElemsScale = g_bZoomOut ? g_fGlobalScaleZoomOut : g_fGlobalScale;

	g_fConcourseScale += Delta;
	if (g_fConcourseScale < 0.2f)
		g_fConcourseScale = 0.2f;
}

void IncreaseHUDParallax(float Delta) {
	g_fHUDParallax += Delta;
	log_debug("[DBG] HUD parallax: %f", g_fHUDParallax);
}

void IncreaseFloatingGUIParallax(float Delta) {
	g_fFloatingGUIParallax += Delta;
	log_debug("[DBG] GUI parallax: %f", g_fFloatingGUIParallax);
}

void IncreaseTextParallax(float Delta) {
	g_fTextParallax += Delta;
	log_debug("[DBG] Text parallax: %f", g_fTextParallax);
}

void IncreaseCockpitThreshold(float Delta) {
	g_fCockpitPZThreshold += Delta;
	g_fBackupCockpitPZThreshold = g_fCockpitPZThreshold;
	log_debug("[DBG] New cockpit threshold: %f", g_fCockpitPZThreshold);
}

void IncreaseGUIThreshold(float Delta) {
	g_fGUIElemPZThreshold += Delta;
	//log_debug("[DBG] New GUI threshold: %f", GUI_elem_pz_threshold);
}

void IncreaseScreenScale(float Delta) {
	g_fGlobalScale += Delta;
	if (g_fGlobalScale < 0.2f)
		g_fGlobalScale = 0.2f;
	log_debug("[DBG] New g_fGlobalScale: %f", g_fGlobalScale);
}

void IncreaseFocalDist(float Delta) {
	g_fFocalDist += Delta;
	if (g_fFocalDist < 0.01f)
		g_fFocalDist = 0.01f;
	log_debug("[DBG] g_fFocalDist: %f", g_fFocalDist);
}

void IncreaseNoDrawBeforeIndex(int Delta) {
	g_iNoDrawBeforeIndex += Delta;
	log_debug("[DBG] NoDraw BeforeIdx, AfterIdx: %d, %d", g_iNoDrawBeforeIndex, g_iNoDrawAfterIndex);
}

void IncreaseNoDrawAfterIndex(int Delta) {
	g_iNoDrawAfterIndex += Delta;
	log_debug("[DBG] NoDraw BeforeIdx, AfterIdx: %d, %d", g_iNoDrawBeforeIndex, g_iNoDrawAfterIndex);
}

void IncreaseNoExecIndices(int DeltaBefore, int DeltaAfter) {
	g_iNoExecBeforeIndex += DeltaBefore;
	g_iNoExecAfterIndex += DeltaAfter;
	if (g_iNoExecBeforeIndex < -1)
		g_iNoExecBeforeIndex = -1;

	log_debug("[DBG] NoExec BeforeIdx, AfterIdx: %d, %d", g_iNoExecBeforeIndex, g_iNoExecAfterIndex);
}

void IncreaseSkipNonZBufferDrawIdx(int Delta) {
	g_iSkipNonZBufferDrawIdx += Delta;
	log_debug("[DBG] New g_iSkipNonZBufferDrawIdx: %d", g_iSkipNonZBufferDrawIdx);
}

void IncreaseSkyBoxIndex(int Delta) {
	g_iSkyBoxExecIndex += Delta;
	log_debug("[DBG] New g_iSkyBoxExecIndex: %d", g_iSkyBoxExecIndex);
}

void IncreaseLensK1(float Delta) {
	g_fLensK1 += Delta;
	log_debug("[DBG] New k1: %f", g_fLensK1);
}

void IncreaseLensK2(float Delta) {
	g_fLensK2 += Delta;
	log_debug("[DBG] New k2: %f", g_fLensK2);
}

/* Restores the various VR parameters to their default values. */
void ResetVRParams() {
	//g_fFocalDist = g_bSteamVREnabled ? DEFAULT_FOCAL_DIST_STEAMVR : DEFAULT_FOCAL_DIST;
	g_fFocalDist = DEFAULT_FOCAL_DIST;
	EvaluateIPD(DEFAULT_IPD);
	g_bCockpitPZHackEnabled = true;
	g_fGUIElemPZThreshold = DEFAULT_GUI_ELEM_PZ_THRESHOLD;
	g_fGUIElemScale = DEFAULT_GUI_ELEM_SCALE;
	//g_fGlobalScale = g_bSteamVREnabled ? DEFAULT_GLOBAL_SCALE_STEAMVR : DEFAULT_GLOBAL_SCALE;
	g_fGlobalScale = DEFAULT_GLOBAL_SCALE;
	g_fGlobalScaleZoomOut = DEFAULT_ZOOM_OUT_SCALE;
	g_fGUIElemsScale = g_bZoomOut ? g_fGlobalScaleZoomOut : g_fGlobalScale;
	g_fConcourseScale = DEFAULT_CONCOURSE_SCALE;
	g_fCockpitPZThreshold = DEFAULT_COCKPIT_PZ_THRESHOLD;
	g_fBackupCockpitPZThreshold = g_fCockpitPZThreshold;

	g_fAspectRatio = DEFAULT_ASPECT_RATIO;
	g_fConcourseAspectRatio = DEFAULT_CONCOURSE_ASPECT_RATIO;
	g_fLensK1 = DEFAULT_LENS_K1;
	g_fLensK2 = DEFAULT_LENS_K2;
	g_fLensK3 = DEFAULT_LENS_K3;

	g_bFixSkyBox = true;
	g_iSkyBoxExecIndex = DEFAULT_SKYBOX_INDEX;
	g_bSkipText = false;
	g_bSkipGUI = false;
	g_bSkipSkyBox = false;

	g_iNoDrawBeforeIndex = 0; g_iNoDrawAfterIndex = -1;
	g_iNoExecBeforeIndex = 0; g_iNoExecAfterIndex = -1;
	g_fHUDParallax = DEFAULT_HUD_PARALLAX;
	g_fTextParallax = DEFAULT_TEXT_PARALLAX;
	g_fFloatingGUIParallax = DEFAULT_FLOATING_GUI_PARALLAX;
	g_fTechLibraryParallax = DEFAULT_TECH_LIB_PARALLAX;
	g_fFloatingGUIObjParallax = DEFAULT_FLOATING_OBJ_PARALLAX;

	g_fBrightness = DEFAULT_BRIGHTNESS;

	g_bInterleavedReprojection = DEFAULT_INTERLEAVED_REPROJECTION;
	if (g_bUseSteamVR)
		g_pVRCompositor->ForceInterleavedReprojectionOn(g_bInterleavedReprojection);

	g_bDisableBarrelEffect = g_bUseSteamVR ? !DEFAULT_BARREL_EFFECT_STATE_STEAMVR : !DEFAULT_BARREL_EFFECT_STATE;
	// Load CRCs
	ReloadCRCs();
}

/* Saves the current view parameters to vrparams.cfg */
void SaveVRParams() {
	FILE *file;
	int error = 0;
	
	try {
		error = fopen_s(&file, "./vrparams.cfg", "wt");
	} catch (...) {
		log_debug("[DBG] Could not save vrparams.cfg");
	}

	if (error != 0) {
		log_debug("[DBG] Error %d when saving vrparams.cfg", error);
		return;
	}
	fprintf(file, "; VR parameters. Write one parameter per line.\n");
	fprintf(file, "; Always make a backup copy of this file before modifying it.\n");
	fprintf(file, "; If you want to restore it to its default settings, simply delete the\n");
	fprintf(file, "; file and restart the game.Then press Ctrl + Alt + S to save a\n");
	fprintf(file, "; new config file with the default parameters.\n");
	fprintf(file, "; To reload this file during game (at any point) just press Ctrl+Alt+L.\n");
	fprintf(file, "; You can also press Ctrl+Alt+R to reset the viewing params to default values.\n\n");

	fprintf(file, "; VR Mode. Select from None, DirectSBS and SteamVR.\n");
	if (!g_bEnableVR)
		fprintf(file, "%s = %s\n", VR_MODE_VRPARAM, VR_MODE_NONE_SVAL);
	else {
		if (!g_bSteamVREnabled)
			fprintf(file, "%s = %s\n", VR_MODE_VRPARAM, VR_MODE_DIRECT_SBS_SVAL);
		else
			fprintf(file, "%s = %s\n", VR_MODE_VRPARAM, VR_MODE_STEAMVR_SVAL);
	}
	fprintf(file, "\n");

	//fprintf(file, "focal_dist = %0.6f # Try not to modify this value, change IPD instead.\n", focal_dist);

	fprintf(file, "; %s is measured in cms; but it's an approximation to in-game units. Set it to 0 to\n", STEREOSCOPY_STRENGTH_VRPARAM);
	fprintf(file, "; remove the stereoscopy effect. The maximum allowed by the engine is 12cm\n");
	fprintf(file, "%s = %0.1f\n", STEREOSCOPY_STRENGTH_VRPARAM, g_fIPD * IPD_SCALE_FACTOR);
	fprintf(file, "%s = %0.3f\n", SIZE_3D_WINDOW_VRPARAM, g_fGlobalScale);
	fprintf(file, "%s = %0.3f\n", SIZE_3D_WINDOW_ZOOM_OUT_VRPARAM, g_fGlobalScaleZoomOut);
	fprintf(file, "%s = %0.3f\n", CONCOURSE_WINDOW_SCALE_VRPARAM, g_fConcourseScale);
	fprintf(file, "; The following is a hack to increase the stereoscopy on objects. Unfortunately it\n");
	fprintf(file, "; also causes some minor artifacts: this is basically the threshold between the\n");
	fprintf(file, "; cockpit and the 'outside' world in normalized coordinates (0 is ZNear 1 is ZFar).\n");
	fprintf(file, "; Set it to 2 to disable this hack (stereoscopy will be reduced).\n");
	fprintf(file, "%s = %0.3f\n\n", COCKPIT_Z_THRESHOLD_VRPARAM, g_fCockpitPZThreshold);

	fprintf(file, "; Specify the aspect ratio here to override the aspect ratio computed by the library.\n");
	fprintf(file, "; ALWAYS specify BOTH the Concourse and 3D window aspect ratio.\n");
	fprintf(file, "; You can also edit ddraw.cfg and set 'PreserveAspectRatio = 1' to get the library to\n");
	fprintf(file, "; estimate the aspect ratio for you (this is the preferred method).\n");
	fprintf(file, "%s = %0.3f\n", ASPECT_RATIO_VRPARAM, g_fAspectRatio);
	fprintf(file, "%s = %0.3f\n", CONCOURSE_ASPECT_RATIO_VRPARAM, g_fConcourseAspectRatio);

	fprintf(file, "\n; Lens correction parameters. k2 has the biggest effect and k1 fine-tunes the effect.\n");
	fprintf(file, "%s = %0.6f\n", K1_VRPARAM, g_fLensK1);
	fprintf(file, "%s = %0.6f\n", K2_VRPARAM, g_fLensK2);
	fprintf(file, "%s = %0.6f\n", K3_VRPARAM, g_fLensK3);
	fprintf(file, "%s = %d\n", BARREL_EFFECT_STATE_VRPARAM, !g_bDisableBarrelEffect);

	fprintf(file, "\n; Depth for various GUI elements. Set these to 0 to put them at infinity (ZFar).\n");
	fprintf(file, "%s = %0.3f\n", HUD_PARALLAX_VRPARAM, g_fHUDParallax);
	fprintf(file, "%s = %0.3f\n", GUI_PARALLAX_VRPARAM, g_fFloatingGUIParallax);
	fprintf(file, "%s = %0.3f\n", GUI_OBJ_PARALLAX_VRPARAM, g_fFloatingGUIObjParallax);
	fprintf(file, "; %s is always added to %s\n", GUI_OBJ_PARALLAX_VRPARAM, GUI_PARALLAX_VRPARAM);
	fprintf(file, "; This has the effect of making the targeted object \"hover\" above the targeting computer\n");
	fprintf(file, "%s = %0.3f\n", TEXT_PARALLAX_VRPARAM, g_fTextParallax);
	fprintf(file, "; As a rule of thumb always make %s > %s so that\n", TEXT_PARALLAX_VRPARAM, GUI_PARALLAX_VRPARAM);
	fprintf(file, "; the text hovers above the targeting computer\n");
	fprintf(file, "%s = %0.3f\n", TECH_LIB_PARALLAX_VRPARAM, g_fTechLibraryParallax);

	fprintf(file, "\n");
	fprintf(file, "; Set the following parameter to lower the brightness of the text,\n");
	fprintf(file, "; Concourse and 2D menus (avoids unwanted bloom when using ReShade).\n");
	fprintf(file, "; A value of 1 is normal brightness, 0 will render everything black.\n");
	fprintf(file, "%s = %0.3f\n", BRIGHTNESS_VRPARAM, g_fBrightness);

	fprintf(file, "\n");
	fprintf(file, "%s = %d\n", INTERLEAVED_REPROJ_VRPARAM, g_bInterleavedReprojection);

	fclose(file);
	log_debug("[DBG] vrparams.cfg saved");
}

/* Loads the VR parameters from vrparams.cfg */
void LoadVRParams() {
	log_debug("[DBG] Loading view params...");
	FILE *file;
	int error = 0;

	try {
		error = fopen_s(&file, "./vrparams.cfg", "rt");
	} catch (...) {
		log_debug("[DBG] Could not load vrparams.cfg");
	}

	if (error != 0) {
		log_debug("[DBG] Error %d when loading vrparams.cfg", error);
		goto next;
	}

	char buf[160], param[80], svalue[80];
	int param_read_count = 0;
	float value = 0.0f;

	while (fgets(buf, 160, file) != NULL) {
		// Skip comments and blank lines
		if (buf[0] == ';' || buf[0] == '#')
			continue;
		if (strlen(buf) == 0)
			continue;

		if (sscanf_s(buf, "%s = %s", param, 80, svalue, 80) > 0) {
			value = (float )atof(svalue);
			if (_stricmp(param, FOCAL_DIST_VRPARAM) == 0) {
				g_fFocalDist = value;
			}
			else if (_stricmp(param, STEREOSCOPY_STRENGTH_VRPARAM) == 0) {
				EvaluateIPD(value);
			}
			else if (_stricmp(param, SIZE_3D_WINDOW_VRPARAM) == 0) {
				// Size of the window while playing the game
				g_fGlobalScale = value;
			}
			else if (_stricmp(param, SIZE_3D_WINDOW_ZOOM_OUT_VRPARAM) == 0) {
				// Size of the window while playing the game; but zoomed out to see all the GUI
				g_fGlobalScaleZoomOut = value;
			}
			else if (_stricmp(param, CONCOURSE_WINDOW_SCALE_VRPARAM) == 0) {
				// Concourse and 2D menus scale
				g_fConcourseScale = value;
			}
			else if (_stricmp(param, COCKPIT_Z_THRESHOLD_VRPARAM) == 0) {
				g_fCockpitPZThreshold = value;
			}
			else if (_stricmp(param, ASPECT_RATIO_VRPARAM) == 0) {
				g_fAspectRatio = value;
				g_bOverrideAspectRatio = true;
			}
			else if (_stricmp(param, CONCOURSE_ASPECT_RATIO_VRPARAM) == 0) {
				g_fConcourseAspectRatio = value;
				g_bOverrideAspectRatio = true;
			}
			else if (_stricmp(param, K1_VRPARAM) == 0) {
				g_fLensK1 = value;
			}
			else if (_stricmp(param, K2_VRPARAM) == 0) {
				g_fLensK2 = value;
			}
			else if (_stricmp(param, K3_VRPARAM) == 0) {
				g_fLensK3 = value;
			}
			else if (_stricmp(param, BARREL_EFFECT_STATE_VRPARAM) == 0) {
				g_bDisableBarrelEffect = !((bool)value);
			}
			else if (_stricmp(param, HUD_PARALLAX_VRPARAM) == 0) {
				g_fHUDParallax = value;
				//log_debug("[DBG] HUD Parallax read: %f", value);
			}
			else if (_stricmp(param, GUI_PARALLAX_VRPARAM) == 0) {
				// "Floating" GUI elements: targetting computer and the like
				g_fFloatingGUIParallax = value;
			}
			else if (_stricmp(param, GUI_OBJ_PARALLAX_VRPARAM) == 0) {
				// "Floating" GUI targeted elements
				g_fFloatingGUIObjParallax = value;
			}
			else if (_stricmp(param, TEXT_PARALLAX_VRPARAM) == 0) {
				g_fTextParallax = value;
			}
			else if (_stricmp(param, TECH_LIB_PARALLAX_VRPARAM) == 0) {
				g_fTechLibraryParallax = value;
			}
			else if (_stricmp(param, BRIGHTNESS_VRPARAM) == 0) {
				g_fBrightness = value;
			}
			else if (_stricmp(param, VR_MODE_VRPARAM) == 0) {
				if (_stricmp(svalue, VR_MODE_NONE_SVAL) == 0) {
					//g_VRMode = VR_MODE_NONE;
					g_bEnableVR = false;
					g_bSteamVREnabled = false;
					log_debug("[DBG] Disabling VR");
				}
				else if (_stricmp(svalue, VR_MODE_DIRECT_SBS_SVAL) == 0) {
					//g_VRMode = VR_MODE_DIRECT_SBS;
					g_bSteamVREnabled = false;
					g_bEnableVR = true;
					log_debug("[DBG] Using Direct SBS mode");
				}
				else if (_stricmp(svalue, VR_MODE_STEAMVR_SVAL) == 0) {
					//g_VRMode = VR_MODE_STEAMVR;
					g_bSteamVREnabled = true;
					g_bEnableVR = true;
					log_debug("[DBG] Using SteamVR");
				}
			}
			else if (_stricmp(param, INTERLEAVED_REPROJ_VRPARAM) == 0) {
				g_bInterleavedReprojection = (bool)value;
				if (g_bUseSteamVR && g_bInterleavedReprojection) {
					log_debug("[DBG] Setting Interleaved Reprojection to: %d", g_bInterleavedReprojection);
					g_pVRCompositor->ForceInterleavedReprojectionOn(g_bInterleavedReprojection);
				}

			}
			param_read_count++;
		}
	} // while ... read file
	g_fGUIElemsScale = g_bZoomOut ? g_fGlobalScaleZoomOut : g_fGlobalScale;
	fclose(file);

next:
	// Load CRCs
	ReloadCRCs();
}

/*
 * Back-projects a 2D coordinate + Logarithmic ZBuffer coordinate into 3D space.
 * The 2D coordinate must be in normalized space (-0.5..0.5) with the center of the image at (0,0).
 */
static inline void
back_project(float px, float py, float pz, float direct_pz, float &X, float &Y, float &Z)
{
	// This is the formula from qUINT_common.fxh for the logarithmic Z buffer:
	//const float C = 0.01;
	//depth = (exp(depth * log(C + 1.0)) - 1.0) / C;

	//Z = (exp(pz * LOG_K) - 1.0f) / C;
	Z = (exp(pz * LOG_K) - 1.0f) / direct_pz;
	X = Z * px / g_fFocalDist;
	Y = Z * py / g_fFocalDist;

	X *= GAME_SCALE_FACTOR;
	Y *= GAME_SCALE_FACTOR;
	Z *= GAME_SCALE_FACTOR_Z;
}

/*
 * Projects a 3D coordinate back into 2D normailzed space (center of the image is at
 * the origin).
 */
static inline void
project(float X, float Y, float Z, float &px, float &py, float &pz) 
{
	// Z = (exp(pz * LOG_K) - 1.0f) / direct_pz;
	// Z / direct_pz + 1.0f = exp(pz * LOG_K)
	//pz = log(Z * C + 1.0f) / LOG_K;
	//pz = log(Z/(1 - pz) * C + 1.0f) / LOG_K;
	px = X * g_fFocalDist / Z;
	py = Y * g_fFocalDist / Z;
}

////////////////////////////////////////////////////////////////
// SteamVR functions
////////////////////////////////////////////////////////////////
char *GetTrackedDeviceString(vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError *peError = NULL)
{
	char *pchBuffer = NULL;
	uint32_t unRequiredBufferLen = vr::VRSystem()->GetStringTrackedDeviceProperty(unDevice, prop, NULL, 0, peError);
	if (unRequiredBufferLen == 0)
		return pchBuffer;

	pchBuffer = new char[unRequiredBufferLen];
	unRequiredBufferLen = vr::VRSystem()->GetStringTrackedDeviceProperty(unDevice, prop, pchBuffer, unRequiredBufferLen, peError);
	std::string sResult = pchBuffer;
	return pchBuffer;
}

void DumpMatrix34(FILE *file, const vr::HmdMatrix34_t &m) {
	if (file == NULL)
		return;

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 4; j++) {
			fprintf(file, "%0.6f", m.m[i][j]);
			if (j < 3)
				fprintf(file, ", ");
		}
		fprintf(file, "\n");
	}
}

void DumpMatrix44(FILE *file, const vr::HmdMatrix44_t &m) {
	if (file == NULL)
		return;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			fprintf(file, "%0.6f", m.m[i][j]);
			if (j < 3)
				fprintf(file, ", ");
		}
		fprintf(file, "\n");
	}
}

void ShowMatrix4(const Matrix4 &mat, char *name) {
	log_debug("[DBG] -----------------------------------------");
	if (name != NULL)
		log_debug("[DBG] %s", name);
	log_debug("[DBG] %0.6f, %0.6f, %0.6f, %0.6f", mat[0], mat[4], mat[8], mat[12]);
	log_debug("[DBG] %0.6f, %0.6f, %0.6f, %0.6f", mat[1], mat[5], mat[9], mat[13]);
	log_debug("[DBG] %0.6f, %0.6f, %0.6f, %0.6f", mat[2], mat[6], mat[10], mat[14]);
	log_debug("[DBG] %0.6f, %0.6f, %0.6f, %0.6f", mat[3], mat[7], mat[11], mat[15]);
	log_debug("[DBG] =========================================");
}

void ShowHmdMatrix34(const vr::HmdMatrix34_t &mat, char *name) {
	log_debug("[DBG] -----------------------------------------");
	if (name != NULL)
		log_debug("[DBG] %s", name);
	log_debug("[DBG] %0.6f, %0.6f, %0.6f, %0.6f", mat.m[0][0], mat.m[0][1], mat.m[0][2], mat.m[0][3]);
	log_debug("[DBG] %0.6f, %0.6f, %0.6f, %0.6f", mat.m[1][0], mat.m[1][1], mat.m[1][2], mat.m[1][3]);
	log_debug("[DBG] %0.6f, %0.6f, %0.6f, %0.6f", mat.m[2][0], mat.m[2][1], mat.m[2][2], mat.m[2][3]);
	log_debug("[DBG] =========================================");
}

void ShowHmdMatrix44(const vr::HmdMatrix44_t &mat, char *name) {
	log_debug("[DBG] -----------------------------------------");
	if (name != NULL)
		log_debug("[DBG] %s", name);
	log_debug("[DBG] %0.6f, %0.6f, %0.6f, %0.6f", mat.m[0][0], mat.m[0][1], mat.m[0][2], mat.m[0][3]);
	log_debug("[DBG] %0.6f, %0.6f, %0.6f, %0.6f", mat.m[1][0], mat.m[1][1], mat.m[1][2], mat.m[1][3]);
	log_debug("[DBG] %0.6f, %0.6f, %0.6f, %0.6f", mat.m[2][0], mat.m[2][1], mat.m[2][2], mat.m[2][3]);
	log_debug("[DBG] %0.6f, %0.6f, %0.6f, %0.6f", mat.m[3][0], mat.m[3][1], mat.m[3][2], mat.m[3][3]);
	log_debug("[DBG] =========================================");
}

void Matrix4toHmdMatrix44(const Matrix4 &m4, vr::HmdMatrix44_t &mat) {
	mat.m[0][0] = m4[0];  mat.m[1][0] = m4[1];  mat.m[2][0] = m4[2];  mat.m[3][0] = m4[3];
	mat.m[0][1] = m4[4];  mat.m[1][1] = m4[5];  mat.m[2][1] = m4[6];  mat.m[3][1] = m4[7];
	mat.m[0][2] = m4[8];  mat.m[1][2] = m4[9];  mat.m[2][2] = m4[10]; mat.m[3][2] = m4[11];
	mat.m[0][3] = m4[12]; mat.m[1][3] = m4[13]; mat.m[2][3] = m4[14]; mat.m[3][3] = m4[14];
}

Matrix4 HmdMatrix44toMatrix4(const vr::HmdMatrix44_t &mat) {
	Matrix4 matrixObj(
		mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[3][0],
		mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[3][1],
		mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[3][2],
		mat.m[0][3], mat.m[1][3], mat.m[2][3], mat.m[3][3]
	);
	return matrixObj;
}

/*

Chee:
eyeLeft:
0.984808, 0.000000, 0.173648, -0.033236
0.000000, 1.000000, 0.000000, 0.000000
-0.173648, 0.000000, 0.984808, 0.000000

Fullbody:
eyeLeft:
1.000000, 0.000000, 0.000000, -0.033050
0.000000, 1.000000, 0.000000, 0.000000
0.000000, 0.000000, 1.000000, 0.015000

*/
void ProcessSteamVREyeMatrices(vr::EVREye eye) {
	if (g_pHMD == NULL) {
		log_debug("[DBG] Cannot process SteamVR matrices because g_pHMD == NULL");
		return;
	}

	vr::HmdMatrix34_t eyeMatrix = g_pHMD->GetEyeToHeadTransform(eye);
	//ShowHmdMatrix34(eyeMatrix, "HmdMatrix34_t eyeMatrix");
	if (eye == vr::EVREye::Eye_Left)
		g_EyeMatrixLeft = eyeMatrix;
	else
		g_EyeMatrixRight = eyeMatrix;

	Matrix4 matrixObj(
		eyeMatrix.m[0][0], eyeMatrix.m[1][0], eyeMatrix.m[2][0], 0.0f,
		eyeMatrix.m[0][1], eyeMatrix.m[1][1], eyeMatrix.m[2][1], 0.0f,
		eyeMatrix.m[0][2], eyeMatrix.m[1][2], eyeMatrix.m[2][2], 0.0f,
		eyeMatrix.m[0][3], eyeMatrix.m[1][3], eyeMatrix.m[2][3], 1.0f
	);

	/*
	// Pimax matrix: 11.11 degrees on the Y axis and 6.64 IPD
	Matrix4 matrixObj(
		0.984808, 0.000000, 0.173648, -0.033236,
		0.000000, 1.000000, 0.000000, 0.000000,
		-0.173648, 0.000000, 0.984808, 0.000000,
		0, 0, 0, 1);
	matrixObj.transpose();
	*/

	// Invert the matrix and store it
	matrixObj.invertGeneral();
	if (eye == vr::EVREye::Eye_Left)
		g_EyeMatrixLeftInv = matrixObj;
	else
		g_EyeMatrixRightInv = matrixObj;
}

void ShowVector4(const Vector4 &X, char *name) {
	if (name != NULL)
		log_debug("[DBG] %s = %0.6f, %0.6f, %0.6f, %0.6f",
			name, X[0], X[1], X[2], X[3]);
	else
		log_debug("[DBG] %0.6f, %0.6f, %0.6f, %0.6f",
			X[0], X[1], X[2], X[3]);
}

void TestProjMatrices() {
	Vector4 X;
	float x, y, z;

	X.set(0, 0, 1, 1);
	ShowVector4(X, "0,0,1, left = ");
	projectSteamVR(X[0], X[1], X[2], vr::EVREye::Eye_Left, x, y, z);
	log_debug("[DBG] (%0.3f, %0.3f, %0.3f)", x, y, z);

	X.set(-1, -1, 1, 1);
	ShowVector4(X, "-1, -1, 1, left = ");
	projectSteamVR(X[0], X[1], X[2], vr::EVREye::Eye_Left, x, y, z);
	log_debug("[DBG] (%0.3f, %0.3f, %0.3f)", x, y, z);

	X.set(1, 1, 1, 1);
	ShowVector4(X, "1, 1, 1, left = ");
	projectSteamVR(X[0], X[1], X[2], vr::EVREye::Eye_Left, x, y, z);
	log_debug("[DBG] (%0.3f, %0.3f, %0.3f)", x, y, z);

}

bool InitSteamVR()
{
	char *strDriver = NULL;
	char *strDisplay = NULL;
	FILE *file = NULL;
	bool result = true;

	int file_error = fopen_s(&file, "./steamvr_mat.txt", "wt");
	log_debug("[DBG] Initializing SteamVR");
	vr::EVRInitError eError = vr::VRInitError_None;
	g_pHMD = vr::VR_Init(&eError, vr::VRApplication_Scene);

	if (eError != vr::VRInitError_None)
	{
		g_pHMD = NULL;
		log_debug("[DBG] Unable to init VR runtime: %s", vr::VR_GetVRInitErrorAsEnglishDescription(eError));
		result = false;
		goto out;
	}
	log_debug("[DBG] SteamVR Initialized");
	g_pHMD->GetRecommendedRenderTargetSize(&g_steamVRWidth, &g_steamVRHeight);
	log_debug("[DBG] Recommended steamVR width, height: %d, %d", g_steamVRWidth, g_steamVRHeight);

	strDriver = GetTrackedDeviceString(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_TrackingSystemName_String);
	if (strDriver) {
		log_debug("[DBG] Driver: %s", strDriver);
		strDisplay = GetTrackedDeviceString(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_SerialNumber_String);
	}

	if (strDisplay)
		log_debug("[DBG] Display: %s", strDisplay);

	g_pVRCompositor = vr::VRCompositor();
	if (g_pVRCompositor == NULL)
	{
		log_debug("[DBG] SteamVR Compositor Initialization failed.");
		result = false;
		goto out;
	}
	log_debug("[DBG] SteamVR Compositor Initialized");

	g_pVRCompositor->ForceInterleavedReprojectionOn(g_bInterleavedReprojection);
	log_debug("[DBG] InterleavedReprojection: %d", g_bInterleavedReprojection);

	g_pVRScreenshots = vr::VRScreenshots();
	if (g_pVRScreenshots != NULL) {
		log_debug("[DBG] SteamVR screenshot system enabled");
	}

	// Reset the seated pose
	g_pHMD->ResetSeatedZeroPose();

	// Pre-multiply and store the eye and projection matrices:
	ProcessSteamVREyeMatrices(vr::EVREye::Eye_Left);
	ProcessSteamVREyeMatrices(vr::EVREye::Eye_Right);

	// Should I use Z_FAR here?
	vr::HmdMatrix44_t projLeft  = g_pHMD->GetProjectionMatrix(vr::EVREye::Eye_Left, 0.001f, 25000.0f);
	vr::HmdMatrix44_t projRight = g_pHMD->GetProjectionMatrix(vr::EVREye::Eye_Right, 0.001f, 25000.0f);

	//vr::HmdMatrix44_t projLeft  = g_pHMD->GetProjectionMatrix(vr::EVREye::Eye_Left, DEFAULT_FOCAL_DIST, 50.0f);
	//vr::HmdMatrix44_t projRight = g_pHMD->GetProjectionMatrix(vr::EVREye::Eye_Right, DEFAULT_FOCAL_DIST, 50.0f);

	//vr::HmdMatrix44_t projLeft = g_pHMD->GetProjectionMatrix(vr::EVREye::Eye_Left, DEFAULT_FOCAL_DIST, Z_FAR);
	//vr::HmdMatrix44_t projRight = g_pHMD->GetProjectionMatrix(vr::EVREye::Eye_Right, DEFAULT_FOCAL_DIST, Z_FAR);

	g_projLeft  = HmdMatrix44toMatrix4(projLeft);
	g_projRight = HmdMatrix44toMatrix4(projRight);

	// The order is wrong; but I'm not sure if I also should transpose, so multiply each
	// matrix separately for now
	g_fullMatrixLeft  = g_projLeft  * g_EyeMatrixLeftInv;
	g_fullMatrixRight = g_projRight * g_EyeMatrixRightInv;

	//ShowHmdMatrix44(projLeft, "progLeft Z_FAR: 1");
	//ShowMatrix4(g_projLeft, "g_progLeft Z_FAR: 1");
	ShowMatrix4(g_EyeMatrixLeftInv, "g_EyeMatrixLeftInv");
	ShowMatrix4(g_projLeft, "g_projLeft");

	ShowMatrix4(g_EyeMatrixRightInv, "g_EyeMatrixRightInv");
	ShowMatrix4(g_projRight, "g_projRight");
	
	//ShowMatrix4(g_fullMatrixLeft, "g_fullMatrixLeft");
	//ShowMatrix4(g_fullMatrixRight, "g_fullMatrixRight");

	TestProjMatrices();

	// Dump information about the view matrices
	if (file_error == 0) {
		if (strDriver != NULL)
			fprintf(file, "Driver: %s\n", strDriver);
		if (strDisplay != NULL)
			fprintf(file, "Display: %s\n", strDisplay);
		fprintf(file, "\n");

		fprintf(file, "eyeLeft:\n");
		DumpMatrix34(file, g_EyeMatrixLeft);
		fprintf(file, "\n");

		fprintf(file, "eyeRight:\n");
		DumpMatrix34(file, g_EyeMatrixLeft);
		fprintf(file, "\n");

		// Z_FAR was 50 for version 0.9.6, and I believe Z_Near was 0.5 (focal dist)
		vr::HmdMatrix44_t projLeft = g_pHMD->GetProjectionMatrix(vr::EVREye::Eye_Left, DEFAULT_FOCAL_DIST, 50.0f);
		vr::HmdMatrix44_t projRight = g_pHMD->GetProjectionMatrix(vr::EVREye::Eye_Right, DEFAULT_FOCAL_DIST, 50.0f);

		fprintf(file, "projLeft:\n");
		DumpMatrix44(file, projLeft);
		fprintf(file, "\n");

		fprintf(file, "projRight:\n");
		DumpMatrix44(file, projRight);
		fprintf(file, "\n");

		float left, right, top, bottom;
		g_pHMD->GetProjectionRaw(vr::EVREye::Eye_Left, &left, &right, &top, &bottom);
		fprintf(file, "Raw data (Left eye):\n");
		fprintf(file, "Left: %0.6f, Right: %0.6f, Top: %0.6f, Bottom: %0.6f\n",
			left, right, top, bottom);

		g_pHMD->GetProjectionRaw(vr::EVREye::Eye_Right, &left, &right, &top, &bottom);
		fprintf(file, "Raw data (Right eye):\n");
		fprintf(file, "Left: %0.6f, Right: %0.6f, Top: %0.6f, Bottom: %0.6f\n",
			left, right, top, bottom);
		fclose(file);
	}

out:
	if (strDriver != NULL)
		delete[] strDriver;
	if (strDisplay != NULL)
		delete[] strDisplay;
	if (file != NULL)
		fclose(file);
	return result;
}

void ShutDownSteamVR() {
	// We can't shut down SteamVR twice, we either shut it down here or in the cockpit look hook.
	// It looks like the right order is to shut SteamVR down in the cockpit look hook
	return;
	//log_debug("[DBG] Not shutting down SteamVR, just returning...");
	//return;
	log_debug("[DBG] Shutting down SteamVR...");
	vr::VR_Shutdown();
	g_pHMD = NULL;
	g_pVRCompositor = NULL;
	g_pVRScreenshots = NULL;
	log_debug("[DBG] SteamVR shut down");
}

/*
void ProcessVREvent(const vr::VREvent_t & event)
{
	switch (event.eventType)
	{
	case vr::VREvent_TrackedDeviceDeactivated:
	{
		log_debug("[DBG] Device %u detached.\n", event.trackedDeviceIndex);
	}
	break;
	case vr::VREvent_TrackedDeviceUpdated:
	{
		log_debug("[DBG] Device %u updated.\n", event.trackedDeviceIndex);
	}
	break;
	}
}
*/

/*
bool HandleVRInput()
{
	bool bRet = false;

	// Process SteamVR events
	vr::VREvent_t event;
	while (g_pHMD->PollNextEvent(&event, sizeof(event)))
	{
		ProcessVREvent(event);
	}

	return bRet;
}
*/

////////////////////////////////////////////////////////////////
// End of SteamVR functions
////////////////////////////////////////////////////////////////

class RenderStates
{
public:
	RenderStates(DeviceResources* deviceResources)
	{
		this->_deviceResources = deviceResources;

		this->TextureAddress = D3DTADDRESS_WRAP;

		this->AlphaBlendEnabled = FALSE;
		this->TextureMapBlend = D3DTBLEND_MODULATE;
		this->SrcBlend = D3DBLEND_ONE;
		this->DestBlend = D3DBLEND_ZERO;

		this->ZEnabled = TRUE;
		this->ZWriteEnabled = TRUE;
		this->ZFunc = D3DCMP_GREATER;
	}
	
	static D3D11_TEXTURE_ADDRESS_MODE TextureAddressMode(D3DTEXTUREADDRESS address)
	{
		switch (address)
		{
		case D3DTADDRESS_WRAP:
			return D3D11_TEXTURE_ADDRESS_WRAP;
		case D3DTADDRESS_MIRROR:
			return D3D11_TEXTURE_ADDRESS_MIRROR;
		case D3DTADDRESS_CLAMP:
			return D3D11_TEXTURE_ADDRESS_CLAMP;
		}

		return D3D11_TEXTURE_ADDRESS_WRAP;
	}

	static D3D11_BLEND Blend(D3DBLEND blend)
	{
		switch (blend)
		{
		case D3DBLEND_ZERO:
			return D3D11_BLEND_ZERO;
		case D3DBLEND_ONE:
			return D3D11_BLEND_ONE;
		case D3DBLEND_SRCCOLOR:
			return D3D11_BLEND_SRC_COLOR;
		case D3DBLEND_INVSRCCOLOR:
			return D3D11_BLEND_INV_SRC_COLOR;
		case D3DBLEND_SRCALPHA:
			return D3D11_BLEND_SRC_ALPHA;
		case D3DBLEND_INVSRCALPHA:
			return D3D11_BLEND_INV_SRC_ALPHA;
		case D3DBLEND_DESTALPHA:
			return D3D11_BLEND_DEST_ALPHA;
		case D3DBLEND_INVDESTALPHA:
			return D3D11_BLEND_INV_DEST_ALPHA;
		case D3DBLEND_DESTCOLOR:
			return D3D11_BLEND_DEST_COLOR;
		case D3DBLEND_INVDESTCOLOR:
			return D3D11_BLEND_INV_DEST_COLOR;
		case D3DBLEND_SRCALPHASAT:
			return D3D11_BLEND_SRC_ALPHA_SAT;
		case D3DBLEND_BOTHSRCALPHA:
			return D3D11_BLEND_SRC1_ALPHA;
		case D3DBLEND_BOTHINVSRCALPHA:
			return D3D11_BLEND_INV_SRC1_ALPHA;
		}

		return D3D11_BLEND_ZERO;
	}

	static D3D11_COMPARISON_FUNC ComparisonFunc(D3DCMPFUNC func)
	{
		switch (func)
		{
		case D3DCMP_NEVER:
			return D3D11_COMPARISON_NEVER;
		case D3DCMP_LESS:
			return D3D11_COMPARISON_LESS; // Original setting
			//return D3D11_COMPARISON_GREATER;
		case D3DCMP_EQUAL:
			return D3D11_COMPARISON_EQUAL;
		case D3DCMP_LESSEQUAL:
			return D3D11_COMPARISON_LESS_EQUAL;
		case D3DCMP_GREATER:
			return D3D11_COMPARISON_GREATER; // Original setting
			//return D3D11_COMPARISON_LESS;
		case D3DCMP_NOTEQUAL:
			return D3D11_COMPARISON_NOT_EQUAL;
		case D3DCMP_GREATEREQUAL:
			return D3D11_COMPARISON_GREATER_EQUAL;
		case D3DCMP_ALWAYS:
			return D3D11_COMPARISON_ALWAYS;
		}

		return D3D11_COMPARISON_ALWAYS;
	}

	D3D11_SAMPLER_DESC GetSamplerDesc()
	{
		D3D11_SAMPLER_DESC desc;
		desc.Filter = this->_deviceResources->_useAnisotropy ? D3D11_FILTER_ANISOTROPIC : D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.MaxAnisotropy = this->_deviceResources->_useAnisotropy ? this->_deviceResources->GetMaxAnisotropy() : 1;
		desc.AddressU = TextureAddressMode(this->TextureAddress);
		desc.AddressV = TextureAddressMode(this->TextureAddress);
		desc.AddressW = TextureAddressMode(this->TextureAddress);
		desc.MipLODBias = 0.0f;
		desc.MinLOD = 0;
		desc.MaxLOD = FLT_MAX;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.BorderColor[0] = 0.0f;
		desc.BorderColor[1] = 0.0f;
		desc.BorderColor[2] = 0.0f;
		desc.BorderColor[3] = 0.0f;

		return desc;
	}

	D3D11_BLEND_DESC GetBlendDesc()
	{
		D3D11_BLEND_DESC desc{};

		desc.AlphaToCoverageEnable = FALSE;
		desc.IndependentBlendEnable = FALSE;
		desc.RenderTarget[0].BlendEnable = this->AlphaBlendEnabled;
		desc.RenderTarget[0].SrcBlend = Blend(this->SrcBlend);
		desc.RenderTarget[0].DestBlend = Blend(this->DestBlend);
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

		desc.RenderTarget[0].SrcBlendAlpha = this->TextureMapBlend == D3DTBLEND_MODULATEALPHA ? D3D11_BLEND_SRC_ALPHA : D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = this->TextureMapBlend == D3DTBLEND_MODULATEALPHA ? D3D11_BLEND_INV_SRC_ALPHA : D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		return desc;
	}

	D3D11_DEPTH_STENCIL_DESC GetDepthStencilDesc()
	{
		D3D11_DEPTH_STENCIL_DESC desc{};

		desc.DepthEnable = this->ZEnabled;
		desc.DepthWriteMask = this->ZWriteEnabled ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc = ComparisonFunc(this->ZFunc);
		desc.StencilEnable = FALSE;

		return desc;
	}

	inline void SetTextureAddress(D3DTEXTUREADDRESS textureAddress)
	{
		this->TextureAddress = textureAddress;
	}

	inline void SetAlphaBlendEnabled(BOOL alphaBlendEnabled)
	{
		this->AlphaBlendEnabled = alphaBlendEnabled;
	}

	inline void SetTextureMapBlend(D3DTEXTUREBLEND textureMapBlend)
	{
		this->TextureMapBlend = textureMapBlend;
	}

	inline void SetSrcBlend(D3DBLEND srcBlend)
	{
		this->SrcBlend = srcBlend;
	}

	inline void SetDestBlend(D3DBLEND destBlend)
	{
		this->DestBlend = destBlend;
	}

	inline void SetZEnabled(BOOL zEnabled)
	{
		this->ZEnabled = zEnabled;
	}

	inline bool GetZEnabled()
	{
		return this->ZEnabled;
	}

	inline void SetZWriteEnabled(BOOL zWriteEnabled)
	{
		this->ZWriteEnabled = zWriteEnabled;
	}

	inline bool GetZWriteEnabled()
	{
		return this->ZWriteEnabled;
	}

	inline void SetZFunc(D3DCMPFUNC zFunc)
	{
		this->ZFunc = zFunc;
	}

	inline D3DCMPFUNC GetZFunc() {
		return this->ZFunc;
	}

private:
	DeviceResources* _deviceResources;

	D3DTEXTUREADDRESS TextureAddress;

	BOOL AlphaBlendEnabled;
	D3DTEXTUREBLEND TextureMapBlend;
	D3DBLEND SrcBlend;
	D3DBLEND DestBlend;

	BOOL ZEnabled;
	BOOL ZWriteEnabled;
	D3DCMPFUNC ZFunc;
};

Direct3DDevice::Direct3DDevice(DeviceResources* deviceResources)
{
	this->_refCount = 1;
	this->_deviceResources = deviceResources;

	this->_renderStates = new RenderStates(this->_deviceResources);

	this->_maxExecuteBufferSize = 0;
}

Direct3DDevice::~Direct3DDevice()
{
	DeleteStereoVertices();
	g_iNumVertices = 0;
	delete this->_renderStates;
}

HRESULT Direct3DDevice::QueryInterface(
	REFIID riid,
	LPVOID* obp
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tE_NOINTERFACE");
	LogText(str.str());
#endif

	return E_NOINTERFACE;
}

ULONG Direct3DDevice::AddRef()
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

	this->_refCount++;

#if LOGGER
	str.str("");
	str << "\t" << this->_refCount;
	LogText(str.str());
#endif

	return this->_refCount;
}

ULONG Direct3DDevice::Release()
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

	this->_refCount--;

#if LOGGER
	str.str("");
	str << "\t" << this->_refCount;
	LogText(str.str());
#endif

	if (this->_refCount == 0)
	{
		delete this;
		return 0;
	}

	return this->_refCount;
}

HRESULT Direct3DDevice::Initialize(
	LPDIRECT3D lpd3d,
	LPGUID lpGUID,
	LPD3DDEVICEDESC lpd3ddvdesc
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

HRESULT Direct3DDevice::GetCaps(
	LPD3DDEVICEDESC lpD3DHWDevDesc,
	LPD3DDEVICEDESC lpD3DHELDevDesc
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

HRESULT Direct3DDevice::SwapTextureHandles(
	LPDIRECT3DTEXTURE lpD3DTex1,
	LPDIRECT3DTEXTURE lpD3DTex2
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

HRESULT Direct3DDevice::CreateExecuteBuffer(
	LPD3DEXECUTEBUFFERDESC lpDesc,
	LPDIRECT3DEXECUTEBUFFER *lplpDirect3DExecuteBuffer,
	IUnknown *pUnkOuter
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

	if (lpDesc == nullptr)
	{
#if LOGGER
		str.str("\tDDERR_INVALIDPARAMS");
		LogText(str.str());
#endif

		return DDERR_INVALIDPARAMS;
	}

	if ((lpDesc->dwFlags & D3DDEB_BUFSIZE) == 0)
	{
#if LOGGER
		str.str("\tDDERR_INVALIDPARAMS");
		LogText(str.str());
#endif

		return DDERR_INVALIDPARAMS;
	}

	if (lplpDirect3DExecuteBuffer == nullptr)
	{
#if LOGGER
		str.str("\tDDERR_INVALIDPARAMS");
		LogText(str.str());
#endif

		return DDERR_INVALIDPARAMS;
	}

	if (lpDesc->dwBufferSize > this->_maxExecuteBufferSize)
	{
		auto& device = this->_deviceResources->_d3dDevice;

		D3D11_BUFFER_DESC vertexBufferDesc;
		vertexBufferDesc.ByteWidth = lpDesc->dwBufferSize;
		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		if (FAILED(device->CreateBuffer(&vertexBufferDesc, nullptr, &this->_vertexBuffer)))
			return DDERR_INVALIDOBJECT;
		if (FAILED(device->CreateBuffer(&vertexBufferDesc, nullptr, &this->_vertexBufferL)))
			return DDERR_INVALIDOBJECT;
		if (FAILED(device->CreateBuffer(&vertexBufferDesc, nullptr, &this->_vertexBufferR)))
			return DDERR_INVALIDOBJECT;
		if (g_bUseSteamVR) {
			if (FAILED(device->CreateBuffer(&vertexBufferDesc, nullptr, &this->_vertexBuffer3D)))
				return DDERR_INVALIDOBJECT;
		}

		D3D11_BUFFER_DESC indexBufferDesc;
		indexBufferDesc.ByteWidth = lpDesc->dwBufferSize;
		indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;

		if (FAILED(device->CreateBuffer(&indexBufferDesc, nullptr, &this->_indexBuffer)))
			return DDERR_INVALIDOBJECT;

		this->_maxExecuteBufferSize = lpDesc->dwBufferSize;
	}

	*lplpDirect3DExecuteBuffer = new Direct3DExecuteBuffer(this->_deviceResources, lpDesc->dwBufferSize);

#if LOGGER
	str.str("");
	str << "\t" << *lplpDirect3DExecuteBuffer;
	LogText(str.str());
#endif

	return D3D_OK;
}

HRESULT Direct3DDevice::GetStats(
	LPD3DSTATS lpD3DStats
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

void DeleteStereoVertices() {
	if (g_OrigVerts != NULL)
		delete[] g_OrigVerts;
	if (g_LeftVerts != NULL)
		delete[] g_LeftVerts;
	if (g_RightVerts != NULL)
		delete[] g_RightVerts;
	if (g_3DVerts != NULL)
		delete[] g_3DVerts;
}

void ResizeStereoVertices(int numVerts) {
	// Only resize if we need more space, otherwise ignore.
	if (numVerts <= g_iNumVertices)
		return;

	g_iNumVertices = numVerts + 256; // Add more elements than we need, this prevents resizing too often.
	DeleteStereoVertices();

	g_OrigVerts = new D3DTLVERTEX[g_iNumVertices];
	g_LeftVerts = new D3DTLVERTEX[g_iNumVertices];
	g_RightVerts = new D3DTLVERTEX[g_iNumVertices];
	g_3DVerts = new D3DTLVERTEX[g_iNumVertices];
}

#ifdef DBG_VR
void DumpOrigVertices(FILE *file, int numVerts)
{
	char buf[256];
	float px, py, pz;

	// Don't catpure things we are not drawing
	if (g_iDrawCounter < g_iNoDrawBeforeIndex)
		return;
	if (g_iNoDrawAfterIndex > -1 && g_iDrawCounter > g_iNoDrawAfterIndex)
		return;

	for (register int i = 0; i < numVerts; i++) {
		px = g_orig_verts[i].sx;
		py = g_orig_verts[i].sy;
		pz = g_orig_verts[i].sz;

		sprintf_s(buf, 256, "v %f %f %f\n", px, py, pz);
		fprintf(file, buf);
	}
}
#endif

/* 
 * In SteamVR, the coordinate system is as follows:
 * +x is right
 * +y is up
 * -z is forward
 * Distance is meters
 */
void projectSteamVR(float X, float Y, float Z, vr::EVREye eye, float &x, float &y, float &z) {
	Vector4 PX; // PY;

	PX.set(X, -Y, -Z, 1.0f);
	if (eye == vr::EVREye::Eye_Left) {
		PX = g_fullMatrixLeft * PX;
	} else {
		PX = g_fullMatrixRight * PX;
	}
	// Project
	PX /= PX[3];
	// Convert to 2D
	x =  PX[0]; // / PX[2];
	y = -PX[1]; // / PX[2];
	z =  PX[2];
}

void PreprocessVerticesStereo(float width, float height, int numVerts)
{
	// Pre-process vertices for Stereo
	float /* X, Y, Z, */ px, py, pz, /* qx, qy, qz, */ direct_pz;
	bool is_cockpit;
	float scale_x = 1.0f / width;
	float scale_y = 1.0f / height;
	//float w;
	//float scale = scale_x;
	bool is_GUI = false;

	// Back-project and do stereo
	for (register int i = 0; i < numVerts; i++) {
		g_LeftVerts[i]  = g_OrigVerts[i];
		g_RightVerts[i] = g_OrigVerts[i];
		g_3DVerts[i]    = g_OrigVerts[i];
		// Normalize the coords: move the screen's center to (0,0) and scale the (x,y) axes
		// to -0.5..0.5
		px = g_OrigVerts[i].sx * scale_x - 0.5f;
		py = g_OrigVerts[i].sy * scale_y - 0.5f;
		// Also invert the Z axis so that z = 0 is the screen plane and z = 1 is ZFar, the original
		// values have ZFar = 0, and ZNear = 1
		direct_pz = g_OrigVerts[i].sz;
		pz = 1.0f - direct_pz;

		// GUI elements seem to be in the range 0..0.0005, so 0.0006 sounds like a good threshold:
		is_GUI = (pz <= g_fGUIElemPZThreshold);
		is_cockpit = (pz <= g_fCockpitPZThreshold);

		g_3DVerts[i].sx = px;
		g_3DVerts[i].sy = py;

		// Reproject back into 2D space
		if (is_GUI) {
			// We need to restore the original ZBuffer value for the GUI elements or
			// they will cause Z-Fighting with the 3D objects. Also the depth of the
			// GUI elements is fixed by directly setting their parallax. So, nothing
			// to do here.
			/*
			qx = px;
			qy = py;
			qz = pz;
			*/
			//g_3DVerts[i].sz = g_fFocalDist;
			//g_3DVerts[i].sz = 0.0008f;
			g_3DVerts[i].sz = g_fFloatingGUIParallax;
		} 
		/*
		else { //if (is_cockpit) {
			if (g_bUseSteamVR) {
				projectSteamVR(X, Y, Z, vr::EVREye::Eye_Left, px, py, pz);
				projectSteamVR(X, Y, Z, vr::EVREye::Eye_Right, qx, qy, qz);
			} else {
				project(X + g_fHalfIPD, Y, Z, px, py, pz);
				project(X - g_fHalfIPD, Y, Z, qx, qy, qz);
			}			
		} /* else {
			float disp = 10.0f; // Objects "out there" need to have their parallax boosted or they just look flat
			project(X + g_fHalfIPD * disp, Y, Z, px, py, pz);
			project(X - g_fHalfIPD * disp, Y, Z, qx, qy, qz);
		} */

		/*
		// (px,py) and (qx,qy) are now in the range [-0.5,..,0.5], we need
		// to map them to the range [0..width, 0..height]

		// Compute the vertices for the left image
		{
			// De-normalize coords (left image)
			g_LeftVerts[i].sx = (px + 0.5f) / scale_x;
			g_LeftVerts[i].sy = (py + 0.5f) / scale_y;
			g_LeftVerts[i].sz = 1.0f - pz;
		}
		// Compute the vertices for the right image
		{
			// De-normalize coords (right image)
			g_RightVerts[i].sx = (qx + 0.5f) / scale_x;
			g_RightVerts[i].sy = (qy + 0.5f) / scale_y;
			g_RightVerts[i].sz = 1.0f - qz;
		}
		// Restore the original Z for the GUI elements: this will avoid Z-fighting
		// (I think this also helps with Z-fighting in general)
		//if (is_GUI) {
			g_LeftVerts[i].sz  = g_OrigVerts[i].sz;
			g_RightVerts[i].sz = g_OrigVerts[i].sz;
		//}

		*/
	}

#ifdef DBG_VR
	// DBG: Hack: Dump the 3D scene. Triggered with Ctrl-Alt-C
	if (g_bDo3DCapture)
	{
		if (g_hack_file == NULL)
			fopen_s(&g_hack_file, "./vertexbuf.obj", "wt");
		DumpOrigVertices(g_hack_file, numVerts);
	}
#endif
}

/* Function to quickly enable/disable ZWrite. Currently only used for brackets */
HRESULT Direct3DDevice::QuickSetZWriteEnabled(BOOL Enabled) {
	HRESULT hr;
	D3D11_DEPTH_STENCIL_DESC desc = this->_renderStates->GetDepthStencilDesc();
	ComPtr<ID3D11DepthStencilState> depthState;
	auto &resources = this->_deviceResources;

	desc.DepthEnable = Enabled;
	desc.DepthWriteMask = Enabled ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	desc.DepthFunc = Enabled ? D3D11_COMPARISON_GREATER : D3D11_COMPARISON_ALWAYS;
	desc.StencilEnable = FALSE;
	hr = resources->_d3dDevice->CreateDepthStencilState(&desc, &depthState);
	if (SUCCEEDED(hr))
		resources->_d3dDeviceContext->OMSetDepthStencilState(depthState, 0);
	return hr;
}

HRESULT Direct3DDevice::Execute(
	LPDIRECT3DEXECUTEBUFFER lpDirect3DExecuteBuffer,
	LPDIRECT3DVIEWPORT lpDirect3DViewport,
	DWORD dwFlags
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

	if (lpDirect3DExecuteBuffer == nullptr)
	{
#if LOGGER
		str.str("\tDDERR_INVALIDPARAMS");
		LogText(str.str());
#endif

		return DDERR_INVALIDPARAMS;
	}

	Direct3DExecuteBuffer* executeBuffer = (Direct3DExecuteBuffer*)lpDirect3DExecuteBuffer;

#if LOGGER
	DumpExecuteBuffer(executeBuffer);
#endif

	auto& resources = this->_deviceResources;
	auto& device = resources->_d3dDevice;
	auto& context = resources->_d3dDeviceContext;

	HRESULT hr = S_OK;
	UINT width, height, left, top;
	UINT vertexBufferStride = sizeof(D3DTLVERTEX), vertexBufferOffset = 0;
	D3D11_VIEWPORT viewport;
	D3D11_MAPPED_SUBRESOURCE map;
	bool bModifiedShaders = false, bZWriteEnabled = false;

	g_VSCBuffer = { 0 };
	g_VSCBuffer.aspect_ratio = g_fAspectRatio;
	g_VSCBuffer.z_override = -1.0f;
	g_PSCBuffer.brightness = MAX_BRIGHTNESS;

	char* step = "";

	this->_deviceResources->InitInputLayout(resources->_inputLayout);
	if (g_bEnableVR)
		this->_deviceResources->InitVertexShader(resources->_sbsVertexShader);
	else
		// The original code used _vertexShader:
		this->_deviceResources->InitVertexShader(resources->_vertexShader);	
	this->_deviceResources->InitPixelShader(resources->_pixelShaderTexture);
	this->_deviceResources->InitTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->_deviceResources->InitRasterizerState(resources->_rasterizerState);

	// Do stereo preprocessing
	int numVerts = executeBuffer->_executeData.dwVertexCount;
	size_t vertsLength = sizeof(D3DTLVERTEX) * numVerts;
	// Resize the aux buffers that store the left and right vertices
	ResizeStereoVertices(numVerts);
	// Save the original vertices
	memcpy(g_OrigVerts, executeBuffer->_buffer, sizeof(D3DTLVERTEX) * numVerts);
	// Generate vertex positions for the left and right images
	float displayWidth = (float)resources->_displayWidth;
	float displayHeight = (float)resources->_displayHeight;
	if (g_bEnableVR) PreprocessVerticesStereo(displayWidth, displayHeight, numVerts);

	// Copy the vertex data to the vertexbuffers
	// Original data, no parallax
	step = "VertexBuffer (Monoscopic)";
	hr = context->Map(this->_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
	if (SUCCEEDED(hr))
	{
		memcpy(map.pData, g_OrigVerts, vertsLength);
		context->Unmap(this->_vertexBuffer, 0);
	}

	// Copy the vertex data to the left and right VertexBuffers
	if (g_bEnableVR) {
		// Left vertices
		step = "VertexBuffer (Left)";
		hr = context->Map(this->_vertexBufferL, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
		if (SUCCEEDED(hr))
		{
			memcpy(map.pData, g_LeftVerts, vertsLength);
			context->Unmap(this->_vertexBufferL, 0);
		}

		// Right vertices
		step = "VertexBuffer (Right)";
		hr = context->Map(this->_vertexBufferR, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
		if (SUCCEEDED(hr))
		{
			memcpy(map.pData, g_RightVerts, vertsLength);
			context->Unmap(this->_vertexBufferR, 0);
		}

		// 3D vertices
		step = "VertexBuffer (3D)";
		hr = context->Map(this->_vertexBuffer3D, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
		if (SUCCEEDED(hr))
		{
			memcpy(map.pData, g_3DVerts, vertsLength);
			context->Unmap(this->_vertexBuffer3D, 0);
		}

		// Send the 3D vertices, let the shader do the projection
		resources->InitVertexBuffer(this->_vertexBuffer3D.GetAddressOf(), &vertexBufferStride, &vertexBufferOffset);
	}

	// Constant Buffer step (and aspect ratio)
	if (SUCCEEDED(hr))
	{
		step = "ConstantBuffer";

		if (g_config.AspectRatioPreserved)
		{
			if (resources->_backbufferHeight * resources->_displayWidth <= this->_deviceResources->_backbufferWidth * this->_deviceResources->_displayHeight)
			{
				width = this->_deviceResources->_backbufferHeight * this->_deviceResources->_displayWidth / this->_deviceResources->_displayHeight;
				height = this->_deviceResources->_backbufferHeight;
			}
			else
			{
				width = this->_deviceResources->_backbufferWidth;
				height = this->_deviceResources->_backbufferWidth * this->_deviceResources->_displayHeight / this->_deviceResources->_displayWidth;
			}
			// In this version, we're stretching the viewports to cover the full backbuffer. If no aspect correction is done in the
			// block above, then the SBS aspect_ratio is simply 2 because we're halving the screen.
			// However, if some aspect ratio was performed above, we need to compensate for that. The SBS viewports will already
			// stretch "width/2" to "backBufferWidth/2", (e.g. with displayWidth,Height set to 1280x1025 in the game and an actual
			// screen res of 3240x2160, the code will take the first case and set width to 2700, which will be stretched to 3240)
			// Thus, we need to revert that stretching and then multiply by 2 (because we're halving the screen size)
			if (!g_bOverrideAspectRatio) { // Only compute the aspect ratio if we didn't read it off of the config file
				g_fAspectRatio = 2.0f * width / this->_deviceResources->_backbufferWidth;
			}
		}
		else
		{
			width = resources->_backbufferWidth;
			height = resources->_backbufferHeight;

			if (!g_bOverrideAspectRatio) {
				float original_aspect = (float)this->_deviceResources->_displayWidth / (float)this->_deviceResources->_displayHeight;
				float actual_aspect = (float)this->_deviceResources->_backbufferWidth / (float)this->_deviceResources->_backbufferHeight;
				g_fAspectRatio = actual_aspect / original_aspect;
			}
		}

		left = (this->_deviceResources->_backbufferWidth - width) / 2;
		top = (this->_deviceResources->_backbufferHeight - height) / 2;

		float scale;

		if (this->_deviceResources->_frontbufferSurface == nullptr)
		{
			scale = 1.0f;
		}
		else
		{
			if (this->_deviceResources->_backbufferHeight * this->_deviceResources->_displayWidth <= this->_deviceResources->_backbufferWidth * this->_deviceResources->_displayHeight)
			{
				scale = (float)this->_deviceResources->_backbufferHeight / (float)this->_deviceResources->_displayHeight;
			}
			else
			{
				scale = (float)this->_deviceResources->_backbufferWidth / (float)this->_deviceResources->_displayWidth;
			}

			scale *= g_config.Concourse3DScale;
		}

		g_VSCBuffer.viewportScale[0] =  2.0f / (float)resources->_displayWidth;
		g_VSCBuffer.viewportScale[1] = -2.0f / (float)resources->_displayHeight;
		g_VSCBuffer.viewportScale[2] = scale;
		g_VSCBuffer.viewportScale[3] = g_fGlobalScale;
		g_VSCBuffer.aspect_ratio = g_fAspectRatio;
		g_VSCBuffer.parallax = 0.0f;
		g_VSCBuffer.z_override = -1.0f;
		resources->InitVSConstantBuffer3D(resources->_VSConstantBuffer.GetAddressOf(), &g_VSCBuffer);
		resources->InitPSConstantBufferBrightness(resources->_PSConstantBuffer.GetAddressOf(), &g_PSCBuffer);

		// This is the original monoscopic viewport:
		/*
		viewport.TopLeftX = (float)left;
		viewport.TopLeftY = (float)top;
		viewport.Width = (float)width;
		viewport.Height = (float)height;
		viewport.MinDepth = D3D11_MIN_DEPTH;
		viewport.MaxDepth = D3D11_MAX_DEPTH;
		resources->InitViewport(&viewport);
		*/
	}

	// IndexBuffer Step
	if (SUCCEEDED(hr))
	{
		step = "IndexBuffer";

		D3D11_MAPPED_SUBRESOURCE map;
		hr = context->Map(this->_indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);

		if (SUCCEEDED(hr))
		{
			char* pData = executeBuffer->_buffer + executeBuffer->_executeData.dwInstructionOffset;
			char* pDataEnd = pData + executeBuffer->_executeData.dwInstructionLength;

			WORD* index = (WORD*)map.pData;

			while (pData < pDataEnd)
			{
				LPD3DINSTRUCTION instruction = (LPD3DINSTRUCTION)pData;
				pData += sizeof(D3DINSTRUCTION) + instruction->bSize * instruction->wCount;

				switch (instruction->bOpcode)
				{
				case D3DOP_TRIANGLE:
				{
					LPD3DTRIANGLE triangle = (LPD3DTRIANGLE)(instruction + 1);

					for (WORD i = 0; i < instruction->wCount; i++)
					{
						*index = triangle->v1;
						index++;

						*index = triangle->v2;
						index++;

						*index = triangle->v3;
						index++;

						triangle++;
					}
				}
				}
			}

			context->Unmap(this->_indexBuffer, 0);
		}

		if (SUCCEEDED(hr))
		{
			this->_deviceResources->InitIndexBuffer(this->_indexBuffer);
		}
	}

	// Render images
	if (SUCCEEDED(hr))
	{
		char* pData = executeBuffer->_buffer + executeBuffer->_executeData.dwInstructionOffset;
		char* pDataEnd = pData + executeBuffer->_executeData.dwInstructionLength;
		// lastTextureSelected is used to predict what is going to be rendered next
		Direct3DTexture* lastTextureSelected = NULL;

		UINT currentIndexLocation = 0;

		while (pData < pDataEnd)
		{
			LPD3DINSTRUCTION instruction = (LPD3DINSTRUCTION)pData;
			pData += sizeof(D3DINSTRUCTION) + instruction->bSize * instruction->wCount;

			switch (instruction->bOpcode)
			{
				// lastTextureSelected is updated here, in the TEXTUREHANDLE subcase
			case D3DOP_STATERENDER:
			{
				LPD3DSTATE state = (LPD3DSTATE)(instruction + 1);

				for (WORD i = 0; i < instruction->wCount; i++)
				{
					switch (state->drstRenderStateType)
					{
					case D3DRENDERSTATE_TEXTUREHANDLE:
					{
						Direct3DTexture* texture = g_config.WireframeFillMode ? nullptr : (Direct3DTexture*)state->dwArg[0];
						ID3D11PixelShader* pixelShader;

						if (texture == nullptr)
						{
							ID3D11ShaderResourceView* view = nullptr;
							context->PSSetShaderResources(0, 1, &view);

							pixelShader = resources->_pixelShaderSolid;
							// Nullify the last texture selected
							lastTextureSelected = NULL;
						}
						else
						{
							context->PSSetShaderResources(0, 1, texture->_textureView.GetAddressOf());

							pixelShader = resources->_pixelShaderTexture;
							// Keep the last texture selected
							lastTextureSelected = texture;
						}

						resources->InitPixelShader(pixelShader);
						break;
					}

					case D3DRENDERSTATE_TEXTUREADDRESS:
						this->_renderStates->SetTextureAddress((D3DTEXTUREADDRESS)state->dwArg[0]);
						break;

					case D3DRENDERSTATE_ALPHABLENDENABLE:
						this->_renderStates->SetAlphaBlendEnabled(state->dwArg[0]);
						break;
					case D3DRENDERSTATE_TEXTUREMAPBLEND:
						this->_renderStates->SetTextureMapBlend((D3DTEXTUREBLEND)state->dwArg[0]);
						break;
					case D3DRENDERSTATE_SRCBLEND:
						this->_renderStates->SetSrcBlend((D3DBLEND)state->dwArg[0]);
						break;
					case D3DRENDERSTATE_DESTBLEND:
						this->_renderStates->SetDestBlend((D3DBLEND)state->dwArg[0]);
						break;

					case D3DRENDERSTATE_ZENABLE:
						this->_renderStates->SetZEnabled(state->dwArg[0]);
						break;
					case D3DRENDERSTATE_ZWRITEENABLE:
						this->_renderStates->SetZWriteEnabled(state->dwArg[0]);
						break;
					case D3DRENDERSTATE_ZFUNC:
						this->_renderStates->SetZFunc((D3DCMPFUNC)state->dwArg[0]);
						break;
					}

					state++;
				}

				break;
			}

			case D3DOP_TRIANGLE:
			{
				step = "SamplerState";
				D3D11_SAMPLER_DESC samplerDesc = this->_renderStates->GetSamplerDesc();
				if (FAILED(hr = this->_deviceResources->InitSamplerState(nullptr, &samplerDesc)))
					break;

				step = "BlendState";
				D3D11_BLEND_DESC blendDesc = this->_renderStates->GetBlendDesc();
				if (FAILED(hr = this->_deviceResources->InitBlendState(nullptr, &blendDesc)))
					break;

				step = "DepthStencilState";
				D3D11_DEPTH_STENCIL_DESC depthDesc = this->_renderStates->GetDepthStencilDesc();
				if (FAILED(hr = this->_deviceResources->InitDepthStencilState(nullptr, &depthDesc)))
					break;

				step = "Draw";

				/*********************************************************************
					 State management begins here
				 *********************************************************************

				   Unfortunately, we need to maintain a number of flags to tell what the
				   engine is about to render. If we had access to the engine, we could do
				   away with all these flags.

				   The sequence of events goes like this:
					  - Draw the Skybox with ZWrite disabled, this is done in the first few draw calls.
					  - Draw 3D objects, including the cockpit.
					  - Disable ZWrite to draw engine glow and brackets.
					  - Disable ZWrite to draw the GUI elements.
					  - Enable ZWrite to draw the targeting computer HUD.
					  - Enable ZWrite to draw the targeted 3D object.

					NOTE: "Floating GUI elements" is what I call the GUI elements drawn along the bottom half of the
						  screen: the left and right panels, the main targeting computer and the laser charge state.
						  I call these "Floating GUI elements" because they are expected to "float" before the cockpit
						  or they will just cause visual contention. The rest of the GUI (radars, HUD, etc) is not in
						  this category -- I just call them GUI.
						  Floating GUI elems have their own list of CRCs independent of the GUI CRCs

					At the beginning of every frame:
					  - g_bTargetCompDrawn will be set to false
					  - g_iFloating_GUI_drawn_counter is set to 0

					We're using a non-exhaustive list of GUI CRCs to tell when the 3D content has finished drawing.
				*/

				/* ZWriteEnabled is false when rendering the background starfield or when
				 * rendering the GUI elements -- except that the targetting computer GUI
				 * is rendered with ZWriteEnabled == true. I wonder why? */
				bZWriteEnabled = this->_renderStates->GetZWriteEnabled();

				/* If we have drawn at least one Floating GUI element and now the ZWrite has been enabled
				   again, then we're about to draw the floating 3D element */
				if (!g_bTargetCompDrawn && g_iFloatingGUIDrawnCounter > 0 && bZWriteEnabled)
					g_bTargetCompDrawn = true;
				// bIsNoZWrite is true if ZWrite is disabled and the SkyBox has been rendered.
				bool bIsNoZWrite = !bZWriteEnabled && g_iExecBufCounter > g_iSkyBoxExecIndex;
				// bIsSkyBox is true if we're about to render the SkyBox
				bool bIsSkyBox = !bZWriteEnabled && g_iExecBufCounter <= g_iSkyBoxExecIndex;
				bool bIsTrianglePointer = lastTextureSelected != NULL && lastTextureSelected->is_TrianglePointer;
				bool bIsText = (lastTextureSelected != NULL) && lastTextureSelected->is_Text;
				bool bIsHUD = lastTextureSelected != NULL && lastTextureSelected->is_HUD;
				bool bIsGUI = lastTextureSelected != NULL && lastTextureSelected->is_GUI;
				// In the hangar, shadows are enabled. Shadows don't have a texture and are rendered with
				// ZWrite disabled. So, how can we tell if a bracket is being rendered or a shadow?
				// Brackets are rendered with ZFunc D3DCMP_ALWAYS (8),
				// Shadows are rendered with ZFunc D3DCMP_GREATEREQUAL (7)
				bool bIsBracket = bIsNoZWrite && lastTextureSelected == NULL && 
					this->_renderStates->GetZFunc() == D3DCMP_ALWAYS;
				bool bIsFloatingGUI = lastTextureSelected != NULL && lastTextureSelected->is_Floating_GUI;
				bool bIsFloating3DObject = g_bTargetCompDrawn && lastTextureSelected != NULL &&
					!lastTextureSelected->is_Text && !lastTextureSelected->is_TrianglePointer &&
					!lastTextureSelected->is_HUD && !lastTextureSelected->is_Floating_GUI;
				// The GUI starts rendering whenever we detect a GUI element, or Text, or a bracket.
				g_bStartedGUI |= bIsGUI || bIsText || bIsBracket;
				//g_bStartedGUI |= bIsGUI || bIsText; // bIsBracket is true for brackets *and* for (hangar) shadows
				// bIsScaleableGUIElem is true when we're about to render a HUD element that can be scaled down with Ctrl+Z
				bool bIsScaleableGUIElem = g_bStartedGUI && !bIsHUD && !bIsBracket && !bIsTrianglePointer;
				// lastTextureSelected can be NULL. This happens when drawing the square
				// brackets around the currently-selected object.
				/*************************************************************************
					State management ends here
				 *************************************************************************/

				 // Skip specific draw calls for debugging purposes.
#ifdef DBG_VR
				if (!bZWriteEnabled)
					g_iNonZBufferCounter++;
				if (!bZWriteEnabled && g_iSkipNonZBufferDrawIdx > -1 && g_iNonZBufferCounter >= g_iSkipNonZBufferDrawIdx)
					goto out;

				//if (bIsText && g_bSkipText)
				//	goto out;

				// Skip the draw calls after the targetting computer has been drawn?
				//if (bIsFloating3DObject && g_bSkipAfterTargetComp)
				//	goto out;

				if (bIsSkyBox && g_bSkipSkyBox)
					goto out;

				if (g_bStartedGUI && g_bSkipGUI)
					goto out;
				// Engine glow:
				//if (bIsNoZWrite && lastTextureSelected != NULL && g_bSkipGUI)
				//	goto out;

				/* if (bIsBracket) {
					log_debug("[DBG] ZEnabled: %d, ZFunc: %d", this->_renderStates->GetZEnabled(),
						this->_renderStates->GetZFunc());
				} */
#endif
				
				// Early exit: if we're not in VR mode, we only need the state; but not the extra
				// processing. (The state will be used later to do post-processing like Bloom and AO.
				if (!g_bEnableVR) {
					viewport.TopLeftX = (float)left;
					viewport.TopLeftY = (float)top;
					viewport.Width = (float)width;
					viewport.Height = (float)height;
					viewport.MinDepth = D3D11_MIN_DEPTH;
					viewport.MaxDepth = D3D11_MAX_DEPTH;
					resources->InitViewport(&viewport);

					// For non-VR mode, send the original 2D vertices
					//g_VSMatrixCB.projEye.identity();
					//resources->InitVSConstantBufferMatrix(resources->_VSMatrixBuffer.GetAddressOf(), &g_VSMatrixCB);
					resources->InitVertexBuffer(this->_vertexBuffer.GetAddressOf(), &vertexBufferStride, &vertexBufferOffset);

					context->OMSetRenderTargets(1, resources->_renderTargetView.GetAddressOf(),
						resources->_depthStencilViewL.Get());
					context->DrawIndexed(3 * instruction->wCount, currentIndexLocation, 0);
					goto no_vr_out;
				}

				/********************************************************************
				   Modify the state of the render for VR
				 ********************************************************************/

				 // Elements that are drawn with ZBuffer disabled:
				 // * All GUI HUD elements except for the targetting computer (why?)
				 // * Lens flares.
				 // * All the text and brackets around objects. The brackets have their own draw call.
				 // * Glasses on other cockpits and engine glow <-- Good candidate for bloom!
				 // * Maybe explosions and other animations? I think explosions are actually rendered at depth (?)
				 // * Cockpit sparks?

				 // We will be modifying the normal render state from this point on. The state and the Pixel/Vertex
				 // shaders are already set by this point; but if we modify them, we'll set bModifiedShaders to true
				 // so that we can restore the state at the end of the draw call.
				bModifiedShaders = false;

				// *****************************************************************************
				// Common state settings: things that apply to both the left and right images.
				// *****************************************************************************
				// The game renders brackets with ZWrite disabled; but we need to enable it temporarily so that we
				// can place the brackets at infinity and avoid visual contention
				if (bIsBracket) {
					bModifiedShaders = true;
					QuickSetZWriteEnabled(TRUE);
					g_VSCBuffer.z_override = g_fZOverride;
				}

				// Reduce the scale for GUI elements, except for the HUD
				if (bIsScaleableGUIElem) {
					bModifiedShaders = true;
					g_VSCBuffer.viewportScale[3] = g_fGUIElemsScale;
				}

				// Dim all the GUI elements
				if (g_bStartedGUI && !bIsFloating3DObject) {
					bModifiedShaders = true;
					g_PSCBuffer.brightness = g_fBrightness;
				}

				if (bIsSkyBox) {
					bModifiedShaders = true;
					// Make the skybox a bit bigger to enable roll in the future:
					g_VSCBuffer.viewportScale[3] = g_fGlobalScale /* + 0.2f */;
					// Send the skybox to infinity:
					g_VSCBuffer.z_override = 0.01f;
				}

				// Add an extra parallax to HUD elements
				if (bIsHUD) {
					bModifiedShaders = true;
					g_VSCBuffer.z_override = g_fHUDParallax;
				}

				if (bIsTrianglePointer) { // Now a common setting
					bModifiedShaders = true;
					g_VSCBuffer.viewportScale[3] = g_fGUIElemScale;
					g_VSCBuffer.parallax = g_fTextParallax;
				}

				// Add extra parallax to Floating GUI elements, left image
				if (bIsFloatingGUI || bIsFloating3DObject || bIsScaleableGUIElem) { // This is now a common setting
					bModifiedShaders = true;
					/*
					g_VSCBuffer.parallax = bIsFloating3DObject ?
						g_fHalfIPD * (g_fFloatingGUIParallax + g_fFloatingGUIObjParallax) :
						g_fHalfIPD * g_fFloatingGUIParallax;
					*/
					g_VSCBuffer.z_override = bIsFloating3DObject ?
						g_fFloatingGUIParallax + g_fFloatingGUIObjParallax :
						g_fFloatingGUIParallax;
				}

				if (bModifiedShaders)
					resources->InitPSConstantBufferBrightness(resources->_PSConstantBuffer.GetAddressOf(), &g_PSCBuffer);

				// ****************************************************************************
				// Left image state settings
				// ****************************************************************************

				// Let's render the triangle pointer closer to the center so that we can see it all the time,
				// also, render the triangle pointer at text depth.
				

				// Add an extra parallax to Text elements, left image
				if (bIsText) { // This is now a common setting
					bModifiedShaders = true;
					//g_VSCBuffer.parallax = g_fHalfIPD * g_fTextParallax;
					g_VSCBuffer.z_override = g_fTextParallax;
				}

				// Select either the original vertex buffer or the left-image vertex buffer. Certain elements are
				// rendered at infinity using the original VB and others are rendered at various depths but using
				// the original VB and adding a specific parallax (like the text and HUD)
				if (SUCCEEDED(hr))
				{
					// Use the original vertices for the Sky, HUD, Text and Brackets:
					/*
					if (bIsSkyBox || bIsHUD || bIsText || bIsBracket) // Should I add Floating GUI elements here as well?
						resources->InitVertexBuffer(this->_vertexBuffer.GetAddressOf(), &vertexBufferStride, &vertexBufferOffset);
					else
						resources->InitVertexBuffer(this->_vertexBufferL.GetAddressOf(), &vertexBufferStride, &vertexBufferOffset);
					*/
				}

				if (bModifiedShaders)
					resources->InitVSConstantBuffer3D(resources->_VSConstantBuffer.GetAddressOf(), &g_VSCBuffer);

				// Skip the draw call for debugging purposes depending on g_iNoDrawBeforeIndex and g_iNoDrawAfterIndex
#ifdef DBG_VR
				if (g_iDrawCounter < g_iNoDrawBeforeIndex)
					goto out;
				if (g_iNoDrawAfterIndex > -1 && g_iDrawCounter > g_iNoDrawAfterIndex)
					goto out;
#endif

				// ****************************************************************************
				// Render the left image
				// ****************************************************************************
				// SteamVR probably requires independent ZBuffers; for non-Steam we can get away
				// with just using one, though.
				if (g_bUseSteamVR)
					context->OMSetRenderTargets(1, resources->_renderTargetView.GetAddressOf(),
						resources->_depthStencilViewL.Get());
				else
					context->OMSetRenderTargets(1, resources->_renderTargetView.GetAddressOf(),
						resources->_depthStencilViewL.Get());
				// VIEWPORT-LEFT
				if (g_bUseSteamVR) {
					viewport.Width = (float)resources->_backbufferWidth;
				}
				else {
					viewport.Width = (float)resources->_backbufferWidth / 2.0f;
				}
				viewport.Height = (float)resources->_backbufferHeight;
				viewport.TopLeftX = 0.0f;
				viewport.TopLeftY = 0.0f;
				viewport.MinDepth = D3D11_MIN_DEPTH;
				viewport.MaxDepth = D3D11_MAX_DEPTH;
				resources->InitViewport(&viewport);
				// Set the left projection matrix
				g_VSMatrixCB.projEye = g_fullMatrixLeft;
				resources->InitVSConstantBufferMatrix(resources->_VSMatrixBuffer.GetAddressOf(), &g_VSMatrixCB);
				// Draw the Left Image
				context->DrawIndexed(3 * instruction->wCount, currentIndexLocation, 0);

				// ****************************************************************************
				// Right image state settings
				// ****************************************************************************

				// Let's render the triangle pointer closer to the center so that we can see it all the time,
				// also, render the triangle pointer at text depth.
				/*
				if (bIsTrianglePointer) {
					bModifiedShaders = true;
					g_VSCBuffer.viewportScale[3] = g_fGUIElemScale;
					g_VSCBuffer.parallax = -g_fHalfIPD * g_fTextParallax;
				}
				*/
				// Add an extra parallax to HUD elements, right image
				/*
				if (bIsHUD) { // This is now a common setting, no need to re-do it here
					bModifiedShaders = true;
					//g_VSCBuffer.parallax = -g_fHalfIPD * g_fHUDParallax;
					g_VSCBuffer.parallax = g_fHUDParallax;
				}
				*/

				/*
				// Add extra parallax to Floating GUI elements, right image
				if (bIsFloatingGUI || bIsFloating3DObject || bIsScaleableGUIElem) { // THis is now a common setting
					bModifiedShaders = true;
					 g_VSCBuffer.parallax = bIsFloating3DObject ?
						-g_fHalfIPD * (g_fFloatingGUIParallax + g_fFloatingGUIObjParallax) :
						-g_fHalfIPD * g_fFloatingGUIParallax; 
					g_VSCBuffer.z_override = g_fFloatingGUIParallax;
				}
				*/

				// Add an extra parallax to Text elements, right image
				if (bIsText) { // This is now a common setting
					bModifiedShaders = true;
					//g_VSCBuffer.parallax = -g_fHalfIPD * g_fTextParallax;
					g_VSCBuffer.z_override = g_fTextParallax;
				}

				// Select either the original vertex buffer or the right-image vertex buffer. Certain elements are
				// rendered at infinity using the original VB and others are rendered at various depths but using
				// the original VB and adding a specific parallax (like the text and HUD)
				if (SUCCEEDED(hr))
				{
					// Use the original vertices for the Sky, HUD, Text and Brackets:
					/*
					if (bIsSkyBox || bIsHUD || bIsText || bIsBracket) // Should I add Floating GUI elements here as well?
						resources->InitVertexBuffer(this->_vertexBuffer.GetAddressOf(), &vertexBufferStride, &vertexBufferOffset);
					else
						resources->InitVertexBuffer(this->_vertexBufferR.GetAddressOf(), &vertexBufferStride, &vertexBufferOffset);
					*/
				}

				if (bModifiedShaders)
					resources->InitVSConstantBuffer3D(resources->_VSConstantBuffer.GetAddressOf(), &g_VSCBuffer);

				// ****************************************************************************
				// Render the right image
				// ****************************************************************************
				// For SteamVR, we probably need two ZBuffers; but for non-Steam VR content we
				// just need one ZBuffer.
				//context->OMSetRenderTargets(1, resources->_renderTargetView.GetAddressOf(),
				//	resources->_depthStencilViewR.Get());
				if (g_bUseSteamVR)
					context->OMSetRenderTargets(1, resources->_renderTargetViewR.GetAddressOf(),
						resources->_depthStencilViewR.Get());
				else
					context->OMSetRenderTargets(1, resources->_renderTargetView.GetAddressOf(),
						resources->_depthStencilViewL.Get());

				// VIEWPORT-RIGHT
				if (g_bUseSteamVR) {
					viewport.Width = (float)resources->_backbufferWidth;
					viewport.TopLeftX = 0.0f;
				}
				else {
					viewport.Width = (float)resources->_backbufferWidth / 2.0f;
					viewport.TopLeftX = 0.0f + viewport.Width;
				}
				viewport.Height = (float)resources->_backbufferHeight;
				viewport.TopLeftY = 0.0f;
				viewport.MinDepth = D3D11_MIN_DEPTH;
				viewport.MaxDepth = D3D11_MAX_DEPTH;
				resources->InitViewport(&viewport);
				// Set the right projection matrix
				g_VSMatrixCB.projEye = g_fullMatrixRight;
				resources->InitVSConstantBufferMatrix(resources->_VSMatrixBuffer.GetAddressOf(), &g_VSMatrixCB);
				// Draw the Right Image
				context->DrawIndexed(3 * instruction->wCount, currentIndexLocation, 0);

				// out: label
#ifdef DBG_VR
			out:
#endif
				// Update counters
				g_iDrawCounter++;
				// Have we just finished drawing the targetting computer?
				if (lastTextureSelected != NULL && lastTextureSelected->is_Floating_GUI)
					g_iFloatingGUIDrawnCounter++;

				if (bIsBracket) {
					// Restore the No-Z-Write state for bracket elements
					QuickSetZWriteEnabled(bZWriteEnabled);
					g_VSCBuffer.z_override = -1.0f;
					resources->InitVSConstantBuffer3D(resources->_VSConstantBuffer.GetAddressOf(), &g_VSCBuffer);
				}

				// Restore the normal state of the render (currently this only means restore the original Vertex/Pixel
				// constant buffers); but only if we altered it previously.
				if (bModifiedShaders) {
					g_VSCBuffer.viewportScale[3] = g_fGlobalScale;
					g_VSCBuffer.parallax = 0.0f;
					g_PSCBuffer.brightness = MAX_BRIGHTNESS;
					g_VSCBuffer.z_override = -1.0f;
					resources->InitVSConstantBuffer3D(resources->_VSConstantBuffer.GetAddressOf(), &g_VSCBuffer);
					resources->InitPSConstantBufferBrightness(resources->_PSConstantBuffer.GetAddressOf(), &g_PSCBuffer);
				}

			no_vr_out:
				currentIndexLocation += 3 * instruction->wCount;
				break;
			}
			}

			if (FAILED(hr))
				break;
		}
	}

//noexec:
	g_iExecBufCounter++; // This variable is used to find when the SkyBox has been rendered

	if (FAILED(hr))
	{
		static bool messageShown = false;

		if (!messageShown)
		{
			char text[512];
			strcpy_s(text, step);
			strcat_s(text, "\n");
			strcat_s(text, _com_error(hr).ErrorMessage());

			MessageBox(nullptr, text, __FUNCTION__, MB_ICONERROR);
		}

		messageShown = true;

#if LOGGER
		str.str("\tD3DERR_EXECUTE_FAILED");
		LogText(str.str());
#endif

		return D3DERR_EXECUTE_FAILED;
	}

#if LOGGER
	str.str("\tD3D_OK");
	LogText(str.str());
#endif

	return D3D_OK;
}

HRESULT Direct3DDevice::AddViewport(
	LPDIRECT3DVIEWPORT lpDirect3DViewport
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tD3D_OK");
	LogText(str.str());
#endif

	return D3D_OK;
}

HRESULT Direct3DDevice::DeleteViewport(
	LPDIRECT3DVIEWPORT lpDirect3DViewport
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tD3D_OK");
	LogText(str.str());
#endif

	return D3D_OK;
}

HRESULT Direct3DDevice::NextViewport(
	LPDIRECT3DVIEWPORT lpDirect3DViewport,
	LPDIRECT3DVIEWPORT *lplpDirect3DViewport,
	DWORD dwFlags
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

HRESULT Direct3DDevice::Pick(
	LPDIRECT3DEXECUTEBUFFER lpDirect3DExecuteBuffer,
	LPDIRECT3DVIEWPORT lpDirect3DViewport,
	DWORD dwFlags,
	LPD3DRECT lpRect
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

HRESULT Direct3DDevice::GetPickRecords(
	LPDWORD lpCount,
	LPD3DPICKRECORD lpD3DPickRec
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

HRESULT Direct3DDevice::EnumTextureFormats(
	LPD3DENUMTEXTUREFORMATSCALLBACK lpd3dEnumTextureProc,
	LPVOID lpArg
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

	if (lpd3dEnumTextureProc == nullptr)
	{
		return DDERR_INVALIDPARAMS;
	}

	DDSURFACEDESC sd{};
	sd.dwSize = sizeof(DDSURFACEDESC);
	sd.dwFlags = DDSD_CAPS | DDSD_PIXELFORMAT;
	sd.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
	sd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);

	// 0555
	sd.ddpfPixelFormat.dwFlags = DDPF_RGB;
	sd.ddpfPixelFormat.dwRGBBitCount = 16;
	sd.ddpfPixelFormat.dwRBitMask = 0x7C00;
	sd.ddpfPixelFormat.dwGBitMask = 0x03E0;
	sd.ddpfPixelFormat.dwBBitMask = 0x001F;
	sd.ddpfPixelFormat.dwRGBAlphaBitMask = 0x0000;

	if (lpd3dEnumTextureProc(&sd, lpArg) != D3DENUMRET_OK)
		return D3D_OK;

	// 1555
	sd.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
	sd.ddpfPixelFormat.dwRGBBitCount = 16;
	sd.ddpfPixelFormat.dwRBitMask = 0x7C00;
	sd.ddpfPixelFormat.dwGBitMask = 0x03E0;
	sd.ddpfPixelFormat.dwBBitMask = 0x001F;
	sd.ddpfPixelFormat.dwRGBAlphaBitMask = 0x8000;

	if (lpd3dEnumTextureProc(&sd, lpArg) != D3DENUMRET_OK)
		return D3D_OK;

	// 4444
	sd.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
	sd.ddpfPixelFormat.dwRGBBitCount = 16;
	sd.ddpfPixelFormat.dwRBitMask = 0x0F00;
	sd.ddpfPixelFormat.dwGBitMask = 0x00F0;
	sd.ddpfPixelFormat.dwBBitMask = 0x000F;
	sd.ddpfPixelFormat.dwRGBAlphaBitMask = 0xF000;

	if (lpd3dEnumTextureProc(&sd, lpArg) != D3DENUMRET_OK)
		return D3D_OK;

	// 0565
	sd.ddpfPixelFormat.dwFlags = DDPF_RGB;
	sd.ddpfPixelFormat.dwRGBBitCount = 16;
	sd.ddpfPixelFormat.dwRBitMask = 0xF800;
	sd.ddpfPixelFormat.dwGBitMask = 0x07E0;
	sd.ddpfPixelFormat.dwBBitMask = 0x001F;
	sd.ddpfPixelFormat.dwRGBAlphaBitMask = 0x0000;

	if (lpd3dEnumTextureProc(&sd, lpArg) != D3DENUMRET_OK)
		return D3D_OK;

	// 0888
	sd.ddpfPixelFormat.dwFlags = DDPF_RGB;
	sd.ddpfPixelFormat.dwRGBBitCount = 32;
	sd.ddpfPixelFormat.dwRBitMask = 0x0FF0000;
	sd.ddpfPixelFormat.dwGBitMask = 0x0000FF00;
	sd.ddpfPixelFormat.dwBBitMask = 0x000000FF;
	sd.ddpfPixelFormat.dwRGBAlphaBitMask = 0x00000000;

	if (lpd3dEnumTextureProc(&sd, lpArg) != D3DENUMRET_OK)
		return D3D_OK;

	// 8888
	sd.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
	sd.ddpfPixelFormat.dwRGBBitCount = 32;
	sd.ddpfPixelFormat.dwRBitMask = 0x00FF0000;
	sd.ddpfPixelFormat.dwGBitMask = 0x0000FF00;
	sd.ddpfPixelFormat.dwBBitMask = 0x000000FF;
	sd.ddpfPixelFormat.dwRGBAlphaBitMask = 0xFF000000;

	if (lpd3dEnumTextureProc(&sd, lpArg) != D3DENUMRET_OK)
		return D3D_OK;

	return D3D_OK;
}

HRESULT Direct3DDevice::CreateMatrix(
	LPD3DMATRIXHANDLE lpD3DMatHandle
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

HRESULT Direct3DDevice::SetMatrix(
	D3DMATRIXHANDLE d3dMatHandle,
	LPD3DMATRIX lpD3DMatrix
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

HRESULT Direct3DDevice::GetMatrix(
	D3DMATRIXHANDLE d3dMatHandle,
	LPD3DMATRIX lpD3DMatrix
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

HRESULT Direct3DDevice::DeleteMatrix(
	D3DMATRIXHANDLE d3dMatHandle
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

HRESULT Direct3DDevice::BeginScene()
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

	if (!this->_deviceResources->_renderTargetView)
	{
#if LOGGER
		str.str("\tD3DERR_SCENE_BEGIN_FAILED");
		LogText(str.str());
#endif

		return D3DERR_SCENE_BEGIN_FAILED;
	}

	this->_deviceResources->inScene = true;
	this->_deviceResources->inSceneBackbufferLocked = false;

	auto& device = this->_deviceResources->_d3dDevice;
	auto& context = this->_deviceResources->_d3dDeviceContext;

	context->ClearRenderTargetView(this->_deviceResources->_renderTargetView, this->_deviceResources->clearColor);
	if (g_bUseSteamVR)
		context->ClearRenderTargetView(this->_deviceResources->_renderTargetViewR, this->_deviceResources->clearColor);
	context->ClearDepthStencilView(this->_deviceResources->_depthStencilViewL, D3D11_CLEAR_DEPTH, this->_deviceResources->clearDepth, 0);
	context->ClearDepthStencilView(this->_deviceResources->_depthStencilViewR, D3D11_CLEAR_DEPTH, this->_deviceResources->clearDepth, 0);

	if (FAILED(this->_deviceResources->RenderMain(this->_deviceResources->_backbufferSurface->_buffer, this->_deviceResources->_displayWidth, this->_deviceResources->_displayHeight, this->_deviceResources->_displayBpp)))
		return D3DERR_SCENE_BEGIN_FAILED;

	if (this->_deviceResources->_displayBpp == 2)
	{
		unsigned short* buffer = (unsigned short*)this->_deviceResources->_backbufferSurface->_buffer;
		int length = this->_deviceResources->_displayWidth * this->_deviceResources->_displayHeight;

		for (int i = 0; i < length; i++)
		{
			buffer[i] = 0x2000;
		}
	}
	else
	{
		unsigned int* buffer = (unsigned int*)this->_deviceResources->_backbufferSurface->_buffer;
		int length = this->_deviceResources->_displayWidth * this->_deviceResources->_displayHeight;

		for (int i = 0; i < length; i++)
		{
			buffer[i] = 0x200000;
		}
	}

	return D3D_OK;
}

HRESULT Direct3DDevice::EndScene()
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

	this->_deviceResources->sceneRendered = true;

	this->_deviceResources->inScene = false;

	return D3D_OK;
}

HRESULT Direct3DDevice::GetDirect3D(
	LPDIRECT3D *lplpD3D
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

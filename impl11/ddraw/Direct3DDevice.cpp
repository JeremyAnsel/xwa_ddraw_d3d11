// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt
// Extended for VR by Leo Reyes, 2019

// Shaders by Marty McFly (used with permission from the author)
// https://github.com/martymcmodding/qUINT/tree/master/Shaders

// _deviceResources->_backbufferWidth, _backbufferHeight: 3240, 2160 -- SCREEN Resolution
// resources->_displayWidth, resources->_displayHeight -- in-game resolution

#include "common.h"
#include "DeviceResources.h"
#include "Direct3DDevice.h"
#include "Direct3DExecuteBuffer.h"
#include "Direct3DTexture.h"
#include "BackbufferSurface.h"
#include "ExecuteBufferDumper.h"
// TODO: Remove later
//#include "TextureSurface.h"

#include <ScreenGrab.h>
#include <wincodec.h>
#include <vector>

#include "FreePIE.h"
#include <headers/openvr.h>
#include "Matrices.h"

#include "XWAObject.h"

#define DBG_MAX_PRESENT_LOGS 0

FILE *g_HackFile = NULL;

//ObjectEntry* objects = *(ObjectEntry **)0x7B33C4;
PlayerDataEntry *PlayerDataTable = (PlayerDataEntry *)0x8B94E0;
uint32_t *g_playerInHangar = (uint32_t *)0x09C6E40;

const float DEFAULT_FOCAL_DIST = 0.5f;
//const float DEFAULT_FOCAL_DIST_STEAMVR = 0.6f;
const float DEFAULT_IPD = 6.5f; // Ignored in SteamVR mode.

const float DEFAULT_HUD_PARALLAX = 1.7f;
const float DEFAULT_TEXT_PARALLAX = 0.45f;
const float DEFAULT_FLOATING_GUI_PARALLAX = 0.495f;
const float DEFAULT_FLOATING_OBJ_PARALLAX = -0.025f;

const float DEFAULT_TECH_LIB_PARALLAX = -2.0f;
const float DEFAULT_GUI_ELEM_SCALE = 0.75f;
const float DEFAULT_GUI_ELEM_PZ_THRESHOLD = 0.0008f;
const float DEFAULT_ZOOM_OUT_SCALE = 1.0f;
const bool DEFAULT_ZOOM_OUT_INITIAL_STATE = false;
//const float DEFAULT_ASPECT_RATIO = 1.33f;
const float DEFAULT_ASPECT_RATIO = 1.25f;
//const float DEFAULT_CONCOURSE_SCALE = 0.4f;
const float DEFAULT_CONCOURSE_SCALE = 12.0f;
//const float DEFAULT_CONCOURSE_ASPECT_RATIO = 2.0f; // Default for non-SteamVR
const float DEFAULT_CONCOURSE_ASPECT_RATIO = 1.33f; // Default for non-SteamVR
const float DEFAULT_GLOBAL_SCALE = 1.8f;
//const float DEFAULT_GLOBAL_SCALE_STEAMVR = 1.4f;
const float DEFAULT_LENS_K1 = 2.0f;
const float DEFAULT_LENS_K2 = 0.22f;
const float DEFAULT_LENS_K3 = 0.0f;
//const float DEFAULT_COCKPIT_PZ_THRESHOLD = 0.166f; // I used 0.13f for a long time until I jumped on a TIE-Interceptor
const float DEFAULT_COCKPIT_PZ_THRESHOLD = 10.0f; // De-activated
const int DEFAULT_SKYBOX_INDEX = 2;
const bool DEFAULT_INTERLEAVED_REPROJECTION = false;
const bool DEFAULT_BARREL_EFFECT_STATE = true;
const bool DEFAULT_BARREL_EFFECT_STATE_STEAMVR = false; // SteamVR provides its own lens correction, only enable it if the user really wants it
const float DEFAULT_BRIGHTNESS = 0.95f;
const bool DEFAULT_INVERSE_TRANSPOSE = false;
const float MAX_BRIGHTNESS = 1.0f;
const bool DEFAULT_FLOATING_AIMING_HUD = true;
const bool DEFAULT_NATURAL_CONCOURSE_ANIM = true;
const bool DEFAULT_DYNAMIC_COCKPIT_ENABLED = false;
const bool DEFAULT_FIXED_GUI_STATE = true;
// 6dof
const int DEFAULT_FREEPIE_SLOT = 0;
const float DEFAULT_ROLL_MULTIPLIER =  -1.0f;
const float DEFAULT_POS_X_MULTIPLIER =  1.0f;
const float DEFAULT_POS_Y_MULTIPLIER =  1.0f;
const float DEFAULT_POS_Z_MULTIPLIER = -1.0f;
const float DEFAULT_MIN_POS_X = -1.5f;
const float DEFAULT_MAX_POS_X =  1.5f;
const float DEFAULT_MIN_POS_Y = -1.5f;
const float DEFAULT_MAX_POS_Y =  1.5f;
const float DEFAULT_MIN_POS_Z = -0.1f;
const float DEFAULT_MAX_POS_Z =  2.5f;
const bool DEFAULT_STEAMVR_POS_FROM_FREEPIE = false;
const bool DEFAULT_RESHADE_ENABLED_STATE = false;
const bool DEFAULT_BLOOM_ENABLED_STATE = false;
// cockpit look constants
const float DEFAULT_YAW_MULTIPLIER   = 1.0f;
const float DEFAULT_PITCH_MULTIPLIER = 1.0f;
const float DEFAULT_YAW_OFFSET   = 0.0f;
const float DEFAULT_PITCH_OFFSET = 0.0f;

const char *FOCAL_DIST_VRPARAM = "focal_dist";
const char *STEREOSCOPY_STRENGTH_VRPARAM = "IPD";
const char *SIZE_3D_WINDOW_VRPARAM = "3d_window_size";
const char *SIZE_3D_WINDOW_ZOOM_OUT_VRPARAM = "3d_window_zoom_out_size";
const char *WINDOW_ZOOM_OUT_INITIAL_STATE_VRPARAM = "zoomed_out_on_startup";
const char *CONCOURSE_WINDOW_SCALE_VRPARAM = "concourse_window_scale";
const char *COCKPIT_Z_THRESHOLD_VRPARAM = "cockpit_z_threshold";
const char *ASPECT_RATIO_VRPARAM = "3d_aspect_ratio";
const char *CONCOURSE_ASPECT_RATIO_VRPARAM = "concourse_aspect_ratio";
const char *K1_VRPARAM = "k1";
const char *K2_VRPARAM = "k2";
const char *K3_VRPARAM = "k3";
const char *HUD_PARALLAX_VRPARAM = "HUD_depth";
const char *GUI_PARALLAX_VRPARAM = "GUI_depth";
const char *GUI_OBJ_PARALLAX_VRPARAM = "GUI_target_relative_depth";
const char *TEXT_PARALLAX_VRPARAM = "Text_depth";
const char *TECH_LIB_PARALLAX_VRPARAM = "Tech_Library_relative_depth";
const char *BRIGHTNESS_VRPARAM = "brightness";
const char *VR_MODE_VRPARAM = "VR_Mode"; // Select "None", "DirectSBS" or "SteamVR"
const char *VR_MODE_NONE_SVAL = "None";
const char *VR_MODE_DIRECT_SBS_SVAL = "DirectSBS";
const char *VR_MODE_STEAMVR_SVAL = "SteamVR";
const char *INTERLEAVED_REPROJ_VRPARAM = "SteamVR_Interleaved_Reprojection";
const char *BARREL_EFFECT_STATE_VRPARAM = "apply_lens_correction";
const char *INVERSE_TRANSPOSE_VRPARAM = "alternate_steamvr_eye_inverse";
const char *FLOATING_AIMING_HUD_VRPARAM = "floating_aiming_HUD";
const char *NATURAL_CONCOURSE_ANIM_VRPARAM = "concourse_animations_at_25fps";
const char *DYNAMIC_COCKPIT_ENABLED_VRPARAM = "dynamic_cockpit_enabled";
const char *FIXED_GUI_VRPARAM = "fixed_GUI";
const char *STICKY_ARROW_KEYS_VRPARAM = "sticky_arrow_keys";
// 6dof vrparams
const char *FREEPIE_SLOT_VRPARAM = "freepie_slot";
const char *ROLL_MULTIPLIER_VRPARAM = "roll_multiplier";
const char *POS_X_MULTIPLIER_VRPARAM = "positional_x_multiplier";
const char *POS_Y_MULTIPLIER_VRPARAM = "positional_y_multiplier";
const char *POS_Z_MULTIPLIER_VRPARAM = "positional_z_multiplier";
const char *MIN_POSITIONAL_X_VRPARAM = "min_positional_track_x";
const char *MAX_POSITIONAL_X_VRPARAM = "max_positional_track_x";
const char *MIN_POSITIONAL_Y_VRPARAM = "min_positional_track_y";
const char *MAX_POSITIONAL_Y_VRPARAM = "max_positional_track_y";
const char *MIN_POSITIONAL_Z_VRPARAM = "min_positional_track_z";
const char *MAX_POSITIONAL_Z_VRPARAM = "max_positional_track_z";
const char *STEAMVR_POS_FROM_FREEPIE_VRPARAM = "steamvr_pos_from_freepie";
const char *BLOOM_ENABLED_VRPARAM = "bloom_effect_enabled";
// Cockpitlook params
const char *YAW_MULTIPLIER_CLPARAM   = "yaw_multiplier";
const char *PITCH_MULTIPLIER_CLPARAM = "pitch_multiplier";
const char *YAW_OFFSET_CLPARAM       = "yaw_offset";
const char *PITCH_OFFSET_CLPARAM     = "pitch_offset";

// Dynamic Cockpit vrparams
const char *UV_COORDS_DCPARAM			= "uv_coords";
const char *COVER_TEX_NAME_DCPARAM		= "cover_texture";
const char *COVER_TEX_SIZE_DCPARAM		= "cover_texture_size";
const char *ERASE_REGION_DCPARAM			= "erase_region";
const char *MOVE_REGION_DCPARAM			= "move_region";
const char *CT_BRIGHTNESS_DCPARAM		= "cover_texture_brightness";
const char *DC_TARGET_COMP_UV_COORDS_VRPARAM   = "dc_target_comp_uv_coords";
const char *DC_LEFT_RADAR_UV_COORDS_VRPARAM    = "dc_left_radar_uv_coords";
const char *DC_RIGHT_RADAR_UV_COORDS_VRPARAM   = "dc_right_radar_uv_coords";
const char *DC_SHIELDS_PANEL_UV_COORDS_VRPARAM = "dc_shields_panel_uv_coords";
const char *DC_LASERS_PANEL_UV_COORDS_VRPARAM  = "dc_lasers_panel_uv_coords";
const char *DC_FRONT_PANEL_UV_COORDS_VRPARAM   = "dc_front_panel_uv_coords";

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
bool g_bInverseTranspose = DEFAULT_INVERSE_TRANSPOSE; // Transpose the eye matrices before computing the inverse
bool g_bResetHeadCenter = true; // Reset the head center on startup
vr::HmdMatrix34_t g_EyeMatrixLeft, g_EyeMatrixRight;
Matrix4 g_EyeMatrixLeftInv, g_EyeMatrixRightInv;
Matrix4 g_projLeft, g_projRight, g_projHead;
Matrix4 g_fullMatrixLeft, g_fullMatrixRight, g_fullMatrixHead;
Matrix4 g_viewMatrix;
bool g_bNaturalConcourseAnimations = DEFAULT_NATURAL_CONCOURSE_ANIM;
bool g_bDynCockpitEnabled = DEFAULT_DYNAMIC_COCKPIT_ENABLED;
float g_fYawMultiplier   = DEFAULT_YAW_MULTIPLIER;
float g_fPitchMultiplier = DEFAULT_PITCH_MULTIPLIER;
float g_fRollMultiplier  = DEFAULT_ROLL_MULTIPLIER;
float g_fYawOffset   = DEFAULT_YAW_OFFSET;
float g_fPitchOffset = DEFAULT_PITCH_OFFSET;
float g_fPosXMultiplier  = DEFAULT_POS_X_MULTIPLIER;
float g_fPosYMultiplier  = DEFAULT_POS_Y_MULTIPLIER;
float g_fPosZMultiplier  = DEFAULT_POS_Z_MULTIPLIER;
float g_fMinPositionX = DEFAULT_MIN_POS_X, g_fMaxPositionX = DEFAULT_MAX_POS_X;
float g_fMinPositionY = DEFAULT_MIN_POS_Y, g_fMaxPositionY = DEFAULT_MAX_POS_Y;
float g_fMinPositionZ = DEFAULT_MIN_POS_Z, g_fMaxPositionZ = DEFAULT_MAX_POS_Z;
bool g_bStickyArrowKeys = false;
Vector3 g_headCenter; // The head's center: this value should be re-calibrated whenever we set the headset
void projectSteamVR(float X, float Y, float Z, vr::EVREye eye, float &x, float &y, float &z);

/* Vertices that will be used for the VertexBuffer. */
D3DTLVERTEX *g_OrigVerts = NULL;

// Counter for calls to DrawIndexed() (This helps us know where were are in the rendering process)
// Gets reset everytime the backbuffer is presented and is increased only after BOTH the left and
// right images have been rendered.
int g_iDrawCounter = 0, g_iNoDrawBeforeIndex = 0, g_iNoDrawAfterIndex = -1, g_iDrawCounterAfterHUD = -1;
// Similar to the above, but only get incremented after each Execute() is finished.
int g_iExecBufCounter = 0, g_iNoExecBeforeIndex = 0, g_iNoExecAfterIndex = -1, g_iNoDrawAfterHUD = -1;
int g_iSkyBoxExecIndex = DEFAULT_SKYBOX_INDEX; // This gives us the threshold for the Skybox
// g_iSkyBoxExecIndex is compared against g_iExecBufCounter to determine when the SkyBox is rendered
// This is important because in XWAU the SkyBox is *not* rendered at infinity and causes a lot of
// visual contention if not handled properly.

bool g_bFixSkyBox = true; // Fix the skybox (send it to infinity: use original vertices without parallax)
bool g_bSkipSkyBox = false;
bool g_bStartedGUI = false; // Set to false at the beginning of each frame. Set to true when the GUI has begun rendering.
bool g_bPrevStartedGUI = false; // Keeps the last value of g_bStartedGUI -- helps detect when the GUI is about to be rendered.
bool g_bIsScaleableGUIElem = false; // Set to false at the beginning of each frame. Set to true when the scaleable GUI has begun rendering.
bool g_bPrevIsScaleableGUIElem = false; // Keeps the last value of g_bIsScaleableGUIElem -- helps detect when scaleable GUI is about to start rendering.
bool g_bScaleableHUDStarted = false; // Becomes true as soon as the scaleable HUD is about to be rendered. Reset to false at the end of each frame
bool g_bSkipGUI = false; // Skip non-skybox draw calls with disabled Z-Buffer
bool g_bSkipText = false; // Skips text draw calls
bool g_bSkipAfterTargetComp = false; // Skip all draw calls after the targetting computer has been drawn
bool g_bTargetCompDrawn = false; // Becomes true after the targetting computer has been drawn
bool g_bPrevIsFloatingGUI3DObject = false; // Stores the last value of g_bIsFloatingGUI3DObject -- useful to detect when the targeted craft is about to be drawn
bool g_bIsFloating3DObject = false; // true when rendering the targeted 3D object.
bool g_bIsTrianglePointer = false, g_bLastTrianglePointer = false;
//bool g_bLaserBoxLimitsUpdated = false; // Set to true whenever the laser/ion charge limit boxes are updated
unsigned int g_iFloatingGUIDrawnCounter = 0;
int g_iPresentCounter = 0, g_iNonZBufferCounter = 0, g_iSkipNonZBufferDrawIdx = -1;
// The following flag tells us when the main GUI elements (HUD, radars, etc) have been rendered
// It's reset to false every time the backbuffer is swapped.
float g_fZBracketOverride = 65530.0f; // 65535 is probably the maximum Z value in XWA

char g_sCurrentCockpit[128] = { 0 };
DCHUDBoxes g_DCHUDBoxes;
DCElemSrcBoxes g_DCElemSrcBoxes;
std::vector<dc_element> g_DCElements = {};
move_region_coords g_DCMoveRegions = { 0 };
float g_fCurInGameWidth = 1, g_fCurInGameHeight = 1, g_fCurScreenWidth = 1, g_fCurScreenHeight = 1;

extern bool g_bRendering3D; // Used to distinguish between 2D (Concourse/Menus) and 3D rendering (main in-flight game)
extern ID3D11Buffer *g_HUDVertexBuffer, *g_ClearHUDVertexBuffer, *g_ClearFullScreenHUDVertexBuffer;

// g_fZOverride is activated when it's greater than -0.9f, and it's used for bracket rendering so that 
// objects cover the brackets. In this way, we avoid visual contention from the brackets.
bool g_bCockpitPZHackEnabled = true;
bool g_bOverrideAspectRatio = false;
bool g_bEnableVR = true; // Enable/disable VR mode.
bool g_bReshadeEnabled = DEFAULT_RESHADE_ENABLED_STATE;
bool g_bBloomEnabled = DEFAULT_BLOOM_ENABLED_STATE;

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
float g_fHUDDepth = DEFAULT_HUD_PARALLAX; // The aiming HUD is rendered at this depth
bool g_bFloatingAimingHUD = DEFAULT_FLOATING_AIMING_HUD; // The aiming HUD can be fixed to the cockpit glass or floating
float g_fTextDepth = DEFAULT_TEXT_PARALLAX; // All text gets rendered at this parallax
float g_fFloatingGUIDepth = DEFAULT_FLOATING_GUI_PARALLAX; // Floating GUI elements are rendered at this depth
float g_fFloatingGUIObjDepth = DEFAULT_FLOATING_OBJ_PARALLAX; // The targeted object must be rendered above the Floating GUI
float g_fTechLibraryParallax = DEFAULT_TECH_LIB_PARALLAX;
float g_fAspectRatio = DEFAULT_ASPECT_RATIO;
bool g_bZoomOut = DEFAULT_ZOOM_OUT_INITIAL_STATE;
bool g_bZoomOutInitialState = DEFAULT_ZOOM_OUT_INITIAL_STATE;
float g_fBrightness = DEFAULT_BRIGHTNESS;
float g_fCoverTextureBrightness = 1.0f;
float g_fGUIElemsScale = DEFAULT_GLOBAL_SCALE; // Used to reduce the size of all the GUI elements
int g_iFreePIESlot = DEFAULT_FREEPIE_SLOT;
bool g_bFixedGUI = DEFAULT_FIXED_GUI_STATE;
bool g_bSteamVRPosFromFreePIE = DEFAULT_STEAMVR_POS_FROM_FREEPIE;
bool g_bDirectSBSInitialized = false;
//float g_fXWAScale = 1.0f; // This is the scale value as computed during Execute()
D3D11_VIEWPORT g_nonVRViewport{};

VertexShaderMatrixCB g_VSMatrixCB;
VertexShaderCBuffer g_VSCBuffer;
PixelShaderCBuffer g_PSCBuffer;

float g_fCockpitPZThreshold = DEFAULT_COCKPIT_PZ_THRESHOLD; // The TIE-Interceptor needs this thresold!
float g_fBackupCockpitPZThreshold = g_fCockpitPZThreshold; // Backup of the cockpit threshold, used when toggling this effect on or off.

const float IPD_SCALE_FACTOR = 100.0f; // Transform centimeters to meters (IPD = 6.5 becomes 0.065)
const float GAME_SCALE_FACTOR = 60.0f; // Estimated empirically
const float GAME_SCALE_FACTOR_Z = 60.0f; // Estimated empirically

// In reality, there should be a different factor per in-game resolution; but for now this should be enough
const float C = 1.0f, Z_FAR = 1.0f;
const float LOG_K = 1.0f;

float g_fIPD = DEFAULT_IPD / IPD_SCALE_FACTOR;
float g_fHalfIPD = g_fIPD / 2.0f;
float g_fFocalDist = DEFAULT_FOCAL_DIST;

/*
 * New Cockpit Textures
 */
extern ComPtr<ID3D11ShaderResourceView> g_NewDCTargetCompCover;
extern ComPtr<ID3D11ShaderResourceView> g_NewHUDLeftRadar;
extern ComPtr<ID3D11ShaderResourceView> g_NewHUDRightRadar;
extern ComPtr<ID3D11ShaderResourceView> g_NewDCLeftRadarCover;
extern ComPtr<ID3D11ShaderResourceView> g_NewDCRightRadarCover;
extern ComPtr<ID3D11ShaderResourceView> g_NewDCShieldsCover;
extern ComPtr<ID3D11ShaderResourceView> g_NewDCLasersCover;
extern ComPtr<ID3D11ShaderResourceView> g_NewDCFrontPanelCover;

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


/* Reloads all the CRCs. */
bool ReloadCRCs();
void LoadCockpitLookParams();
bool isInVector(uint32_t crc, std::vector<uint32_t> &vector);
bool InitDirectSBS();
//bool LoadNewCockpitTextures(ID3D11Device *device);
//void UnloadNewCockpitTextures();
int isInVector(char *name, std::vector<dc_element> dc_elements);

/* Maps (-6, 6) to (-0.5, 0.5) using a sigmoid function */
float centeredSigmoid(float x) {
	return 1.0f / (1.0f + exp(-x)) - 0.5f;
}

HeadPos g_HeadPosAnim = { 0 }, g_HeadPos = { 0 };
bool g_bLeftKeyDown, g_bRightKeyDown, g_bUpKeyDown, g_bDownKeyDown, g_bUpKeyDownShift, g_bDownKeyDownShift;
const float ANIM_INCR = 0.1f, MAX_LEAN_X = 1.5f, MAX_LEAN_Y = 1.5f, MAX_LEAN_Z = 1.5f;
// The MAX_LEAN values will be clamped by the limits from vrparams.cfg

void animTickX() {
	if (g_bRightKeyDown)
		g_HeadPosAnim.x -= ANIM_INCR;
	else if (g_bLeftKeyDown)
		g_HeadPosAnim.x += ANIM_INCR;
	else if (!g_bRightKeyDown && !g_bLeftKeyDown && !g_bStickyArrowKeys) {
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
	else if (!g_bDownKeyDown && !g_bUpKeyDown && !g_bStickyArrowKeys) {
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
	else if (!g_bDownKeyDownShift && !g_bUpKeyDownShift && !g_bStickyArrowKeys) {
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
	if (NewIPD < 0.0f)
		NewIPD = 0.0f;
	/*
	if (NewIPD > 12.0f) {
		NewIPD = 12.0f;
	}
	*/
	g_fIPD = NewIPD / IPD_SCALE_FACTOR;
	log_debug("[DBG] NewIPD: %0.3f, Actual g_fIPD: %0.6f", NewIPD, g_fIPD);
	g_fHalfIPD = g_fIPD / 2.0f;
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
	g_fZBracketOverride += Delta;
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
	g_fHUDDepth += Delta;
	log_debug("[DBG] HUD parallax: %f", g_fHUDDepth);
}

void IncreaseFloatingGUIParallax(float Delta) {
	g_fFloatingGUIDepth += Delta;
	log_debug("[DBG] GUI parallax: %f", g_fFloatingGUIDepth);
}

void IncreaseTextParallax(float Delta) {
	g_fTextDepth += Delta;
	log_debug("[DBG] Text parallax: %f", g_fTextDepth);
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

void IncreaseNoDrawAfterHUD(int Delta) {
	g_iNoDrawAfterHUD += Delta;
	log_debug("[DBG] NoDrawAfterHUD: %d", g_iNoDrawAfterHUD);
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
	g_bZoomOut = g_bZoomOutInitialState;
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
	g_fHUDDepth = DEFAULT_HUD_PARALLAX;
	g_bFloatingAimingHUD = DEFAULT_FLOATING_AIMING_HUD;
	g_fTextDepth = DEFAULT_TEXT_PARALLAX;
	g_fFloatingGUIDepth = DEFAULT_FLOATING_GUI_PARALLAX;
	g_fTechLibraryParallax = DEFAULT_TECH_LIB_PARALLAX;
	g_fFloatingGUIObjDepth = DEFAULT_FLOATING_OBJ_PARALLAX;
	g_bNaturalConcourseAnimations = DEFAULT_NATURAL_CONCOURSE_ANIM;

	g_fBrightness = DEFAULT_BRIGHTNESS;
	g_bStickyArrowKeys = false;

	g_bInterleavedReprojection = DEFAULT_INTERLEAVED_REPROJECTION;
	if (g_bUseSteamVR)
		g_pVRCompositor->ForceInterleavedReprojectionOn(g_bInterleavedReprojection);

	g_bDisableBarrelEffect = g_bUseSteamVR ? !DEFAULT_BARREL_EFFECT_STATE_STEAMVR : !DEFAULT_BARREL_EFFECT_STATE;

	g_bFixedGUI = DEFAULT_FIXED_GUI_STATE;

	g_iFreePIESlot = DEFAULT_FREEPIE_SLOT;
	g_fYawMultiplier   = DEFAULT_YAW_MULTIPLIER;
	g_fPitchMultiplier = DEFAULT_PITCH_MULTIPLIER;
	g_fRollMultiplier  = DEFAULT_ROLL_MULTIPLIER;
	g_fYawOffset   = DEFAULT_YAW_OFFSET;
	g_fPitchOffset = DEFAULT_PITCH_OFFSET;
	g_fPosXMultiplier  = DEFAULT_POS_X_MULTIPLIER;
	g_fPosYMultiplier  = DEFAULT_POS_Y_MULTIPLIER;
	g_fPosZMultiplier  = DEFAULT_POS_Z_MULTIPLIER;
	g_fMinPositionX = DEFAULT_MIN_POS_X; g_fMaxPositionX = DEFAULT_MAX_POS_X;
	g_fMinPositionY = DEFAULT_MIN_POS_Y; g_fMaxPositionY = DEFAULT_MAX_POS_Y;
	g_fMinPositionZ = DEFAULT_MIN_POS_Z; g_fMaxPositionZ = DEFAULT_MAX_POS_Z;

	// Recompute the eye and projection matrices
	if (!g_bUseSteamVR)
		InitDirectSBS();

	g_bReshadeEnabled = DEFAULT_RESHADE_ENABLED_STATE;
	g_bBloomEnabled = DEFAULT_BLOOM_ENABLED_STATE;
	g_bDynCockpitEnabled = DEFAULT_DYNAMIC_COCKPIT_ENABLED;
	// Load CRCs
	ReloadCRCs();
	LoadCockpitLookParams();
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
	fprintf(file, "; If you want to restore it to its default settings, delete the\n");
	fprintf(file, "; file and restart the game. Then press Ctrl+Alt+S to save a\n");
	fprintf(file, "; new config file with the default parameters -- however the\n");
	fprintf(file, "; VR mode may need to be set manually.\n");
	fprintf(file, "; To reload this file during game (at any point) just press Ctrl+Alt+L.\n");
	fprintf(file, "; Most parameters can be re-applied when reloading.\n");
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
	fprintf(file, "; remove the stereoscopy effect.\n");
	fprintf(file, "; This setting is ignored in SteamVR mode. Configure the IPD through SteamVR instead.\n");
	fprintf(file, "%s = %0.1f\n", STEREOSCOPY_STRENGTH_VRPARAM, g_fIPD * IPD_SCALE_FACTOR);
	fprintf(file, "%s = %0.3f\n", SIZE_3D_WINDOW_VRPARAM, g_fGlobalScale);
	fprintf(file, "%s = %0.3f\n", SIZE_3D_WINDOW_ZOOM_OUT_VRPARAM, g_fGlobalScaleZoomOut);
	fprintf(file, "; Set the following to 1 to start the HUD in zoomed-out mode:\n");
	fprintf(file, "%s = %d\n", WINDOW_ZOOM_OUT_INITIAL_STATE_VRPARAM, g_bZoomOutInitialState);
	fprintf(file, "%s = %0.3f\n", CONCOURSE_WINDOW_SCALE_VRPARAM, g_fConcourseScale);
	fprintf(file, "; The concourse animations can be played as fast as possible, or at its original\n");
	fprintf(file, "; 25fps setting:\n");
	fprintf(file, "%s = %d\n", NATURAL_CONCOURSE_ANIM_VRPARAM, g_bNaturalConcourseAnimations);
	/*
	fprintf(file, "; The following is a hack to increase the stereoscopy on objects. Unfortunately it\n");
	fprintf(file, "; also causes some minor artifacts: this is basically the threshold between the\n");
	fprintf(file, "; cockpit and the 'outside' world in normalized coordinates (0 is ZNear 1 is ZFar).\n");
	fprintf(file, "; Set it to 2 to disable this hack (stereoscopy will be reduced).\n");
	fprintf(file, "%s = %0.3f\n\n", COCKPIT_Z_THRESHOLD_VRPARAM, g_fCockpitPZThreshold);
	*/

	fprintf(file, "; Specify the aspect ratio here to override the aspect ratio computed by the library.\n");
	fprintf(file, "; ALWAYS specify BOTH the Concourse and 3D window aspect ratio.\n");
	fprintf(file, "; You can also edit ddraw.cfg and set 'PreserveAspectRatio = 1' to get the library to\n");
	fprintf(file, "; estimate the aspect ratio for you (this is the preferred method).\n");
	fprintf(file, "%s = %0.3f\n", ASPECT_RATIO_VRPARAM, g_fAspectRatio);
	fprintf(file, "%s = %0.3f\n", CONCOURSE_ASPECT_RATIO_VRPARAM, g_fConcourseAspectRatio);

	fprintf(file, "\n; Lens correction parameters. k2 has the biggest effect and k1 fine-tunes the effect.\n");
	fprintf(file, "; Positive values = convex warping; negative = concave warping. SteamVR already provides\n");
	fprintf(file, "; it's own automatic warping effect, so you probably shouldn't enable this in SteamVR mode.\n");
	fprintf(file, "%s = %0.6f\n", K1_VRPARAM, g_fLensK1);
	fprintf(file, "%s = %0.6f\n", K2_VRPARAM, g_fLensK2);
	fprintf(file, "%s = %0.6f\n", K3_VRPARAM, g_fLensK3);
	fprintf(file, "%s = %d\n", BARREL_EFFECT_STATE_VRPARAM, !g_bDisableBarrelEffect);

	fprintf(file, "\n; Depth for various GUI elements in meters from the head's origin.\n");
	fprintf(file, "; Positive depth is forwards, negative is backwards (towards you).\n");
	fprintf(file, "; As a reference, the background starfield is 65km meters away.\n");
	fprintf(file, "%s = %0.3f\n", HUD_PARALLAX_VRPARAM, g_fHUDDepth);
	fprintf(file, "; If 6dof is enabled, the aiming HUD can be fixed to the cockpit or it can \"float\"\n");
	fprintf(file, "; and follow the lasers. When it's fixed, it's probably more realistic; but it will\n");
	fprintf(file, "; be harder to aim when you lean.\n");
	fprintf(file, "; When the aiming HUD is floating, it will follow the lasers when you lean,\n");
	fprintf(file, "; making it easier to aim properly.\n");
	fprintf(file, "%s = %d\n", FLOATING_AIMING_HUD_VRPARAM, g_bFloatingAimingHUD);
	fprintf(file, "%s = %0.3f\n", GUI_PARALLAX_VRPARAM, g_fFloatingGUIDepth);
	fprintf(file, "%s = %0.3f\n", GUI_OBJ_PARALLAX_VRPARAM, g_fFloatingGUIObjDepth);
	fprintf(file, "; %s is relative and it's always added to %s\n", GUI_OBJ_PARALLAX_VRPARAM, GUI_PARALLAX_VRPARAM);
	fprintf(file, "; This has the effect of making the targeted object \"hover\" above the targeting computer\n");
	fprintf(file, "%s = %0.3f\n", TEXT_PARALLAX_VRPARAM, g_fTextDepth);
	fprintf(file, "; As a rule of thumb always make %s <= %s so that\n", TEXT_PARALLAX_VRPARAM, GUI_PARALLAX_VRPARAM);
	fprintf(file, "; the text hovers above the targeting computer\n\n");
	fprintf(file, "; This is the depth added to the controls in the tech library. Make it negative to bring the\n");
	fprintf(file, "; controls towards you. Objects in the tech library are obviously scaled by XWA, because there's\n");
	fprintf(file, "; otherwise no way to visualize both a Star Destroyer and an A-Wing in the same volume.\n");
	fprintf(file, "%s = %0.3f\n", TECH_LIB_PARALLAX_VRPARAM, g_fTechLibraryParallax);

	fprintf(file, "\n; The HUD/GUI can be fixed in space now. If this setting is enabled, you'll be\n");
	fprintf(file, "; able to see all the HUD simply by looking around. You may also lean forward to\n");
	fprintf(file, "; zoom-in on the text messages to make them more readable.\n");
	fprintf(file, "%s = %d\n", FIXED_GUI_VRPARAM, g_bFixedGUI);

	fprintf(file, "\n");
	fprintf(file, "; Set the following parameter to lower the brightness of the text,\n");
	fprintf(file, "; Concourse and 2D menus (avoids unwanted bloom when using ReShade).\n");
	fprintf(file, "; A value of 1 is normal brightness, 0 will render everything black.\n");
	fprintf(file, "%s = %0.3f\n", BRIGHTNESS_VRPARAM, g_fBrightness);

	fprintf(file, "\n; Interleaved Reprojection is a SteamVR setting that locks the framerate at 45fps.\n");
	fprintf(file, "; In some cases, it may help provide a smoother experience. Try toggling it\n");
	fprintf(file, "; to see what works better for your specific case.\n");
	fprintf(file, "%s = %d\n", INTERLEAVED_REPROJ_VRPARAM, g_bInterleavedReprojection);

	//fprintf(file, "\n");
	//fprintf(file, "%s = %d\n", INVERSE_TRANSPOSE_VRPARAM, g_bInverseTranspose);
	fprintf(file, "\n; 6dof section. Set any of these multipliers to 0 to de-activate individual axes.\n");
	fprintf(file, "; The settings for pitch and yaw are in cockpitlook.cfg\n");
	fprintf(file, "%s = %0.3f\n", ROLL_MULTIPLIER_VRPARAM,  g_fRollMultiplier);
	fprintf(file, "%s = %0.3f\n", POS_X_MULTIPLIER_VRPARAM, g_fPosXMultiplier);
	fprintf(file, "%s = %0.3f\n", POS_Y_MULTIPLIER_VRPARAM, g_fPosYMultiplier);
	fprintf(file, "%s = %0.3f\n", POS_Z_MULTIPLIER_VRPARAM, g_fPosZMultiplier);
	fprintf(file, "\n; Limits of the head tracker position in meters: this prevents you from\n");
	fprintf(file, "; \"leaning outside the cockpit\".\n");
	fprintf(file, "; x+ is to the right.\n");
	fprintf(file, "; y+ is down.\n");
	fprintf(file, "; z+ is forward.\n");
	fprintf(file, "%s = %0.3f\n",   MIN_POSITIONAL_X_VRPARAM, g_fMinPositionX);
	fprintf(file, "%s = %0.3f\n\n", MAX_POSITIONAL_X_VRPARAM, g_fMaxPositionX);
	fprintf(file, "%s = %0.3f\n",   MIN_POSITIONAL_Y_VRPARAM, g_fMinPositionY);
	fprintf(file, "%s = %0.3f\n\n", MAX_POSITIONAL_Y_VRPARAM, g_fMaxPositionY);
	fprintf(file, "%s = %0.3f\n",   MIN_POSITIONAL_Z_VRPARAM, g_fMinPositionZ);
	fprintf(file, "%s = %0.3f\n\n", MAX_POSITIONAL_Z_VRPARAM, g_fMaxPositionZ);

	fprintf(file, "; Specify which slot will be used to read FreePIE positional data.\n");
	fprintf(file, "; Only applies in DirectSBS mode (ignored in SteamVR mode).\n");
	fprintf(file, "%s = %d\n", FREEPIE_SLOT_VRPARAM, g_iFreePIESlot);

	// STEAMVR_POS_FROM_FREEPIE_VRPARAM is not saved because it's kind of a hack -- I'm only
	// using it because the PSMoveServiceSteamVRBridge is a bit tricky to setup and why would
	// I do that when my current FreePIEBridgeLite is working properly -- and faster.

	//fprintf(file, "\n; Enable the Bloom effect\n");
	//fprintf(file, "%s = %d\n", BLOOM_ENABLED_VRPARAM, g_bBloomEnabled);

	fclose(file);
	log_debug("[DBG] vrparams.cfg saved");
}

/* Loads cockpitlook params that are relevant to tracking */
void LoadCockpitLookParams() {
	log_debug("[DBG] Loading cockpit look params...");
	FILE *file;
	int error = 0;

	try {
		error = fopen_s(&file, "./cockpitlook.cfg", "rt");
	}
	catch (...) {
		log_debug("[DBG] Could not load cockpitlook.cfg");
	}

	if (error != 0) {
		log_debug("[DBG] Error %d when loading cockpitlook.cfg", error);
		return;
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
			value = (float)atof(svalue);
			if (_stricmp(param, YAW_MULTIPLIER_CLPARAM) == 0) {
				g_fYawMultiplier = value;
			}
			else if (_stricmp(param, PITCH_MULTIPLIER_CLPARAM) == 0) {
				g_fPitchMultiplier = value;
			}
			else if (_stricmp(param, YAW_OFFSET_CLPARAM) == 0) {
				g_fYawOffset = value;
			}
			else if (_stricmp(param, PITCH_OFFSET_CLPARAM) == 0) {
				g_fPitchOffset = value;
			}
			param_read_count++;
		}
	} // while ... read file
	fclose(file);
	log_debug("[DBG] Loaded %d cockpitlook params", param_read_count);
}

/*
 * Loads a "source_def" or "erase_def" line from the global coordinates file
 */
bool LoadDCGlobalUVCoords(char *buf, Box *coords)
{
	float width, height;
	float x0, y0, x1, y1;
	int res = 0;
	char *c = NULL;

	c = strchr(buf, '=');
	if (c != NULL) {
		c += 1;
		try {
			res = sscanf_s(c, "%f, %f, %f, %f, %f, %f",
				&width, &height, &x0, &y0, &x1, &y1);
			if (res < 6) {
				log_debug("[DBG] [DC] ERROR (skipping), expected at least 6 elements in '%s'", c);
				return false;
			}
			else {
				coords->x0 = x0 / width;
				coords->y0 = y0 / height;
				coords->x1 = x1 / width;
				coords->y1 = y1 / height;
			}
		}
		catch (...) {
			log_debug("[DBG] [DC] Could not read uv coords from: %s", buf);
			return false;
		}
	}
	return true;
}

DCHUDBoxes::DCHUDBoxes() {
	Clear();
	//log_debug("[DBG] [DC] Adding g_HUDRegionNames.size(): %d", g_HUDRegionNames.size());
	for (int i = 0; i < MAX_HUD_BOXES; i++) {
		DCHUDBox box = { 0 };
		box.bLimitsComputed = false;
		boxes.push_back(box);
	}
}

DCElemSrcBoxes::DCElemSrcBoxes() {
	Clear();
	//log_debug("[DBG] [DC] Adding g_DCElemSrcNames.size(): %d", g_DCElemSrcNames.size());
	for (int i = 0; i < MAX_DC_SRC_ELEMENTS; i++) {
		DCElemSrcBox src_box;
		src_boxes.push_back(src_box);
	}
}

/* 
 * Load dynamic_cockpit_global.cfg 
 * This function will not grow g_DCHUDBoxes and it expects it to be populated
 * already.
 */
bool LoadDCInternalCoordinates() {
	
	log_debug("[DBG] [DC] Loading Internal Dynamic Cockpit coordinates...");
	FILE *file;
	int error = 0, line = 0;
	static int lastDCElemSelected = -1;

	try {
		error = fopen_s(&file, "./DynamicCockpit/dynamic_cockpit_internal_areas.cfg", "rt");
	}
	catch (...) {
		log_debug("[DBG] [DC] Could not load DynamicCockpit/dynamic_cockpit_internal_areas.cfg");
	}

	if (error != 0) {
		log_debug("[DBG] [DC] ERROR %d when loading DynamicCockpit/dynamic_cockpit_internal_areas.cfg", error);
		return false;
	}

	char buf[256], param[128], svalue[128];
	int param_read_count = 0;
	unsigned int erase_slot = 0, source_slot = 0;
	float value = 0.0f;

	// TODO: Reset the global source and erase coordinates
	while (fgets(buf, 256, file) != NULL) {
		line++;
		// Skip comments and blank lines
		if (buf[0] == ';' || buf[0] == '#')
			continue;
		if (strlen(buf) == 0)
			continue;

		if (sscanf_s(buf, "%s = %s", param, 128, svalue, 128) > 0) {
			value = (float)atof(svalue);

			if (_stricmp(param, "source_def") == 0) {
				if (source_slot >= g_DCElemSrcBoxes.src_boxes.size()) {
					log_debug("[DBG] [DC] Ignoring '%s' because slot: %d does not exist\n", source_slot);
					continue;
				}
				Box box = { 0 };
				if (LoadDCGlobalUVCoords(buf, &box))
					g_DCElemSrcBoxes.src_boxes[source_slot].uv_coords = box;
				else
					log_debug("[DBG] [DC] WARNING: '%s' could not be loaded", buf);
				source_slot++;
			} else if (_stricmp(param, "erase_def") == 0) {
				if (erase_slot >= g_DCHUDBoxes.boxes.size()) {
					log_debug("[DBG] [DC] Ignoring '%s' because slot: %d does not exist\n", erase_slot);
					continue;
				}
				Box box = { 0 };
				if (LoadDCGlobalUVCoords(buf, &box)) {
					g_DCHUDBoxes.boxes[erase_slot].uv_erase_coords = box;
					g_DCHUDBoxes.boxes[erase_slot].bLimitsComputed = false; // Force a recompute of the limits
				}
				else
					log_debug("[DBG] [DC] WARNING: '%s' could not be loaded", buf);
				//log_debug("[DBG] [DC] Region %d = [%s] loaded", erase_slot, g_HUDRegionNames[erase_slot]);
				erase_slot++;
			}
		}
	}
	fclose(file);
	return true;
}

void ClearDynCockpitVector(std::vector<dc_element> &DCElements) {
	int size = (int)DCElements.size();
	for (int i = 0; i < size; i++) {
		DCElements[i].coverTextureName[0] = 0;
		if (DCElements[i].coverTexture != NULL) {
			DCElements[i].coverTexture->Release();
			DCElements[i].coverTexture = NULL;
		}
	}
	DCElements.clear();
}

/*
 * Reads the name-slot from a string of the form:
 * name, x0, y0, x1, y1
 * returns the number of chars read
 */
int ReadNameFromLine(char *buf, char *name) 
{
	int res, len;

	res = sscanf_s(buf, "%s", name, 50);
	if (res < 1) {
		log_debug("[DBG] [DC] ERROR: Expected a slot name in '%s'", buf);
		return 0;
	}

	len = strlen(name);
	if (len <= 0) {
		log_debug("[DBG] [DC] ERROR: Empty slot name in '%s'", buf);
		return 0;
	}

	// Remove the trailing semi-colon from the name:
	if (name[len - 1] == ';' || name[len - 1] == ',')
		name[len - 1] = '\0';
	return len;
}

/* 
 * Loads a uv_coords row:
 * uv_coords = src-slot, x0,y0,x1,y1, [Hex-Color]
 */
bool LoadDCUVCoords(char *buf, float width, float height, uv_src_dst_coords *coords)
{
	float x0, y0, x1, y1;
	int src_slot;
	uint32_t uColor;
	int res = 0, idx = coords->numCoords;
	char *substr = NULL;
	char slot_name[50];

	if (idx >= MAX_DC_COORDS) {
		log_debug("[DBG] [DC] Too many coords already loaded");
		return false;
	}

	substr = strchr(buf, '=');
	if (substr == NULL) {
		log_debug("[DBG] [DC] Missing '=' in '%s', skipping", buf);
		return false;
	}
	// Skip the equal sign:
	substr++;
	// Skip white space chars:
	while (*substr == ' ' || *substr == '\t')
		substr++;

	try {
		int len;
		uColor = 0x121233;

		src_slot = -1;
		slot_name[0] = 0;
		len = ReadNameFromLine(substr, slot_name);
		if (len == 0)
			return false;

		if (strstr(slot_name, "SRC") == NULL) {
			log_debug("[DBG] [DC] ERROR: Invalid source slot name: [%s]", slot_name);
			return false;
		}
				
		src_slot = DCSrcElemNameToIndex(slot_name);
		if (src_slot < 0) {
			log_debug("[DBG] [DC] ERROR: Could not find slot named: [%s]", slot_name);
			return false;
		}

		// Parse the rest of the parameters
		substr += len + 1;
		res = sscanf_s(substr, "%f, %f, %f, %f; 0x%x", &x0, &y0, &x1, &y1, &uColor);
		//log_debug("[DBG] [DC] res: %d, slot_name: %s", res, slot_name);
		if (res < 4) {
			log_debug("[DBG] [DC] ERROR (skipping), expected at least 4 elements in '%s'", substr);
		} else {
			coords->src_slot[idx] = src_slot;
			coords->dst[idx].x0 = x0 / width;
			coords->dst[idx].y0 = y0 / height;
			coords->dst[idx].x1 = x1 / width;
			coords->dst[idx].y1 = y1 / height;
			//if (res == 5) // A color was read, add the alpha
			//	uColor = (uColor << 8) | 0xFF;
			coords->uBGColor[idx] = uColor;
			coords->numCoords++;
		}
	}
	catch (...) {
		log_debug("[DBG] [DC] Could not read uv coords from: %s", buf);
		return false;
	}
	return true;
}

/*
 * Loads a cover_texture_size row
 */
bool LoadDCCoverTextureSize(char *buf, float *width, float *height)
{
	int res = 0;
	char *c = NULL;

	c = strchr(buf, '=');
	if (c != NULL) {
		c += 1;
		try {
			res = sscanf_s(c, "%f, %f", width, height);
			if (res < 2) {
				log_debug("[DBG] [DC] Error (skipping), expected at least 2 elements in '%s'", c);
			}
		}
		catch (...) {
			log_debug("[DBG] [DC] Could not read 'width, height' from: %s", buf);
			return false;
		}
	}
	return true;
}

/*
 * Loads a move_region row into g_DCMoveRegions:
 * move_region = region, x0,y0,x1,y1
 */
bool LoadDCMoveRegion(char *buf)
{
	float x0, y0, x1, y1;
	int region_slot;
	int res = 0, idx = g_DCMoveRegions.numCoords;
	char *substr = NULL;
	char region_name[50];

	if (idx >= MAX_HUD_BOXES) {
		log_debug("[DBG] [DC] Too many move_region commands");
		return false;
	}

	substr = strchr(buf, '=');
	if (substr == NULL) {
		log_debug("[DBG] [DC] Missing '=' in '%s', skipping", buf);
		return false;
	}
	// Skip the equal sign:
	substr++;
	// Skip white space chars:
	while (*substr == ' ' || *substr == '\t')
		substr++;

	try {
		int len;

		region_slot = -1;
		region_name[0] = 0;
		len = ReadNameFromLine(substr, region_name);
		if (len == 0)
			return false;

		if (strstr(region_name, "REGION") == NULL) {
			log_debug("[DBG] [DC] ERROR: Invalid region name: [%s]", region_name);
			return false;
		}

		region_slot = HUDRegionNameToIndex(region_name);
		if (region_slot < 0) {
			log_debug("[DBG] [DC] ERROR: Could not find region named: [%s]", region_name);
			return false;
		}

		// Parse the rest of the parameters
		substr += len + 1;
		res = sscanf_s(substr, "%f, %f, %f, %f", &x0, &y0, &x1, &y1);
		if (res < 4) {
			log_debug("[DBG] [DC] ERROR (skipping), expected at least 5 elements in '%s'", substr);
		}
		else {
			g_DCMoveRegions.region_slot[idx] = region_slot;
			g_DCMoveRegions.dst[idx].x0 = x0;
			g_DCMoveRegions.dst[idx].y0 = y0;
			g_DCMoveRegions.dst[idx].x1 = x1;
			g_DCMoveRegions.dst[idx].y1 = y1;
			g_DCMoveRegions.numCoords++;

			log_debug("[DBG] [DC] move_region [%s], (%0.3f, %0.3f)-(%0.3f, %0.3f)",
				region_name, x0, y0, x1, y1);
		}
	}
	catch (...) {
		log_debug("[DBG] [DC] Could not read coords from: %s", buf);
		return false;
	}
	return true;
}

/*
 * Clears all the move_region commands
 */
static inline void ClearDCMoveRegions() {
	g_DCMoveRegions.numCoords = 0;
}

/*
 * Convert a cockpit name into a DC params file of the form:
 * DynamicCockpit\<CockpitName>.dc
 */
void CockpitNameToDCParamsFile(char *CockpitName, char *sFileName, int iFileNameSize) {
	snprintf(sFileName, iFileNameSize, "DynamicCockpit\\%s.dc", CockpitName);
}

/*
 * Load the DC params for an individual cockpit. 
 * Resets g_DCElements (if we're not rendering in 3D), and the move regions.
 */
bool LoadIndividualDCParams(char *sFileName) {
	log_debug("[DBG] Loading Dynamic Cockpit params for [%s]...", sFileName);
	FILE *file;
	int error = 0, line = 0;
	static int lastDCElemSelected = -1;
	float cover_tex_width = 1, cover_tex_height = 1;

	try {
		error = fopen_s(&file, sFileName, "rt");
	}
	catch (...) {
		log_debug("[DBG] Could not load [%s]", sFileName);
	}

	if (error != 0) {
		log_debug("[DBG] Error %d when loading [%s]", error, sFileName);
		return false;
	}

	char buf[256], param[128], svalue[128];
	int param_read_count = 0;
	float value = 0.0f;

	// Reset the dynamic cockpit vector if we're not rendering in 3D
	if (!g_bRendering3D && g_DCElements.size() > 0) {
		log_debug("[DBG] [DC] Clearing g_DCElements");
		ClearDynCockpitVector(g_DCElements);
	}
	//ClearDCMoveRegions();

	while (fgets(buf, 256, file) != NULL) {
		line++;
		// Skip comments and blank lines
		if (buf[0] == ';' || buf[0] == '#')
			continue;
		if (strlen(buf) == 0)
			continue;

		if (sscanf_s(buf, "%s = %s", param, 128, svalue, 128) > 0) {
			value = (float)atof(svalue);

			if (buf[0] == '[') {
				// This is a new DC element.
				dc_element dc_elem;
				strcpy_s(dc_elem.name, MAX_TEXTURE_NAME, buf + 1);
				// Get rid of the trailing ']'
				char *end = strstr(dc_elem.name, "]");
				if (end != NULL)
					*end = 0;
				// See if we have this DC element already
				lastDCElemSelected = isInVector(dc_elem.name, g_DCElements);
				//log_debug("[DBG] [DC] New dc_elem.name: [%s], idx: %d",
				//	dc_elem.name, lastDCElemSelected);
				if (lastDCElemSelected > -1) {
					g_DCElements[lastDCElemSelected].coords.numCoords = 0;
					g_DCElements[lastDCElemSelected].num_erase_slots = 0;
					log_debug("[DBG] [DC] Resetting coords of exisiting DC elem @ idx: %d", lastDCElemSelected);
				}
				else {
					// Initialize this dc_elem:
					dc_elem.coverTextureName[0] = 0;
					dc_elem.coverTexture = NULL;
					dc_elem.coords = { 0 };
					dc_elem.num_erase_slots = 0;
					dc_elem.bActive = false;
					dc_elem.bNameHasBeenTested = false;
					g_DCElements.push_back(dc_elem);
					lastDCElemSelected = (int)g_DCElements.size() - 1;
				}
			}
			else if (_stricmp(param, UV_COORDS_DCPARAM) == 0) {
				if (g_DCElements.size() == 0) {
					log_debug("[DBG] [DC] ERROR. Line %d, g_DCElements is empty, cannot add %s", line, param, UV_COORDS_DCPARAM);
					continue;
				}
				if (lastDCElemSelected == -1) {
					log_debug("[DBG] [DC] ERROR. Line %d, %s without a corresponding texture section.", line, UV_COORDS_DCPARAM);
					continue;
				}
				LoadDCUVCoords(buf, cover_tex_width, cover_tex_height, &(g_DCElements[lastDCElemSelected].coords));
			}
			/*
			else if (_stricmp(param, MOVE_REGION_DCPARAM) == 0) {
				LoadDCMoveRegion(buf);
			}
			*/
			else if (_stricmp(param, ERASE_REGION_DCPARAM) == 0) {
				if (g_DCElements.size() == 0) {
					log_debug("[DBG] [DC] ERROR. Line %d, g_DCElements is empty, cannot add %s", line, param, ERASE_REGION_DCPARAM);
					continue;
				}
				if (lastDCElemSelected == -1) {
					log_debug("[DBG] [DC] ERROR. Line %d, %s without a corresponding texture section.", line, ERASE_REGION_DCPARAM);
					continue;
				}
				// The name of the region must contain the word "REGION":
				if (strstr(svalue, "REGION") == NULL) {
					log_debug("[DBG] [DC] ERROR: Invalid region name: [%s]", svalue);
					continue;
				}

				int slot = HUDRegionNameToIndex(svalue);
				if (slot < 0) {
					log_debug("[DBG] [DC] ERROR: Unknown region: [%s]", svalue);
					continue;
				}

				if (slot < (int)g_DCHUDBoxes.boxes.size()) {
					int next_idx = g_DCElements[lastDCElemSelected].num_erase_slots;
					g_DCElements[lastDCElemSelected].erase_slots[next_idx] = slot;
					g_DCElements[lastDCElemSelected].num_erase_slots++;
					//log_debug("[DBG] [DC] Added erase slot [%s] to DCElem %d", svalue,
					//	lastDCElemSelected);
				}
				else
					log_debug("[DBG] [DC] WARNING: erase_region = %d IGNORED: Not enough g_DCHUDBoxes", slot);
			}
			else if (_stricmp(param, COVER_TEX_NAME_DCPARAM) == 0) {
				if (lastDCElemSelected == -1) {
					log_debug("[DBG] [DC] ERROR. Line %d, %s without a corresponding texture section.", line, COVER_TEX_NAME_DCPARAM);
					continue;
				}
				strcpy_s(g_DCElements[lastDCElemSelected].coverTextureName, MAX_TEXTURE_NAME, svalue);
			}
			else if (_stricmp(param, COVER_TEX_SIZE_DCPARAM) == 0) {
				LoadDCCoverTextureSize(buf, &cover_tex_width, &cover_tex_height);
			}
		}
	}
	fclose(file);
	return true;
}

/* Loads the dynamic_cockpit.cfg file */
bool LoadDCParams() {
	log_debug("[DBG] Loading Dynamic Cockpit params...");
	FILE *file;
	int error = 0, line = 0;
	static int lastDCElemSelected = -1;
	float cover_tex_width = 1, cover_tex_height = 1;

	try {
		error = fopen_s(&file, "./dynamic_cockpit.cfg", "rt");
	}
	catch (...) {
		log_debug("[DBG] Could not load dynamic_cockpit.cfg");
	}

	if (error != 0) {
		log_debug("[DBG] Error %d when loading dynamic_cockpit.cfg", error);
		return false;
	}

	char buf[256], param[128], svalue[128];
	int param_read_count = 0;
	float value = 0.0f;

	// Reset the dynamic cockpit vector if we're not rendering in 3D
	if (!g_bRendering3D && g_DCElements.size() > 0) {
		log_debug("[DBG] [DC] Clearing g_DCElements");
		ClearDynCockpitVector(g_DCElements);
	}
	ClearDCMoveRegions();

	/* Reload individual cockit parameters if the current cockpit is set */
	bool bCockpitParamsLoaded = false;
	if (g_sCurrentCockpit[0] != 0) {
		char sFileName[80];
		CockpitNameToDCParamsFile(g_sCurrentCockpit, sFileName, 80);
		bCockpitParamsLoaded = LoadIndividualDCParams(sFileName);
	}

	while (fgets(buf, 256, file) != NULL) {
		line++;
		// Skip comments and blank lines
		if (buf[0] == ';' || buf[0] == '#')
			continue;
		if (strlen(buf) == 0)
			continue;

		if (sscanf_s(buf, "%s = %s", param, 128, svalue, 128) > 0) {
			value = (float)atof(svalue);

			if (_stricmp(param, DYNAMIC_COCKPIT_ENABLED_VRPARAM) == 0) {
				g_bDynCockpitEnabled = (bool)value;
				log_debug("[DBG] [DC] g_bDynCockpitEnabled: %d", g_bDynCockpitEnabled);
				if (!g_bDynCockpitEnabled) {
					// Early abort: stop reading coordinates if the dynamic cockpit is disabled
					fclose(file);
					return false;
				}
			}
			else if (_stricmp(param, MOVE_REGION_DCPARAM) == 0) {
				// Individual cockpit move_region commands override the global move_region commands:
				if (!bCockpitParamsLoaded)
					LoadDCMoveRegion(buf);
			}
			else if (_stricmp(param, CT_BRIGHTNESS_DCPARAM) == 0) {
				g_fCoverTextureBrightness = value;
			}
		}
	}
	fclose(file);
	return true;
}

/* Loads the VR parameters from vrparams.cfg */
void LoadVRParams() {
	log_debug("[DBG] Loading view params...");
	FILE *file;
	int error = 0, line = 0;
	static int lastDCElemSelected = -1;

	try {
		error = fopen_s(&file, "./vrparams.cfg", "rt");
	}
	catch (...) {
		log_debug("[DBG] Could not load vrparams.cfg");
	}

	if (error != 0) {
		log_debug("[DBG] Error %d when loading vrparams.cfg", error);
		goto next;
	}

	char buf[256], param[128], svalue[128];
	int param_read_count = 0;
	float value = 0.0f;

	// Reset the dynamic cockpit vector if we're not rendering in 3D
	if (!g_bRendering3D && g_DCElements.size() > 0) {
		log_debug("[DBG] [DC] Clearing g_DCElements");
		ClearDynCockpitVector(g_DCElements);
	}

	while (fgets(buf, 256, file) != NULL) {
		line++;
		// Skip comments and blank lines
		if (buf[0] == ';' || buf[0] == '#')
			continue;
		if (strlen(buf) == 0)
			continue;

		if (sscanf_s(buf, "%s = %s", param, 128, svalue, 128) > 0) {
			value = (float)atof(svalue);
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
			else if (_stricmp(param, WINDOW_ZOOM_OUT_INITIAL_STATE_VRPARAM) == 0) {
				g_bZoomOutInitialState = (bool)value;
				g_bZoomOut = (bool)value;
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
				g_fHUDDepth = value;
			}
			else if (_stricmp(param, FLOATING_AIMING_HUD_VRPARAM) == 0) {
				g_bFloatingAimingHUD = (bool)value;
			}
			else if (_stricmp(param, GUI_PARALLAX_VRPARAM) == 0) {
				// "Floating" GUI elements: targetting computer and the like
				g_fFloatingGUIDepth = value;
			}
			else if (_stricmp(param, GUI_OBJ_PARALLAX_VRPARAM) == 0) {
				// "Floating" GUI targeted elements
				g_fFloatingGUIObjDepth = value;
			}
			else if (_stricmp(param, TEXT_PARALLAX_VRPARAM) == 0) {
				g_fTextDepth = value;
			}
			else if (_stricmp(param, TECH_LIB_PARALLAX_VRPARAM) == 0) {
				g_fTechLibraryParallax = value;
			}
			else if (_stricmp(param, BRIGHTNESS_VRPARAM) == 0) {
				g_fBrightness = value;
			}
			else if (_stricmp(param, STICKY_ARROW_KEYS_VRPARAM) == 0) {
				g_bStickyArrowKeys = (bool)value;
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
				if (g_bUseSteamVR) {
					log_debug("[DBG] Setting Interleaved Reprojection to: %d", g_bInterleavedReprojection);
					g_pVRCompositor->ForceInterleavedReprojectionOn(g_bInterleavedReprojection);
				}
			}
			else if (_stricmp(param, INVERSE_TRANSPOSE_VRPARAM) == 0) {
				g_bInverseTranspose = (bool)value;
				log_debug("[DBG] Inverse Transpose set to: %d", g_bInverseTranspose);
			}
			else if (_stricmp(param, NATURAL_CONCOURSE_ANIM_VRPARAM) == 0) {
				g_bNaturalConcourseAnimations = (bool)value;
			}
			else if (_stricmp(param, FIXED_GUI_VRPARAM) == 0) {
				g_bFixedGUI = (bool)value;
			}

			// 6dof parameters
			else if (_stricmp(param, FREEPIE_SLOT_VRPARAM) == 0) {
				g_iFreePIESlot = (int)value;
			}
			else if (_stricmp(param, ROLL_MULTIPLIER_VRPARAM) == 0) {
				g_fRollMultiplier = value;
			}
			else if (_stricmp(param, POS_X_MULTIPLIER_VRPARAM) == 0) {
				g_fPosXMultiplier = value;
			}
			else if (_stricmp(param, POS_Y_MULTIPLIER_VRPARAM) == 0) {
				g_fPosYMultiplier = value;
			}
			else if (_stricmp(param, POS_Z_MULTIPLIER_VRPARAM) == 0) {
				g_fPosZMultiplier = value;
			}

			else if (_stricmp(param, MIN_POSITIONAL_X_VRPARAM) == 0) {
				g_fMinPositionX = value;
			}
			else if (_stricmp(param, MAX_POSITIONAL_X_VRPARAM) == 0) {
				g_fMaxPositionX = value;
			}
			else if (_stricmp(param, MIN_POSITIONAL_Y_VRPARAM) == 0) {
				g_fMinPositionY = value;
			}
			else if (_stricmp(param, MAX_POSITIONAL_Y_VRPARAM) == 0) {
				g_fMaxPositionY = value;
			}
			else if (_stricmp(param, MIN_POSITIONAL_Z_VRPARAM) == 0) {
				g_fMinPositionZ = value;
			}
			else if (_stricmp(param, MAX_POSITIONAL_Z_VRPARAM) == 0) {
				g_fMaxPositionZ = value;
			}
			else if (_stricmp(param, STEAMVR_POS_FROM_FREEPIE_VRPARAM) == 0) {
				g_bSteamVRPosFromFreePIE = (bool)value;
			}

			/*
			// ReShade state
			else if (_stricmp(param, BLOOM_ENABLED_VRPARAM) == 0) {
				bool state = (bool)value;
				g_bReshadeEnabled |= state;
				g_bBloomEnabled = state;
			}
			*/
			param_read_count++;
		}
	} // while ... read file
	// Apply the initial Zoom Out state:
	g_fGUIElemsScale = g_bZoomOut ? g_fGlobalScaleZoomOut : g_fGlobalScale;
	fclose(file);

	log_debug("[DBG] Reshade Enabled: %d", g_bReshadeEnabled);
	log_debug("[DBG] Bloom Enabled: %d", g_bBloomEnabled);
next:
	// Load CRCs
	ReloadCRCs();
	// Load cockpit look params
	LoadCockpitLookParams();
	// Load the global dynamic cockpit coordinates
	LoadDCInternalCoordinates();
	// Load Dynamic Cockpit params
	LoadDCParams();
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

void DumpMatrix4(FILE *file, const Matrix4 &mat) {
	fprintf(file, "%0.6f, %0.6f, %0.6f, %0.6f\n", mat[0], mat[4], mat[8], mat[12]);
	fprintf(file, "%0.6f, %0.6f, %0.6f, %0.6f\n", mat[1], mat[5], mat[9], mat[13]);
	fprintf(file, "%0.6f, %0.6f, %0.6f, %0.6f\n", mat[2], mat[6], mat[10], mat[14]);
	fprintf(file, " %0.6f, %0.6f, %0.6f, %0.6f\n", mat[3], mat[7], mat[11], mat[15]);
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

Matrix4 HmdMatrix34toMatrix4(const vr::HmdMatrix34_t &mat) {
	Matrix4 matrixObj(
		mat.m[0][0], mat.m[1][0], mat.m[2][0], 0.0f,
		mat.m[0][1], mat.m[1][1], mat.m[2][1], 0.0f,
		mat.m[0][2], mat.m[1][2], mat.m[2][2], 0.0f,
		mat.m[0][3], mat.m[1][3], mat.m[2][3], 1.0f
	);
	return matrixObj;
}

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

	Matrix4 matrixObj = HmdMatrix34toMatrix4(eyeMatrix);
	/*
	// Pimax matrix: 11.11 degrees on the Y axis and 6.64 IPD
	Matrix4 matrixObj(
		0.984808, 0.000000, 0.173648, -0.033236,
		0.000000, 1.000000, 0.000000, 0.000000,
		-0.173648, 0.000000, 0.984808, 0.000000,
		0, 0, 0, 1);
	matrixObj.transpose();
	*/

	/*
	if (g_bInverseTranspose) {
		log_debug("[DBG] Computing Inverse Transpose");
		matrixObj.transpose();
	}
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

void TestPimax() {
	vr::HmdMatrix34_t eyeLeft;
	eyeLeft.m[0][0] =  0.984808f; eyeLeft.m[0][1] = 0.000000f, eyeLeft.m[0][2] = 0.173648f; eyeLeft.m[0][3] = -0.033236f;
	eyeLeft.m[1][0] =  0.000000f; eyeLeft.m[1][1] = 1.000000f; eyeLeft.m[1][2] = 0.000000f; eyeLeft.m[1][3] =  0.000000f;
	eyeLeft.m[2][0] = -0.173648f; eyeLeft.m[2][1] = 0.000000f; eyeLeft.m[2][2] = 0.984808f; eyeLeft.m[2][3] =  0.000000f;

	vr::HmdMatrix34_t eyeRight;
	eyeRight.m[0][0] = 0.984808f; eyeRight.m[0][1] = 0.000000f, eyeRight.m[0][2] = -0.173648f; eyeRight.m[0][3] = 0.033236f;
	eyeRight.m[1][0] = 0.000000f; eyeRight.m[1][1] = 1.000000f; eyeRight.m[1][2] =  0.000000f; eyeRight.m[1][3] = 0.000000f;
	eyeRight.m[2][0] = 0.173648f; eyeRight.m[2][1] = 0.000000f; eyeRight.m[2][2] =  0.984808f; eyeRight.m[2][3] = 0.000000f;

	g_EyeMatrixLeftInv = HmdMatrix34toMatrix4(eyeLeft);
	g_EyeMatrixRightInv = HmdMatrix34toMatrix4(eyeRight);
	g_EyeMatrixLeftInv.invertGeneral();
	g_EyeMatrixRightInv.invertGeneral();

	vr::HmdMatrix44_t projLeft;
	projLeft.m[0][0] = 0.647594f; projLeft.m[0][1] = 0.000000f; projLeft.m[0][2] = -0.128239f; projLeft.m[0][3] =  0.000000f;
	projLeft.m[1][0] = 0.000000f; projLeft.m[1][1] = 0.787500f; projLeft.m[1][2] =  0.000000f; projLeft.m[1][3] =  0.000000f;
	projLeft.m[2][0] = 0.000000f; projLeft.m[2][1] = 0.000000f; projLeft.m[2][2] = -1.010101f; projLeft.m[2][3] = -0.505050f;
	projLeft.m[3][0] = 0.000000f; projLeft.m[3][1] = 0.000000f; projLeft.m[3][2] = -1.000000f; projLeft.m[3][3] =  0.000000f;

	vr::HmdMatrix44_t projRight;
	projRight.m[0][0] = 0.647594f; projRight.m[0][1] = 0.000000f; projRight.m[0][2] =  0.128239f; projRight.m[0][3] =  0.000000f;
	projRight.m[1][0] = 0.000000f; projRight.m[1][1] = 0.787500f; projRight.m[1][2] =  0.000000f; projRight.m[1][3] =  0.000000f;
	projRight.m[2][0] = 0.000000f; projRight.m[2][1] = 0.000000f; projRight.m[2][2] = -1.010101f; projRight.m[2][3] = -0.505050f;
	projRight.m[3][0] = 0.000000f; projRight.m[3][1] = 0.000000f; projRight.m[3][2] = -1.000000f; projRight.m[3][3] =  0.000000f;

	g_projLeft = HmdMatrix44toMatrix4(projLeft);
	g_projRight = HmdMatrix44toMatrix4(projRight);
}

void Test2DMesh() {
	/*
	MainVertex vertices[4] =
	{
		MainVertex(-1, -1, 0, 1),
		MainVertex(1, -1, 1, 1),
		MainVertex(1,  1, 1, 0),
		MainVertex(-1,  1, 0, 0),
	};
	*/
	Matrix4 fullMatrixLeft = g_fullMatrixLeft;
	//fullMatrixLeft.invertGeneral();
	Vector4 P, Q;

	P.set(-12, -12, -30, 0);
	Q = fullMatrixLeft * P;
	Q = Q / Q[3];
	log_debug("[DBG] (%0.3f, %0.3f) --> (%0.3f, %0.3f)", P[0], P[1], Q[0], Q[1]);

	P.set(12, -12, -30, 0);
	Q = fullMatrixLeft * P;
	Q = Q / Q[3];
	log_debug("[DBG] (%0.3f, %0.3f) --> (%0.3f, %0.3f)", P[0], P[1], Q[0], Q[1]);

	P.set(-12, 12, -30, 0);
	Q = fullMatrixLeft * P;
	Q = Q / Q[3];
	log_debug("[DBG] (%0.3f, %0.3f) --> (%0.3f, %0.3f)", P[0], P[1], Q[0], Q[1]);

	P.set(12, 12, -30, 0);
	Q = fullMatrixLeft * P;
	Q = Q / Q[3];
	log_debug("[DBG] (%0.3f, %0.3f) --> (%0.3f, %0.3f)", P[0], P[1], Q[0], Q[1]);
}

struct MainVertex
{
	float pos[2];
	float tex[2];
	MainVertex() {}
	MainVertex(float x, float y, float tx, float ty) {
		pos[0] = x; pos[1] = y;
		tex[0] = tx; tex[1] = ty;
	}
};

bool InitSteamVR()
{
	char *strDriver = NULL;
	char *strDisplay = NULL;
	FILE *file = NULL;
	//Matrix4 RollTest;
	bool result = true;

	Matrix4 g_targetCompView
	(
		3.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 6.0f, 0.0f, 2.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	g_targetCompView.transpose();

	int file_error = fopen_s(&file, "./steamvr_mat.txt", "wt");
	log_debug("[DBG] Initializing SteamVR");
	vr::EVRInitError eError = vr::VRInitError_None;
	g_pHMD = vr::VR_Init(&eError, vr::VRApplication_Scene);
	g_headCenter.set(0, 0, 0);

	// Generic matrix used for the dynamic targeting computer -- maybe I should use the left projection matrix instead?
	g_projHead.set
	(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f, -0.01f, // Use the focal_dist here?
		0.0f, 0.0f, -1.0f, 0.0f
	);
	g_projHead.transpose();

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

	vr::HmdMatrix44_t projLeft = g_pHMD->GetProjectionMatrix(vr::EVREye::Eye_Left, 0.001f, 100.0f);
	vr::HmdMatrix44_t projRight = g_pHMD->GetProjectionMatrix(vr::EVREye::Eye_Right, 0.001f, 100.0f);

	g_projLeft  = HmdMatrix44toMatrix4(projLeft);
	g_projRight = HmdMatrix44toMatrix4(projRight);

	// Override all of the above with the Pimax matrices
	//TestPimax();

	g_fullMatrixLeft  = g_projLeft  * g_EyeMatrixLeftInv;
	g_fullMatrixRight = g_projRight * g_EyeMatrixRightInv;
	g_fullMatrixHead = g_projHead * g_targetCompView; // The center matrix does not have eye parallax

	//Test2DMesh();

	ShowMatrix4(g_EyeMatrixLeftInv, "g_EyeMatrixLeftInv");
	ShowMatrix4(g_projLeft, "g_projLeft");

	ShowMatrix4(g_EyeMatrixRightInv, "g_EyeMatrixRightInv");
	ShowMatrix4(g_projRight, "g_projRight");
	
	// Initialize FreePIE if we're going to use it to read the position.
	if (g_bSteamVRPosFromFreePIE)
		InitFreePIE();

	// Dump information about the view matrices
	if (file_error == 0) {
		Matrix4 eye, test;

		if (strDriver != NULL)
			fprintf(file, "Driver: %s\n", strDriver);
		if (strDisplay != NULL)
			fprintf(file, "Display: %s\n", strDisplay);
		fprintf(file, "\n");

		// Left Eye matrix
		fprintf(file, "eyeLeft:\n");
		DumpMatrix34(file, g_EyeMatrixLeft);
		fprintf(file, "\n");

		fprintf(file, "eyeLeftInv:\n");
		DumpMatrix4(file, g_EyeMatrixLeftInv);
		fprintf(file, "\n");

		eye = HmdMatrix34toMatrix4(g_EyeMatrixLeft);
		test = eye * g_EyeMatrixLeftInv;
		fprintf(file, "Left Eye inverse test:\n");
		DumpMatrix4(file, test);
		fprintf(file, "\n");

		// Right Eye matrix
		fprintf(file, "eyeRight:\n");
		DumpMatrix34(file, g_EyeMatrixRight);
		fprintf(file, "\n");

		fprintf(file, "eyeRightInv:\n");
		DumpMatrix4(file, g_EyeMatrixRightInv);
		fprintf(file, "\n");

		eye = HmdMatrix34toMatrix4(g_EyeMatrixRight);
		test = eye * g_EyeMatrixRightInv;
		fprintf(file, "Right Eye inverse test:\n");
		DumpMatrix4(file, test);
		fprintf(file, "\n");

		// Z_FAR was 50 for version 0.9.6, and I believe Z_Near was 0.5 (focal dist)
		//vr::HmdMatrix44_t projLeft = g_pHMD->GetProjectionMatrix(vr::EVREye::Eye_Left, DEFAULT_FOCAL_DIST, 50.0f);
		//vr::HmdMatrix44_t projRight = g_pHMD->GetProjectionMatrix(vr::EVREye::Eye_Right, DEFAULT_FOCAL_DIST, 50.0f);

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
	g_bSteamVRInitialized = result;

	if (strDriver != NULL) {
		delete[] strDriver;
		strDriver = NULL;
	}
	if (strDisplay != NULL) {
		delete[] strDisplay;
		strDisplay = NULL;
	}
	if (file != NULL) {
		fclose(file);
		file = NULL;
	}
	return result;
}

void ShutDownSteamVR() {
	// If the user sets g_bSteamVRPosFromFreePIE when XWA is starting, they may
	// also reload the vrparams and then unset this flag. In that case, we won't
	// shut FreePIE down. Well, this is a private hack, so I'm not going to care
	// about that case.
	if (g_bSteamVRPosFromFreePIE)
		ShutdownFreePIE();

	// We can't shut down SteamVR twice, we either shut it down here or in the cockpit look hook.
	// It looks like the right order is to shut SteamVR down in the cockpit look hook
	return;
	//log_debug("[DBG] Not shutting down SteamVR, just returning...");
	//return;
	/*
	log_debug("[DBG] Shutting down SteamVR...");
	vr::VR_Shutdown();
	g_pHMD = NULL;
	g_pVRCompositor = NULL;
	g_pVRScreenshots = NULL;
	log_debug("[DBG] SteamVR shut down");
	*/
}

bool InitDirectSBS()
{
	InitFreePIE();
	g_headCenter.set(0, 0, 0);

	g_EyeMatrixLeftInv.set
	(
		1.0f, 0.0f, 0.0f, g_fHalfIPD,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	// Matrices are stored in column-major format, so we have to transpose them to
	// match the format above:
	g_EyeMatrixLeftInv.transpose();

	g_EyeMatrixRightInv.set
	(
		1.0f, 0.0f, 0.0f, -g_fHalfIPD,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	// Matrices are stored in column-major format, so we have to transpose them to
	// match the format above:
	g_EyeMatrixRightInv.transpose();

	g_projLeft.set
	(
		0.847458f, 0.0f,       0.0f,  0.0f,
		0.0f,      0.746269f,  0.0f,  0.0f,
		0.0f,      0.0f,      -1.0f, -0.01f, // Use the focal_dist here?
		0.0f,      0.0f,      -1.0f,  0.0f
	);
	g_projLeft.transpose();
	g_projRight = g_projLeft;

	g_fullMatrixLeft  = g_projLeft  * g_EyeMatrixLeftInv;
	g_fullMatrixRight = g_projRight * g_EyeMatrixRightInv;

	Matrix4 g_targetCompView
	(
		3.0f, 0.0f, 0.0f,  0.0f,
		0.0f, 6.0f, 0.0f,  2.0f,
		0.0f, 0.0f, 1.0f,  0.0f,
		0.0f, 0.0f, 0.0f,  1.0f
	);
	g_targetCompView.transpose();
	g_fullMatrixHead  = g_projLeft * g_targetCompView; // The center matrix does not have eye parallax

	//ShowMatrix4(g_EyeMatrixLeftInv, "g_EyeMatrixLeftInv");
	//ShowMatrix4(g_projLeft, "g_projLeft");
	//ShowMatrix4(g_EyeMatrixRightInv, "g_EyeMatrixRightInv");
	//ShowMatrix4(g_projRight, "g_projRight");
	log_debug("[DBG] DirectSBS mode initialized");
	g_bDirectSBSInitialized = true;
	return true;
}

bool ShutDownDirectSBS() {
	ShutdownFreePIE();
	return true;
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
			return D3D11_BLEND_ZERO; // 1
		case D3DBLEND_ONE:
			return D3D11_BLEND_ONE; // 2
		case D3DBLEND_SRCCOLOR:
			return D3D11_BLEND_SRC_COLOR; // 3
		case D3DBLEND_INVSRCCOLOR:
			return D3D11_BLEND_INV_SRC_COLOR; // 4
		case D3DBLEND_SRCALPHA:
			return D3D11_BLEND_SRC_ALPHA; // 5
		case D3DBLEND_INVSRCALPHA:
			return D3D11_BLEND_INV_SRC_ALPHA; // 6
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
			return D3D11_COMPARISON_LESS;
		case D3DCMP_EQUAL:
			return D3D11_COMPARISON_EQUAL;
		case D3DCMP_LESSEQUAL:
			return D3D11_COMPARISON_LESS_EQUAL;
		case D3DCMP_GREATER:
			return D3D11_COMPARISON_GREATER;
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

public: // HACK: Return this to private after the Dynamic Cockpit is stable
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
	//g_iNumVertices = 0;
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

#ifdef DBG_VR
void DumpOrigVertices(FILE *file, int numVerts)
{
	char buf[256];
	float px, py, pz, rhw;

	// Don't catpure things we are not drawing
	if (g_iDrawCounter < g_iNoDrawBeforeIndex)
		return;
	if (g_iNoDrawAfterIndex > -1 && g_iDrawCounter > g_iNoDrawAfterIndex)
		return;

	for (register int i = 0; i < numVerts; i++) {
		px = g_OrigVerts[i].sx;
		py = g_OrigVerts[i].sy;
		pz = g_OrigVerts[i].sz;
		rhw = g_OrigVerts[i].rhw;

		// 1/rhw is the linear Z depth times a scale factor
		sprintf_s(buf, 256, "%f %f %f %f %f\n", px, py, pz, rhw, 1.0f/rhw);
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
 * This function is not used anymore and the projection is done in the vertex shader
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
	x =  PX[0];
	y = -PX[1];
	z =  PX[2];
}

/* 
 * This function was originally used to reconstruct 3D. Now, most of this logic is in the 
 * SBSVertexShader. We actually don't need this function at all, and at some point it will
 * be moved entirely to the vertex shader. At that point, we can get rid of g_3DVerts too and
 * the logic associated with it.
 */
/*
void PreprocessVerticesStereo(float width, float height, int numVerts)
{
	// Pre-process vertices for Stereo
	float px, py;
	//bool is_cockpit;
	float scale_x = 1.0f / width;
	float scale_y = 1.0f / height;
	//float w;
	//float scale = scale_x;
	//bool is_GUI = false;

	// Back-project and do stereo
	for (register int i = 0; i < numVerts; i++) {
		g_3DVerts[i] = g_OrigVerts[i];
		// Normalize the coords: move the screen's center to (0,0) and scale the (x,y) axes
		// to -0.5..0.5
		px = g_OrigVerts[i].sx * scale_x - 0.5f;
		py = g_OrigVerts[i].sy * scale_y - 0.5f;
		// Also invert the Z axis so that z = 0 is the screen plane and z = 1 is ZFar, the original
		// values have ZFar = 0, and ZNear = 1
		//direct_pz = g_OrigVerts[i].sz;
		//pz = 1.0f - direct_pz;

		// GUI elements seem to be in the range 0..0.0005, so 0.0006 sounds like a good threshold:
		//is_GUI = (pz <= g_fGUIElemPZThreshold);
		//is_cockpit = (pz <= g_fCockpitPZThreshold);

		g_3DVerts[i].sx = px;
		g_3DVerts[i].sy = py;
	}
	*/
		// Reproject back into 2D space
		//if (is_GUI) {
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
			//g_3DVerts[i].sz = g_fFloatingGUIParallax;
			//g_3DVerts[i].sz += 0.9f;
		//} 
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
	//} // Original end of the main for-loop

#ifdef DBG_VR
	// DBG: Hack: Dump the 3D scene. Triggered with Ctrl-Alt-C
	if (g_bDo3DCapture)
	{
		if (g_HackFile == NULL)
			fopen_s(&g_HackFile, "./vertexbuf.obj", "wt");
		DumpOrigVertices(g_HackFile, numVerts);
	}
#endif
//}

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

// Should this function be inlined?
// See:
// xwa_ddraw_d3d11-sm4\impl11\ddraw-May-6-2019-Functionality-Complete\Direct3DDevice.cpp
// For a working implementation of the index buffer read (spoiler alert: it's the same we have here)
void Direct3DDevice::GetBoundingBox(LPD3DINSTRUCTION instruction, UINT curIndex,
	float *minX, float *minY, float *maxX, float *maxY, bool debug) {
	LPD3DTRIANGLE triangle = (LPD3DTRIANGLE)(instruction + 1);
	D3DTLVERTEX v;
	WORD index;
	//int aux_idx = curIndex;
	float px, py;
	*maxX = -1; *maxY = -1;
	*minX = 1000000; *minY = 1000000;
	for (WORD i = 0; i < instruction->wCount; i++)
	{
		index = triangle->v1;
		px = g_OrigVerts[index].sx; py = g_OrigVerts[index].sy;
		if (px < *minX) *minX = px; if (px > *maxX) *maxX = px;
		if (py < *minY) *minY = py; if (py > *maxY) *maxY = py;
		if (debug) {
			v = g_OrigVerts[index];
			log_debug("[DBG] sx: %0.6f, sy: %0.6f, sz: %0.6f, rhw: %0.6f, tu: %0.3f, tv: %0.3f", v.sx, v.sy, v.sz, v.rhw, v.tu, v.tv);
		}

		index = triangle->v2;
		px = g_OrigVerts[index].sx; py = g_OrigVerts[index].sy;
		if (px < *minX) *minX = px; if (px > *maxX) *maxX = px;
		if (py < *minY) *minY = py; if (py > *maxY) *maxY = py;
		if (debug) {
			v = g_OrigVerts[index];
			log_debug("[DBG] sx: %0.6f, sy: %0.6f, sz: %0.6f, rhw: %0.6f, tu: %0.3f, tv: %0.3f", v.sx, v.sy, v.sz, v.rhw, v.tu, v.tv);
		}

		index = triangle->v3;
		px = g_OrigVerts[index].sx; py = g_OrigVerts[index].sy;
		if (px < *minX) *minX = px; if (px > *maxX) *maxX = px;
		if (py < *minY) *minY = py; if (py > *maxY) *maxY = py;
		if (debug) {
			v = g_OrigVerts[index];
			log_debug("[DBG] sx: %0.6f, sy: %0.6f, sz: %0.6f, rhw: %0.6f, tu: %0.3f, tv: %0.3f", v.sx, v.sy, v.sz, v.rhw, v.tu, v.tv);
		}

		triangle++;
	}
}

void Direct3DDevice::GetBoundingBoxUVs(LPD3DINSTRUCTION instruction, UINT curIndex,
	float *minX, float *minY, float *maxX, float *maxY, 
	float *minU, float *minV, float *maxU, float *maxV,
	bool debug) {
	LPD3DTRIANGLE triangle = (LPD3DTRIANGLE)(instruction + 1);
	D3DTLVERTEX vert;
	WORD index;
	float px, py, u, v;
	*maxX = -1000000; *maxY = -1000000;
	*minX =  1000000; *minY =  1000000;
	*minU =  100; *minV =  100;
	*maxU = -100; *maxV = -100;
	if (debug)
		log_debug("[DBG] START Geom");
	for (WORD i = 0; i < instruction->wCount; i++)
	{
		index = triangle->v1;
		px = g_OrigVerts[index].sx; py = g_OrigVerts[index].sy;
		u  = g_OrigVerts[index].tu; v  = g_OrigVerts[index].tv;
		if (px < *minX) *minX = px; if (px > *maxX) *maxX = px;
		if (py < *minY) *minY = py; if (py > *maxY) *maxY = py;
		if (u < *minU) *minU = u; if (u > *maxU) *maxU = u;
		if (v < *minV) *minV = v; if (v > *maxV) *maxV = v;
		if (debug) {
			vert = g_OrigVerts[index];
			log_debug("[DBG] sx: %0.6f, sy: %0.6f, sz: %0.6f, rhw: %0.6f, tu: %0.3f, tv: %0.3f", vert.sx, vert.sy, vert.sz, vert.rhw, vert.tu, vert.tv);
		}

		index = triangle->v2;
		px = g_OrigVerts[index].sx; py = g_OrigVerts[index].sy;
		u  = g_OrigVerts[index].tu; v  = g_OrigVerts[index].tv;
		if (px < *minX) *minX = px; if (px > *maxX) *maxX = px;
		if (py < *minY) *minY = py; if (py > *maxY) *maxY = py;
		if (u < *minU) *minU = u; if (u > *maxU) *maxU = u;
		if (v < *minV) *minV = v; if (v > *maxV) *maxV = v;
		if (debug) {
			vert = g_OrigVerts[index];
			log_debug("[DBG] sx: %0.6f, sy: %0.6f, sz: %0.6f, rhw: %0.6f, tu: %0.3f, tv: %0.3f", vert.sx, vert.sy, vert.sz, vert.rhw, vert.tu, vert.tv);
		}

		index = triangle->v3;
		px = g_OrigVerts[index].sx; py = g_OrigVerts[index].sy;
		u  = g_OrigVerts[index].tu; v  = g_OrigVerts[index].tv;
		if (px < *minX) *minX = px; if (px > *maxX) *maxX = px;
		if (py < *minY) *minY = py; if (py > *maxY) *maxY = py;
		if (u < *minU) *minU = u; if (u > *maxU) *maxU = u;
		if (v < *minV) *minV = v; if (v > *maxV) *maxV = v;
		if (debug) {
			vert = g_OrigVerts[index];
			log_debug("[DBG] sx: %0.6f, sy: %0.6f, sz: %0.6f, rhw: %0.6f, tu: %0.3f, tv: %0.3f", vert.sx, vert.sy, vert.sz, vert.rhw, vert.tu, vert.tv);
		}
		triangle++;
	}
	if (debug)
		log_debug("[DBG] END Geom");
}

/*
void Direct3DDevice::ClearBox(Box box, D3D11_VIEWPORT *viewport, bool fullScreen, float scale, D3DCOLOR clearColor) {
	HRESULT hr;
	auto& resources = _deviceResources;
	auto& device = resources->_d3dDevice;
	auto& context = resources->_d3dDeviceContext;

	// Set the constant buffers
	VertexShaderCBuffer tempVSBuffer = g_VSCBuffer;
	tempVSBuffer.viewportScale[0] =  2.0f / resources->_displayWidth;
	tempVSBuffer.viewportScale[1] = -2.0f / resources->_displayHeight;
	tempVSBuffer.viewportScale[2] =  scale;
	tempVSBuffer.viewportScale[3] =  g_fGlobalScale;
	resources->InitVSConstantBuffer3D(resources->_VSConstantBuffer.GetAddressOf(), &tempVSBuffer);

	// Set the vertex buffer
	// D3DCOLOR seems to be AARRGGBB
	if (!fullScreen) {
		D3DTLVERTEX vertices[6];
		vertices[0].sx = box.left;  vertices[0].sy = box.top;    vertices[0].sz = 0.98f; vertices[0].rhw = 34.0f; vertices[0].color = clearColor;
		vertices[1].sx = box.right; vertices[1].sy = box.top;    vertices[1].sz = 0.98f; vertices[1].rhw = 34.0f; vertices[1].color = clearColor;
		vertices[2].sx = box.right; vertices[2].sy = box.bottom; vertices[2].sz = 0.98f; vertices[2].rhw = 34.0f; vertices[2].color = clearColor;

		vertices[3].sx = box.right; vertices[3].sy = box.bottom; vertices[3].sz = 0.98f; vertices[3].rhw = 34.0f; vertices[3].color = clearColor;
		vertices[4].sx = box.left;  vertices[4].sy = box.bottom; vertices[4].sz = 0.98f; vertices[4].rhw = 34.0f; vertices[4].color = clearColor;
		vertices[5].sx = box.left;  vertices[5].sy = box.top;    vertices[5].sz = 0.98f; vertices[5].rhw = 34.0f; vertices[5].color = clearColor;
		D3D11_MAPPED_SUBRESOURCE map;
		hr = context->Map(g_ClearHUDVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
		if (SUCCEEDED(hr)) {
			memcpy(map.pData, vertices, sizeof(D3DTLVERTEX) * 6);
			context->Unmap(g_ClearHUDVertexBuffer, 0);
		}
	}
	
	D3D11_DEPTH_STENCIL_DESC currentStencilDesc = _renderStates->GetDepthStencilDesc();
	D3D11_DEPTH_STENCIL_DESC desc;

	// Temporarily disable ZWrite: we won't need it to display the HUD
	ComPtr<ID3D11DepthStencilState> depthState;
	desc.DepthEnable = FALSE;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	desc.StencilEnable = FALSE;
	resources->InitDepthStencilState(depthState, &desc);

	// Change the shaders
	resources->InitVertexShader(resources->_vertexShader);
	resources->InitPixelShader(resources->_pixelShaderSolid);
	// Change the render target
	context->OMSetRenderTargets(1, resources->_renderTargetViewDynCockpitAsInput.GetAddressOf(), NULL);
	// Set the viewport
	resources->InitViewport(viewport);
	// Set the vertex buffer (map the vertices from the box)
	UINT stride = sizeof(D3DTLVERTEX);
	UINT offset = 0;
	if (!fullScreen)
		resources->InitVertexBuffer(&g_ClearHUDVertexBuffer, &stride, &offset);
	else
		resources->InitVertexBuffer(&g_ClearFullScreenHUDVertexBuffer, &stride, &offset);
	// Draw
	context->Draw(6, 0);

	// Restore the constant buffers
	resources->InitVSConstantBuffer3D(resources->_VSConstantBuffer.GetAddressOf(), &g_VSCBuffer);
	// Restore the depth stencil state
	resources->InitDepthStencilState(depthState, &currentStencilDesc);
	// Restore the vertex and pixel shaders
	if (g_bEnableVR)
		this->_deviceResources->InitVertexShader(resources->_sbsVertexShader);
	else
		// The original code used _vertexShader:
		this->_deviceResources->InitVertexShader(resources->_vertexShader);
	this->_deviceResources->InitPixelShader(resources->_pixelShaderTexture);
	// Restore the vertex buffer
	UINT vertexBufferStride = sizeof(D3DTLVERTEX), vertexBufferOffset = 0;
	resources->InitVertexBuffer(this->_vertexBuffer.GetAddressOf(), &vertexBufferStride, &vertexBufferOffset);
	// We don't need to restore the viewport, it gets set before each draw call anyway
}
*/

inline void InGameToScreenCoords(UINT left, UINT top, UINT width, UINT height, float x, float y, float *x_out, float *y_out)
{
	*x_out = left + x / g_fCurInGameWidth  * width;
	*y_out = top  + y / g_fCurInGameHeight * height;
}

inline float lerp(float x, float y, float s) {
	return x + s * (y - x);
}

/*
 * Converts normalized uv_coords with respect to a HUD texture into actual pixel
 * coordinates.
 */
Box ComputeCoordsFromUV(UINT left, UINT top, UINT width, UINT height,
	Box uv_minmax, Box HUD_coords, const Box uv_coords) {
	Box result = { 0 };
	// These are coordinates in pixels in texture space
	float u0 = uv_coords.x0;
	float v0 = uv_coords.y0;
	float u1 = uv_coords.x1;
	float v1 = uv_coords.y1;
	// We expect normalized uv coords here
	// Now we need to interpolate, if u == minU --> minX, if u == maxU --> maxX
	result.x0 = lerp(HUD_coords.x0, HUD_coords.x1, (u0 - uv_minmax.x0) / (uv_minmax.x1 - uv_minmax.x0));
	result.y0 = lerp(HUD_coords.y0, HUD_coords.y1, (v0 - uv_minmax.y0) / (uv_minmax.y1 - uv_minmax.y0));
	result.x1 = lerp(HUD_coords.x0, HUD_coords.x1, (u1 - uv_minmax.x0) / (uv_minmax.x1 - uv_minmax.x0));
	result.y1 = lerp(HUD_coords.y0, HUD_coords.y1, (v1 - uv_minmax.y0) / (uv_minmax.y1 - uv_minmax.y0));
	// Normalize to uv coords wrt screen space:
	result.x0 /= g_fCurScreenWidth; result.y0 /= g_fCurScreenHeight;
	result.x1 /= g_fCurScreenWidth; result.y1 /= g_fCurScreenHeight;
	return result;
}

void ComputeCoordsFromUV(UINT left, UINT top, UINT width, UINT height,
	Box uv_minmax, Box HUD_coords, const Box uv_coords, uvfloat4 *result) {
	Box box = ComputeCoordsFromUV(left, top, width, height,
		uv_minmax, HUD_coords, uv_coords);
	result->x0 = box.x0; result->y0 = box.y0;
	result->x1 = box.x1; result->y1 = box.y1;
}

void DisplayBox(char *name, Box box) {
	log_debug("[DBG] %s (%0.3f, %0.3f)-(%0.3f, %0.3f)", name,
		box.x0, box.y0, box.x1, box.y1);
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
	//auto& context1 = resources->_d3dDeviceContext1;

	HRESULT hr = S_OK;
	UINT width, height, left, top;
	float scale;
	UINT vertexBufferStride = sizeof(D3DTLVERTEX), vertexBufferOffset = 0;
	D3D11_VIEWPORT viewport;
	D3D11_MAPPED_SUBRESOURCE map;
	bool bModifiedShaders = false, bZWriteEnabled = false;

	g_VSCBuffer = { 0 };
	g_VSCBuffer.aspect_ratio = g_bRendering3D ? g_fAspectRatio : g_fConcourseAspectRatio;
	g_VSCBuffer.z_override = -1.0f;
	g_VSCBuffer.sz_override = -1.0f;
	g_VSCBuffer.mult_z_override = -1.0f;
	g_VSCBuffer.cockpit_threshold = g_fGUIElemPZThreshold;
	g_VSCBuffer.bPreventTransform = 0.0f;
	g_VSCBuffer.bFullTransform = 0.0f;

	g_PSCBuffer = { 0 };
	g_PSCBuffer.brightness       = MAX_BRIGHTNESS;
	g_PSCBuffer.ct_brightness	 = g_fCoverTextureBrightness;
	//g_PSCBuffer.DynCockpitSlots  = 0;
	//g_PSCBuffer.bUseCoverTexture = 0;
	//g_PSCBuffer.bRenderHUD		 = 0;
	//g_PSCBuffer.bEnhaceLasers	 = 0;
	//g_PSCBuffer.bAlphaOnly		 = 0;

	// Save the current viewMatrix: if the Dynamic Cockpit is enabled, we'll need it later to restore the transform
	//Matrix4 currentViewMat = g_VSMatrixCB.viewMat;
	//bool bModifiedViewMatrix = false;
	
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

	int numVerts = executeBuffer->_executeData.dwVertexCount;
	size_t vertsLength = sizeof(D3DTLVERTEX) * numVerts;
	g_OrigVerts = (D3DTLVERTEX *)executeBuffer->_buffer;
	float displayWidth  = (float)resources->_displayWidth;
	float displayHeight = (float)resources->_displayHeight;

	// Copy the vertex data to the vertexbuffers
	step = "VertexBuffer";
	hr = context->Map(this->_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
	if (SUCCEEDED(hr))
	{
		memcpy(map.pData, g_OrigVerts, vertsLength);
		context->Unmap(this->_vertexBuffer, 0);
	}
	resources->InitVertexBuffer(this->_vertexBuffer.GetAddressOf(), &vertexBufferStride, &vertexBufferOffset);

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
			width  = resources->_backbufferWidth;
			height = resources->_backbufferHeight;

			if (!g_bOverrideAspectRatio) {
				float original_aspect = (float)this->_deviceResources->_displayWidth / (float)this->_deviceResources->_displayHeight;
				float actual_aspect = (float)this->_deviceResources->_backbufferWidth / (float)this->_deviceResources->_backbufferHeight;
				g_fAspectRatio = actual_aspect / original_aspect;
			}
		}

		left = (this->_deviceResources->_backbufferWidth - width) / 2;
		top = (this->_deviceResources->_backbufferHeight - height) / 2;

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

		if (g_bEnableVR) {
			g_VSCBuffer.viewportScale[0] = 1.0f / displayWidth;
			g_VSCBuffer.viewportScale[1] = 1.0f / displayHeight;
		} else {
			g_VSCBuffer.viewportScale[0] =  2.0f / displayWidth;
			g_VSCBuffer.viewportScale[1] = -2.0f / displayHeight;
		}
		g_VSCBuffer.viewportScale[2] = scale;
		g_VSCBuffer.viewportScale[3] = g_fGlobalScale;
		// If we're rendering to the Tech Library, then we should use the Concourse Aspect Ratio
		g_VSCBuffer.aspect_ratio = g_bRendering3D ? g_fAspectRatio : g_fConcourseAspectRatio;
		g_VSCBuffer.cockpit_threshold = g_fCockpitPZThreshold;
		g_VSCBuffer.z_override = -1.0f;
		g_VSCBuffer.sz_override = -1.0f;
		g_VSCBuffer.mult_z_override = -1.0f;
		resources->InitVSConstantBuffer3D(resources->_VSConstantBuffer.GetAddressOf(), &g_VSCBuffer);
		resources->InitPSConstantBuffer3D(resources->_PSConstantBuffer.GetAddressOf(), &g_PSCBuffer);
		//g_fXWAScale = scale; // Store the current scale, the Dynamic Cockpit will use this value

		/*
		// This is the original monoscopic viewport:
		viewport.TopLeftX = (float)left;
		viewport.TopLeftY = (float)top;
		viewport.Width    = (float)width;
		viewport.Height   = (float)height;
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
				int aux_idx = 0;

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
							texture->_refCount++;
							//if (texture->is_RebelLaser)
							//	context->PSSetShaderResources(0, 1, &g_RebelLaser);
							//else
							context->PSSetShaderResources(0, 1, texture->_textureView.GetAddressOf());
							texture->_refCount--;

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
				if (FAILED(hr = this->_deviceResources->InitBlendState(nullptr, &blendDesc))) {
					log_debug("[DBG] Caught error at Execute:BlendState");
					hr = device->GetDeviceRemovedReason();
					log_debug("[DBG] Reason: 0x%x", hr);
					break;
				}

				step = "DepthStencilState";
				D3D11_DEPTH_STENCIL_DESC depthDesc = this->_renderStates->GetDepthStencilDesc();
				if (FAILED(hr = this->_deviceResources->InitDepthStencilState(nullptr, &depthDesc)))
					break;

				// Capture the non-VR viewport that is used with the non-VR vertexshader:
				g_nonVRViewport.TopLeftX = (float)left;
				g_nonVRViewport.TopLeftY = (float)top;
				g_nonVRViewport.Width    = (float)width;
				g_nonVRViewport.Height   = (float)height;
				g_nonVRViewport.MinDepth = D3D11_MIN_DEPTH;
				g_nonVRViewport.MaxDepth = D3D11_MAX_DEPTH;

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
					  - g_bPrevIsFloatingGUI3DObject && g_bIsFloating3DObject are set to false

					We're using a non-exhaustive list of GUI CRCs to tell when the 3D content has finished drawing.
				*/

				/* ZWriteEnabled is false when rendering the background starfield or when
				 * rendering the GUI elements -- except that the targetting computer GUI
				 * is rendered with ZWriteEnabled == true. I wonder why? */
				bZWriteEnabled = this->_renderStates->GetZWriteEnabled();

				/* If we have drawn at least one Floating GUI element and now the ZWrite has been enabled
				   again, then we're about to draw the floating 3D element. Although, g_bTargetCompDrawn
				   isn't fully semantically correct because it should be set to true *after* it has actually
				   been drawn. Here it's being set *before* it's drawn. */
				if (!g_bTargetCompDrawn && g_iFloatingGUIDrawnCounter > 0 && bZWriteEnabled)
					g_bTargetCompDrawn = true;
				// bIsNoZWrite is true if ZWrite is disabled and the SkyBox has been rendered.
				bool bIsNoZWrite = !bZWriteEnabled && g_iExecBufCounter > g_iSkyBoxExecIndex;
				// bIsSkyBox is true if we're about to render the SkyBox
				bool bIsSkyBox = !bZWriteEnabled && g_iExecBufCounter <= g_iSkyBoxExecIndex;
				g_bIsTrianglePointer = lastTextureSelected != NULL && lastTextureSelected->is_TrianglePointer;
				bool bIsText = lastTextureSelected != NULL && lastTextureSelected->is_Text;
				bool bIsHUD = lastTextureSelected != NULL && lastTextureSelected->is_HUD;
				bool bIsGUI = lastTextureSelected != NULL && lastTextureSelected->is_GUI;
				// In the hangar, shadows are enabled. Shadows don't have a texture and are rendered with
				// ZWrite disabled. So, how can we tell if a bracket is being rendered or a shadow?
				// Brackets are rendered with ZFunc D3DCMP_ALWAYS (8),
				// Shadows are rendered with ZFunc D3DCMP_GREATEREQUAL (7)
				// Cockpit glass & Engine Glow is rendered with ZFunc D3DCMP_GREATER (5)
				bool bIsBracket = bIsNoZWrite && lastTextureSelected == NULL && 
					this->_renderStates->GetZFunc() == D3DCMP_ALWAYS;
				bool bIsFloatingGUI = lastTextureSelected != NULL && lastTextureSelected->is_Floating_GUI;
				bool bIsTranspOrGlow = bIsNoZWrite && _renderStates->GetZFunc() == D3DCMP_GREATER;
				// Hysteresis detection (state is about to switch to render something different, like the HUD)
				g_bPrevIsFloatingGUI3DObject = g_bIsFloating3DObject;
				g_bIsFloating3DObject = g_bTargetCompDrawn && lastTextureSelected != NULL &&
					!lastTextureSelected->is_Text && !lastTextureSelected->is_TrianglePointer &&
					!lastTextureSelected->is_HUD && !lastTextureSelected->is_Floating_GUI &&
					!lastTextureSelected->is_TargetingComp;
				// The GUI starts rendering whenever we detect a GUI element, or Text, or a bracket.
				g_bPrevStartedGUI = g_bStartedGUI;
				g_bStartedGUI |= bIsGUI || bIsText || bIsBracket || bIsFloatingGUI;
				// bIsScaleableGUIElem is true when we're about to render a HUD element that can be scaled down with Ctrl+Z
				g_bPrevIsScaleableGUIElem = g_bIsScaleableGUIElem;
				g_bIsScaleableGUIElem = g_bStartedGUI && !bIsHUD && !bIsBracket && !g_bIsTrianglePointer;
				
				// lastTextureSelected can be NULL. This happens when drawing the square
				// brackets around the currently-selected object (and maybe other situations)

				if (g_bReshadeEnabled && !g_bPrevStartedGUI && g_bStartedGUI) {
					// We're about to start rendering *ALL* the GUI: including the triangle pointer and text
					// This is where we can capture the current frame for post-processing effects
					context->ResolveSubresource(resources->_offscreenBufferAsInputReshade, 0,
						resources->_offscreenBuffer, 0, DXGI_FORMAT_B8G8R8A8_UNORM);
				}

				if (!g_bPrevIsScaleableGUIElem && g_bIsScaleableGUIElem && !g_bScaleableHUDStarted) {
					g_bScaleableHUDStarted = true;
					g_iDrawCounterAfterHUD = 0;
					// HACK
					//*g_playerInHangar = 1;
					// We're about to render the scaleable HUD, time to clear the dynamic cockpit texture
					if (g_bDynCockpitEnabled) {
						float bgColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
						context->ClearRenderTargetView(resources->_renderTargetViewDynCockpit, bgColor);
						context->ClearRenderTargetView(resources->_renderTargetViewDynCockpitBG, bgColor);
						// I think we need to clear the depth buffer here so that the targeted craft is drawn properly
						context->ClearDepthStencilView(this->_deviceResources->_depthStencilViewL,
							D3D11_CLEAR_DEPTH, resources->clearDepth, 0);
					}
				}
				bool bRenderToDynCockpitBuffer = g_bDynCockpitEnabled && lastTextureSelected != NULL &&
					g_bScaleableHUDStarted && g_bIsScaleableGUIElem;
				bool bRenderToDynCockpitBGBuffer = false;

				/*************************************************************************
					State management ends here
				 *************************************************************************/

				/*
				if (lastTextureSelected != NULL && lastTextureSelected->is_RebelLaser) {
					ID3D11Resource *res = NULL;
					static int counter = 0;
					wchar_t name[120];
					swprintf_s(name, 120, L"c:\\Temp\\RebelLaser%d.png", counter);
					lastTextureSelected->is_RebelLaser = false;
					lastTextureSelected->_textureView->GetResource(&res);					
					hr = DirectX::SaveWICTextureToFile(context.Get(),
						res, GUID_ContainerFormatPng, name);
					counter++;
				}
				*/

				//if (PlayerDataTable[0].cockpitDisplayed)
				//if (PlayerDataTable[0].cockpitDisplayed2)
				//	goto out;
	
				// Capture the bounds for the left radar:
				if (g_bDynCockpitEnabled && lastTextureSelected != NULL)
				{
					if (lastTextureSelected->is_DC_LeftSensorSrc)
					{
						if (!g_DCHUDBoxes.boxes[LEFT_RADAR_HUD_BOX_IDX].bLimitsComputed)
						{
							DCHUDBox *dcSrcBox = &g_DCHUDBoxes.boxes[LEFT_RADAR_HUD_BOX_IDX];
							DCElemSrcBox *dcElemSrcBox = NULL;
							float minX, minY, maxX, maxY;
							Box uv_minmax = { 0 };
							Box box;
							GetBoundingBoxUVs(instruction, currentIndexLocation, &minX, &minY, &maxX, &maxY,
								&uv_minmax.x0, &uv_minmax.y0, &uv_minmax.x1, &uv_minmax.y1);
							InGameToScreenCoords(left, top, width, height, minX, minY, &box.x0, &box.y0);
							InGameToScreenCoords(left, top, width, height, maxX, maxY, &box.x1, &box.y1);
							// Store the pixel coordinates
							dcSrcBox->coords = box;
							// Compute and store the erase coordinates for this HUD Box
							ComputeCoordsFromUV(left, top, width, height, uv_minmax, box,
								dcSrcBox->uv_erase_coords, &dcSrcBox->erase_coords);
							dcSrcBox->bLimitsComputed = true;

							// Get the limits for the left radar:
							dcElemSrcBox = &g_DCElemSrcBoxes.src_boxes[LEFT_RADAR_DC_ELEM_SRC_IDX];
							dcElemSrcBox->coords = ComputeCoordsFromUV(left, top, width, height, 
								uv_minmax, box,	dcElemSrcBox->uv_coords);
							dcElemSrcBox->bComputed = true;

							// Get the limits for the laser recharge rate:
							dcElemSrcBox = &g_DCElemSrcBoxes.src_boxes[LASER_RECHARGE_DC_ELEM_SRC_IDX];
							dcElemSrcBox->coords = ComputeCoordsFromUV(left, top, width, height,
								uv_minmax, box, dcElemSrcBox->uv_coords);
							dcElemSrcBox->bComputed = true;

							// Get the limits for the shield recharge rate:
							dcElemSrcBox = &g_DCElemSrcBoxes.src_boxes[SHIELD_RECHARGE_DC_ELEM_SRC_IDX];
							dcElemSrcBox->coords = ComputeCoordsFromUV(left, top, width, height,
								uv_minmax, box, dcElemSrcBox->uv_coords);
							dcElemSrcBox->bComputed = true;

							
							//Box elem_coords = g_DCElemSrcBoxes.src_boxes[LEFT_RADAR_DC_ELEM_SRC_IDX].coords;
							//log_debug("[DBG] [DC] Left Radar HUD screen coords: (%0.3f, %0.3f)-(%0.3f, %0.3f)",
							//	box.x0, box.y0, box.x1, box.y1);
							//log_debug("[DBG] [DC] Left Radar ELEMENT screen coords: (%0.3f, %0.3f)-(%0.3f, %0.3f)",
							//	elem_coords.x0, elem_coords.y0, elem_coords.x1, elem_coords.y1);
							//uvfloat4 e = g_DCHUDBoxes.boxes[LEFT_RADAR_HUD_BOX_IDX].erase_coords;
							//log_debug("[DBG] [DC] Left Radar HUD erase coords: (%0.3f, %0.3f)-(%0.3f, %0.3f)",
							//	e.x0 * g_fCurScreenWidth, e.y0 * g_fCurScreenHeight,
							//	e.x1 * g_fCurScreenWidth, e.y1 * g_fCurScreenHeight);
						}
					}
				}

				// Capture the bounds for the right radar:
				if (g_bDynCockpitEnabled && lastTextureSelected != NULL)
				{
					if ((lastTextureSelected->is_DC_RightSensorSrc) ||
					    (lastTextureSelected->is_DC_RightSensor2Src))
					{
						if (!g_DCHUDBoxes.boxes[RIGHT_RADAR_HUD_BOX_IDX].bLimitsComputed)
						{
							DCHUDBox *dcSrcBox = &g_DCHUDBoxes.boxes[RIGHT_RADAR_HUD_BOX_IDX];
							DCElemSrcBox *dcElemSrcBox = NULL;
							float minX, minY, maxX, maxY;
							Box uv_minmax = { 0 };
							Box box;
							GetBoundingBoxUVs(instruction, currentIndexLocation, &minX, &minY, &maxX, &maxY,
								&uv_minmax.x0, &uv_minmax.y0, &uv_minmax.x1, &uv_minmax.y1);
							InGameToScreenCoords(left, top, width, height, minX, minY, &box.x0, &box.y0);
							InGameToScreenCoords(left, top, width, height, maxX, maxY, &box.x1, &box.y1);
							// Store the pixel coordinates
							dcSrcBox->coords = box;
							// Compute and store the erase coordinates for this HUD Box
							ComputeCoordsFromUV(left, top, width, height, uv_minmax, box,
								dcSrcBox->uv_erase_coords, &dcSrcBox->erase_coords);
							dcSrcBox->bLimitsComputed = true;

							// Get the limits for the right radar:
							dcElemSrcBox = &g_DCElemSrcBoxes.src_boxes[RIGHT_RADAR_DC_ELEM_SRC_IDX];
							dcElemSrcBox->coords = ComputeCoordsFromUV(left, top, width, height,
								uv_minmax, box, dcElemSrcBox->uv_coords);
							dcElemSrcBox->bComputed = true;

							// Get the limits for the engine recharge rate:
							dcElemSrcBox = &g_DCElemSrcBoxes.src_boxes[ENGINE_RECHARGE_DC_ELEM_SRC_IDX];
							dcElemSrcBox->coords = ComputeCoordsFromUV(left, top, width, height,
								uv_minmax, box, dcElemSrcBox->uv_coords);
							dcElemSrcBox->bComputed = true;

							// Get the limits for the beam recharge rate:
							dcElemSrcBox = &g_DCElemSrcBoxes.src_boxes[BEAM_RECHARGE_DC_ELEM_SRC_IDX];
							dcElemSrcBox->coords = ComputeCoordsFromUV(left, top, width, height,
								uv_minmax, box, dcElemSrcBox->uv_coords);
							dcElemSrcBox->bComputed = true;
						}
					}
				}
				
				// Capture the bounds for the shields:
				if (g_bDynCockpitEnabled && lastTextureSelected != NULL)
				{
					if (lastTextureSelected->is_DC_ShieldsSrc)
					{
						if (!g_DCHUDBoxes.boxes[SHIELDS_HUD_BOX_IDX].bLimitsComputed)
						{
							DCHUDBox *dcSrcBox = &g_DCHUDBoxes.boxes[SHIELDS_HUD_BOX_IDX];
							DCElemSrcBox *dcElemSrcBox = NULL;
							float minX, minY, maxX, maxY;
							Box uv_minmax = { 0 };
							Box box;
							GetBoundingBoxUVs(instruction, currentIndexLocation, &minX, &minY, &maxX, &maxY,
								&uv_minmax.x0, &uv_minmax.y0, &uv_minmax.x1, &uv_minmax.y1);
							InGameToScreenCoords(left, top, width, height, minX, minY, &box.x0, &box.y0);
							InGameToScreenCoords(left, top, width, height, maxX, maxY, &box.x1, &box.y1);
							// Store the pixel coordinates
							dcSrcBox->coords = box;
							// Compute and store the erase coordinates for this HUD Box
							ComputeCoordsFromUV(left, top, width, height, uv_minmax, box,
								dcSrcBox->uv_erase_coords, &dcSrcBox->erase_coords);
							dcSrcBox->bLimitsComputed = true;

							// Get the limits for the shields:
							dcElemSrcBox = &g_DCElemSrcBoxes.src_boxes[SHIELDS_DC_ELEM_SRC_IDX];
							dcElemSrcBox->coords = ComputeCoordsFromUV(left, top, width, height,
								uv_minmax, box, dcElemSrcBox->uv_coords);
							dcElemSrcBox->bComputed = true;
						}
					}
				}

				// Capture the bounds for the tractor beam:
				if (g_bDynCockpitEnabled && lastTextureSelected != NULL)
				{
					if (lastTextureSelected->is_DC_BeamBoxSrc)
					{
						if (!g_DCHUDBoxes.boxes[BEAM_HUD_BOX_IDX].bLimitsComputed)
						{
							DCHUDBox *dcSrcBox = &g_DCHUDBoxes.boxes[BEAM_HUD_BOX_IDX];
							DCElemSrcBox *dcElemSrcBox = NULL;
							float minX, minY, maxX, maxY;
							Box uv_minmax = { 0 };
							Box box;
							GetBoundingBoxUVs(instruction, currentIndexLocation, &minX, &minY, &maxX, &maxY,
								&uv_minmax.x0, &uv_minmax.y0, &uv_minmax.x1, &uv_minmax.y1);
							InGameToScreenCoords(left, top, width, height, minX, minY, &box.x0, &box.y0);
							InGameToScreenCoords(left, top, width, height, maxX, maxY, &box.x1, &box.y1);
							// Store the pixel coordinates
							dcSrcBox->coords = box;
							// Compute and store the erase coordinates for this HUD Box
							ComputeCoordsFromUV(left, top, width, height, uv_minmax, box,
								dcSrcBox->uv_erase_coords, &dcSrcBox->erase_coords);
							dcSrcBox->bLimitsComputed = true;

							// Get the limits for the shields:
							dcElemSrcBox = &g_DCElemSrcBoxes.src_boxes[BEAM_DC_ELEM_SRC_IDX];
							dcElemSrcBox->coords = ComputeCoordsFromUV(left, top, width, height,
								uv_minmax, box, dcElemSrcBox->uv_coords);
							dcElemSrcBox->bComputed = true;
						}
					}
				}

				// Capture the bounds for the targeting computer:
				if (g_bDynCockpitEnabled && lastTextureSelected != NULL)
				{
					if (lastTextureSelected->is_DC_TargetCompSrc)
					{
						if (!g_DCHUDBoxes.boxes[TARGET_HUD_BOX_IDX].bLimitsComputed)
						{
							DCHUDBox *dcSrcBox = &g_DCHUDBoxes.boxes[TARGET_HUD_BOX_IDX];
							DCElemSrcBox *dcElemSrcBox = NULL;
							float minX, minY, maxX, maxY;
							Box uv_minmax = { 0 };
							Box box;
							GetBoundingBoxUVs(instruction, currentIndexLocation, &minX, &minY, &maxX, &maxY,
								&uv_minmax.x0, &uv_minmax.y0, &uv_minmax.x1, &uv_minmax.y1);
							InGameToScreenCoords(left, top, width, height, minX, minY, &box.x0, &box.y0);
							InGameToScreenCoords(left, top, width, height, maxX, maxY, &box.x1, &box.y1);
							// Store the pixel coordinates
							dcSrcBox->coords = box;
							// Compute and store the erase coordinates for this HUD Box
							ComputeCoordsFromUV(left, top, width, height, uv_minmax, box,
								dcSrcBox->uv_erase_coords, &dcSrcBox->erase_coords);
							dcSrcBox->bLimitsComputed = true;

							// Get the limits for the targeting computer:
							dcElemSrcBox = &g_DCElemSrcBoxes.src_boxes[TARGET_COMP_DC_ELEM_SRC_IDX];
							dcElemSrcBox->coords = ComputeCoordsFromUV(left, top, width, height,
								uv_minmax, box, dcElemSrcBox->uv_coords);
							dcElemSrcBox->bComputed = true;

							// Get the limits for the quad lasers, left side:
							dcElemSrcBox = &g_DCElemSrcBoxes.src_boxes[QUAD_LASERS_L_DC_ELEM_SRC_IDX];
							dcElemSrcBox->coords = ComputeCoordsFromUV(left, top, width, height,
								uv_minmax, box, dcElemSrcBox->uv_coords);
							dcElemSrcBox->bComputed = true;

							// Get the limits for the quad lasers, left side:
							dcElemSrcBox = &g_DCElemSrcBoxes.src_boxes[QUAD_LASERS_R_DC_ELEM_SRC_IDX];
							dcElemSrcBox->coords = ComputeCoordsFromUV(left, top, width, height,
								uv_minmax, box, dcElemSrcBox->uv_coords);
							dcElemSrcBox->bComputed = true;

							// Get the limits for the quad lasers, both sides:
							dcElemSrcBox = &g_DCElemSrcBoxes.src_boxes[QUAD_LASERS_BOTH_DC_ELEM_SRC_IDX];
							dcElemSrcBox->coords = ComputeCoordsFromUV(left, top, width, height,
								uv_minmax, box, dcElemSrcBox->uv_coords);
							dcElemSrcBox->bComputed = true;

							// Get the limits for the dual lasers:
							dcElemSrcBox = &g_DCElemSrcBoxes.src_boxes[DUAL_LASERS_L_DC_ELEM_SRC_IDX];
							dcElemSrcBox->coords = ComputeCoordsFromUV(left, top, width, height,
								uv_minmax, box, dcElemSrcBox->uv_coords);
							dcElemSrcBox->bComputed = true;

							dcElemSrcBox = &g_DCElemSrcBoxes.src_boxes[DUAL_LASERS_R_DC_ELEM_SRC_IDX];
							dcElemSrcBox->coords = ComputeCoordsFromUV(left, top, width, height,
								uv_minmax, box, dcElemSrcBox->uv_coords);
							dcElemSrcBox->bComputed = true;

							dcElemSrcBox = &g_DCElemSrcBoxes.src_boxes[DUAL_LASERS_BOTH_DC_ELEM_SRC_IDX];
							dcElemSrcBox->coords = ComputeCoordsFromUV(left, top, width, height,
								uv_minmax, box, dcElemSrcBox->uv_coords);
							dcElemSrcBox->bComputed = true;

							// Get the limits for the B-Wing lasers
							dcElemSrcBox = &g_DCElemSrcBoxes.src_boxes[B_WING_LASERS_DC_ELEM_SRC_IDX];
							dcElemSrcBox->coords = ComputeCoordsFromUV(left, top, width, height,
								uv_minmax, box, dcElemSrcBox->uv_coords);
							dcElemSrcBox->bComputed = true;

							// Get the limits for the TIE-Defender lasers
							dcElemSrcBox = &g_DCElemSrcBoxes.src_boxes[SIX_LASERS_BOTH_DC_ELEM_SRC_IDX];
							dcElemSrcBox->coords = ComputeCoordsFromUV(left, top, width, height,
								uv_minmax, box, dcElemSrcBox->uv_coords);
							dcElemSrcBox->bComputed = true;

							dcElemSrcBox = &g_DCElemSrcBoxes.src_boxes[SIX_LASERS_L_DC_ELEM_SRC_IDX];
							dcElemSrcBox->coords = ComputeCoordsFromUV(left, top, width, height,
								uv_minmax, box, dcElemSrcBox->uv_coords);
							dcElemSrcBox->bComputed = true;

							dcElemSrcBox = &g_DCElemSrcBoxes.src_boxes[SIX_LASERS_R_DC_ELEM_SRC_IDX];
							dcElemSrcBox->coords = ComputeCoordsFromUV(left, top, width, height,
								uv_minmax, box, dcElemSrcBox->uv_coords);
							dcElemSrcBox->bComputed = true;

							// Other lasers/ion combinations to follow...
						}
					}
				}

				// Capture the bounds for the left/right message boxes:
				if (g_bDynCockpitEnabled && lastTextureSelected != NULL)
				{
					if (lastTextureSelected->is_DC_BorderMsgSrc ||
						lastTextureSelected->is_DC_SolidMsgSrc)
					{
						DCHUDBox *dcSrcBoxL = &g_DCHUDBoxes.boxes[LEFT_MSG_HUD_BOX_IDX];
						DCHUDBox *dcSrcBoxR = &g_DCHUDBoxes.boxes[RIGHT_MSG_HUD_BOX_IDX];
						if (!dcSrcBoxL->bLimitsComputed || !dcSrcBoxR->bLimitsComputed)
						{
							DCHUDBox *dcSrcBox = NULL;
							DCElemSrcBox *dcElemSrcBox = NULL;
							bool bLeft = false;
							float midX, minX, minY, maxX, maxY;
							Box uv_minmax = { 0 };
							Box box = { 0 };
							GetBoundingBoxUVs(instruction, currentIndexLocation, &minX, &minY, &maxX, &maxY,
								&uv_minmax.x0, &uv_minmax.y0, &uv_minmax.x1, &uv_minmax.y1);
							// This HUD is used for both the left and right message boxes, so we need to check
							// which one is being rendered
							midX = (minX + maxX) / 2.0f;
							if (midX < g_fCurInGameWidth / 2.0f && !dcSrcBoxL->bLimitsComputed) {
								bLeft = true;
								dcSrcBox = dcSrcBoxL;
								dcElemSrcBox = &g_DCElemSrcBoxes.src_boxes[LEFT_MSG_DC_ELEM_SRC_IDX];
							} else if (midX > g_fCurInGameWidth / 2.0f && !dcSrcBoxR->bLimitsComputed) {
								bLeft = false;
								dcSrcBox = dcSrcBoxR;
								dcElemSrcBox = &g_DCElemSrcBoxes.src_boxes[RIGHT_MSG_DC_ELEM_SRC_IDX];
							}

							if (dcSrcBox != NULL) {
								InGameToScreenCoords(left, top, width, height, minX, minY, &box.x0, &box.y0);
								InGameToScreenCoords(left, top, width, height, maxX, maxY, &box.x1, &box.y1);
								//if (bLeft) DisplayBox("LEFT MSG: ", box); else DisplayBox("RIGHT MSG: ", box);
								// Store the pixel coordinates
								dcSrcBox->coords = box;
								// Compute and store the erase coordinates for this HUD Box
								ComputeCoordsFromUV(left, top, width, height, uv_minmax, box,
									dcSrcBox->uv_erase_coords, &dcSrcBox->erase_coords);
								dcSrcBox->bLimitsComputed = true;

								// Get the limits for the text contents itself
								dcElemSrcBox->coords = ComputeCoordsFromUV(left, top, width, height,
									uv_minmax, box, dcElemSrcBox->uv_coords);
								dcElemSrcBox->bComputed = true;
							}
						}
					}
				}

				// Capture the bounds for the top-left bracket:
				if (g_bDynCockpitEnabled && lastTextureSelected != NULL)
				{
					if (lastTextureSelected->is_DC_TopLeftSrc)
					{
						if (!g_DCHUDBoxes.boxes[TOP_LEFT_BOX_IDX].bLimitsComputed)
						{
							DCHUDBox *dcSrcBox = &g_DCHUDBoxes.boxes[TOP_LEFT_BOX_IDX];
							DCElemSrcBox *dcElemSrcBox = NULL;
							float minX, minY, maxX, maxY;
							Box uv_minmax = { 0 };
							Box box;
							GetBoundingBoxUVs(instruction, currentIndexLocation, &minX, &minY, &maxX, &maxY,
								&uv_minmax.x0, &uv_minmax.y0, &uv_minmax.x1, &uv_minmax.y1);
							InGameToScreenCoords(left, top, width, height, minX, minY, &box.x0, &box.y0);
							InGameToScreenCoords(left, top, width, height, maxX, maxY, &box.x1, &box.y1);
							// Store the pixel coordinates
							dcSrcBox->coords = box;
							// Compute and store the erase coordinates for this HUD Box
							ComputeCoordsFromUV(left, top, width, height, uv_minmax, box,
								dcSrcBox->uv_erase_coords, &dcSrcBox->erase_coords);
							dcSrcBox->bLimitsComputed = true;

							// Get the limits for Speed & Throttle
							dcElemSrcBox = &g_DCElemSrcBoxes.src_boxes[SPEED_N_THROTTLE_DC_ELEM_SRC_IDX];
							dcElemSrcBox->coords = ComputeCoordsFromUV(left, top, width, height,
								uv_minmax, box, dcElemSrcBox->uv_coords);
							dcElemSrcBox->bComputed = true;

							// Get the limits for Missiles
							dcElemSrcBox = &g_DCElemSrcBoxes.src_boxes[MISSILES_DC_ELEM_SRC_IDX];
							dcElemSrcBox->coords = ComputeCoordsFromUV(left, top, width, height,
								uv_minmax, box, dcElemSrcBox->uv_coords);
							dcElemSrcBox->bComputed = true;
						}
					}
				}

				// Capture the bounds for the top-right bracket:
				if (g_bDynCockpitEnabled && lastTextureSelected != NULL)
				{
					if (lastTextureSelected->is_DC_TopRightSrc)
					{
						if (!g_DCHUDBoxes.boxes[TOP_RIGHT_BOX_IDX].bLimitsComputed)
						{
							DCHUDBox *dcSrcBox = &g_DCHUDBoxes.boxes[TOP_RIGHT_BOX_IDX];
							DCElemSrcBox *dcElemSrcBox = NULL;
							float minX, minY, maxX, maxY;
							Box uv_minmax = { 0 };
							Box box;
							GetBoundingBoxUVs(instruction, currentIndexLocation, &minX, &minY, &maxX, &maxY,
								&uv_minmax.x0, &uv_minmax.y0, &uv_minmax.x1, &uv_minmax.y1);
							InGameToScreenCoords(left, top, width, height, minX, minY, &box.x0, &box.y0);
							InGameToScreenCoords(left, top, width, height, maxX, maxY, &box.x1, &box.y1);
							// Store the pixel coordinates
							dcSrcBox->coords = box;
							// Compute and store the erase coordinates for this HUD Box
							ComputeCoordsFromUV(left, top, width, height, uv_minmax, box,
								dcSrcBox->uv_erase_coords, &dcSrcBox->erase_coords);
							dcSrcBox->bLimitsComputed = true;

							// Get the limits for Name & Time
							dcElemSrcBox = &g_DCElemSrcBoxes.src_boxes[NAME_TIME_DC_ELEM_SRC_IDX];
							dcElemSrcBox->coords = ComputeCoordsFromUV(left, top, width, height,
								uv_minmax, box, dcElemSrcBox->uv_coords);
							dcElemSrcBox->bComputed = true;

							// Get the limits for Number of Crafts
							dcElemSrcBox = &g_DCElemSrcBoxes.src_boxes[NUM_CRAFTS_DC_ELEM_SRC_IDX];
							dcElemSrcBox->coords = ComputeCoordsFromUV(left, top, width, height,
								uv_minmax, box, dcElemSrcBox->uv_coords);
							dcElemSrcBox->bComputed = true;
						}
					}
				}

				// Render HUD backgrounds to their own layer (HUD BG)
				if (g_bDynCockpitEnabled && lastTextureSelected != NULL)
					if (lastTextureSelected->is_DC_HUDSource)
						bRenderToDynCockpitBGBuffer = true;

				// Dynamic Cockpit: Remove all the alpha overlays in hi-res mode
				if (g_bDynCockpitEnabled && lastTextureSelected != NULL && lastTextureSelected->is_DynCockpitAlphaOverlay) {
					//if (lastTextureSelected->is_SpecialDebug)
					//	log_debug("[DBG] Skipping: [%s]", lastTextureSelected->_surface->_name);
					goto out;
				}

				//if (bIsNoZWrite && _renderStates->GetZFunc() == D3DCMP_GREATER) {
				//	goto out;
					//log_debug("[DBG] NoZWrite, ZFunc: %d", _renderStates->GetZFunc());
				//}

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

				// DBG HACK: Skip draw calls after the HUD has started rendering
				if (g_iDrawCounterAfterHUD > -1) {
					if (g_iNoDrawAfterHUD > -1 && g_iDrawCounterAfterHUD > g_iNoDrawAfterHUD)
						goto out;
				}
#endif
				
				// We will be modifying the normal render state from this point on. The state and the Pixel/Vertex
				// shaders are already set by this point; but if we modify them, we'll set bModifiedShaders to true
				// so that we can restore the state at the end of the draw call.
				bModifiedShaders = false;

				//if (g_bDynCockpitEnabled && !g_bPrevIsFloatingGUI3DObject && g_bIsFloating3DObject) {
					// The targeted craft is about to be drawn!
					// Let's clear the render target view for the dynamic cockpit
					//float bgColor[4] = { 0.1f, 0.1f, 0.3f, 0.0f };
					//context->ClearRenderTargetView(resources->_renderTargetViewDynCockpit, bgColor);
					//context->ClearDepthStencilView(this->_deviceResources->_depthStencilViewL, D3D11_CLEAR_DEPTH, resources->clearDepth, 0);
				//}

				// _offscreenAsInputSRVDynCockpit is resolved in PrimarySurface.cpp, right before we
				// present the backbuffer. That prevents resolving the texture multiple times (and we
				// also don't have to resolve it here).

				// Modify the state for both VR and regular game modes:

				// DYNAMIC COCKPIT: REPLACE TEXTURES AT RUN-TIME:
				if (g_bDynCockpitEnabled && lastTextureSelected != NULL && lastTextureSelected->is_DynCockpitDst)
				{
					int idx = lastTextureSelected->DCElementIndex;
					if (idx <= -1)
						continue;

					bModifiedShaders = true;
					dc_element *dc_element = &g_DCElements[idx];
					int numCoords = 0;
					for (int i = 0; i < dc_element->coords.numCoords; i++) {
						int src_slot = dc_element->coords.src_slot[i];
						// Skip invalid src slots
						if (src_slot < 0)
							continue;

						DCElemSrcBox *src_box = &g_DCElemSrcBoxes.src_boxes[src_slot];
						// Skip src boxes that haven't been computed yet
						if (!src_box->bComputed)
							continue;
						uvfloat4 uv_src;
						uv_src.x0 = src_box->coords.x0; uv_src.y0 = src_box->coords.y0;
						uv_src.x1 = src_box->coords.x1; uv_src.y1 = src_box->coords.y1;
						g_PSCBuffer.src[numCoords] = uv_src;
						g_PSCBuffer.dst[numCoords] = dc_element->coords.dst[i];
						g_PSCBuffer.bgColor[numCoords] = dc_element->coords.uBGColor[i];
						numCoords++;
					}
					g_PSCBuffer.DynCockpitSlots = numCoords;
					g_PSCBuffer.bUseCoverTexture = (dc_element->coverTexture != NULL) ? 1 : 0;

					// slot 0 is the cover texture
					// slot 1 is the HUD offscreen buffer
					context->PSSetShaderResources(1, 1, resources->_offscreenAsInputSRVDynCockpit.GetAddressOf());
					if (g_PSCBuffer.bUseCoverTexture)
						context->PSSetShaderResources(0, 1, &(dc_element->coverTexture));
					// No need for an else statement, slot 0 is already set to:
					// context->PSSetShaderResources(0, 1, texture->_textureView.GetAddressOf());
					// See D3DRENDERSTATE_TEXTUREHANDLE, where lastTextureSelected is set.

					/*
					static bool bDumped = false;
					if (g_iPresentCounter > 100 && !bDumped) {
						hr = DirectX::SaveWICTextureToFile(context.Get(),
							resources->_offscreenBufferAsInputDynCockpit.Get(), GUID_ContainerFormatPng, L"c://temp//dyncock.png");
						log_debug("[DBG] Dumping offscreenBufferDynCockpit");
						bDumped = true;
					}
					*/
				}

				// Send the flag for lasers (enhance them in 32-bit mode)
				if (lastTextureSelected != NULL && lastTextureSelected->is_Laser) {
					bModifiedShaders = true;
					g_PSCBuffer.bIsLaser = 1;
					if (g_config.EnhanceLasers)
						g_PSCBuffer.bIsLaser = 2; // Enhance the lasers (intended for 32-bit mode)
				}

				// Send the flag for light textures (enhance them in 32-bit mode)
				if (lastTextureSelected != NULL && lastTextureSelected->is_LightTexture) {
					bModifiedShaders = true;
					g_PSCBuffer.bIsLightTexture = 1;
					if (g_config.EnhanceIllumination)
						g_PSCBuffer.bIsLightTexture = 2; // Enhance the light textures (intended for 32-bit mode)
				}

				// Set the flag for EngineGlow (enhance them in 32-bit mode)
				if (lastTextureSelected != NULL && lastTextureSelected->is_EngineGlow) {
					bModifiedShaders = true;
					g_PSCBuffer.bIsEngineGlow = 1;
					if (g_config.EnhanceEngineGlow)
						g_PSCBuffer.bIsEngineGlow = 2; // Enhance the Engine Glow (intended for 32-bit mode)
				}

				// Dim all the GUI elements
				if (g_bStartedGUI && !g_bIsFloating3DObject) {
					bModifiedShaders = true;
					g_PSCBuffer.brightness = g_fBrightness;
				}

				// Early exit 1: Render the HUD/GUI to the Dynamic Cockpit (BG) RTV and continue
				if (bRenderToDynCockpitBuffer || bRenderToDynCockpitBGBuffer) {
					// Looks like we don't need to restore the blend/depth state
					//D3D11_BLEND_DESC curBlendDesc = _renderStates->GetBlendDesc();
					//D3D11_DEPTH_STENCIL_DESC curDepthDesc = _renderStates->GetDepthStencilDesc();
					//if (!g_bPrevIsFloatingGUI3DObject && g_bIsFloating3DObject) {
						// The targeted craft is about to be drawn! Clear both depth stencils?
						//context->ClearDepthStencilView(resources->_depthStencilViewL, D3D11_CLEAR_DEPTH, resources->clearDepth, 0);
					//}

					// Restore the non-VR dimensions:
					g_VSCBuffer.viewportScale[0] =  2.0f / displayWidth;
					g_VSCBuffer.viewportScale[1] = -2.0f / displayHeight;
					// Apply the brightness settings to the pixel shader
					g_PSCBuffer.brightness = g_fBrightness;
					//g_PSCBuffer.bAlphaOnly = 1;
					resources->InitViewport(&g_nonVRViewport);
					resources->InitVSConstantBuffer3D(resources->_VSConstantBuffer.GetAddressOf(), &g_VSCBuffer);
					resources->InitPSConstantBuffer3D(resources->_PSConstantBuffer.GetAddressOf(), &g_PSCBuffer);
					// Set the original vertex buffer and dynamic cockpit RTV:
					resources->InitVertexShader(resources->_vertexShader);
					if (bRenderToDynCockpitBGBuffer)
						context->OMSetRenderTargets(1, resources->_renderTargetViewDynCockpitBG.GetAddressOf(),
							resources->_depthStencilViewL.Get());
					else
						context->OMSetRenderTargets(1, resources->_renderTargetViewDynCockpit.GetAddressOf(),
							resources->_depthStencilViewL.Get());
					// Enable Z-Buffer if we're drawing the targeted craft
					if (g_bIsFloating3DObject)
						QuickSetZWriteEnabled(TRUE);
					// Render
					context->DrawIndexed(3 * instruction->wCount, currentIndexLocation, 0);

					// Restore the regular texture, RTV, shaders, etc:
					context->PSSetShaderResources(0, 1, lastTextureSelected->_textureView.GetAddressOf());
					context->OMSetRenderTargets(1, resources->_renderTargetView.GetAddressOf(),
						resources->_depthStencilViewL.Get());
					if (g_bEnableVR) {
						resources->InitVertexShader(resources->_sbsVertexShader);
						// Restore the right constants in case we're doing VR rendering
						g_VSCBuffer.viewportScale[0] = 1.0f / displayWidth;
						g_VSCBuffer.viewportScale[1] = 1.0f / displayHeight;
						resources->InitVSConstantBuffer3D(resources->_VSConstantBuffer.GetAddressOf(), &g_VSCBuffer);
					} else {
						resources->InitVertexShader(resources->_vertexShader);
					}

					// Restore the Pixel Shader constant buffers:
					g_PSCBuffer.brightness = MAX_BRIGHTNESS;
					//g_PSCBuffer.bAlphaOnly = 0;
					resources->InitPSConstantBuffer3D(resources->_PSConstantBuffer.GetAddressOf(), &g_PSCBuffer);
					// Restore the original blend state
					//if (g_bIsFloating3DObject)
					//	hr = resources->InitBlendState(nullptr, &curBlendDesc);
					goto out;
				}

				// Early exit 2: if we're not in VR mode, we only need the state; but not the extra
				// processing. (The state will be used later to do post-processing like Bloom and AO.
				if (!g_bEnableVR) {
					resources->InitViewport(&g_nonVRViewport);

					if (bModifiedShaders) {
						resources->InitPSConstantBuffer3D(resources->_PSConstantBuffer.GetAddressOf(), &g_PSCBuffer);
						resources->InitVSConstantBuffer3D(resources->_VSConstantBuffer.GetAddressOf(), &g_VSCBuffer);
					}

					// The original 2D vertices are already in the GPU, so just render as usual
					context->OMSetRenderTargets(1, resources->_renderTargetView.GetAddressOf(),
						resources->_depthStencilViewL.Get());
					context->DrawIndexed(3 * instruction->wCount, currentIndexLocation, 0);
					goto out;
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

				// Reduce the scale for GUI elements, except for the HUD
				if (g_bIsScaleableGUIElem) {
					bModifiedShaders = true;
					g_VSCBuffer.viewportScale[3] = g_fGUIElemsScale;
					// Enable the fixed GUI
					if (g_bFixedGUI)
						g_VSCBuffer.bFullTransform = 1.0f;
				}

				// The game renders brackets with ZWrite disabled; but we need to enable it temporarily so that we
				// can place the brackets at infinity and avoid visual contention
				if (bIsBracket) {
					bModifiedShaders = true;
					QuickSetZWriteEnabled(TRUE);
					g_VSCBuffer.sz_override = 0.05f;
					g_VSCBuffer.z_override = g_fZBracketOverride;
				}

				/* // Looks like we no longer need to clear the depth buffers for the targeted object
				if (!g_bPrevIsFloatingGUI3DObject && g_bIsFloating3DObject) {
					// The targeted craft is about to be drawn! Clear both depth stencils?
					context->ClearDepthStencilView(this->_deviceResources->_depthStencilViewL, D3D11_CLEAR_DEPTH, resources->clearDepth, 0);
					context->ClearDepthStencilView(this->_deviceResources->_depthStencilViewR, D3D11_CLEAR_DEPTH, resources->clearDepth, 0);
				}
				*/

				if (bIsSkyBox) {
					bModifiedShaders = true;
					// If we make the skybox a bit bigger to enable roll, it "swims" -- it's probably not going to work.
					//g_VSCBuffer.viewportScale[3] = g_fGlobalScale + 0.2f;
					// Send the skybox to infinity:
					g_VSCBuffer.sz_override = 0.01f;
					g_VSCBuffer.mult_z_override = 5000.0f; // Infinity is probably at 65535, we can probably multiply by something bigger here.
				}

				/*
				if (bIsTranspOrGlow) {
					bModifiedShaders = true;
				}
				*/

				// Add an extra depth to HUD elements
				if (bIsHUD) {
					bModifiedShaders = true;
					g_VSCBuffer.z_override = g_fHUDDepth;
					if (g_bFloatingAimingHUD)
						g_VSCBuffer.bPreventTransform = 1.0f;
				}

				// Let's render the triangle pointer closer to the center so that we can see it all the time,
				// and let's put it at text depth so that it doesn't cause visual contention against the
				// cockpit
				if (g_bIsTrianglePointer) {
					bModifiedShaders = true;
					g_VSCBuffer.viewportScale[3] = g_fGUIElemScale;
					g_VSCBuffer.z_override = g_fTextDepth;
				}

				// Add extra depth to Floating GUI elements
				if (bIsFloatingGUI || g_bIsFloating3DObject || g_bIsScaleableGUIElem) {
					bModifiedShaders = true;
					if (!bIsBracket)
						g_VSCBuffer.z_override = g_fFloatingGUIDepth;
					if (g_bIsFloating3DObject && !bIsBracket) {
						g_VSCBuffer.z_override += g_fFloatingGUIObjDepth;
					}
				}

				// Add an extra depth to Text elements
				if (bIsText) {
					bModifiedShaders = true;
					g_VSCBuffer.z_override = g_fTextDepth;
				}

				/*
				// HACK
				// Skip the text call after the triangle pointer is rendered
				if (g_bLastTrianglePointer && lastTextureSelected != NULL && lastTextureSelected->is_Text) {
					g_bLastTrianglePointer = false;
					//log_debug("[DBG] Skipping text");
					bModifiedShaders = true;
					g_VSCBuffer.viewportScale[3] = g_fGUIElemScale;
					g_VSCBuffer.z_override = g_fTextDepth;
					//goto out;
				}
				*/

				if (bModifiedShaders) {
					resources->InitPSConstantBuffer3D(resources->_PSConstantBuffer.GetAddressOf(), &g_PSCBuffer);
					resources->InitVSConstantBuffer3D(resources->_VSConstantBuffer.GetAddressOf(), &g_VSCBuffer);
				}

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
				// with just using one, though... but why would we use just one? To make AO 
				// computation faster? On the other hand, having always 2 z-buffers makes the code
				// easier.

				if (g_bUseSteamVR)
					context->OMSetRenderTargets(1, resources->_renderTargetView.GetAddressOf(),
						resources->_depthStencilViewL.Get());
				else
					context->OMSetRenderTargets(1, resources->_renderTargetView.GetAddressOf(),
						resources->_depthStencilViewL.Get());
				
				// VIEWPORT-LEFT
				if (g_bUseSteamVR) {
					viewport.Width = (float)resources->_backbufferWidth;
				} else {
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
				// The viewMatrix is set at the beginning of the frame
				resources->InitVSConstantBufferMatrix(resources->_VSMatrixBuffer.GetAddressOf(), &g_VSMatrixCB);
				// Draw the Left Image
				context->DrawIndexed(3 * instruction->wCount, currentIndexLocation, 0);

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
				} else {
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
//#ifdef DBG_VR
			out:
//#endif
				// Update counters
				g_iDrawCounter++;
				if (g_iDrawCounterAfterHUD > -1)
					g_iDrawCounterAfterHUD++;
				//g_iDrawIndexAfterHUD
				// Have we just finished drawing the targetting computer?
				if (lastTextureSelected != NULL && lastTextureSelected->is_Floating_GUI)
					g_iFloatingGUIDrawnCounter++;

				if (g_bIsTrianglePointer)
					g_bLastTrianglePointer = true;

				if (bIsBracket && bModifiedShaders) {
					// Restore the No-Z-Write state for bracket elements
					QuickSetZWriteEnabled(bZWriteEnabled);
					g_VSCBuffer.z_override      = -1.0f;
					g_VSCBuffer.sz_override     = -1.0f;
					g_VSCBuffer.mult_z_override = -1.0f;
					resources->InitVSConstantBuffer3D(resources->_VSConstantBuffer.GetAddressOf(), &g_VSCBuffer);
				}

				// Restore the normal state of the render (currently this only means restore the original Vertex/Pixel
				// constant buffers); but only if we altered it previously.
				if (bModifiedShaders) {
					g_VSCBuffer.viewportScale[3]  =  g_fGlobalScale;
					g_VSCBuffer.z_override        = -1.0f;
					g_VSCBuffer.sz_override       = -1.0f;
					g_VSCBuffer.mult_z_override   = -1.0f;
					g_VSCBuffer.bPreventTransform =  0.0f;
					g_VSCBuffer.bFullTransform    =  0.0f;

					g_PSCBuffer = { 0 };
					g_PSCBuffer.brightness        = MAX_BRIGHTNESS;
					g_PSCBuffer.ct_brightness	  = g_fCoverTextureBrightness;
					//g_PSCBuffer.bUseCoverTexture  = 0;
					//g_PSCBuffer.DynCockpitSlots   = 0;
					//g_PSCBuffer.bRenderHUD		  = 0;
					//g_PSCBuffer.bEnhaceLasers	  = 0;
					//g_PSCBuffer.bIsLightTexture   = 0;
					resources->InitVSConstantBuffer3D(resources->_VSConstantBuffer.GetAddressOf(), &g_VSCBuffer);
					resources->InitPSConstantBuffer3D(resources->_PSConstantBuffer.GetAddressOf(), &g_PSCBuffer);
				}

			//no_vr_out:
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

			//MessageBox(nullptr, text, __FUNCTION__, MB_ICONERROR);
			log_debug("[DBG] %s, %s", text, __FUNCTION__);
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

	//log_debug("[DBG] BeginScene");
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
	//log_debug("[DBG] EndScene");
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

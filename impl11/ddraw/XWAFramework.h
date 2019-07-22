#pragma once
#include "XWAObject.h"
#include "XWAEnums.h"
#include <windows.h>
#include "XWATypes.h"

// Functions from XWA
const auto CopyRectToFrom = (LONG(*)(RECT *pRectDest, RECT *pRectSrc))0x558CB0;
const auto CreateButtonWithHighlightAndSnd = (int(*)(RECT *rect, char *highlightedImage, char *imageStr, char *hoverText, int highlightColor, int color, int offset, char *soundStr))0x5563C0;
const auto GetFrontTxtString = (char *(*)(int stringIndex))0x55CB50;
const auto LoadSkirmishFile = (int(*)(char *fileStr, int))0x5529B0;
const auto PlayDirectSound = (int(*)(char *soundStr, int, int, int, int, int))0x538850;
const auto LoadMissionsFromLstFile = (char(*)())0x566D90;
const auto SetNumOfPlayerSlotsPerTeam = (int(*)())0x54D620;
const auto FormatFGRoster = (int(*)())0x54D6D0;
const auto DisplayMissionDescription = (void(*)(char *))0x548140;
const auto CreateSelectedButton = (int(*)(RECT *rect, char *imageStr, char *hoverText, int selectedButtonColor))0x5568E0;
//const auto LoadImage = (void(*)(char *imageStr))0x532080;
const auto CombatSimulatorBackgrounds = (int(*)(int))0x54CF90;
const auto SetGlobalStringColorCode = (char(*)(unsigned char))0x434E30;
const auto DP_PlayerIsHost = (int(*)())0x52DD80;
const auto MouseCursorCollidedWithRect = (bool(*)(RECT *pRect, int cursor_X_Pos, int cursor_Y_Pos))0x5592E0;
const auto GetMousePosition = (int(*)(int* cursorX, int* cursorY))0x55BA50;
const auto SyncCombatSimulatorConfig = (bool(*)(int))0x549330;
const auto SetRectDimensions = (RECT*(*)(RECT *pRect, int left, int top, int right, int bottom))0x558C90;
const auto LoadWaveLst = (int(*)(char*, int, int))0x43A150;
const auto AlignRectToDatapad = (int(*)(LONG *))0x558D10;
const auto CreateSettingButton = (int(*)(RECT* rect, char*, char*, int, int, int textColor, int, char* sound))0x556660;
const auto UI_DisplayTextWithRect = (int(*)(int pixels, char *text, RECT *pRect, int textColor))0x5575A0;
const auto ReadSkirmishFile = (int(*)(const char*, int))0x552370;
const auto SyncRoster = (int(*)(int))0x549570;
//const auto sprintf = (int(*)(char* str, const char * format, ...))0x59A680;
//const auto remove = (int(*)(const char *lpFileName))0x59BC30;
const auto LoadSpecificMissionLst = (void(*)(int missionDirectory))0x547800;
const auto UI_CreateTransparentImage = (int(*)(char* resStr, int x, int y))0x532350;
const auto UI_DisplayTextWidthHeight = (int(*)(int pixels, unsigned __int8 *text, int width, int height, int textColor))0x557310;
const auto UI_CreateScrollBar = (int(*)(RECT *rect, int scrollPosition, int, int, int, int, int))0x5555D0;
const auto GetCraftSpeciesShortNamePtr = (int(*)(int speciesIndex))0x4DCE50;
const auto UI_CreateClickableText = (int(*)(int, int, int buffer, int, int highlightColor, int, int, char* sound))0x556100;
const auto GetFGPointWorth = (int(*)(int craftType, int fgCreationFlags, int countermeasures, int warheads, int fgAIRank, int fgDuty))0x555420;
const auto GetSpeciesExternalData = (int(*)(int *speciesPtr))0x5552A0;
const auto GetShipBMP = (int(*)(int speciesIndex))0x55DC20;
const auto DisplayShipBMP = (int(*)())0x55DC70;
const auto SetRectWidthHeight = (RECT*(*)(RECT* rect, int width, int height))0x558CD0;
const auto PlayMusic = (int(*)(int musicID))0x49AA40;
const auto StopMusic = (char(*)())0x49ADA0;
const auto AllocateImage = (void**(*)(char* image, char* overlay))0x531D70;
const auto PositionImage = (int(*)(char* image, int height, int width))0x534A60;
const auto DisplayMessage = (int(*)(int messageIndex, int playerIndex))0x497D40;
const auto GetKeyboardDeviceState = (int(*)())0x42B900;
const auto DirectInputKeyboardReaquire = (char(*)())0x42B920;



// Globals from XWA
const auto missionDirectory = (int *)0xAE2A8A;
const auto implogoStringPtr = (char *)0x60492C;
const auto selectedColor = (int *)0xAE2A30;
const auto normalColor = (int *)0xABD224;
const auto highlightedTextColor = (int *)0xABD228;
const auto highlightedTextColor2 = (int *)0xAE2A48;
const auto combatSimScreenDisplayed = (int *)0x9F4BC8;
const auto rightPanelState = (int *)0x9F4B94;
const auto rightPanelState2 = (int *)0x9F4B48;
const auto leftPanelState = (int *)0x9F4B4C;
const auto configConcourseSFXVolume = (__int8 *)0xB0C887;
const auto configDifficulty = (__int8 *)0xB0C8BB;
const auto missionDescriptionPtr = *(char **)0x9F4BD0;
const auto missionLstPtr = (int *)0x9F4B98;
const auto missionIndexLoaded = (int *)0x9F5E74;
const auto missionSelectedOnLoadScrn = (int *)0x7830C0;
const auto totalMissionsInLst = (int *)0x9F5EC0;
const auto missionDirectoryMissionSelected = (int *)0xAE2A8E;
const auto missionDescriptionScrollPosition = (int *)0x7831B0;
//const auto rectStandard3 = (int (*)[4])0x6031A8;
RECT* rectStandard3 = (RECT *)0x6031A8;
const auto battleSelectScrollMovement = (int *)0x78317C;
const auto loadScrnTotalMissionsListed = (int *)0x7830BC;
const auto localPlayerIndex = (int *)0x8C1CC8;
ObjectEntry* objects = *(ObjectEntry **)0x7B33C4;
PlayerDataEntry* PlayerDataTable = (PlayerDataEntry *)0x8B94E0;
const auto localPlayerConnectedAs = (int *)0xABD7B4;
const auto flightGroupInfo = (int *)0x783194;
const auto battleSelectMissionScrollIndex = (int *)0x783174;
const auto tacOfficerLoaded = (int *)0x7B6FAE;
const auto battleSelectScrollReturnMovement = (int *)0x783184;
int* FGCreationSlotNum_ = (int*)0x783188;
const auto FGCreationSelectCraftMenu = (int*)0x7830DC;
const auto FGCreationSpecies = (__int16*)0x9F4B82;
const auto FGCreationNumCraft = (__int8*)0x9F4B8D;
const auto FGCreationNumWaves = (__int8*)0x9F4B8C;
const auto provingGrounds = (__int8*)0x8053E5;
const auto tacOfficerInMission = (__int8*)0x7B6FAE;
const auto waveLstString = (int*)0x5B328C;
const auto FGCreationPlayerInSlotTable = (int*)0x9F5EE0;
const auto mainStringBuffer = (char(*)[])0xABD680;
const auto aSS_3 = (char*)0x603394;
const auto skirmishConstStr = (char*)0x603174;
const auto configGoalType = (__int8*)0xB0C8DE;
const auto colorOfLastDisplayedMessage = (__int16*)0x91AC9C;
const auto g_messageColor = (char*)0x8D6C4C;
const auto FGCreationNumOfCraftListed = (int*)0x7830D0;
const auto FGCreationScrollPosition = (int*)0x78319C;
const auto FGCreationCraftIteration = (int*)0x783178;
const auto totalCraftInCraftTable = (int*)0xABD7DC;
const auto speciesShipListPtr = (int*)0xABD22C;
const auto FGCreationPlayerNum = (int*)0x9F4B84;
const auto FGCreationPlayerGunnerNum = (int*)0x9F4B88;
const auto FGCreationGenusSelected = (int*)0x7831A4;
const auto aSS_4 = (char*)0x603548;
const auto aS_0 = (char*)0x5B0E4C;
const auto FGCreationFlags = (int*)0x9F4B90;
const auto FGCreationAIRank = (__int8*)0x9F4B8E;
const auto FGCreationDuty = (__int8*)0x9F4B8F;
const auto FGCreationNumOfWaves = (__int8*)0x9F4B8C;
const auto FGCreationCraftSlotNumTable = (int*)0xABD280;
const auto aD = (char*)0x6012D4;
const auto campaignIncomplete = (int*)0xABCF80;
const auto currentMissionInCampaign = (int*)0xABC970;
const auto currentMusicPlaying = (int*)0xAE2A44;
const auto configDatapadMusic = (__int8*)0xB0C88E;
const auto numberOfPlayersInGame = (int*)0x910DEC;
const auto viewingFilmState = (__int8*)0x80DB68;
const auto mouseLook = (__int8*)0x77129C;
const auto mouseLookWasNotEnabled = (__int8*)0x7712A0;
const auto keyPressedAfterLocaleAfterMapping = (__int16*)0x8053C0;
const auto win32NumPad2Pressed = (__int8*)0x9E9570;
const auto win32NumPad4Pressed = (__int8*)0x9E956B;
const auto win32NumPad5Pressed = (__int8*)0x9E956C;
const auto win32NumPad6Pressed = (__int8*)0x9E956D;
const auto win32NumPad8Pressed = (__int8*)0x9E9568;
const auto inMissionFilmState = (__int8*)0x7D4C4D;
const auto mouseLook_Y = (int*)0x9E9624;
const auto mouseLook_X = (int*)0x9E9620;
const auto mouseLookInverted = (__int8*)0x771298;
const auto mouseLookResetPosition = (int*)0x9E962C;



// Unknowns

const auto dword_7833D4 = (int*)0x7833D4;
const auto dword_B07C6C = (int*)0xB07C6C;
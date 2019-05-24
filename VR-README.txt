xwa_vr_ddraw_d3d11:

Virtual Reality implementation of ddraw.dll. Based on Jeremy Ansel's
ddraw.dll. This is only a graphics library: it has no direct integration
with any VR headset (yet). You will need a third party solution to map the
VR motion into mouse motion, like Trinus, iVRy, OpenTrack, VRidge (or
others, suggestions?)

More information at xwaupgrade.com -- search for "X-Wing Alliance VR Mod"

*** Requirements ***

- You'll need a Joystick to play the game.
- Either the GOG or Steam versions of X-Wing Alliance (other versions may also work)
- The X-Wing Alliance Upgradde (XWAU) craft pack 1.6 (from xwaupgrade.com)
- DirectX 11
- a CPU with SSE2 support
- VR Headset or other SBS-compatible solution

Optional:
- Software that maps the VR orientation to mouse motion (like Trinus, iVRy, VRidge
  or OpenTrack). You'll still be able to see stereoscopy with a VR Headset (or even
  with a Google Cardboard); but the view will be locked forward.

*** Installation (TL;DR) ***

* Install the XWAU craft pack 1.6 from xwaupgrade.com 
  -- I suggest installing the modified X-Wing POV: it has a better view.
* Copy the VR ddraw.dll to the installation path.
* Set your desktop resolution to the resolution supported by your HMD.
* Configure vrparams.cfg to use either SteamVR or Direct SBS mode + FreePIE.
* If using SteamVR, set the resolution of your game to 100% to minimize performance loss, and
  enable "Do not fade to grid when app hangs" in SteamVR -> Settings -> Developer options.
* Rename in the XWA's installation directory rename "Hook_Windowed.dll" for "Hook_Windowed.dl_"
  and copy "Hook_XWACockpitLook.dll" in that path. This hook maps FreePIE and SteamVR directly
  to the in-game's camera.
* Optional: if you're not using "Hook_XWACockpitLook.dll", then you'll need to map head tracking
  data to mouse motion using third party software, like Trinus or OpenTrack.

Installation Instructions (long version):

* Install the X-Wing Alliance Upgrade craft pack 1.6.
   - I also suggest the alternate POV X-Wing cockpit since it has a better view.

* Copy "Hook_XWACockpitLook.dll" on the same path where xwingalliance.exe is located.
   - This hook will let you use FreePIE or SteamVR to map head-tracking data directly
     to the in-game camera. The original hook was made by Justagai and I just added
	 support for these trackers (more trackers will be added in the future).
   - The Trinus 1.01 PSVR Beta has an option to enable FreePIE in the advanced settings.
* Rename "Hook_Windowed.dll" to "Hook_Windowed.dl_".
   - If you get a message box stating that "Hook_XWACockpitLook" could not be initialized
     then you probably missed this step.
* If you're not using SteamVR or FreePIE, then you'll need to enable and configure
  mouse look. Run XWA and test the mouse look (scroll lock key). Map the scroll lock
  key to a joystick button or keyboard key that you can find easily.

   - Things to try if mouse look spins the cockpit wildly:
   
   	* Set the Windows DPI Scaling to 100%.
   	* Stop background processes that may be consuming too much CPU or disk.
   	* Enable hardware acceleration.
   	* Other? Suggestions are welcome!
	
* Adjust the mouse motion sensitivity in Window's Mouse Properties. Try to use low
  values like 3 or 4 (window's default is 5 and the highest setting is 10). You'll need 
  to use a third-party application to map headset motion to mouse motion. This is the point
  where you should setup that software.
  
   - If you're using Trinus, you may need to enable sensor smoothing (I've got it set to 9.5).
     This helps reduce noise from the VR headset.

* Highly recommended: Enable hardware acceleration. 
   - This can be done in the nVidia Control panel, not sure about other cards.
   
* Open ddraw.cfg and set "PreserveAspectRatio = 1" and restart the game. Alternatively, dump the
  VR configuration file by pressing Ctrl+Alt+S at any point during the game and set the aspect
  ratio manually in that file. You can apply the changes in vrparams.cfg without restaring the
  game, just press Ctrl+Alt+L.

* Open the installation directory on the place where xwingalliance.exe was installed. This would be:

	C:\Program Files (x86)\GOG Galaxy\Games\Star Wars - X-Wing Alliance
	C:\Program Files (x86)\Steam\steamapps\common\Star Wars X-Wing Alliance
	etc...

* Make a backup copy of the ddraw.dll file you'll find in this path.
* Copy the VR ddraw.dll on the same path where xwingalliance.exe is located.
* Enjoy!

*** Configuration ***

This game reads a configuration file called vrparams.cfg. This file is optional -- if it 
doesn't exist the game uses default VR params. The VR experience can be further tweaked
by editing this file. Press Ctrl+Alt+S to create the file and open it to see further
instructions.

*** SteamVR (Performance, etc) ***

SteamVR was designed for pipelined multithreaded games; XWA, unfortunately, does not fall
in this category. What this means is that the performance in SteamVR may not be as high
as direct SBS mode. I'm looking for ways to solve this situation; but in the meantime,
there are a few things you can do:

* Open SteamVR -> Settings -> Developer. Enable "Do not fade to grid when app hangs".
  During the loading of a mission, the game will no longer refresh the screen. Unfortunately,
  SteamVR will misinterpret this as the game freezing and will fade to the default grid.
  This setting will just keep the previous image in front of you.
* SteamVR -> Settings -> Application. Enable Custom Resolution and set it to 100%. Having
  larger resolutions improves the quality; but at the cost of performance. If you're experiencing
  a low framerate, try lowering it to 100% (or maybe lower if necessary).
* SteamVR -> Settings -> Video. Click on "Enable custom resolution" and set it to 100%.

*** Usage ***

This VR mod adds a few new keyboard mappings to customize the VR experience:

General Configuration:
Ctrl + Alt + S: Save current VR parameters to vrparams.cfg
Ctrl + Alt + L: Load VR parameters from vrparams.cfg
Ctrl + Alt + R: Reset VR parameters to defaults.
Ctrl + Alt + V: Enable/Disable VR mode -- this is for people who don't like VR. But seriously,
				I have plans to add native support for ReShade in the future. This option will
				allow users to play the game in traditional mode and still enjoy the new shaders.

Window Scale:
Ctrl + Z: Toggle Zoom-Out Mode (allows you to see all the corners of the screen)
Ctrl + Alt + Shift + (+/-): Adjust Zoom-Out Mode/Concourse/Menu scale
Ctrl + Alt + (+/-): Adjust 3D screen scale.

Stereoscopy Adjustment:
Ctrl + Alt + Shift + Up Arrow, Down Arrow: Increase/Decrease IPD by 1mm. 
-- WARNING: A high value here will cause eye strain, fatigue or headaches. ALWAYS take
			care of your health!
Ctrl + Alt + H: Toggle the stereoscopy boost (the "cockpit hack"). The default is ON; but
				it may cause some minor artifacts when you fire warheads or fly *too close*
				to any object.

Lens Distortion Adjustment:
Ctrl + Alt + B: Toggle the Barrel Distortion Effect.
Ctrl + Alt + Left/Right: Adjust K1 Lens Distortion Coefficient (fine-tunes the effect)
Ctrl + Alt + Up/Down:	 Adjust K2 Lens Distortion Coefficient (bigger effect, start here)

GUI Depth Adjustment:
Ctrl + Shift + Up Arrow, Down Arrow: Adjust Targeting Computer GUI Depth
Ctrl + Shift + Left Arrow, Right Arrow: Adjust Text Depth -- This should be above the Targeting Computer Depth.
Shift + Left Arrow, Right Arrow: Adjust Aiming HUD Depth.

Camera Position:
Left Arrow, Right Arrow: Lean left and right
Up Arrow, Down Arrow: Boost, Duck.
Shift+Up Arrow, Down Arrow: Lean forward/backward.

*** Known Issues ***

- If you fly too close to any object, the geometry may break. This can also be noticed
  when firing a missile. This happens because of the "stereoscopy boost" option. It can
  be disabled in the vrparams.cfg or by pressing Ctrl+Alt+H; but it usually isn't a problem.
- The key shortcuts added in this mod become unresponsive in the hangar.
- The targetting computer and other GUI elements tend to be placed in the wrong position
  in the hangar before a mission.

*** License ***

xwa_vr_ddraw_d3d11 is licensed under the MIT license (see VR-LICENSE.txt)

*** Version History ***

Version 1.0.0:
Rebased all the code on Jeremy Ansel's latest-and-greatest (so the mod has better performance
now). General code cleanup and minor bug fixing. Added support for FreePIE. Added support for
SteamVR.

Version 0.9.5, May 13, 2019:
Fixed the "offscreenBuffer invalid parameter" bug that occurred at startup on some
video cards. Fixed some minor bugs and code cleanup. Extended the brightness option
to the Concourse and Menus so that ReShade + Bloom can be used without affecting
these elements. Note that the "Text_brightness" cfg option is now just "brightness"

Version 0.9.4, May 9, 2019:
Changed internal parameters to use IPD in cms. Zoom-Out mode now scales down only the HUD.
Some other minor fixes (scale cannot be decreased below 0.2, etc)

Version 0.9.3, May 7, 2019:
Minor fixes and code cleanup. The text can now be dimmed through vrparams.cfg to prevent
ReShade's Bloom effect on these elements.

Version 0.9.2, May 5, 2019:
Fixed the Tech Library. Fixed the multiple images when displaying animations in the concourse
or when going into a menu. Fixed black-out screen when in the hangar during a campaign. Minor
performance boost and other minor fixes.

Version 0.9.1, May 2, 2019:
Added code to estimate the aspect ratio. Enabled lean forward/backward.

Version 0.9.0, Apr 30, 2019:
First BETA public release. There are some known artifacts; but the game is stable and
playable. I recommend the Quick Skirmish mode; but the campaign can be played too (although
sometimes the screen momentarily goes black when launching from the hangar).

*** Credits ***

Jeremy Ansel's xwa_ddraw_d3d11 (used with permission)
https://github.com/JeremyAnsel/xwa_ddraw_d3d11

BlueSkyDefender's Polynomial Barrel Distortion: (used with permission)
https://github.com/BlueSkyDefender/Depth3D/blob/master/Shaders/Polynomial_Barrel_Distortion_for_HMDs.fx
https://github.com/crosire/reshade-shaders/blob/master/Shaders/Depth3D.fx

Justagai's Hook_XWACockpitLook.dll: this new hook allowed me to map FreePIE and SteamVR tracking
directly to the in-game's camera.

All the guys behind the X-Wing Alliance Upgrade project:
xwaupgrade.com

Extended for VR by Leo Reyes, 2019.

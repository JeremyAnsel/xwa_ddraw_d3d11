This is an implementation of ddraw.dll using Direct3D 11.
It is intented to be used with the X-Wing Alliance game.
More info at xwaupgrade.com:
http://www.xwaupgrade.com/phpBB3008/viewtopic.php?f=10&t=11167


*** Requirements ***

This dll requires:
- DirectX 11
- a CPU with SSE2 support

How to install the latest version of DirectX
http://support.microsoft.com/kb/179113/en

To download the latest vc redist runtime:
https://docs.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist


*** Usage ***

To use this dll:
- place ddraw.dll and ddraw.cfg next to xwingalliance.exe

ddraw.cfg is a config file. Open it with a text editor for more details.

Note:
By default, the game supports only the following 4:3 resolutions:
- 640 x 480
- 800 x 600
- 1024 x 768
- 1152 x 864
- 1280 x 1024
- 1600 x 1200

You can replace these resolutions (and other things) with XWA Hacker by Reimar Döffinger (http://sourceforge.net/projects/xwahacker/files/).


*** License ***

xwa_ddraw_d3d11 is licensed under the MIT license. See LICENSE.txt


*** Source code ***

The source code of xwa_ddraw_d3d11 is situated at:
https://github.com/JeremyAnsel/xwa_ddraw_d3d11


*** Credits ***

- Jérémy Ansel (JeremyaFr)

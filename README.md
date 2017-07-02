# OpenLieroX Classic

[OpenLieroX](http://www.openlierox.net/) is a fantastic continuation of JasonB's excellent [Liero](http://www.liero.be/) clone, Liero Xtreme. However, personally my favourite version of the game was always Liero Xtreme v0.56b.

This project aims to preserve the source code for that version of the game, as released by JasonB in 2006.

## Build

The included Visual Studio project should build without errors.

If you see the error **cannot open include file 'afxres.h'**, you need to install the "MFC and ATL support" component:

 > Visual Studio Installer > Modify > Individual components > "SDKs, libraries and frameworks" section

Building the project results in *Project/obj/Debug/Game.exe* (for a debug build) or *Project/obj/Release/LieroX.exe* (for a release build) being created. Copy this file to the *GameDir* and you should be able to run it!

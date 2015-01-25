# MayaPlugin-VertexChameleon
Advanced vertex colouring plugin for Autodesk Maya
Created by RenderHeads Ltd in 2008, source released in 2015.
Plugin website: http://renderheads.com/portfolio/VertexChameleon/

##Maya SDK Setup

The core Maya SDK must be available to the plugin in order to compile.  The 3 folders needed are:
 + include
 + lib
 + lib64

In order to build for multiple versions of Maya we use the following directory structure:

	MayaSDK/
		Common/
		Windows/
		MacOSX/
		Linux/

The Common folder contains all of the header files (contents of include folder), and the platform folders contain all the of the platform specific libraries (lib and lib64 folder).

Each sub-folder starts with the Maya version number.

For example:

	MayaSDK/
		Common/
			6.0/
				maya/
			8.5/
				maya/
			2011/
				maya/
				tbb/
		Windows/
			6.0/
				lib/
			2008/
				lib/
				lib64/
			2011/
				lib/
				lib64/
		MacOSX/
		Linux/

In Windows set the environment variable MAYALIBBASE to the path of your MayaSDK folder.  After adding the environment variable you may need to reopen Visual Studio.

##Compiling for Windows

+ The project solution is created with Visual Studio 2008.
+ Make sure you have set up your Maya SDK files as specified above
+ Each version of Maya has its own solution configuration. 32-bit and 64-bit cpu targets can be selected from the solution platform list.
+ Hit 'Build' and it should build a plugin for the current configuration.

#### Add support for additional Maya versions
To add a new configuration for another version of Maya:
+ Right-click on the solution and open Configuration Manager
+ In "Action solution configuration" select "New"
+ Select the latest release version of Maya from the list of "Copy settings from"
+ Make the name the same format but replace the version number
+ Make sure that the 32 and 64-bit settings are correct

+ Once the new configuration is created you'll need to configure the project for that configuration:
	+ Right-click on the Vertex Chameleon project and go to Properties
	+ Select C/C++ > General
		+ Edit "Additional Include Directories" to point the location of that versions include files.  For example in Maya 2011 it would read "$(MAYALIBBASE)\Common\2011;"
	+ Select C/C++ > Preprocessor
		+ Make sure the list contains an entry for your version of Maya.  For 2011 use "MAYA2011".  This preprocessor is used later when including the Maya SDK headers to exclude or include certain features based on the version we're building for.
	+ Select Linker > General 
		+ Set the Output File property to match your Maya version number.  For 2011 it should read "$(OutDir)\VertexChameleon2011.mll"
		+ Set Additional Library Directories to point to the relevent Maya SDK folder.  
			For Maya 2011 32-bit it would read "$(OutDir)";"$(MAYALIBBASE)\Windows\2011\lib";
			For Maya 2011 64-bit it would read "$(OutDir)";"$(MAYALIBBASE)\Windows\2011\lib64";

##Compiling for MacOSX

+ Use XCode to load the project "vertexChameleon.xcodeproj"

##Compiling for Linux

+ See build.sh and buildall.sh
+ Use 'make'
+ The build script needs the following variales defines:
	+ export ARCH=64		(for 64-bit building)
	+ export MAYAVER=2011		(this is the maya version number, eg 8.5)
	+ export MAYAVERs=2011		(this is the string version of the Maya version, so Maya 8.5 would be 85)

##Packaging for Windows

+ We build to a setup executable
+ Install NSIS v2.46 with the FindProcDLL plugin (http://nsis.sourceforge.net/FindProcDLL_plug-in)
+ Open InstallScript.NSI and increment the version numbers at the top to match the new build version number
+ Right-click the InstallScript.NSI and compile with NSI
+ The resulting installer file should appear in the Releases folder

NOTE: If the installer fails to run, it may be because using a mapped drive

##Packaging for OSX and Linux

+ There is no setup, simply ZIP up all the files in the module folder to an appropriatly named ZIP file.

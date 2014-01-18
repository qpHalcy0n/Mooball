solution "Mooball"
	location "build"
	
	configurations { "Debug", "Release" }
	platforms { "Native", "x32", "x64" }
	
	targetdir "bin"
	
local EXTLIBS = "extlibs"

local DLLName = "MooballDLL" 
local WIN_SDK_DIR = "$(WindowsSdkDir)"
local MEDIA = "bin/Media"

project (DLLName)
	uuid "bf47d341-0a8d-4de3-a363-443648bf21af"
	kind "WindowedApp"
	language "C++"
	location "build"
	defines { "_MOOBALL_DLL", "WIN32", "__WIN32" }

	files { project().name .. "/include/**.h", project().name .. "/include/**.hpp", project().name .. "/src/**.cpp" }
	
	includedirs( project().name .. "/include" )
	includedirs( WIN_SDK_DIR .. "Include" )
	
	implibdir ( project().name .. "/lib" )
	libdirs { WIN_SDK_DIR .. "Lib" }
	
	vpaths	
	{ 
		["Headers/*"] =	
		{ 
			project().name .. "/include/**.h", 
			project().name .. "/include/**.hpp" 
		},
		["Source/*"] =	
		{ 
			project().name .. "/src/**.c", 
			project().name .. "/src/**.cpp" 
		}
	}
	
	configuration { "Debug" }
		debugdir "bin"
		targetname ( project().name .. "_d" )
		flags { "Symbols" }
		defines { "_DEBUG" }
		links { "OpenGL32", "glu32" }
		
		configuration { "Native" }			
		configuration { "x32" }
		configuration { "x64" }
			
	configuration { "Release" }
		debugdir "bin"
		targetname ( project().name )
		flags { "Optimize" }
		defines { "_RELEASE" }
		links { "OpenGL32", "glu32" }
		
		configuration { "Native" }
		configuration { "x32" }
		configuration { "x64" }

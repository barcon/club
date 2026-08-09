-- premake5.lua
workspace "club"
   configurations { "Debug", "ReleaseCL" }
   location "build"

project "club"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	architecture "x86_64" 
	objdir "%{cfg.location}/obj/%{cfg.platform}_%{cfg.buildcfg}"   

	targetdir "build/%{cfg.buildcfg}"
	includedirs { "../utils/src"}
	includedirs { "../logger/src"}
	includedirs { "../opencl/inc"}

	files { "src/**.hpp", "src/**.cpp" }

	filter "configurations:Debug"  
	  defines { "DEBUG" }
	  symbols "On"

	filter "configurations:ReleaseCL"
	  architecture "x86_64" 	  
	  defines { "NDEBUG" }
	  optimize "Speed"
-- premake5.lua
workspace "club"
   configurations { "Debug", "Release" }
   location "build"

project "club"
   kind "StaticLib"
   language "C++"
   cppdialect "C++20"
   
   targetdir "build/%{cfg.buildcfg}"
   includedirs { "../utils/src"}
   includedirs { "../logger/src"}
   includedirs { "../opencl/inc"}

   files { "src/**.hpp", "src/**.cpp" }

   filter "configurations:Debug"
	  architecture "x86_64"    
	  defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      architecture "x86_64" 	  
	  defines { "NDEBUG" }
      optimize "Speed"
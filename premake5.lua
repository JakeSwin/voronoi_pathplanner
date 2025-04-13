workspace "VoroPathplanner"
configurations { "Debug", "Release" }

project "VoroPathplanner"
kind "ConsoleApp"
language "C++"
targetdir "bin/%{cfg.buildcfg}"

files { "./src/**.cpp", "./src/**.hpp" }
includedirs { "include" }

filter "system:linux"
links { "m", "raylib", "lua" }

filter "system:macosx"
links { "m", "raylib", "lua", "Cocoa.framework", "IOKit.framework", "OpenGL.framework" }

filter "configurations:Debug"
defines { "DEBUG" }
symbols "On"

filter "configurations:Release"
defines { "NDEBUG" }
optimize "On"

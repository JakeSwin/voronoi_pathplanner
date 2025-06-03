workspace "VoroPathplanner"
configurations { "Debug", "Release" }

project "VoroPathplanner"
kind "ConsoleApp"
language "C++"
cppdialect "C++17"
toolset "gcc-14"
targetdir "bin/%{cfg.buildcfg}"

files { "./src/**.cpp", "./src/**.hpp" }
includedirs {
    "/usr/include/eigen3",
    "third_party/libgp/include",
    "third_party/libgp/build/include",
    "third_party/stb_image",
    "third_party/cy",
    "third_party/jc_voronoi"
}

libdirs { "third_party/libgp/build" }

filter "system:linux"
links { "m", "raylib", "lua", "gp" }

filter "system:macosx"
architecture "arm64"
defines { "CY_NO_INTRIN_H" }
includedirs { "/opt/homebrew/include/lua", "/usr/local/include" }
libdirs { "/opt/homebrew/lib/lua", "/usr/local/lib" }
links { "m", "raylib", "lua", "gp", "Cocoa.framework", "IOKit.framework", "OpenGL.framework" }
buildoptions { "-isystem/opt/homebrew/include/lua" }

filter "configurations:Debug"
defines { "DEBUG" }
symbols "On"

filter "configurations:Release"
defines { "NDEBUG" }
optimize "On"

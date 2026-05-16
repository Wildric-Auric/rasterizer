workspace "Rasterizer"
    configurations {"Debug64", "Release64"}
    architecture "x64"
    language   "C++"
    cppdialect "C++11"
    targetdir "bin/%{prj.name}/%{cfg.buildcfg}"
    objdir    "bin/objs"
    location "bin/"
    characterset("MBCS")
    defines("GLEW_STATIC")
filter "system:windows"
    buildoptions { "/EHsc" }
filter {}
    startproject "Rasterizer"

project "Rasterizer"
    kind("ConsoleApp")
    includedirs {
        "../include/",
        "src/**.h",
        "src/**.cpp",
    }
    files {
        "src/**.cpp",
        "src/**.h",
    }
    filter "configurations:*unity*"
    removefiles {"src/**.cpp"}
    files {"unity/*.cpp"}
    filter {}
    filter "configurations:*Debug*"
        symbols "On"
        staticruntime "Off"
        filter { "configurations:*Debug*", "system:windows" }
            links {
                "../lib/NWengineCore64d.lib",
            }
        filter { "configurations:*Debug*", "system:linux" }
            links {
                "../lib/NWengineCore64d", "freetype", "GLEW"
            }
    filter "configurations:*Release*"
        optimize "On"
        staticruntime "On"
        filter { "configurations:*Release*", "system:windows" }
            links {"../lib/NWengineCore64.lib"}
        filter { "configurations:*Release*", "system:linux" }
            links {"../lib/NWengineCore64", "freetype", "GLEW" }
    filter "system:windows"
        links {"dwmapi.lib"}
    filter "system:linux"
        links {
            "X11",
            "Xi",
            "GL",
        }

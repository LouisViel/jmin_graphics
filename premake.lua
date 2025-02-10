solution "Minicraft"
	location "."
	configurations {  "Debug", "Release" }
	platforms { "x64" }
	startproject "Minicraft"

project "Minicraft"
	system "Windows"
	kind "WindowedApp"
	architecture "x86_64"
	language "C++"
	cppdialect "C++17"

	targetname "minicraft"
	targetdir "Bin/$(Platform)/$(Configuration)"
	objdir "Obj/$(Platform)/$(Configuration)"
	debugdir "Resources"

	dependson "DirectXTK_Desktop_2022"

	pchheader "pch.h"
	pchsource "Sources/pch.cpp"
	
	files {
		"Sources/**.h",
		"Sources/**.cpp",
		"Resources/Shaders/**.hlsl",
		"main.cpp"
	}

	-- ImGui files
	files {
		"Deps/imgui/*.h",
		"Deps/imgui/*.cpp",
		"Deps/imgui/backends/imgui_impl_win32.*",
		"Deps/imgui/backends/imgui_impl_dx11.*",
		"Deps/imgui/misc/cpp/imgui_stdlib.*",
	}
	includedirs {
		"Sources",
		"Deps/DirectXTK/Inc",
		"Deps/PerlinNoise",
		"Deps/imgui"
	}

	links {
		"d3d11.lib",
		"dxgi.lib",
		"DirectXTK.lib"
	}

	filter "files:**imgui**.cpp"
    	flags {"NoPCH"}
	filter "files:**.hlsl or **.hlsli" 
	   shadermodel "5.0"
	   shaderobjectfileoutput "%%(RelativeDir)Compiled/%%(Filename).cso"
	filter "files:**ps_**.hlsl" 
	   shadertype "Pixel"
	filter "files:**vs_**.hlsl" 
	   shadertype "Vertex"
	filter "files:**_ps**.hlsl" 
	   shadertype "Pixel"
	filter "files:**_vs**.hlsl" 
	   shadertype "Vertex"
	filter {}
	
	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"
		libdirs {
			"Deps/DirectXTK/Bin/Desktop_2022/x64/Debug/",
		}

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
		libdirs {
			"Deps/DirectXTK/Bin/Desktop_2022/x64/Release/",
		}

externalproject "DirectXTK_Desktop_2022"
	location "Deps/DirectXTK"
	uuid "E0B52AE7-E160-4D32-BF3F-910B785E5A8E"
	kind "StaticLib"
	language "C++"
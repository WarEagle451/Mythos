project "Testbed"
	language "C++"
	cppdialect "C++20"
	staticruntime "Off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"src/**.hpp",
		"src/**.cpp"
	}

	includedirs {
		"src",
		"%{wks.location}/Mythos/src",
		"%{wks.location}/Mythos/vendor/spdlog/include",
		"%{wks.location}/Mythos/vendor"
	}

	links {
		"Mythos"
	}
	
	postbuildcommands {
		"{COPYDIR} \"%{LibraryDir.MythosDLL}\" \"%{cfg.targetdir}\""
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "MYL_BUILD_DEBUG"
		kind "ConsoleApp"
		runtime "Debug"
		symbols "On"

	filter "configurations:Distribution"
		defines "MYL_BUILD_DISTRIBUTION"
		kind "WindowedApp"
		runtime "Release"
		optimize "On"
		entrypoint "mainCRTStartup"

	filter "configurations:Release"
		defines "MYL_BUILD_RELEASE"
		kind "WindowedApp"
		runtime "Release"
		optimize "On"
		entrypoint "mainCRTStartup"
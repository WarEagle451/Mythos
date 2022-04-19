LibraryDir = {}
LibraryDir["MythosDLL"] = ("%{wks.location}/bin/" .. outputdir .. "/Mythos")

project "TestBed"
	kind "ConsoleApp"
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
		defines "MYL_DEBUG_BUILD"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "MYL_RELEASE_BUILD"
		runtime "Release"
		optimize "On"

	filter "configurations:Distribution"
		defines "MYL_DISTRIBUTION_BUILD"
		runtime "Release"
		optimize "On"
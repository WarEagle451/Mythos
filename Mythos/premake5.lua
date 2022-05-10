project "Mythos"
	kind "SharedLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "Off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"src/**.hpp",
		"src/**.cpp"
	}
	
	defines {
		"MYL_EXPORT",
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs {
		"src",
		"vendor/spdlog/include",
		"%{IncludeDir.VulkanSDK}"
	}
	
	links {
		"%{Library.Vulkan}"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "MYL_BUILD_DEBUG"
		runtime "Debug"
		symbols "On"
	
	filter "configurations:Release"
		defines "MYL_BUILD_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Distribution"
		defines "MYL_BUILD_DISTRIBUTION"
		runtime "Release"
		optimize "On"
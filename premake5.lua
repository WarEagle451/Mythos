include "./vendor/premake/premake_customization/solution_items.lua"

workspace "Mythos"
	architecture "x86_64"
	startproject "Mythos"

	configurations {
		"Debug",
		"Release",
		"Distribution"
	}
	
	solution_items {
		".editorconfig"
	}
	
	flags {
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "vendor/premake"
group ""

include "Mythos"
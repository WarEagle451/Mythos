include "./vendor/premake/premake_customization/solution_items.lua"
include "dependencies.lua"

workspace "Mythos"
	architecture "x86_64"
	startproject "Testbed"

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
include "Testbed"
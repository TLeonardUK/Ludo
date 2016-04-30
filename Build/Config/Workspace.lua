-- ---------------------------------------------------------------
--		Ludo Engine Premake Configuration 
--		Copyright TwinDrills 2016
-- ---------------------------------------------------------------

-- ---------------------------------------------------------------
--		Workspace configuration
-- ---------------------------------------------------------------	
workspace "Ludo"
	configurations 
	{ 
		"Debug", 
		"Release", 
		"Shipping" 
	}	
	platforms 
	{ 
		"x64", 
		"x86" 
	}	
	startproject("Runner")
	location(LudoRoot .. "/Build/ProjectFiles")
		
-- ---------------------------------------------------------------
--		Global build configuration
-- ---------------------------------------------------------------		
filter "configurations:Debug"
	defines 
	{ 
		"LD_DEBUG_BUILD" 
	}	
	flags 
	{ 
		"FatalCompileWarnings",	
		"MultiProcessorCompile",
		"ShadowedVariables",
		"Symbols",
		"StaticRuntime",
		"C++11"
	}
	rtti "Off"
	optimize "Off"	
	strictaliasing "Level3"
	runtime "Debug"
	
filter "configurations:Release"
	defines 
	{ 
		"LD_RELEASE_BUILD" 
	}	
	flags 
	{ 
		"FatalCompileWarnings",	
		"MultiProcessorCompile",
		"ShadowedVariables",
		"Symbols",
		"StaticRuntime",
		"C++11"
	}
	rtti "Off"
	optimize "Debug"
	strictaliasing "Level3"
	runtime "Release"
	
filter "configurations:Shipping"
	defines 
	{ 
		"LD_SHIPPING_BUILD" 
	}	
	flags 
	{ 
		"FatalCompileWarnings",	
		"MultiProcessorCompile",
		"LinkTimeOptimization",
		"NoRuntimeChecks",
		"ShadowedVariables",
		"Symbols",
		"StaticRuntime",
		"C++11"
	}
	rtti "Off"
	optimize "Full"
	strictaliasing "Level3"
	runtime "Release"
				
filter "architecture:x64"
	defines 
	{ 
		"LD_ARCHITECTURE_X64" 
	}		
filter "architecture:x86"
	defines 
	{ 
		"LD_ARCHITECTURE_X86" 
	}		
	
filter "system:windows"
	defines 
	{ 
		"LD_PLATFORM_WINDOWS", 
		"LD_LITTLE_ENDIAN",
		"_HAS_ITERATOR_DEBUGGING=0",		-- To slow, we want debug configs to still be realtime.
		"_SECURE_SCL=0",					-- Additional checks disabled for iterators.
		--"_NO_DEBUG_HEAP=1"				-- On the fence about this one, seems worth the performance hit.
	}
	toolset "v140"
filter "system:macosx"
	defines 
	{ 
		"LD_PLATFORM_MACOS", 
		"LD_LITTLE_ENDIAN" 
	}
filter "system:linux"
	defines 
	{ 
		"LD_PLATFORM_LINUX", 
		"LD_LITTLE_ENDIAN" 
	}

-- This is gross, but premakes forcedincludes is broken. It turns the paths into relative paths
-- which will never work correctly with nested project directories as the forced include is always
-- relative to the file being compiled. So we pass the includes manually.
filter "system:windows"
	buildoptions { "/FI\"$(SolutionDir)../../Engine/Source/Core/Build/Global.h\"" }
filter "system:macosx"
	buildoptions { "-include \"$(SolutionDir)../../Engine/Source/Core/Build/Global.h\"" }
filter "system:linux"
	buildoptions { "-include \"$(SolutionDir)../../Engine/Source/Core/Build/Global.h\"" }

filter {}
		
includedirs 
{
	(LudoRoot .. "/Engine/Source/")
}

-- ---------------------------------------------------------------
--		NatVis Support for VS2015
-- ---------------------------------------------------------------
if (_ACTION == "vs2015") then
	ok, err = os.copyfile(
		LudoRoot .. "/Tools/VisualStudio/Ludo.natvis", 
		os.realpath(os.getenv("USERPROFILE") .. "/Documents/Visual Studio 2015/Visualizers/Ludo.natvis")
	)
	if (ok ~= true) then
		error("Failed to copy natvis with error: " .. err)
	end
end
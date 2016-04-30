-- ---------------------------------------------------------------
--		Ludo Engine Premake Configuration 
--		Copyright TwinDrills 2016
-- ---------------------------------------------------------------
group("Engine/Binaries")
	
	project "Runner"
		kind "WindowedApp"
		language "C++"
		location(LudoRoot .. "/Build/ProjectFiles/Engine/Binaries/Runner/")
		targetdir(LudoRoot .. "/Engine/Binaries/%{cfg.system:lower()}.%{cfg.buildcfg:lower()}.%{cfg.architecture:lower()}/")
		objdir(LudoRoot .. "/Build/ProjectFiles/Temp/Engine/Binaries/Runner/%{cfg.system:lower()}.%{cfg.buildcfg:lower()}.%{cfg.architecture:lower()}/")

		dependson
		{		
			"Framework" 
		}

		links
		{
			"Core", 
			"Framework",
			
			"Imm32.lib",			-- IME integration.
			"Winmm.lib",			-- General media functionality.
			"dbghelp.lib",			-- Symbol loading and resolving at runtime.
			"ws2_32.lib",			-- Socket networking.
			"Shell32.lib",			-- General shell functionality.
			"WbemUuid.lib",			-- Windows management instrumentation code. Used to query system information.
			"comsuppw.lib",			-- General COM integration functionality.
			"Advapi32.lib"			-- Authentication/Security routines.
		}
					
		filter "system:windows"
			links( "Windows" )
		filter {}

		files 
		{ 
			(LudoRoot .. "/Engine/Source/Runner/**.h"), 
			(LudoRoot .. "/Engine/Source/Runner/**.cpp") 
		}

	project "Editor"
		kind "WindowedApp"
		language "C++"
		location(LudoRoot .. "/Build/ProjectFiles/Engine/Binaries/Editor/")
		targetdir(LudoRoot .. "/Engine/Binaries/%{cfg.system:lower()}.%{cfg.buildcfg:lower()}.%{cfg.architecture:lower()}/")
		objdir(LudoRoot .. "/Build/ProjectFiles/Temp/Engine/Binaries/Editor/%{cfg.system:lower()}.%{cfg.buildcfg:lower()}.%{cfg.architecture:lower()}/")

		dependson
		{		
			"Framework" 
		}

		links
		{
			"Core", 
			"Framework",
			
			"Imm32.lib",			-- IME integration.
			"Winmm.lib",			-- General media functionality.
			"dbghelp.lib",			-- Symbol loading and resolving at runtime.
			"ws2_32.lib",			-- Socket networking.
			"Shell32.lib",			-- General shell functionality.
			"WbemUuid.lib",			-- Windows management instrumentation code. Used to query system information.
			"comsuppw.lib",			-- General COM integration functionality.
			"Advapi32.lib"			-- Authentication/Security routines.
		} 
					
		filter "system:windows"
			links( "Windows" )		
		filter {}

		files 
		{ 
			(LudoRoot .. "/Engine/Source/Editor/**.h"), 
			(LudoRoot .. "/Engine/Source/Editor/**.cpp") 
		}
		
group("Engine/Engine")

	project "Core"
		kind "StaticLib"
		language "C++"
		location(LudoRoot .. "/Build/ProjectFiles/Engine/Engine/Core/")
		targetdir(LudoRoot .. "/Build/ProjectFiles/Temp/Engine/Engine/Core/%{cfg.system:lower()}.%{cfg.buildcfg:lower()}.%{cfg.architecture:lower()}/Output/")
		objdir(LudoRoot .. "/Build/ProjectFiles/Temp/Engine/Engine/Core/%{cfg.system:lower()}.%{cfg.buildcfg:lower()}.%{cfg.architecture:lower()}/")

		files 
		{ 
			(LudoRoot .. "/Engine/Source/Core/**.h"), 
			(LudoRoot .. "/Engine/Source/Core/**.cpp") 
		}

	project "Framework"
		kind "StaticLib"
		language "C++"
		location(LudoRoot .. "/Build/ProjectFiles/Engine/Engine/Framework/")
		targetdir(LudoRoot .. "/Build/ProjectFiles/Temp/Engine/Engine/Framework/%{cfg.system:lower()}.%{cfg.buildcfg:lower()}.%{cfg.architecture:lower()}/Output/")
		objdir(LudoRoot .. "/Build/ProjectFiles/Temp/Engine/Engine/Framework/%{cfg.system:lower()}.%{cfg.buildcfg:lower()}.%{cfg.architecture:lower()}/")
		dependson( "Core" )

		files 
		{ 
			(LudoRoot .. "/Engine/Source/Framework/**.h"), 
			(LudoRoot .. "/Engine/Source/Framework/**.cpp")
		}
	  	  
group("Engine/Engine/Platforms")

	project "Windows"
		kind "StaticLib"
		language "C++"
		location(LudoRoot .. "/Build/ProjectFiles/Engine/Engine/Platforms/Windows/")
		targetdir(LudoRoot .. "/Build/ProjectFiles/Temp/Engine/Engine/Platforms/Windows/%{cfg.system:lower()}.%{cfg.buildcfg:lower()}.%{cfg.architecture:lower()}/Output/")
		objdir(LudoRoot .. "/Build/ProjectFiles/Temp/Engine/Engine/Platforms/Windows/%{cfg.system:lower()}.%{cfg.buildcfg:lower()}.%{cfg.architecture:lower()}/")
		dependson( "Core" )

		files 
		{ 
			(LudoRoot .. "/Engine/Source/Platforms/Windows/**.h"), 
			(LudoRoot .. "/Engine/Source/Platforms/Windows/**.cpp") 
		}

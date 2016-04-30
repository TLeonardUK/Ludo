-- ---------------------------------------------------------------
--		Ludo Engine Premake Configuration 
--		Copyright TwinDrills 2016
-- ---------------------------------------------------------------
group("Projects/Samples")
							  
	project "Basic"
		kind "StaticLib"
		language "C++"
		location(LudoRoot .. "/Build/ProjectFiles/Projects/Samples/Basic/")
		targetdir(LudoRoot .. "/Projects/Samples/Basic/Binaries/%{cfg.system:lower()}.%{cfg.buildcfg:lower()}.%{cfg.architecture:lower()}/")
		objdir(LudoRoot .. "/Build/ProjectFiles/Temp/Projects/Samples/Basic/%{cfg.system:lower()}.%{cfg.buildcfg:lower()}.%{cfg.architecture:lower()}/")

		dependson
		{		
			"Framework" 
		}
		
		files 
		{ 
			(LudoRoot .. "/Projects/Samples/Basic/Source/**.h"), 
			(LudoRoot .. "/Projects/Samples/Basic/Source/**.cpp") 
		}

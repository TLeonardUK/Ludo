-- ---------------------------------------------------------------
--		Ludo Engine Premake Configuration 
--		Copyright TwinDrills 2016
-- ---------------------------------------------------------------
os.chdir("../../")

group("Projects/Samples")
							  
	project "Basic"
		kind "StaticLib"
		language "C++"
		location "Build/ProjectFiles/Projects/Samples/Basic/"
		targetdir "Projects/Samples/Basic/Binaries/%{cfg.system:lower()}.%{cfg.buildcfg:lower()}.%{cfg.architecture:lower()}/"
		objdir "Build/ProjectFiles/Temp/Projects/Samples/Basic/%{cfg.system:lower()}.%{cfg.buildcfg:lower()}.%{cfg.architecture:lower()}/"

		dependson
		{		
			"Framework" 
		}
		
		files 
		{ 
			"Projects/Samples/Basic/Source/**.h", 
			"Projects/Samples/Basic/Source/**.cpp" 
		}

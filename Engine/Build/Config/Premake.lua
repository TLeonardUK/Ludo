-- ---------------------------------------------------------------
--		Ludo Engine Premake Configuration 
--		Copyright TwinDrills 2016
-- ---------------------------------------------------------------
os.chdir("../../")

group("Engine/Binaries")
	
	project "Runner"
		kind "WindowedApp"
		language "C++"
		location "Build/ProjectFiles/Engine/Binaries/Runner/"
		targetdir "Engine/Binaries/%{cfg.system:lower()}.%{cfg.buildcfg:lower()}.%{cfg.architecture:lower()}/"
		objdir "Build/ProjectFiles/Temp/Engine/Binaries/Runner/%{cfg.system:lower()}.%{cfg.buildcfg:lower()}.%{cfg.architecture:lower()}/"

		dependson
		{		
			"Framework" 
		}

		links
		{
			"Core", 
			"Framework" 
		}
					
		filter "system:windows"
			links( "Windows" )
		filter {}

		files 
		{ 
			"Engine/Source/Runner/**.h", 
			"Engine/Source/Runner/**.cpp" 
		}

	project "Editor"
		kind "WindowedApp"
		language "C++"
		location "Build/ProjectFiles/Engine/Binaries/Editor/"
		targetdir "Engine/Binaries/%{cfg.system:lower()}.%{cfg.buildcfg:lower()}.%{cfg.architecture:lower()}/"
		objdir "Build/ProjectFiles/Temp/Engine/Binaries/Editor/%{cfg.system:lower()}.%{cfg.buildcfg:lower()}.%{cfg.architecture:lower()}/"

		dependson
		{		
			"Framework" 
		}

		links
		{
			"Core", 
			"Framework" 
		} 
					
		filter "system:windows"
			links( "Windows" )		
		filter {}

		files 
		{ 
			"Engine/Source/Editor/**.h", 
			"Engine/Source/Editor/**.cpp" 
		}
		
group("Engine/Engine")

	project "Core"
		kind "StaticLib"
		language "C++"
		location "Build/ProjectFiles/Engine/Engine/Core/"
		targetdir "Build/ProjectFiles/Temp/Engine/Engine/Core/%{cfg.system:lower()}.%{cfg.buildcfg:lower()}.%{cfg.architecture:lower()}/Output/"
		objdir "Build/ProjectFiles/Temp/Engine/Engine/Core/%{cfg.system:lower()}.%{cfg.buildcfg:lower()}.%{cfg.architecture:lower()}/"

		files 
		{ 
			"Engine/Source/Core/**.h", 
			"Engine/Source/Core/**.cpp" 
		}

	project "Framework"
		kind "StaticLib"
		language "C++"
		location "Build/ProjectFiles/Engine/Engine/Framework/"
		targetdir "Build/ProjectFiles/Temp/Engine/Engine/Framework/%{cfg.system:lower()}.%{cfg.buildcfg:lower()}.%{cfg.architecture:lower()}/Output/"
		objdir "Build/ProjectFiles/Temp/Engine/Engine/Framework/%{cfg.system:lower()}.%{cfg.buildcfg:lower()}.%{cfg.architecture:lower()}/"
		dependson( "Core" )

		files 
		{ 
			"Engine/Source/Framework/**.h", 
			"Engine/Source/Framework/**.cpp" 
		}
	  	  
group("Engine/Engine/Platforms")

	project "Windows"
		kind "StaticLib"
		language "C++"
		location "Build/ProjectFiles/Engine/Engine/Platforms/Windows/"
		targetdir "Build/ProjectFiles/Temp/Engine/Engine/Platforms/Windows/%{cfg.system:lower()}.%{cfg.buildcfg:lower()}.%{cfg.architecture:lower()}/Output/"
		objdir "Build/ProjectFiles/Temp/Engine/Engine/Platforms/Windows/%{cfg.system:lower()}.%{cfg.buildcfg:lower()}.%{cfg.architecture:lower()}/"
		dependson( "Core" )

		files 
		{ 
			"Engine/Source/Platforms/Windows/**.h", 
			"Engine/Source/Platforms/Windows/**.cpp" 
		}

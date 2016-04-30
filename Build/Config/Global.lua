-- ---------------------------------------------------------------
--		Ludo Engine Premake Configuration 
--		Copyright TwinDrills 2016
-- ---------------------------------------------------------------

-- Figure out the root directory for ludo. This makes assumptions
-- on directory layout, so don't fiddle :)
LudoRoot = os.realpath(os.getcwd() .. "/../../")
print("Ludo Root: " .. LudoRoot)

-- Find all engine/project folders that contain a premake lua file in the appropriate place.
local matches = os.matchdirs("../../**/") 

if (#matches == 0) then
	error("No project directories found.")
end

include("Workspace.lua")

print("Project Directories:")
for k,v in pairs(matches) do
	local premakeFile = v .. "/Build/Config/Premake.lua"
	if (os.isfile(premakeFile)) then
		include(premakeFile)
	end
end

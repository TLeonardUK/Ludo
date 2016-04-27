-- ---------------------------------------------------------------
--		Ludo Engine Premake Configuration 
--		Copyright TwinDrills 2016
-- ---------------------------------------------------------------

-- Find all engine/project folders that contain a premake lua file in the appropriate place.
matches = os.matchdirs("../../**/Build/Config/Premake.lua") 

if (#matches == 0) then
	error("No project directories found.")
end

print("Project Directories:")
for k,v in pairs(matches) do
	print(k)
	print(v)
end

-- 
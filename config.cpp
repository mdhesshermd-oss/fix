class CfgPatches
{
	class SHADOWFOX_CarLock
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Scripts", "DZ_Vehicles_Wheeled", "JM_CF_Scripts"};
	};
};

class CfgMods
{
	class SHADOWFOX_CarLock
	{
		dir = "SHADOWFOX_CarLock";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "SHADOWFOX CarLock";
		credits = "SHADOWFOX";
		author = "SHADOWFOX";
		authorID = "0";
		version = "1.0";
		extra = 0;
		type = "mod";
		dependencies[] = {"Game", "World", "Mission"};
		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = {"SHADOWFOX_CarLock/3_Game"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"SHADOWFOX_CarLock/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"SHADOWFOX_CarLock/5_Mission"};
			};
		};
	};
};

class CfgInputs
{
	class UASHADOWFOX_CarLockToggle
	{
		name = "SHADOWFOX CarLock Toggle";
		description = "Toggle Vehicle Lock";
		visible = 1;
		container = "PrivacySettings";
		slot = "SHADOWFOX_CarLock";
	};
};

class PrivacySettings
{
	class SHADOWFOX_CarLock
	{
		class Inputs
		{
			class SHADOWFOX_CarLockToggle
			{
				key = "UASHADOWFOX_CarLockToggle";
				name = "SHADOWFOX CarLock Toggle";
				description = "Toggle Vehicle Lock";
				default_key = "KC_U";
			};
		};
	};
};

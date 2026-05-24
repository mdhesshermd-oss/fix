class CfgPatches
{
	class SHADOWFOX_CarLock
	{
		units[] = {"SHADOWFOX_CarLock"};
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

class CfgVehicles
{
	class Inventory_Base;
	class SHADOWFOX_CarLock: Inventory_Base
	{
		scope = 2;
		displayName = "$STR_shadowfox_carlock_item_name";
		descriptionShort = "$STR_shadowfox_carlock_item_desc";
		model = "SHADOWFOX_CarLock/Data/CarLock.p3d";
		rotationFlags = 17;
		weight = 200;
		itemSize[] = {2, 2};
		itemBehaviour = 1;
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"SHADOWFOX_CarLock/Data/Textures/codelock_co.paa"};
	};
};

class CfgSoundSets
{
	class SHADOWFOX_CarAlarm_SoundSet
	{
		soundShaders[] = {"SHADOWFOX_CarAlarm_SoundShader"};
	};
	class SHADOWFOX_CarLock_SoundSet
	{
		soundShaders[] = {"SHADOWFOX_CarLock_SoundShader"};
	};
	class SHADOWFOX_CarUnlock_SoundSet
	{
		soundShaders[] = {"SHADOWFOX_CarUnlock_SoundShader"};
	};
};

class CfgSoundShaders
{
	class SHADOWFOX_CarAlarm_SoundShader
	{
		samples[] = {{"SHADOWFOX_CarLock/Data/Sounds/caralarm", 1}};
		volume = 1.0;
		range = 150;
	};
	class SHADOWFOX_CarLock_SoundShader
	{
		samples[] = {{"SHADOWFOX_CarLock/Data/Sounds/carlock", 1}};
		volume = 1.0;
		range = 30;
	};
	class SHADOWFOX_CarUnlock_SoundShader
	{
		samples[] = {{"SHADOWFOX_CarLock/Data/Sounds/carunlock", 1}};
		volume = 1.0;
		range = 30;
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

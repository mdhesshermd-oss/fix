class CfgPatches
{
	class LBmaster_HoverLoot
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]={"LBmaster_Core"};
	};
};

class CfgMods
{
	class LBmaster_HoverLoot
	{
		dir="LBmaster_HoverLoot";
		picture="";
		action="";
		hideName=0;
		hidePicture=1;
		name="LBmaster_HoverLoot";
		author="LBmaster";
		credits="LBmaster";
		authorID="0";
		version="1.0";
		type="mod";
		inputs = "LBmaster_HoverLoot/inputsLBmaster.xml";
		dependencies[]=
		{
			"Game",
			"World",
			"Mission"
		};
		class defs
		{
			class gameScriptModule
			{
				value="";
				files[]=
				{
					"LBmaster_HoverLoot/scripts/3_Game"
				};
			};
			class worldScriptModule
			{
				value="";
				files[]=
				{
					"LBmaster_HoverLoot/scripts/4_World"
				};
			};
			class missionScriptModule
			{
				value="";
				files[]=
				{
					"LBmaster_HoverLoot/scripts/5_Mission"
				};
			};
		};
	};
};

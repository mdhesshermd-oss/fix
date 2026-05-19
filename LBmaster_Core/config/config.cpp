class CfgPatches {

	class LBmaster_Core {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {
			"DZ_Data"
		};
	};

};
class CfgMods {
	class LBmaster_Core {
		dir = "LBmaster_Core";
		name = "LBmaster Core Library";
		version = "3.0";
		type = "mod";
		author = "LBmaster";
		credits = "LBmaster";
		authorID = "76561198141097113";
		hideName = 0;
		hidePicture = 0;
		tooltip = "LBmasters Core Script Files Library";
		action = "https://lbmaster.de";
		inputs = "LBmaster_Core/inputsLBmaster.xml";
		dependencies[] = {
			"Game",
			"World",
			"Mission"
		};
		defines[] = {
			"LBmaster_Rework",
			"LBmaster_QrCode"
		};
		class defs {
			class widgetStyles {
				files[] = {
					"LBmaster_Core/gui/styles/lbstyles.styles"
				};
			};
			class imageSets {
				files[] = {
					"LBmaster_Core/gui/imagesets/lb_core_set.imageset",
					"LBmaster_Core/gui/imagesets/loading.imageset"
				};
			};
			class gameScriptModule {
				value = "";
				files[] = {
					"LBmaster_Core/scripts/3_Game"
				};
			};
			class worldScriptModule {
				value = "";
				files[] = {
					"LBmaster_Core/scripts/4_World"
				};
			};
			class missionScriptModule {
				value = "";
				files[] = {
					"LBmaster_Core/scripts/5_Mission"
				};
			};
		};
	};
};

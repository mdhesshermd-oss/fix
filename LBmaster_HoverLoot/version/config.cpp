class CfgPatches {
    class Hover_Loot_Version {
        requiredVersion = 1.0;
        requiredAddons[] = {};
    };
};
class CfgMods {
    class Hover_Loot_Version {
        dir="Hover_Loot_Version";
        name="Hover_Loot_Version";
        type="mod";
        dependencies[]=
        {
            "gamelib"
        };
        class defs
        {
            class gameLibScriptModule
            {
                value="";
                files[]=
                {
                    "LBmaster_HoverLoot/version/scripts"
                };
            };
        };
    };
};
class LBVersions {
    class LBmaster_HoverLoot {
        version= "1.1.1";
    };
};

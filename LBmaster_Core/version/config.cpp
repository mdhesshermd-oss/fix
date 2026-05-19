class CfgPatches {
    class Core_Version {
        requiredVersion = 1.0;
        requiredAddons[] = {};
    };
};
class CfgMods {
    class Core_Version {
        dir="Core_Version";
        name="Core_Version";
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
                    "LBmaster_Core/version/scripts"
                };
            };
        };
    };
};
class LBVersions {
    class LBmaster_Core {
        version= "4.2.1";
    };
};

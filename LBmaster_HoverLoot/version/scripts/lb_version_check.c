#ifndef LB_VERSION_CHECK
#define LB_VERSION_CHECK
string lb_version_list;
#endif
int lb_version_Hover_Loot = lb_version_Hover_Loot_();
int lb_version_Hover_Loot_() {
    Print("Loaded Hover Loot (Client PBO) Version 1.1.1:stable");
    lb_version_list += "Hover_Loot#1.1.1:stable;";
    return 0;
}


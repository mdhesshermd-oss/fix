#ifndef LB_VERSION_CHECK
#define LB_VERSION_CHECK
string lb_version_list;
#endif
int lb_version_Core = lb_version_Core_();
int lb_version_Core_() {
    Print("Loaded Core (Client PBO) Version 4.2.1:stable");
    lb_version_list += "Core#4.2.1:stable;";
    return 0;
}


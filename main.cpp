#include <iostream>
#include "event_timestamp.hpp"

int main()
{
    event_timestamp et1;
    event_timestamp<CLOCK_REALTIME> et2;

    if (et1.is_valid()) {
        std::cout << et1.nanoseconds_resolution() << "\n";
    }
    if (et2.is_valid()) {
        std::cout << et2.nanoseconds_resolution() << "\n";
    }

    auto et3 = et2;
    if (et3.is_valid()) {
        std::cout << et3.nanoseconds_resolution() << "\n";
    }

    tm_extended tmx_gm;
    get_gmtime(et3, tmx_gm);
    tm_extended tmx_local;
    get_localtime(et3, tmx_local);

    char buff[128];

    strftime(buff, sizeof(buff), "%FT%T %z %Z", (const tm *) tmx_gm);
    std::cout << buff << "\n";

    strftime(buff, sizeof(buff), "%FT%T %z %Z", (const tm *) tmx_local);
    std::cout << buff << "\n";
    return 0;
}

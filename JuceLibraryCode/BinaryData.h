/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   UbuntuBold_ttf;
    const int            UbuntuBold_ttfSize = 331612;

    extern const char*   UbuntuLight_ttf;
    const int            UbuntuLight_ttfSize = 413600;

    extern const char*   large_church_wav;
    const int            large_church_wavSize = 768044;

    extern const char*   large_hall_wav;
    const int            large_hall_wavSize = 3341002;

    extern const char*   medium_chamber_wav;
    const int            medium_chamber_wavSize = 1382674;

    extern const char*   medium_hall_wav;
    const int            medium_hall_wavSize = 1588186;

    extern const char*   open_air_wav;
    const int            open_air_wavSize = 415144;

    extern const char*   logo_dial_png;
    const int            logo_dial_pngSize = 27567;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Number of elements in the namedResourceList array.
    const int namedResourceListSize = 8;

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes) throw();
}

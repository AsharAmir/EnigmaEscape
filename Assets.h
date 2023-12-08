
#ifndef ASSETS_H
#define ASSETS_H

#include <string>
using namespace std;

namespace Assets
{
    //SYMBOLS
    const string leftArrow = "\xE2\x97\x84";
    const string upArrow = "\xE2\x96\xb2";
    const string rightArrow = "\xE2\x96\xba";
    const string downArrow = "\xE2\x96\xbc";
    const string blackDiamond = "\xE2\x97\x86";
    const string healthBar = "\x1b[32m\u25AE\x1b[0m";
    const string coin = "\033[1;33m$\033[0m";
    const string redFire = "\x1b[91m🔥\x1b[0m";
    const string warning = "\x1b[91m⚠\x1b[0m";


    //COLORS
    const string greenColor = "\x1b[32m";
    const string blueColor = "\x1b[34m";
    const string purpleColor = "\x1b[35m";
    const string redColor = "\x1b[31m";
    const string cyanColor = "\x1b[36m";
    const string yellowColor = "\x1b[33m";
    const string whiteColor = "\x1b[37m";
    const string magentaColor = "\x1b[35m";
    const string brightBlackColor = "\x1b[90m";
    const string brightRedColor = "\x1b[91m";
    const string brightGreenColor = "\x1b[92m";
    const string brightYellowColor = "\x1b[93m";
    const string brightBlueColor = "\x1b[94m";
    const string brightMagentaColor = "\x1b[95m";
    const string brightCyanColor = "\x1b[96m";
    const string brightWhiteColor = "\x1b[97m";
}

#endif

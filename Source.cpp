

using namespace std;
#include "Header.h"

int main()
{

    // game.run();
    /*game.autoRun();*/
    if (!isFullScreen)
    {
        // to trigger fullscreen
        keybd_event(VK_F11, 0, KEYEVENTF_EXTENDEDKEY, 0); // press F11 key
        keybd_event(VK_F11, 0, KEYEVENTF_KEYUP, 0);       // release f11 key
        isFullScreen = true;
    }
    cout << "Input player name: " << endl;
    getline(cin, playerName);
    printWelcomeScreen();
    choiceHandler();

    return 0;
}

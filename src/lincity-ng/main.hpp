#ifndef __MAIN_HPP__
#define __MAIN_HPP__

enum MainState {
    MAINMENU, INGAME, QUIT
};

void initVideo(int width = 800, int height = 600);
void flipScreenBuffer();

class Painter;

/** global instance of currently used painter object.
 * Note: Don't use this in your components, but the one passed in the draw
 *       function!
 */
extern Painter* painter;

#endif


#ifndef __MAIN_HPP__
#define __MAIN_HPP__

#include "gui/TinyGetText/TinyGetText.hpp"

enum MainState {
    MAINMENU, INGAME, QUIT
};

void initVideo(int width, int height);
void flipScreenBuffer();

class Painter;

/** global instance of currently used painter object.
 * Note: Don't use this in your components, but the one passed in the draw
 *       function!
 */
extern Painter* painter;

/// global instance of tinygettext
extern TinyGetText::DictionaryManager dictionaryManager;

#endif


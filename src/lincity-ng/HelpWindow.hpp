#ifndef __HELPWINDOW_HPP__
#define __HELPWINDOW_HPP__

#include <string>

class Desktop;
class Paragraph;

class HelpWindow
{
public:
    HelpWindow(Desktop* desktop);
    ~HelpWindow();

    void showTopic(const std::string& topic);
    void update();

private:
    void linkClicked(Paragraph* paragraph, const std::string& href);
    std::string getHelpFile(const std::string& topic);

    Desktop* desktop;
    std::string nextTopic;
};

#endif

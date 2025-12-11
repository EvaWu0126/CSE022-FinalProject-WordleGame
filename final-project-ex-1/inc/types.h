#ifndef TYPES_H
#define TYPES_H

#include <bobcat_ui/all.h>
#include <bobcat_ui/bobcat_ui.h>
#include <bobcat_ui/button.h>
#include <bobcat_ui/input.h>
#include <bobcat_ui/menu.h>
#include <bobcat_ui/textbox.h>
#include <bobcat_ui/window.h>
#include <string>

struct Guess {
    bobcat::TextBox *wordGuess;
    std::string word;
};

struct Game {
    std::string name;
    std::string theWord;
    std::string status;
};

#endif
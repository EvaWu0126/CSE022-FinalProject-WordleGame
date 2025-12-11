#ifndef APPLICATION_H
#define APPLICATION_H

#include "types.h"
#include <bobcat_ui/all.h>
#include <bobcat_ui/bobcat_ui.h>
#include <bobcat_ui/button.h>
#include <bobcat_ui/input.h>
#include <bobcat_ui/list_box.h>
#include <bobcat_ui/menu.h>
#include <bobcat_ui/textbox.h>
#include <bobcat_ui/window.h>

class Application : public bobcat::Application_ {
    // main game screen window:
    bobcat::Window *mainWindow;

    bobcat::Menu *mainMenu;
    bobcat::MenuItem *instrucItem;
    bobcat::MenuItem *statsItem;

    // start screen screen window (subwindow):
    bobcat::Window *startScreenWindow;

    bobcat::TextBox *mainTitle;
    bobcat::Button *singlePlayerButton;
    bobcat::Button *multiPlayerButton;

    // game interface window (subwindow):
    bobcat::Window *gameWindow;

    bobcat::Button *checkWordButton;
    bobcat::Button *deleteButton;
    bobcat::TextBox *player1Status;
    bobcat::TextBox *player2Status;

    std::vector<bobcat::Button *> keyboard;
    std::vector<Guess> playerGuess;
    std::vector<std::string> words;
    std::vector<Game> stats;

    std::string targetWord;
    std::string playerStatus;

    // login (user name input) screen window:
    bobcat::Window *loginWindow;

    bobcat::TextBox *player1Text;
    bobcat::TextBox *player2Text;
    bobcat::Input *name1Input;
    bobcat::Input *name2Input;

    bobcat::Button *startButton;

    // extra window for menu pop-ups:
    bobcat::Window *instructionWindow;
    bobcat::TextBox *instructionsText;

    bobcat::Window *statsWindow;
    bobcat::ListBox *gameDataList;

    // helper function:
    void initKeyboard(std::string row, int y);
    void initGuesses();
    void clearGameRecord();
    void checkWhosTurn();
    void checkLetterPosition();
    bool searchInList(const std::string &word);
    bool containsSubstring(const std::string& mainString, const std::string& subString);
    void updateStats();
    void readGameDataFromFile();

    std::string getRandomWord();
    std::string makeUpper(const std::string &s);
    std::string makeLower(const std::string &s);

    // event handler:
    bool handleGameModeClick(bobcat::Widget *sender);
    void handleGameStartClick(bobcat::Widget *sender);
    void handleKeyboardClick(bobcat::Widget *sender);
    void handleCheckClick(bobcat::Widget *sender);
    void handleDeleteButtonClick(bobcat::Widget *sender);
    void handleMenuClick(bobcat::Widget *sender);
    

public:
    Application();

    friend struct ::AppTest;
};

#endif
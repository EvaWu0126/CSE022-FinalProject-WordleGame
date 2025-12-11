#include <Application.h>
#include <FL/Enumerations.H>
#include <FL/Fl_Menu_Item.H>
#include <RandomSupport.h>
#include <bobcat_ui/bobcat_ui.h>
#include <bobcat_ui/button.h>
#include <bobcat_ui/list_box.h>
#include <bobcat_ui/menu.h>
#include <bobcat_ui/textbox.h>
#include <bobcat_ui/window.h>
#include <cctype>
#include <climits>
#include <cstddef>
#include <fstream>
#include <math.h>
#include <string>

using namespace std;
using namespace bobcat;

Application::Application() {
    // main game screen window:
    mainWindow = new Window(100, 100, 400, 500, "Wordle Game");

    mainMenu = new Menu();
    instrucItem = new MenuItem("Help/Game Instruction");
    statsItem = new MenuItem("Statistics/Player Info");
    mainMenu->addItem(instrucItem);
    mainMenu->addItem(statsItem);

    // generate a random word:
    targetWord = getRandomWord();
    cout << "the target word for this round is: " << targetWord << endl;

    // game window (subwindow)
    gameWindow = new Window(0, 25, 400, 500);

    initKeyboard("QWERTYUIOP", 370);
    initKeyboard("ASDFGHJKL", 400);
    initKeyboard("ZXCVBNM", 430);
    checkWordButton = new Button(90, 330, 100, 20, "Check");
    deleteButton = new Button(200, 330, 100, 20, "Delete");

    player1Status = new TextBox(60, 300, 150, 20, "");
    player2Status = new TextBox(220, 300, 150, 20, "");

    initGuesses();

    gameWindow->end();
    gameWindow->hide();

    // start screen window (subwindow):
    startScreenWindow = new Window(0, 25, 400, 500);

    mainTitle = new TextBox(70, 100, 200, 100, " A Very Cool\nWordle Game");
    mainTitle->labelsize(40);
    mainTitle->labelcolor(FL_DARK_BLUE);
    
    singlePlayerButton = new Button(150, 250, 100, 50, "Single Player");
    multiPlayerButton = new Button(150, 330, 100, 50, "Multi Player");

    startScreenWindow->end();
    startScreenWindow->show();

    mainWindow->end();

    // login screen window:
    loginWindow = new Window(100, 100, 300, 400, "play mode");

    player1Text = new TextBox(20, 20, 50, 30, "Player 1");
    player2Text = new TextBox(20, 150, 50, 30, "Player 2");
    name1Input = new Input(20, 70, 260, 30, "User Name");
    name2Input = new Input(20, 200, 260, 30, "User Name");

    startButton = new Button(110, 300, 80, 30, "Start");

    loginWindow->end();

    mainWindow->show();

    // menu pop-up windows:
    instructionWindow = new Window(100, 100, 370, 400, "How to play?");
    instructionsText = new TextBox(10, 10, 370, 380, "This is a wordle game!\n\nYou can play by yourself in Single Player Mode,\nor play with one other friend in Multi Player Mode.\n\nIn Multi Player Mode, you and your friend will\ntake turns to guess the word.\nYour name will turn blue to indicate your turn.\n\nIf your guess has a correct letter at a correct\nposition, a () will surround it.\nIf your guess has a correct letter but wrong\nposition, a [] will surround it.\n\n Guess the word in 6 rounds!\n Good luck!\n\n\n*only single player stats will be recorded");
    instructionWindow->end();
    instructionWindow->hide();

    statsWindow = new Window(100, 100, 400, 300, "Statistics");
    gameDataList = new ListBox(20, 20, 300, 250, "Game Data");
    statsWindow->end();
    statsWindow->hide();

    // button click:
    ON_CLICK(instrucItem, Application::handleMenuClick);
    ON_CLICK(statsItem, Application::handleMenuClick);

    ON_CLICK(singlePlayerButton, Application::handleGameModeClick);
    ON_CLICK(multiPlayerButton, Application::handleGameModeClick);

    ON_CLICK(startButton, Application::handleGameStartClick);

    ON_CLICK(checkWordButton, Application::handleCheckClick);
    ON_CLICK(deleteButton, Application::handleDeleteButtonClick);

    readGameDataFromFile();
}

// Define other functions here

void Application::handleMenuClick(bobcat::Widget *sender){
    if(sender == instrucItem){
        instructionWindow->show();
        instructionWindow->set_modal();
    }else if(sender == statsItem){
        statsWindow->show();
        statsWindow->set_modal();
    }
}

// on screen keyboard variables
int currentRow = 0;
string currentBlank;
string blanks = "-----";

// start screen functions
bool Application::handleGameModeClick(Widget *sender) {
    if (sender == singlePlayerButton) {
        playerStatus = "Single Player";
        loginWindow->set_modal();

        loginWindow->label(playerStatus);
        player2Text->hide();
        name2Input->hide();

    } else if (sender == multiPlayerButton) {
        playerStatus = "Multi Player";
        loginWindow->set_modal();

        loginWindow->label(playerStatus);
        player2Text->show();
        name2Input->show();
    }

    loginWindow->show();
    cout << "Game mode is " << playerStatus << endl;

    return true;
}

void Application::handleGameStartClick(bobcat::Widget *sender) {

    string name1 = name1Input->value();
    string name2 = name2Input->value();

    if (playerStatus == "Single Player") {

        if(name1.length() > 7 ){
                showMessage("Please keep your name under 7 characters.");
        } else if(name1 != "") {
            loginWindow->hide();
            // close start screen subwindow
            startScreenWindow->hide();
            gameWindow->show();

            player1Status->label("Player "+ name1);

        } else{
            showMessage("Please enter player name to start the game.");
        }

    } else {
        if(name1.length() > 7 || name2.length() > 7){
            showMessage("Please keep your name under 7 characters and no blank.");
        } else if(name1 == name2){
            showMessage("Please enter two different names.");

        } else if(name1 != "" && name2 != ""){
            loginWindow->hide();
            // close start screen subwindow
            startScreenWindow->hide();
            gameWindow->show();

            player1Status->label("Player "+ name1);
            player2Status->label("Player "+ name2);

        }else{
            showMessage("Please enter player name(s) to start the game.");
        }
    }
    if(playerStatus == "Single Player"){
        player1Status->show();
        player2Status->hide();
    }else{
        checkWhosTurn();
        player1Status->show();
        player2Status->show();
    }
}


// on screen keyboard
void Application::handleKeyboardClick(Widget *sender) {
    string letter = sender->label();
    cout << "The letter " << letter << " was clicked..." << endl;

    string updatedWord = playerGuess[currentRow].word;

    // for the current row player is on, if the current slot is empty, add
    // player pressed letter

    if (updatedWord.length() < 5) {
        updatedWord += letter;

        currentBlank = updatedWord + blanks.substr(updatedWord.length());

        playerGuess[currentRow].wordGuess->label("");
        playerGuess[currentRow].wordGuess->label(currentBlank);

        playerGuess[currentRow].word = updatedWord;

        cout << updatedWord << updatedWord.length() << endl;
    }
}

void Application::initKeyboard(string row, int y) {
    int width = mainWindow->w();
    int rowWidth = (row.length() * 30) - 5;

    int x = (width - rowWidth) / 2;

    for (size_t i = 0; i < row.length(); i++) {
        string letter = "";
        letter += row[i];
        keyboard.push_back(new Button(x + (i * 30), y, 25, 25, letter));
        ON_CLICK(keyboard.back(), Application::handleKeyboardClick);
    }
}

// word enter (guesses) textbox
void Application::initGuesses() {
    int width = gameWindow->w();
    int rowWidth = 5 * 45 - 5;
    int x = (width - rowWidth) / 2;
    int y = 10;

    for (size_t r = 0; r < 6; r++) {
        playerGuess.push_back({new TextBox(x, y, 250, 40, "-----"), ""});
        playerGuess.back().wordGuess->labelsize(40);
        playerGuess.back().wordGuess->align(FL_ALIGN_CENTER);
        y += 45;
    }
}

// check button click
void Application::handleCheckClick(Widget *sender) {
    string word = playerGuess[currentRow].wordGuess->label();

    if (currentRow == 5 && word != targetWord) {
        cout << "gameover" << endl;
        gameWindow->hide();
        showMessage("The word was: " + targetWord + "\n Game Over :(");
        
        if(playerStatus == "Single Player"){
            string newData = player1Status->label().substr(7) + "--" + targetWord + "--lose";
            Game g = {newData, targetWord, "no"};
            stats.push_back(g);  
        }
        updateStats();
        clearGameRecord();
        startScreenWindow->show();

    } else if (word == makeUpper(targetWord)) {
        cout << "you won" << endl;
        gameWindow->hide();
        showMessage("You got it! The word was: " + targetWord +
                    "! \nCongratulations!");

        if(playerStatus == "Single Player"){
            string newData = player1Status->label().substr(7) + "--" + targetWord + "--win";
            Game g = {newData, targetWord, "yes"};
            stats.push_back(g);  
        }
        updateStats();
        clearGameRecord();
        startScreenWindow->show();

    } else if (word.length() == 5 && currentRow < 6) {
        if (searchInList(makeLower(word)) == false) {
            showMessage("This is not a valid word in the list.");
        } else {
            checkLetterPosition();
            currentRow++;
            if(playerStatus == "Multi Player"){
                checkWhosTurn();
            }
            cout << currentRow << endl;
        }
    }
}

void Application::handleDeleteButtonClick(Widget *sender) {
    string currentWord = playerGuess[currentRow].word;

    if (currentWord.length() != 0) {
        currentWord = currentWord.substr(0, currentWord.length() - 1);
        string reLable = currentWord + blanks.substr(currentWord.length());

        playerGuess[currentRow].wordGuess->label(reLable);
        playerGuess[currentRow].word = currentWord;
    }
}

// reset game
void Application::clearGameRecord() {
    for (size_t i = 0; i < 6; i++) {
        playerGuess[i].wordGuess->label("-----");
        playerGuess[i].word = "";
    }
    currentRow = 0;
    targetWord = getRandomWord();
    cout << "the target word for this round is: " << targetWord << endl;
}

// pick target word:
std::string Application::getRandomWord() {
    ifstream file("assets/5letterWords.txt");

    if (!file.is_open()) {
        cout << "Error with opening file..." << endl;
        exit(1);
    }

    string word;
    while (getline(file, word)) {
        words.push_back(word);
    }

    file.close();

    // Create a random number generator
    // that will give values from 0 to the number of words
    RNG rng(0, words.size() - 1);

    int index = rng.get();

    string chosenWord = words[index];

    return chosenWord;
}

// uppercase a word:
string Application::makeUpper(const string &s) {
    string result = "";
    for (size_t i = 0; i < s.length(); i++) {
        result += toupper(s[i]);
    }

    return result;
}

// lowercase a word:
string Application::makeLower(const string &s) {
    string result = "";
    for (size_t i = 0; i < s.length(); i++) {
        result += tolower(s[i]);
    }

    return result;
}

// check if word is valid
bool Application::searchInList(const std::string &word) {
    for (size_t i = 0; i < words.size(); i++) {
        if (words[i] == word) {
            return true;
        }
    }
    return false;
}

// check player turn in multi player mode
void Application::checkWhosTurn(){
    if(currentRow % 2 == 0){
        player1Status->labelcolor(FL_BLUE);
        player2Status->labelcolor(FL_BLACK);
    }else{
        player1Status->labelcolor(FL_BLACK);
        player2Status->labelcolor(FL_BLUE);
    }
}


// check if letter position is correct
void Application::checkLetterPosition(){
    string checkUpdate;
    string checkTarget = makeUpper(targetWord);

    for(size_t i = 0; i < 5; i++){
        char letter = playerGuess[currentRow].word[i];
        string letterString = "";
        letterString += letter;

        if(playerGuess[currentRow].word[i] == checkTarget[i]){ // correct position and letter
            checkUpdate = checkUpdate + "(" + playerGuess[currentRow].word[i] + ")";

        }else if(containsSubstring(checkTarget, letterString) == true){ // correct letter but wrong position
            checkUpdate = checkUpdate + "[" + playerGuess[currentRow].word[i] + "]";

        }else{ // wrong letter wrong position (no change)
            checkUpdate += playerGuess[currentRow].word[i];
        }
    }
    playerGuess[currentRow].wordGuess->label(checkUpdate);
}

// find if a string contains a substring
// I looked up string function find(), credit goes to: https://stackoverflow.com/questions/43629363
bool Application::containsSubstring(const std::string& mainString, const std::string& subString) {
    if(mainString.find(subString) != std::string::npos){
        return true;
    }else{
        return false;
    }
}


// storing and updating data
void Application::updateStats(){
    ofstream file("data/gameData.csv");

    gameDataList->clear();


    for (size_t i = 0; i < stats.size(); i++){
        string userName = stats[i].name;
        string roundTargetWord = stats [i].theWord;
        string winStatus = stats[i].status;

        cout << userName  << "*" << endl;

        gameDataList->add(userName);

        file << userName << "," << roundTargetWord << "," << winStatus << endl;
    }

    file.close();
}

void Application::readGameDataFromFile(){
    ifstream file("data/gameData.csv");

    if (!file.is_open()) {
        cout << "No game data found" << endl;
        return;
    }

    string line;

    while (getline(file, line)) {
        string nameField;
        string wordField;
        string statusField;
        
        stringstream ss(line);
        getline(ss, nameField, ',');
        getline(ss, wordField, ',');
        getline(ss, statusField);

        string name = nameField;
        string word = wordField;
        string status = statusField;

        cout << name << endl;
        cout << word << endl;

        Game g = {name, word, status};
        stats.push_back(g);        
    }

    file.close();

    updateStats();
}

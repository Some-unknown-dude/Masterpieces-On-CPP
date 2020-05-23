#include <iostream>
#include "Board.h"
#include <fstream>

using namespace std;

const char* path = "LastGame.txt";
const char firstSymbol = 33;

void Menu();
void MenuText();
void Game();
void WatchReplay();
void Rules();
void InfoAboutGame();
void EnterCoordinates(char&, int&);
void EnterMenu(char&);
bool IsNumber(char);
void Choose(Board&, char&, int&);
void Move(Board&, char&, int&);

int main()
{
    Menu();
    return 0;
}
void Menu()
{
    char choice = '\0';
    while (true)
    {
        MenuText();
        EnterMenu(choice);
        system("cls");
        switch (choice)
        {
        case '1':
            Game();
            break;
        case '2':
            WatchReplay();
            break;
        case '3':
            Rules();
            break;
        case '4':
            return;
        default:
            break;
        }
    }
}

void MenuText()
{
    cout << "////////////////////////// RUSSIAN DRAUGHTS //////////////////////////" << endl
        << "------------------------------   MENU   ------------------------------" << endl
        << "1 - play game" << endl
        << "2 - watch replay" << endl
        << "3 - rules" << endl
        << "4 - exit" << endl;

}
void Game()
{
    InfoAboutGame();
    Board game;
    ofstream recordInFile(path);
    char letter = '\0';
    int number = 0;
    bool captured = false;
    while (game.getNumOfPieces('w') && game.getNumOfPieces('b') && !game.areAllPiecesStuckForCurSide())
    {
        Choose(game, letter, number);
        recordInFile << letter << number;
        captured = game.canCapture(game.getCurLet(), game.getCurNum()) ? true : false;
        do
        {
            Move(game, letter, number);
            recordInFile << (captured ? ':' : '-') << letter << number;
        } while (game.canCapture(game.getCurLet(), game.getCurNum()) && captured);
        recordInFile << endl;
        game.changeSide();
    }
    if (game.getNumOfPieces('w') == 0 || game.getNumOfPieces('b') == 0)
    {
        game.ASCII(cout, '*', 131) << endl;
        game.getNumOfPieces('b') ? cout << "Black won" << endl : cout << "White won" << endl;
        game.ASCII(cout, '*', 131) << endl;
    }
    else if (game.areAllPiecesStuckForCurSide())
    {
        game.ASCII(cout, '*', 131) << endl;
        game.getCurSide() == 'b' ? cout << "White won" << endl : cout << "Black won" << endl;
        game.ASCII(cout, '*', 131) << endl;
    }
    recordInFile.close();
    cout << game;
    system("pause");
    system("cls");
}
void WatchReplay()
{
    Board game;
    ifstream replay(path);
    char letter, nextSymbol;
    int number;
    while (replay.peek() >= firstSymbol)
    {
        cout << game << endl;
        letter = replay.get();
        number = replay.get() - '0';
        replay.ignore();
        game.choosePiece(letter, number);
        system("pause");
        system("cls");
        if (replay.peek() < firstSymbol)
        {
            break;
        }
        do
        {
            cout << game << endl;
            letter = replay.get();
            number = replay.get() - '0';
            game.makeMove(letter, number);
            nextSymbol = replay.get();
            system("pause");
            system("cls");
        } while (nextSymbol == ':');
        game.changeSide();
    }
    if (game.getNumOfPieces('w') == 0 || game.getNumOfPieces('b') == 0)
    {
        game.ASCII(cout, '*', 131) << endl;
        game.getNumOfPieces('b') ? cout << "Black won" << endl : cout << "White won" << endl;
        game.ASCII(cout, '*', 131) << endl;
    }
    else if(game.areAllPiecesStuckForCurSide())
    {
        game.ASCII(cout, '*', 131) << endl;
        game.getCurSide() == 'b' ? cout << "White won" << endl : cout << "Black won" << endl;
        game.ASCII(cout, '*', 131) << endl;
    }
    else
    {
        game.ASCII(cout, '*', 131) << endl;
        cout << "Game wasn't completed last time" << endl;
        game.ASCII(cout, '*', 131) << endl;
    }
    replay.close();
    cout << game;
    system("pause");
    system("cls");
}
void Rules()
{
    cout << "===================================================  RULES  ===================================================\n"
        << "As in all draughts variants, Russian draughts is played by two people, alternating moves.\n"
        << "One player has dark pieces, and the other has light pieces. Pieces move diagonally and pieces of the opponent are captured by jumping over them.\n\n"
        << "---------------------------------\nBoard\n---------------------------------\n"
        << "Played on an 8x8 board with alternating dark and light squares.The left down square field should be dark.\n"
        << "---------------------------------\nStarting position\n---------------------------------\n"
        << "Each player starts with 12 pieces on the three rows closest to their own side.The row closest to each player is called the \"crownhead\" or \"kings row\".\n"
        << "The colors of the pieces are black and white.The player with white pieces moves first.\n"
        << "---------------------------------\nPieces\n---------------------------------\n"
        << "There are two kinds of pieces: \"men\" and \"kings\".\n"
        << "> Men: Men move forward diagonally to an adjacent unoccupied square.\n"
        << "> King: If a player's piece moves into the kings row on the opposing player's side of the board, that piece is to be \"crowned\".\n"
        << "It becomes a \"king\" and gains the ability to move backwards as well as forwards and to choose on which free square on this diagonal to stop.\n"
        << "---------------------------------\nCapture\n---------------------------------\n"
        << "If the adjacent square contains an opponent's piece, and the square immediately beyond it is vacant, the opponent's piece may be captured (and removed from the game) by jumping over it.\n"
        << "Jumping can be done forward and backward. Multiple-jump moves are possible if, when the jumping piece lands, there is another piece that can be jumped.\n"
        << "Jumping is mandatory and cannot be passed up to make a non-jumping move.\n"
        << "When there is more than one way for a player to jump, one may choose which sequence to make, not necessarily the sequence that will result in the most captures.\n"
        << "However, one must make all the captures in that sequence.\n"
        << "A captured piece is left on the board until all captures in a sequence have been made but cannot be jumped again (this rule also applies for the kings).\n\n"
        << "If a man touches the kings row during a capture and can continue a capture, it jumps backwards as a king. The player can choose where to land after the capture.\n\n"
        << "---------------------------------\nWinning\n---------------------------------\n"
        << " A player with no valid move remaining loses.\n"
        << "This is the case if the player either has no pieces left or if a player's pieces are obstructed from making a legal move by the pieces of the opponent.\n"
        << "A game is a draw if neither opponent has the possibility to win the game.\n\n";
    system("pause");
    system("cls");
}
void InfoAboutGame()
{
    cout << "FEATURES OF THE GAME: " << endl
        << "\t> While capturing multiple pieces in a row, the coordinates of move for capturing only one piece have to be entered.\n"
        << "Thus, for capturing n pieces you have to enter n coordinates\n";
    system("pause");
    system("cls");
}
void EnterMenu(char& choice)
{
    char choiceBuff;
    cin >> choiceBuff;
    if (cin.peek() <= firstSymbol)
    {
        choice = choiceBuff;
    }
    cin.ignore(9999, '\n');
}
void EnterCoordinates(char& letter, int& number)
{
    char numberBuff, letterBuff;
    cin >> letterBuff >> numberBuff;
    if (cin.peek() <= firstSymbol)  //if next symbol in buffer wasn't from keybord
    {
        letter = letterBuff;
        number = numberBuff - '0';
    }
    else 
    {
        letter = '\0';
        number = '\0';
    }
    cin.ignore(9999, '\n');
}
bool IsNumber(char symbol)
{
    return (symbol >= '0' && symbol <= '9');
}
void Choose(Board& game, char& letter, int& number)
{
    const char* messageOutOfRange = "Coordinates are out of range. Try again";
    const char* messageInvalid = "You didn't choose valid piece. Try again";
    const char* messageCannotMove = "The piece you choose can't move. Try again";
    const char* messageChooseAnother = "There is a piece on your side that has to capture. Choose it";
    bool valid = false;
    while (!valid)
    {
        cout << game << endl << "Choose piece: ";
        EnterCoordinates(letter, number);
        if (!Board::areCoordinatesInRange(letter, number))
        {
            cout << messageOutOfRange << endl;
            valid = false;
        }
        else if (!game.isFriendPieceHere(letter, number))
        {
            cout << messageInvalid << endl;
            valid = false;
        }
        else if (game.isStuck(letter, number))
        {
            cout << messageCannotMove << endl;
            valid = false;
        }
        else if (!game.canCapture(letter, number) && game.isCaptureAvailableForCurrentSide())
        {
            cout << messageChooseAnother << endl;
            valid = false;
        }
        else
        {
            valid = true;
        }
        if (!valid)
        {
            system("pause");
        }
        system("cls");
    }
    game.choosePiece(letter, number);
}
void Move(Board& game, char& letter, int& number)
{
    const char* messageOutOfRange = "Coordinates are out of range. Try again";
    const char* messageInvalid = "You didn't choose valid move. Try again";
    const char* messageHaveToCapture = "You have to capture enemy piece. Try again";
    bool valid = false;
    while (!valid)
    {
        valid = true;
        cout << game << "Choose move: ";
        EnterCoordinates(letter, number);
        if (!Board::areCoordinatesInRange(letter, number))
        {
            cout << messageOutOfRange << endl;
            valid = false;
        }
        else if (!game.isMoveValid(letter, number))
        {
            cout << messageInvalid << endl;
            valid = false;
        }
        else if (game.canCapture(game.getCurLet(), game.getCurNum()) && !game.willCaptureWithMove(letter, number))
        {
            cout << messageHaveToCapture << endl;
            valid = false;
        }
        if (!valid)
        {
            system("pause");
        }
        system("cls");
    }
    game.makeMove(letter, number);
}

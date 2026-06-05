#include "board.h"
#include "check.h"
#include "verification.h"
#include <windows.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
int main(){
    _setmode(_fileno(stdout), _O_U16TEXT);

    // Enable Virtual Terminal Processing for ANSI colors
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    SetConsoleMode(hOut, dwMode | 0x0004);
    
    GameState state;
    initialiseBoard(state.board);
    state.currentPlayer=Colour::White;
    do{
        displayBoard(state.board);
        Move move{};
        std::string from,to;
        std::wcout << L"\nEnter move:";
        std::cin >> from >> to;
        move.from=sqaureToIndex(from);
        move.to=sqaureToIndex(to);
        if(verify(state,move) && validateForNoPins(move, state)){
            movePiece(state,move);
            changeCurrentPlayer(state.currentPlayer);
        }
        
    }while(!isCheckmate(state) && !isStalemate(state) && !isInsufficientMaterial(state));
    return 0;
}
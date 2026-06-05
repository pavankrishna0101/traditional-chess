#include "board.h"
#include "verification.h"
#include <iostream>
#include <cmath>

int sqaureToIndex(const std::string& square){
    if(square.length()!=2) return -1;
    int col=square[0]-'a';
    int row=square[1]-'1';
    if(col<0 || col>7 || row<0 || row>7) return -1;
    return row*8+col;
}

std::string cordToString(int row, int col){
    if(row<0 || row>7 || col<0 || col>7) return "";
    std::string square{};
    square+=('a'+col);
    square+=('1'+row);
    return square;
}

void initialiseBoard(Board& board){
    for(int i=0;i<8;i++){
        board[1][i]={PieceType::Pawn,Colour::White};
        board[6][i]={PieceType::Pawn,Colour::Black};
    }

    std::array<PieceType,8> MainPiecesOrder={
        PieceType::Rook,
        PieceType::Knight,
        PieceType::Bishop,
        PieceType::Queen,
        PieceType::King,
        PieceType::Bishop,
        PieceType::Knight,
        PieceType::Rook
    };

    for(int i=0;i<8;i++){
        board[0][i]={MainPiecesOrder[i],Colour::White};
        board[7][i]={MainPiecesOrder[i],Colour::Black};
    }
}

void printPiece(const Piece& piece){
    // Set piece color (Foreground)
    if(piece.colour == Colour::White) {
        std::wcout << L"\033[97m"; // Bright White
    } else {
        std::wcout << L"\033[30m"; // Black
    }

    switch (piece.type) {
        case PieceType::Pawn:   std::wcout << (piece.colour == Colour::White ? L"♙" : L"♟"); break;
        case PieceType::Rook:   std::wcout << (piece.colour == Colour::White ? L"♖" : L"♜"); break;
        case PieceType::Knight: std::wcout << (piece.colour == Colour::White ? L"♘" : L"♞"); break;
        case PieceType::Bishop: std::wcout << (piece.colour == Colour::White ? L"♗" : L"♝"); break;
        case PieceType::Queen:  std::wcout << (piece.colour == Colour::White ? L"♕" : L"♛"); break;
        case PieceType::King:   std::wcout << (piece.colour == Colour::White ? L"♔" : L"♚"); break;
        default: std::wcout << L"?";
    }
}

void displayBoard(const Board& board){
    // \033[2J clears the entire screen
    // \033[H moves the cursor back to the top-left corner
    std::wcout << L"\033[2J\033[H\n"; 
    
    std::wcout << L"   a  b  c  d  e  f  g  h \n";

    for(int i = 7; i >= 0; i--){
        std::wcout << i + 1 << L" "; // Left side coordinates
        
        for(int j = 0; j < 8; j++){
            // Determine if the square is light or dark
            bool isLightSquare = (i + j) % 2 != 0;
            
            // Apply ANSI background colors:
            // 47 is light grey (white square), 100 is dark grey (black square)
            if(isLightSquare) {
                std::wcout << L"\033[47m"; 
            } else {
                std::wcout << L"\033[100m";
            }

            // Print the piece with padding so the square looks even
            std::wcout << L" ";
            if(board[i][j].type == PieceType::Empty){
                std::wcout << L" "; // Empty space instead of a dot
            }
            else{
                printPiece(board[i][j]);
            }
            std::wcout << L" ";
            
            // Reset colors back to normal terminal defaults
            std::wcout << L"\033[0m"; 
        }
        std::wcout << L" " << i + 1 << L"\n"; // Right side coordinates
    }
    std::wcout << L"   a  b  c  d  e  f  g  h \n\n";
}

void movePiece(GameState& state,Move& move){
    auto& board=state.board;

    int from=move.from;
    int to=move.to;

    if(from==-1 || to==-1) return;

    move.obkcastle=state.bkcastle;
    move.obqcastle=state.bqcastle;
    move.owkcastle=state.wkcastle;
    move.owqcastle=state.wqcastle;

    move.oldEnPassantSquare= state.enPassantsqaure;

    CastleType castleType = returnCastleType(move);
    
    //code for movement of castling
    if(castleType != CastleType::None){
        move.isCastlingMove = true;
        move.movedPiece = board[move.from/8][move.from%8];

        if(castleType == CastleType::Kingside){

            if(state.currentPlayer == Colour::White){
                board[0][6] = board[0][4]; // King e1 -> g1
                board[0][5] = board[0][7]; // Rook h1 -> f1

                board[0][4] = {PieceType::Empty, Colour::Empty};
                board[0][7] = {PieceType::Empty, Colour::Empty};

                state.wkcastle = false;
                state.wqcastle = false;
            }

            if(state.currentPlayer == Colour::Black){
                board[7][6] = board[7][4]; // King e8 -> g8
                board[7][5] = board[7][7]; // Rook h8 -> f8

                board[7][4] = {PieceType::Empty, Colour::Empty};
                board[7][7] = {PieceType::Empty, Colour::Empty};

                state.bkcastle = false;
                state.bqcastle = false;
            }
        }

        if(castleType == CastleType::Queenside){

            if(state.currentPlayer == Colour::White){
                board[0][2] = board[0][4]; // King e1 -> c1
                board[0][3] = board[0][0]; // Rook a1 -> d1

                board[0][4] = {PieceType::Empty, Colour::Empty};
                board[0][0] = {PieceType::Empty, Colour::Empty};

                state.wkcastle = false;
                state.wqcastle = false;
            }

            if(state.currentPlayer == Colour::Black){
                board[7][2] = board[7][4]; // King e8 -> c8
                board[7][3] = board[7][0]; // Rook a8 -> d8

                board[7][4] = {PieceType::Empty, Colour::Empty};
                board[7][0] = {PieceType::Empty, Colour::Empty};

                state.bkcastle = false;
                state.bqcastle = false;
            }
        }
        updateCastlingRights(state, move);
        updateEnPassantSquare(state,move);
        return;
    }

    //code for normal movements and en passant
    int fromcol=from%8;
    int fromrow=from/8;
    int tocol=to%8;
    int torow=to/8;

    move.movedPiece=board[fromrow][fromcol];

    if(isEnPassant(state, move)){
        move.isEnPassantMove= true;

        int capturedRow = fromrow;
        int capturedCol = tocol;

        move.capturedPiece=board[capturedRow][capturedCol];
        board[capturedRow][capturedCol]={PieceType::Empty,Colour::Empty};
        board[torow][tocol]=board[fromrow][fromcol];
        board[fromrow][fromcol]={PieceType::Empty,Colour::Empty};
        updateEnPassantSquare(state,move);
        return;
    }

    move.capturedPiece=board[torow][tocol];
    board[fromrow][fromcol]={PieceType::Empty,Colour::Empty};
    board[torow][tocol]=move.movedPiece;

    if(move.movedPiece.type==PieceType::Pawn){
        if(isPawnPromotionMove(board, move)){
            move.promotedPiece=PieceType::Queen;
            board[torow][tocol]={move.promotedPiece,move.movedPiece.colour};
        }
    }

    updateCastlingRights(state, move);
    updateEnPassantSquare(state,move);
}

void undoMove(GameState& state, const Move& move){
    auto& board = state.board;

    if(move.isCastlingMove){

        CastleType castleType = returnCastleType(move);

        if(move.movedPiece.colour == Colour::White){

            if(castleType == CastleType::Kingside){

                board[0][4] = {PieceType::King, Colour::White};
                board[0][7] = {PieceType::Rook, Colour::White};

                board[0][5] = {PieceType::Empty, Colour::Empty};
                board[0][6] = {PieceType::Empty, Colour::Empty};
            }
            else{ // Queenside

                board[0][4] = {PieceType::King, Colour::White};
                board[0][0] = {PieceType::Rook, Colour::White};

                board[0][2] = {PieceType::Empty, Colour::Empty};
                board[0][3] = {PieceType::Empty, Colour::Empty};
            }
        }
        else{

            if(castleType == CastleType::Kingside){

                board[7][4] = {PieceType::King, Colour::Black};
                board[7][7] = {PieceType::Rook, Colour::Black};

                board[7][5] = {PieceType::Empty, Colour::Empty};
                board[7][6] = {PieceType::Empty, Colour::Empty};
            }
            else{ // Queenside

                board[7][4] = {PieceType::King, Colour::Black};
                board[7][0] = {PieceType::Rook, Colour::Black};

                board[7][2] = {PieceType::Empty, Colour::Empty};
                board[7][3] = {PieceType::Empty, Colour::Empty};
            }
        }

        state.bkcastle = move.obkcastle;
        state.bqcastle = move.obqcastle;
        state.wkcastle = move.owkcastle;
        state.wqcastle = move.owqcastle;
        state.enPassantsqaure = move.oldEnPassantSquare;
        return;
    }

    if(move.isEnPassantMove){
        state.enPassantsqaure=move.oldEnPassantSquare;
        
        int fromcol = move.from % 8;
        int fromrow = move.from / 8;
        int tocol   = move.to % 8;
        int torow   = move.to / 8;
        int capturedRow = fromrow;
        int capturedCol = tocol;

        board[fromrow][fromcol]=move.movedPiece;
        board[torow][tocol]={PieceType::Empty,Colour::Empty};
        board[capturedRow][capturedCol]=move.capturedPiece;
        return;
    }

    int fromcol = move.from % 8;
    int fromrow = move.from / 8;
    int tocol   = move.to % 8;
    int torow   = move.to / 8;

    board[fromrow][fromcol] = move.movedPiece;
    board[torow][tocol] = move.capturedPiece;

    state.bkcastle = move.obkcastle;
    state.bqcastle = move.obqcastle;
    state.wkcastle = move.owkcastle;
    state.wqcastle = move.owqcastle;
    state.enPassantsqaure = move.oldEnPassantSquare;
}

void changeCurrentPlayer(Colour& currentPlayer){
    currentPlayer= (currentPlayer==Colour::White) ? Colour::Black : Colour::White;
}

bool isPawnPromotionMove(const Board& board, const Move& move){
    if(move.movedPiece.type!=PieceType::Pawn) return false;
    int toRow=move.to/8;
    if(move.movedPiece.colour==Colour::White && toRow==7){
        return true;
    }else if(move.movedPiece.colour==Colour::Black && toRow==0){
        return true;
    }
    return false;
}

void updateCastlingRights(GameState& state, const Move& move){

    // --- KING MOVED ---
    if(move.from == 4){ // e1
        state.wkcastle = false;
        state.wqcastle = false;
    }

    if(move.from == 60){ // e8
        state.bkcastle = false;
        state.bqcastle = false;
    }

    // --- ROOK MOVED ---
    if(move.from == 0){ // a1 (queenside rook)
        state.wqcastle = false;
    }

    if(move.from == 7){ // h1 (kingside rook)
        state.wkcastle = false;
    }

    if(move.from == 56){ // a8 (queenside rook)
        state.bqcastle = false;
    }

    if(move.from == 63){ // h8 (kingside rook)
        state.bkcastle = false;
    }

    // --- ROOK CAPTURED ---
    if(move.to == 0){ // a1
        state.wqcastle = false;
    }

    if(move.to == 7){ // h1
        state.wkcastle = false;
    }

    if(move.to == 56){ // a8
        state.bqcastle = false;
    }

    if(move.to == 63){ // h8
        state.bkcastle = false;
    }
}

void updateEnPassantSquare(GameState& state, const Move& move){
    state.enPassantsqaure = -1;

    int fromRow = move.from / 8;
    int toRow = move.to / 8;

    if(move.movedPiece.type == PieceType::Pawn &&
       abs(toRow - fromRow) == 2)
    {
        int col = move.to % 8;

        if(move.movedPiece.colour == Colour::White)
            state.enPassantsqaure = (toRow - 1) * 8 + col;
        else
            state.enPassantsqaure = (toRow + 1) * 8 + col;
    }
} 

CastleType returnCastleType(const Move move){
    int fromcol=move.from%8;
    int fromrow=move.from/8;
    int tocol=move.to%8;
    int torow=move.to/8;
    if (fromrow == 0 || fromrow == 7){
        if(fromcol==4 && tocol==6) return CastleType::Kingside;
        if(fromcol==4 && tocol==2) return CastleType::Queenside;
    }
    return CastleType::None;
}




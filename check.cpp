#include "check.h"

bool isUnderAttackByPawn(const GameState& state, int squareIndex, Colour attackerColour){
    auto& board = state.board;

    int row = squareIndex / 8;
    int col = squareIndex % 8;

    int attackRow = (attackerColour == Colour::White)
        ? row - 1
        : row + 1;

    if(attackRow < 0 || attackRow > 7)
        return false;

    if(col > 0 &&
       board[attackRow][col-1].colour == attackerColour &&
       board[attackRow][col-1].type == PieceType::Pawn)
        return true;

    if(col < 7 &&
       board[attackRow][col+1].colour == attackerColour &&
       board[attackRow][col+1].type == PieceType::Pawn)
        return true;

    return false;
}

bool isUnderAttackByKnight(const GameState& state, int squareIndex, Colour attackerColour){
    auto& board = state.board;

    int row = squareIndex / 8;
    int col = squareIndex % 8;

    int moves[8][2] = {
        {2,1},{2,-1},{-2,1},{-2,-1},
        {1,2},{1,-2},{-1,2},{-1,-2}
    };

    for(auto& move : moves){
        int r = row + move[0];
        int c = col + move[1];

        if(r >= 0 && r < 8 && c >= 0 && c < 8){
            if(board[r][c].colour == attackerColour &&
               board[r][c].type == PieceType::Knight)
                return true;
        }
    }

    return false;
}

bool isUnderAttackByKing(const GameState& state, int squareIndex, Colour attackerColour){
    auto& board = state.board;

    int row = squareIndex / 8;
    int col = squareIndex % 8;

    for(int dr = -1; dr <= 1; dr++){
        for(int dc = -1; dc <= 1; dc++){

            if(dr == 0 && dc == 0)
                continue;

            int r = row + dr;
            int c = col + dc;

            if(r >= 0 && r < 8 && c >= 0 && c < 8){
                if(board[r][c].colour == attackerColour &&
                   board[r][c].type == PieceType::King)
                    return true;
            }
        }
    }

    return false;
}

bool isUnderAttackByRook(const GameState& state, int squareIndex, Colour attackerColour){
    auto& board = state.board;

    int row = squareIndex / 8;
    int col = squareIndex % 8;

    int directions[4][2] = {
        {1,0},
        {-1,0},
        {0,1},
        {0,-1}
    };

    for(auto& dir : directions){

        int r = row + dir[0];
        int c = col + dir[1];

        while(r >= 0 && r < 8 && c >= 0 && c < 8){

            if(board[r][c].type != PieceType::Empty){

                if(board[r][c].colour == attackerColour &&
                   (board[r][c].type == PieceType::Rook ||
                    board[r][c].type == PieceType::Queen))
                    return true;

                break;
            }

            r += dir[0];
            c += dir[1];
        }
    }

    return false;
}

bool isUnderAttackByBishop(const GameState& state, int squareIndex, Colour attackerColour){
    auto& board = state.board;

    int row = squareIndex / 8;
    int col = squareIndex % 8;

    int directions[4][2] = {
        {1,1},
        {1,-1},
        {-1,1},
        {-1,-1}
    };

    for(auto& dir : directions){

        int r = row + dir[0];
        int c = col + dir[1];

        while(r >= 0 && r < 8 && c >= 0 && c < 8){

            if(board[r][c].type != PieceType::Empty){

                if(board[r][c].colour == attackerColour &&
                   (board[r][c].type == PieceType::Bishop ||
                    board[r][c].type == PieceType::Queen))
                    return true;

                break;
            }

            r += dir[0];
            c += dir[1];
        }
    }

    return false;
}

bool isUnderAttack(const GameState& state, int squareIndex, Colour attackerColour){
    return isUnderAttackByPawn(state, squareIndex, attackerColour)
        || isUnderAttackByKnight(state, squareIndex, attackerColour)
        || isUnderAttackByBishop(state, squareIndex, attackerColour)
        || isUnderAttackByRook(state, squareIndex, attackerColour)
        || isUnderAttackByKing(state, squareIndex, attackerColour);
}

int findKingCord(const GameState& state, Colour colour){
    auto& board = state.board;

    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            if(board[i][j].colour == colour &&
               board[i][j].type == PieceType::King){
                return i * 8 + j;
            }
        }
    }

    return -1;
}

bool isCheck(GameState& state, Colour kingColour){
    int kingCord=findKingCord(state, kingColour);
    Colour attackerColour = (kingColour==Colour::White) ? Colour::Black : Colour::White;
    return isUnderAttack(state, kingCord, attackerColour);
}

bool isCheckmate(GameState& state){
    Colour kingColour = state.currentPlayer;
    if(!isCheck(state,kingColour)) return false;
    for(int i=0;i<64;i++){
        if(state.board[i/8][i%8].colour!=kingColour) continue;
        for(int j=0;j<64;j++){
            Move move;
            move.to=j;
            move.from=i;
            if(verify(state,move)){
                movePiece(state,move);
                if(!isCheck(state,kingColour)){
                    undoMove(state,move);
                    return false;
                }
                undoMove(state,move);
            }
        }
    }
    return true;
}

bool isStalemate(GameState& state){
    Colour kingColour = state.currentPlayer;
    if(isCheck(state, kingColour)) return false; 
    
    auto& board = state.board;

    for(int i = 0; i < 64; i++){
        int fromRow = i / 8;
        int fromCol = i % 8;

        if(board[fromRow][fromCol].colour != kingColour) continue;

        for(int j = 0; j < 64; j++){
            Move move;
            move.from = i;
            move.to = j;

            if(verify(state, move)){
                movePiece(state, move);

                if(!isCheck(state, kingColour)){
                    undoMove(state, move);
                    return false; 
                }
                undoMove(state, move);
            }
        }
    }
    return true;
}

bool isInsufficientMaterial(const GameState& state){
    int totalMinors = 0;
    int bishopLightSquare = 0;
    int bishopDarkSquare = 0;

    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            Piece p = state.board[i][j];
            
            if(p.type == PieceType::Empty || p.type == PieceType::King) 
                continue;

            // If there is p,q,r checkmate is possible.
            if(p.type == PieceType::Pawn || p.type == PieceType::Rook || p.type == PieceType::Queen){
                return false;
            }

            if(p.type == PieceType::Knight){
                totalMinors++;
            }

            if(p.type == PieceType::Bishop){
                totalMinors++;
                // Check if square is light or dark
                // (i + j) being even means it's a dark square, odd means light
                if((i + j) % 2 == 0){
                    bishopDarkSquare++;
                } else {
                    bishopLightSquare++;
                }
            }
        }
    }

    if(totalMinors <= 1) return true;
    if(totalMinors == 2 && (bishopLightSquare == 2 || bishopDarkSquare == 2)){
        return true;
    }

    return false;
}
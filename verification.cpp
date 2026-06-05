#include "verification.h"
#include <cmath>

static bool verifyPawnMovement(int fromRow, int fromCol, int toRow, int toCol,const Board& board){

    Piece piece=board[fromRow][fromCol];
    if(piece.colour==Colour::White){
        //normal
        if(board[toRow][toCol].type==PieceType::Empty && fromCol==toCol && toRow==fromRow+1) return true;
        //double
        else if(board[toRow][toCol].type==PieceType::Empty && fromCol==toCol && toRow==fromRow+2 && fromRow==1 && board[fromRow+1][fromCol].type==PieceType::Empty) return true;
        //cut
        else if(!(board[toRow][toCol].type==PieceType::Empty) && (toCol==fromCol-1 || toCol==fromCol+1) && toRow==fromRow+1) return true;
        return false;
    }else if(piece.colour==Colour::Black){
        //normal
        if(board[toRow][toCol].type==PieceType::Empty && fromCol==toCol && toRow==fromRow-1) return true;
        //double
        else if(board[toRow][toCol].type==PieceType::Empty && fromCol==toCol && toRow==fromRow-2 && fromRow==6 && board[fromRow-1][fromCol].type==PieceType::Empty) return true;
        //cut
        else if(!(board[toRow][toCol].type==PieceType::Empty) && (toCol==fromCol-1 || toCol==fromCol+1) && toRow==fromRow-1) return true;
        return false;
    }
    return false;
}
static bool verifyRookMovement(int fromRow, int fromCol, int toRow, int toCol,const Board& board){
    if(fromRow!=toRow && fromCol!=toCol) return false;
    if(fromRow==toRow){
        if(fromCol<toCol){
            for(int i=fromCol+1;i<toCol;i++){
                if(board[fromRow][i].type!=PieceType::Empty) return false;
            }
        }else if(fromCol>toCol){
            for(int i=fromCol-1;i>toCol;i--){
                if(board[fromRow][i].type!=PieceType::Empty) return false;
            }
        }
    }else if(fromCol==toCol){
        if(fromRow<toRow){
            for(int i=fromRow+1;i<toRow;i++){
                if(board[i][fromCol].type!=PieceType::Empty) return false;
            }
        }else if(fromRow>toRow){
            for(int i=fromRow-1;i>toRow;i--){
                if(board[i][fromCol].type!=PieceType::Empty) return false;
            }
        }
    }
    return true;
    
}
static bool verifyKnightMovement(int fromRow, int fromCol, int toRow, int toCol,const Board& board){
    if((toRow==fromRow+2 || toRow==fromRow-2) && (toCol==fromCol+1 || toCol==fromCol-1)) return true;
    else if((toRow==fromRow+1 || toRow==fromRow-1) && (toCol==fromCol+2 || toCol==fromCol-2)) return true;
    return false;
}
static bool verifyBishopMovement(int fromRow, int fromCol, int toRow, int toCol,const Board& board){
    if(abs(toRow-fromRow)==abs(toCol-fromCol)){
        int rowDirection= (toRow > fromRow) ? 1: -1;
        int colDirection= (toCol > fromCol) ? 1: -1;
        int i=fromRow+rowDirection;
        int j=fromCol+colDirection;
        while(i!=toRow){
            if(board[i][j].type!=PieceType::Empty) return false;
            i+=rowDirection;
            j+=colDirection;
        }
        return true;
    }
    else return false;
        
}

static bool verifyKingMovement(int fromRow, int fromCol, int toRow, int toCol, const Board& board){
    return std::abs(toRow - fromRow) <= 1 && std::abs(toCol - fromCol) <= 1;
}

static bool verifyCastling(GameState& state, const Move& move, CastleType castleType){   
        int fromRow=move.from/8;
        int fromCol=move.from%8;
        auto& board= state.board;

        if(board[fromRow][fromCol].colour != state.currentPlayer ||
        board[fromRow][fromCol].type != PieceType::King)
        {
            return false;
        }

        if(isCheck(state, state.currentPlayer)) return false;

        if(state.currentPlayer == Colour::White){

            if(castleType == CastleType::Kingside){

                if(!state.wkcastle)
                    return false;

                if(board[0][7].type != PieceType::Rook ||
                board[0][7].colour != Colour::White)
                {
                    return false;
                }

                if(board[0][5].type != PieceType::Empty ||
                board[0][6].type != PieceType::Empty)
                {
                    return false;
                }

                if(isUnderAttack(state, 5, Colour::Black) ||
                isUnderAttack(state, 6, Colour::Black))
                {
                    return false;
                }
            }
            else{ // Queenside

                if(!state.wqcastle)
                    return false;

                if(board[0][0].type != PieceType::Rook ||
                board[0][0].colour != Colour::White)
                {
                    return false;
                }

                if(board[0][1].type != PieceType::Empty ||
                board[0][2].type != PieceType::Empty ||
                board[0][3].type != PieceType::Empty)
                {
                    return false;
                }

                if(isUnderAttack(state, 3, Colour::Black) ||
                isUnderAttack(state, 2, Colour::Black))
                {
                    return false;
                }
            }
        }
        else{ // Black

            if(castleType == CastleType::Kingside){

                if(!state.bkcastle)
                    return false;

                if(board[7][7].type != PieceType::Rook ||
                board[7][7].colour != Colour::Black)
                {
                    return false;
                }

                if(board[7][5].type != PieceType::Empty ||
                board[7][6].type != PieceType::Empty)
                {
                    return false;
                }

                if(isUnderAttack(state, 61, Colour::White) ||
                isUnderAttack(state, 62, Colour::White))
                {
                    return false;
                }
            }
            else{ // Queenside

                if(!state.bqcastle)
                    return false;

                if(board[7][0].type != PieceType::Rook ||
                board[7][0].colour != Colour::Black)
                {
                    return false;
                }

                if(board[7][1].type != PieceType::Empty ||
                board[7][2].type != PieceType::Empty ||
                board[7][3].type != PieceType::Empty)
                {
                    return false;
                }

                if(isUnderAttack(state, 59, Colour::White) ||
                isUnderAttack(state, 58, Colour::White))
                {
                    return false;
                }
            }
        }
        return true;
}

bool isEnPassant(GameState& state, const Move move){
    auto& board = state.board;
    int fromCol=move.from%8;
    int fromRow=move.from/8;
    int toCol=move.to%8;
    int toRow=move.to/8;
    if(board[fromRow][fromCol].type == PieceType::Pawn &&
    board[toRow][toCol].type == PieceType::Empty &&
    move.to == state.enPassantsqaure &&
    abs(toCol - fromCol) == 1)
    {
        if(board[fromRow][fromCol].colour == Colour::White && toRow == fromRow + 1) return true;
        if(board[fromRow][fromCol].colour == Colour::Black && toRow == fromRow - 1) return true;
    }
    return false;
}
    
bool verify(GameState& state, const Move& move){
    int fromRow=move.from/8;
    int fromCol=move.from%8;
    int toRow=move.to/8;
    int toCol=move.to%8;

    if(toRow > 7 || toRow <0 || toCol >7 || toCol <0) return false;
    if(fromRow > 7 || fromRow <0 || fromCol >7 || fromCol <0) return false;

    //castle
    CastleType castleType = returnCastleType(move);
    if(castleType!=CastleType::None && state.board[fromRow][fromCol].type==PieceType::King) return verifyCastling(state, move, castleType);

    const Board& board = state.board;
    Colour currentPlayer = state.currentPlayer;

    if(board[fromRow][fromCol].colour!=currentPlayer) return false;
    if(toRow==fromRow && toCol==fromCol) return false;
    if(board[fromRow][fromCol].type==PieceType::Empty) return false;
    if(board[toRow][toCol].colour==board[fromRow][fromCol].colour) return false;

    //en passant
    if(isEnPassant(state,move)) return true;

    switch(board[fromRow][fromCol].type){
        case PieceType::Empty: return false; break;
        case PieceType::Pawn: return verifyPawnMovement(fromRow, fromCol, toRow, toCol, board); break;
        case PieceType::Rook: return verifyRookMovement(fromRow, fromCol, toRow, toCol, board); break;
        case PieceType::Knight: return verifyKnightMovement(fromRow, fromCol, toRow, toCol, board); break;
        case PieceType::Bishop: return verifyBishopMovement(fromRow, fromCol, toRow, toCol, board); break;
        case PieceType::Queen: return verifyRookMovement(fromRow, fromCol, toRow, toCol, board) || verifyBishopMovement(fromRow, fromCol, toRow, toCol, board); break;
        case PieceType::King: return verifyKingMovement(fromRow,fromCol,toRow,toCol,board); break;
        default: return false;
    }
    return false; 
}

bool validateForNoPins(Move& move, GameState& state){
    auto& board= state.board;
    Colour kingcolour = state.currentPlayer;
    movePiece(state, move);
    if(isCheck(state,kingcolour)){
        undoMove(state, move);
        return false;
    }else{
        undoMove(state, move);
        return true;
    }
}
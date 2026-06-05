#pragma once
#include <array>
#include <string>
enum class PieceType{
    Empty,
    Pawn,
    Rook,
    Knight,
    Bishop,
    Queen,
    King,
};

enum class Colour{
    Empty,
    Black,
    White,
};

struct Piece{
    PieceType type{PieceType::Empty};
    Colour colour{Colour::Empty};
};

struct GameState{
    std::array<std::array<Piece, 8>, 8> board;
    bool wkcastle{true}, wqcastle{true};
    bool bkcastle{true}, bqcastle{true};
    Colour currentPlayer;
    int enPassantsqaure{-1};
};

struct Move{
    int from{-1};
    int to{-1};

    Piece movedPiece{PieceType::Empty,Colour::Empty};
    Piece capturedPiece{PieceType::Empty,Colour::Empty};
    PieceType promotedPiece{PieceType::Empty};

    bool owkcastle{false}, owqcastle{false};
    bool obkcastle{false}, obqcastle{false};
    bool isCastlingMove{false};

    bool isEnPassantMove{false};
    int oldEnPassantSquare{-1};
};

enum class CastleType{
    None,
    Kingside,
    Queenside,
};

using Board=std::array<std::array<Piece, 8>, 8>;
void initialiseBoard(Board& board);
void displayBoard(const Board& board);
void movePiece(GameState& state,Move& move);
void undoMove(GameState& state,const Move& move);
int sqaureToIndex(const std::string& square);
std::string cordToString(int row, int col);
void changeCurrentPlayer(Colour& currentPlayer);
bool isPawnPromotionMove(const Board& board, const Move& move);
void updateCastlingRights(GameState& state, const Move& move);
CastleType returnCastleType(const Move move);
void updateEnPassantSquare(GameState& state, const Move& move);
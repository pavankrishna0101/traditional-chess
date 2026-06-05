#pragma once
#include "board.h"
#include "verification.h"
#include <vector>
bool isCheck(GameState& state, Colour kingColour);
bool isCheckmate(GameState& state);
bool isUnderAttack(const GameState& state, int squareIndex, Colour attackerColour);
int findKingCord(const GameState& state, Colour colour);
bool isStalemate(GameState& state);
bool isInsufficientMaterial(const GameState& state);
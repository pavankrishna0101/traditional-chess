#pragma once
#include "board.h"
#include "check.h"
#include <cmath>
bool verify(GameState& state, const Move& move);
bool validateForNoPins(Move& move, GameState& state);
bool isEnPassant(GameState& state, const Move move);
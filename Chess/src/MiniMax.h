#pragma once
class Chessboard;
class ChessPiece;
#include "Move.h"

class MiniMax {
public:
	int difficulty;
	Chessboard* chessboard;
	MiniMax(int difficulty, Chessboard* chessboard);
	int CalculateScore(ChessPiece* board[8][8]);
	int Minimax(ChessPiece* board[8][8], bool isMaximising, int depth);
	Move* GetNextMove(ChessPiece* board[8][8], char colour);
};



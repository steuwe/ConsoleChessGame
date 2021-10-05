#pragma once
#include "ChessPiece.h"

class Knight : public ChessPiece {
public:
	Knight();
	Knight(char colour);
	void DeleteMoveList();
	bool CheckMove(Move* move, ChessPiece* board[8][8]);
	Move* GetMoveList(int a, int b, ChessPiece* board[8][8]);
	int GetPotentialMoveCount(int a, int b, ChessPiece* board[8][8]);
	void PrintPiece();
};



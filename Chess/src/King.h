#pragma once
#include "ChessPiece.h"

class King : public ChessPiece {
public:
	King();
	King(char colour);
	void DeleteMoveList();
	int GetPotentialMoveCount(int a, int b, ChessPiece* board[8][8]);
	Move* GetMoveList(int a, int b, ChessPiece* board[8][8]);
	bool CheckMove(Move* move, ChessPiece* board[8][8]);
	void PrintPiece();
};



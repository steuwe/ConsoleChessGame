#pragma once
#include "ChessPiece.h"

class Pawn : public ChessPiece {
public:
	Pawn();
	Pawn(char colour);
	void DeleteMoveList();
	int GetPotentialMoveCount(int a, int b, ChessPiece* board[8][8]);
	Move* GetMoveList(int a, int b, ChessPiece* board[8][8]);
	bool CheckMove(Move* move, ChessPiece* board[8][8]);
	bool CheckMoveBlack(Move* move, ChessPiece* board[8][8]);
	bool CheckMoveWhite(Move* move, ChessPiece* board[8][8]);
	void PrintPiece();
};



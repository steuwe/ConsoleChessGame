#pragma once
#include "ChessPiece.h"
class Bishop : public ChessPiece {
public:
	Bishop();
	Bishop(char colour);
	void DeleteMoveList();
	int GetPotentialMoveCount(int a, int b, ChessPiece* board[8][8]);
	Move* GetMoveList(int a, int b, ChessPiece* board[8][8]);
	bool CheckMove(Move* move, ChessPiece* board[8][8]);
	bool CheckDownLeft(Move* move, ChessPiece* board[8][8]);
	bool CheckUpLeft(Move* move, ChessPiece* board[8][8]);
	bool CheckDownRight(Move* move, ChessPiece* board[8][8]);
	bool CheckUpRight(Move* move, ChessPiece* board[8][8]);
	void PrintPiece();
};

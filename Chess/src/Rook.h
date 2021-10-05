#pragma once
#include "ChessPiece.h"

class Rook : public ChessPiece {
public: 
	Rook();
	Rook(char colour);
	void DeleteMoveList();
	bool CheckMove(Move* move, ChessPiece* board[8][8]);
	Move* GetMoveList(int a, int b, ChessPiece* board[8][8]);
	int GetPotentialMoveCount(int a, int b, ChessPiece* board[8][8]);
	bool CheckUp(Move* move, ChessPiece* board[8][8]);
	bool CheckDown(Move* move, ChessPiece* board[8][8]);
	bool CheckLeft(Move* move, ChessPiece* board[8][8]);
	bool CheckRight(Move* move, ChessPiece* board[8][8]);
	void PrintPiece();
};



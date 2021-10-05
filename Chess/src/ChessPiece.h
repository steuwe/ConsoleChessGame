#pragma once
#include <string>
#include "Chessboard.h"
#include "Move.h"
class ChessPiece
{
public:
	int value;
	char id;
	char colour;
	int px;
	int py;
	Move* potentialMoveList;
	bool justMovedTwo;
	bool castlingRightsKS;
	bool castlingRightsQS;
	ChessPiece(char colour);
	ChessPiece();
	virtual bool CheckMove(Move* move, ChessPiece* board[8][8]);
	virtual int GetPotentialMoveCount(int a, int b, ChessPiece* board[8][8]);
	virtual Move* GetMoveList(int a, int b, ChessPiece* board[8][8]);
	virtual void DeleteMoveList();
	virtual void PrintPiece();
};



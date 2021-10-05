#pragma once
class ChessPiece;

class Move
{
public:
	int px;
	int py;
	int x;
	int y;
	bool enPassant;
	bool promotion;
	bool castling;
	bool movedTwo;
	bool undidMovedTwo;
	bool undidCastlingRightsKS;
	bool undidCastlingRightsQS;
	ChessPiece* capturedPiece;
	Move* next;
	Move* previous;
	Move();
	Move(int px, int py, int x, int y);
	void Reset();
};



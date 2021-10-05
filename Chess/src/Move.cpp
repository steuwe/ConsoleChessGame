#include "Move.h"
#include "ChessPiece.h"


Move::Move(int px, int py, int x, int y) {
	this->px = px;
	this->py = py;
	this->x = x;
	this->y = y;
	castling = false;
	enPassant = false;
	promotion = false;
	movedTwo = false;
	undidMovedTwo = false;
	undidCastlingRightsQS = false;
	undidCastlingRightsKS = false;
	this->next = NULL;
	this->previous = NULL;
	capturedPiece = NULL;
}

Move::Move() {
	this->px = 0;
	this->py = 0;
	this->x = 0;
	this->y = 0;
	castling = false;
	enPassant = false;
	promotion = false;
	movedTwo = false;
	undidMovedTwo = false;
	undidCastlingRightsQS = false;
	undidCastlingRightsKS = false;
	this->next = NULL;
	this->previous = NULL;
	capturedPiece = NULL;
}

void Move::Reset() {
	this->px = 0;
	this->py = 0;
	this->x = 0;
	this->y = 0;
	castling = false;
	enPassant = false;
	promotion = false;
	movedTwo = false;
	undidMovedTwo = false;
	undidCastlingRightsQS = false;
	undidCastlingRightsKS = false;
	this->next = NULL;
	this->previous = NULL;
	capturedPiece = NULL;
}

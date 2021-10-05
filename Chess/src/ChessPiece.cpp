#include "ChessPiece.h"
#include "Chessboard.h"
#include "Move.h"

#pragma once


ChessPiece::ChessPiece(char colour) {
	this->colour = colour;
	this->castlingRightsQS = true;
	this->castlingRightsKS = true;
	this->justMovedTwo = false;
	this->value = 0;
	px = -1;
	py = -1;
	potentialMoveList = NULL;
}

ChessPiece::ChessPiece() {
	this->colour = NULL;
	this->castlingRightsQS = true;
	this->castlingRightsKS = true;
	this->justMovedTwo = false;
	this->value = 0;
	px = -1;
	py = -1;
	potentialMoveList = NULL;
}

bool ChessPiece::CheckMove(Move* move, ChessPiece* board[8][8]) {
	return false;
}

int ChessPiece::GetPotentialMoveCount(int a, int b, ChessPiece* board[8][8]) {
	return 0;
}

Move* ChessPiece::GetMoveList(int a, int b, ChessPiece* board[8][8]) {
	return NULL;
}

void ChessPiece::DeleteMoveList() {
}

void ChessPiece::PrintPiece() {
}

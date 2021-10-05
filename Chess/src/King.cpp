#include "king.h"
#include "ChessPiece.h"
#include "Chessboard.h"
#include "Move.h"
#include <iostream>


King::King() {
	value = 100;
	id = 'k';
	colour = NULL;
	this->castlingRightsKS = true;
	this->castlingRightsQS = true;
	this->px = -1;
	this->py = -1;
	this->potentialMoveList = new Move();
}

King::King(char colour) {
	value = 100;
	id = 'k';
	this->colour = colour;
	this->castlingRightsKS = true;
	this->castlingRightsQS = true;
	this->px = -1;
	this->py = -1;
	this->potentialMoveList = new Move();
}

void King::DeleteMoveList() {
	Move* head = potentialMoveList->next;
	Move* next;
	while (head != NULL) {
		next = head->next;
		delete head;
		head = next;
	}
}

void King::PrintPiece() {
	std::cout << "k";
}
/*
Check if move is legal for this piece on the given board position
@param:
	board is the game board with current position
	move is the move to be played
@return:
	true if move is legal
	false if move is illegal
*/
bool King::CheckMove(Move* move, ChessPiece* board[8][8])
{
	if (move->x < 0 || move->y < 0 || move->x > 7 || move->y > 7) {
		throw "Illegal King move";
	}
	if (move->px == move->x && move->py == move->y) {
		return false;
	}
	//Cannot capture own piece
	if (board[move->x][move->y] != 0 && board[move->x][move->y]->colour == this->colour) {
		return false;
	}
	//Check for castling
	if (move->py == 4 && move->y == 6 && move->x == move->px) { 
		if (this->colour == 'W' && move->px != 0) return false;
		if (this->colour == 'B' && move->px != 7) return false;
		if (board[move->px][5] == NULL && board[move->px][6] == NULL) {
			if (this->castlingRightsKS) {
				move->castling = true;
				return true;
			}
		}
		return false;
	}
	if (move->py == 4 && move->y == 2 && move->x == move->px) {
		if (this->colour == 'W' && move->px != 0) return false;
		if (this->colour == 'B' && move->px != 7) return false;
		if (board[move->px][3] == NULL && board[move->px][2] == NULL &&
				board[move->px][1] == NULL) {
			if (this->castlingRightsQS) {
				move->castling = true;
				return true;
			}
		}
		return false;
	}
	// if distance travelled is more than one square, move is illegal
	if (move->px - move->x < -1 || move->px - move->x > 1) {
		return false;
	}
	if (move->py - move->y < -1 || move->py - move->y > 1) {
		return false;
	}
	
	return true;
}

/*
King mobility should not be counted, as it is a piece that should be protected rather than out in the open.
*/
int King::GetPotentialMoveCount(int a, int b, ChessPiece* board[8][8]) {
	return 0;
}

/*
Makes or returns a list of all potential moves this piece can make.
If the location of the piece hasn't changed since the last list was made, that list is returned.
@param:
	a: current x coordinate
	b: current y coordinate
@returns:
	list of all potential moves
*/
Move* King::GetMoveList(int a, int b, ChessPiece* board[8][8]) {
	if (a == px && b == py) {
		return potentialMoveList;
	}
	//reference to current Node
	Move* moveRef = potentialMoveList;
	moveRef->next = NULL;
	//castling moves
	if (this->colour == 'W') {
		if (this->castlingRightsKS) {
			Move* temp = new Move(0, 4, 0, 6);
			moveRef->next = temp;
			moveRef = temp;
		}
		if (this->castlingRightsQS) {
			Move* temp = new Move(0, 4, 0, 2);
			moveRef->next = temp;
			moveRef = temp;
		}
	}
	else {
		if (this->castlingRightsKS) {
			Move* temp = new Move(7, 4, 7, 6);
			moveRef->next = temp;
			moveRef = temp;
		}
		if (this->castlingRightsQS) {
			Move* temp = new Move(7, 4, 7, 2);
			moveRef->next = temp;
			moveRef = temp;
		}
	}

	//row below current square
	if (a - 1 >= 0) {
		if (b - 1 >= 0) {
			Move* temp = new Move(a, b, a - 1, b - 1);
			moveRef->next = temp;
			moveRef = temp;
		}
		Move* temp = new Move(a, b, a - 1, b);
		moveRef->next = temp;
		moveRef = temp;
		if (b + 1 < 8) {
			Move* temp = new Move(a, b, a - 1, b + 1);
			moveRef->next = temp;
			moveRef = temp;
		}
	}
	//same row as current square
	if (b - 1 >= 0) {
		Move* temp = new Move(a, b, a, b - 1);
		moveRef->next = temp;
		moveRef = temp;
	}
	if (b + 1 < 8) {
		Move* temp = new Move(a, b, a, b + 1);
		moveRef->next = temp;
		moveRef = temp;
	}
	//row above current square
	if (a + 1 < 8) {
		if (b - 1 >= 0) {
			Move* temp = new Move(a, b, a + 1, b - 1);
			moveRef->next = temp;
			moveRef = temp;
		}
		Move* temp = new Move(a, b, a + 1, b);
		moveRef->next = temp;
		moveRef = temp;
		if (b + 1 < 8) {
			Move* temp = new Move(a, b, a + 1, b + 1);
			moveRef->next = temp;
			moveRef = temp;
		}
	}
	px = a;
	py = b;
	return potentialMoveList;
}

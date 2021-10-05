#include "Knight.h"
#include "ChessPiece.h"
#include "Chessboard.h"
#include "Move.h"
#include <iostream>


Knight::Knight() {
	value = 3;
	id = 'K';
	colour = NULL;
	px = -1;
	py = -1;
	potentialMoveList = new Move();
}

Knight::Knight(char colour) {
	value = 3;
	id = 'K';
	this->colour = colour;
	px = -1;
	py = -1;
	potentialMoveList = new Move();
}

void Knight::DeleteMoveList() {
	Move* head = potentialMoveList->next;
	Move* next;
	while (head != NULL) {
		next = head->next;
		delete head;
		head = next;
	}
}

void Knight::PrintPiece() {
	std::cout << "K";
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
bool Knight::CheckMove(Move* move, ChessPiece* board[8][8])
{
	if (move->x >= 8 || move->y >= 8 || move->x < 0 || move->y < 0) {
		throw "Illegal Move";
	}
	//check if piece was moved
	if (move->x == move->px && move->y == move->py) {
		return false;
	}
	//Cannot capture own piece
	if (board[move->x][move->y] != 0 && board[move->x][move->y]->colour == this->colour) {
		return false;
	}
	
	//Check if move is among legal moves
	if (move->x == move->px + 2) {
		if (move->y == move->py + 1) {
			return true;
		}
		if (move->y == move->py - 1) {
			return true;
		}
		else {
			return false;
		}
	} 
	if (move->x == move->px - 2) {
		if (move->y == move->py + 1) {
			return true;
		}
		if (move->y == move->py - 1) {
			return true;
		}
		else {
			return false;
		}
	}

	if (move->y == move->py + 2) {
		if (move->x == move->px + 1) {
			return true;
		}
		if (move->x == move->px - 1) {
			return true;
		}
		else {
			return false;
		}
	}
	if (move->y == move->py - 2) {
		if (move->x == move->px + 1) {
			return true;
		}
		if (move->x == move->px - 1) {
			return true;
		}
		else {
			return false;
		}
	}
	return false;
}

int Knight::GetPotentialMoveCount(int a, int b, ChessPiece* board[8][8]) {
	int moves = 0;
	if (a + 2 < 8) {
		if (b + 1 < 8) {
			if (board[a + 2][b + 1] == NULL || (board[a + 2][b + 1]->colour != this->colour)) {
				moves++;
			}
		}
		if (b - 1 >= 0) {
			if (board[a + 2][b - 1] == NULL || (board[a + 2][b - 1]->colour != this->colour)) {
				moves++;
			}
		}
	}
	if (a + 1 < 8) {
		if (b - 2 >= 0) {
			if (board[a + 1][b - 2] == NULL || (board[a + 1][b - 2]->colour != this->colour)) {
				moves++;
			}
		}
		if (b + 2 < 8) {
			if (board[a + 1][b + 2] == NULL || (board[a + 1][b + 2]->colour != this->colour)) {
				moves++;
			}
		}
	}
	if (a - 1 >= 0){
		if (b - 2 >= 0) {
			if (board[a - 1][b - 2] == NULL || (board[a - 1][b - 2]->colour != this->colour)) {
				moves++;
			}
		}
		if (b + 2 < 8) {
			if (board[a - 1][b + 2] == NULL || (board[a - 1][b + 2]->colour != this->colour)) {
				moves++;
			}
		}
	}
	if (a - 2 >= 0) {
		if (b + 1 < 8) {
			if (board[a - 2][b + 1] == NULL || (board[a - 2][b + 1]->colour != this->colour)) {
				moves++;
			}
		}
		if (b - 1 >= 0) {
			if (board[a - 2][b - 1] == NULL || (board[a - 2][b - 1]->colour != this->colour)) {
				moves++;
			}
		}
	}
	return moves;
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
Move* Knight::GetMoveList(int a, int b, ChessPiece* board[8][8]) {
	if (a == px && b == py) {
		return potentialMoveList;
	}
	//reference to current Node
	Move* moveRef = potentialMoveList;	
	moveRef->next = NULL;
	if (a + 2 < 8) {
		if (b + 1 < 8) {
			Move* temp = new Move(a, b, a + 2, b + 1);
			moveRef->next = temp;
			moveRef = temp;
		}
		if (b - 1 >= 0) {
			Move* temp = new Move(a, b, a + 2, b - 1);
			moveRef->next = temp;
			moveRef = temp;
		}
	}
	if (a + 1 < 8) {
		if (b - 2 >= 0) {
			Move* temp = new Move(a, b, a + 1, b - 2);
			moveRef->next = temp;
			moveRef = temp;
		}
		if (b + 2 < 8) {
			Move* temp = new Move(a, b, a + 1, b + 2);
			moveRef->next = temp;
			moveRef = temp;
		}
	}
	if (a - 1 >= 0) {
		if (b - 2 >= 0) {
			Move* temp = new Move(a, b, a - 1, b - 2);
			moveRef->next = temp;
			moveRef = temp;
		}
		if (b + 2 < 8) {
			Move* temp = new Move(a, b, a - 1, b + 2);
			moveRef->next = temp;
			moveRef = temp;
		}
	}
	if (a - 2 >= 0) {
		if (b + 1 < 8) {
			Move* temp = new Move(a, b, a - 2, b + 1);
			moveRef->next = temp;
			moveRef = temp;
		}
		if (b - 1 >= 0) {
			Move* temp = new Move(a, b, a - 2, b - 1);
			moveRef->next = temp;
			moveRef = temp;
		}
	}
	px = a;
	py = b;
	return potentialMoveList;
}

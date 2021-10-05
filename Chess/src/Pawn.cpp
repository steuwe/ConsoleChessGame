#include "Pawn.h"
#include "ChessPiece.h"
#include "Chessboard.h"
#include "Move.h"
#include <iostream>

Pawn::Pawn() {
	value = 1;
	id = 'P';
	colour = NULL;
	justMovedTwo = false;
	px = -1;
	py = -1;
	potentialMoveList = new Move();
}

Pawn::Pawn(char colour) {
	value = 1;
	id = 'P';
	justMovedTwo = false;
	this->colour = colour;
	px = -1;
	py = -1;
	potentialMoveList = new Move();
}

void Pawn::DeleteMoveList() {
	Move* head = potentialMoveList->next;
	Move* next;
	while (head != NULL) {
		next = head->next;
		delete head;
		head = next;
	}
}

void Pawn::PrintPiece() {
	std::cout << "P";
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
bool Pawn::CheckMove(Move* move, ChessPiece* board[8][8])
{
	if (move->x >= 8 || move->y >= 8 || move->x < 0 || move->y < 0) {
		throw "Illegal Move";
	}
	//check if piece was moved
	if (move->x == move->px && move->y == move->py) {
		return false;
	}
	//cannot capture own piece
	if (board[move->x][move->y] != 0 && board[move->x][move->y]->colour == this->colour) {
		return false;
	}
	//check for each colour
	if (this->colour == 'B') {
		return CheckMoveBlack(move, board);
	} else 
	if (this->colour == 'W') {
		return CheckMoveWhite(move, board);
	}
	else {
		throw "Pawn has no colour!";
	}
	return false;
}

/*
Checks the move for a white pawn, which can only go down the board.
Checks for en moving 1 square and 2 squares.
@param:
	move: move to be played
	board: game board with current position
@return:
	true if move is legal
	false if move is illegal
*/
bool Pawn::CheckMoveWhite(Move* move, ChessPiece* board[8][8])
{
	//if pawn moves diagonally:
	if (move->x == move->px + 1 && move->y == move->py + 1) {
		//check if opponent piece is captured
		if (board[move->x][move->y] != 0) {
			if (move->x == 7) {
				move->promotion = true;
			}
			return true;
		}
		//check en passant
		if (board[move->px][move->py + 1] != NULL && board[move->px][move->py + 1]->value == 1
			&& board[move->px][move->py + 1]->justMovedTwo && board[move->px][move->py + 1]->colour != this->colour) {
			move->enPassant = true;
			return true;
		}
		// if no capture accurs, move is illegal
		return false;
	}
	if (move->x == move->px + 1 && move->y == move->py - 1) {
		//check if opponent piece is captured
		if (board[move->x][move->y] != 0) {
			if (move->x == 7) {
				move->promotion = true;
			}
			return true;
		}
		//check en passant
		if (board[move->px][move->py - 1] != NULL && board[move->px][move->py - 1]->value == 1 
			&& board[move->px][move->py - 1]->justMovedTwo && board[move->px][move->py - 1]->colour != this->colour) {
			move->enPassant = true;
			return true;
		}
		// if no capture accurs, move is illegal
		return false;
	}
	//check for moving 1 square
	if (move->x == move->px + 1 && move->y == move->py && board[move->x][move->y] == 0) {
		if (move->x == 7) {
			move->promotion = true;
		}
		return true;
	}
	//check for moving 2 squares
	if (move->x == move->px + 2 && move->y == move->py) {
		//if pawn has not moved yet, check if 2 squares are free
		if (move->px == 1) {
			if (board[move->px + 1][move->py] == 0 && board[move->px + 2][move->py] == 0) {
				this->justMovedTwo = true; 
				move->movedTwo = true; //used to reset 'justMovedTwo' when this move is undone
				return true;
			}
		}
	}
	return false;
}

/*
Checks the move for a black pawn, which can only go up the board.
Checks for moving 1 square and moving 2 squares
@param:
	move: move to be played
	board: game board with current position
@return:
	true if move is legal
	false if move is illegal
*/
bool Pawn::CheckMoveBlack(Move* move, ChessPiece* board[8][8])
{
	
	//if pawn moves diagonally:
	if (move->x == move->px - 1 && move->y == move->py + 1) {
		//check if opponent piece is captured
		if (board[move->x][move->y] != 0) {
			if (move->x == 0) {
				move->promotion = true;
			}
			return true;
		}
		//check en passant
		if (board[move->px][move->py + 1] != NULL && board[move->px][move->py + 1]->value == 1
			&& board[move->px][move->py + 1]->justMovedTwo && board[move->px][move->py + 1]->colour != this->colour) {
			move->enPassant = true;
			return true;
		}
		// if no capture accurs, move is illegal
		return false;
	}
	if (move->x == move->px - 1 && move->y == move->py - 1) {
		//check if opponent piece is captured
		if (board[move->x][move->y] != 0) {
			if (move->x == 0) {
				move->promotion = true;
			}
			return true;
		}
		//check en passant
		if (board[move->px][move->py - 1] != NULL && board[move->px][move->py - 1]->value == 1
			&& board[move->px][move->py - 1]->justMovedTwo && board[move->px][move->py - 1]->colour != this->colour) {
			move->enPassant = true;
			return true;
		}
		// if no capture accurs, move is illegal
		return false;
	}
	//check for moving 1 square
	if (move->x == move->px - 1 && move->y == move->py && board[move->x][move->y] == 0) {
		if (move->x == 0) {
			move->promotion = true;
		}
		return true;
	}
	//check for moving 2 squares
	if (move->x == move->px - 2 && move->y == move->py) {
		//if pawn has not moved yet, check if 2 squares are free
		if (move->px == 6) {
			if (board[move->px - 1][move->py] == 0 && board[move->px - 2][move->py] == 0) {
				this->justMovedTwo = true; 
				move->movedTwo = true;
				return true;
			}
		}
	}
	return false;
}

/*
Counts the available squares to move to, given the starting coordinates
Captures and en-passant are not accounted for.
@param:
	a: the x-coordinate of the piece to be moved
	b: the y-coordinate of the piece to be moved
*/
int Pawn::GetPotentialMoveCount(int a, int b, ChessPiece* board[8][8]) {
	int moves = 0;
	if (this->colour == 'W') {
		if (a + 1 < 8 && board[a + 1][b] == NULL) moves++;
		if (a + 2 < 8 && board[a + 2][b] == NULL) moves++;
	}
	else {
		if (a - 1 >= 0 && board[a - 1][b] == NULL) moves++;
		if (a - 2 >= 0 && board[a - 2][b] == NULL) moves++;
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
Move* Pawn::GetMoveList(int a, int b, ChessPiece* board[8][8]) {
	if (a == px && b == py) {
		return potentialMoveList;
	}
	//reference to current Node
	Move* moveRef = potentialMoveList;
	moveRef->next = NULL;
	if (this->colour == 'W') {
		if (a + 1 < 8) {
			Move* temp = new Move(a, b, a + 1, b);
			moveRef->next = temp;
			moveRef = temp;
			if (b - 1 >= 0) {
				Move* temp = new Move(a, b, a + 1, b - 1);
				moveRef->next = temp;
				moveRef = temp;
			}
			if (b + 1 < 8) {
				Move* temp = new Move(a, b, a + 1, b + 1);
				moveRef->next = temp;
				moveRef = temp;
			}
		}
		if (a + 2 < 8) {
			Move* temp = new Move(a, b, a + 2, b);
			moveRef->next = temp;
			moveRef = temp;
		}
	}
	else {
		if (a - 1 >= 0) {
			Move* temp = new Move(a, b, a - 1, b);
			moveRef->next = temp;
			moveRef = temp;
			if (b - 1 >= 0) {
				Move* temp = new Move(a, b, a - 1, b - 1);
				moveRef->next = temp;
				moveRef = temp;
			}
			if (b + 1 < 8) {
				Move* temp = new Move(a, b, a - 1, b + 1);
				moveRef->next = temp;
				moveRef = temp;
			}
		}
		if (a - 2 >= 0) {
			Move* temp = new Move(a, b, a - 2, b);
			moveRef->next = temp;
			moveRef = temp;
		}
	}
	px = a;
	py = b;
	return potentialMoveList;
}

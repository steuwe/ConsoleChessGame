#include "Rook.h"
#include "ChessPiece.h"
#include "Chessboard.h"
#include "Move.h"
#include <iostream>

Rook::Rook() {
	value = 5;
	id = 'R';
	colour = NULL;
	px = -1;
	py = -1;
	potentialMoveList = new Move();
}

Rook::Rook(char colour) {
	value = 5;
	id = 'R';
	this->colour = colour;
	px = -1;
	py = -1;
	potentialMoveList = new Move();
}

void Rook::DeleteMoveList() {
	Move* head = potentialMoveList->next;
	Move* next;
	while (head != NULL) {
		next = head->next;
		delete head;
		head = next;
	}
}

void Rook::PrintPiece() {
	std::cout << "R";
}

/*
Check if move is legal for this piece on the given board position
///////Since the board has been changed from white pieces being on row[7] to row[0], function names are no longer accurate
@param:
	board is the game board with current position
	move is the move to be played
@return:
	true if move is legal
	false if move is illegal
*/
bool Rook::CheckMove(Move* move, ChessPiece* board[8][8])
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
	//cannot move diagonally
	if (move->x != move->px && move->y != move->py) {
		return false;
	}
	//check horizontals and verticals
	if (move->x == move->px) {
		if (move->y < move->py) {
			return CheckLeft(move, board);
		}
		if (move->y > move->py) {
			return CheckRight(move, board);
		}
	}
	if (move->y == move->py) {
		if (move->x < move->px) {
			return CheckUp(move, board);
		}
		if (move->x > move->px) {
			return CheckDown(move, board);
		}
	}
	return false;
}

/*
Follow path up until move is found or edge of the board is reached.
@param:
	move = the move played
	board = the playing board
@return
	false if there is a piece in the way or the coordinates were not found on the path
	true if move was reached
*/
bool Rook::CheckDown(Move* move, ChessPiece* board[8][8])
{
	int i = move->px;
	// move along path until edge of board is reached
	while (i < 8) {
		// move one square up
		i++;
		// if destination square has been reached, move must be legal
		if (i == move->x) {
			return true;
		}
		// if piece is in the way, move is illegal
		if (i < 8 && board[i][move->y] != 0) {
			return false;
		}
	}
	return false;
}

/*
Follow path down until move is found or edge of the board is reached.
@param:
	move = the move played
	board = the playing board
@return
	false if there is a piece in the way or the coordinates were not found on the path
	true if move was reached
*/
bool Rook::CheckUp(Move* move, ChessPiece* board[8][8])
{
	int i = move->px;
	// move along path until edge of board is reached
	while (i >= 0) {
		// move one square up
		i--;
		// if destination square has been reached, move must be legal
		if (i == move->x) {
			return true;
		}
		// if piece is in the way, move is illegal
		if (i >= 0 && board[i][move->y] != 0) {
			return false;
		}
	}
	return false;
}

/*
Follow path right until move is found or edge of the board is reached.
@param:
	move = the move played
	board = the playing board
@return
	false if there is a piece in the way or the coordinates were not found on the path
	true if move was reached
*/
bool Rook::CheckRight(Move* move, ChessPiece* board[8][8])
{
	int j = move->py;
	// move along path until edge of board is reached
	while (j < 8) {
		// move one square to the right
		j++;
		// if destination square has been reached, move must be legal
		if (j == move->y) {
			return true;
		}
		// if piece is in the way, move is illegal
		if (j < 8 && board[move->x][j] != 0) {
			return false;
		}
	}
	return false;
}

/*
Follow path left until move is found or edge of the board is reached.
@param:
	move = the move played
	board = the playing board
@return
	false if there is a piece in the way or the coordinates were not found on the path
	true if move was reached
*/
bool Rook::CheckLeft(Move* move, ChessPiece* board[8][8])
{
	int j = move->py;
	// move along path until edge of board is reached
	while (j >= 0) {
		// move one square to the left
		j--;
		// if destination square has been reached, move must be legal
		if (j == move->y) {
			return true;
		}
		// if piece is in the way, move is illegal
		if (j >= 0 && board[move->x][j] != 0) {
			return false;
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
int Rook::GetPotentialMoveCount(int a, int b, ChessPiece* board[8][8]) {
	int moves = 0;
	int i = 0;
	int j = 0;
	//check moves down 
	i = a - 1;
	j = b;
	while (i >= 0 && board[i][j] == NULL) {
		i--;
		moves++;
	}
	if (i >= 0 && board[i][j] != NULL && board[i][j]->colour != this->colour) moves++;
	//check moves up
	i = a + 1;
	j = b;
	while (i < 8 && board[i][j] == NULL) {
		i++;
		moves++;
	}
	if (i < 8 && board[i][j] != NULL && board[i][j]->colour != this->colour) moves++;
	//check moves to the left
	i = a;
	j = b - 1;
	while (j >= 0 && board[i][j] == NULL) {
		j--;
		moves++;
	}
	if (j >= 0 && board[i][j] != NULL && board[i][j]->colour != this->colour) moves++;
	//check moves to the right
	i = a;
	j = b + 1;
	while (j < 8 && board[i][j] == NULL) {
		j++;
		moves++;
	}
	if (j < 8 && board[i][j] != NULL && board[i][j]->colour != this->colour) moves++;

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
Move* Rook::GetMoveList(int a, int b, ChessPiece* board[8][8]) {
	if (a == px && b == py) {
		return potentialMoveList;
	}
	//reference to current Node
	Move* moveRef = potentialMoveList;
	moveRef->next = NULL;
	int i = 0;
	int j = 0;
	//check moves down 
	i = a - 1;
	j = b;
	while (i >= 0) {
		Move* temp = new Move(a, b, i, j);
		moveRef->next = temp;
		moveRef = temp;
		i--;
	}
	//check moves up
	i = a + 1;
	j = b;
	while (i < 8) {
		Move* temp = new Move(a, b, i, j);
		moveRef->next = temp;
		moveRef = temp;
		i++;
	}
	//check moves to the left
	i = a;
	j = b - 1;
	while (j >= 0) {
		Move* temp = new Move(a, b, i, j);
		moveRef->next = temp;
		moveRef = temp;
		j--;
	}
	//check moves to the right
	i = a;
	j = b + 1;
	while (j < 8) {
		Move* temp = new Move(a, b, i, j);
		moveRef->next = temp;
		moveRef = temp;
		j++;
	}
	px = a;
	py = b;
	return potentialMoveList;
}

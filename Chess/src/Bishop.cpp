#include "Bishop.h"
#include "ChessPiece.h"
#include "Chessboard.h"
#include "Move.h"
#include <iostream>

Bishop::Bishop() {
	value = 3;
	id = 'B';
	colour = NULL;
	px = -1;
	py = -1;
	potentialMoveList = new Move();
}

Bishop::Bishop(char colour) {
	this->colour = colour;
	id = 'B';
	value = 3;
	px = -1;
	py = -1;
	potentialMoveList = new Move();
}

void Bishop::DeleteMoveList() {
	Move* head = potentialMoveList->next;
	Move* next;
	while (head != NULL) {
		next = head->next;
		delete head;
		head = next;
	}
}

void Bishop::PrintPiece() {
	std::cout << "B";
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
bool Bishop::CheckMove(Move* move, ChessPiece* board[8][8]) {
	if (move->x >= 8 || move->y >= 8 || move->x < 0 || move->y < 0) {
		throw "Illegal Move";
	}
	//check if piece was moved
	if (move->x == move->px && move->y == move->py) {
		return false;
	}
	//Bishop cannot have same x or y coordinate after moving.
	if (move->x == move->px || move->y == move->py) {
		return false;
	}
	//Cannot capture own piece
	if (board[move->x][move->y] != 0 && board[move->x][move->y]->colour == this->colour) {
		return false;
	}
	if (move->x < move->px && move->y < move->py) {
		return CheckUpLeft(move, board);
	}
	if (move->x < move->px && move->y > move->py) {
		return CheckUpRight(move, board);
	}
	if (move->x > move->px && move->y < move->py) {
		return CheckDownLeft(move, board);
	}
	if (move->x > move->px && move->y > move->py) {
		return CheckDownRight(move, board);
	}
	return false;
}

/*
Follow up-left diagonal until move is found or edge of the board is reached.
@param:
	move = the move played
	board = the playing board
@return
	false if there is a piece in the way or the coordinates were not found on the path
	true if move was reached
*/
bool Bishop::CheckUpLeft(Move* move, ChessPiece* board[8][8])
{
	int i = move->px;
	int j = move->py;
	// move along diagonal until edge of board is reached
	while (i >= 0 && j >= 0) {
		// move one square up-left
		i--;
		j--;
		// if destination square has been reached, move must be legal
		if (i == move->x && j == move->y) {
			return true;
		}
		// if piece is in the way, move is illegal
		if (board[i][j] != 0) {
			return false;
		} 
	}
	//if edge of board was found and destination square was not reached, move must be illegal
	return false;
}

/*
Follow up-right diagonal until move is found or edge of the board is reached.
@param:
	move = the move played
	board = the playing board
@return
	false if there is a piece in the way or the coordinates were not found on the path
	true if move was reached
*/
bool Bishop::CheckUpRight(Move* move, ChessPiece* board[8][8])
{
	int i = move->px;
	int j = move->py;
	// move along diagonal until edge of board is reached
	while (i >= 0 && j < 8) {
		// move one square up-right
		i--;
		j++;
		// if destination square has been reached, move must be legal
		if (i == move->x && j == move->y) {
			return true;
		}
		// if piece is in the way, move is illegal
		if (i >= 0 && j < 8 && board[i][j] != NULL) {
			return false;
		}
	}
	//if edge of board was found and destination square was not reached, move must be illegal
	return false;
}

/*
Follow down-left diagonal until move is found or edge of the board is reached.
@param:
	move = the move played
	board = the playing board
@return
	false if there is a piece in the way or the coordinates were not found on the path
	true if move was reached
*/
bool Bishop::CheckDownLeft(Move* move, ChessPiece* board[8][8])
{
	int i = move->px;
	int j = move->py;
	// move along diagonal until edge of board is reached
	while (i < 8 && j >= 0) {
		// move one square down-left
		i++;
		j--;
		// if destination square has been reached, move must be legal
		if (i == move->x && j == move->y) {
			return true;
		}
		// if piece is in the way, move is illegal
		if (i < 8 && j >= 0 && board[i][j] != 0) {
			return false;
		}
	}
	//if edge of board was found and destination square was not reached, move must be illegal
	return false;
}

/*
Follow down-right diagonal until move is found or edge of the board is reached. 
@param:
	move = the move played
	board = the playing board
@return 
	false if there is a piece in the way or the coordinates were not found on the path
	true if move was reached
*/
bool Bishop::CheckDownRight(Move* move, ChessPiece* board[8][8])
{
	int i = move->px;
	int j = move->py;
	// move along diagonal until edge of board is reached
	while (i < 8 && j < 8) {
		// move one square down-right
		i++;
		j++;
		// if destination square has been reached, move must be legal
		if (i == move->x && j == move->y) {
			return true;
		}
		// if piece is in the way, move is illegal
		if (i < 8 && j < 8 && board[i][j] != NULL) {
			return false;
		}
	}
	//if edge of board was found and destination square was not reached, move must be illegal
	return false;
}

/*
Counts the available squares to move to, given the starting coordinates
@param:
	a: the x-coordinate of the piece to be moved
	b: the y-coordinate of the piece to be moved
*/
int Bishop::GetPotentialMoveCount(int a, int b, ChessPiece* board[8][8]) {
	int moves = 0;
	int i = 0;
	int j = 0;

	//check down left
	i = a - 1;
	j = b - 1;
	while (i >= 0 && j >= 0 && board[i][j] == NULL) {
		i--;
		j--;
		moves++;
	}
	if (i >= 0 && j >= 0 && board[i][j] != NULL && board[i][j]->colour != this->colour) moves++;
	//check up right
	i = a + 1;
	j = b + 1;
	while (i < 8 && j < 8 && board[i][j] == NULL) {
		i++;
		j++;
		moves++;
	}
	if (i < 8 && j < 8 && board[i][j] != NULL && board[i][j]->colour != this->colour) moves++;
	//check up left
	i = a + 1;
	j = b - 1;
	while (i < 8 && j >= 0 && board[i][j] == NULL) {
		i++;
		j--;
		moves++;
	}
	if (i < 8 && j >= 0 && board[i][j] != NULL && board[i][j]->colour != this->colour) moves++;
	//check down right
	i = a - 1;
	j = b + 1;
	while (i >= 0 && j < 8 && board[i][j] == NULL) {
		i--;
		j++;
		moves++;
	}
	if (i >= 0 && j < 8 && board[i][j] != NULL && board[i][j]->colour != this->colour) moves++;

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
Move* Bishop::GetMoveList(int a, int b, ChessPiece* board[8][8]) {
	int i = 0;
	int j = 0;

	if (a == px && b == py) {
		return potentialMoveList;
	}
	
	Move* moveRef = potentialMoveList;
	moveRef->next = NULL; //so that the old list is not returned if no new move is added
	//check down left
	i = a - 1;
	j = b - 1;
	while (i >= 0 && j >= 0) {
		Move* temp = new Move(a, b, i, j);
		moveRef->next = temp;
		moveRef = temp;
		i--;
		j--;
	}
	
	//check up right
	i = a + 1;
	j = b + 1;
	while (i < 8 && j < 8) {
		Move* temp = new Move(a, b, i, j);
		moveRef->next = temp;
		moveRef = temp;
		i++;
		j++;
	}
	//check up left
	i = a + 1;
	j = b - 1;
	while (i < 8 && j >= 0) {
		Move* temp = new Move(a, b, i, j);
		moveRef->next = temp;
		moveRef = temp;
		i++;
		j--;
	}
	//check down right
	i = a - 1;
	j = b + 1;
	while (i >= 0 && j < 8) {
		Move* temp = new Move(a, b, i, j);
		moveRef->next = temp;
		moveRef = temp;
		i--;
		j++;
	}
	px = a;
	py = b;
	return potentialMoveList;
}

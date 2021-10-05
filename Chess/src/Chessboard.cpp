#include "Chessboard.h"
#include "ChessPiece.h"
#include "Pawn.h"
#include "Bishop.h"
#include "King.h"
#include "Queen.h"
#include "Knight.h"
#include "Rook.h"
#include "MiniMax.h"
#include "Move.h"
#include <iostream>
#include <cstdlib>
#include <time.h>
#include <sys/time.h>
#include "Zobrist.h"
#include "Entry.h"
#include "PositionEval.h"

Chessboard::Chessboard(int difficulty) {
	gameOver = true;
	moves = 0;
	scoreBlack = 0;
	scoreWhite = 0;
	materialCountBlack = 39;
	materialCountWhite = 39;
	this->difficulty = difficulty;
	tt = new Zobrist();
	blackKing_posx = 7;
	blackKing_posy = 4;
	whiteKing_posx = 0;
	whiteKing_posy = 4;
	hasWhiteCastled = false;
	hasBlackCastled = false;
	moveListBlack = new Move(0, 0, 0, 0);
	moveListWhite = new Move(0, 0, 0, 0);
	for (int i = 16; i < 24; i++) {
		pieces[i] = new Pawn('W');
		pieces[i + 8] = new Pawn('B');
	}
	pieces[0] = new Rook('W');
	pieces[1] = new Rook('W');
	pieces[2] = new Rook('B');
	pieces[3] = new Rook('B');
	pieces[4] = new Knight('W');
	pieces[5] = new Knight('W');
	pieces[6] = new Knight('B');
	pieces[7] = new Knight('B');
	pieces[8] = new Bishop('W');
	pieces[9] = new Bishop('W');
	pieces[10] = new Bishop('B');
	pieces[11] = new Bishop('B');
	pieces[12] = new King('W');
	pieces[13] = new King('B');
	pieces[14] = new Queen('W');
	pieces[15] = new Queen('B');

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			board[i][j] = 0;
		}
	}

	for (int i = 0; i < 8; i++) {
		board[1][i] = pieces[i + 16];
		board[6][i] = pieces[i + 24];
	}
	board[0][0] = pieces[0];
	board[0][7] = pieces[1];
	board[7][0] = pieces[2];
	board[7][7] = pieces[3];
	board[0][1] = pieces[4];
	board[0][6] = pieces[5];
	board[7][1] = pieces[6];
	board[7][6] = pieces[7];
	board[0][2] = pieces[8];
	board[0][5] = pieces[9];
	board[7][2] = pieces[10];
	board[7][5] = pieces[11];
	board[0][4] = pieces[12];
	board[7][4] = pieces[13];
	board[0][3] = pieces[14];
	board[7][3] = pieces[15];
	for (int i = 0; i < 8; i++) {
		queens[i] = new Queen('W');
		queens[i + 8] = new Queen('B');
	}
	extraWhiteQueens = 0;
	extraBlackQueens = 8;
	for (int i = 0; i < 16; i++) {
		killerMoves[i] = new Move();
	}
}

Chessboard::~Chessboard() {
	for (int i = 0; i < 32; i++) {
		delete pieces[i];
	}
	for (int i = 0; i < 16; i++) {
		delete queens[i];
		delete killerMoves[i];
	}

	Move* current = moveListBlack;
	Move* previous;
	while (current != NULL) {
		previous = current->previous;
		delete current;
		current = previous;
	}
	current = moveListWhite;
	while (current != NULL) {
		previous = current->previous;
		delete current;
		current = previous;
	}
	tt->~Zobrist();

	//delete moveListBlack; why does this throw an exception?
	//delete moveListWhite;

}

void Chessboard::Start() {
	gameOver = false;
	do {
		if (!PlayWhiteMove(GetNextMove('W'))) {
			gameOver = true;
			return;
		}
		if (!PlayBlackMove(GetNextMove('B'))) {
			gameOver = true;
			return;
		}
	} while (!gameOver);
}

/*
 Gets next move from CPU.
 */
Move* Chessboard::GetNextMove(char colour) {
	//return moveGenerator->GetNextMove(board, colour);
	return GetNextMinimaxMove_MoveList(colour);
}

/*
 Plays a move for white if it is deemed legal.
 CheckMove() checks if there is a piece in the way or coordinates are wrong.
 isInCheck() checks if the player would be in check after playing the move, making it illegal.
 @param:
 move: the move which is to be played
 @return:
 true: if move is legal
 false: if move is illegal
 */
bool Chessboard::PlayWhiteMove(Move* move) {
	if (move->px < 0 || move->px > 7 || move->py < 0 || move->py > 7
			|| move->x < 0 || move->x > 7 || move->y < 0 || move->y > 7) {
		throw "Wrong Coordinates";
	}
	//check if piece was moved
	if (move->px == move->x && move->py == move->y) {
		return false;
	}
	//check if white piece is being moved
	if (board[move->px][move->py] == NULL
			|| board[move->px][move->py]->colour == 'B') {
		return false;
	}

	ChessPiece* movedPiece = board[move->px][move->py];
	ChessPiece* capturedPiece = board[move->x][move->y];
	bool isLegal = movedPiece->CheckMove(move, board);
	//if move is not among possible moves for the piece, return false. 
	//else, add the move and undo it later in this function if other rules do not allow it
	if (!isLegal) {
		return false;
	}

	//if king is moved, check castling and update position for easier logic in isInCheck
	if (movedPiece->value == 100) {
		return CheckKingMoveWhite(move, movedPiece, capturedPiece);
	}

	//check for pawn promotion or en passant
	if (movedPiece->value == 1) {
		if (move->x == move->px + 1) { //if pawn moves one square, justMovedTwo value is no longer valid
			if (movedPiece->justMovedTwo) {
				move->undidMovedTwo = true; //lets UndoMove know if justMovedTwo should be reset
			}
			movedPiece->justMovedTwo = false;
		}
		if (move->promotion) { //check for promotion
			//switch out an extra queen for the pawn
			for (int i = 0; i < 8; i++) {
				if (queens[i]->id == 'Q' && queens[i]->colour == 'W') {
					ChessPiece* temp = queens[extraWhiteQueens];
					queens[extraWhiteQueens] = movedPiece;
					movedPiece = temp;
					extraWhiteQueens++;
					materialCountWhite += 8;
					break;
				}
			}
		}
		if (move->enPassant) { // check for en passant
			if (!moveListBlack->movedTwo || moveListBlack->x != move->x - 1
					|| moveListBlack->y != move->y) {
				return false;
			}
			move->capturedPiece = board[move->x - 1][move->y];
			materialCountBlack -= 1;
			board[move->x - 1][move->y] = NULL; //remove captured pawn
		}
	}

	//check if piece is captured
	if (board[move->x][move->y] != 0) {
		capturedPiece = board[move->x][move->y];
		move->capturedPiece = capturedPiece;
		materialCountBlack -= capturedPiece->value;
	}
	board[move->x][move->y] = movedPiece;
	board[move->px][move->py] = NULL;

	if (movedPiece->value == 5) { //check state of castling rights before rook move, in case the rook move is undone
		if (move->py == 0) {
			if (board[whiteKing_posx][whiteKing_posy]->castlingRightsQS) {
				move->undidCastlingRightsQS = true;
			}
			board[whiteKing_posx][whiteKing_posy]->castlingRightsQS = false;
		} else if (move->py == 7) {
			if (board[whiteKing_posx][whiteKing_posy]->castlingRightsKS) {
				move->undidCastlingRightsKS = true;
			}
			board[whiteKing_posx][whiteKing_posy]->castlingRightsKS = false;
		}
	}

	addWhiteMove(move); //for correct UndoMove, which calls removeMove

	// if white was first to give check, it doesnt matter if he would be in check afterwards
	if (isInCheck('W')) {
		UndoMove(move, board);
		return false;
	}
	return true;
}

/*
 Plays a move for black if it is deemed legal. CheckMove() checks if there is a piece in the way or coordinates are wrong.
 isInCheck() checks if the player would be in check after playing the move, making it illegal.
 @param:
 move: the move which is to be played
 @return:
 true: if move is legal
 false: if move is illegal
 */
bool Chessboard::PlayBlackMove(Move* move) {
	if (move->px < 0 || move->px > 7 || move->py < 0 || move->py > 7
			|| move->x < 0 || move->x > 7 || move->y < 0 || move->y > 7) {
		throw "Wrong Coordinates";
	}
	//check if piece was moved
	if (move->px == move->x && move->py == move->y) {
		return false;
	}
	//check if black piece is being moved
	if (board[move->px][move->py] == NULL
			|| board[move->px][move->py]->colour == 'W') {
		return false;
	}

	ChessPiece* movedPiece = board[move->px][move->py];
	ChessPiece* capturedPiece = board[move->x][move->y];
	bool isLegal = movedPiece->CheckMove(move, board);
	//if move is legal:
	if (!isLegal) {
		return false;
	}

	//if king is moved, delegate to other function
	if (movedPiece->value == 100) {
		return CheckKingMoveBlack(move, movedPiece, capturedPiece);
	}

	//check for pawn promotion or en passant
	if (movedPiece->value == 1) {
		if (move->x == move->px - 1) { //if pawn moves one square, justMovedTwo value is no longer valid
			if (movedPiece->justMovedTwo) {
				move->undidMovedTwo = true; //lets UndoMove know if justMovedTwo should be reset
			}
			movedPiece->justMovedTwo = false;
		}
		if (move->promotion) { //check for promotion
			for (int i = 8; i < 16; i++) {
				if (queens[i]->id == 'Q' && queens[i]->colour == 'B') {
					ChessPiece* temp = queens[i];
					queens[i] = movedPiece;
					movedPiece = temp;
					extraBlackQueens++;
					materialCountBlack += 8;
					break;
				}
			}
		}
		if (move->enPassant) { // check for en passant
			if (!moveListWhite->movedTwo || moveListWhite->x != move->x + 1
					|| moveListWhite->y != move->y) {
				return false;
			}
			move->capturedPiece = board[move->x + 1][move->y];
			materialCountWhite -= 1;
			board[move->x + 1][move->y] = NULL;
		}
	}

	//check if piece is captured
	if (board[move->x][move->y] != 0) {
		capturedPiece = board[move->x][move->y];
		move->capturedPiece = capturedPiece;
		materialCountWhite -= capturedPiece->value;
	}
	board[move->x][move->y] = movedPiece;
	board[move->px][move->py] = NULL;

	if (movedPiece->value == 5) { //if rook is moved, king cannot castle that side 
		if (move->py == 0) { //check if castling rights were intact before
			if (board[blackKing_posx][blackKing_posy]->castlingRightsQS) {
				move->undidCastlingRightsQS = true;
			}
			board[blackKing_posx][blackKing_posy]->castlingRightsQS = false;
		} else if (move->py == 7) {
			if (board[blackKing_posx][blackKing_posy]->castlingRightsKS) {
				move->undidCastlingRightsKS = true;
			}
			board[blackKing_posx][blackKing_posy]->castlingRightsKS = false;
		}
	}

	addBlackMove(move);

	// if player is (still) in check after move, move is illegal
	if (isInCheck('B')) {
		UndoMove(move, board);
		return false;
	}
	return true;
}

/*
 Undoes the last move. Called if a player's move results in their own King being in check,
 or by MiniMax to simulate possible games. Can also be called generally to undo a move.
 */
void Chessboard::UndoMove(Move* move, ChessPiece* moveBoard[8][8]) {
	ChessPiece* movedPiece = board[move->x][move->y];
	ChessPiece* capturedPiece = move->capturedPiece;

	if (movedPiece->colour == 'W') {
		if (move->undidCastlingRightsKS) {
			board[whiteKing_posx][whiteKing_posy]->castlingRightsKS = true;
		}
		if (move->undidCastlingRightsQS) {
			board[whiteKing_posx][whiteKing_posy]->castlingRightsQS = true;
		}
	} else {
		if (move->undidCastlingRightsKS) {
			board[blackKing_posx][blackKing_posy]->castlingRightsKS = true;
		}
		if (move->undidCastlingRightsQS) {
			board[blackKing_posx][blackKing_posy]->castlingRightsQS = true;
		}
	}

	if (movedPiece->value == 1) {
		//if pawn's justMovedTwo status was undone it is reset to true
		if (move->undidMovedTwo) {
			movedPiece->justMovedTwo = true;
		}
		if (move->enPassant) {
			if (move->y == move->py + 1) {
				board[move->px][move->py + 1] = move->capturedPiece;
			} else if (move->y == move->py - 1) {
				board[move->px][move->py - 1] = move->capturedPiece;
			}
			board[move->px][move->py] = movedPiece;
			board[move->x][move->y] = NULL;
			if (movedPiece->colour == 'W') {
				materialCountBlack += 1;
				removeWhiteMove(move);
			} else {
				materialCountWhite += 1;
				removeBlackMove(move);
			}
			return;
		}
	}
	if (move->promotion) {
		char colour = movedPiece->colour;
		ChessPiece* temp;
		if (colour == 'W') {
			materialCountWhite -= 8;
			for (int i = 0; i < 8; i++) {
				if (queens[i]->id == 'P' && queens[i]->colour == 'W') {
					temp = queens[i];
					queens[i] = movedPiece;
					movedPiece = temp;
					extraWhiteQueens--;
					break;
				}
			}
		} else {
			materialCountBlack -= 8;
			for (int i = 8; i < 16; i++) {
				if (queens[i]->id == 'P' && queens[i]->colour == 'B') {
					temp = queens[i];
					queens[i] = movedPiece;
					movedPiece = temp;
					extraBlackQueens--;
					break;
				}
			}
		}
	}
	if (movedPiece->value == 100) {
		if (movedPiece->colour == 'W') {
			if (move->castling) {
				UndoCastlingWhite(move);
				return;
			} else { //regular king move
				whiteKing_posx = move->px;
				whiteKing_posy = move->py;
			}
		} else {
			if (move->castling) {
				UndoCastlingBlack(move);
				return;
			} else {
				blackKing_posx = move->px;
				blackKing_posy = move->py;
			}
		}
	}
	board[move->px][move->py] = movedPiece;
	//replace target square with either the captured piece or NULL
	if (capturedPiece != 0) {
		board[move->x][move->y] = capturedPiece;
		if (capturedPiece->colour == 'W') {
			materialCountWhite += capturedPiece->value;
		} else {
			materialCountBlack += capturedPiece->value;
		}
	} else {
		board[move->x][move->y] = NULL;
	}

	if (movedPiece->colour == 'W') {
		removeWhiteMove(move);
	} else {
		removeBlackMove(move);
	}
}

bool Chessboard::isInCheck(char colour) {
	int i = 0;
	int j = 0;
	Move move;
	if (colour == 'W') {
		move.x = whiteKing_posx;
		move.y = whiteKing_posy;
		//up 
		i = whiteKing_posx + 1;
		j = whiteKing_posy;
		move.py = j;
		while (i < 7 && board[i][j] == NULL) {
			i++;
		}
		move.px = i;
		if (i < 8 && board[i][j] != NULL
				&& board[i][j]->CheckMove(&move, board)) {
			return true;
		}
		//up-right
		i = whiteKing_posx + 1;
		j = whiteKing_posy + 1;
		while (i < 7 && j < 7 && board[i][j] == NULL) {
			i++;
			j++;
		}
		move.px = i;
		move.py = j;
		if (i < 8 && j < 8 && board[i][j] != NULL
				&& board[i][j]->CheckMove(&move, board)) {
			return true;
		}
		//right
		i = whiteKing_posx;
		j = whiteKing_posy + 1;
		move.px = i;
		while (j < 7 && board[i][j] == NULL) {
			j++;
		}
		move.py = j;
		if (j < 8 && board[i][j] != NULL
				&& board[i][j]->CheckMove(&move, board)) {
			return true;
		}
		//down-right
		i = whiteKing_posx - 1;
		j = whiteKing_posy + 1;
		while (i > 0 && j < 7 && board[i][j] == NULL) {
			i--;
			j++;
		}
		move.px = i;
		move.py = j;
		if (i >= 0 && j < 8 && board[i][j] != NULL
				&& board[i][j]->CheckMove(&move, board)) {
			return true;
		}
		//down
		i = whiteKing_posx - 1;
		j = whiteKing_posy;
		move.py = j;
		while (i > 0 && board[i][j] == NULL) {
			i--;
		}
		move.px = i;
		if (i >= 0 && board[i][j] != NULL
				&& board[i][j]->CheckMove(&move, board)) {
			return true;
		}
		//down-left
		i = whiteKing_posx - 1;
		j = whiteKing_posy - 1;
		while (i > 0 && j > 0 && board[i][j] == NULL) {
			i--;
			j--;
		}
		move.px = i;
		move.py = j;
		if (i >= 0 && j >= 0 && board[i][j] != NULL
				&& board[i][j]->CheckMove(&move, board)) {
			return true;
		}
		//left
		i = whiteKing_posx;
		j = whiteKing_posy - 1;
		move.px = i;
		while (j > 0 && board[i][j] == NULL) {
			j--;
		}
		move.py = j;
		if (j >= 0 && board[i][j] != NULL
				&& board[i][j]->CheckMove(&move, board)) {
			return true;
		}
		//up-left
		i = whiteKing_posx + 1;
		j = whiteKing_posy - 1;
		while (i < 7 && j > 0 && board[i][j] == NULL) {
			i++;
			j--;
		}
		move.px = i;
		move.py = j;
		if (i < 8 && j >= 0 && board[i][j] != NULL
				&& board[i][j]->CheckMove(&move, board)) {
			return true;
		}
		//knight moves
		i = whiteKing_posx + 1;
		j = whiteKing_posy + 2;
		if (i < 8 && j < 8) {
			move.px = i;
			move.py = j;
			if (board[i][j] != NULL && board[i][j]->CheckMove(&move, board)) {
				return true;
			}
		}
		i = whiteKing_posx + 1;
		j = whiteKing_posy - 2;
		if (i < 8 && j >= 0) {
			move.px = i;
			move.py = j;
			if (board[i][j] != NULL && board[i][j]->CheckMove(&move, board)) {
				return true;
			}
		}
		i = whiteKing_posx + 2;
		j = whiteKing_posy + 1;
		if (i < 8 && j < 8) {
			move.px = i;
			move.py = j;
			if (board[i][j] != NULL && board[i][j]->CheckMove(&move, board)) {
				return true;
			}
		}
		i = whiteKing_posx + 2;
		j = whiteKing_posy - 1;
		if (i < 8 && j >= 0) {
			move.px = i;
			move.py = j;
			if (board[i][j] != NULL && board[i][j]->CheckMove(&move, board)) {
				return true;
			}
		}
		i = whiteKing_posx - 1;
		j = whiteKing_posy + 2;
		if (i >= 0 && j < 8) {
			move.px = i;
			move.py = j;
			if (board[i][j] != NULL && board[i][j]->CheckMove(&move, board)) {
				return true;
			}
		}
		i = whiteKing_posx - 1;
		j = whiteKing_posy - 2;
		if (i >= 0 && j >= 0) {
			move.px = i;
			move.py = j;
			if (board[i][j] != NULL && board[i][j]->CheckMove(&move, board)) {
				return true;
			}
		}
		i = whiteKing_posx - 2;
		j = whiteKing_posy + 1;
		if (i >= 0 && j < 8) {
			move.px = i;
			move.py = j;
			if (board[i][j] != NULL && board[i][j]->CheckMove(&move, board)) {
				return true;
			}
		}
		i = whiteKing_posx - 2;
		j = whiteKing_posy - 1;
		if (i >= 0 && j >= 0) {
			move.px = i;
			move.py = j;
			if (board[i][j] != NULL && board[i][j]->CheckMove(&move, board)) {
				return true;
			}
		}

	} else if (colour == 'B') {
		move.x = blackKing_posx;
		move.y = blackKing_posy;
		//up 
		i = blackKing_posx + 1;
		j = blackKing_posy;
		move.py = j;
		while (i < 7 && board[i][j] == NULL) {
			i++;
		}
		move.px = i;
		if (i < 8 && board[i][j] != NULL
				&& board[i][j]->CheckMove(&move, board)) {
			return true;
		}
		//up-right
		i = blackKing_posx + 1;
		j = blackKing_posy + 1;
		while (i < 7 && j < 7 && board[i][j] == NULL) {
			i++;
			j++;
		}
		move.px = i;
		move.py = j;
		if (i < 8 && j < 8 && board[i][j] != NULL
				&& board[i][j]->CheckMove(&move, board)) {
			return true;
		}
		//right
		i = blackKing_posx;
		j = blackKing_posy + 1;
		move.px = i;
		while (j < 7 && board[i][j] == NULL) {
			j++;
		}
		move.py = j;
		if (j < 8 && board[i][j] != NULL
				&& board[i][j]->CheckMove(&move, board)) {
			return true;
		}
		//down-right
		i = blackKing_posx - 1;
		j = blackKing_posy + 1;
		while (i > 0 && j < 7 && board[i][j] == NULL) {
			i--;
			j++;
		}
		move.px = i;
		move.py = j;
		if (i >= 0 && j < 8 && board[i][j] != NULL
				&& board[i][j]->CheckMove(&move, board)) {
			return true;
		}
		//down
		i = blackKing_posx - 1;
		j = blackKing_posy;
		move.py = j;
		while (i > 0 && board[i][j] == NULL) {
			i--;
		}
		move.px = i;
		if (i >= 0 && board[i][j] != NULL
				&& board[i][j]->CheckMove(&move, board)) {
			return true;
		}
		//down-left
		i = blackKing_posx - 1;
		j = blackKing_posy - 1;
		while (i > 0 && j > 0 && board[i][j] == NULL) {
			i--;
			j--;
		}
		move.px = i;
		move.py = j;
		if (i >= 0 && j >= 0 && board[i][j] != NULL
				&& board[i][j]->CheckMove(&move, board)) {
			return true;
		}
		//left
		i = blackKing_posx;
		j = blackKing_posy - 1;
		move.px = i;
		while (j > 0 && board[i][j] == NULL) {
			j--;
		}
		move.py = j;
		if (j >= 0 && board[i][j] != NULL
				&& board[i][j]->CheckMove(&move, board)) {
			return true;
		}
		//up-left
		i = blackKing_posx + 1;
		j = blackKing_posy - 1;
		while (i < 7 && j > 0 && board[i][j] == NULL) {
			i++;
			j--;
		}
		move.px = i;
		move.py = j;
		if (i < 8 && j >= 0 && board[i][j] != NULL
				&& board[i][j]->CheckMove(&move, board)) {
			return true;
		}
		//knight moves
		i = blackKing_posx + 1;
		j = blackKing_posy + 2;
		if (i < 8 && j < 8) {
			move.px = i;
			move.py = j;
			if (board[i][j] != NULL && board[i][j]->CheckMove(&move, board)) {
				return true;
			}
		}
		i = blackKing_posx + 1;
		j = blackKing_posy - 2;
		if (i < 8 && j >= 0) {
			move.px = i;
			move.py = j;
			if (board[i][j] != NULL && board[i][j]->CheckMove(&move, board)) {
				return true;
			}
		}
		i = blackKing_posx + 2;
		j = blackKing_posy + 1;
		if (i < 8 && j < 8) {
			move.px = i;
			move.py = j;
			if (board[i][j] != NULL && board[i][j]->CheckMove(&move, board)) {
				return true;
			}
		}
		i = blackKing_posx + 2;
		j = blackKing_posy - 1;
		if (i < 8 && j >= 0) {
			move.px = i;
			move.py = j;
			if (board[i][j] != NULL && board[i][j]->CheckMove(&move, board)) {
				return true;
			}
		}
		i = blackKing_posx - 1;
		j = blackKing_posy + 2;
		if (i >= 0 && j < 8) {
			move.px = i;
			move.py = j;
			if (board[i][j] != NULL && board[i][j]->CheckMove(&move, board)) {
				return true;
			}
		}
		i = blackKing_posx - 1;
		j = blackKing_posy - 2;
		if (i >= 0 && j >= 0) {
			move.px = i;
			move.py = j;
			if (board[i][j] != NULL && board[i][j]->CheckMove(&move, board)) {
				return true;
			}
		}
		i = blackKing_posx - 2;
		j = blackKing_posy + 1;
		if (i >= 0 && j < 8) {
			move.px = i;
			move.py = j;
			if (board[i][j] != NULL && board[i][j]->CheckMove(&move, board)) {
				return true;
			}
		}
		i = blackKing_posx - 2;
		j = blackKing_posy - 1;
		if (i >= 0 && j >= 0) {
			move.px = i;
			move.py = j;
			if (board[i][j] != NULL && board[i][j]->CheckMove(&move, board)) {
				return true;
			}
		}
	}
	return false;
}

bool Chessboard::isDrawByRepetition() {
	if (moves < 12) {
		return false;
	}
	bool result = false;
	if (moveListWhite->x == moveListWhite->previous->px
			&& moveListWhite->previous->px
					== moveListWhite->previous->previous->x
			&& moveListWhite->previous->previous->x
					== moveListWhite->previous->previous->previous->px
			&& moveListWhite->previous->previous->previous->px
					== moveListWhite->previous->previous->previous->previous->x
			&& moveListWhite->previous->previous->previous->previous->x
					== moveListWhite->previous->previous->previous->previous->previous->px
			&& moveListWhite->y == moveListWhite->previous->py
			&& moveListWhite->previous->py
					== moveListWhite->previous->previous->y
			&& moveListWhite->previous->previous->y
					== moveListWhite->previous->previous->previous->py
			&& moveListWhite->previous->previous->previous->py
					== moveListWhite->previous->previous->previous->previous->y
			&& moveListWhite->previous->previous->previous->previous->y
					== moveListWhite->previous->previous->previous->previous->previous->py) {
		result = true;
	}
	if (moveListBlack->x == moveListBlack->previous->px
			&& moveListBlack->previous->px
					== moveListBlack->previous->previous->x
			&& moveListBlack->previous->previous->x
					== moveListBlack->previous->previous->previous->px
			&& moveListBlack->previous->previous->previous->px
					== moveListBlack->previous->previous->previous->previous->x
			&& moveListBlack->previous->previous->previous->previous->x
					== moveListBlack->previous->previous->previous->previous->previous->px
			&& moveListBlack->y == moveListBlack->previous->py
			&& moveListBlack->previous->py
					== moveListBlack->previous->previous->y
			&& moveListBlack->previous->previous->y
					== moveListBlack->previous->previous->previous->py
			&& moveListBlack->previous->previous->previous->py
					== moveListBlack->previous->previous->previous->previous->y
			&& moveListBlack->previous->previous->previous->previous->y
					== moveListBlack->previous->previous->previous->previous->previous->py
			&& result == true) {
		return true;
	}
	return false;
}

bool Chessboard::hasWhiteWon() {
	Move* move = new Move();
	//check every square for every possible move
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			for (int a = 0; a < 8; a++) {
				for (int b = 0; b < 8; b++) {
					move->x = a;
					move->y = b;
					move->px = i;
					move->py = j;
					ChessPiece* temp = board[i][j];
					//if black can still play a legal move, game is not over
					if (temp != NULL && temp->colour == 'B') {
						if (PlayBlackMove(move)) {
							std::cout << move->px << move->py << move->x << move->y;
							//reset board to before the hypothetical move was played
							UndoMove(move, board);
							delete move;
							return false;
						}
					}
					move->Reset();
				}
			}
		}
	}
	delete move;
	return true;
}

bool Chessboard::hasBlackWon() {
	Move* move = new Move();
	//check every square for every possible move
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			for (int a = 0; a < 8; a++) {
				for (int b = 0; b < 8; b++) {
					move->x = a;
					move->y = b;
					move->px = i;
					move->py = j;
					ChessPiece* temp = board[i][j];
					//if black can still play a legal move, game is not over
					if (temp != NULL && temp->colour == 'W') {
						if (PlayWhiteMove(move)) {
							//reset board to before the hypothetical move was played
							UndoMove(move, board);
							delete move;
							return false;
						}
					}
					move->Reset();
				}
			}

		}
	}
	delete move;
	return true;
}

/*
 Checks if castling rights are correctly used and adjusts king position.
 Handles rest of the move 'admin' as well.
 */
bool Chessboard::CheckKingMoveBlack(Move* move, ChessPiece* movedPiece,
		ChessPiece* capturedPiece) {
	move->capturedPiece = capturedPiece;
	//castling KS
	if (move->py == 4 && move->y == 6) {
		if (isInCheck('B')) {
			return false;
		} //can't castle out of check
		if (!movedPiece->castlingRightsKS) {
			return false;
		}
		//move king one step at a time to see if he would pass through check
		board[7][5] = movedPiece;
		board[7][4] = NULL;
		blackKing_posx = 7; //update kingpos to get accurate isInCheck result
		blackKing_posy = 5;
		if (isInCheck('B')) {
			blackKing_posx = move->px; //undo this move manually
			blackKing_posy = move->py;
			board[7][4] = movedPiece;
			board[7][5] = NULL;
			return false;
		}
		board[7][5] = board[7][7];
		board[7][6] = movedPiece;
		board[7][7] = NULL;
		hasBlackCastled = true;
	}
	//castling QS
	else if (move->py == 4 && move->y == 2) {
		if (isInCheck('B')) {
			return false;
		} //can't castle out of check
		if (!movedPiece->castlingRightsQS) {
			return false;
		}
		//move king one step at a time to see if he would pass through check
		board[7][3] = movedPiece;
		board[7][4] = NULL;
		blackKing_posx = 7;
		blackKing_posy = 3;
		if (isInCheck('B')) {
			blackKing_posx = move->px;
			blackKing_posy = move->py;
			board[7][3] = NULL;
			board[7][4] = movedPiece;
			return false;
		}
		board[7][3] = board[7][0];
		board[7][2] = movedPiece;
		board[7][0] = NULL;
		hasBlackCastled = true;
	} else {
		// if regular move
		board[move->x][move->y] = movedPiece;
		board[move->px][move->py] = NULL;
		if (capturedPiece != NULL)
			materialCountWhite -= capturedPiece->value;
	}
	blackKing_posx = move->x;
	blackKing_posy = move->y;

	addBlackMove(move); //necessary, since UndoMove calls removeMove

	move->undidCastlingRightsKS = true;
	move->undidCastlingRightsQS = true;
	movedPiece->castlingRightsQS = false;
	movedPiece->castlingRightsKS = false;

	if (isInCheck('B')) {
		UndoMove(move, board);
		return false;
	}
	return true;
}

bool Chessboard::CheckKingMoveWhite(Move* move, ChessPiece* movedPiece,
		ChessPiece* capturedPiece) {
	move->capturedPiece = capturedPiece;
	if (move->py == 4 && move->y == 6) {
		if (isInCheck('W')) {
			return false;
		} //can't castle out of check
		if (!movedPiece->castlingRightsKS) {
			return false;
		}
		//move king one step at a time to see if he would pass through check
		board[0][5] = movedPiece;
		board[0][4] = NULL;
		//update king position for isInCheck
		whiteKing_posx = 0;
		whiteKing_posy = 5;
		if (isInCheck('W')) { //if king passes through check, manually undo move and reset king positions
			board[0][5] = NULL;
			board[0][4] = movedPiece;
			whiteKing_posx = move->px;
			whiteKing_posy = move->py;
			return false;
		}
		board[0][5] = board[0][7];
		board[0][6] = movedPiece;
		board[0][7] = NULL;
		hasWhiteCastled = true;
	} else if (move->py == 4 && move->y == 2) {
		if (isInCheck('W'))
			return false; //can't castle out of check
		if (!movedPiece->castlingRightsQS) {
			return false;
		}
		//move king one step at a time to see if he would pass through check
		board[0][3] = movedPiece;
		board[0][4] = NULL;
		whiteKing_posx = 0;
		whiteKing_posy = 3;
		if (isInCheck('W')) {
			//undo this last move if king passes through check
			board[0][3] = NULL;
			board[0][4] = movedPiece;
			whiteKing_posx = move->px;
			whiteKing_posy = move->py;
			return false;
		}
		board[0][3] = board[0][0];
		board[0][2] = movedPiece;
		board[0][0] = NULL;
		hasWhiteCastled = true;
	} else {
		// if regular move
		board[move->x][move->y] = movedPiece;
		board[move->px][move->py] = NULL;
		if (capturedPiece != NULL)
			materialCountBlack -= capturedPiece->value;
	}

	whiteKing_posx = move->x;
	whiteKing_posy = move->y;

	addWhiteMove(move); //necessary, since UndoCastling calls removeMove

	move->undidCastlingRightsKS = true;
	move->undidCastlingRightsQS = true;
	movedPiece->castlingRightsKS = false;
	movedPiece->castlingRightsQS = false;

	if (isInCheck('W')) {
		UndoMove(move, board);
		return false;
	}
	return true;
}

void Chessboard::UndoCastlingBlack(Move* move) {
	if (move->y == 2) {
		board[blackKing_posx][blackKing_posy]->castlingRightsQS = true;
		board[7][0] = board[7][3];
		board[7][4] = board[7][2];
		board[7][3] = NULL;
		board[7][2] = NULL;
	} else if (move->y == 6) {
		board[blackKing_posx][blackKing_posy]->castlingRightsKS = true;
		board[7][7] = board[7][5];
		board[7][4] = board[7][6];
		board[7][5] = NULL;
		board[7][6] = NULL;
	}
	hasBlackCastled = false;

	blackKing_posx = move->px;
	blackKing_posy = move->py;

	removeBlackMove(move);
}

void Chessboard::UndoCastlingWhite(Move* move) {

	if (move->y == 2) {
		board[whiteKing_posx][whiteKing_posy]->castlingRightsQS = true;
		board[0][0] = board[0][3];
		board[0][4] = board[0][2];
		board[0][3] = NULL;
		board[0][2] = NULL;
	} else if (move->y == 6) {
		board[whiteKing_posx][whiteKing_posy]->castlingRightsKS = true;
		board[0][7] = board[0][5];
		board[0][4] = board[0][6];
		board[0][5] = NULL;
		board[0][6] = NULL;
	}
	hasWhiteCastled = false;

	whiteKing_posx = move->px;
	whiteKing_posy = move->py;

	removeWhiteMove(move);
}

void Chessboard::AlternateTurn() {

}

int Chessboard::CalculateScoreBlack() {
	return 0;
}

int Chessboard::CalculateScoreWhite() {
	return 0;
}

void Chessboard::PrintBoard() {
	for (int i = 7; i >= 0; i--) {
		for (int j = 0; j < 8; j++) {
			if (board[i][j] != NULL) {
				board[i][j]->PrintPiece();
			} else {
				std::cout << " ";
			}
			std::cout << " | ";
		}
		std::cout << "\n--------------------------------" << std::endl;
	}
	std::cout << "\n" << std::endl;
}

void Chessboard::PrintMoveList() {
	Move* currentWhite = moveListWhite;
	Move* currentBlack = moveListBlack;

	while (currentWhite != NULL && currentBlack != NULL) {
		std::cout << "White: " << (char) currentWhite->px + 97
				<< (char) currentWhite->py + 49 << "-"
				<< (char) currentWhite->x + 97 << (char) currentWhite->y + 49
				<< std::endl;
		std::cout << "Black: " << (char) currentBlack->px + 97
				<< (char) currentBlack->py + 49 << "-"
				<< (char) currentBlack->x + 97 << (char) currentBlack->y + 49
				<< std::endl;
		currentWhite = currentWhite->previous;
		currentBlack = currentBlack->previous;
	}
}

/*
 Adds the most recent move to the end of the list of black moves.
 */
void Chessboard::addBlackMove(Move* move) {
	/*move->previous = moveListBlack;
	 moveListBlack->next = move;
	 moveListBlack = move;
	 moves++;
	 */
	Move* newMove = new Move();
	*newMove = *move;
	newMove->previous = moveListBlack;
	moveListBlack->next = newMove;
	moveListBlack = newMove;
	moves++;
}

/*
 Adds the most recent move to the end of the list of white moves.
 */
void Chessboard::addWhiteMove(Move* move) {
	/*move->previous = moveListWhite;
	 moveListWhite->next = move;
	 moveListWhite = move;
	 moves++;
	 */
	Move* newMove = new Move();
	*newMove = *move;
	newMove->previous = moveListWhite;
	moveListWhite->next = newMove;
	moveListWhite = newMove;
	moves++;
}

void Chessboard::removeBlackMove(Move* move) {
	moveListBlack = moveListBlack->previous;
	delete moveListBlack->next;
	//moveListBlack->next = NULL;
	moves--;
}

void Chessboard::removeWhiteMove(Move* move) {
	moveListWhite = moveListWhite->previous;
	delete moveListWhite->next;
	//moveListWhite->next = NULL;
	moves--;

}

int Chessboard::CalculateScore(ChessPiece* board[8][8]) {
	//return materialCountWhite - materialCountBlack;
	int kingSafety = CalculateKingSafety(board);
	//std::cout << "kingSafety: " << kingSafety << std::endl;
	//int pawnIslands = CalculatePawnIslands(board);
	//std::cout << "pawn islands: " << pawnIslands << std::endl;
	//int centerControl = CalculateCenterControl(board);
	//std::cout << "central control: " << centerControl << std::endl;
	int materialCount = materialCountWhite - materialCountBlack;
	//std::cout << "material count: " << materialCount << std::endl;
	int development = CalculateDevelopment(board);
	//std::cout << "development: " << development << std::endl;
	int evaluation = (kingSafety) + (development) + (100 * materialCount);
	return evaluation;
}

/*
 Count the amount of a player's pieces directly surrounding their King.
 */
int Chessboard::CalculateKingSafety(ChessPiece* board[8][8]) {
	int blackKingSafety = 0;
	int whiteKingSafety = 0;
	if (hasBlackCastled) {
		blackKingSafety += 5;
	}
	if (hasWhiteCastled) {
		whiteKingSafety += 5;
	}
	if (whiteKing_posx == 0&& (whiteKing_posy == 4 || whiteKing_posy == 6)
	&& board[1][5] == NULL) {
		whiteKingSafety -= 3;
	}
	if (blackKing_posx == 7&& (blackKing_posy == 4 || blackKing_posy == 6)
	&& board[6][5] == NULL) {
		blackKingSafety -= 3;
	}
	if (moves >= 20) {
		if (blackKing_posx - 1 >= 0) {
			if (board[blackKing_posx - 1][blackKing_posy] != NULL
					&& board[blackKing_posx][blackKing_posy]->colour == 'B') {
				blackKingSafety++;
			}
			if (blackKing_posy - 1 >= 0) {
				if (board[blackKing_posx - 1][blackKing_posy - 1] != NULL
						&& board[blackKing_posx][blackKing_posy]->colour
								== 'B') {
					blackKingSafety++;
				}
				if (board[blackKing_posx][blackKing_posy - 1] != NULL
						&& board[blackKing_posx][blackKing_posy]->colour
								== 'B') {
					blackKingSafety++;
				}
			}
			if (blackKing_posy + 1 < 8) {
				if (board[blackKing_posx - 1][blackKing_posy + 1] != NULL
						&& board[blackKing_posx][blackKing_posy]->colour
								== 'B') {
					blackKingSafety++;
				}
				if (board[blackKing_posx][blackKing_posy - 1] != NULL
						&& board[blackKing_posx][blackKing_posy]->colour
								== 'B') {
					blackKingSafety++;
				}
			}
		}

		if (whiteKing_posx + 1 < 8) {
			if (board[whiteKing_posx + 1][whiteKing_posy] != NULL
					&& board[whiteKing_posx][whiteKing_posy]->colour == 'W') {
				whiteKingSafety++;
			}
			if (whiteKing_posy - 1 >= 0) {
				if (board[whiteKing_posx + 1][whiteKing_posy - 1] != NULL
						&& board[whiteKing_posx][whiteKing_posy]->colour
								== 'W') {
					whiteKingSafety++;
				}
				if (board[whiteKing_posx][whiteKing_posy - 1] != NULL
						&& board[whiteKing_posx][whiteKing_posy]->colour
								== 'W') {
					whiteKingSafety++;
				}
			}
			if (whiteKing_posy + 1 < 8) {
				if (board[whiteKing_posx + 1][whiteKing_posy + 1] != NULL
						&& board[whiteKing_posx][whiteKing_posy]->colour
								== 'W') {
					whiteKingSafety++;
				}
				if (board[whiteKing_posx][whiteKing_posy - 1] != NULL
						&& board[whiteKing_posx][whiteKing_posy]->colour
								== 'W') {
					whiteKingSafety++;
				}
			}
		}
	}
	return whiteKingSafety - blackKingSafety;

}

/*
 Checks how many pieces of each colour have potential access to the four central squares, d4, d5, e4, e5.
 */
int Chessboard::CalculateCenterControl(ChessPiece* board[8][8]) {
	int blackCenterControl = 0;
	int whiteCenterControl = 0;
	if (board[3][3] != NULL && board[3][3]->value == 1) {
		if (board[3][3]->colour == 'W') {
			whiteCenterControl += 5;
		} else {
			blackCenterControl += 5;
		}
	}
	if (board[3][4] != NULL && board[3][4]->value == 1) {
		if (board[3][4]->colour == 'W') {
			whiteCenterControl += 5;
		} else {
			blackCenterControl += 5;
		}
	}
	if (board[4][3] != NULL && board[4][3]->value == 1) {
		if (board[4][3]->colour == 'W') {
			whiteCenterControl += 5;
		} else {
			blackCenterControl += 5;
		}
	}
	if (board[4][4] != NULL && board[4][4]->value == 1) {
		if (board[4][4]->colour == 'W') {
			whiteCenterControl += 5;
		} else {
			blackCenterControl += 5;
		}
	}
	/*
	 Move move;
	 for (int i = 7; i > 2; i--) {
	 for (int j = 0; j < 8; j++) {
	 if (board[i][j] != NULL && board[i][j]->value == 9)
	 continue;
	 move.px = i;
	 move.py = j;
	 move.x = 4;
	 move.y = 3;
	 if (board[i][j] != NULL && board[i][j]->colour == 'B'
	 && board[i][j]->CheckMove(&move, board))
	 blackCenterControl++;
	 move.y = 4;
	 if (board[i][j] != NULL && board[i][j]->colour == 'B'
	 && board[i][j]->CheckMove(&move, board))
	 blackCenterControl++;
	 move.x = 3;
	 if (board[i][j] != NULL && board[i][j]->colour == 'B'
	 && board[i][j]->CheckMove(&move, board))
	 blackCenterControl++;
	 move.y = 3;
	 if (board[i][j] != NULL && board[i][j]->colour == 'B'
	 && board[i][j]->CheckMove(&move, board))
	 blackCenterControl++;
	 }
	 }

	 for (int i = 0; i < 5; i++) {
	 for (int j = 0; j < 8; j++) {
	 if (board[i][j] != NULL && board[i][j]->value == 9)
	 continue;
	 move.px = i;
	 move.py = j;
	 move.x = 4;
	 move.y = 3;
	 if (board[i][j] != NULL && board[i][j]->colour == 'W'
	 && board[i][j]->CheckMove(&move, board))
	 whiteCenterControl++;
	 move.y = 4;
	 if (board[i][j] != NULL && board[i][j]->colour == 'W'
	 && board[i][j]->CheckMove(&move, board))
	 whiteCenterControl++;
	 move.x = 3;
	 if (board[i][j] != NULL && board[i][j]->colour == 'W'
	 && board[i][j]->CheckMove(&move, board))
	 whiteCenterControl++;
	 move.y = 3;
	 if (board[i][j] != NULL && board[i][j]->colour == 'W'
	 && board[i][j]->CheckMove(&move, board))
	 whiteCenterControl++;
	 }
	 }
	 */
	return whiteCenterControl - blackCenterControl;
}

int Chessboard::CalculatePawnIslands(ChessPiece* board[8][8]) {
	int pawnsOnFile = 0;
	int pawnIslandsWhite = 0;
	for (int j = 0; j < 8; j++) {
		pawnsOnFile = 0;
		for (int i = 0; i < 8; i++) {
			if (board[i][j] != NULL && board[i][j]->colour == 'W'
					&& board[i][j]->value == 1) {
				pawnsOnFile++;
				break;
			}
		}
		if (pawnsOnFile == 0)
			pawnIslandsWhite++;
	}
	int pawnIslandsBlack = 0;
	for (int j = 0; j < 8; j++) {
		pawnsOnFile = 0;
		for (int i = 0; i < 8; i++) {
			if (board[i][j] != NULL && board[i][j]->colour == 'B'
					&& board[i][j]->value == 1) {
				pawnsOnFile++;
				break;
			}
		}
		if (pawnsOnFile == 0)
			pawnIslandsBlack++;
	}

	return pawnIslandsWhite - pawnIslandsBlack;
}

int Chessboard::CalculateDevelopment(ChessPiece* board[8][8]) {
	int whiteDevelopment = 0;
	int blackDevelopment = 0;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (board[i][j] != NULL) {
				if (board[i][j]->colour == 'W') {
					if (board[i][j]->value == 1) {
						whiteDevelopment += pawnEval[i][j];
					} else if (board[i][j]->id == 'B') {
						whiteDevelopment += bishopEval[i][j];
					} else if (board[i][j]->id == 'K') {
						whiteDevelopment += knightEval[i][j];
					} else if (board[i][j]->value == 5) {
						whiteDevelopment += rookEval[i][j];
					}
				} else {
					if (board[i][j]->value == 1) {
						blackDevelopment += pawnEval[7 - i][j];
					} else if (board[i][j]->id == 'B') {
						blackDevelopment += bishopEval[7 - i][j];
					} else if (board[i][j]->id == 'K') {
						blackDevelopment += knightEval[7 - i][j];
					} else if (board[i][j]->value == 5) {
						blackDevelopment += rookEval[7 - i][j];
					}
				}
			}
		}
	}
	return whiteDevelopment - blackDevelopment;
}

void Chessboard::InsertBestMove(Move* bestMoves[5], Move* move, int *bestScores,
		int score, bool isMaximising) {
	for (int i = 0; i < 5; i++) {
		if (bestMoves[i]->px == move->px && bestMoves[i]->py == move->py
				&& bestMoves[i]->x == move->x && bestMoves[i]->y == move->y) {
			bestMoves[i]->Reset();
			bestMoves[i]->px = -1;
			if (isMaximising) {
				bestScores[i] = -9999;
			} else {
				bestScores[i] = 9999;
			}
			int j = i + 1;
			while (j < 5) {
				int temp = bestScores[j - 1];
				bestScores[j - 1] = bestScores[j];
				bestScores[j] = temp;
				Move* t = bestMoves[j - 1];
				bestMoves[j - 1] = bestMoves[j];
				bestMoves[j] = t;
				j++;
			}
		}
	}
	if (isMaximising) {
		if (score > bestScores[4]) {
			bestScores[4] = score;
			*bestMoves[4] = *move;
			int j = 3;
			while (j >= 0 && score > bestScores[j]) {
				bestScores[j + 1] = bestScores[j];
				bestScores[j] = score;
				*bestMoves[j + 1] = *bestMoves[j];
				*bestMoves[j] = *move;
				j--;
			}
		}
	} else {
		if (score < bestScores[4]) {
			bestScores[4] = score;
			*bestMoves[4] = *move;
			int j = 3;
			while (j >= 0 && score < bestScores[j]) {
				bestScores[j + 1] = bestScores[j];
				bestScores[j] = score;
				*bestMoves[j + 1] = *bestMoves[j];
				*bestMoves[j] = *move;
				j--;
			}
		}
	}
}

Move* Chessboard::GetNextMinimaxMove_MoveList(char colour) {
	Move* bestMove = new Move();
	Move* actualMove = new Move();
	Move* move;
	int bestScore;
	int bestScores[5];
	Move* bestMoves[5];
	for (int i = 0; i < 6; i++) {
		bestMoves[i] = new Move(-1, -1, -1, -1);
	}
	for (int maxDepth = 0; maxDepth < 4; maxDepth++) {
		//check every square for every possible move
		if (colour == 'W') {
			bestScore = -9999;
			for (int i = 0; i < 5; i++) {
				bestScores[i] = -9999;
			}
			for (int i = 0; i < 8; i++) {
				for (int j = 0; j < 8; j++) {
					if (board[i][j] != NULL && board[i][j]->colour == 'W') {
						if (i != board[i][j]->px || j != board[i][j]->py) { //means the position of the piece has changed permanently
							board[i][j]->DeleteMoveList();
						}
						move = board[i][j]->GetMoveList(i, j, board);
						if (move == NULL)
							throw "potentialMoveList should never be null!";
						move = move->next;
						while (move != NULL) {
							*actualMove = *move;
							if (PlayWhiteMove(actualMove)) {
								//play every legal move and calls Minimax on resulting board
								int score = Minimax_MoveList(board, false, 0,
										maxDepth, actualMove, -9999, 9999, 0);
								//reset board to before the hypothetical move was played
								UndoMove(actualMove, board);
								//update score and bestMove if score from resulting game is better
								if (score > bestScore) {
									bestScore = score;
									*bestMove = *move;
									InsertBestMove(bestMoves, actualMove,
											bestScores, score, true);
								} else if (score > bestScores[4]) {
									InsertBestMove(bestMoves, actualMove,
											bestScores, score, true);
								}
							}
							move = move->next;
							actualMove->Reset();
						}
					}
				}
			}
			for (int i = 0; i < 5; i++) {
				if (maxDepth > 0 && bestMoves[i]->px != -1) {
					*actualMove = *bestMoves[i];
					if (PlayWhiteMove(actualMove)) {
						int score = Minimax_MoveList(board, false, 0,
								maxDepth + 2, actualMove, -9999, 9999, 0);
						UndoMove(actualMove, board);
						if (score > bestScores[0]) {
							InsertBestMove(bestMoves, actualMove, bestScores,
									score, true);
						}
					}
					actualMove->Reset();
				}
			}
		}

		if (colour == 'B') {
			bestScore = 9999;
			for (int i = 0; i < 5; i++) {
				bestScores[i] = 9999;
			}
			for (int i = 0; i < 8; i++) {
				for (int j = 0; j < 8; j++) {
					if (board[i][j] != NULL && board[i][j]->colour == 'B') {
						if (i != board[i][j]->px || j != board[i][j]->py) { //means the position of the piece has changed permanently
							board[i][j]->DeleteMoveList();
						}
						move = board[i][j]->GetMoveList(i, j, board);
						if (move == NULL)
							continue;
						move = move->next;
						while (move != NULL) {
							*actualMove = *move;
							if (PlayBlackMove(actualMove)) {
								//play every legal move and calls Minimax on resulting board
								int score = Minimax_MoveList(board, true, 0,
										maxDepth, actualMove, -9999, 9999, 0);

								//reset board to before the hypothetical move was played
								UndoMove(actualMove, board);
								//update score and bestMove if score from resulting game is better
								if (score < bestScore) {
									bestScore = score;
									*bestMove = *actualMove;
									InsertBestMove(bestMoves, actualMove,
											bestScores, score, false);
								} else if (score < bestScores[4]) {
									InsertBestMove(bestMoves, actualMove,
											bestScores, score, false);
								}
							}
							move = move->next;
							actualMove->Reset();
						}
					}
				}
			}
			for (int i = 0; i < 5; i++) {
				if (maxDepth > 0 && bestMoves[i]->px != -1) {
					*actualMove = *bestMoves[i];
					if (PlayWhiteMove(actualMove)) {
						int score = Minimax_MoveList(board, false, 0,
								maxDepth + 2, actualMove, -9999, 9999, 0);
						UndoMove(actualMove, board);
						if (score < bestScores[0]) {
							InsertBestMove(bestMoves, actualMove, bestScores,
									score, false);
						}
					}
					actualMove->Reset();
				}
			}
		}
		for (int i = 0; i < 5; i++) {
			std::cout << (char) (bestMoves[i]->py + 97)
					<< (char) (bestMoves[i]->px + 49)
					<< (char) (bestMoves[i]->y + 97)
					<< (char) (bestMoves[i]->x + 49) << ": " << bestScores[i]
					<< std::endl;
		}
		std::cout << std::endl;
	}
	*bestMove = *bestMoves[0];
	for (int i = 0; i < 5; i++) {
		delete bestMoves[i];
	}
	delete actualMove;
	return bestMove;
}

int Chessboard::Minimax_MoveList(ChessPiece* board[8][8], bool isMaximising,
		int depth, int maxDepth, Move* lastMove, int alpha, int beta,
		unsigned long long pz) {
	if (depth >= maxDepth && lastMove->capturedPiece == NULL) {
		return CalculateScore(board);
	}
	if (depth >= 8) {
		return CalculateScore(board);
	}
	int currentScore = CalculateScore(board);
	if (isMaximising && currentScore + 1000 < alpha) {
		return currentScore;
	} else if (!isMaximising && currentScore - 1000 > beta) {
		return currentScore;
	}
	bool alphaBetaBreak = false;
	unsigned long long z;
	int bestScore;
	int score = 0;
	Move* move;
	Move* next;
	Move* headPrevious;
	Move* actualMove = new Move();
	int pieces = 0;
	int movesCount = 0;
	bool reset = false;
	int pxPiece = -1;
	int pyPiece = -1;
	if (isMaximising) {
		bestScore = -9999;
		alpha = -9999;
		for (int i = 0; i < 2; i++) {
			if (board[killerMoves[(depth * 2) + i]->px][killerMoves[(depth * 2)
					+ i]->py] != NULL
					&& board[killerMoves[(depth * 2) + i]->px][killerMoves[(depth
							* 2) + i]->py]->colour == 'B') {
				actualMove->Reset();
				actualMove->px = killerMoves[(depth * 2) + i]->px;
				actualMove->py = killerMoves[(depth * 2) + i]->py;
				actualMove->x = killerMoves[(depth * 2) + i]->x;
				actualMove->y = killerMoves[(depth * 2) + i]->y;
				if (PlayWhiteMove(actualMove)) {
					if (actualMove->capturedPiece != 0
							&& actualMove->capturedPiece->value == 100) {
						UndoMove(actualMove, board);
						delete actualMove;
						return -500;
					}
					if (isDrawByRepetition()) {
						UndoMove(actualMove, board);
						delete actualMove;
						return 50;
					}
					if (pz == 0) {
						z = tt->generateZ(board, false);
					} else {
						z = tt->updateZ(board, actualMove, pz);
					}
					struct Entry* entry = tt->getEntry(z);
					if (entry != NULL && entry->depth >= maxDepth - depth) {
						score = entry->score;
					} else {
						score = Minimax_MoveList(board, false, depth + 1,
								maxDepth, actualMove, alpha, beta, z);
						entry = new Entry();
						entry->score = score;
						entry->z = z;
						entry->depth = maxDepth - depth;
						tt->insertEntry(entry, z);
					}
					if (score > bestScore) {
						bestScore = score;
					}
					if (score > alpha) {
						alpha = score;
					}
					UndoMove(actualMove, board);
					movesCount++;
					if (alpha >= beta) {
						alphaBetaBreak = true;
						break;
					}
				}
			}
		}
		for (int i = 0; i < 8; i++) {
			if (alphaBetaBreak) {
				break;
			}
			for (int j = 0; j < 8; j++) {
				if (alphaBetaBreak) {
					break;
				}
				if (board[i][j] != NULL && board[i][j]->colour == 'W') {
					pieces++;
					headPrevious = board[i][j]->potentialMoveList->next;
					if (board[i][j]->px != i || board[i][j]->py != j) {
						reset = true;
						pxPiece = board[i][j]->px;
						pyPiece = board[i][j]->py;
					}
					move = board[i][j]->GetMoveList(i, j, board);
					if (move == NULL)
						continue;
					move = move->next;
					while (move != NULL) {
						movesCount++;
						actualMove->Reset();
						*actualMove = *move;
						if (PlayWhiteMove(actualMove)) {
							if (actualMove->capturedPiece != 0
									&& actualMove->capturedPiece->value
											== 100) {
								UndoMove(actualMove, board);
								delete actualMove;
								return -500;
							}
							if (isDrawByRepetition()) {
								UndoMove(actualMove, board);
								delete actualMove;
								return 50;
							}
							if (pz == 0) {
								z = tt->generateZ(board, false);
							} else {
								z = tt->updateZ(board, actualMove, pz);
							}
							struct Entry* entry = tt->getEntry(z);
							if (entry != NULL
									&& entry->depth >= maxDepth - depth) {
								score = entry->score;
							} else {
								score = Minimax_MoveList(board, false,
										depth + 1, maxDepth, actualMove, alpha,
										beta, z);
								entry = new Entry();
								entry->score = score;
								entry->z = z;
								entry->depth = maxDepth - depth;
								tt->insertEntry(entry, z);
							}
							if (score > bestScore) {
								bestScore = score;
							}
							if (score > alpha) {
								alpha = score;
							}
							UndoMove(actualMove, board);
							movesCount++;
							if (alpha >= beta) {
								*killerMoves[depth] = *killerMoves[(depth * 2)
										+ 1];
								*killerMoves[(depth * 2) + 1] = *actualMove;
								alphaBetaBreak = true;
								break;
							}
						}
						move = move->next;
					}
					if (reset) {
						reset = false;
						move = board[i][j]->potentialMoveList->next;
						while (move != NULL) { //delete newly created moveList
							next = move->next;
							delete move;
							move = next;
						}
						board[i][j]->potentialMoveList->next = headPrevious; //reset moves to previous state after undoing simulated move
						board[i][j]->px = pxPiece;
						board[i][j]->py = pyPiece;
					}
				}
			}
		}
		if (bestScore == -9999) {
			delete actualMove;
			if (!isInCheck('W')) {
				return 0;
			}
			return -500;
		}
		delete actualMove;
		return bestScore;
	}
	if (!isMaximising) {
		bestScore = 9999;
		beta = 9999;
		for (int i = 0; i < 2; i++) {
			if (board[killerMoves[(depth * 2) + i]->px][killerMoves[(depth * 2)
					+ i]->py] != NULL
					&& board[killerMoves[(depth * 2) + i]->px][killerMoves[(depth
							* 2) + i]->py]->colour == 'B') {
				actualMove->Reset();
				actualMove->px = killerMoves[(depth * 2) + i]->px;
				actualMove->py = killerMoves[(depth * 2) + i]->py;
				actualMove->x = killerMoves[(depth * 2) + i]->x;
				actualMove->y = killerMoves[(depth * 2) + i]->y;
				if (PlayBlackMove(actualMove)) {
					if (actualMove->capturedPiece != 0
							&& actualMove->capturedPiece->value == 100) {
						UndoMove(actualMove, board);
						delete actualMove;
						return 500;
					}
					if (isDrawByRepetition()) {
						UndoMove(actualMove, board);
						delete actualMove;
						return -50;
					}
					if (pz == 0) {
						z = tt->generateZ(board, true);
					} else {
						z = tt->updateZ(board, actualMove, pz);
					}
					struct Entry* entry = tt->getEntry(z);
					if (entry != NULL && entry->depth >= maxDepth - depth) {
						score = entry->score;
					} else {
						score = Minimax_MoveList(board, true, depth + 1,
								maxDepth, actualMove, alpha, beta, z);
						entry = new Entry();
						entry->score = score;
						entry->z = z;
						entry->depth = maxDepth - depth;
						tt->insertEntry(entry, z);
					}
					if (score < bestScore) {
						bestScore = score;
					}
					if (score < beta) {
						beta = score;
					}
					movesCount++;
					UndoMove(actualMove, board);
					if (alpha >= beta) {
						alphaBetaBreak = true;
						break;
					}
				}
			}
		}
		for (int i = 0; i < 8; i++) {
			if (alphaBetaBreak) {
				break;
			}
			for (int j = 0; j < 8; j++) {
				if (alphaBetaBreak) {
					break;
				}
				if (board[i][j] != NULL && board[i][j]->colour == 'B') {
					pieces++;
					headPrevious = board[i][j]->potentialMoveList->next;
					if (board[i][j]->px != i || board[i][j]->py != j) {
						reset = true;
						pxPiece = board[i][j]->px;
						pyPiece = board[i][j]->py;
					}
					move = board[i][j]->GetMoveList(i, j, board);
					if (move == NULL)
						continue;
					move = move->next;
					while (move != NULL) {
						movesCount++;
						actualMove->Reset();
						*actualMove = *move;
						if (PlayBlackMove(actualMove)) {
							if (actualMove->capturedPiece != 0
									&& actualMove->capturedPiece->value
											== 100) {
								UndoMove(actualMove, board);
								delete actualMove;
								return 500;
							}
							if (isDrawByRepetition()) {
								UndoMove(actualMove, board);
								delete actualMove;
								return -50;
							}
							if (pz == 0) {
								z = tt->generateZ(board, true);
							} else {
								z = tt->updateZ(board, actualMove, pz);
							}
							struct Entry* entry = tt->getEntry(z);
							if (entry != NULL
									&& entry->depth >= maxDepth - depth) {
								score = entry->score;
							} else {
								score = Minimax_MoveList(board, true, depth + 1,
										maxDepth, actualMove, alpha, beta, z);
								entry = new Entry();
								entry->score = score;
								entry->z = z;
								entry->depth = maxDepth - depth;
								tt->insertEntry(entry, z);
							}
							if (score < bestScore) {
								bestScore = score;
							}
							if (score < beta) {
								beta = score;
							}
							movesCount++;
							UndoMove(actualMove, board);
							if (alpha >= beta) {
								alphaBetaBreak = true;
								*killerMoves[depth * 2] = *killerMoves[(depth
										* 2) + 1];
								*killerMoves[(depth * 2) + 1] = *actualMove;
								actualMove->Reset();
								break;
							}
						}
						move = move->next;
					}
					if (reset) {
						reset = false;
						move = board[i][j]->potentialMoveList->next;
						while (move != NULL) { //delete newly created moveList
							next = move->next;
							delete move;
							move = next;
						}
						board[i][j]->potentialMoveList->next = headPrevious; //reset moves to previous state after undoing simulated move
						board[i][j]->px = pxPiece;
						board[i][j]->py = pyPiece;
					}
				}
			}
		}
		if (bestScore == 9999) {
			delete actualMove;
			if (!isInCheck('B')) {
				return 0;
			}
			return 500;
		}
		delete actualMove;
		return bestScore;
	}
	delete actualMove;
	return bestScore;
}

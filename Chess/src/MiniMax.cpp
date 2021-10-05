#include "MiniMax.h"
#include "ChessPiece.h"
#include "Chessboard.h"
#include <iostream>

MiniMax::MiniMax(int difficulty, Chessboard* chessboard) {
	this->difficulty = difficulty;
	this->chessboard = chessboard;
}

/*
Goes through all possible moves and picks the one with the highest score
TODO check if end of game?
*/
Move* MiniMax::GetNextMove(ChessPiece* board[8][8], char colour)
{

	Move* move = new Move();
	Move* bestMove = new Move();
	int bestScore;
	//check every square for every possible move
	if (colour == 'W') {
		bestScore = -999;
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				for (int a = 0; a < 8; a++) {
					for (int b = 0; b < 8; b++) {
						move->x = a;
						move->y = b;
						move->px = i;
						move->py = j;
						ChessPiece* temp = board[i][j];
						ChessPiece* capturedPiece = board[a][b];
						//if there is a white piece, check all its moves
						if (temp != NULL && temp->colour == 'W') {
							if (chessboard->PlayWhiteMove(move)) {
								//play every legal move and calls Minimax on resulting board
								int score = Minimax(board, false, 0);
								std::cout << "White: score = " << score << ", move = " << move->px << move->py << move->x << move->y << std::endl;
								board[a][b] = capturedPiece;
								board[i][j] = temp;
								chessboard->removeWhiteMove(move); //remove move from move-list
								//chessboard->UndoMove(move, board[a][b], board, false); //why doesnt this work?
								//update score and bestMove if score from resulting game is better 
								if (score > bestScore) {
									bestScore = score;
									bestMove = move;
									std::cout << "White: best score = " << bestScore << ", best move = " << bestMove->px << bestMove->py << bestMove->x << bestMove->y << std::endl;
								}
							}
						}
					}
				}

			}
		}
	}

	if (colour == 'B') {
		bestScore = 999;
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				for (int a = 0; a < 8; a++) {
					for (int b = 0; b < 8; b++) {
						move->x = a;
						move->y = b;
						move->px = i;
						move->py = j;
						ChessPiece* temp = board[i][j];
						ChessPiece* capturedPiece = board[a][b];
						//if there is a black piece, check all its moves
						if (temp != NULL && temp->colour == 'B') {
							//play every legal move and call minimax on resulting board
							if (chessboard->PlayBlackMove(move)) {
								int score = Minimax(board, true, 0);
								std::cout << "Black: score = " << score << ", move = " << move->px << move->py << move->x << move->y << std::endl;
								board[a][b] = capturedPiece;
								board[i][j] = temp;
								chessboard->removeBlackMove(move);
								//chessboard->UndoMove(move, board[a][b], board, false);
								//update score and bestMove if score of resulting game is better
								if (score < bestScore) {
									bestScore = score;
									bestMove = move;
									std::cout << "Black: best score = " << bestScore << ", best move = " << bestMove->px << bestMove->py << bestMove->x << bestMove->y << std::endl;
								}
							}
						}
					}
				}

			}
		}
	}

	return bestMove;
}

int MiniMax::Minimax(ChessPiece* board[8][8], bool isMaximising, int depth) {
	if (difficulty == 1 && depth == 1) {
		return CalculateScore(board);
	}
	if (difficulty == 2 && depth == 2) {
		return CalculateScore(board);
	}
	if (difficulty == 3 && depth == 3) {
		return CalculateScore(board);
	}

	int bestScore;
	int score = 0;
	Move* move = new Move();
	if (isMaximising) {
		bestScore = -999;
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				for (int a = 0; a < 8; a++) {
					for (int b = 0; b < 8; b++) {
						move->px = i;
						move->py = j;
						move->x = a;
						move->y = b;
						ChessPiece* temp = board[i][j];
						ChessPiece* capturedPiece = board[a][b];
						if (temp != NULL && temp->colour == 'W'){
							if (chessboard->PlayWhiteMove(move)) {
								//check for checkmate
								if (capturedPiece != 0 && capturedPiece->value == 100) {
									board[a][b] = capturedPiece; //must not forget to undo the move when returning
									board[i][j] = temp;
									chessboard->removeWhiteMove(move);
									return -1000;
								}
								if (chessboard->isDrawByRepetition()) {
									board[a][b] = capturedPiece; //must not forget to undo the move when returning
									board[i][j] = temp;
									chessboard->removeWhiteMove(move);
									std::cout << "White moves-> Draw found at depth " << depth << std::endl;
									return 50;
								}
								score = Minimax(board, false, depth + 1);
								if (score > bestScore) {
									bestScore = score;
								}
								board[a][b] = capturedPiece;
								board[i][j] = temp;
								chessboard->removeWhiteMove(move);
								//chessboard->UndoMove(move, board[a][b], board, false);
							}
						}
					}
				}
			}
		}
		return bestScore;
	}
	if (!isMaximising) {
		bestScore = 999;
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				for (int a = 0; a < 8; a++) {
					for (int b = 0; b < 8; b++) {
						move->px = i;
						move->py = j;
						move->x = a;
						move->y = b;
						ChessPiece* temp = board[i][j];
						ChessPiece* capturedPiece = board[a][b];
						if (temp != NULL && temp->colour == 'B'){
							if (chessboard->PlayBlackMove(move)) {
								//check for checkmate
								if (capturedPiece != 0 && capturedPiece->value == 100) {
									board[a][b] = capturedPiece; //must not forget to undo the move when returning
									board[i][j] = temp;
									chessboard->removeBlackMove(move);
									return 1000;
								}
								if (chessboard->isDrawByRepetition()) { 
									board[a][b] = capturedPiece; //must not forget to undo the move when returning
									board[i][j] = temp;
									chessboard->removeBlackMove(move);
									std::cout << "Black moves-> Draw found at depth " << depth << std::endl;
									return -50;
								}
								score = Minimax(board, true, depth + 1);
								if (score < bestScore) {
									bestScore = score;
								}
								board[a][b] = capturedPiece;
								board[i][j] = temp;
								chessboard->removeBlackMove(move);
								//chessboard->UndoMove(move, board[a][b], board, false);
							}
						}
					}
				}
			}
		}
		return bestScore;
	}
	return 0;
}


int MiniMax::CalculateScore(ChessPiece* board[8][8]) {
	//return chessboard->materialCountWhite - chessboard->materialCountBlack; doesn't work because undoMove is not called to reset material. 
	int materialWhite = 0;
	int materialBlack = 0;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (board[i][j] != 0) {
				if (board[i][j]->colour == 'W') {
					materialWhite += board[i][j]->value;
				} else {
					materialBlack -= board[i][j]->value;
				}
			}
		}
	}
	return materialWhite + materialBlack;
}

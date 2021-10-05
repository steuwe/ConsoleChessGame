#pragma once
#include "ChessPiece.h"
#include "MiniMax.h"
#include "Move.h"
#include "Zobrist.h"

class Zobrist;

class Chessboard {
public:
	int moves;
	int scoreBlack;
	int scoreWhite;
	int materialCountWhite;
	int materialCountBlack;
	bool gameOver;
	int difficulty;
	int blackKing_posx;
	int blackKing_posy;
	int whiteKing_posx;
	int whiteKing_posy;
	bool hasBlackCastled;
	bool hasWhiteCastled;
	void Start();
	Move* GetNextMove(char colour);
	bool PlayWhiteMove(Move* move);
	bool PlayBlackMove(Move* move);
	void AlternateTurn();
	bool isInCheck(char colour);
	bool isInCheck2(char colour);
	void UndoMove(Move* move, ChessPiece* board[8][8]);
	ChessPiece* board[8][8];
	ChessPiece* pieces[32];
	ChessPiece* queens[16];
	short extraWhiteQueens;
	short extraBlackQueens;
	Move* moveListWhite;
	Move* moveListBlack;
	Move* killerMoves[16];
	Zobrist* tt;
	Chessboard(int difficulty);
	~Chessboard();
	int CalculateScoreBlack();
	int CalculateScoreWhite();
	void PrintBoard();
	void PrintMoveList();
	bool isDrawByRepetition();
	bool hasWhiteWon();
	bool hasBlackWon();
	void addBlackMove(Move* move);
	void addWhiteMove(Move* move);
	void removeBlackMove(Move* move);
	void removeWhiteMove(Move* move);
	bool CheckKingMoveBlack(Move* move, ChessPiece* movedPiece, ChessPiece* capturedPiece);
	bool CheckKingMoveWhite(Move* move, ChessPiece* movedPiece, ChessPiece* capturedPiece);
	void UndoCastlingBlack(Move* move);
	void UndoCastlingWhite(Move* move);
	Move* GetNextMinimaxMove(char colour);
	Move* GetNextMinimaxMove_MoveList(char colour);
	int Minimax(ChessPiece* board[8][8], bool isMaximising, int depth, int previousScore, Move* lastMove);
	int Minimax_MoveList(ChessPiece* board[8][8], bool isMaximising, int depth, int maxDepth, Move* lastMove, int alpha, int beta, unsigned long long pz);
	int CalculateScore(ChessPiece* board[8][8]);
	int CalculateKingSafety(ChessPiece* board[8][8]);
	int CalculateCenterControl(ChessPiece* board[8][8]);
	int CalculatePawnIslands(ChessPiece* board[8][8]);
	int CalculateDevelopment(ChessPiece* board[8][8]);
	void InsertBestMove(Move* bestMoves[5], Move* move, int *bestScores, int score, bool isMaximising);
};




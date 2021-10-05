/*
 * Zobrist.h
 *
 *  Created on: 16 Aug 2020
 *      Author: simon
 */

#ifndef ZOBRIST_H_
#define ZOBRIST_H_
#include "ChessPiece.h"
#include "Move.h"
#include "Entry.h"

class Zobrist
{
public:
	int tableSize;
	struct Entry* entries[1000];
	unsigned long long pieces[2][6][64];
	unsigned long long enPassant[8];
	unsigned long long castling[4];
	unsigned long long blackMove;
	Zobrist();
	unsigned long long generateZ(ChessPiece* board[8][8], bool blackMove);
	unsigned long long updateZ(ChessPiece* board[8][8], Move* move, unsigned long long z);
	struct Entry* getEntry(unsigned long long z);
	void insertEntry(struct Entry* entry, unsigned long long z);
	~Zobrist();
};



#endif /* ZOBRIST_H_ */

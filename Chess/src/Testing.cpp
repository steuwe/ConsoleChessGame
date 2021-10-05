/*
 * testing.cpp
 *
 *  Created on: 17 Aug 2020
 *      Author: simon
 */

#include "Zobrist.h"
#include "ChessPiece.h"
#include "Chessboard.h"
#include "Testing.h"
#include "Entry.h"
#include <stdexcept>
#include <iostream>

Testing::Testing() {
	Chessboard* chessboard = new Chessboard(3);

	Zobrist* zobrist = new Zobrist();

	unsigned long long x = zobrist->generateZ(chessboard->board, false);
	unsigned long long y = zobrist->generateZ(chessboard->board, false);
	if (x != y) {
		throw "Zobrist generateZ incorrect";
	}
	struct Entry* entry = new Entry();
	entry->depth = 3;
	entry->score = 4;
	entry->z = x;
	zobrist->insertEntry(entry, x);
	if (zobrist->entries[x%(zobrist->tableSize)] == NULL) {
		throw std::runtime_error("Zobrist insertEntry incorrect");
	}
	struct Entry* ent = zobrist->getEntry(x);
	if (ent == NULL || ent != entry) {
		throw std::runtime_error("Zobrist getEntry incorrect");
	}
	delete entry;
}

/*
 * Zobrist.cpp
 *
 *  Created on: 16 Aug 2020
 *      Author: simon
 */
#include <random>
#include "Entry.h"
#include "Zobrist.h"
#include <chrono>
#include <iostream>

Zobrist::Zobrist() {
	std::mt19937_64 generator(time(0));
	tableSize = 1000;
	for (int i = 0; i < tableSize; i++) {
		entries[i] = NULL;
	}
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 6; j++) {
			for (int k = 0; k < 64; k++) {
				this->pieces[i][j][k] = generator();
			}
		}
	}
	for (int i = 0; i < 8; i++) {
		this->enPassant[i] = generator();
	}
	for (int i = 0; i < 4; i++) {
		this->castling[i] = generator();
	}
	this->blackMove = generator();
}

unsigned long long Zobrist::generateZ(ChessPiece* board[8][8], bool blackMove) {
	unsigned long long num = 0;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (board[i][j] != NULL) {
				int colour = board[i][j]->colour == 'W' ? 0 : 1;
				if (board[i][j]->id == 'P') {
					num = num xor pieces[colour][0][j + i * 8];
					if ((colour == 0 && blackMove == false)
							|| (colour == 1 && blackMove == true)) {
						if (j - 1 > 0 && board[i][j - 1] != NULL
								&& board[i][j - 1]->colour
										!= board[i][j - 1]->colour
								&& board[i][j - 1]->id == 'P'
								&& board[i][j - 1]->justMovedTwo) {
							num = num xor enPassant[j - 1];
						}
						if (j + 1 > 0 && board[i][j + 1] != NULL
								&& board[i][j + 1]->colour
										!= board[i][j + 1]->colour
								&& board[i][j + 1]->id == 'P'
								&& board[i][j + 1]->justMovedTwo) {
							num = num xor enPassant[j + 1];
						}
					}
				} else if (board[i][j]->id == 'K') {
					num = num xor pieces[colour][1][j + i * 8];
				} else if (board[i][j]->id == 'B') {
					num = num xor pieces[colour][2][j + i * 8];
				} else if (board[i][j]->id == 'R') {
					num = num xor pieces[colour][3][j + i * 8];
				} else if (board[i][j]->id == 'Q') {
					num = num xor pieces[colour][4][j + i * 8];
				} else if (board[i][j]->id == 'k') {
					num = num xor pieces[colour][5][j + i * 8];
					if (board[i][j]->castlingRightsKS) {
						num = num xor castling[colour + 1];
					}
					if (board[i][j]->castlingRightsQS) {
						num = num xor castling[colour + 2];
					}
				}
			}
		}
	}
	return num;
}

unsigned long long Zobrist::updateZ(ChessPiece* board[8][8], Move* move, unsigned long long z) {
	char id = board[move->x][move->y]->id;
	int colour = board[move->x][move->y]->colour == 'W' ? 0 : 1;
	if (move->undidCastlingRightsKS) {
		z = z xor castling[colour + 1];
	}
	if (move->undidCastlingRightsQS) {
		z = z xor castling[colour + 2];
	}
	switch (id) {
	case 'P':
		z = z xor pieces[colour][0][move->py + move->px * 8];
		z = z xor pieces[colour][0][move->y + move->x * 8];
		break;
	case 'K':
		z = z xor pieces[colour][1][move->py + move->px * 8];
		z = z xor pieces[colour][1][move->y + move->x * 8];
		break;
	case 'B':
		z = z xor pieces[colour][2][move->py + move->px * 8];
		z = z xor pieces[colour][2][move->y + move->x * 8];
		break;
	case 'R':
		z = z xor pieces[colour][3][move->py + move->px * 8];
		z = z xor pieces[colour][3][move->y + move->x * 8];
		break;
	case 'Q':
		z = z xor pieces[colour][4][move->py + move->px * 8];
		z = z xor pieces[colour][4][move->y + move->x * 8];
		break;
	case 'k':
		z = z xor pieces[colour][5][move->py + move->px * 8];
		z = z xor pieces[colour][5][move->y + move->x * 8];
		break;
	default:
		break;
	}
	if (move->capturedPiece != NULL) {
		id = move->capturedPiece->id;
		switch (id) {
		case 'P':
			z = z xor pieces[colour][0][move->y + move->x * 8];
			break;
		case 'K':
			z = z xor pieces[colour][1][move->y + move->x * 8];
			break;
		case 'B':
			z = z xor pieces[colour][2][move->y + move->x * 8];
			break;
		case 'R':
			z = z xor pieces[colour][3][move->y + move->x * 8];
			break;
		case 'Q':
			z = z xor pieces[colour][4][move->y + move->x * 8];
			break;
		case 'k':
			z = z xor pieces[colour][5][move->y + move->x * 8];
			break;
		default:
			break;
		}
	}
	z = z xor blackMove;
	return z;
}

struct Entry* Zobrist::getEntry(unsigned long long z) {
	if (entries[z%tableSize] != NULL && entries[z%tableSize]->z == z) {
		return entries[z%tableSize];
	}
	return NULL;
}

void Zobrist::insertEntry(struct Entry* entry, unsigned long long z) {
	if (entries[z%tableSize] != NULL) {
		if (entry->depth < 3 && entries[z%tableSize]->depth > entry->depth) {
			return;
		}
		delete entries[z%tableSize];
	}
	entries[z%tableSize] = entry;
}

Zobrist::~Zobrist() {
	for (int i = 0; i < tableSize; i++) {
		if (entries[i] != NULL) {
			delete entries[i];
		}
	}
}

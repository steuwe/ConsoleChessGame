#include "Chessboard.h"
#include "Move.h"
#include <iostream>
#include <string>
#include "Testing.h"

Chessboard* chessboard;
char currentPlayer;
Move* newMove;
Move* cpuMove;

int board[8][8] = { -1, -2, -3, -4, -5, -3, -2, -1, -6, -6, -6, -6, -6, -6, -6,
		-6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 1, 2, 3, 4, 5, 3, 2,
		1 };

int main(int argc, char** argv) {
	//Testing* test = new Testing();
	chessboard = new Chessboard(3);
	currentPlayer = 'W';

	newMove = new Move();
	//for console moves
	std::string currentMove;
	std::string openingMessage;
	openingMessage = "";
	openingMessage.append(
			"CPU moves can be called by typing 'c' and pressing ENTER \n");
	openingMessage.append(
			"The format is the original square of the moved piece and its destination square, e.g. 'a2a4' \nwithout punctuation or spaces.\n");
	openingMessage.append(
			"This format also applies for castling (e.g. 'e1g1'), en passant and promotion.\n");
	openingMessage.append("You can then press Enter to confirm the move\n\n");
	openingMessage.append("Pawns are automatically promoted to queens.\n\n");

	chessboard->PrintBoard();
	std::cout << openingMessage << std::endl;
	std::string input;
	while (true) {
		std::cin >> input;
		if (input == "c") {
			if (currentPlayer == 'W') {
				cpuMove = chessboard->GetNextMove('W');
				if (!chessboard->PlayWhiteMove(cpuMove)) {
					std::cout << "Game over!\n";
					break;
				} else {
					std::cout << (char) (chessboard->moveListWhite->py + 97)
							<< (char) (chessboard->moveListWhite->px + 49)
							<< (char) (chessboard->moveListWhite->y + 97)
							<< (char) (chessboard->moveListWhite->x + 49)
							<< std::endl;
					currentPlayer = 'B';
					if (chessboard->isDrawByRepetition()) {
						std::cout << "Draw by repetition!";
						break;
					}
					if (chessboard->hasWhiteWon()) {
						if (chessboard->isInCheck('B')) {
							std::cout << "White wins!";
						} else {
							std::cout << "Stalemate!";
						}
						break;
					}
				}
			} else {
				cpuMove = chessboard->GetNextMove('B');
				if (!chessboard->PlayBlackMove(cpuMove)) {
					std::cout << "Game over!\n";
					break;
				} else {
					std::cout << (char) (chessboard->moveListBlack->py + 97)
							<< (char) (chessboard->moveListBlack->px + 49)
							<< (char) (chessboard->moveListBlack->y + 97)
							<< (char) (chessboard->moveListBlack->x + 49)
							<< std::endl;
					currentPlayer = 'W';
					if (chessboard->isDrawByRepetition()) {
						std::cout << "Draw by repetition!";
						break;
					}
					if (chessboard->hasBlackWon()) {
						if (chessboard->isInCheck('W')) {
							std::cout << "Black wins!";
						} else {
							std::cout << "Stalemate!";
						}
						break;
					}
				}
			}
		} else if (input == "undo") {
			if (currentPlayer == 'W') {
				chessboard->UndoMove(chessboard->moveListBlack,
						chessboard->board);
				currentPlayer = 'B';
			} else {
				chessboard->UndoMove(chessboard->moveListWhite,
						chessboard->board);
				currentPlayer = 'W';
			}
		} else {
			if (input.length() != 4 || input[0] < 'a' || input[0] > 'h'
					|| input[1] - 48 < 1 || input[1] - 48 > 8 || input[2] < 'a'
					|| input[2] > 'h' || input[3] - 48 < 1
					|| input[3] - 48 > 8) {
				std::cout << "Wrong input! Example: e1g1" << std::endl;
				continue;
			}
			newMove->py = input[0] - 97;
			newMove->px = input[1] - 49;
			newMove->y = input[2] - 97;
			newMove->x = input[3] - 49;
			if (currentPlayer == 'W') {
				if (!chessboard->PlayWhiteMove(newMove)) {
					std::cout << "Illegal move! Try again" << std::endl;
				} else {
					currentPlayer = 'B';
					if (chessboard->isDrawByRepetition()) {
						std::cout << "Draw by repetition!";
						break;
					}
					if (chessboard->hasWhiteWon()) {
						if (chessboard->isInCheck('B')) {
							std::cout << "White wins!";
						} else {
							std::cout << "Stalemate!";
						}
						break;
					}
				}
			} else {
				if (!chessboard->PlayBlackMove(newMove)) {
					std::cout << "Illegal move! Try again" << std::endl;
				} else {
					currentPlayer = 'W';
					if (chessboard->isDrawByRepetition()) {
						std::cout << "Draw by repetition!";
						break;
					}
					if (chessboard->hasBlackWon()) {
						if (chessboard->isInCheck('W')) {
							std::cout << "Black wins!";
						} else {
							std::cout << "Stalemate!";
						}
						break;
					}
				}
			}
		}
		newMove->Reset();
		std::cout << "Development: "
				<< chessboard->CalculateDevelopment(chessboard->board)
				<< std::endl;
		std::cout << "King safety: "
				<< chessboard->CalculateKingSafety(chessboard->board)
				<< std::endl;
		std::cout << "Material: "
				<< (chessboard->materialCountWhite
						- chessboard->materialCountBlack) * 10 << std::endl;
		chessboard->PrintBoard();
		if (chessboard->isInCheck(currentPlayer)) {
			std::cout << "CHECK!" << std::endl;
		}
	}
	chessboard->~Chessboard();
	return 0;
}

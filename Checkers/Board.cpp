#include <iostream>
#include <windows.h>
#include "Board.h"

//PUBLIC:

Board::Board()
{
	numberOfBlackPieces = 0;
	numberOfWhitePieces = 0;
	iCur = 0;
	jCur = 0;
	board = new char* [rows];
	for (int i = 0; i < rows; ++i)
	{
		board[i] = new char[columns + 1];
		board[i][columns] = '\0';
	}
	capturedPieces = new int* [2];
	for (int i = 0; i < 2; ++i)
	{
		capturedPieces[i] = new int[12];
		for (int j = 0; j < 12; ++j)
		{
			capturedPieces[i][j] = 0;
		}
	}
	numOfCaptured = 0;
	hasCaptureJustBeenDone = false;
	FillVoid();
	FillEdges();
	FillPlayField();
	ArrangePieces();
	side = 'w';
	moveDirection = 1;
}
Board::~Board()
{
	for (int i = 0; i < rows; ++i)
	{
		delete[] board[i];
	}
	delete board;
	for (int i = 0; i < 2; ++i)
	{
		delete[] capturedPieces[i];
	}
	delete capturedPieces;
}

std::ostream& operator << (std::ostream& os, const Board& board)
{
	board.ASCII(os, '_', board.linesLength) << std::endl;
	if (board.side == 'b')
	{
		os << "\t\t\t\t\t\t\t\t  ";
	}
	board.ASCII(os, 219, 16);
	if (board.side == 'b')
	{
		os << " vvv  black side makes move  vvv ";
		board.ASCII(os, 219, 16);
	}
	else
	{
		os << " vvv  white side makes move  vvv ";
		board.ASCII(os, 219, 16);
	}
	os << std::endl;

	board.DisplayBoard(os);

	board.ASCII(os, '_', board.linesLength) << std::endl;
	if (board.side == 'b')
	{
		os << "\t\t\t\t\t\t\t\t  ";
	}
	board.ASCII(os, 219, 65) << std::endl;
	board.ASCII(os, '_', board.linesLength) << std::endl;
	os << " white pieces: " << board.numberOfWhitePieces << "\t\t\t\t\t\t   black pieces: " << board.numberOfBlackPieces << std::endl;
	board.ASCII(os, '-', board.linesLength) << std::endl;
	return os;
}
std::ostream& Board::ASCII(std::ostream& os, const int numInTable, const int quantity)
{
	if (quantity < 1)
	{
		throw std::invalid_argument("quantity must be positive non-zero!");
	}
	for (int i = 0; i < quantity; ++i)
	{
		os << char(numInTable);
	}
	return os;
}
void Board::choosePiece(const char& letter, const int& number)
{
	if (!areCoordinatesInRange(letter, number))
	{
		throw std::out_of_range("Coordinates are out of range. Validation wasn't done");
	}
	else if (!isFriendPieceHere(letter, number))
	{
		throw std::out_of_range("Wrong square was chosen. Validation wasn't done");
	}
 	else if (isStuck(letter, number))
	{
		throw std::out_of_range("This piece can't move. Validation wasn't done");
	}
	else if (!canCapture(letter, number) && isCaptureAvailableForCurrentSide())
	{
		throw std::out_of_range("Wrong piece was chosen: there is capture available. Validation wasn't done");
	}
	iCur = Toi(number);
	jCur = Toj(letter);
	PutArrows(iCur, jCur);
}
void Board::makeMove(const char& newLetter, const int& newNumber)
{
	if (!areCoordinatesInRange(newLetter, newNumber))
	{
		throw std::out_of_range("Coordinates are out of range. Validation wasn't done");
	}
	int iNew = Toi(newNumber), jNew = Toj(newLetter), iOld = iCur, jOld = jCur, iCaptured = 0, jCaptured = 0;
	bool isKing = IsKing(iCur, jCur), captured = false;
	if (canCapture(ToLet(jOld), ToNum(iOld)))
	{
		hasCaptureJustBeenDone = false;
		captured = true;
		SetCapturedCoords(iCaptured, jCaptured, iNew, jNew, iOld, jOld);
		MarkCaptured(iCaptured, jCaptured);
		capturedPieces[0][numOfCaptured] = iCaptured;
		capturedPieces[1][numOfCaptured] = jCaptured;
		++numOfCaptured;
	}
	this->iCur = iNew;
	this->jCur = jNew;
	DeletePiece(iOld, jOld);
	PutPiece(iNew, jNew, this->side);
	if (isKing || (side == 'w' && iNew == iZero + 7 * iDelta) || (side == 'b' && iNew == iZero))
	{
		TurnToKing(iNew, jNew );
	}
	if (captured && !canCapture(ToLet(jNew), ToNum(iNew)))
	{
		for (int i = 0; i < numOfCaptured; ++i)
		{
			DeletePiece(capturedPieces[0][i], capturedPieces[1][i]);
		}
		numOfCaptured = 0;
		hasCaptureJustBeenDone = true;
	}
	DeleteArrows(iOld, jOld);
	PutArrows(iNew, jNew);
}
void Board::changeSide()
{
	side = EnemySide();
	moveDirection *= -1;
	hasCaptureJustBeenDone = false;
	DeleteArrows(this->iCur, this->jCur);
}
//validation:
bool Board::areCoordinatesInRange(const char& letter, const int& number)
{
	return (GetLower(letter) >= 'a' && GetLower(letter) <= 'h' && number >= 1 && number <= 8);
}
bool Board::isFriendPieceHere(const char& currentLetter, const int& currentNumber) const
{
	if (!areCoordinatesInRange(currentLetter, currentNumber))
	{
		throw std::out_of_range("Position is out of range");
	}
	int i0 = Toi(currentNumber), j0 = Toj(currentLetter);
	char squareStatus = SquareStatus(i0, j0);
	return (GetLower(squareStatus) == side);
}
bool Board::areNeighbourSquaresFree(const char& currentLetter, const int& currentNumber) const
{
	if (!isFriendPieceHere(currentLetter, currentNumber))
	{
		throw std::invalid_argument("Current position is not valid");
	}
	int i0 = Toi(currentNumber), j0 = Toj(currentLetter);
	if (IsKing(i0, j0))
	{
		return (SquareStatus(i0 + iDelta, j0 + jDelta) == ' ' ||
				SquareStatus(i0 + iDelta, j0 - jDelta) == ' ' ||
				SquareStatus(i0 - iDelta, j0 + jDelta) == ' ' ||
				SquareStatus(i0 - iDelta, j0 - jDelta) == ' ');
	}
	else 
	{
		return (SquareStatus(i0 + moveDirection * iDelta, j0 - jDelta) == ' ' ||
				SquareStatus(i0 + moveDirection * iDelta, j0 + jDelta) == ' ');
	}
}
bool Board::canCapture(const char& currentLetter, const int& currentNumber) const
{
	if (!isFriendPieceHere(currentLetter, currentNumber))
	{
		throw std::invalid_argument("Current position is not valid");
	}
	if (hasCaptureJustBeenDone)
	{
		return false;
	}
	int i0 = Toi(currentNumber), j0 = Toj(currentLetter);
	if (IsKing(i0, j0))
	{
		return (CanKingCaptureInDirection(i0, j0, 1, 1) ||
			CanKingCaptureInDirection(i0, j0, -1, 1) ||
			CanKingCaptureInDirection(i0, j0, 1, -1) ||
			CanKingCaptureInDirection(i0, j0, -1, -1));
	}
	else
	{
		return (CanMenCaptureInDirection(i0, j0, 1, 1) ||
			CanMenCaptureInDirection(i0, j0, 1, -1) ||
			CanMenCaptureInDirection(i0, j0, -1, 1) ||
			CanMenCaptureInDirection(i0, j0, -1, -1));
	}
}
bool Board::isStuck(const char& currentLetter, const int& currentNumber) const
{
	if (!isFriendPieceHere(currentLetter, currentNumber))
	{
		throw std::invalid_argument("Current position is not valid");
	}
	return (!areNeighbourSquaresFree(currentLetter, currentNumber) && !canCapture(currentLetter, currentNumber));
}
bool Board::isCaptureAvailableForCurrentSide() const
{
	for (int i = iZero; i <= iZero + 7 * iDelta; i += iDelta)
	{
		for (int j = jZero; j <= jZero + 7 * jDelta; j += jDelta)
		{
			if (GetLower(SquareStatus(i, j)) == FriendSide())
			{
				if (canCapture(ToLet(j), ToNum(i)))
				{
					return true;
				}
			}
		}
	}
	return false;
}
bool Board::areAllPiecesStuckForCurSide() const
{
	for (int i = iZero; i <= iZero + 7 * iDelta; i += iDelta)
	{
		for (int j = jZero; j <= jZero + 7 * jDelta; j += jDelta)
		{
			if (GetLower(SquareStatus(i, j)) == FriendSide())
			{
				if (!isStuck(ToLet(j), ToNum(i)))
				{
					return false;
				}
			}
		}
	}
	return true;
}
bool Board::isMoveValid(const char& newLetter, const int& newNumber) const
{
	if (!areCoordinatesInRange(newLetter, newNumber))
	{
		throw std::out_of_range("Position is out of range");
	}
	int iNew = Toi(newNumber), jNew = Toj(newLetter);
	if (!IsMoveDiagonal(iNew, jNew))
	{
		return false;
	}
	if (IsKing(iCur, jCur))
	{
		return IsMoveValidForKing(iNew, jNew);
	}
	return IsMoveValidForMen(iNew, jNew);
}
bool Board::willCaptureWithMove(const char& newLetter, const int& newNumber) const
{
	if (!isMoveValid(newLetter, newNumber))
	{
		return false;
	}
	int iNew = Toi(newNumber), jNew = Toj(newLetter);
	int iMoveDirection = (iNew - iCur < 0) ? -1 : 1,
		jMoveDirection = (jNew - jCur < 0) ? -1 : 1;	//1 == up/right, -1 == down/left
	for (int i = iCur + iMoveDirection * iDelta, j = jCur + jMoveDirection * jDelta; i != iNew; i += iMoveDirection * iDelta, j += jMoveDirection * jDelta)
	{
		if (GetLower(SquareStatus(i, j)) == EnemySide() && SquareStatus(i + iMoveDirection * iDelta, j + jMoveDirection * jDelta) == ' ')
		{
			return !IsCaptured(i, j);
		}
	}
	return false;
}
//setters:
void Board::setSide(char side)
{
	if (side == 'b' || side == 'B')
	{
		this->side = 'b';
	}
	else if (side == 'w' || side == 'W')
	{
		this->side = 'w';
	}
	else
	{
		throw std::invalid_argument("The only arguments are: 'b', 'B', 'w', 'W'");
	}
}
//getters:
int Board::getNumOfPieces(char side) const
{
	switch (side)
	{
	case 'b':
		return numberOfBlackPieces;
	case 'w':
		return numberOfWhitePieces;
	default:
		throw std::invalid_argument("only 'b' or 'w' can be used");
	}
}
char Board::getCurLet() const
{
	return ToLet(this->jCur);
}
int Board::getCurNum() const
{
	return ToNum(this->iCur);
}
char Board::getCurSide() const
{
	return side;
}


//PRIVATE:

//static:
int Board::Toi(const int& number)
{
	return iZero + (number - 1)  * iDelta;
}
int Board::Toj(const char& letter)
{
	char firstLetter = IsUpperCase(letter) ? 'A' : 'a';
	return jZero + (letter - firstLetter) * jDelta;
}
char Board::ToLet(const int& j0)
{
	return 'A' + (j0 - jZero) / jDelta;
}
int Board::ToNum(const int& i0)
{
	return 1 + (i0 - iZero) / iDelta;
}
bool Board::IsUpperCase(const char& letter)
{
	return (letter >= 'A' && letter <= 'Z');
}
char Board::GetLower(const char& letter)
{
	if (IsUpperCase(letter))
	{
		return letter + differenceBetweenCases;
	}
	return letter;
}
bool Board::AreIJCoordinatesInRange(const int& i0, const int& j0)
{
	if (i0 < iZero || i0 > iZero + 7 * iDelta || j0 < jZero || j0 > jZero + 7 * jDelta)
	{
		return false;
	}
	return true;
}
//for drawing board:
void Board::FillVoid()
{
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < columns; ++j)
		{
			board[i][j] = ' ';
		}
	}
}
void Board::FillEdges()
{
	for (int i = 2; i < rows; i += (rows - 5))
	{
		for (int j = 2; j < columns - 2; ++j)
		{
			board[i][j] = boardEdge; 
		}
	}
	for (int i = 2; i < rows - 2; ++i)
	{
		for (int j = 2; j < columns; j += (columns - 5))
		{
			board[i][j] = boardEdge;
		}
	}
	for (int i = 1; i < rows; i += (rows - 3))
	{
		for (int j = 6; j < columns - 1; j += 7)
		{
			board[i][j] = 'A' + (j + 1) / 7 - 1;
		}
	}
	for (int i = 4; i < rows - 2; i += 3)
	{
		for (int j = 1; j < columns; j += (columns - 3))
		{
			board[i][j] = '1' + i / 3 - 1;
		}
	}
}
void Board::FillPlayField()
{
	for (int i = iZero; i <= iZero + 7 * iDelta; i += iDelta)
	{
		for (int j = jZero; j <= jZero + 7 * jDelta; j += jDelta)
		{
			if (MustBeFilled(i, j))
			{
				FillSquare(i, j);
			}
		}
	}
}
void Board::FillSquare(const int& i0, const int& j0)
{
	for (int i = -1; i <= 1; ++i)
	{
		for (int j = -3; j <= 3; ++j)
		{
			board[i0 + i][j0 + j] = whiteBlock; 
		}
	}
}
bool Board::MustBeFilled(const int& i0, const int& j0) const
{
	if ((i0 - iZero) % (2 * iDelta) == 0)
	{
		if ((j0 - jZero) % (2 * jDelta) != 0)
		{
			return true;
		}
	}
	else
	{
		if ((j0 - jZero) % (2 * jDelta) == 0)
		{
			return true;
		}
	}
	return false;
}
//actions:
void Board::ArrangePieces()
{
	for (int i = iZero; i <= iZero + 2 * iDelta; i += iDelta)
	{
		for (int j = jZero; j <= jZero + 7 * jDelta; j += jDelta)
		{
			if (MustPieceBePut(i, j))
			{
				PutPiece(i, j, 'w');
			}
		}
	}
	for (int i = iZero + 5 * iDelta; i <= iZero + 7 * iDelta; i += iDelta)
	{
		for (int j = jZero; j <= jZero + 7 * jDelta; j += jDelta)
		{
			if (MustPieceBePut(i, j))
			{
				PutPiece(i, j, 'b');
			}
		}
	}
	//TestArrangement1();
	//TestArrangement2();
}
bool Board::MustPieceBePut(const int& i0, const int& j0) const
{
	if ((j0 - jZero) % (2 * jDelta) == 0)
	{
		if (i0 == iZero || i0 == iZero + 2 * iDelta || i0 == iZero + 6 * iDelta)
		{
			return true;
		}
	}
	else
	{
		if (i0 == iZero + iDelta || i0 == iZero + 5 * iDelta || i0 == iZero + 7 * iDelta)
		{
			return true;
		}
	}
	return false;
}
void Board::PutPiece(const int& i0, const int& j0, const char& side)
{
	int symbolAscii = 0;
	if (side == 'b' || side == 'B')
	{
		symbolAscii = blackPiece;
		++numberOfBlackPieces;
	}
	else if (side == 'w' || side == 'W')
	{
		symbolAscii = whitePiece;
		++numberOfWhitePieces;
	}
	else
	{
		throw std::invalid_argument("The only arguments are: 'b', 'B', 'w', 'W'");
	}
	for (int k = -1; k <= 1; ++k)
	{
		board[i0][j0 + k] = symbolAscii;
	}
}
void Board::DeletePiece(const int& i0, const int& j0)
{
	GetLower(SquareStatus(i0, j0)) == 'b' ? --numberOfBlackPieces : --numberOfWhitePieces;
	for (int i = -1; i <= 1; ++i)
	{
		for (int j = -3; j <= 3; ++j)
		{
			board[i0 + i][j0 + j] = ' ';
		}
	}
}
void Board::TurnToKing(const int& i0, const int& j0)
{
	for (int i = -1; i <= 1; i += 2)
	{
		for (int j = -1; j <= 1; ++j)
		{
			board[i0 + i][j0 + j] = crown;
		}
	}
}
void Board::SetCapturedCoords(int& i0, int& j0, const int& iNew, const int& jNew, const int& iOld, const int& jOld)
{
	int iMoveDirection = iNew - iOld < 0 ? -1 : 1,	//1 == up, -1 == down
		jMoveDirection = jNew - jOld < 0 ? -1 : 1;	//1 == right, -1 == left
	for (int i = iOld + iMoveDirection * iDelta, j = jOld + jMoveDirection * jDelta; i != iNew; i += iMoveDirection * iDelta, j += jMoveDirection * jDelta)
	{
		if (GetLower(SquareStatus(i, j)) == EnemySide())
		{
			i0 = i;
			j0 = j;
		}
	}
}
void Board::MarkCaptured(const int& i0, const int& j0)
{
	board[i0 - 1][j0 - 3] = 'X';
	board[i0 - 1][j0 + 3] = 'X';
	board[i0 + 1][j0 - 3] = 'X';
	board[i0 + 1][j0 + 3] = 'X';

}
void Board::PutArrows(const int& i0, const int& j0)
{
	for (int i = -1; i <= 1; ++i)
	{
		board[i0 + i][0] = horizLine; 
	}
	board[i0][0] = cross;
	for (int i = -1; i <= 1; ++i)
	{
		board[i0 + i][columns - 1] = horizLine;
	}
	board[i0][columns - 1] = cross;
	for (int j = -3; j <= 3; ++j)
	{
		board[0][j0 + j] = vertLine;
	}
	board[0][j0] = cross;
	for (int j = -3; j <= 3; ++j)
	{
		board[rows - 1][j0 + j] = vertLine;
	}
	board[rows - 1][j0] = cross;
	board[i0][j0 - 2] = 254;
	board[i0][j0 + 2] = 254;
}
void Board::DeleteArrows(const int& i0, const int& j0)
{
	for (int i = -1; i <= 1; ++i)
	{
		board[i0 + i][0] = ' ';
	}
	board[i0][0] = ' ';
	for (int i = -1; i <= 1; ++i)
	{
		board[i0 + i][columns - 1] = ' ';
	}
	board[i0][columns - 1] = ' ';
	for (int j = -3; j <= 3; ++j)
	{
		board[0][j0 + j] = ' ';
	}
	board[0][j0] = ' ';
	for (int j = -3; j <= 3; ++j)
	{
		board[rows - 1][j0 + j] = ' ';
	}
	board[rows - 1][j0] = ' ';
	board[i0][j0 - 2] = ' ';
	board[i0][j0 + 2] = ' ';
}
//others:
void Board::DisplayBoard(std::ostream& os) const
{
	for (int i = rows - 1; i >= 0; --i)
	{
		for (int j = 0; j < columns; ++j)
		{
			os << board[i][j];
		}
		os << "   |   ";
		for (int j = columns - 1; j >= 0; --j)
		{
			os << board[rows - i - 1][j];
		}
		os << std::endl;
	}
}
char Board::SquareStatus(const int& i0, const int& j0) const
{
	char result = '\0';
	if (!AreIJCoordinatesInRange(i0, j0))
	{
		return result;
	}
	if (this->board[i0][j0] == blackPiece)
	{
		result = 'b';
	}
	else if (this->board[i0][j0] == whitePiece)
	{
		result = 'w';
	}
	else if (this->board[i0][j0] == ' ')
	{
		result = ' ';
	}
	if (this->board[i0 + 1][j0] == crown)  //253 == '¤'
	{
		result -= differenceBetweenCases;	//big letter == king
	}
	return result;
}
char Board::FriendSide() const
{
	return side;
}
char Board::EnemySide() const
{
	return this->side == 'b' ? 'w' : 'b';
}
//info about move:
bool Board::IsMoveDiagonal(const int& iNew, const int& jNew) const
{
	return (abs(iNew - iCur) / iDelta == abs(jNew - jCur) / jDelta && iNew - iCur != 0);
}
bool Board::IsMoveValidForKing(const int& iNew, const int& jNew) const
{
	int iMoveDirection = iNew - iCur < 0 ? -1 : 1,
		jMoveDirection = jNew - jCur < 0 ? -1 : 1,	//1 == up/right, -1 == down/left
		iLastEnemyPieceCoord = 0,
		jLastEnemyPieceCoord = 0;
	bool jumpedOverEnemyPiece = false;
	for (int i = iCur + iMoveDirection * iDelta, j = jCur + jMoveDirection * jDelta;
		(iNew - i) * iMoveDirection >= 0;
		i += iMoveDirection * iDelta, j += jMoveDirection * jDelta)
	{
		if (GetLower(SquareStatus(i, j)) == FriendSide())
		{
			return false;
		}
		if (GetLower(SquareStatus(i, j)) == EnemySide())
		{
			if (jumpedOverEnemyPiece)
			{
				return false;
			}
			else
			{
				if (i == iNew)
				{
					return false;
				}
				jumpedOverEnemyPiece = true;
				iLastEnemyPieceCoord = i;
				jLastEnemyPieceCoord = j;
			}
		}
	}
	if (jumpedOverEnemyPiece)
	{
		return HasKingChosenValidPosition(iLastEnemyPieceCoord, jLastEnemyPieceCoord, iNew, jNew, iMoveDirection, jMoveDirection);
	}
	return true;
}
bool Board::IsMoveValidForMen(const int& iNew, const int& jNew) const
{
	if (iNew - iCur == moveDirection * iDelta && SquareStatus(iNew, jNew) == ' ')
	{
		return true;
	}
	if (abs(iNew - iCur) == 2 * iDelta)
	{
		if (SquareStatus(iNew, jNew) == ' ' && GetLower(SquareStatus((iNew + iCur) / 2, (jNew + jCur) / 2)) == EnemySide())
		{
			return true;
		}
	}
	return false;
}
bool Board::CanKingCaptureInDirection(const int& i0, const int& j0, int iDirection, int jDirection) const
{
	for (int i = i0 + iDirection * iDelta, j = j0 + jDirection * jDelta;
		SquareStatus(i, j) == ' ' || GetLower(SquareStatus(i, j)) == EnemySide();
		i += iDirection * iDelta, j += jDirection * jDelta)
	{
		if (GetLower(SquareStatus(i, j)) == EnemySide())
		{
			return SquareStatus(i + iDirection * iDelta, j + jDirection * jDelta) == ' ' && !IsCaptured(i, j);
		}
	}
	return false;
}
bool Board::CanMenCaptureInDirection(const int& i0, const int& j0, int iDirection, int jDirection) const
{
	return GetLower(SquareStatus(i0 + iDirection * iDelta, j0 + jDirection * jDelta)) == EnemySide() &&
		SquareStatus(i0 + iDirection * 2 * iDelta, j0 + jDirection * 2 * jDelta) == ' ' &&
		!IsCaptured(i0 + iDirection * iDelta, j0 + jDirection * jDelta);
}
bool Board::HasKingChosenValidPosition(const int& iLastEnemyPieceCoord, const int& jLastEnemyPieceCoord, const int& iNew, const int& jNew, const int& iMoveDirection, const int& jMoveDirection) const
{
	if (CanKingCaptureInDirection(iNew, jNew, iMoveDirection, jMoveDirection) ||
		CanKingCaptureInDirection(iNew, jNew, -iMoveDirection, jMoveDirection) ||
		CanKingCaptureInDirection(iNew, jNew, iMoveDirection, -jMoveDirection))
	{
		return true;
	}
	int iOldPosition = iCur;
	int jOldPosition = jCur;
	for (int i = iLastEnemyPieceCoord + iMoveDirection * iDelta, j = jLastEnemyPieceCoord + jMoveDirection * jDelta;
		AreIJCoordinatesInRange(i, j) && SquareStatus(i, j) == ' ';
		i += iMoveDirection * iDelta, j += jMoveDirection * jDelta)
	{
		if (CanKingCaptureInDirection(i, j, -iMoveDirection, jMoveDirection) || CanKingCaptureInDirection(i, j, iMoveDirection, -jMoveDirection))
		{
			return false;
		}
	}
	return true;
}
//other info:
bool Board::IsKing(int i0, int j0) const
{
	return IsUpperCase(SquareStatus(i0, j0));
}
bool Board::IsCaptured(const int& i0, const int& j0) const
{
	return board[i0 + 1][j0 + 3] == 'X';
}


//TEST ARRANGEMENTS:

void Board::TestArrangement1()
{
	PutPiece(iZero + 0 * iDelta, jZero + 0 * jDelta, 'b');
	TurnToKing(iZero + 0 * iDelta, jZero + 0 * jDelta);
	PutPiece(iZero + 3 * iDelta, jZero + 7 * jDelta, 'b');
	PutPiece(iZero + 1 * iDelta, jZero + 3 * jDelta, 'w');
	PutPiece(iZero + 1 * iDelta, jZero + 5 * jDelta, 'w');
	PutPiece(iZero + 6 * iDelta, jZero + 2 * jDelta, 'w');
	PutPiece(iZero + 5 * iDelta, jZero + 3 * jDelta, 'w');
	PutPiece(iZero + 4 * iDelta, jZero + 4 * jDelta, 'w');
	PutPiece(iZero + 6 * iDelta, jZero + 4 * jDelta, 'w');
}	  
void Board::TestArrangement2()
{
	PutPiece(iZero + 2 * iDelta, jZero + 2 * jDelta, 'b');
	PutPiece(iZero + 7 * iDelta, jZero + 1 * jDelta, 'b');
	PutPiece(iZero + 7 * iDelta, jZero + 7 * jDelta, 'b');
	PutPiece(iZero + 7 * iDelta, jZero + 5 * jDelta, 'b');
	PutPiece(iZero + 6 * iDelta, jZero + 6 * jDelta, 'b');
	PutPiece(iZero + 5 * iDelta, jZero + 1 * jDelta, 'w');
	PutPiece(iZero + 4 * iDelta, jZero + 6 * jDelta, 'w');

}

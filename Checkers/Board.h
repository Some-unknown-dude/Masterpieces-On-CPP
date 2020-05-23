#pragma once
#include <iostream>
class Board
{
public:
	Board();
	~Board();
	friend std::ostream& operator << (std::ostream&, const Board&);
	static std::ostream& ASCII(std::ostream&, const int, const int);
	void choosePiece(const char&, const int&);
	void makeMove(const char&, const int&);
	void changeSide();
	//validation:
	static bool areCoordinatesInRange(const char& letter, const int& number);
	bool isFriendPieceHere(const char&, const int&) const;
	bool areNeighbourSquaresFree(const char&, const int&) const;
	bool canCapture(const char&, const int&) const;
	bool isStuck(const char&, const int&) const;
	bool isCaptureAvailableForCurrentSide() const;
	bool areAllPiecesStuckForCurSide() const;
	bool isMoveValid(const char&, const int&) const;
	bool willCaptureWithMove(const char&, const int&) const;
	//setters:
	void setSide(char);
	// getters:
	int getNumOfPieces(char) const;
	char getCurLet() const; //get current letter
	int getCurNum() const; //get current number
	char getCurSide() const;
private:
	char** board;
	int** capturedPieces; //*(1)
	bool hasCaptureJustBeenDone; //always false except between the end of capture and change of side
	int numOfCaptured;	
	char side;	//current side: 'b' (black) or 'w' (white)
	int moveDirection;  //1 for white, -1 for black

	static const int differenceBetweenCases = 32,	//'A' + 32 = 'a'
		rows = 30,
		columns = 62,
		iZero = 4,	//Zero is the center coordinate of top left square (A1)
		jZero = 6,
		iDelta = 3,	//Delta is the value which should be added to reach the center of next square
		jDelta = 7,
		linesLength = 131; //for displaying

	//symbols for drawing:
	static const char blackPiece = 176, //176 == '░'
		whitePiece = 178, //------------- 178 == '▓'
		crown = 253, //------------------ 253 == '¤'
		whiteBlock = 219, //------------- 219 == '█'
		littleWhiteBlock = 254, //------- 254 == '■'
		boardEdge = 177, //-------------- 177 == '▒'
		horizLine = 186, //-------------- 186 == '═'
		vertLine = 205, //--------------- 205 == '║'
		cross = 206; //------------------ 206 == '╬'

	int numberOfBlackPieces;
	int numberOfWhitePieces;
	int iCur;
	int jCur;
	//static:
	static int Toi(const int&);
	static int Toj(const char&);
	static char ToLet(const int&);
	static int ToNum(const int&);
	static bool IsUpperCase(const char&);
	static char GetLower(const char&);
	static bool AreIJCoordinatesInRange(const int& i0, const int& j0);
	//for drawing board:
	void FillVoid();
	void FillEdges();
	void FillPlayField();
	void FillSquare(const int&, const int&);
	bool MustBeFilled(const int&, const int&) const;
	void ArrangePieces();
	bool MustPieceBePut(const int&, const int&) const;
	//actions:
	void PutPiece(const int&, const int&, const char&);
	void TurnToKing(const int&, const int&);
	void DeletePiece(const int&, const int&);
	void SetCapturedCoords(int&, int&, const int&, const int&, const int&, const int&);
	void MarkCaptured(const int&, const int&);	//draws 'X' in each corner of square
	void PutArrows(const int&, const int&);
	void DeleteArrows(const int&, const int&);
	//others:
	void DisplayBoard(std::ostream&) const;
	char SquareStatus(const int&, const int&) const; //returns: 'b' == usual black, 'w' == usual white, 'B' == king black, 'W' == king white, ' ' == empty, '\0' == wrong coordinates
	char FriendSide() const;
	char EnemySide() const;
	//info about move:
	bool IsMoveDiagonal(const int&, const int&) const;
	bool IsMoveValidForKing(const int&, const int&) const;
	bool IsMoveValidForMen(const int&, const int&) const;
	bool CanKingCaptureInDirection(const int&, const int&, int, int) const;
	bool CanMenCaptureInDirection(const int&, const int&, int, int) const;
	bool HasKingChosenValidPosition(const int&, const int&, const int&, const int&, const int&, const int&) const;	// *(2)
	//other info:
	bool IsKing(int, int) const;
	bool IsCaptured(const int&, const int&) const;

	//TEST ARRANGEMENTS:

	void TestArrangement1(); //*(3)
	void TestArrangement2(); //*(4)
};
//			NOTES:
//	*(1) this matrix contains list of (ij) coordinates of pieces that have been captured during the current move, if multiple pieces is captures during this move
//
//		 0	1	2	3	4	5	6	7	8	9	10	11
//		----------------------------------------------
//	 0|	i	i	i	...		0	0	0	0	0	0
//	 1|	j	j	j	...		0	0	0	0	0	0
//
//	if [0][0] element is -1, it means that multiple-captures move has been finished and there can't be any moves anymore
//
//	*(2) if after capturing king chooses the spot, from which he cannot capture pieces anymore, 
// despite the fact that he could choose another place, from which he would be able to capture more pieces, the move is invalid,
// because the maximum number of pieces wasn't captured
//
//	*(3) King captures a lot of enemies and can't capture one more because of captured pieces left on board
//
//	*(4) Game is finished because player has all it's pieces stuck
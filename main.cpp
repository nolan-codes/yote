#include <cstdint>
#include <cstdio>

//VALUES
using Bitboard = uint32_t;

const Bitboard lEdge = 0b00'100000'100000'100000'100000'100000;
const Bitboard rEdge = 0b00'000001'000001'000001'000001'000001;

Bitboard wPieces = 0b00'1010101'101010'010101'101010'010101;
Bitboard bPieces = 0b00'000000'000000'000000'000000'000000;

uint8_t wHand = 12;
uint8_t bHand = 12;

bool wTurn = false;

enum MoveType : uint8_t {
    PLACE = 0,
    STEP,
    JUMP
};

struct Move {
    MoveType type;
    uint8_t start;
    uint8_t end;
    uint8_t other_captured;
};

struct MoveList {
    Move moves[1024]; //MAKE A FUNCTION TO TEST ALL POSSIBLE BOARD STATES AND THEIR LEGAL MOVE COUNTS AND THEN LOWER THAT UPPER BOUND
    uint8_t count = 0;

    void push(Move m) { moves[count++] = m; }
    Move operator[](int i) const { return moves[i]; }
};

//HELPFUL FUNCTIONS

inline bool wOccs(int coord) {return 1 & wPieces >> coord;}
inline bool bOccs(int coord) {return 1 & bPieces >> coord;}
inline bool onLE(int coord) {return 1 & lEdge >> coord;}
inline bool onRE(int coord) {return 1 & rEdge >> coord;}
inline bool empty(int coord) {return !onLE && !onRE; }

MoveList getMoves() {
    MoveList moves{};

    for (uint8_t square = 0; square < 30; ++square) {
        if (wOccs(square)) {
            if (wTurn) {
                if (!onRE(square)) {
                    if (empty(square+1)) moves.push({STEP, square, square+1, 0});
                    else if (bOccs(square+1) && !onRE(square+1) && empty(square+2)) moves.push({JUMP, square, square+2, 0}); //ACTUALLY PUSH ALL PICKUPS
                }
                if (!onLE(square)) {
                    if (empty(square-1)) moves.push({STEP, square, square-1, 0});
                    else if (bOccs(square-1) && !onLE(square-1) && empty(square-2)) moves.push({JUMP, square, square-2, 0}); //ACTUALLY PUSH ALL PICKUPS
                }
                if (square < 24) {
                    if (empty(square+6)) moves.push({STEP, square, square+6, 0});
                    else if (wOccs(square+6) && square+6 < 24 && empty(square+12)) moves.push({JUMP, square, square+6, 0}); //ACTUALLY PUSH ALL PICKUPS
                }
                if (square > 5) {
                    if (empty(square-6)) moves.push({STEP, square, square-6, 0});
                    else if (wOccs(square-6) && square-6 > 5 && empty(square-12)) {
                        moves.push({JUMP, square, square-12, 0}); //ACTUALLY PUSH ALL PICKUPS
                    }
                }
            }
        } else if (bOccs(square)) {
            if (!wTurn) {
                if (!onRE(square)) {
                    if (empty(square+1)) moves.push({STEP, square, square+1, 0});
                    else if (wOccs(square+1) && !onRE(square+1) && empty(square+2)) {
                        moves.push({JUMP, square, square+2, 0}); //ACTUALLY PUSH ALL PICKUPS
                    }
                }
                if (!onLE(square)) {
                    if (empty(square-1)) moves.push({STEP, square, square-1, 0});
                    else if (wOccs(square-1) && !onLE(square-1) && empty(square-2)) {
                        moves.push({JUMP, square, square-2, 0}); //ACTUALLY PUSH ALL PICKUPS
                    }
                }
                if (square < 24) {
                    if (empty(square+6)) moves.push({STEP, square, square+6, 0});
                    else if (wOccs(square+6) && square+6 < 24 && empty(square+12)) {
                        moves.push({JUMP, square, square+6, 0}); //ACTUALLY PUSH ALL PICKUPS
                    }
                }
                if (square > 5) {
                    if (empty(square-6)) moves.push({STEP, square, square-6, 0});
                    else if (wOccs(square-6) && square-6 > 5 && empty(square-12)) {
                        moves.push({JUMP, square, square-12, 0});
                    }
                }
            }
        } else {
            if ((wTurn && wHand > 0) || (!wTurn && bHand > 0)) moves.push({PLACE, 0, square, 0});
        }
    }

    return moves;
}

void showMoves() {
    MoveList moves = getMoves();
    for (int i = 0; i < moves.count; ++i) {
        Move move = moves[i];

        printf("TYPE: %d ", move.type);
        printf("SQUARE: %d", move.end);
        printf("\n");
    }
}

void showBoard() {
    for (int x = 0; x < 5; ++x) {
        for (int y = 0; y < 6; ++y) {
            auto square = " . ";
            if (wOccs(x*6+y)) square = " W ";
            if (bOccs(x*6+y)) square = " B ";

            printf("%s", square);
        }
        printf("\n");
    }
}

//MAIN
int main() {
    showBoard();
    showMoves();
    return 0;
}

//TODO
//PROVABLY LOWER MOVELIST UPPER BOUND
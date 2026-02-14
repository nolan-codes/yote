#include <cstdint>
#include <cstdio>

//VALUES
using Bitboard = uint32_t;

const Bitboard lEdge = 0b00'000001'000001'000001'000001'000001;
const Bitboard rEdge = 0b00'100000'100000'100000'100000'100000;

Bitboard wPieces = 0b00'100000'000000'000000'000000'000000;
Bitboard bPieces = 0b00'000000'100000'000000'000000'000000;

uint8_t wHand = 12;
uint8_t bHand = 12;

bool wTurn = true;

enum MoveType : uint8_t {
    PLACE = 0,
    STEP,
    JUMP
};

enum GameState : uint8_t {
    PLAYING = 0,
    DRAW,
    WWIN,
    BWIN
};

struct Move {
    MoveType type;
    uint8_t start;
    uint8_t end;
    uint8_t other;
};

struct MoveList {
    Move moves[1024]; //UPPER BOUND IS GENEROUS, ILL LOWER IT LATER
    int count = 0;

    void push(Move m) { moves[count++] = m; }
    Move operator[](int i) const { return moves[i]; }
};

//HELPFUL FUNCTIONS

inline bool wOccs(int coord) {return 1 & wPieces >> coord;}
inline bool bOccs(int coord) {return 1 & bPieces >> coord;}
inline bool onLE(int coord) {return 1 & lEdge >> coord;}
inline bool onRE(int coord) {return 1 & rEdge >> coord;}
inline bool empty(int coord) {return !wOccs(coord) && !bOccs(coord);}

inline void wSet(bool val, int coord) {wPieces ^= (-val ^ wPieces) & (1 << coord);}
inline void bSet(bool val, int coord) {bPieces ^= (-val ^ bPieces) & (1 << coord);}

MoveList getMoves() {
    MoveList moves{};

    for (uint8_t square = 0; square < 30; ++square) {
        if (wOccs(square)) {
            if (wTurn) {
                if (!onRE(square)) {
                    if (empty(square+1)) moves.push({STEP, square, (uint8_t)(square+1), 0});
                    else if (bOccs(square+1) && !onRE(square+1) && empty(square+2)) {
                        if (__builtin_popcount(bPieces) >= 2) {
                            for (uint8_t i=0; i<30; ++i) {
                                if (bOccs(i) && (i != square+2)) {
                                    moves.push({JUMP, square, (uint8_t)(square+2), i});
                                }
                            }
                        } else {
                            moves.push({JUMP, square, (uint8_t)(square+2), (uint8_t)(square+1)});
                        }
                    }
                }
                if (!onLE(square)) {
                    if (empty(square-1)) moves.push({STEP, square, (uint8_t)(square-1), 0});
                    else if (bOccs(square-1) && !onLE(square-1) && empty(square-2)) {
                        if (__builtin_popcount(bPieces) >= 2) {
                            for (uint8_t i=0; i<30; ++i) {
                                if (bOccs(i) && (i != square-2)) {
                                    moves.push({JUMP, square, (uint8_t)(square-2), i});
                                }
                            }
                        } else {
                            moves.push({JUMP, square, (uint8_t)(square-2), (uint8_t)(square-1)});
                        }
                    }
                }
                if (square < 24) {
                    if (empty(square+6)) moves.push({STEP, square, (uint8_t)(square+6), 0});
                    else if (bOccs(square+6) && square+6 < 24 && empty(square+12)) {
                        if (__builtin_popcount(bPieces) >= 2) {
                            for (uint8_t i=0; i<30; ++i) {
                                if (bOccs(i) && (i != square+12)) {
                                    moves.push({JUMP, square, (uint8_t)(square+12), i}); 
                                }
                            }
                        } else {
                            moves.push({JUMP, square, (uint8_t)(square+12), (uint8_t)(square+6)});
                        }
                    }
                }
                if (square > 5) {
                    if (empty(square-6)) moves.push({STEP, square, (uint8_t)(square-6), 0});
                    else if (bOccs(square-6) && square-6 > 5 && empty(square-12)) {
                        if (__builtin_popcount(bPieces) >= 2) {
                            for (uint8_t i=0; i<30; ++i) {
                                if (bOccs(i) && (i != square-12)) {
                                    moves.push({JUMP, square, (uint8_t)(square-12), i}); 
                                }
                            }
                        } else {
                            moves.push({JUMP, square, (uint8_t)(square-12), (uint8_t)(square-6)});
                        }
                    }
                }
            }
        } else if (bOccs(square)) {
            if (!wTurn) {
                if (!onRE(square)) {
                    if (empty(square+1)) moves.push({STEP, square, (uint8_t)(square+1), 0});
                    else if (wOccs(square+1) && !onRE(square+1) && empty(square+2)) {
                        if (__builtin_popcount(wPieces) >= 2) {
                            for (uint8_t i=0; i<30; ++i) {
                                if (wOccs(i) && (i != square+2)) {
                                    moves.push({JUMP, square, (uint8_t)(square+2), i}); 
                                }
                            }
                        } else {
                            moves.push({JUMP, square, (uint8_t)(square+2), (uint8_t)(square+1)});
                        }
                    }
                }
                if (!onLE(square)) {
                    if (empty(square-1)) moves.push({STEP, square, (uint8_t)(square-1), 0});
                    else if (wOccs(square-1) && !onLE(square-1) && empty(square-2)) {
                        if (__builtin_popcount(wPieces) >= 2) {
                            for (uint8_t i=0; i<30; ++i) {
                                if (wOccs(i) && (i != square-2)) {
                                    moves.push({JUMP, square, (uint8_t)(square-2), i}); 
                                }
                            }
                        } else {
                            moves.push({JUMP, square, (uint8_t)(square-2), (uint8_t)(square-1)});
                        }
                    }
                }
                if (square < 24) {
                    if (empty(square+6)) moves.push({STEP, square, (uint8_t)(square+6), 0});
                    else if (wOccs(square+6) && square+6 < 24 && empty(square+12)) {
                        if (__builtin_popcount(wPieces) >= 2) {
                            for (uint8_t i=0; i<30; ++i) {
                                if (wOccs(i) && (i != square+12)) {
                                    moves.push({JUMP, square, (uint8_t)(square+12), i}); 
                                }
                            }
                        } else {
                            moves.push({JUMP, square, (uint8_t)(square+12), (uint8_t)(square+6)});
                        }
                    }
                }
                if (square > 5) {
                    if (empty(square-6)) moves.push({STEP, square, (uint8_t)(square-6), 0});
                    else if (wOccs(square-6) && square-6 > 5 && empty(square-12)) {
                        if (__builtin_popcount(wPieces) >= 2) {
                            for (uint8_t i=0; i<30; ++i) {
                                if (wOccs(i) && (i != square-12)) {
                                    moves.push({JUMP, square, (uint8_t)(square-12), i}); 
                                }
                            }
                        } else {
                            moves.push({JUMP, square, (uint8_t)(square-12), (uint8_t)(square-6)});
                        }
                    }
                }
            }
        } else {
            if ((wTurn && wHand > 0) || (!wTurn && bHand > 0)) moves.push({PLACE, 0, square, 0});
        }
    }

    return moves;
}

GameState getState() { //if white has 
    if (__builtin_popcount(wPieces) == 0 && wHand == 0) {
        return BWIN;
    } else if (__builtin_popcount(bPieces) == 0 && bHand == 0) {
        return WWIN;
    } else if (__builtin_popcount(wPieces) + wHand <= 3 && __builtin_popcount(bPieces) + bHand <= 3) {
        return DRAW;
    } else return PLAYING;
}

void applyMove(Move move) {
    switch (move.type) {
        case PLACE:
            if (wTurn) {
                wSet(1, move.end);
                wHand--;
            } else {
                bSet(1, move.end);
                bHand--;
            }
            break;

        case STEP:
            if (wTurn) {
                wSet(0, move.start);
                wSet(1, move.end);
            } else {           
                bSet(0, move.start);
                bSet(1, move.end);
            }
            break;

        case JUMP:
            if (wTurn) {
                wSet(0, move.start);
                wSet(1, move.end);
                bSet(0, (move.start + move.end)/2);
                bSet(0, move.other);
            } else {
                bSet(0, move.start);
                bSet(1, move.end);     
                wSet(0, (move.start + move.end)/2);
                wSet(0, move.other);
            }
            break;
    }
}

//When undoing, wTurn must be the same as when the move was applied
void undoMove(Move move) {
    switch (move.type) {
        case PLACE:
            if (wTurn) {
                wSet(0, move.end);
                wHand++;
            } else {
                bSet(0, move.end);
                bHand++;
            }
            break;

        case STEP:
            if (wTurn) {
                wSet(1, move.start);
                wSet(0, move.end);
            } else {           
                bSet(1, move.start);
                bSet(0, move.end);
            }
            break;

        case JUMP:
            if (wTurn) {
                wSet(1, move.start);
                wSet(0, move.end);
                bSet(1, (move.start + move.end)/2);
                bSet(1, move.other);
            } else {
                bSet(1, move.start);
                bSet(0, move.end);     
                wSet(1, (move.start + move.end)/2);
                wSet(1, move.other);
            }
            break;
    }
}

void showMoves() {
    MoveList moves = getMoves();
    for (int i = 0; i < moves.count; ++i) {
        Move move = moves[i];

        if (move.type != PLACE) {
            printf("TYPE: %d ", move.type);
            printf("SQUARE: %d", move.end);
            printf("\n");
        }
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

    applyMove({PLACE, 0, 17, 0});
    showBoard();
    showMoves();

    undoMove({PLACE, 0, 17, 0});
    showBoard();
    showMoves();

    return 0;
}

//TODO
//PROVABLY LOWER MOVELIST UPPER BOUND
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <ctype.h>

using Bitboard = uint32_t;

const Bitboard board = 0b00'111111'111111'111111'111111'111111;

// GAME VALUES //////////////////////////////////////////////////
Bitboard wPieces = 0b00'000000'000000'000000'000000'000000;
Bitboard bPieces = 0b00'000000'000000'000000'000000'000000;

uint8_t wHand = 12;
uint8_t bHand = 12;

bool wTurn = true;
/////////////////////////////////////////////////////////////////

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

struct BoardState {
    bool wTurn;
    uint8_t wHand;
    uint8_t bHand;
    Bitboard wPieces;
    Bitboard bPieces;
};

constexpr void prebuildMoves(Bitboard table[30][4], int dist) {
    for (int i=0; i<30; ++i) {
        int x = i%6;
        int y = i/6;

        table[i][0] = (x+dist <  6) ? (1u << (i + 1*dist)) : 0u;
        table[i][1] = (x-dist >= 0) ? (1u << (i - 1*dist)) : 0u;
        table[i][2] = (y+dist <  5) ? (1u << (i + 6*dist)) : 0u;
        table[i][3] = (y-dist >= 0) ? (1u << (i - 6*dist)) : 0u;
    }
}

Bitboard steps[30][4];
Bitboard jumps[30][4];

inline bool wOccs(int coord) {return 1u & wPieces >> coord;}
inline bool bOccs(int coord) {return 1u & bPieces >> coord;}

inline void wSet(bool val, int coord) {wPieces ^= (-val ^ wPieces) & (1u << coord);}
inline void bSet(bool val, int coord) {bPieces ^= (-val ^ bPieces) & (1u << coord);}

MoveList getMovesWhite() {
    MoveList moves{};

    Bitboard unocc = ~(bPieces | wPieces) & board;

    for (uint32_t bb = wPieces; bb; bb &= bb - 1) {
        uint8_t square = __builtin_ctz(bb);

        for (int i=0; i<4; ++i) {
            if (unocc & steps[square][i]) moves.push({STEP, square, (uint8_t)(__builtin_ctz(steps[square][i])), 0});
            else if ((bPieces & steps[square][i]) && (unocc & jumps[square][i])) 
                for (uint32_t cap = bPieces; cap; cap &= cap - 1) 
                    moves.push({JUMP, square, (uint8_t)__builtin_ctz(jumps[square][i]), (uint8_t)__builtin_ctz(cap)});
        }
    }

    if (wHand) {
        while (unocc) {
            moves.push({PLACE, 0, (uint8_t)__builtin_ctz(unocc), 0});
            unocc &= (unocc - 1);
        }
    }

    return moves;
}

MoveList getMovesBlack() {
    MoveList moves{};

    Bitboard unocc = ~(bPieces | wPieces) & board;

    for (uint32_t bb = bPieces; bb; bb &= bb - 1) {
        uint8_t square = __builtin_ctz(bb);

        for (int i=0; i<4; ++i) {
            if (unocc & steps[square][i]) moves.push({STEP, square, (uint8_t)(__builtin_ctz(steps[square][i])), 0});
            else if ((wPieces & steps[square][i]) && (unocc & jumps[square][i])) 
                for (uint32_t cap = wPieces; cap; cap &= cap - 1) 
                    moves.push({JUMP, square, (uint8_t)__builtin_ctz(jumps[square][i]), (uint8_t)__builtin_ctz(cap)});
        }
    }

    if (bHand) {
        while (unocc) {
            moves.push({PLACE, 0, (uint8_t)__builtin_ctz(unocc), 0});
            unocc &= (unocc - 1);
        }
    }

    return moves;
}

MoveList getMoves() {
    return wTurn ? getMovesWhite() : getMovesBlack();
}

int countMovesWhite() {
    int count = 0;
    
    Bitboard unocc = ~(bPieces | wPieces) & board;

    for (uint32_t bb = wPieces; bb; bb &= bb - 1) {
        uint8_t square = __builtin_ctz(bb);

        for (int i=0; i<4; ++i) {
            if (unocc & steps[square][i]) count++;
            else if ((bPieces & steps[square][i]) && (unocc & jumps[square][i])) count += __builtin_popcount(bPieces);
        }
    }

    if (wHand) count += __builtin_popcount(unocc);

    return count;
}

int countMovesBlack() {
    int count = 0;
    
    Bitboard unocc = ~(bPieces | wPieces) & board;

    for (uint32_t bb = bPieces; bb; bb &= bb - 1) {
        uint8_t square = __builtin_ctz(bb);

        for (int i=0; i<4; ++i) {
            if (unocc & steps[square][i]) count++;
            else if ((wPieces & steps[square][i]) && (unocc & jumps[square][i])) count += __builtin_popcount(wPieces);
        }
    }

    if (bHand) count += __builtin_popcount(unocc);

    return count;
}

int countMoves() {
    return wTurn ? countMovesWhite() : countMovesBlack();
}

GameState getState() {
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

// When undoing, wTurn must be the same as when the move was applied
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

BoardState saveBoard() {
    return {wTurn, wHand, bHand, wPieces, bPieces};
}

int scoreBoard() {
    int mate = 1500;

    switch (getState()) {
        case PLAYING: {
            int wScore = __builtin_popcount(wPieces) * 100 + wHand * 50;
            int bScore = __builtin_popcount(bPieces) * 100 + bHand * 50;
            return wScore - bScore;
        }
        case WWIN: return mate;
        case BWIN: return -mate;
        default: return 0;
    }
}

void loadBoard(BoardState state) {
    wTurn = state.wTurn;
    wHand = state.wHand;
    bHand = state.bHand;
    wPieces = state.wPieces;
    bPieces = state.bPieces;
}

uint8_t coordToIndex(char* coord) {
    int x = coord[0] - 'A';
    int y = coord[1] - '1';
    return (uint8_t)(y*6+x);
}

void indexToCoord(uint8_t index, char coord[3]) {
    int x = index % 6;
    int y = index / 6;

    coord[0] = 'A' + x;
    coord[1] = '1' + y;
    coord[2] = '\0'; 
}

Move stringToMove(const char* string) {
    Move move{};
    char startCoord[3] = "A1";
    char endCoord[3]   = "A1";
    char otherCoord[3] = "A1";

    if (strncmp(string, "PLACE ", 6) == 0) {
        move.type = PLACE;
        sscanf(string + 6, "%2s", endCoord);
    } else if (strncmp(string, "STEP ", 5) == 0) {
        move.type = STEP;
        sscanf(string + 5, "%2s %2s", startCoord, endCoord);
    } else if (strncmp(string, "JUMP ", 5) == 0) {
        move.type = JUMP;
        sscanf(string + 5, "%2s %2s TAKE %2s", startCoord, endCoord, otherCoord);
    }

    move.start = coordToIndex(startCoord);
    move.end   = coordToIndex(endCoord);
    move.other = coordToIndex(otherCoord);

    return move;
}

void printMove(Move move) {
    char start[3] = {0};
    char end[3]   = {0};
    char other[3] = {0};

    switch (move.type) {
        case PLACE:
            indexToCoord(move.end, end);
            printf("PLACE %s", end);
            break;

        case STEP:
            indexToCoord(move.start, start);
            indexToCoord(move.end, end);
            printf("STEP %s TO %s", start, end);
            break;

        case JUMP:
            indexToCoord(move.start, start);
            indexToCoord(move.end, end);
            indexToCoord(move.other, other);
            printf("JUMP %s TO %s TAKE %s", start, end, other);
            break;
    }
}

void showMoves() {
    MoveList moves = getMoves();
    for (int i = 0; i < moves.count; ++i) {
        Move move = moves[i];
        printMove(move);
        printf("\n");
    }
}

void showBoard() {
    printf("\n   A  B  C  D  E  F  \n");
    for (int y = 0; y < 5; ++y) {
        printf("%d ", y+1);
        for (int x = 0; x < 6; ++x) {
            auto square = ".";
            if (wOccs(y*6+x)) square = "W";
            if (bOccs(y*6+x)) square = "B";

            printf(" %s ", square);
        }
        printf("\n");
    }
    printf("\n");
}

//MAIN
int main() {
    prebuildMoves(steps, 1);
    prebuildMoves(jumps, 2);

    while (getState() == PLAYING) {
        printf("\033[2J\033[H");
        printf("Turn: %s\n", wTurn ? "WHITE" : "BLACK");
        printf("%d possible moves\n", countMoves());

        showBoard();

        MoveList moves = getMoves();
        printf("Move format:\n");
        printf("  PLACE XY\n");
        printf("  STEP XY XY\n");
        printf("  JUMP XY XY TAKE XY\n");
        printf("  X is A-F, Y is 1-5.\n");
        
        if (moves.count == 0) break;

        char input[128] = {0};
        printf("\nEnter move (or QUIT): ");
        if (!fgets(input, sizeof(input), stdin)) break;
        input[strcspn(input, "\n")] = '\0';

        for (int i = 0; input[i]; i++) input[i] = toupper(input[i]);

        if (strcmp(input, "QUIT") == 0) break;

        Move parsed = stringToMove(input);

        bool found = false;
        for (int i = 0; i < moves.count; ++i) {
            Move m = moves[i];
            if (m.type == parsed.type && 
                m.start == parsed.start &&
                m.end == parsed.end &&
                m.other == parsed.other) {
                found = true;
                break;
            }
        }

        if (!found) {
            printf("Illegal move, press enter.");
            fgets(input, sizeof(input), stdin);
            continue;
        }

        applyMove(parsed);
        wTurn = !wTurn;
    }

    printf("\033[2J\033[H");

    switch (getState()) {
        case DRAW:
            printf("It's a draw!");
            break;
        case WWIN:
            printf("White wins!");
            break;
        case BWIN:
            printf("Black wins!");
            break;
    }

    showBoard();

    return 0;
}

//TODO
//PROVABLY LOWER MOVELIST UPPER BOUND

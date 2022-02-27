/*
 *----------------------------------------
 * Program Name: TI-84 Wordle
 * Author:       Blackgaurdian3
 * License:
 * Description:  Wordle for TI-84 Plus CE
 *----------------------------------------
 */

//! letters will be lowercase
// TODO: switch letter handling from os to graphx

#include <graphx.h>
#include <keypadc.h>
#include <stdint.h>
#include <tice.h>

#include "wordlist.h"

typedef uint8_t color;
typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;

// constants
#define SIZE 35
#define GAP 2
#define ROWS 6
#define COLS 5
const u16 START_X = (LCD_WIDTH - COLS * SIZE - GAP * (COLS - 1)) / 2;
const u16 START_Y = (LCD_HEIGHT - ROWS * SIZE - GAP * (ROWS - 1)) / 2;

// color codes
const color LIGHT_GRAY = 0xde;  // kinda looks blue im no artist
const color GRAY = 0xb5;
const color DARK_GRAY = 0x4a;
const color RED = 0xe0;
const color YELLOW = 0xe5;
const color GREEN = 0x04;
const color BLACK = 0x00;
const color WHITE = 0xff;

// grid & wordle variables
u8 rowPtr = 0, colPtr = 0;
char answer[COLS + 1] = {'\0'}, guess[COLS + 1] = {'\0'};

// game messages
const char *YOU_WIN = "YOU WIN!";
const char *YOU_LOSE = "YOU LOSE!";
const char *PLAY_AGAIN = "Press \"enter\" to play again";
char *reveal = "The word was:      ";

// keypress safety
char prevkey;

// game loop
u8 gameRunning = 1;

// helper functions
void copyStr(char *dest, char *src) {
    u8 i;
    for (i = 0; i < COLS; i++) {
        dest[i] = src[i];
    }
}
u8 inStr(char ch, char *str) {
    u8 i;
    for (i = 0; i < COLS; i++) {
        if (str[i] == ch) return 1;
    }
    return 0;
}
s8 strCmp(char *str1, char *str2) {
    // returns:
    // 1 if str1 > str2
    // 0 if str1 == str2
    // -1 if str1 < str2
    u8 i;
    for (i = 0; i < COLS; i++) {
        if (str1[i] > str2[i]) return 1;
        if (str1[i] < str2[i]) return -1;
    }
    return 0;
}
void revealAnswer() {
    for (u8 i = 0; i < COLS; i++) {
        reveal[i + 14] = answer[i] - 'a' + 'A';
    }
}

// render functions
void clearScreenSafe(color clr) {
    gfx_SetColor(clr);
    gfx_FillRectangle(0, 0, LCD_WIDTH, LCD_HEIGHT);
}
void drawGrid() {
    gfx_SetColor(GRAY);

    // for some reason x is across and y is down
    // opposite to how 2d arrays are usually done
    u8 i, j;
    for (i = 0; i < COLS; i++) {
        for (j = 0; j < ROWS; j++) {
            gfx_Rectangle(START_X + i * (SIZE + GAP), START_Y + j * (SIZE + GAP), SIZE, SIZE);
        }
    }
}
void clearSquare(u8 x, u8 y) {
    u8 posx = START_X + x * (SIZE + GAP), posy = START_Y + y * (SIZE + GAP);
    gfx_SetColor(WHITE);
    gfx_FillRectangle(posx, posy, SIZE, SIZE);
    gfx_SetColor(GRAY);
    gfx_Rectangle(posx, posy, SIZE, SIZE);
}
void fillSquare(u8 x, u8 y, color clr) {
    u8 posx = START_X + x * (SIZE + GAP), posy = START_Y + y * (SIZE + GAP);
    gfx_SetColor(clr);
    gfx_FillRectangle(posx, posy, SIZE, SIZE);
    gfx_SetColor(GRAY);
    gfx_Rectangle(posx, posy, SIZE, SIZE);
}
void dispLetter(char letter, u8 x, u8 y) {
    u8 posx = START_X + x * (SIZE + GAP), posy = START_Y + y * (SIZE + GAP);

    char nullTerminated[2] = {letter, '\0'};
    os_FontDrawText(nullTerminated, (posx) / 2 + 3, (posy) / 2 + 2);

    // cover edges with rectangle
    gfx_SetColor(GRAY);
    gfx_Rectangle(posx, posy, SIZE, SIZE);
}
void winLossDisplay(u8 pos) {
    // pos: 0 is top, 1 is bottom
    const u16 width = gfx_GetStringWidth(PLAY_AGAIN) + 50;
    const u16 height = 100;
    const u8 x = (LCD_WIDTH - width) / 2, y = (pos ? 0 : LCD_HEIGHT / 2) + (LCD_HEIGHT / 2 - height) / 2;

    // draw outline
    gfx_SetColor(BLACK);
    gfx_Rectangle(x - 1, y - 1, width + 2, height + 2);
    gfx_Rectangle(x - 2, y - 2, width + 4, height + 4);
    // fill in
    gfx_SetColor(LIGHT_GRAY);
    gfx_FillRectangle(x, y, width, height);
}

// wordle functions
void enterLetter(char letter) {
    if (colPtr == COLS) return;
    dispLetter(letter, colPtr, rowPtr);
    guess[colPtr] = letter;
    colPtr++;
}
void backspace() {
    if (colPtr == 0) return;
    colPtr--;
    clearSquare(colPtr, rowPtr);
}
char handleKeys() {
    kb_Scan();
    if (kb_Data[2] & kb_Math)
        return 'a';
    else if (kb_Data[3] & kb_Apps)
        return 'b';
    else if (kb_Data[4] & kb_Prgm)
        return 'c';
    else if (kb_Data[2] & kb_Recip)
        return 'd';  //! assuming this is the x^-1 key
    else if (kb_Data[3] & kb_Sin)
        return 'e';
    else if (kb_Data[4] & kb_Cos)
        return 'f';
    else if (kb_Data[5] & kb_Tan)
        return 'g';
    else if (kb_Data[6] & kb_Power)
        return 'h';
    else if (kb_Data[2] & kb_Square)
        return 'i';
    else if (kb_Data[3] & kb_Comma)
        return 'j';
    else if (kb_Data[4] & kb_LParen)
        return 'k';
    else if (kb_Data[5] & kb_RParen)
        return 'l';
    else if (kb_Data[6] & kb_Div)
        return 'm';
    else if (kb_Data[2] & kb_Log)
        return 'n';
    else if (kb_Data[3] & kb_7)
        return 'o';
    else if (kb_Data[4] & kb_8)
        return 'p';
    else if (kb_Data[5] & kb_9)
        return 'q';
    else if (kb_Data[6] & kb_Mul)
        return 'r';
    else if (kb_Data[2] & kb_Ln)
        return 's';
    else if (kb_Data[3] & kb_4)
        return 't';
    else if (kb_Data[4] & kb_5)
        return 'u';
    else if (kb_Data[5] & kb_6)
        return 'v';
    else if (kb_Data[6] & kb_Sub)
        return 'w';
    else if (kb_Data[2] & kb_Sto)
        return 'x';
    else if (kb_Data[3] & kb_1)
        return 'y';
    else if (kb_Data[4] & kb_2)
        return 'z';
    else if (kb_Data[1] & kb_Del)
        return '\b';
    else if (kb_Data[6] & kb_Enter)
        return '\n';
    else if (kb_Data[6] & kb_Clear)
        gameRunning = 0;
    return '\0';
}
u8 inDictionary(char *str) {
    // based on assumption that dictionary
    // word list is sorted lexicographically
    // O(log(DICTIONARY_LEN) * COLS)
    // ~60 operations per word
    u16 lo = 0, hi = DICTIONARY_LEN - 1;
    while (lo <= hi) {
        u16 mid = (lo + hi) / 2;
        switch (strCmp(str, DICTIONARY[mid])) {
            case 1:  // str1 > str2
                lo = mid + 1;
                break;
            case 0:  // str1 == str2
                return 1;
            case -1:  // str1 < str2
                hi = mid - 1;
                break;
        }
    }
    return 0;
}
u8 inSelected(char *str) {
    u16 lo = 0, hi = SELECTED_LEN - 1;
    while (lo <= hi) {
        u16 mid = (lo + hi) / 2;
        switch (strCmp(str, SELECTED[mid])) {
            case 1:  // str1 > str2
                lo = mid + 1;
                break;
            case 0:  // str1 == str2
                return 1;
            case -1:  // str1 < str2
                hi = mid - 1;
                break;
        }
    }
    return 0;
}
void resetGame() {
    clearScreenSafe(WHITE);
    drawGrid();
    copyStr(answer, SELECTED[randInt(0, SELECTED_LEN - 1)]);
    rowPtr = colPtr = 0;
#ifdef DEBUG
    char nT[6];
    for (u8 i = 0; i < COLS; i++) {
        nT[i] = answer[i];
    }
    nT[COLS] = '\0';
    os_FontDrawText(nT, 0, 0);
#endif
}
void gameWin() {
    u8 pos = (rowPtr >= ROWS / 2);
    winLossDisplay(pos);

    gfx_SetTextFGColor(GREEN);
    gfx_SetTextScale(2, 2);
    gfx_PrintStringXY(YOU_WIN, (LCD_WIDTH - gfx_GetStringWidth(YOU_WIN)) / 2, LCD_HEIGHT / 4 - 16 + (pos ? 0 : LCD_HEIGHT / 2));
    gfx_SetTextFGColor(BLACK);
    gfx_SetTextScale(1, 1);
    gfx_PrintStringXY(PLAY_AGAIN, (LCD_WIDTH - gfx_GetStringWidth(PLAY_AGAIN)) / 2, LCD_HEIGHT / 4 + 6 + (pos ? 0 : LCD_HEIGHT / 2));
    char ch;
    while (1) {
        ch = handleKeys();
        if (ch != prevkey && ch == '\n') {
            resetGame();
            break;
        }
        if (!gameRunning) break;
        prevkey = ch;
    }
}
void gameLoss() {
    winLossDisplay(1);

    gfx_SetTextFGColor(RED);
    gfx_SetTextScale(2, 2);
    gfx_PrintStringXY(YOU_LOSE, (LCD_WIDTH - gfx_GetStringWidth(YOU_LOSE)) / 2, LCD_HEIGHT / 4 - 20);
    gfx_SetTextFGColor(BLACK);
    gfx_SetTextScale(1, 1);
    revealAnswer();
    gfx_PrintStringXY(reveal, (LCD_WIDTH - gfx_GetStringWidth(reveal)) / 2, LCD_HEIGHT / 4 + 3);
    gfx_PrintStringXY(PLAY_AGAIN, (LCD_WIDTH - gfx_GetStringWidth(PLAY_AGAIN)) / 2, LCD_HEIGHT / 4 + 20);
    char ch;
    while (1) {
        ch = handleKeys();
        if (ch != prevkey && ch == '\n') {
            resetGame();
            break;
        }
        if (!gameRunning) break;
        prevkey = ch;
    }
}
void evaluateGuess() {
    if (!inSelected(guess) && !inDictionary(guess)) {
        // ~55 + ~60 = ~115 operations
        return;
    }

    u8 i, correct = 0, ansFreq[26];
    for (i = 0; i < 26; i++) ansFreq[i] = 0;
    for (i = 0; i < COLS; i++) {
        ansFreq[answer[i] - 'a']++;
    }

    // first scan: check for correct placement
    for (i = 0; i < COLS; i++) {
        if (guess[i] == answer[i]) {
            correct++;
            ansFreq[guess[i] - 'a']--;
            fillSquare(i, rowPtr, GREEN);
        }
    }

    // second scan: check for wrong placement and wrong letters
    for (i = 0; i < COLS; i++) {
        if (guess[i] == answer[i]) {
            dispLetter(guess[i], i, rowPtr);
            continue;
        }
        if (ansFreq[guess[i] - 'a'] > 0 && inStr(guess[i], answer)) {
            ansFreq[guess[i] - 'a']--;
            fillSquare(i, rowPtr, YELLOW);
        } else {
            fillSquare(i, rowPtr, DARK_GRAY);
        }
        dispLetter(guess[i], i, rowPtr);
    }

    rowPtr++;
    colPtr = 0;

    if (correct == COLS) {
        gameWin();
    } else if (rowPtr == ROWS) {
        gameLoss();
    }
}

void init() {
    os_FontSelect(os_LargeFont);
    srandom(rtc_Time());
    resetGame();
}
void mainGame() {
    char ch = handleKeys();
    if (ch == prevkey) return;
    prevkey = ch;

    if (ch == '\0') return;
    if (ch == '\n') {
        if (colPtr == COLS) {
            evaluateGuess();
        }
    } else if (ch == '\b') {
        backspace();
    } else {
        enterLetter(ch);
    }
}
int main(void) {
    gfx_Begin();
    init();

    do {
        mainGame();
    } while (gameRunning);

    gfx_End();
    return 0;
}

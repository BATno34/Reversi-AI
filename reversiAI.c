#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

bool isAvailable (char check, char oppose, int initialRow, int initialCol, int row, int col, int Size, char board[Size][Size], int rowDir, int colDir);
bool checkValidity (char check, int row, int col, int Size, char board[Size][Size]);
int checkScore (char check, char oppose, int row, int col, int Size, char board[Size][Size]);
void boardConfigurate (char check, char oppose, int row, int col, int size, char *pboard);
int makeMove (const char board[][26], int n, char turn, int *row, int *col);
int checkMove (char check, char oppose, int row, int col, int Size, char board[Size][Size]);
int countMove (char check, char oppose, int row, int col, int Size, char board[Size][Size]);
int checkStability (char check, char oppose, int row, int col, int Size, char board[Size][Size]);
bool isFound (char check, char lookFor, int row, int col, int rowDir, int colDir, int Size, char board[Size][Size]);

char black = 'B';
char white = 'W';
char empty = 'U';

bool isAvailable (char check, char oppose, int initialRow, int initialCol, int row, int col, int Size, char newBoard[Size][Size], int rowDir, int colDir){
    //printTheBoard (Size, &newBoard[0][0]);
    row += rowDir;
    col += colDir;

    if (row >= 0 && row < Size && col >= 0 && col < Size){
        if (newBoard[row][col] == oppose)     //saws an opposition sign which is what valid
            return isAvailable (check, oppose, initialRow, initialCol, row, col, Size, newBoard, rowDir, colDir);     //continue until it ends
        else if (newBoard[row][col] == check && initialRow + rowDir == row && initialCol + colDir == col)     //when two characters next to each other are the same, which is invalid
            return false;
        else if (newBoard[row][col] == check)     //the end of the validity
            return true;
        else if (newBoard[row][col] == empty)     //next to the empty square is another empty which is not what we're looking for
            return false;
    }
    
    return false;
}

bool checkValidity (char check, int row, int col, int Size, char board[Size][Size]){
    
    bool valid = false;
    char oppose;
    if (check == black)
        oppose = white;
    else if (check == white)
        oppose = black;

    if (row >= Size || col >= Size || (check != black && check != white) || board[row][col] != empty)     //if the input itself had problem
        return false;
    else{
        //check all directions for validity
        for (int rowDir = -1; rowDir <= 1; rowDir++){
            for (int colDir = -1; colDir <= 1; colDir++){
                if (!(rowDir == 0 && rowDir == colDir) && !valid){
                    valid = isAvailable (check, oppose, row, col, row, col, Size, board, rowDir, colDir);
                }
            }
        }
    }

    return valid;
}

int checkScore (char check, char oppose, int row, int col, int Size, char board[Size][Size]){

    int score = 1;
    int initialRow = row;
    int initialCol = col;
    int emptySpots = 1;
    for (int i = 0; i < Size; i++){
        for (int j = 0; j < Size; j++){
            if (board[i][j] == empty)
                emptySpots++;
        }
    }
    
    for (int rowDir = -1; rowDir <= 1; rowDir++){
        for (int colDir = -1; colDir <= 1; colDir++){
            if (!(rowDir == 0 && rowDir == colDir) && isAvailable (check, oppose, row, col, row, col, Size, board, rowDir, colDir)){
                row += rowDir;
                col += colDir;

                while (board[row][col] == oppose){
                    score++;
                    row += rowDir;
                    col += colDir;
                }

                row = initialRow;
                col = initialCol;
            }
        }
    }

    if (emptySpots < (Size * Size / 8))
        return (200 * score);
    else if (emptySpots < (Size * Size / 4))
        return (80 * score);
    else if (emptySpots < (Size * Size / 2))
        return (20 * score);
    else
        return (10 * score);
}

void boardConfigurate (char check, char oppose, int row, int col, int size, char *pboard){
    
    *(pboard + (row * size + col)) = check;
    int initialRow = row;
    int initialCol = col;
    char temp[size][size];

    for (int rowDir = -1; rowDir <= 1; rowDir++){
        for (int colDir = -1; colDir <= 1; colDir++){

            for (int r = 0; r < size; r++){
                for (int c = 0; c < size; c++){
                    temp[r][c] = *(pboard + (r * size + c));
                }
            }
            
            if (!(rowDir == 0 && rowDir == colDir) && isAvailable (check, oppose, row, col, row, col, size, temp, rowDir, colDir)){
                row += rowDir;
                col += colDir;

                while (*(pboard + (row * size + col)) == oppose){
                    *(pboard + (row * size + col)) = check;
                    row += rowDir;
                    col += colDir;
                }

                row = initialRow;
                col = initialCol;
            }
        }
    }

}

int makeMove (const char board[][26], int n, char turn, int *row, int *col){
    
    int scoreCheck = 0, moveCheck = 0, playerMoveAvailable = 0, stabilityCheck = 0;
    int highestScore = -5000;
    int totalScore;
    char oppose;
    if (turn == black)
        oppose = white;
    else
        oppose = black;
    
    char temp[n][n];
    for (int r = 0; r < n; r++){
        for (int c = 0; c < n; c++){
            temp[r][c] = board[r][c];
        }
    }

    for (int r = 0; r < n; r++){
        for (int c = 0; c < n; c++){
            if (checkValidity(turn, r, c, n, temp)){
                scoreCheck = checkScore (turn, oppose, r, c, n, temp);
                moveCheck = checkMove (turn, oppose, r, c, n, temp);
                playerMoveAvailable = countMove (turn, oppose, r, c, n, temp);
                stabilityCheck = checkStability (turn, oppose, r, c, n, temp);
                if ((r == 0 || r == n - 1) && (c == 0 || c == n - 1)){
                    scoreCheck += 600;
                }
                else if (r == 0 || r == n - 1 || c == 0 || c == n - 1){
                    scoreCheck += 100;
                }
                else if ((r == 1 || r == n - 2) && (c == 0 || c == 1 || c == n - 2 || c == n - 1)){
                    scoreCheck -= 50;
                }
                else if ((c == 1 || c == n - 2) && (r == 0 || r == 1 || r == n - 2 || r == n - 1)){
                    scoreCheck -= 50;
                }
                else if ((r + c == n - 1) || (r == c)){
                    scoreCheck += 30;
                }

                totalScore = scoreCheck + (stabilityCheck) - moveCheck - playerMoveAvailable;
                if (totalScore >= highestScore){
                    *row = r;
                    *col = c;
                    highestScore = totalScore;
                }
            }
        }
    }
    
    return 0;
}

int checkMove (char check, char oppose, int row, int col, int Size, char board[Size][Size]){

    int scoreCheck = 0;
    int highestScore = 0;
    int emptySpots = -1;
    char boardCopy[Size][Size];
    for (int i = 0; i < Size; i++){
        for (int j = 0; j < Size; j++){
            boardCopy[i][j] = board[i][j];
            if (boardCopy[i][j] == empty)
                emptySpots++;
        }
    }
    boardConfigurate (check, oppose, row, col, Size, &boardCopy[0][0]);

    for (int r = 0; r < Size; r++){
        for (int c = 0; c < Size; c++){
            if (checkValidity(oppose, r, c, Size, boardCopy)){
                scoreCheck = checkScore (oppose, check, r, c, Size, boardCopy);
                if ((r == 0 || r == Size - 1) && (c == 0 || c == Size - 1)){
                    scoreCheck += 600;
                }
                else if (r == 0 || r == Size - 1 || c == 0 || c == Size - 1){
                    scoreCheck += 100;
                }
                else if ((r == 1 || r == Size - 2) && (c == 0 || c == 1 || c == Size - 2 || c == Size - 1)){
                    scoreCheck -= 50;
                }
                else if ((c == 1 || c == Size - 2) && (r == 0 || r == 1 || r == Size - 2 || r == Size - 1)){
                    scoreCheck -= 50;
                }
                else if ((r + c == Size - 1) || (r == c)){
                    scoreCheck += 30;
                }
                
                if (scoreCheck >= highestScore){
                    highestScore = scoreCheck;
                }
            }
        }
    }

    if (emptySpots < (Size * Size / 8))
        return (4 * highestScore);
    else if (emptySpots < (Size * Size / 4))
        return (3 * highestScore);
    else if (emptySpots < (Size * Size / 2))
        return (2 * highestScore);
    else
        return (1 * highestScore);
}

int countMove (char check, char oppose, int row, int col, int Size, char board[Size][Size]){

    int count = 0;
    int emptySpots = 1;
    char boardCopy[Size][Size];
    for (int i = 0; i < Size; i++){
        for (int j = 0; j < Size; j++){
            boardCopy[i][j] = board[i][j];
            if (board[i][j] == empty)
                emptySpots++;
        }
    }
    boardConfigurate (check, oppose, row, col, Size, &boardCopy[0][0]);

    for (int r = 0; r < Size; r++){
        for (int c = 0; c < Size; c++){
            if (checkValidity(oppose, r, c, Size, boardCopy)){
                count++;
            }
        }
    }

    if (emptySpots < (Size * Size / 16))
        return (150 * count);
    else if (emptySpots < (Size * Size / 8))
        return (120 * count);
    else if (emptySpots < (Size * Size / 4))
        return (100 * count);
    else if (emptySpots < (Size * Size / 2))
        return (20 * count);
    else
        return (10 * count);
}

int checkStability (char check, char oppose, int row, int col, int Size, char board[Size][Size]){

    int score = 0;
    int count = 0;
    int emptySpots = 1;
    char boardCopy[Size][Size];
    int flippedChips [Size][2];
    for (int i = 0; i < Size; i++){
        for (int j = 0; j < Size; j++){
            boardCopy[i][j] = board[i][j];
            if (board[i][j] == empty)
                emptySpots++;
        }
    }
    boardConfigurate (check, oppose, row, col, Size, &boardCopy[0][0]);

    for (int r = 0; r < Size; r++){
        for (int c = 0; c < Size; c++){
            if (boardCopy[r][c] != board[r][c]){
                flippedChips[count][0] = r;
                flippedChips[count][1] = c;
                count++;
            }
        }
    }
      
    for (int i = 0; i < count; i++){
        int r = flippedChips[i][0];
        int c = flippedChips[i][1];
        
        if ((isFound (check, oppose, r, c, -1, -1, Size, boardCopy) || isFound (check, empty, r, c, 1, 1, Size, boardCopy)) && (isFound (check, empty, r, c, -1, -1, Size, boardCopy) || isFound (check, oppose, r, c, 1, 1, Size, boardCopy)))
            score++;
        if ((isFound (check, oppose, r, c, -1, 1, Size, boardCopy) || isFound (check, empty, r, c, 1, -1, Size, boardCopy)) && (isFound (check, empty, r, c, -1, 1, Size, boardCopy) || isFound (check, oppose, r, c, 1, -1, Size, boardCopy)))
            score++;
        if ((isFound (check, oppose, r, c, 1, 0, Size, boardCopy) || isFound (check, empty, r, c, -1, 0, Size, boardCopy)) && (isFound (check, empty, r, c, 1, 0, Size, boardCopy) || isFound (check, oppose, r, c, -1, 0, Size, boardCopy)))
            score++;
        if ((isFound (check, oppose, r, c, 0, 1, Size, boardCopy) || isFound (check, empty, r, c, 0, -1, Size, boardCopy)) && (isFound (check, empty, r, c, 0, 1, Size, boardCopy) || isFound (check, oppose, r, c, 0, -1, Size, boardCopy)))
            score++;
              
    }

    if (emptySpots < (Size * Size / 12))
        return (800 * score);
    else if (emptySpots < (Size * Size / 8))
        return (700 * score);
    else if (emptySpots < (Size * Size / 4))
        return (400 * score);
    else if (emptySpots < (Size * Size / 2))
        return (100 * score);
    else
        return (60 * score);

}

bool isFound (char check, char lookFor, int row, int col, int rowDir, int colDir, int Size, char board[Size][Size]){

    bool found = true;

    while (found && row + rowDir >= 0 && row + rowDir < Size && col + colDir < Size){
        row += rowDir;
        col += colDir;
        if (board[row][col] == lookFor){
            found = false;
        }
        else if (board[row][col] != lookFor && board[row][col] != check){
            row = -10;
        }
    }

    return found;
}

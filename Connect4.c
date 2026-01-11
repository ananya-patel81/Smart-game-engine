// Lab 3 Take Home Assignment 
// Fresh Connect 4 Implementation
// Ananya Siddharth Patel 24110038

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROWS 4
#define COLS 5
#define MAX_DEPTH 10

char board[ROWS][COLS];

// Initialize the board with empty spaces
void init_board() {
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            board[r][c] = '.';
        }
    }
}

// Print the current board state
void print_board() {
    printf("\n");
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            printf("%c ", board[r][c]);
        }
        printf("\n");
    }
    printf("1 2 3 4 5\n");
    printf("\n");
    fflush(stdout);
}

// Check if a column is valid for dropping a piece
int is_valid_column(int col) {
    return (col >= 0 && col < COLS && board[0][col] == '.');
}

// Drop a piece in the specified column
int drop_piece(char player, int col) {
    if (!is_valid_column(col)) return -1;
    
    for (int r = ROWS - 1; r >= 0; r--) {
        if (board[r][col] == '.') {
            board[r][col] = player;
            return r;
        }
    }
    return -1;
}

// Remove a piece from the specified column (for minimax undo)
void remove_piece(int col) {
    for (int r = 0; r < ROWS; r++) {
        if (board[r][col] != '.') {
            board[r][col] = '.';
            return;
        }
    }
}

// Check if a player has won
int check_winner(char player) {
    // Check horizontal
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c <= COLS - 4; c++) {
            if (board[r][c] == player && 
                board[r][c+1] == player && 
                board[r][c+2] == player && 
                board[r][c+3] == player) {
                return 1;
            }
        }
    }
    
    // Check vertical
    for (int r = 0; r <= ROWS - 4; r++) {
        for (int c = 0; c < COLS; c++) {
            if (board[r][c] == player && 
                board[r+1][c] == player && 
                board[r+2][c] == player && 
                board[r+3][c] == player) {
                return 1;
            }
        }
    }
    
    // Check diagonal (top-left to bottom-right)
    for (int r = 0; r <= ROWS - 4; r++) {
        for (int c = 0; c <= COLS - 4; c++) {
            if (board[r][c] == player && 
                board[r+1][c+1] == player && 
                board[r+2][c+2] == player && 
                board[r+3][c+3] == player) {
                return 1;
            }
        }
    }
    
    // Check diagonal (top-right to bottom-left)
    for (int r = 0; r <= ROWS - 4; r++) {
        for (int c = 3; c < COLS; c++) {
            if (board[r][c] == player && 
                board[r+1][c-1] == player && 
                board[r+2][c-2] == player && 
                board[r+3][c-3] == player) {
                return 1;
            }
        }
    }
    
    return 0;
}

// Check if the board is full (draw condition)
int is_board_full() {
    for (int c = 0; c < COLS; c++) {
        if (board[0][c] == '.') {
            return 0;
        }
    }
    return 1;
}

// Minimax algorithm with alpha-beta pruning - Fixed version
int minimax(char player, char original_player, int depth, int alpha, int beta, int maximizing) {
    char opponent = (player == 'Y') ? 'R' : 'Y';
    
    // Terminal conditions
    if (check_winner(original_player)) {
        return 1000 - depth;  // Computer wins - prefer sooner wins
    }
    if (check_winner((original_player == 'Y') ? 'R' : 'Y')) {
        return -1000 + depth; // Computer loses - prefer later losses
    }
    if (is_board_full() || depth >= MAX_DEPTH) {
        return 0; // Draw or depth limit
    }
    
    if (maximizing) {
        int max_eval = -10000;
        for (int c = 0; c < COLS; c++) {
            if (is_valid_column(c)) {
                drop_piece(player, c);
                int eval = minimax(opponent, original_player, depth + 1, alpha, beta, 0);
                remove_piece(c);
                
                if (eval > max_eval) {
                    max_eval = eval;
                }
                if (eval > alpha) {
                    alpha = eval;
                }
                if (beta <= alpha) {
                    break; // Alpha-beta pruning
                }
            }
        }
        return max_eval;
    } else {
        int min_eval = 10000;
        for (int c = 0; c < COLS; c++) {
            if (is_valid_column(c)) {
                drop_piece(player, c);
                int eval = minimax(opponent, original_player, depth + 1, alpha, beta, 1);
                remove_piece(c);
                
                if (eval < min_eval) {
                    min_eval = eval;
                }
                if (eval < beta) {
                    beta = eval;
                }
                if (beta <= alpha) {
                    break; // Alpha-beta pruning
                }
            }
        }
        return min_eval;
    }
}

// Get the best move for the computer
int get_best_move(char computer_player) {
    int best_col = 2; // Default to center
    int best_score = -10000;
    char opponent = (computer_player == 'Y') ? 'R' : 'Y';
    
    // First priority: Win immediately if possible
    for (int c = 0; c < COLS; c++) {
        if (is_valid_column(c)) {
            drop_piece(computer_player, c);
            if (check_winner(computer_player)) {
                remove_piece(c);
                return c;
            }
            remove_piece(c);
        }
    }
    
    // Second priority: Block opponent's winning move
    for (int c = 0; c < COLS; c++) {
        if (is_valid_column(c)) {
            drop_piece(opponent, c);
            if (check_winner(opponent)) {
                remove_piece(c);
                return c;
            }
            remove_piece(c);
        }
    }
    
    // Third priority: Use minimax for strategic play
    for (int c = 0; c < COLS; c++) {
        if (is_valid_column(c)) {
            drop_piece(computer_player, c);
            int score = minimax(computer_player, 0, -10000, 10000, 0);
            remove_piece(c);
            
            if (score > best_score) {
                best_score = score;
                best_col = c;
            }
        }
    }
    
    return best_col;
}

int main() {
    init_board();
    
    char computer_color, opponent_color;
    char input;
    
    // Read which color the computer should play
    scanf(" %c", &input);
    computer_color = input;
    opponent_color = (computer_color == 'Y') ? 'R' : 'Y';
    
    char current_player = 'Y'; // Y always goes first
    
    // Print initial board
    printf("Game started! Computer is %c\n", computer_color);
    print_board();
    
    // If computer is Y, make the first move
    if (computer_color == 'Y') {
        int best_col = get_best_move(computer_color);
        drop_piece(computer_color, best_col);
        printf("%c %d\n", computer_color, best_col + 1);
        print_board();
        current_player = 'R';
    }
    
    // Main game loop
    while (1) {
        if (current_player == computer_color) {
            // Computer's turn
            int best_col = get_best_move(computer_color);
            drop_piece(computer_color, best_col);
            printf("%c %d\n", computer_color, best_col + 1);
            print_board();
            
            if (check_winner(computer_color)) {
                printf("%c won\n", computer_color);
                break;
            }
        } else {
            // Human player's turn
            char player_char;
            int col;
            scanf(" %c %d", &player_char, &col);
            
            if (col >= 1 && col <= COLS && is_valid_column(col - 1)) {
                drop_piece(opponent_color, col - 1);
                print_board();
                
                if (check_winner(opponent_color)) {
                    printf("%c won\n", opponent_color);
                    break;
                }
            } else {
                printf("Invalid move! Try again.\n");
                continue;
            }
        }
        
        // Check for draw
        if (is_board_full()) {
            printf("draw\n");
            break;
        }
        
        // Switch players
        current_player = (current_player == 'Y') ? 'R' : 'Y';
    }
    
    return 0;
}

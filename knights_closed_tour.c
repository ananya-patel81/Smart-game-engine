#include <stdio.h>
#include <stdbool.h>

int moves[8][2] = {
    {2, 1}, {1, 2}, {-1, 2}, {-2, 1},
    {-2, -1}, {-1, -2}, {1, -2}, {2, -1}
};

bool isValid(int x, int y, int n, int m, int board[n][m]) {
    return (x >= 0 && x < n && y >= 0 && y < m && board[x][y] == 0);
}

// count how many onward moves possible from (x,y)
int countOnwardMoves(int x, int y, int n, int m, int board[n][m]) {
    int cnt = 0;
    for (int i = 0; i < 8; i++) {
        int nx = x + moves[i][0];
        int ny = y + moves[i][1];
        if (isValid(nx, ny, n, m, board))
            cnt++;
    }
    return cnt;
}

bool knightTour(int x, int y, int n, int m,
                int board[n][m], int startX, int startY, int moveCount) {
    if (moveCount == n*m + 1) {
        // check closed condition
        for (int i = 0; i < 8; i++) {
            int nx = x + moves[i][0];
            int ny = y + moves[i][1];
            if (nx == startX && ny == startY)
                return true;
        }
        return false;
    }

    // Try moves in Warnsdorff’s order (fewest onward moves first)
    int nextMoves[8][3]; // {nx, ny, degree}
    int k = 0;

    for (int i = 0; i < 8; i++) {
        int nx = x + moves[i][0];
        int ny = y + moves[i][1];
        if (isValid(nx, ny, n, m, board)) {
            nextMoves[k][0] = nx;
            nextMoves[k][1] = ny;
            nextMoves[k][2] = countOnwardMoves(nx, ny, n, m, board);
            k++;
        }
    }

    // sort moves by degree (ascending)
    for (int i = 0; i < k; i++) {
        for (int j = i+1; j < k; j++) {
            if (nextMoves[i][2] > nextMoves[j][2]) {
                int tmp0 = nextMoves[i][0], tmp1 = nextMoves[i][1], tmp2 = nextMoves[i][2];
                nextMoves[i][0] = nextMoves[j][0];
                nextMoves[i][1] = nextMoves[j][1];
                nextMoves[i][2] = nextMoves[j][2];
                nextMoves[j][0] = tmp0; nextMoves[j][1] = tmp1; nextMoves[j][2] = tmp2;
            }
        }
    }

    for (int i = 0; i < k; i++) {
        int nx = nextMoves[i][0];
        int ny = nextMoves[i][1];
        board[nx][ny] = moveCount;
        if (knightTour(nx, ny, n, m, board, startX, startY, moveCount+1))
            return true;
        board[nx][ny] = 0;
    }

    return false;
}

int main() {
    int n, m, r, c;
    scanf("%d %d %d %d", &n, &m, &r, &c);

    // quick impossible checks
    if ((n*m) % 2 == 1 || n < 5 || m < 5) {
        printf("-1\n");
        return 0;
    }

    int board[n][m];
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            board[i][j] = 0;

    board[r][c] = 1;

    if (knightTour(r, c, n, m, board, r, c, 2)) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++)
                printf("%3d ", board[i][j]);
            printf("\n");
        }
    } else {
        printf("-1\n");
    }
    return 0;
}

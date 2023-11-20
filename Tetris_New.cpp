#include <iostream> //agar dapat menggunakan fungsi input output
#include <ctime> //agar piece dapat jatuh otomatis perdetik
#include <string> //untuk menampilkan text game over dan skor
#include <glut.h> //agar dapat menampilkan piece sekaligus dengan warna

using namespace std;

//deklarasi ukuran window permainan
const int WIDTH = 10; //lebar grid
const int HEIGHT = 20; //tinggi grid
const int BLOCK_SIZE = 30; //luas per blok = 30x30 px

struct Block {
    bool filled; //diisi nilai true untuk blok yang terisi, dan false untuk blok yang kosong
    float color[3]; //menyimpan nilai warna jika blok terisi piece (R,G,B)
};

Block grid[WIDTH][HEIGHT];
Block currentPiece[4][4];
int currentPieceX, currentPieceY;
int score = 0;
bool gameInProgress = true;
int finalScore = 0;

//deklarasi warna untuk setiap piece
const float COLORS[7][3] = {
    {1.0, 0.0, 0.0},   // Red
    {0.0, 1.0, 0.0},   // Green
    {0.0, 0.0, 1.0},   // Blue
    {1.0, 1.0, 0.0},   // Yellow
    {1.0, 0.0, 1.0},   // Lilac
    {0.0, 1.0, 1.0},   // Cyan
    {1.0, 0.5, 0.0}    // Orange
};

void init() {
    glClearColor(0.0, 0.0, 0.0, 1.0); //background hitam, nilai 1.0 adalah transparansi
    glMatrixMode(GL_PROJECTION); //mengubah koordinat objek menjadi koordinat layar
    glLoadIdentity(); //mengembalikan matriks proyeksi ke matriks identitas/keadaan awal piece
    gluOrtho2D(0, WIDTH * BLOCK_SIZE, 0, HEIGHT * BLOCK_SIZE); //mengatur batas atas bawah kiri kanan grid
}

//fungsi untuk menggambar persegi (karena setiap piece disusun atas persegi-persegi)
void drawSquare(int x, int y, float color[3]) {
    glColor3fv(color);
    glBegin(GL_QUADS); //library dari glut untuk menggambar persegi
    glVertex2f(x, y); //mengatur posisi vertex kiri bawah
    glVertex2f(x + BLOCK_SIZE, y); //mengatur posisi vertex kanan bawah
    glVertex2f(x + BLOCK_SIZE, y + BLOCK_SIZE); //mengatur posisi vertex kanan atas
    glVertex2f(x, y + BLOCK_SIZE); //mengatur posisi vertex kiri atas
    glEnd();
}

// fungsi untuk menggambar grid
void drawGrid() {
    for (int i = 0; i < WIDTH; ++i) { //perulangan pada kolom
        for (int j = 0; j < HEIGHT; ++j) { //perulangan pada baris
            if (grid[i][j].filled) { //pengecekan nilai i dan j
                drawSquare(i * BLOCK_SIZE, j * BLOCK_SIZE, grid[i][j].color);
            }
        }
    }
}

//fungsi untuk menggambar piece yang sedang jatuh
void drawCurrentPiece() {
    //iterasi untuk menggambar piece dengan matriks maksimal 4x4
    for (int i = 0; i < 4; ++i) { //i merepresentasikan kolom
        for (int j = 0; j < 4; ++j) { //j merepresentasikan baris
            if (currentPiece[i][j].filled) { //mengecek apakah i dan j memiliki nilai (terisi)
                drawSquare((currentPieceX + i) * BLOCK_SIZE, (currentPieceY + j) * BLOCK_SIZE, currentPiece[i][j].color);
            }
        }
    }
}

//fungsi yang memeriksa apakah terjadi collision antar-piece
bool isCollision() {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (currentPiece[i][j].filled) {
                if (currentPieceX + i < 0 || currentPieceX + i >= WIDTH || currentPieceY + j < 0 || currentPieceY + j >= HEIGHT || grid[currentPieceX + i][currentPieceY + j].filled) {
                    return true;
                }
            }
        }
    }
    return false;
}

//fungsi untuk 'menempelkan' piece
void mergePiece() {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (currentPiece[i][j].filled) {
                grid[currentPieceX + i][currentPieceY + j] = currentPiece[i][j];
                //mengganti nilai blok yang ada dengan nilai blok dari piece yang sedang bergerak
            }
        }
    }
}

void spawnPiece() {
    currentPieceX = WIDTH / 2 - 2; //mengatur posisi awal piece pada sumbu x
    currentPieceY = HEIGHT - 4; //mengatur posisi awal piece pada sumbu y

    int pieceType = rand() % 7; // 7 jenis bentuk piece

    // Definisi bentuk-bentuk piece
    int pieces[7][4][4] = {
        {{1, 1, 1, 1}, //Piece I
         {0, 0, 0, 0},
         {0, 0, 0, 0},
         {0, 0, 0, 0}},
        {{1, 1, 1, 0}, //Piece L
         {1, 0, 0, 0},
         {0, 0, 0, 0},
         {0, 0, 0, 0}},
        {{1, 1, 0, 0}, //Piece O
         {1, 1, 0, 0},
         {0, 0, 0, 0},
         {0, 0, 0, 0}},
        {{1, 1, 1, 0}, //Piece T
         {0, 1, 0, 0},
         {0, 0, 0, 0},
         {0, 0, 0, 0}},
        {{1, 1, 1, 0}, //Piece J
         {0, 0, 1, 0},
         {0, 0, 0, 0},
         {0, 0, 0, 0}},
        {{1, 1, 1, 0}, //Piece U
         {1, 0, 1, 0},
         {0, 0, 0, 0},
         {0, 0, 0, 0}},
        {{0, 1, 1, 0}, //Piece S
         {1, 1, 0, 0},
         {0, 0, 0, 0},
         {0, 0, 0, 0}}
    };

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            //mengisi properti filled berdasarkan nilai 1 atau 0 pada matriks pieces
            currentPiece[i][j].filled = pieces[pieceType][i][j] == 1;
            //mengatur warna blok berdasarkan jenis piece yang dipilih
            for (int k = 0; k < 3; ++k) {
                currentPiece[i][j].color[k] = COLORS[pieceType][k];
            }
        }
    }
}

//fungsi untuk menghapus baris ketika semua blok terisi
int clearLines() {
    int linesCleared = 0;
    for (int j = HEIGHT - 1; j >= 0; --j) {
        bool lineFull = true;
        for (int i = 0; i < WIDTH; ++i) {
            if (!grid[i][j].filled) {
                lineFull = false;
                break;
            }
        }
        if (lineFull) {
            linesCleared++;
            // Geser baris ke bawah
            for (int k = j; k < HEIGHT - 1; ++k) {
                for (int i = 0; i < WIDTH; ++i) {
                    grid[i][k] = grid[i][k + 1];
                }
            }
            // Baris baru di bagian atas diisi dengan blok kosong
            for (int i = 0; i < WIDTH; ++i) {
                grid[i][HEIGHT - 1].filled = false;
            }
            j++; // Periksa baris yang sama lagi setelah geser
        }
    }

    // Tambah skor berdasarkan jumlah baris yang dihapus
    score += linesCleared * 10;

    return linesCleared;
}


//fungsi agar piece dapat diputar (rotasi)
void rotatePiece() {
    Block tempPiece[4][4];
    // Simpan posisi awal
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            tempPiece[i][j] = currentPiece[i][j];
        }
    }

    // Rotasi
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            currentPiece[i][j] = tempPiece[3 - j][i];
        }
    }

    // Jika setelah rotasi terdapat collision, kembalikan ke posisi awal
    if (isCollision()) {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                currentPiece[i][j] = tempPiece[i][j];
            }
        }
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT); //memastikan latar belakang bersih

    drawGrid();
    drawCurrentPiece();

    if (!gameInProgress) {
        // Tampilan text game over dan skor yang diperoleh
        glColor3f(1.0, 1.0, 1.0); //menentukan warna teks
        int textWidth = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)"Game Over! Score: ");
        int xPosition = (WIDTH * BLOCK_SIZE - textWidth) / 2; //menentukan posisi text di sb x
        int yPosition = HEIGHT * BLOCK_SIZE / 2; //menentukan posisi teks di sb y
        glRasterPos2i(xPosition, yPosition);

        string gameOverMessage = "Game Over! Score: " + to_string(finalScore); //menampilkan berapa skor yang didapat
        for (char c : gameOverMessage) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }

        // Tampilkan pesan replay
        xPosition = 10; //posisi text pada sb x
        yPosition -= 30; // posisi text pada sb y
        glRasterPos2i(xPosition, yPosition);

        string additionalMessage = "Press Enter to replay or 'x' to exit";
        for (char c : additionalMessage) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }

        glutSwapBuffers();
    }
    else {
        glutSwapBuffers();
    }
}

//fungsi agar piece turun otomatis setiap satu detik
void update(int value) {
    if (gameInProgress) {
        currentPieceY--;

        if (isCollision()) {
            currentPieceY++;

            mergePiece();
            int linesCleared = clearLines();

            if (linesCleared > 0) {
                // Tambah skor berdasarkan jumlah baris yang dihapus
                score += linesCleared * 10;
            }

            spawnPiece();

            if (isCollision()) {
                // Game over
                gameInProgress = false;
                finalScore = score;
            }
        }

        glutPostRedisplay();
        if (gameInProgress) {
            glutTimerFunc(1000, update, 0);
        }
    }
}


// Fungsi callback untuk menangani input keyboard.
void keyboard(unsigned char key, int x, int y) {
    // Jika permainan belum berakhir, maka lakukan hal berikut:
    if (gameInProgress) {
        // Periksa apakah ada tombol yang ditekan.
        switch (key) {
        case 'j':
            currentPieceX--;
            if (isCollision()) {
                currentPieceX++;
            }
            break;
        case 'l':
            currentPieceX++;
            if (isCollision()) {
                currentPieceX--;
            }
            break;
        case 'r':
            rotatePiece();
            break;
        case 32: // ASCII untuk spasi
            // Spasi untuk mempercepat piece turun
            while (!isCollision()) {
                currentPieceY--;
            }
            currentPieceY++;
            mergePiece();
            clearLines();
            spawnPiece();
            if (isCollision()) {
                // Game over
                gameInProgress = false;
                finalScore = score;
            }
            break;
        }

        glutPostRedisplay();
    }
    else {
        if (key == 13) {  // ASCII untuk Enter
            // Reset nilai-nilai permainan
            gameInProgress = true;
            score = 0;
            finalScore = 0;

            // Bersihkan grid dan spawn piece baru
            for (int i = 0; i < WIDTH; ++i) {
                for (int j = 0; j < HEIGHT; ++j) {
                    grid[i][j].filled = false;
                }
            }

            spawnPiece();

            // Mulai kembali pemanggilan fungsi update
            glutTimerFunc(1000, update, 0);

            glutPostRedisplay();
        }
    }

    // Jika tombol 'k' ditekan, keluar dari program.
    if (key == 'x' || key == 'X') {
        exit(0);
    }
}

//main program
int main(int argc, char** argv) {
    srand(time(0)); //random number generator, agar piece random

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WIDTH * BLOCK_SIZE, HEIGHT * BLOCK_SIZE);
    glutCreateWindow("Tetris"); //membuat window berjudul Tetris

    init();
    spawnPiece();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(1000, update, 0);

    glutMainLoop();
    return 0;
}
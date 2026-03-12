#include "RunSession.h"

// ============================================================
// main.cpp
// Entry point — hanya buat RunSession dan panggil start().
// Tidak ada logika game di sini.
// ============================================================

int main() {
    RunSession session(1); // MUTABLE: ganti angka untuk ante berbeda
    session.start();
    return 0;
}

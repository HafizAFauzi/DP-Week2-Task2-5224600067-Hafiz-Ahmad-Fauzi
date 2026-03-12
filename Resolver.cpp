// ============================================================
// Resolver.cpp
// Gabungan PlayerAction + HandEvaluator + ScoringSystem + Shop
// ============================================================
#include "Resolver.h"
#include <iostream>
using namespace std;

Resolver::Resolver(const GameConfig& cfg)
    : config(cfg) {}

// ============================================================
// playerAction() - INVARIANT posisi
// Harus dipanggil setelah showHand(), sebelum resolveHand().
// Pemain harus melihat kartu dulu sebelum memilih.
//
// Yang MUTABLE: cara input (cin interaktif, AI, simulasi)
// ============================================================
void Resolver::playerAction(int discardsRemaining) {
    cout << "  [Resolver] PlayerAction:\n";
    cout << "    >> TODO: pemain memilih aksi:\n";
    cout << "       (1) Play - pilih 1-5 kartu untuk dimainkan\n";
    cout << "       (2) Discard - buang kartu (sisa: "
         << discardsRemaining << ")\n";
    cout << "    >> Simulasi: Play, memilih kartu index [0, 1].\n";
    // TODO: cin >> pilihan, validasi index, simpan pilihan
}

// ============================================================
// resolveHand() - INVARIANT: urutan D1 : D2 : D3
// Menjalankan evaluate : calculate : applyMods secara berurutan.
// Mengembalikan skor akhir turn ini.
// ============================================================
int Resolver::resolveHand() {
    cout << "  [Resolver] ResolveHand:\n";

    evaluate();      // D1 - harus sebelum calculate
    int score = calculate(); // D2 - harus sebelum applyMods
    applyMods();     // D3 - modifikasi skor sebelum dikembalikan

    cout << "    >> Skor turn ini: " << score << "\n";
    return score;    // dikembalikan ke RunSession untuk UpdateState
}

// ============================================================
// D1 - evaluate() - INVARIANT posisi
// Tentukan kombinasi poker dari kartu yang dipilih.
// Harus sebelum calculate() karena score bergantung pada rank.
// ============================================================
void Resolver::evaluate() {
    cout << "    [D1] Evaluate hand rank:\n";
    cout << "      >> TODO: cek frekuensi rank : One Pair, Flush, dll.\n";
    cout << "      >> TODO: cek Straight (5 rank berurutan).\n";
    cout << "      >> TODO: cek Flush (5 kartu suit sama).\n";
    cout << "      >> Simulasi: One Pair\n";
}

// ============================================================
// D2 - calculate() - nilai MUTABLE (dibaca dari GameConfig)
// Hitung chips x mult berdasarkan hand rank.
// Nilai dibaca dari GameConfig - bisa diubah tanpa sentuh loop.
// ============================================================
int Resolver::calculate() {
    // MUTABLE: nilai chips dan mult dibaca dari GameConfig
    int chips = config.onePairChips; // simulasi One Pair
    int mult  = config.onePairMult;
    int total = chips * mult;

    cout << "    [D2] Calculate score:\n";
    cout << "      >> " << chips << " chips x "
         << mult << " mult = " << total << "\n";
    cout << "      >> (nilai dari GameConfig - MUTABLE)\n";

    return total;
}

// ============================================================
// D3 - applyMods() - list modifier MUTABLE
// Terapkan semua modifier aktif yang dibeli dari shop.
// List bertambah setiap pemain beli item - loop tidak berubah.
// ============================================================
void Resolver::applyMods() {
    cout << "    [D3] Apply modifiers:\n";
    cout << "      >> Simulasi: tidak ada modifier aktif.\n";
    // TODO: if modifier == "Pair Booster" : mult += 1
    // TODO: if modifier == "Double Score" : total *= 2
}

// ============================================================
// openShop() - INVARIANT posisi (hanya dipanggil setelah WIN)
// Hitung reward, tampilkan item, proses pembelian.
// Konten item dan harga dibaca dari GameConfig (MUTABLE).
// ============================================================
void Resolver::openShop(int& coins) {
    cout << "  [Resolver] Shop dibuka!\n";
    cout << "  Coins saat ini: " << coins << "\n\n";

    // MUTABLE: item dan harga dari GameConfig
    cout << "    1. Pair Booster - " << config.pairBoosterCost
         << " coins  (One Pair +1 mult)\n";
    cout << "    2. Double Score - " << config.doubleScoreCost
         << " coins  (total score x2)\n";
    cout << "    0. Keluar shop\n\n";

    cout << "  >> TODO: cin >> pilihan, coins -= harga.\n";
    cout << "  >> TODO: tambah modifier ke activeModifiers.\n";
    cout << "  >> Simulasi: tidak membeli (pilih 0).\n";
    cout << "  [Resolver] Shop ditutup.\n";
}

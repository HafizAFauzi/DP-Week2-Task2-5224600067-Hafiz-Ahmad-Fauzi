// ============================================================
// DeckManager.cpp
// Gabungan Deck + Hand - semua urusan kartu ada di sini
// ============================================================
#include "DeckManager.h"
#include <iostream>
using namespace std;

DeckManager::DeckManager(const GameConfig& cfg)
    : config(cfg) {}

// ============================================================
// reset() - INVARIANT: harus dipanggil di awal setiap blind
// Mengocok deck dan mengosongkan tangan pemain
// ============================================================
void DeckManager::reset() {
    cout << "  [DeckManager] Reset deck dan tangan pemain.\n";
    cout << "    >> TODO: deck = 52 kartu tema Genshin, dikocok acak.\n";
    cout << "    >> Deck berisi 4 elemen (suit):\n";
    cout << "       Pyro    : 10 kartu biasa + Diluc, Mavuika, Haborym\n";   // MUTABLE
    cout << "       Hydro   : 10 kartu biasa + Barbara, Furina, Focalors\n"; // MUTABLE
    cout << "       Cryo    : 10 kartu biasa + Eula, La Signora, Tsaritsa\n"; // MUTABLE
    cout << "       Electro : 10 kartu biasa + Raiden, Ei, Beelzebul\n";     // MUTABLE
}

// ============================================================
// draw() - INVARIANT: harus dipanggil sebelum showHand
// Tarik kartu dari deck sampai tangan berisi handSize kartu
// handSize dibaca dari GameConfig (MUTABLE)
// ============================================================
void DeckManager::draw() {
    cout << "  [DeckManager] Draw kartu sampai tangan berisi "
         << config.handSize << " kartu.\n"; // nilai dari GameConfig
    cout << "    >> TODO: sort tangan descending by rank.\n";
}

// ============================================================
// showHand() - INVARIANT: harus setelah draw, sebelum Resolver
// Tampilkan semua kartu di tangan dengan index
// ============================================================
void DeckManager::showHand() {
    cout << "  [DeckManager] Tangan pemain saat ini:\n";
    // MUTABLE: format tampilan bisa diubah (warna, icon, dsb)
    cout << "    [0] A  - Haborym (Pyro)\n";
    cout << "    [1] K  - Beelzebul (Electro)\n";
    cout << "    [2] Q  - Focalors (Hydro)\n";
    cout << "    [3] J  - Tsaritsa (Cryo)\n";
    cout << "    [4] 10 - Pyro\n";
    cout << "    [5] 9  - Electro\n";
    cout << "    [6] 8  - Hydro\n";
    cout << "    [7] 7  - Cryo\n";
    cout << "    >> TODO: ganti dengan kartu hasil draw() asli.\n";
}

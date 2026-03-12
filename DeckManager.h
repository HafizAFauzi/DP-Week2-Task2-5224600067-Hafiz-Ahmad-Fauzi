#pragma once
// ============================================================
// DeckManager.h
// STATUS : Interface = INVARIANT | Konten kartu = MUTABLE
//
// Gabungan: Deck + Hand
// Tanggung jawab:
//   - Menyimpan dan mengocok deck (tema Genshin Impact)
//   - Menarik kartu ke tangan pemain
//   - Menampilkan tangan pemain
//
// Yang MUTABLE : nama karakter, jumlah kartu, tema deck
// Yang INVARIANT: interface reset() → draw() → showHand()
// ============================================================
#include "GameConfig.h"

class DeckManager {
public:
    explicit DeckManager(const GameConfig& cfg);

    void reset();      // INVARIANT: kocok ulang + kosongkan tangan
    void draw();       // INVARIANT: tarik kartu sampai tangan penuh
    void showHand();   // INVARIANT: tampilkan tangan pemain

private:
    const GameConfig& config; // baca handSize dari config (MUTABLE)
};

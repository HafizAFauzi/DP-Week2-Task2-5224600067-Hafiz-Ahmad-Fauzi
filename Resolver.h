#pragma once
// ============================================================
// Resolver.h
// STATUS : Posisi = INVARIANT | Nilai & konten = MUTABLE
//
// Gabungan: PlayerAction + HandEvaluator + ScoringSystem + Shop
// Tanggung jawab:
//   - Menerima pilihan kartu dari pemain      (PlayerAction)
//   - Mengevaluasi kombinasi poker            (HandEvaluator)
//   - Menghitung skor: chips x multiplier     (ScoringSystem)
//   - Menerapkan modifier aktif               (Modifier)
//   - Mengelola reward dan shop               (Shop)
//
// Yang MUTABLE : cara input, tabel chips/mult, item shop
// Yang INVARIANT: urutan act() → evaluate() → score() → modify()
// ============================================================
#include "GameConfig.h"

class Resolver {
public:
    explicit Resolver(const GameConfig& cfg);

    // ---- Dipanggil RunSession dalam loop (urutan INVARIANT) ----
    void    playerAction(int discardsRemaining); // C: pilih kartu / discard
    int     resolveHand();                       // D: evaluasi + hitung skor
    void    openShop(int& coins);                // Phase 5: reward + beli item

private:
    // Sub-step dalam resolveHand (urutan juga INVARIANT)
    void evaluate();   // D1: tentukan hand rank
    int  calculate();  // D2: chips x mult dari GameConfig (MUTABLE)
    void applyMods();  // D3: terapkan modifier aktif (list MUTABLE)

    const GameConfig& config; // baca nilai dari GameConfig
};

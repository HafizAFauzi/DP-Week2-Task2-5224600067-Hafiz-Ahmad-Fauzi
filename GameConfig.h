#pragma once
// ============================================================
// GameConfig.h
// STATUS : MUTABLE
//
// Semua nilai yang boleh diubah dikumpulkan di sini.
// Mengubah angka di file ini TIDAK akan merusak urutan loop.
// RunSession membaca nilai dari sini, bukan hardcode sendiri.
// ============================================================

struct GameConfig {

    // ---- Blind target score (per ante) ----------------------
    int smallBlindMultiplier = 100;  // MUTABLE: target = 100 * ante
    int bigBlindMultiplier   = 200;  // MUTABLE: target = 200 * ante
    int bossBlindMultiplier  = 400;  // MUTABLE: target = 400 * ante

    // ---- Jumlah giliran per blind ---------------------------
    int handsPerBlind    = 4;  // MUTABLE: ganti 3/5 untuk hard/easy
    int discardsPerBlind = 3;  // MUTABLE: bisa ditambah via shop

    // ---- Ukuran tangan pemain -------------------------------
    int handSize = 8;  // MUTABLE: jumlah kartu di tangan

    // ---- Reward coins setelah blind clear -------------------
    int baseReward = 7;  // MUTABLE: 7 coins base + sisa hands

    // ---- Harga item shop ------------------------------------
    int pairBoosterCost = 5;  // MUTABLE
    int doubleScoreCost = 6;  // MUTABLE

    // ---- Nilai chips x mult per hand rank (base Balatro) ----
    // MUTABLE: ubah untuk balancing tanpa menyentuh loop
    int highCardChips      =   5;  int highCardMult      = 1;
    int onePairChips       =  10;  int onePairMult       = 1;
    int twoPairChips       =  20;  int twoPairMult       = 2;
    int threeOfAKindChips  =  30;  int threeOfAKindMult  = 2;
    int straightChips      =  40;  int straightMult      = 3;
    int flushChips         =  50;  int flushMult         = 3;
    int fullHouseChips     =  70;  int fullHouseMult     = 4;
    int fourOfAKindChips   = 100;  int fourOfAKindMult   = 5;
    int straightFlushChips = 150;  int straightFlushMult = 8;
};

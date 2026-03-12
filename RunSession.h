#pragma once
// ============================================================
// RunSession.h
// STATUS : INVARIANT — orchestrator utama core loop
//
// RunSession HANYA mengatur urutan fase.
// Semua logika didelegasikan ke class pendukung:
//   GameConfig   → semua nilai MUTABLE
//   DeckManager  → urusan kartu (Draw + Hand + ShowHand)
//   Resolver     → pilihan pemain + evaluasi + skor + shop
//
// Urutan fase TIDAK BOLEH berubah:
//   start()     : Small Blind → Big Blind → Boss Blind
//   playBlind() : InitBlind → PlayerTurnLoop → EvaluateResult → Shop
//   loop step   : Draw → Show → PlayerAction → Resolve → UpdateState
// ============================================================
#include "GameConfig.h"
#include "DeckManager.h"
#include "Resolver.h"
#include <string>

class RunSession {
public:
    explicit RunSession(int ante);
    void start(); // INVARIANT: entry point, urutan blind terkunci

private:
    // ---- Invariant phases -----------------------------------
    bool playBlind(const std::string& name, int target);

    void phase_InitBlind(const std::string& name, int target);
    void phase_PlayerTurnLoop();
    bool phase_EvaluateResult();
    void phase_Shop();

    // ---- Invariant steps dalam loop -------------------------
    void step_Draw();
    void step_ShowHand();
    void step_PlayerAction();
    void step_ResolveSystem();
    void step_UpdateState(int scoreGained);

    // ---- Class pendukung ------------------------------------
    GameConfig  config;      // MUTABLE: semua nilai dikumpulkan di sini
    DeckManager deckManager; // INVARIANT interface, MUTABLE konten
    Resolver    resolver;    // INVARIANT urutan, MUTABLE nilai & item

    // ---- State permainan (nilai semua MUTABLE) --------------
    int  currentAnte;
    int  totalScore;
    int  targetScore;
    int  handsRemaining;
    int  discardsRemaining;
    int  coins;
    std::string currentBlindName;
};

// ============================================================
// RunSession.cpp
// STATUS : INVARIANT - orchestrator murni
//
// RunSession HANYA mengatur urutan pemanggilan.
// Tidak ada logika game di sini - semuanya di class pendukung.
// ============================================================
#include "RunSession.h"
#include <iostream>
using namespace std;

// ============================================================
// Konstruktor
// Nilai awal dibaca dari GameConfig - semua MUTABLE
// ============================================================
RunSession::RunSession(int ante)
    : config(),
      deckManager(config),
      resolver(config),
      currentAnte(ante),
      totalScore(0),
      targetScore(0),
      handsRemaining(config.handsPerBlind),
      discardsRemaining(config.discardsPerBlind),
      coins(0)
{}

// ============================================================
// start() - INVARIANT
// Urutan Small → Big → Boss tidak boleh dibalik.
// Difficulty naik bertahap - pemain butuh coins dari blind
// sebelumnya sebelum menghadapi Boss Blind.
// ============================================================
void RunSession::start() {
    cout << "\n================================================\n";
    cout << "   POKER, REACTION?  -  Ante " << currentAnte << "\n";
    cout << "   Genshin Impact x Balatro-style card game\n";
    cout << "================================================\n";
    cout << "\n[INVARIANT] PHASE 1 - InitRun (Ante " << currentAnte << ")\n";

    // INVARIANT: urutan ini tidak boleh diubah
    // target score dibaca dari GameConfig (MUTABLE)
    if (!playBlind("Small Blind", config.smallBlindMultiplier * currentAnte)) {
        cout << "\n[RESULT] GAME OVER - Gagal di Small Blind.\n";
        return;
    }
    if (!playBlind("Big Blind", config.bigBlindMultiplier * currentAnte)) {
        cout << "\n[RESULT] GAME OVER - Gagal di Big Blind.\n";
        return;
    }
    if (!playBlind("Boss Blind", config.bossBlindMultiplier * currentAnte)) {
        cout << "\n[RESULT] GAME OVER - Gagal di Boss Blind.\n";
        return;
    }

    cout << "\n[RESULT] CONGRATULATIONS! Ante " << currentAnte << " selesai!\n";
}

// ============================================================
// playBlind() - INVARIANT orchestrator per blind
// Memanggil 4 fase secara berurutan - tidak boleh diubah.
// ============================================================
bool RunSession::playBlind(const string& name, int target) {
    phase_InitBlind(name, target);           // FASE 2 - harus pertama
    phase_PlayerTurnLoop();                  // FASE 3 - inti loop
    bool cleared = phase_EvaluateResult();   // FASE 4 - cek menang/kalah
    if (cleared) phase_Shop();               // FASE 5 - hanya jika menang
    return cleared;
}

// ============================================================
// PHASE 2 - InitBlind  [INVARIANT]
// Reset state, lalu delegasi reset deck ke DeckManager.
// Nilai reset dibaca dari GameConfig (MUTABLE).
// ============================================================
void RunSession::phase_InitBlind(const string& name, int target) {
    currentBlindName  = name;
    targetScore       = target;
    totalScore        = 0;
    handsRemaining    = config.handsPerBlind;    // MUTABLE via GameConfig
    discardsRemaining = config.discardsPerBlind; // MUTABLE via GameConfig

    cout << "\n------------------------------------------------\n";
    cout << "[INVARIANT] PHASE 2 - InitBlind\n";
    cout << "  Blind   : " << currentBlindName << "\n";
    cout << "  Target  : " << targetScore << " pts\n";
    cout << "  Hands   : " << handsRemaining
         << "  (dari GameConfig - MUTABLE)\n";
    cout << "  Discards: " << discardsRemaining
         << "  (dari GameConfig - MUTABLE)\n";
    cout << "------------------------------------------------\n";

    deckManager.reset(); // delegasi ke DeckManager
    step_Draw();         // tarik tangan awal
}

// ============================================================
// PHASE 3 - PlayerTurnLoop  [INVARIANT]
// Urutan step dalam loop juga invariant:
//   Show → PlayerAction → Resolve → UpdateState
// step_Draw dipanggil di UpdateState setelah kartu dibuang.
// ============================================================
void RunSession::phase_PlayerTurnLoop() {
    cout << "\n[INVARIANT] PHASE 3 - PlayerTurnLoop\n";

    while (handsRemaining > 0 && totalScore < targetScore) {

        cout << "\n  === Turn | Score: " << totalScore
             << "/" << targetScore
             << " | Hands: " << handsRemaining << " ===\n";

        step_ShowHand();           // B - tampilkan dulu, baru input
        step_PlayerAction();       // C - pilih kartu via Resolver
        step_ResolveSystem();      // D - evaluasi + skor via Resolver
        step_UpdateState(10);      // E - commit skor, draw ulang
        // TODO: ganti 10 dengan nilai asli dari resolveHand()
    }

    cout << "\n[INVARIANT] PHASE 3 - PlayerTurnLoop selesai\n";
}

// ============================================================
// PHASE 4 - EvaluateResult  [INVARIANT]
// Cek SETELAH loop - tidak boleh dicek di dalam loop.
// ============================================================
bool RunSession::phase_EvaluateResult() {
    cout << "\n[INVARIANT] PHASE 4 - EvaluateResult\n";
    cout << "  Score akhir : " << totalScore << "\n";
    cout << "  Target      : " << targetScore << "\n";

    if (totalScore >= targetScore) {
        cout << "  [WIN] Blind cleared!\n";
        return true;
    }
    cout << "  [LOSE] Skor tidak mencapai target.\n";
    return false;
}

// ============================================================
// PHASE 5 - Shop  [MUTABLE konten, INVARIANT posisi]
// Hitung reward lalu delegasi ke Resolver::openShop().
// Formula reward dibaca dari GameConfig (MUTABLE).
// ============================================================
void RunSession::phase_Shop() {
    // MUTABLE: formula reward dari GameConfig
    int reward = handsRemaining + config.baseReward;
    coins += reward;

    cout << "\n[MUTABLE] PHASE 5 - Shop\n";
    cout << "  Reward: " << reward
         << " coins (total: " << coins << ")\n";

    resolver.openShop(coins); // delegasi ke Resolver
}

// ============================================================
// STEP A - Draw  [INVARIANT]
// Delegasi sepenuhnya ke DeckManager.
// ============================================================
void RunSession::step_Draw() {
    cout << "  [STEP A | INVARIANT] Draw\n";
    deckManager.draw(); // DeckManager tahu cara tarik kartu
}

// ============================================================
// STEP B - ShowHand  [INVARIANT]
// Delegasi ke DeckManager.
// Harus sebelum PlayerAction - pemain lihat kartu dulu.
// ============================================================
void RunSession::step_ShowHand() {
    cout << "  [STEP B | INVARIANT] ShowHand\n";
    deckManager.showHand(); // DeckManager tahu cara tampilkan
}

// ============================================================
// STEP C - PlayerAction  [INVARIANT posisi]
// Delegasi ke Resolver.
// Harus setelah ShowHand, sebelum ResolveSystem.
// ============================================================
void RunSession::step_PlayerAction() {
    cout << "  [STEP C | INVARIANT posisi] PlayerAction\n";
    resolver.playerAction(discardsRemaining); // Resolver terima input
}

// ============================================================
// STEP D - ResolveSystem  [INVARIANT]
// Delegasi ke Resolver::resolveHand().
// Harus setelah PlayerAction (butuh kartu yang dipilih).
// ============================================================
void RunSession::step_ResolveSystem() {
    cout << "  [STEP D | INVARIANT] ResolveSystem\n";
    resolver.resolveHand(); // Resolver handle D1 → D2 → D3
}

// ============================================================
// STEP E - UpdateState  [INVARIANT]
// Commit skor, kurangi hand, buang kartu, draw ulang.
// Harus paling terakhir dalam satu turn.
// ============================================================
void RunSession::step_UpdateState(int scoreGained) {
    totalScore     += scoreGained;
    handsRemaining--;

    cout << "  [STEP E | INVARIANT] UpdateState\n";
    cout << "    >> Score turn ini  : " << scoreGained << "\n";
    cout << "    >> Total score     : " << totalScore << "\n";
    cout << "    >> Hands tersisa   : " << handsRemaining << "\n";
    cout << "    >> Kartu dimainkan dibuang, tarik ulang.\n";

    step_Draw(); // draw ulang setelah kartu dibuang
}

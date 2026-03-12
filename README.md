# Task 2 — Core Loop: Poker, Reaction?

**Course    :** Design Pattern  
**Game      :** Poker, Reaction?  
**Inspirasi :** Balatro — dengan deck bertema Genshin Impact  

---

## Step 1 — Core Loop

Poker, Reaction? adalah card game berbasis poker di mana pemain harus
mencapai target skor sebelum kehabisan giliran tangan (hand), melawan
tiga tingkatan blind per ante.

```
1. Blind baru dimulai — deck dikocok, tangan awal ditarik (DeckManager)
2. Pemain melihat 8 kartu di tangan
3. Pemain memilih 1–5 kartu untuk dimainkan, atau memilih Discard
4. Sistem mengevaluasi kombinasi poker dari kartu yang dipilih  (Resolver - D1)
5. Skor dihitung: chips × multiplier berdasarkan hand rank       (Resolver - D2)
6. Modifier aktif yang dibeli dari shop diterapkan ke skor       (Resolver - D3)
7. Skor diakumulasi ke total score, jumlah hand berkurang
8. Jika total score >= target  →  Blind cleared  →  buka Shop
   Jika hand habis sebelum target tercapai  →  Game Over
9. Ulangi dari langkah 1 untuk blind berikutnya: Small → Big → Boss
```

---

## Step 2 — Identify Invariants

### Bagian yang TIDAK BOLEH berubah urutannya

| # | Fase / Step | Apa yang rusak jika urutan diubah |
|---|-------------|----------------------------------|
| 1 | `start()` memanggil Small → Big → Boss | Jika Boss dipindah ke depan, pemain belum punya coins atau modifier apapun. Target 400 pts tidak bisa dicapai → game *unwinnable*. |
| 2 | `phase_InitBlind` sebelum `phase_PlayerTurnLoop` | Tanpa reset di InitBlind, skor dan deck dari blind sebelumnya terbawa masuk ke loop berikutnya. State menjadi tidak valid. |
| 3 | `step_Draw` sebelum `step_ShowHand` | Tidak bisa menampilkan kartu yang belum ditarik dari deck. |
| 4 | `step_ShowHand` sebelum `step_PlayerAction` | Pemain tidak bisa memilih kartu yang belum mereka lihat. |
| 5 | `step_PlayerAction` sebelum `step_ResolveSystem` | Tidak ada kartu yang terpilih untuk dievaluasi oleh Resolver. |
| 6 | `step_ResolveSystem` sebelum `step_UpdateState` | Skor yang di-commit ke `totalScore` adalah hasil dari turn sebelumnya (*stale data*). |
| 7 | `phase_EvaluateResult` setelah loop selesai | Jika dicek di dalam loop, giliran pemain terpotong sebelum semua hand terpakai. |
| 8 | `phase_Shop` hanya jika blind cleared | Jika shop dibuka sebelum EvaluateResult, pemain bisa upgrade meski sudah kalah. |
| 9 | Di dalam Resolver: D1 → D2 → D3 | D2 (calculate) bergantung pada hasil D1 (rank). D3 (modifier) harus memodifikasi hasil D2 sebelum skor dikembalikan. |

### Komponen yang harus selalu ada

- **RunSession** — tanpanya tidak ada yang mengatur urutan fase.
- **DeckManager** — loop tidak bisa berjalan tanpa kartu yang bisa ditarik dan ditampilkan.
- **Resolver** — tanpa evaluasi dan perhitungan skor, loop tidak punya resolusi.
- **GameConfig** — nilai target dan skor harus terdefinisi agar kondisi menang bisa dicek.

---

## Step 3 — Identify Mutable Elements

Semua nilai mutable dikumpulkan di **`GameConfig.h`** sehingga perubahan
balancing tidak perlu menyentuh logika loop sama sekali.

| Elemen | Lokasi | Mengapa Mutable |
|--------|--------|-----------------|
| `handsPerBlind = 4` | `GameConfig.h` | Bisa diubah ke 3 (Hard) atau 5 (Easy). Loop hanya membaca nilainya, tidak peduli berapa angkanya. |
| `discardsPerBlind = 3` | `GameConfig.h` | Bisa ditambah lewat item shop. Loop tidak perlu tahu berapa nilai awalnya. |
| Formula target blind (`100`, `200`, `400` × ante) | `GameConfig.h` | Keputusan balancing murni. Ganti ke `150 * ante` untuk Hard Mode tanpa ubah urutan fase. |
| Tabel `chips` dan `mult` per hand rank | `GameConfig.h` | Nilai seperti `10 chips x1 mult` untuk One Pair adalah tuning — bisa diubah tiap patch tanpa menyentuh loop. |
| Nama karakter deck (Diluc, Furina, Raiden, dll) | `DeckManager.cpp` | Tema Genshin Impact adalah konten, bukan struktur. Bisa diganti tema lain selama tetap 52 kartu 4 suit. |
| Daftar item dan harga di Shop | `Resolver.cpp` | Item baru (misal: "Element Bonus") cukup ditambah di `openShop()` tanpa mengubah kapan atau bagaimana shop dipanggil. |
| `handSize = 8` | `GameConfig.h` | Jumlah kartu di tangan bisa diubah ke 5 atau 10 untuk variasi gameplay. |
| `baseReward = 7` | `GameConfig.h` | Formula reward coins setelah blind clear bisa disesuaikan. |

---

## Step 4 — Implementasi C++ Core Loop Skeleton

### Struktur File

```
Task2/
├── main.cpp          ← entry point, hanya buat RunSession dan panggil start()
├── GameConfig.h      ← semua nilai MUTABLE dikumpulkan di sini
├── RunSession.h      ← deklarasi fase-fase INVARIANT
├── RunSession.cpp    ← orchestrator murni, hanya memanggil class lain
├── DeckManager.h     ← interface draw + hand + tampilkan (INVARIANT)
├── DeckManager.cpp   ← konten kartu Genshin (MUTABLE)
├── Resolver.h        ← interface evaluasi + skor + shop (INVARIANT urutan)
└── Resolver.cpp      ← nilai chips/mult + item shop (MUTABLE)
```

### Peran Masing-masing Class

```
main.cpp
  └── RunSession          ← orchestrator: hanya atur urutan pemanggilan
        ├── GameConfig    ← semua nilai MUTABLE (tidak ada logika)
        ├── DeckManager   ← draw() + showHand() — urusan kartu
        └── Resolver      ← playerAction() + resolveHand() + openShop()
                               D1: evaluate rank
                               D2: calculate chips x mult
                               D3: apply modifiers
```

### RunSession sebagai Orchestrator

`RunSession` tidak mengandung logika game apapun.
Isi `playBlind()` hanya memanggil class lain secara berurutan:

```cpp
bool RunSession::playBlind(const string& name, int target) {
    phase_InitBlind(name, target);         // reset via DeckManager
    phase_PlayerTurnLoop();                // loop: show → action → resolve → update
    bool cleared = phase_EvaluateResult(); // cek menang/kalah
    if (cleared) phase_Shop();             // reward via Resolver
    return cleared;
}
```

Dan isi loop hanya memanggil step secara berurutan:

```cpp
while (handsRemaining > 0 && totalScore < targetScore) {
    step_ShowHand();      // DeckManager::showHand()
    step_PlayerAction();  // Resolver::playerAction()
    step_ResolveSystem(); // Resolver::resolveHand()
    step_UpdateState();   // commit skor, DeckManager::draw()
}
```

### Output saat Dijalankan

Setiap fase mencetak label `[INVARIANT]` atau `[MUTABLE]`
sehingga alur core loop terlihat jelas dari output:

```
[INVARIANT] PHASE 1 — InitRun
[INVARIANT] PHASE 2 — InitBlind
  [DeckManager] Reset deck dan tangan pemain.
  [STEP A | INVARIANT] Draw
[INVARIANT] PHASE 3 — PlayerTurnLoop
  === Turn | Score: 0/100 | Hands: 4 ===
  [STEP B | INVARIANT] ShowHand       ← DeckManager
  [STEP C | INVARIANT] PlayerAction   ← Resolver (cara input MUTABLE)
  [STEP D | INVARIANT] ResolveSystem  ← Resolver D1→D2→D3
    [D1] Evaluate hand rank
    [D2] Calculate score              ← nilai dari GameConfig (MUTABLE)
    [D3] Apply modifiers              ← list modifier MUTABLE
  [STEP E | INVARIANT] UpdateState
[INVARIANT] PHASE 4 — EvaluateResult
[MUTABLE]   PHASE 5 — Shop           ← Resolver::openShop()
```

### Cara Compile dan Run

```bash
g++ -std=c++17 main.cpp RunSession.cpp DeckManager.cpp Resolver.cpp -o poker_reaction
./poker_reaction
```

---

## Reflection

### 1. Apa struktur yang tetap (invariant) dan apa yang bisa diubah (mutable)?

Struktur invariant dari game ini adalah **urutan fase** yang harus selalu
dijalankan dalam urutan yang sama, terlepas dari nilai atau konten apapun
yang digunakan. Ada dua level urutan yang dikunci.

Level pertama adalah urutan antar fase dalam satu blind. `InitBlind` harus
selalu mendahului `PlayerTurnLoop` karena loop membutuhkan state bersih —
deck yang sudah dikocok dan skor yang sudah di-reset. `EvaluateResult`
harus selalu mengikuti loop karena ia memeriksa akumulasi skor yang
dibangun selama loop berjalan. `Shop` hanya boleh dibuka setelah
`EvaluateResult` mengkonfirmasi kemenangan — membuka shop sebelumnya
berarti memberikan reward kepada pemain yang belum tentu menang.

Level kedua adalah urutan antar blind dalam satu ante: Small → Big → Boss.
Pemain membutuhkan coins dari Small dan Big Blind untuk membeli modifier
sebelum menghadapi Boss Blind yang target skornya empat kali lipat dari
Small Blind. Membalik urutan ini membuat game tidak bisa diselesaikan.

Yang mutable adalah semua **nilai dan konten** yang tidak mempengaruhi
urutan eksekusi. Semua nilai ini dikumpulkan di `GameConfig.h` sehingga
perubahan apapun untuk keperluan balancing atau difficulty scaling tidak
perlu menyentuh `RunSession`, `DeckManager`, maupun `Resolver`.

### 2. Jika ingin menambah fitur baru, class mana yang berubah?

Bergantung jenis fiturnya, dan dalam semua kasus `RunSession` tidak perlu
diubah.

Jika ingin menambah item shop baru seperti "Element Bonus" yang memberi
+3 mult saat semua kartu berasal dari elemen yang sama, cukup tambahkan
entry di `Resolver::openShop()` dan tambah kondisi di `applyMods()`.
Jika ingin mengubah jumlah hands dari 4 menjadi 5, cukup ubah satu baris
di `GameConfig.h`. Jika ingin mengganti tema deck dari Genshin Impact ke
tema lain, cukup ubah isi `DeckManager::reset()` — selama tetap 52 kartu
dan 4 suit, `Resolver` tidak perlu tahu perubahannya.

Pemisahan ini adalah alasan mengapa `GameConfig` dibuat sebagai class
tersendiri: semua nilai mutable terpusat di satu tempat, bukan tersebar
di berbagai file.

### 3. Apa yang akan rusak jika urutan loop diubah?

Jika `step_ResolveSystem` dipindah sebelum `step_PlayerAction`, Resolver
mengevaluasi kartu yang belum dipilih — hasilnya adalah evaluasi terhadap
pilihan kosong atau pilihan dari turn sebelumnya. Jika `step_UpdateState`
dipindah sebelum `step_ResolveSystem`, skor yang di-commit ke `totalScore`
adalah nilai lama, bukan hasil dari kartu yang baru dimainkan. Jika
`phase_InitBlind` dihilangkan, deck tidak pernah di-reset dan skor dari
blind sebelumnya terbawa — pemain bisa mulai dengan skor yang sudah
melebihi target tanpa bermain sama sekali. Jika `phase_Shop` dipindah
sebelum `phase_EvaluateResult`, pemain bisa membeli upgrade bahkan setelah
gagal melewati blind.

Setiap perubahan urutan ini bukan sekadar bug kecil — mereka merusak
**kontrak logis** antara fase-fase yang membuat game bisa dimainkan
dengan benar.

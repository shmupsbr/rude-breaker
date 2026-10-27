# Rude Breaker — PC port (engine only)

🇬🇧 [English](#english) · 🇧🇷 [Português](#português)

---

## English

An unofficial PC (Windows) port of **Rude Breaker**, the vertical shooter by Compile released on *Disc Station Vol. 10* (PC-98, 1996). The original x86 game code runs inside an interpreter with a modern SDL2 host: vsync, gamepad support, remappable keys, a sound test with a VU meter, Training Mode, a saved hi-score table and more. See `docs/MANUAL.md`.

### Why the game files are not included
This repository contains **only the engine** (the code written for this port). It does **not** include any of the original game's files, and never will:

- **Rude Breaker is not free software or shareware.** It was a commercial release by Compile, and its rights are still active: D4 Enterprise re-released it on **Project EGG** in 2014.
- The game's graphics, music, sound, stage data and program (`SGM1.DAT`, `SONG.DAT`, `FPLAY.COM`) belong to their rights holders. Redistributing them, even for free, is not allowed.
- The YM2608 rhythm ROM (`ym2608_adpcm_rom.bin`) is Yamaha's property and is not included either.

You must supply these files from **your own legal copy** of the game.

### Building the play package
1. Get the game legally, for example from [Project EGG](https://www.amusement-center.com/), or from your own *Disc Station Vol. 10* disc.
2. Extract these files from the Rude Breaker disk: `SGM1.DAT`, `SONG.DAT` and `FPLAY.COM`.
3. Obtain `ym2608_adpcm_rom.bin` by dumping it from your own hardware.
4. Download or build `rudebreaker.exe`, then put everything in one folder:
   ```
   Rude Breaker PC/
     rudebreaker.exe
     SDL2.dll
     SGM1.DAT
     SONG.DAT
     FPLAY.COM
     ym2608_adpcm_rom.bin
   ```
5. Run `rudebreaker.exe`.

The rhythm ROM is optional: without it, the drums are silent. The other three files are required.

### Distribution notice
Do **not** distribute the game files. You may only use them if you own the game legally: the original *Disc Station Vol. 10* disc, or a purchase or subscription through Project EGG. If you do not own the game, you may play it for no more than **24 hours, for testing purposes only**, and must then delete the game files.

### Legal
This is a non-commercial fan project with no affiliation to Compile, D4 Enterprise or Yamaha. Third-party licenses: SDL2 (zlib) and ymfm (BSD-3-Clause).

---

## Português

Port não oficial pra PC (Windows) do **Rude Breaker**, o shooter vertical da Compile lançado na *Disc Station Vol. 10* (PC-98, 1996). O código x86 original roda dentro de um interpretador com uma camada SDL2 moderna: vsync, suporte a controle, teclas configuráveis, sound test com VU meter, Training Mode, tabela de recordes salva e mais. Veja `docs/MANUAL.md`.

### Por que os arquivos do jogo não estão incluídos
Este repositório contém **só o motor** (o código escrito pra este port). Ele **não** inclui nenhum arquivo do jogo original, e nunca vai incluir:

- **Rude Breaker não é software livre nem shareware.** Foi um lançamento comercial da Compile, e os direitos continuam ativos: a D4 Enterprise relançou o jogo na **Project EGG** em 2014.
- Os gráficos, músicas, sons, fases e o programa do jogo (`SGM1.DAT`, `SONG.DAT`, `FPLAY.COM`) pertencem aos donos dos direitos. Redistribuí-los, mesmo de graça, não é permitido.
- A ROM de ritmo do YM2608 (`ym2608_adpcm_rom.bin`) é propriedade da Yamaha e também não vem junto.

Você precisa tirar esses arquivos da **sua própria cópia legal** do jogo.

### Como montar o pacote pra jogar
1. Consiga o jogo legalmente, por exemplo na [Project EGG](https://www.amusement-center.com/) ou no seu próprio disco da *Disc Station Vol. 10*.
2. Extraia estes arquivos do disco do Rude Breaker: `SGM1.DAT`, `SONG.DAT` e `FPLAY.COM`.
3. Obtenha a `ym2608_adpcm_rom.bin` fazendo o dump do seu próprio hardware.
4. Baixe ou compile o `rudebreaker.exe` e coloque tudo numa pasta só:
   ```
   Rude Breaker PC/
     rudebreaker.exe
     SDL2.dll
     SGM1.DAT
     SONG.DAT
     FPLAY.COM
     ym2608_adpcm_rom.bin
   ```
5. Rode o `rudebreaker.exe`.

A ROM de ritmo é opcional: sem ela, a bateria fica muda. Os outros três arquivos são obrigatórios.

### Aviso de distribuição
**Não** distribua os arquivos do jogo. Você só pode usá-los se tiver o jogo legalmente: o disco original da *Disc Station Vol. 10*, ou uma compra ou assinatura pela Project EGG. Se você não tem o jogo, pode jogá-lo por no máximo **24 horas, só pra fins de teste**, e depois deve apagar os arquivos do jogo.

### Aviso legal
Projeto de fã, sem fins comerciais e sem vínculo com a Compile, a D4 Enterprise ou a Yamaha. Licenças de terceiros: SDL2 (zlib) e ymfm (BSD-3-Clause).

# Rude Breaker — PC port (engine only)

🇬🇧 [English](README.md#english) · 🇧🇷 [Português](README.md#português) · 🇯🇵 [日本語](README.ja.md)

---

## English

An unofficial PC (Windows and Linux) port of **Rude Breaker**, the vertical shooter by Compile released on *Disc Station Vol. 10* (PC-98, 1996). The original x86 game code runs inside an interpreter with a modern SDL2 host: vsync, gamepad support, remappable keys, a sound test with a VU meter, Training Mode, a saved hi-score table and more. The music is the original driver playing on an emulated YM2203, the PC-98 FM sound chip. See `docs/MANUAL.md`.

### Why the game files are not included
This repository contains **only the engine** (the code written for this port). It does **not** include any of the original game's files, and never will:

- **Rude Breaker is not free software or shareware.** It was a commercial release by Compile, and its rights are still active: D4 Enterprise re-released it on **Project EGG** in 2014.
- The game's graphics, music, sound, stage data and program (`SGM1.DAT`, `SONG.DAT`, `FPLAY.COM`) belong to their rights holders. Redistributing them, even for free, is not allowed.

You must supply these files from **your own legal copy** of the game.

### Building the play package
1. Get the game legally, for example from [Project EGG](https://www.amusement-center.com/), or from your own *Disc Station Vol. 10* disc.
2. Extract these files from the Rude Breaker disk: `SGM1.DAT`, `SONG.DAT` and `FPLAY.COM`.
3. Download `releases/RudeBreaker-PC-pr9o.zip` from this repository (or build `rudebreaker.exe` yourself), unzip it, and add the game files so the folder looks like this:
   ```
   Rude Breaker PC/
     rudebreaker.exe
     SDL2.dll
     SGM1.DAT
     SONG.DAT
     FPLAY.COM
   ```
4. Run `rudebreaker.exe`.

These three files are all you need. No other file (BIOS, sound ROM or the like) is required: the music uses YM2203 emulation, which needs no ROM.

### Linux
A Linux x86_64 build is in `releases/RudeBreaker-Linux-pr9o.zip`. It ships its own `libSDL2-2.0.so.0` and runs on most distributions with glibc 2.17 or newer (X11 or Wayland; PulseAudio, PipeWire or ALSA).

1. Unzip it and add the same game files, so the folder looks like this:
   ```
   Rude Breaker Linux/
     rudebreaker
     libSDL2-2.0.so.0
     SGM1.DAT
     SONG.DAT
     FPLAY.COM
   ```
   The file names must match exactly, including upper case (`SGM1.DAT`, not `sgm1.dat`).
2. In a terminal, inside that folder:
   ```
   chmod +x rudebreaker
   ./rudebreaker
   ```

To build it yourself, install a C/C++ compiler, `make`, `pkg-config` and the SDL2 development package (Debian/Ubuntu: `sudo apt install build-essential pkg-config libsdl2-dev`; Fedora: `sudo dnf install gcc-c++ make SDL2-devel`; Arch: `sudo pacman -S base-devel sdl2`), then run:
```
make linux
```
The binary is `build/rudebreaker`. `make linux` looks for `libSDL2-2.0.so.0` next to the binary first and falls back to the system library, and links the C++ runtime statically.

### Distribution notice
Do **not** distribute the game files. You may only use them if you own the game legally: the original *Disc Station Vol. 10* disc, or a purchase or subscription through Project EGG. If you do not own the game, you may play it for no more than **24 hours, for testing purposes only**, and must then delete the game files.

### Legal
This is a non-commercial fan project with no affiliation to Compile or D4 Enterprise. Third-party licenses: SDL2 (zlib) and ymfm (BSD-3-Clause).

---

## Português

Port não oficial pra PC (Windows e Linux) do **Rude Breaker**, o shooter vertical da Compile lançado na *Disc Station Vol. 10* (PC-98, 1996). O código x86 original roda dentro de um interpretador com uma camada SDL2 moderna: vsync, suporte a controle, teclas configuráveis, sound test com VU meter, Training Mode, tabela de recordes salva e mais. A música é o driver original tocando num YM2203 emulado, o chip de som FM do PC-98. Veja `docs/MANUAL.md`.

### Por que os arquivos do jogo não estão incluídos
Este repositório contém **só o motor** (o código escrito pra este port). Ele **não** inclui nenhum arquivo do jogo original, e nunca vai incluir:

- **Rude Breaker não é software livre nem shareware.** Foi um lançamento comercial da Compile, e os direitos continuam ativos: a D4 Enterprise relançou o jogo na **Project EGG** em 2014.
- Os gráficos, músicas, sons, fases e o programa do jogo (`SGM1.DAT`, `SONG.DAT`, `FPLAY.COM`) pertencem aos donos dos direitos. Redistribuí-los, mesmo de graça, não é permitido.

Você precisa tirar esses arquivos da **sua própria cópia legal** do jogo.

### Como montar o pacote pra jogar
1. Consiga o jogo legalmente, por exemplo na [Project EGG](https://www.amusement-center.com/) ou no seu próprio disco da *Disc Station Vol. 10*.
2. Extraia estes arquivos do disco do Rude Breaker: `SGM1.DAT`, `SONG.DAT` e `FPLAY.COM`.
3. Baixe o `releases/RudeBreaker-PC-pr9o.zip` deste repositório (ou compile o `rudebreaker.exe`), descompacte e coloque os arquivos do jogo junto, deixando a pasta assim:
   ```
   Rude Breaker PC/
     rudebreaker.exe
     SDL2.dll
     SGM1.DAT
     SONG.DAT
     FPLAY.COM
   ```
4. Rode o `rudebreaker.exe`.

Só esses três arquivos são necessários. Não precisa de nenhum outro (BIOS, ROM de som ou parecido): a música usa emulação do YM2203, que não precisa de ROM.

### Linux
Tem uma versão pra Linux x86_64 em `releases/RudeBreaker-Linux-pr9o.zip`. Ela já vem com a própria `libSDL2-2.0.so.0` e roda na maioria das distribuições com glibc 2.17 ou mais nova (X11 ou Wayland; PulseAudio, PipeWire ou ALSA).

1. Descompacte e coloque os mesmos arquivos do jogo, deixando a pasta assim:
   ```
   Rude Breaker Linux/
     rudebreaker
     libSDL2-2.0.so.0
     SGM1.DAT
     SONG.DAT
     FPLAY.COM
   ```
   Os nomes precisam ser exatamente esses, com maiúsculas (`SGM1.DAT`, não `sgm1.dat`).
2. No terminal, dentro dessa pasta:
   ```
   chmod +x rudebreaker
   ./rudebreaker
   ```

Pra compilar você mesmo, instale um compilador C/C++, `make`, `pkg-config` e o pacote de desenvolvimento do SDL2 (Debian/Ubuntu: `sudo apt install build-essential pkg-config libsdl2-dev`; Fedora: `sudo dnf install gcc-c++ make SDL2-devel`; Arch: `sudo pacman -S base-devel sdl2`) e rode:
```
make linux
```
O binário fica em `build/rudebreaker`. O `make linux` procura a `libSDL2-2.0.so.0` primeiro na pasta do binário e, se não achar, usa a do sistema; o runtime de C++ é linkado estático.

### Aviso de distribuição
**Não** distribua os arquivos do jogo. Você só pode usá-los se tiver o jogo legalmente: o disco original da *Disc Station Vol. 10*, ou uma compra ou assinatura pela Project EGG. Se você não tem o jogo, pode jogá-lo por no máximo **24 horas, só pra fins de teste**, e depois deve apagar os arquivos do jogo.

### Aviso legal
Projeto de fã, sem fins comerciais e sem vínculo com a Compile ou a D4 Enterprise. Licenças de terceiros: SDL2 (zlib) e ymfm (BSD-3-Clause).

# Rude Breaker 30th AE — port para PC (só o motor)

🇬🇧 [English](README.md) · 🇧🇷 [Português](README.pt-BR.md) · 🇯🇵 [日本語](README.ja.md)

---

## Português

**Rude Breaker 30th AE** (30th Anniversary Edition) comemora os 30 anos do lançamento do jogo, em 5 de abril de 1996. É um port não oficial pra PC (Windows e Linux) do **Rude Breaker**, o shooter vertical da Compile lançado na *Disc Station Vol. 10* (PC-98, 1996). O código x86 original roda dentro de um interpretador com uma camada SDL2 moderna: vsync, suporte a controle, teclas configuráveis, sound test com VU meter, Training Mode, tabela de recordes salva, botão SPEED pra mudar a velocidade da nave durante o jogo (com indicador SPD), sistema de RANK opcional no estilo do Zanac e mais. A música é o driver original tocando num YM2203 emulado, o chip de som FM do PC-98. Manuais: [English](docs/MANUAL.md) · [Português](docs/MANUAL.pt-BR.md) · [日本語](docs/MANUAL.ja.md) (a seção 2 explica como montar os arquivos do jogo).

### Uma palavra pessoal
Este programa nasceu para criar alterações em um jogo do qual sou muito fã, de uma companhia que me ensinou a gostar de videogames. Não pretendo, nunca, prejudicar os atuais detentores dos direitos, quem quer que sejam. Se eles quiserem, basta me procurar por email (shmupsbr@gmail.com) ou pelo X/Twitter ([@shmupsbr](https://x.com/shmupsbr)), e eu tiro do ar, se for o caso. Os detentores têm total direito de reutilizar o código deste repositório, em parte ou por inteiro, sem me dar nenhum crédito (mas, se derem, vou ficar muito feliz!).

— Filipe Gracioli (ShmupsBR)

### Novidades do 30th AE
- **Botão SPEED** (C / Ctrl, Y no controle): muda a velocidade da nave durante o jogo, no ciclo LOW / MID / MAX / MID / LOW (lenta, normal, máxima). Cada partida começa em MID por padrão, ou na DEFAULT SPEED escolhida no menu. A barra de cima mostra como `SPD LOW`, `SPD MID` ou `SPD MAX`.
- **RANK** (menu GAMEPLAY, CLASSIC por padrão): rank no estilo do Zanac. Sobe enquanto você sobrevive e pontua, cai pela metade quando você perde uma vida e acelera os tiros inimigos (nunca mais lentos que na dificuldade escolhida). Os bônus continuam seguindo a dificuldade escolhida. No VERY EASY e no EASY o rank só sobe até a metade; no HARD e no VERY HARD ele já começa na metade. Desligado, o jogo fica igual ao original.
- **RANK MODE ADVANCED** (RANK em ADVANCED; CLASSIC é o padrão): rank oculto de 0 a 1000, sem números na tela (uma barra fina sob o gráfico do RANK). Sobe com o tempo vivo, o poder do tiro principal, sub-armas e medalhas pegas, vidas em estoque e tiro segurado (+10% com NO DEATH), e cai 15–25% ao perder uma vida. Controla a velocidade dos tiros inimigos de forma contínua (nunca abaixo da sua DIFFICULTY, até VERY HARD e +15%), liga os tiros de revanche a partir de 700 e aumenta os pontos em até +50%. Detalhes na seção 6 do manual.
- **Dois loops**: vencer a fase 5 começa a fase 1 de novo como loop 2, mantendo pontuação, vidas e armas. O loop 2 sobe a dificuldade base um nível (até VERY HARD) e dobra todo valor controlado pelo rank (a velocidade dos tiros inimigos nunca passa do VERY HARD; a aceleração no teto nunca passa de +50%). A partida termina depois do loop 2. Não há opção LOOP. Se você pegou todas as medalhas nos dois loops e não perdeu a nave nenhuma vez, o chefe que entra e sai em toda fase volta depois da fase 5 do loop 2 como Kraken. Detalhes na seção 6 do manual.
- **HITBOX** (menu GAMEPLAY, desligado por padrão): mostra uma bolinha na nave com o tamanho exato da sua caixa de colisão real. Cores: PINK, LILAC, BLUE, GREEN, RED, WHITE; a cor dos tiros inimigos é sempre pulada.
- **SOUND CHANNELS** (menu GAMEPLAY): EXTRA (padrão) toca os efeitos num segundo chip emulado, e a música nunca é cortada; ORIGINAL divide o chip FM entre música e efeitos, como no PC-98.
- **SCALING** (menu GAMEPLAY): AUTO (padrão: INTEGER quando ocupa pelo menos 80% da altura da tela, senão SHARP), INTEGER (escala inteira, bordas pretas; com SIDE PANELS é a área de jogo que fica em escala inteira), SHARP (ocupa a tela mantendo a proporção, sem borrar) ou SMOOTH (ocupa a tela com filtro bilinear).
- **ASPECT** (menu GAMEPLAY): 4:3 (padrão) mostra a imagem 640x400 no formato do monitor PC-98, com pixels 1,2 vez mais altos que largos (1440x1080 numa tela 1080p); 8:5 mostra a imagem 640x400 com pixels quadrados (16:10).
- **RANK OFF**: o rank não afeta o jogo; o quadro RANK mostra RANK SYSTEM / OFFLINE em verde, sem os números.
- **FULLSCREEN** (menu GAMEPLAY, desligado por padrão) e **F11** a qualquer momento: ocupa o monitor mantendo a proporção, com os painéis laterais em telas largas, e esconde o ponteiro do mouse.
- **RETURN** no menu do título volta ao começo (logo, demo, título) e **EXIT** fecha o jogo, como no original.
- O menu GAMEPLAY mostra as opções em linhas seguidas, sem linhas em branco entre elas.
- **Paleta MODERN** agora é o padrão.
- **Troca de arma durante o jogo** (SHIP SELECT OFF) agora é o padrão: pegue as cápsulas V, L ou M pra trocar de arma.
- O **Esc** só pausa durante a partida.
- O tiro teleguiado do chefe final agora vira o traço na direção em que voa, como no original.
- Tela de título nova, com a marca 30th AE e o crédito © ShmupsBR 2026, e uma linha de crédito do 30th AE no final. A tela de ranking continua com o visual original. O jogo começa no logo da Compile, como no original.

### Por que os arquivos do jogo não estão incluídos
Este repositório contém **só o motor** (o código escrito pra este port). Ele **não** inclui nenhum arquivo do jogo original, e nunca vai incluir:

- **Rude Breaker não é software livre nem shareware.** Foi um lançamento comercial da Compile, e os direitos continuam ativos: a D4 Enterprise relançou o jogo na **Project EGG** em 2014.
- Os gráficos, músicas, sons, fases e o programa do jogo (`SGM1.DAT`, `SONG.DAT`, `FPLAY.COM`) pertencem aos donos dos direitos. Redistribuí-los, mesmo de graça, não é permitido.

Você precisa tirar esses arquivos da **sua própria cópia legal** do jogo.

### Como montar o pacote pra jogar
1. Consiga o jogo legalmente, por exemplo na [Project EGG](https://www.amusement-center.com/) ou no seu próprio disco da *Disc Station Vol. 10*.
2. Extraia estes arquivos do disco do Rude Breaker: `SGM1.DAT`, `SONG.DAT` e `FPLAY.COM`.
3. Baixe o `releases/RudeBreaker-Windows-v0.10.0.zip` (versão 0.10.0; `rudebreaker --version` mostra) deste repositório (ou compile o `rudebreaker.exe`), descompacte e coloque os arquivos do jogo junto, deixando a pasta assim:
   ```
   Rude Breaker PC/
     rudebreaker.exe
     SDL2.dll
     SGM1.DAT
     SONG.DAT
     FPLAY.COM
   ```
4. Rode o `rudebreaker.exe`.

Em vez dos arquivos soltos, você também pode colocar uma imagem de disco PC-98 do jogo junto do programa (`.hdm`, `.fdi`, `.d88`, `.hdi`, `.nhd`, ou um `.zip` com uma delas); os arquivos são lidos direto dela. Veja a seção 2 do manual.

Só esses três arquivos são necessários. Não precisa de nenhum outro (BIOS, ROM de som ou parecido): a música usa emulação do YM2203, que não precisa de ROM.

### Linux
Tem uma versão pra Linux x86_64 em `releases/RudeBreaker-Linux-x86_64-v0.10.0.zip`. Ela já vem com a própria `libSDL2-2.0.so.0` e roda na maioria das distribuições com glibc 2.27 ou mais nova (X11 ou Wayland; PulseAudio, PipeWire ou ALSA).

**Wayland ou X11:** o programa abre primeiro em X11; num desktop Wayland isso quer dizer XWayland, que funciona em todo lugar. Para Wayland nativo, abra com `SDL_VIDEODRIVER=wayland ./rudebreaker`; para forçar X11, `SDL_VIDEODRIVER=x11 ./rudebreaker`.

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

### Raspberry Pi
Os pacotes para Raspberry Pi são `RudeBreaker-RaspberryPi-*.zip`. Coloque os arquivos do jogo como no Linux e, dentro da pasta, rode `chmod +x rudebreaker` e `./rudebreaker`. Se a janela der problema no Wayland, tente `SDL_VIDEODRIVER=x11 ./rudebreaker`; no Pi OS Lite, use `SDL_VIDEODRIVER=kmsdrm SDL_AUDIODRIVER=alsa ./rudebreaker` no console. O recomendado é um Pi 4 ou Pi 5. São dois pacotes: `RudeBreaker-RaspberryPi-aarch64-v0.10.0.zip` para o Raspberry Pi OS de 64 bits (`uname -m` mostra `aarch64`) e `RudeBreaker-RaspberryPi-armhf-v0.10.0.zip` para o Raspberry Pi OS de 32 bits (`armv6l` ou `armv7l`, qualquer modelo de Pi). "cannot execute binary file" quer dizer que é o outro pacote. Os detalhes estão no manual.

### macOS
`releases/RudeBreaker-macOS-Universal-v0.10.0.zip` traz o `Rude Breaker.app` (Apple Silicon e Intel, macOS 10.13 ou mais novo). Coloque os arquivos do jogo em `Rude Breaker.app/Contents/Resources` (botão direito > Mostrar Conteúdo do Pacote). O app tem só assinatura ad-hoc, sem notarização: na primeira vez, botão direito > Abrir. Veja o `README-Mac.txt` no zip.

### Aviso de distribuição
**Não** distribua os arquivos do jogo. Você só pode usá-los se tiver o jogo legalmente: o disco original da *Disc Station Vol. 10*, ou uma compra ou assinatura pela Project EGG. Se você não tem o jogo, pode jogá-lo por no máximo **24 horas, só pra fins de teste**, e depois deve apagar os arquivos do jogo.

### Aviso legal
Projeto de fã, sem fins comerciais e sem vínculo com a Compile ou a D4 Enterprise. Licenças de terceiros: SDL2 (zlib) e ymfm (BSD-3-Clause). Fonte dos painéis: Robotica, de Courtney Novits (fonte gratuita).

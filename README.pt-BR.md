# Rude Breaker 30th AE — port para PC (só o motor)

🇬🇧 [English](README.md) · 🇧🇷 [Português](README.pt-BR.md) · 🇯🇵 [日本語](README.ja.md)

---

## Português

**Rude Breaker 30th AE** (30th Anniversary Edition) comemora os 30 anos do lançamento do jogo, em 5 de abril de 1996. É um port não oficial pra PC (Windows e Linux) do **Rude Breaker**, o shooter vertical da Compile lançado na *Disc Station Vol. 10* (PC-98, 1996). O código x86 original roda dentro de um interpretador com uma camada SDL2 moderna: vsync, suporte a controle, teclas configuráveis, sound test com VU meter, Training Mode, tabela de recordes salva, botão SPEED pra mudar a velocidade da nave durante o jogo (com indicador SPD), sistema de RANK opcional no estilo do Zanac e mais. A música é o driver original tocando num YM2203 emulado, o chip de som FM do PC-98. Manuais: [English](docs/MANUAL.md) · [Português](docs/MANUAL.pt-BR.md) · [日本語](docs/MANUAL.ja.md) (a seção 2 explica como montar os arquivos do jogo).

### Uma palavra pessoal
Este programa nasceu para criar alterações em um jogo do qual sou muito fã, de uma companhia que me ensinou a gostar de videogames. Não pretendo, nunca, prejudicar os atuais detentores dos direitos, quem quer que sejam. Se eles quiserem, basta me procurar por email (shmupsbr@gmail.com) ou pelo X/Twitter ([@shmupsbr](https://x.com/shmupsbr)), e eu tiro do ar, se for o caso. Os detentores têm total direito de reutilizar o código deste repositório, em parte ou por inteiro, sem me dar nenhum crédito (mas, se derem, vou ficar muito feliz!).

— Filipe Gracioli (ShmupsBR)

### Novidades do 30th AE
- **Botão SPEED** (C / Ctrl, Y no controle): muda a velocidade da nave durante o jogo, no ciclo LOW / MID / MAX / MID / LOW (lenta, normal, máxima). Cada partida começa em MID por padrão, ou na MOVE SPEED escolhida no menu. A barra de cima mostra como `SPD LOW`, `SPD MID` ou `SPD MAX`.
- **RANK** (menu GAMEPLAY, ligado por padrão): rank no estilo do Zanac. Sobe enquanto você sobrevive e pontua, cai pela metade quando você perde uma vida e acelera os tiros inimigos (nunca mais lentos que na dificuldade escolhida). Os bônus continuam seguindo a dificuldade escolhida. No VERY EASY e no EASY o rank só sobe até a metade; no HARD e no VERY HARD ele já começa na metade. Desligado, o jogo fica igual ao original.
- **LOOP** (menu GAMEPLAY, desligado por padrão): com LOOP ON, vencer a fase 5 começa a fase 1 de novo como loop 2, 3 e assim por diante, mantendo pontuação, vidas e armas. Cada loop sobe a dificuldade base um nível (até VERY HARD), e todo valor controlado pelo rank é multiplicado pelo número do loop (limite x8; a velocidade dos tiros inimigos nunca passa do VERY HARD; a aceleração no teto nunca passa de +50%). Perder uma vida volta para a dificuldade base do loop. A partir do loop 2, o título do quadro RANK mostra LOOP 2, LOOP 3... OFF mantém o final original. Detalhes na seção 6 do manual.
- **HITBOX** (menu GAMEPLAY, desligado por padrão): mostra uma bolinha na nave com o tamanho exato da sua caixa de colisão real. Cores: PINK, LILAC, BLUE, GREEN, RED, WHITE; a cor dos tiros inimigos é sempre pulada.
- **FULLSCREEN** (menu GAMEPLAY, desligado por padrão) e **F11** a qualquer momento: ocupa o monitor mantendo a proporção, com os painéis laterais em telas largas, e esconde o ponteiro do mouse.
- **RETURN** no menu do título volta ao começo (logo, demo, título) e **EXIT** fecha o jogo, como no original.
- O menu GAMEPLAY mostra as opções em linhas seguidas, sem linhas em branco entre elas.
- **Paleta MODERN** agora é o padrão.
- **Troca de arma durante o jogo** (SHIP SELECT OFF) agora é o padrão: pegue as cápsulas V, L ou M pra trocar de arma.
- O **Esc** só pausa durante a partida.
- O tiro teleguiado do chefe final agora vira o traço na direção em que voa, como no original.
- Um cartão curto do 30th AE ("Rude Breaker - 30th Anniversary Edition by ShmupsBR 2026") antes do logo da Compile; o tiro ou o Start pula.
- Tela de título nova, com a marca 30th AE e o crédito © ShmupsBR 2026, e uma linha de crédito do 30th AE no final. A tela de ranking continua com o visual original.

### Por que os arquivos do jogo não estão incluídos
Este repositório contém **só o motor** (o código escrito pra este port). Ele **não** inclui nenhum arquivo do jogo original, e nunca vai incluir:

- **Rude Breaker não é software livre nem shareware.** Foi um lançamento comercial da Compile, e os direitos continuam ativos: a D4 Enterprise relançou o jogo na **Project EGG** em 2014.
- Os gráficos, músicas, sons, fases e o programa do jogo (`SGM1.DAT`, `SONG.DAT`, `FPLAY.COM`) pertencem aos donos dos direitos. Redistribuí-los, mesmo de graça, não é permitido.

Você precisa tirar esses arquivos da **sua própria cópia legal** do jogo.

### Como montar o pacote pra jogar
1. Consiga o jogo legalmente, por exemplo na [Project EGG](https://www.amusement-center.com/) ou no seu próprio disco da *Disc Station Vol. 10*.
2. Extraia estes arquivos do disco do Rude Breaker: `SGM1.DAT`, `SONG.DAT` e `FPLAY.COM`.
3. Baixe o `releases/RudeBreaker-PC-pr9p.zip` deste repositório (ou compile o `rudebreaker.exe`), descompacte e coloque os arquivos do jogo junto, deixando a pasta assim:
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
Tem uma versão pra Linux x86_64 em `releases/RudeBreaker-Linux-pr9p.zip`. Ela já vem com a própria `libSDL2-2.0.so.0` e roda na maioria das distribuições com glibc 2.17 ou mais nova (X11 ou Wayland; PulseAudio, PipeWire ou ALSA).

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
Projeto de fã, sem fins comerciais e sem vínculo com a Compile ou a D4 Enterprise. Licenças de terceiros: SDL2 (zlib) e ymfm (BSD-3-Clause). Fonte dos painéis: Robotica, de Courtney Novits (fonte gratuita).

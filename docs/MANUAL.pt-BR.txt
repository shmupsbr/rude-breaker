RUDE BREAKER 30th AE - PORT PARA PC - MANUAL DO USUÁRIO
=======================================================

Versão 0.10.1 - 04/11/2026

O programa mostra o mesmo número: rudebreaker --version, o título da
janela e a linha de cima da página CONFIG (VER 0.10.1).

Também em: inglês (MANUAL.md / .txt), japonês (MANUAL.ja.md / .txt).


1. O QUE É ISTO
---------------

Este é um port para PC (Windows e Linux) do Rude Breaker, o shoot 'em
up vertical que a Compile fez para o NEC PC-98 (Disc Station, 1996).

Rude Breaker 30th AE (30th Anniversary Edition, edição de 30 anos)
marca os 30 anos do lançamento do jogo na Disc Station Vol. 10, em 5
de abril de 1996. É o mesmo jogo, com estes acréscimos:

  - Botão SPEED: muda a velocidade da nave durante a partida, com o
    indicador SPD na barra de cima (seção 3).
  - RANK: um sistema de rank opcional no estilo do Zanac, ligado por
    padrão (seções 4 e 6).
  - A paleta MODERN agora é o padrão (a ORIGINAL continua disponível).
  - A troca de arma durante a partida (SHIP SELECT OFF) agora é o
    padrão (seção 4).
  - O Esc só pausa durante a partida, nunca no título ou nos menus.
  - O tiro teleguiado do chefe final agora vira o sprite do rastro na
    direção em que voa, como no original.
  - Tela de título nova, com a marca 30th AE e um crédito ShmupsBR
    2026.

O port não é um remake. Ele roda os scripts e a lógica originais do
jogo, a partir dos arquivos de dados originais. Padrões dos inimigos,
pontuação, HP dos chefes e dificuldade vêm todos do código original.
A música é o driver FPLAY original tocando num YM2203 emulado (ymfm),
o chip de som FM do PC-98 para o qual o jogo foi escrito.

O lado do PC cuida da janela, da saída de som, do teclado e do
controle, do ritmo dos quadros e de um conjunto de opções no menu
CONFIG.

IMPORTANTE: os dados do jogo (SGM1.DAT, SONG.DAT, FPLAY.COM) têm
direitos autorais. Eles NÃO fazem parte do port público e não podem
ser redistribuídos. Você usa os da sua própria cópia legal do jogo
(seção 2). Uma build de teste privada que os inclua é só para o seu
testador. Por favor, não compartilhe.


2. INSTALAÇÃO, EXECUÇÃO E ARQUIVOS DO JOGO
------------------------------------------

Descompacte a pasta em qualquer lugar. Não é preciso instalar nada.

  Windows: rode o rudebreaker.exe.
  Linux:   num terminal, dentro da pasta: chmod +x rudebreaker, e
           depois ./rudebreaker
  Raspberry Pi: igual ao Linux; veja RASPBERRY PI no fim desta
           seção.

Arquivos do port:

  rudebreaker.exe       O jogo (Windows).
  SDL2.dll              Biblioteca de janela, som e controle (Windows).
  rudebreaker           O jogo (Linux x86_64).
  libSDL2-2.0.so.0      A mesma biblioteca (Linux).
  LICENSE-ymfm.txt      Licença do emulador do chip de som ymfm.
  LICENSE-SDL2.txt      Licença da biblioteca SDL2.
  MANUAL.txt / .md      O manual.

ARQUIVOS DO JOGO

O port não tem dados de jogo próprios. Ele lê os dados da sua própria
cópia do Rude Breaker (Disc Station Vol. 10, PC-98):

  SGM1.DAT     obrigatório  Fases, sprites, scripts e o programa do jogo.
  FPLAY.COM    obrigatório  O driver de música original.
  SONG.DAT     obrigatório  Os dados de música e efeitos sonoros.
  RBRANK.DAT   opcional     O arquivo original de recordes e opções.

SGC.COM, READJS.COM e RUDEBR.BAT também estão no disco; eles não são
lidos e não são necessários. Não precisa de BIOS, ROM de fonte nem
ROM de som: a música usa emulação do YM2203, que não precisa de ROM.

Há duas formas de entregar os arquivos ao port:

  1. Arquivos soltos. Copie SGM1.DAT, FPLAY.COM e SONG.DAT (e o
     RBRANK.DAT, se quiser seus recordes antigos) para a pasta do
     jogo, junto do programa, ou para uma subpasta chamada RUDEBR:

       Rude Breaker PC/
         rudebreaker.exe
         SDL2.dll
         SGM1.DAT
         FPLAY.COM
         SONG.DAT

     No Linux os nomes precisam ser exatamente esses, em maiúsculas
     (SGM1.DAT, não sgm1.dat).

  2. Uma imagem de disco. Coloque uma imagem de disco PC-98 do jogo
     na pasta do jogo (ou na subpasta RUDEBR) do jeito que ela está.
     Formatos aceitos: .hdm, .fdi, .d88, .hdi, .nhd, ou um .zip com
     uma dessas dentro. O port lê os arquivos direto do sistema de
     arquivos FAT da imagem, inclusive de uma pasta RUDEBR dentro
     dela. Nada é extraído para o disco. Se houver várias imagens,
     vale a primeira que tiver os arquivos, testando .hdm, .zip,
     .fdi, .d88, .hdi e .nhd, nessa ordem.

Onde o port procura, nesta ordem. Uma pasta ou imagem indicada em 1
ou 2 é sempre usada, mesmo sem os arquivos (aí aparece a janela
descrita abaixo). Os lugares 3 a 5 só valem se tiverem os arquivos;
se nenhum tiver, o aviso mostra a pasta do programa:

  1. Uma pasta ou imagem de disco passada como argumento do programa,
     por exemplo: rudebreaker.exe D:\jogos\rudebr.hdm
  2. A pasta ou imagem na variável de ambiente RUDEBR_ASSETS.
  3. A própria pasta do programa: arquivos soltos, depois imagem.
  4. A subpasta RUDEBR da pasta do programa.
  5. assets/raw/RUDEBR dentro da pasta atual (para desenvolvedores).

Se faltar um arquivo obrigatório, o jogo não abre. Uma janela
"disk files needed" mostra a pasta ou imagem que ele tentou e, para
cada arquivo, se foi encontrado (found) ou se está faltando
(MISSING). Coloque os arquivos que faltam e rode o jogo de novo. Se
faltar só o RBRANK.DAT, o jogo começa com a tabela de recordes
original e as opções padrão, e cria o arquivo quando salvar.

Arquivos que o jogo cria:

  RBRANK.DAT       O arquivo de save original: a tabela de recordes e
                   as opções do jogo (dificuldade e as opções de
                   GAMEPLAY). Fica na pasta de onde vêm os arquivos do
                   jogo (junto da imagem de disco, se você usar uma).
                   Apague para zerar recordes e opções.
  rudebreaker.cfg  Opções do PC: paleta, rank, treino, teclas e botões
                   do controle. Fica junto do programa.
  rudebreaker.ini  Escolha do ritmo dos quadros: pacing=smooth ou
                   pacing=exact.
  pace.txt         Um pequeno relatório de tempo (veja a seção 7).
                   Pode ser apagado.


RASPBERRY PI

O pacote para Raspberry Pi é para o Raspberry Pi OS de 64 bits:

  RudeBreaker-RaspberryPi-*.zip    Raspberry Pi OS de 64 bits
                                   (aarch64). Pi 3, Pi 4, Pi 5,
                                   Pi Zero 2 W.

Para conferir o seu sistema, abra um terminal e rode:

  uname -m           aarch64 = 64 bits
  getconf LONG_BIT   64

Descompacte o pacote, coloque SGM1.DAT, SONG.DAT e FPLAY.COM junto
do programa (mesmos nomes, em maiúsculas) e, dentro dessa pasta:

  chmod +x rudebreaker
  ./rudebreaker

O pacote já traz a sua própria libSDL2-2.0.so.0, então não é preciso
instalar mais nada.

Com área de trabalho: no Raspberry Pi OS atual a área de trabalho
roda em Wayland, e o jogo usa ele por padrão. Se a janela não abrir
ou aparecer errada, force o X11:

  SDL_VIDEODRIVER=x11 ./rudebreaker

Raspberry Pi OS Lite (sem área de trabalho): rode o jogo no console
de texto, não por SSH, assim:

  SDL_VIDEODRIVER=kmsdrm SDL_AUDIODRIVER=alsa ./rudebreaker

O seu usuário precisa estar nos grupos video e input (o usuário
padrão já está). Se não estiver:

  sudo usermod -aG video,input $USER

e depois saia e entre de novo.

Desempenho: o recomendado é um Pi 4 ou Pi 5. No Pi 3, use uma janela
pequena em vez da tela cheia e feche os outros programas.

"cannot execute binary file: Exec format error" quer dizer que você
está num sistema de 32 bits ou pegou o pacote errado. Só o Raspberry
Pi OS de 64 bits é suportado; as versões de 32 bits não são mais feitas.


3. CONTROLES
------------

Teclado (padrão):

  Mover              Setas, W A S D, ou 8 4 6 2 do teclado numérico
  Tiro / confirmar   Z, Espaço, ou 0 do teclado numérico
  Modo               X, N, M, Enter, ou Enter do teclado numérico
                     (alterna os pods laterais entre ataque e defesa)
  Especial           Shift esquerdo ou direito
  Velocidade         C, Ctrl esquerdo ou direito (veja abaixo)
  Pausa / voltar     Esc
  Sair               F10
  Tela cheia         F11 (liga/desliga a qualquer momento; salvo
                     como a opção FULLSCREEN)
  Info de tempo      F8 (mostra o ritmo dos quadros, veja a seção 7)

Controle:

  Qualquer controle padrão de PC (XInput ou DirectInput) é detectado
  sozinho. Dá para ligar ou desligar o controle com o jogo rodando.

  Mover              Analógico esquerdo ou direcional
  Tiro / confirmar   Botão de baixo (A no controle do Xbox)
  Modo               Botão da direita (B)
  Especial           Botão da esquerda (X)
  Velocidade         Botão de cima (Y). Em controles sem layout
                     padrão: botão 4 (o quarto botão).
  Pausa              Start
  Sair               Back / Select

Botão SPEED: cada toque muda a velocidade da nave durante a partida,
subindo e descendo alternadamente:

  LOW  ->  MID  ->  MAX  ->  MID  ->  LOW  ->  MID  ...

  LOW  lenta
  MID  normal (padrão)
  MAX  rápida

A barra de cima mostra a velocidade atual como SPD LOW, SPD MID ou
SPD MAX.

Cada partida começa em MID. A mudança feita pelo botão vale até o fim
da partida. O quanto cada nível anda depende do SHIP SPEED em
GAMEPLAY.

PAUSA: o Esc ou o botão START do controle pausa só enquanto você está
jogando. O START é detectado sozinho: em controles conhecidos pelo SDL
(Xbox, PlayStation, Switch e a maioria) é o START/OPTIONS/+; em
joysticks simples é o botão 8 ou 10, ou o primeiro outro botão acima
de 10 que você apertar durante o jogo. Pausado, aparece um menu com
RESUME e MAIN MENU. RESUME (ou Esc/START/B de novo) volta ao jogo. MAIN MENU encerra a partida e volta à
sequência do título (começando pelo logo da Compile). No
título, na demo e nos menus ele não pausa. Nos menus, o Esc (teclado)
ou o botão B (controle) volta uma página.
O menu de pausa é desenhado por cima da imagem congelada do jogo, que
mantém o COLOR FILTER e os efeitos CRT ligados. O menu de pausa funciona também no controle: direcional ou
analógico para mover, A confirma, B ou START retoma.

A janela pode ser redimensionada. A imagem mantém a proporção
original de 640x400.


4. TELA DE TÍTULO E MENU CONFIG
-------------------------------

O logo, o título, a tela de ranking e a introdução são as telas
originais. O jogo começa direto no logo da Compile e na música dele,
como no original. Com SIDE
PANELS ligado, a linha de pontuação no alto da área de jogo fica uns
pixels para dentro de cada lado, para não ser cortada pelas molduras
dos painéis.
A tela de título mostra "30TH ANNIVERSARY EDITION" em branco,
centralizado sob o logo, com um brilho suave em volta das letras a
cada 5 segundos (o mesmo brilho aparece sob o logo no painel lateral
esquerdo), o PUSH TRIGGER mudando suavemente entre amarelo e verde a
cada 10 segundos, e as linhas de crédito (c) COMPILE 1996 e (c) SHMUPSBR
2026.
Aperte Tiro no título para começar, ou abra o CONFIG pelo menu do
título.
No menu do título, RETURN volta ao começo do jogo (logo da Compile,
demo e título, como no original) e EXIT fecha o jogo. As linhas do
menu ficam sobre a linha grande (c) COMPILE 1996 da arte do título;
é assim no original.

Modo atração (GAMEPLAY > ATTRACT MODE, ON por padrão): se ninguém
mexer no teclado ou no controle, o ciclo original (logo da Compile, voo
de introdução, título, ranking) roda uma vez e, assim que o ranking
termina, vem uma demo de 30 segundos de uma fase, sem música (só os
efeitos sonoros do jogo), com um INSERT COIN piscando no meio da tela, no estilo
fliperama (só de brincadeira: não precisa de ficha); então o ciclo de novo
e a próxima fase, da fase 1 à 5 e recomeça. Qualquer tecla ou botão
encerra a demo e volta à sequência do título. A demo não salva nada:
nem recorde, nem ranking, nem configurações. O movimento da nave na
demo segue o trajeto de um jogador de verdade; é uma gravação de
comandos nossa, não dado do jogo.

Os dois rankings são coisas diferentes. No modo atração o ranking faz
parte do ciclo original: aparece sozinho por alguns segundos e passa,
sem precisar de tecla (qualquer tecla sai do ciclo). O RANKING do
CONFIG abre a mesma tabela para olhar o tempo que quiser; fecha quando
você aperta uma tecla.

O CONFIG tem estas páginas: GAMEPLAY, GRAPHICS, SOUND, ALTERNATE VERSIONS, CONTROLS,
RANKING, PASSWORD, ORIGINAL PRESET, RESTORE DEFAULTS e BACK. As opções mantêm os nomes antigos no
rudebreaker.cfg, então saves de versões anteriores carregam sem mudança.

GAMEPLAY

  Opções, nesta ordem: DIFFICULTY, RANK, SUICIDE BULLETS, SHIP SELECT,
  SHIP SPEED, TRAINING, LOOP, ATTRACT MODE, I-FRAMES e BACK.

  DIFFICULTY       VERY EASY, EASY, NORMAL, HARD, VERY HARD. São os
                   próprios níveis de dificuldade do jogo original.
                   NORMAL é o padrão original. Salvo no RBRANK.DAT.

  RANK             ADVANCED (padrão), CLASSIC ou OFF. ADVANCED: um rank
                   oculto que também reage à sua força, aos itens e às
                   medalhas, e acrescenta tiros de volta e um bônus de
                   pontos (veja a seção 6, RANK MODE ADVANCED). CLASSIC:
                   os tiros inimigos ficam mais rápidos ou mais lentos
                   conforme você joga (veja a seção 6). OFF: a
                   velocidade dos tiros fica fixa, como no original.
                   ADVANCED só é o padrão num rudebreaker.cfg novo; um
                   save existente mantém a escolha.

  SUICIDE BULLETS  ON: os inimigos soltam tiros de volta quando são
                   destruídos (tiros suicidas), como na opção
                   original. OFF: não soltam. (Versões anteriores à
                   pr9zg mostravam este rótulo invertido. A opção
                   salva não muda, então o jogo continua igual; só o
                   rótulo agora está certo.)

  SHIP SELECT      A opção de mesmo nome do jogo original. Ela define
                   como as armas funcionam:
                   ON: no começo você escolhe uma arma (VULCAN, LASER
                   ou MISSILE) na tela "PLEASE SELECT YOUR OPTION".
                   Depois disso toda cápsula de arma é dessa arma, e
                   você fica com ela a partida inteira.
                   OFF (padrão): não há escolha. As cápsulas vêm como
                   V, L e M, e você troca de arma durante a partida
                   pegando uma cápsula de outra letra.

  SHIP SPEED       STANDARD ou AGGRESSIVE (padrão). STANDARD: LOW, MID
                   e MAX movem a nave como no original. AGGRESSIVE:
                   LOW fica 15% mais lento, MID igual e MAX 15% mais
                   rápido. Toda partida começa em MID; o botão de
                   velocidade continua trocando LOW / MID / MAX. O
                   demo usa sempre o movimento original. Salvo no
                   rudebreaker.cfg como ship_speed.

  TRAINING         ON: modo de treino (veja a seção 5). OFF: jogo
                   normal.

  LOOP             ON (padrão) ou OFF. OFF: o jogo termina depois da
                   fase 5 com o final, como no original. ON: vencer a fase 5 começa
                   a fase 1 de novo como loop 2, depois loop 3 e assim
                   por diante, mantendo pontuação, vidas e armas. Cada
                   loop é mais difícil (veja a seção 6). Salvo no
                   rudebreaker.cfg.

  ATTRACT MODE     ON (padrão) ou OFF. ON: depois da tela de ranking
                   roda uma demo de 30 segundos (sem música, só efeitos
                   sonoros) e depois o logo da Compile de novo. OFF: sem
                   demo. Salvo no rudebreaker.cfg.

  I-FRAMES         CLASSIC, NEW - FIRE, NEW - LIME ou
                   NEW - BLUE (padrão). CLASSIC: a invencibilidade da nave
                   depois de um golpe ou de um power-up aparece como
                   no original (o contorno vermelho). NEW: uma luz na
                   cor escolhida (FIRE: vermelhos quentes e laranja,
                   LIME: verdes, BLUE: azuis, todas escolhidas para
                   combinar com as cores PC-98 do jogo) também
                   percorre a própria nave em ondas, do centro para
                   fora, e para na borda do sprite; os options também
                   recebem enquanto estão acoplados. As ondas começam
                   junto com a invencibilidade, e em um quadro a cada
                   seis a nave inteira pisca como silhueta nessa cor.
                   Pulsa e vai afinando conforme o tempo restante
                   acaba, então dá para ver quando está para terminar.
                   É desenhada pelo port por cima da imagem, em pixels
                   puros (sem suavização); os filtros de cor e as
                   opções de CRT valem para ela também. Só muda o
                   visual: o tempo de invencibilidade é o mesmo. Salvo
                   no rudebreaker.cfg (iframes=classic / fire / lime /
                   blue; o valor antigo new vale fire).

  BACK

GRAPHICS

  Opções, nesta ordem: FULLSCREEN, SCALING, ASPECT, ORIENTATION, SIDE
  PANELS, PALETTE, BULLET COLOR, HITBOX, os seis efeitos
  de CRT, FRAME PACING e BACK.

  FULLSCREEN       OFF (padrão) ou ON. Ocupa o monitor inteiro,
                   mantendo a proporção do jogo (as SIDE PANELS
                   preenchem as laterais em telas largas) e escondendo
                   o ponteiro do mouse. F11 alterna a qualquer
                   momento. Salvo no rudebreaker.cfg.

  SCALING          Como a imagem é ampliada para a janela ou o
                   monitor. AUTO: usa INTEGER quando ele
                   ocupa pelo menos 80% da altura da tela; se não,
                   SHARP. Fica SHARP em 720p e 1080p, e INTEGER em
                   1440p, 4K e 800x480.
                   INTEGER: cada linha do jogo vira o mesmo número
                   inteiro de linhas da tela, com bordas pretas em
                   volta. Com ASPECT 8:5 os pixels são quadrados e
                   todos do mesmo tamanho. Com ASPECT 4:3 a altura é
                   um múltiplo inteiro (2x = 800 linhas, 3x = 1200) e
                   a largura é ajustada pra fechar o formato 4:3.
                   SHARP (padrão): ocupa o máximo da tela mantendo a
                   proporção, sem borrar (alguns pixels saem um pixel
                   de tela maiores). SMOOTH: ocupa do mesmo jeito, com
                   filtro suave (bilinear). Se a janela for pequena
                   demais para o INTEGER, ele funciona como SHARP. Com
                   as SIDE PANELS a imagem 16:9 (com os painéis) ocupa
                   a tela; com INTEGER (ou AUTO escolhendo INTEGER) a
                   área de jogo dentro dela é desenhada num múltiplo
                   inteiro das linhas, centralizada com preto em volta.
                   Salvo no rudebreaker.cfg.

  ASPECT           Formato da imagem inteira na tela.
                   4:3 (padrão): a imagem 640x400 aparece no formato
                   de um monitor PC-98, com cada pixel 1,2 vez mais
                   alto que largo (5:6), como a arte foi desenhada pra
                   ser vista. Numa tela 1080p fica 1440x1080 com
                   SHARP. 8:5: a imagem 640x400 como ela é (16:10,
                   pixels quadrados); o jogo fica um pouco mais
                   achatado que no original. Com as SIDE PANELS a
                   área de jogo segue a mesma escolha (8:5 deixa
                   faixas pretas em cima e embaixo). Com 8:5 o
                   movimento vertical da nave fica 1,2 vez mais
                   rápido, pra ela atravessar a área de jogo mais
                   achatada no mesmo tempo que no 4:3 (o demo mantém
                   o movimento original). Salvo no rudebreaker.cfg.

  ORIENTATION      0 (padrão), 90, 180 ou 270. Gira a imagem inteira
                   (jogo, título e menus) esse número de graus no
                   sentido horário, para monitor virado ou montado de
                   cabeça para baixo. Nunca é adivinhado pelo monitor:
                   você escolhe. 0 e 180 usam o layout largo normal. 90
                   e 270 usam o layout vertical (TATE): em cima, o logo
                   com a moldura do título, depois SCORE, LOADOUT, e NO
                   DEATH sobre SPEED; o playfield inteiro no meio, na
                   largura toda; embaixo RANK, HIT RATIO / HIT COUNT,
                   LEVEL / TIME, EXTENDS / I-FRAMES, e MEDALS sobre
                   INPUT. Com 90, vire o monitor com a borda direita
                   para cima; com 270, a borda esquerda. Melhor com
                   FULLSCREEN ON. SCALING e ASPECT funcionam
                   normalmente. Salvo no rudebreaker.cfg como
                   orientation.

  SIDE PANELS      ON (padrão) ou OFF. ON: numa janela 16:9 ou em tela cheia, a
                   área de jogo fica no meio, com painéis de
                   informação dos dois lados: gráfico do rank, armas,
                   vidas extras, invencibilidade, velocidade,
                   pontuação, taxa de acerto, medalhas, nível, tempo,
                   NO DEATH e um arcade stick que mostra os seus
                   comandos. O logo e a arte lateral vêm da sua
                   própria tela de título.
                   OFF, ou uma janela 4:3 / 16:10: o visual clássico.
                   LOADOUT mostra dois níveis, um bloco por nível,
                   incluindo o nível 1 (que enche o primeiro bloco).
                   SHOT é o tiro principal: 4 níveis, logo 4 blocos;
                   os níveis 2, 3 e 4 (MAX) chegam com 2, 10 e 18
                   itens de power-up, e os blocos vão enchendo item a
                   item. Perder uma vida tira um nível. Cada sub-arma
                   V / M / L tem 3 níveis, logo 3 blocos; mais uma
                   cápsula da arma que você já tem sobe um nível; no
                   MAX a cápsula dá 1000 pontos. Uma cápsula de outra
                   cor troca a arma e mantém o nível. Um acerto
                   enquanto você tem uma sub-arma leva a sub-arma em
                   vez de uma vida: o quadro passa a mostrar NO SUB e
                   um espaço de cápsula vazio. EXTENDS mostra a sua
                   nave e as vidas em estoque.
                   I-FRAMES é o seu tempo de invencibilidade, como uma
                   pizza: um disco cheio quando é dado, uma fatia que
                   se abre no sentido horário a partir do meio-dia
                   conforme acaba, e só um contorno fraco quando está
                   vazio. Amarelo: item de power-up ou cápsula.
                   Violeta: depois de um acerto ou ao renascer.
                   No gráfico do rank uma caveira marca cada vida
                   perdida. O gráfico mostra os últimos 60 segundos e
                   rola: os dados antigos saem pela esquerda conforme
                   entram os novos. Ele é limpo no começo de cada fase
                   (o rank em si não).
                   Com RANK OFF (ou no Training) o quadro do gráfico
                   mostra RANK SYSTEM / OFFLINE em verde, com uma linha
                   de varredura lenta, estilo CRT, subindo por ele a
                   cada 15 segundos, e os números do rank ficam
                   escondidos. O rank então não muda nada no jogo.
                   O rank passa de uma fase para a outra; ele só
                   recomeça numa partida nova. O pequeno "x1.05" ao
                   lado é o multiplicador atual de subida do rank.
                   A área de jogo segue o ASPECT: 4:3 (padrão) mostra
                   no formato de um monitor PC-98 (pixels 1,2 vez mais
                   altos que largos); 8:5 usa pixels quadrados.
                   HIT COUNT mostra os inimigos da fase que você
                   destruiu / o número fixo de inimigos da fase (369,
                   315, 128, 205 e 46 nas fases 1 a 5: os inimigos
                   que a própria fase coloca; ondas infinitas,
                   geradores de tiros e extras que dependem do rank ou
                   do tempo ficam de fora, assim como tiros, itens e
                   pontos escondidos). HIT RATIO é o mesmo em
                   porcentagem, com uma barra. Depois de uma vida
                   perdida parte da fase se repete; a contagem nunca
                   passa do total.
                   MEDALS conta as medalhas escondidas pegas nesta
                   fase, das 10 que cada fase esconde (fases 1 a 5). A
                   contagem é o próprio contador de medalhas do jogo:
                   só sobe quando você pega uma medalha revelada e
                   volta a 0 a cada fase.
                   NO DEATH mostra YES até você perder uma vida, e
                   depois NO (sem contagem). Continua NO pelas cinco fases e num continue, e só
                   volta a YES numa partida nova. No Training o
                   cabeçalho do quadro também mostra TRAINING.
                   A partir do loop 2 (LOOP ON) o título do quadro
                   RANK também mostra o loop: LOOP 2, LOOP 3 ... Fica
                   oculto no loop 1 e com LOOP OFF.
                   O layout vertical (retrato) não é mais uma escolha do
                   SIDE PANELS: ele vem do ORIENTATION 90 ou 270. Um
                   save de versão anterior com SIDE PANELS TATE carrega
                   como ON com ORIENTATION 90.

  PALETTE          MODERN (padrão): o rebalanceamento de cores do
                   jogo feito pelo Filipe. ORIGINAL: as cores do
                   PC-98.

  BULLET COLOR     PINK (padrão), LILAC, BLUE, GREEN ou ORIGINAL.
                   Todo tiro inimigo que não pode ser destruído é
                   recolorido, inclusive os lasers dos chefes em todo
                   o comprimento, no estilo da Cave, qualquer que seja
                   a cor original (vermelho, laranja ou dourado); a
                   borda escura, o corpo e o núcleo claro mantêm o
                   sombreado. O jogo marca esses tiros pelo tipo do
                   objeto, não pela cor: tiros inimigos têm o bit de
                   tiro e nenhum bit de "pode ser destruído". Tiros
                   que podem ser destruídos, inimigos, itens e o seu
                   próprio tiro mantêm as cores, assim como as bolas
                   de fogo inofensivas que sobram quando você destrói
                   um tiro. ORIGINAL deixa todos os tiros como no
                   jogo.

  HITBOX           OFF (padrão), PINK, LILAC, BLUE, GREEN ou RED
                   (um WHITE antigo vira PINK). Desenha a caixa de colisão real da nave (12 x
                   18 pixels) como um contorno fino fechado com
                   preenchimento 40 % transparente, para a nave
                   continuar visível e você ver o que pode ser
                   atingido. A caixa nunca usa a mesma cor dos tiros inimigos:
                   essa cor é pulada, e se você mudar o BULLET COLOR
                   para a cor da hitbox, a hitbox passa para a
                   próxima. RED é pulado com BULLET COLOR ORIGINAL (os
                   tiros originais são vermelhos). Salvo no
                   rudebreaker.cfg.

  COLOR FILTER     ORIGINAL (padrão), GAME BOY, VIRTUAL BOY, COMMODORE 64,
                   GAME GEAR, PC ENGINE, ZX SPECTRUM, MSX2, MEGA DRIVE (em ALTERNATE VERSIONS > CUSTOM). Muda as cores só do
                   playfield, e só
                   durante uma partida (ou o demo): título, menus, ranking,
                   entrada de nome e os painéis laterais ficam sempre com as
                   cores reais. A arte e a paleta do jogo não mudam, e o
                   PALETTE continua valendo por baixo. Os filtros são para
                   diversão: cada um exagera o visual da sua máquina. GAME
                   BOY: verdes do DMG pelo brilho, com o LCD lento (o que se
                   move deixa rastro) os pixels da própria imagem (sem blocos) sob uma grade fixa de pontos quadrados do LCD no passo real do DMG (160 na largura do campo de jogo), com as linhas escuras bem visíveis, como no GAME GEAR. VIRTUAL
                   BOY: tons de vermelho no preto, com o brilho curto dos
                   LEDs e a mesma grade de pontos quadrada e limpa.
                   COMMODORE 64: as 16 cores do C64 (Pepto), pixels de máquina 2x2, 3 cores mais preto por célula 8x8 no cenário; sprites, tiros e itens mantêm as próprias cores, e a mistura PAL (cada
                   linha se mistura com a de cima). GAME GEAR: a versão
                   colorida do visual GAME BOY: paleta de 4096 cores (4 bits
                   por canal) com no máximo 31 na tela (16 de fundo + 15 de sprite, a cor 0 dos sprites é transparente), escolhidas a cada quadro, pixels grandes e
                   nítidos (detalhe de cerca de 240x200), sem rastro de LCD, bordas nítidas, grade de pontos do LCD bem visível e contraste e saturação reforçados para sprites e tiros se destacarem do cenário. PC ENGINE: cores fortes,
                   saturadas e claras (VCE HuC6260: 3 bits por canal, 512 cores, níveis v x 255 / 7), 16 cores por tile 8x8 de fundo e 15 mais transparente por célula 16x16 de sprite, sem dithering, só uma leve suavidade de composto, na mesma grade do MEGA DRIVE (240 colunas no campo de jogo, todas as linhas do jogo; sem reamostrar, então nada treme enquanto a fase rola). ZX SPECTRUM: os limites da máquina real, incluindo o color clash: a escala de 192 linhas do Spectrum com pixels grandes e quadrados, e cada célula 8x8 tem só duas cores, uma INK e uma PAPER entre as 8 cores básicas, com um único bit BRIGHT para as duas; cada pixel é uma ou a outra. O PAPER preto é o preferido, como na maioria dos jogos do Spectrum, e contornos e detalhes são mantidos. As células do cenário ficam presas ao próprio cenário e rolam com ele, então as cores não se arrastam nem piscam. O cenário usa uma cor sólida escura do Spectrum (sem BRIGHT), sem dither ordenado. Sprites não levam dither: cada tipo fica numa cor chapada e em preto. As cápsulas de power-up, as options e os seus tiros ficam da cor da arma (VULCAN vermelho, LASER azul, MISSILE amarelo). A sua nave é branca com detalhe preto (o cinza claro continua branco, e as costuras mais escuras viram preto) e as pontas das asas continuam visíveis no voo reto (na invencibilidade só o brilho e o piscar do I-FRAMES ficam da cor do modo); as options laterais usam a cor da arma, também com detalhe preto. Os tiros inimigos nunca ficam da mesma cor do fundo atrás deles. No espaço (fundo preto com estrelas brancas) os inimigos não são brancos: ficam amarelos, magenta ou verdes. A primeira nave grande da fase 2 é amarelo escuro e a segunda é ciano, chapadas, mesmo quando essa nave é desenhada na página que rola. Na fase das nuvens o céu é ciano sólido e as nuvens são brancas com dither preto; um campo de cidade verde-escuro continua verde escuro. Contornos escuros, bordas e linhas de detalhe internas dos sprites e do cenário ficam em preto para as formas aparecerem, e as estrelas do fundo são sempre brancas. Cenário escuro que sumiria no preto é desenhado numa só cor por objeto (os asteroides da fase 1 em magenta escuro, outro cenário escuro em ciano). O texto na tela (linha de pontos no topo, legendas entre as fases) ignora as células: é desenhado em baixa resolução, monocromático e opaco por cima do cenário, para nunca sumir. A mesma regra vale em todos os COLOR FILTER: a linha de pontos e as legendas das fases são desenhadas em baixa resolução (2x2) e opacas por cima, em branco e preto (no GAME BOY o verde mais claro e o mais escuro do LCD, no VIRTUAL BOY o vermelho mais forte e o preto), para continuarem legíveis. MSX2: estilo SCREEN 5, 256x212 (pixels do cenário presos ao cenário, os dos sprites e da linha de pontos à tela, então nada treme), 16 cores por vez escolhidas para a cena entre as 512 do MSX2 (3 bits por vermelho, verde e azul); o conjunto muda devagar para não piscar; cada pixel fica com a mais próxima das 16, sem color clash. MEGA DRIVE: modo entrelaçado, 240 colunas no campo de jogo com as linhas do próprio jogo (uma grade de 320x224 fazia tudo, sprites inclusos, parecer se mexer quando o cenário rolava), as 512 cores do Mega Drive (3 bits por canal, níveis reais do DAC) em 4 linhas de 15 cores mais o fundo preto: duas linhas para o cenário e duas para os sprites; cada tile 8x16 usa uma linha, e as linhas mudam devagar para não piscar. Não é salvo: toda vez que o jogo abre, e ao fim de cada partida, volta para ORIGINAL (valores antigos de color_filter no rudebreaker.cfg são ignorados e removidos).

  Efeitos de CRT: seis chaves ON/OFF separadas, todas OFF por padrão.
  Como o COLOR FILTER, só afetam o playfield durante o jogo (e o demo),
  nunca título, menus, ranking ou painéis laterais. Podem ser
  combinados entre si e com qualquer COLOR FILTER, e funcionam em toda
  ORIENTATION. Salvos no rudebreaker.cfg como crt_scanlines, crt_mask,
  crt_curvature, crt_bloom, crt_vignette e crt_badrf. Cada um custa um
  pouco de velocidade; no Raspberry Pi ligue só os que quiser.

  SCANLINES        Faixas escuras entre as linhas, como num CRT de baixa
                   resolução.

  CRT MASK         A máscara vermelho/verde/azul do tubo de TV.

  CURVATURE        Curva o playfield como o vidro de um tubo, com cantos
                   pretos.

  BLOOM            As partes claras brilham suavemente em volta.

  VIGNETTE         Bordas e cantos escurecem aos poucos.

  BAD RF CABLE     Uma ligação de antena (RF) barata: linhas tremendo,
                   cor vazando, uma imagem fantasma fraca e um pouco de
                   chuvisco.

  FRAME PACING     SMOOTH ou EXACT (veja a seção 7).

  BACK

SOUND

  Opções: SOUND CHANNELS, MASTER VOLUME, BALANCE, OUTPUT PROFILE,
  CUSTOM EQ, SOUND TEST e BACK.

  SOUND CHANNELS   EXTRA (padrão): os efeitos tocam num segundo chip
                   emulado, e a música nunca é cortada. ORIGINAL: os
                   efeitos sonoros dividem o chip FM com a música,
                   como no PC-98, então um efeito pode cortar um
                   canal da música por um instante. Salvo no
                   rudebreaker.cfg.

  MUSIC CHIP       (ALTERNATE VERSIONS > CUSTOM) ORIGINAL (padrão), SID, GAME BOY, PSG, PC ENGINE, VIRTUAL BOY, ZX SPECTRUM, MSX2, MEGA DRIVE. Para
                   diversão: toca a música por um modelo simples do chip de
                   som de outra máquina em vez do chip FM do PC-98. As notas
                   que o jogo manda para o chip FM são lidas (altura, nota
                   ligada/desligada, volume) e as mais importantes vão para
                   as vozes que esse chip tem, então partes podem sumir e o
                   som é tosco de propósito; não é uma conversão exata. SID
                   (Commodore 64): 3 vozes com dente de serra, pulso (com
                   variação de largura), triângulo e ruído, passando por um
                   filtro ressonante. GAME BOY: 2 canais de pulso, um canal
                   de onda de 4 bits para o baixo e um canal de ruído, com
                   DAC de 4 bits. PSG (SN76489 do Game Gear): 3 ondas
                   quadradas e ruído, volume em passos de 2 dB e nenhuma
                   nota abaixo de cerca de 110 Hz. PC ENGINE: 6 canais de
                   wavetable de 32 passos e 5 bits. VIRTUAL BOY (VSU): 5
                   canais de wavetable de 32 passos e 6 bits (o quinto com
                   sweep e modulação) e um canal de ruído, nada abaixo de cerca de 76 Hz. ZX SPECTRUM (AY-3-8912 do 128K, 1,7734 MHz, mono): 3 ondas quadradas, um gerador de ruído compartilhado e o envelope de hardware, 16 níveis de volume logarítmicos. MSX2 (o PSG e um cartucho SCC da Konami, sem FM): 3 ondas quadradas e ruído do PSG e 5 canais SCC de wavetable de 32 passos (os dois últimos dividem a forma de onda); a guitarra são duas serras do SCC na tônica e na quinta sobre uma quadrada do PSG uma oitava abaixo; o baixo e as outras partes tocam no SCC; a bateria usa quadradas e ruído do PSG. MEGA DRIVE (Mega Drive 1: FM YM2612, 6 canais de 4 operadores, mais o PSG SN76489): patches FM de metais, baixo e guitarra (a guitarra é uma guitarra FM de metal tocada num núcleo YM2612 emulado: patch de feedback alto, distorcido no próprio chip, a nota dobrada uma oitava abaixo cerca de 11 cents mais grave e de novo duas oitavas abaixo, notas curtas palhetadas que deslizam até a altura e caem ao soltar, sem vibrato, sem amp nem caixa), com os degraus de volume grossos e o som do DAC de 9 bits do YM2612; bateria nas quadradas e no ruído do PSG, sem samples. O SOUND TEST também toca
                   pelo MUSIC CHIP escolhido. Independente do COLOR FILTER.
                   Não é salvo (volta para ORIGINAL a cada abertura e após cada partida).
                   A guitarra distorcida ganha um chip inteiro (no VIRTUAL BOY e no SID, cada parte de guitarra ganha um), com todas
                   as vozes formando uma guitarra (SID: sync/ring e filtro;
                   GAME BOY: dois pulsos, onda distorcida e palhetada no
                   ruído; PSG: três quadradas e palhetada no ruído; PC
                   ENGINE e VIRTUAL BOY: wavetables e palhetada no ruído; ZX SPECTRUM: uma quadrada na tônica (a serra zumbindo do envelope só quando o período grosso dele acerta a nota afinada), quinta e oitava de power chord e palhetada no ruído; MSX2: duas serras do SCC na tônica e na quinta, uma quadrada do PSG uma oitava abaixo e palhetada no ruído do PSG; MEGA DRIVE: uma cadeia FM saturada na tônica e na quinta com palhetada no ruído),
                   depois
                   coloração de amplificador e caixa. Outro chip toca o
                   resto, com baixo e bateria no equilíbrio do original, e uma segunda parte de guitarra também toca nele: dois chips no total (VIRTUAL BOY e SID: até três).

  SFX CHIP         As mesmas opções para os efeitos sonoros, independente do
                   MUSIC CHIP. Vale para o segundo chip usado por SOUND
                   CHANNELS: EXTRA; com ORIGINAL os efeitos tocam no chip da
                   música e seguem o MUSIC CHIP. Não é salvo.

  ALTERNATE VERSIONS  (página própria do CONFIG, depois de SOUND) Uma
                   chave para "e se o jogo tivesse saído em outra
                   máquina": ORIGINAL PC-98 (ORIGINAL/ORIGINAL/ORIGINAL),
                   COMMODORE 64 (filtro COMMODORE 64, chips SID), GAME BOY
                   (GAME BOY, GAME BOY), GAME GEAR (GAME GEAR, PSG), PC
                   ENGINE (PC ENGINE, PC ENGINE), VIRTUAL BOY (VIRTUAL BOY,
                   VIRTUAL BOY), ZX SPECTRUM 128K (ZX SPECTRUM, ZX SPECTRUM),
                   MSX2 (MSX2, MSX2), MEGA DRIVE (MEGA DRIVE, MEGA DRIVE).
                   A ativa mostra ON. CUSTOM abre mais três linhas, COLOR
                   FILTER, MUSIC CHIP e SFX CHIP (acima), para escolher
                   cada um à mão (ESQUERDA/DIREITA). Nada disso é salvo:
                   toda vez que o jogo abre, e ao fim de cada partida (game
                   over, final ou saída para o título), volta para ORIGINAL
                   PC-98.

  MASTER VOLUME    0 a 100 em passos de 10 (padrão 100). O volume de
                   toda a saída de som do programa, ajustado pelo port
                   por cima dos volumes de música e efeitos do próprio
                   jogo (que não mudam). Em 100 o som sai exatamente
                   como antes. Salvo no rudebreaker.cfg (master_volume=).

  BALANCE          CENTER (padrão), ou LEFT / RIGHT de 1 a 10: o
                   outro lado fica mais baixo, um décimo por passo, então
                   10 é só um lado. Feito pelo port na saída. Salvo
                   (balance=, de -10 a 10).
  OUTPUT PROFILE   Uma curva de EQ para onde você ouve: FLAT (padrão,
                   sem mudança), HEADPHONES (agudos mais suaves e um
                   crossfeed leve entre os ouvidos), TV (menos grave
                   profundo, faixa da voz à frente), MONITOR (caixas de
                   mesa), STEREO SPEAKERS, 2.1 (grave abaixo de uns 120 Hz
                   igual nos dois lados, para subwoofer, e um reforço de
                   grave), LARGE SPEAKERS, HOME THEATER, LAPTOP e SMALL
                   SPEAKERS (corta o grave profundo para alto-falantes
                   pequenos não distorcerem), CUSTOM (a curva do CUSTOM
                   EQ). O nível baixa pelo maior reforço para nada
                   clipar. FLAT com BALANCE CENTER deixa o som
                   exatamente como antes. Salvo (output_profile=).
  CUSTOM EQ        Página com um EQ gráfico de 10 bandas: 31, 62, 125,
                   250, 500 Hz, 1K, 2K, 4K, 8K, 16K, cada uma de -12 a
                   +12 dB em passos de 1 dB com esquerda/direita; o Tiro
                   volta a banda para 0. Mexer numa banda escolhe OUTPUT
                   PROFILE CUSTOM. Salvo (custom_eq=, dez números).
  SOUND TEST       Abre a página SOUND TEST (abaixo).

  BACK

CONTROLS

  JOYSTICK         ON ou OFF: se o jogo lê um controle. Um controle é
                   ligado sozinho ao ser conectado, mas se você puser
                   JOYSTICK em OFF aqui ele fica desligado, também
                   depois de reiniciar e quando um controle é conectado
                   de novo, até você pôr em ON (salvo como
                   joystick=off; auto é o padrão).
                   Embaixo: DEVICE mostra o único controle em uso (o
                   nome dele), ou NONE. Com dois controles conectados,
                   o que está em uso continua em uso quando o outro é
                   conectado ou removido.
  DEFINE KEYS      Remapeia UP, DOWN, LEFT, RIGHT, FIRE, MODE, SHIFT,
                   PAUSE e SPEED. Escolha uma ação e aperte a tecla
                   ou o botão novo. RESTORE DEFAULTS volta as teclas
                   listadas na seção 3.
                   Os botões do controle aparecem com nomes legíveis,
                   pela família do controle quando ela é conhecida:
                   A / B / X / Y, LB / RB, VIEW / BACK e MENU / START em
                   controles estilo Xbox; CROSS, CIRCLE, SQUARE,
                   TRIANGLE, L1 / R1 em controles PlayStation; letras
                   Nintendo pela posição. Gatilhos, analógicos, o
                   D-PAD, BACK PADDLE 1-4, TOUCHPAD CLICK e o botão de
                   share / captura têm nomes próprios.
  BACK

SOUND TEST (aberto pelo SOUND)

  SOUND            Música e efeitos sonoros ON ou OFF.
  BGM              Escolha uma música (0-47) com esquerda/direita; o
                   Tiro toca.
  SE               Escolha um efeito sonoro (0-47) com
                   esquerda/direita; o Tiro toca.
  PLAY / PAUSE / STOP   Controles do player.
  MUSIC CHIP / SFX CHIP   Os chips do ALTERNATE VERSIONS, trocados
                   aqui com esquerda/direita; a faixa tocando muda na
                   hora. Como lá, não são salvos: voltam para ORIGINAL
                   na próxima abertura e depois de uma partida.
  VU               LEVEL (padrão) ou SPECTRUM: o estilo do medidor
                   embaixo. Salvo no rudebreaker.cfg (vu=).
  A linha de cima mostra a faixa e o estado. O medidor embaixo mostra
  o que está tocando: LEVEL é um VU em segmentos de LED do verde ao
  vermelho com pico retido; SPECTRUM é um analisador de espectro no
  estilo de micro system, com barras de frequência e uma linha com
  pontos de pico.
  (Alguns números são mudos: estão vazios nos dados originais.)

RANKING            Abre a tabela de recordes original.

PASSWORD           Abre a tela de senha original.

ORIGINAL PRESET    Um passo para o jogo como era no disco do PC-98,
                   por exemplo para comparar. Aperte Fire uma vez:
                   aparece PRESS AGAIN; aperte Fire de novo para
                   confirmar. Põe LOOP OFF, RANK OFF, SHIP SELECT ON (a
                   escolha de arma, como no disco), SOUND CHANNELS
                   ORIGINAL, a paleta original, sem COLOR FILTER, sem
                   CRT, sem painéis laterais, cores de tiro originais,
                   HITBOX OFF, SHIP SPEED STANDARD, TRAINING OFF,
                   I-FRAMES CLASSIC, os chips de música e efeitos
                   originais, OUTPUT PROFILE FLAT e BALANCE CENTER. Janela, escala, aspecto, orientação,
                   teclas, DIFFICULTY, SUICIDE BULLETS, MASTER VOLUME,
                   recordes, o ranking (RBRANK.DAT) e senhas não são
                   mexidos; nada é apagado. Dá para mudar qualquer opção
                   depois. A linha preset=original no rudebreaker.cfg
                   faz o mesmo uma vez na próxima abertura.

RESTORE DEFAULTS   Volta todas as opções do CONFIG ao padrão de uma
                   instalação nova (teclas incluídas). Aperte Fire uma
                   vez: aparece PRESS AGAIN; aperte Fire de novo para
                   confirmar (mudar de linha cancela). Recordes, ranking
                   e senhas são mantidos; JOYSTICK fica como está.

BACK               Volta ao título.


PERFIS DE CONTROLE (Steam Deck e outros)

  Qualquer controle que o SDL conhece funciona sem configurar: analógico
  ou D-PAD movem, o botão de baixo atira, o da direita é o MODE, START
  pausa. Use o DEFINE KEYS para o resto.
  Steam Deck (no modo desktop ou jogo, pelo Steam Input com o layout
  "Gamepad"): analógico esquerdo ou D-PAD movem, A FIRE, B MODE, X
  SHIFT, MENU (o botão à direita da tela) PAUSE. Os botões de trás (L4,
  L5, R4, R5) chegam como BACK PADDLE 1-4, então dá para pôr SPEED ou
  SHIFT num deles no DEFINE KEYS; se o Steam mapear eles para outros
  botões, configure no Steam Input. Ainda não testado num Deck de
  verdade.

LINUX: WAYLAND E X11

  No Linux o programa abre primeiro em X11 (num desktop Wayland, pelo
  XWayland), que funciona em todo lugar. Para usar Wayland nativo, abra
  com SDL_VIDEODRIVER=wayland ./rudebreaker ; para voltar, tire a
  variável ou use SDL_VIDEODRIVER=x11.

5. MODO DE TREINO
-----------------

O TRAINING é para treinar:

  - A sua nave não pode ser atingida. Você nunca perde a arma nem uma
    vida.
  - Os pontos de medalha escondidos piscam em vermelho no lugar o
    tempo todo, o mesmo piscar vermelho que normalmente só aparece
    quando você atira neles, para você aprender onde estão. Você
    ainda precisa atirar no ponto até a medalha aparecer, como no
    jogo normal, e a pontuação não muda.
  - O RANK não é usado: a dificuldade fica na opção DIFFICULTY.
  - Pontuações de uma partida de treino não entram na tabela de
    recordes, e nada dessa partida é salvo no RBRANK.DAT.

Desligue o TRAINING para jogar normalmente.


6. PONTUAÇÃO, SAVE E OS SEGREDOS ORIGINAIS
------------------------------------------

Recordes e opções são salvos no RBRANK.DAT, no formato original. Uma
pontuação nova é salva quando você digita o seu nome.

A dificuldade e todas as regras do jogo funcionam como no original.

O RANK (opção de GAMEPLAY, ADVANCED por padrão num cfg novo) é um acréscimo deste
port, inspirado no sistema de rank do Zanac. Um valor de rank
escondido sobe enquanto você continua vivo e quando você pontua, e
zera quando você perde uma vida (veja abaixo). Ele sobe a um terço da
taxa usada nas versões anteriores deste port, vezes um multiplicador
que cresce ao longo da partida: +1% para cada nível de tiro principal
ganho, +5% para cada nível de sub-arma ganho (uma cápsula que só dá
1000 pontos ou só troca a cor não conta) e +5% para cada vida extra
ganha por pontos. O multiplicador começa em x1.00 a cada partida nova
e não muda o zerar ao perder uma vida nem os limites definidos pela
DIFFICULTY.
Perder a sub-arma (um acerto que deixa você só com o tiro principal)
tira um terço do rank atual. O valor tirado fica guardado, e pegar
uma sub-arma de novo devolve esse valor (nunca acima do limite da sua
DIFFICULTY). Uma partida nova limpa isso. Um rank mais alto acelera
os tiros inimigos, do mesmo jeito que uma DIFFICULTY mais alta; nada
mais muda, e os bônus continuam seguindo a DIFFICULTY escolhida. A
DIFFICULTY define o ponto de partida e o piso: o rank nunca deixa os
tiros mais lentos que na dificuldade escolhida. No VERY EASY e no
EASY o rank só sobe até a metade; no HARD e no VERY HARD ele começa
na metade. O rank recomeça a cada partida nova.

Quando o rank chega ao topo da dificuldade em jogo, o jogo sobe um
nível de dificuldade e o rank recomeça no ponto de partida desse
nível, até chegar ao teto da DIFFICULTY escolhida:

  VERY EASY  -> até NORMAL (passando pelo EASY)
  EASY       -> até HARD (passando pelo NORMAL)
  NORMAL     -> até HARD
  HARD       -> até VERY HARD
  VERY HARD  -> fica em VERY HARD

No teto o rank fica no topo. A cada subida o quadro LEVEL do painel
lateral pisca por 2 segundos e o gráfico do RANK ganha uma linha
verde com uma seta para cima naquele momento (mortes mostram uma
caveira). Cada subida também paga o bônus de 1000 pontos do próprio
jogo, com o aviso 1,000 acima da sua nave.

Se você escolheu NORMAL ou VERY HARD, quando estiver no teto com o
rank no topo, os tiros inimigos ficam 1% mais rápidos a cada 20
segundos de jogo, até +25%. O quadro LEVEL mostra isso (por exemplo
V.HARD +3% ou HARD +3%). Levar um acerto (perder a sub-arma) ou
perder uma vida volta para +0%.

Perder uma vida leva você de volta à DIFFICULTY escolhida, com o rank
inicial dela. A subida só muda o que o rank muda (a velocidade dos
tiros inimigos e os limites do rank): pontuação, bônus e todo o resto
continuam seguindo a DIFFICULTY escolhida, e a opção salva não muda.
Não vale no Training. Com RANK OFF o jogo fica exatamente igual ao
original: sem subida, sem aceleração dos tiros e sem o bônus de
+1000, e o painel lateral mostra RANK SYSTEM / OFFLINE no gráfico do
rank.

RANK MODE ADVANCED (RANK em ADVANCED; salvo como rank_mode=advanced)
troca as regras acima por um rank oculto de 0 a 1000. Nenhum número
aparece: o gráfico do RANK fica sem valores e uma barra fina embaixo
dele mostra o progresso até a próxima faixa do LEVEL: vazia no começo
da faixa, cheia na próxima, quando o quadro LEVEL pisca, uma seta para cima
aparece no gráfico do RANK (como no CLASSIC) e a barra
recomeça para a faixa nova. No teto ela fica cheia, mais clara e com
contorno. Ele começa e fica entre limites
definidos pela DIFFICULTY:

  VERY EASY  começa em 0,   no máximo 400
  EASY       começa em 80,  no máximo 550
  NORMAL     começa em 150, no máximo 800
  HARD       começa em 280, no máximo 900
  VERY HARD  começa em 400, no máximo 1000

Ele nunca fica abaixo do início. Sobe:
  - a cada segundo vivo, mais rápido quanto mais tempo desde a última
    vida perdida (até três vezes mais rápido depois de 3 minutos);
  - quando o tiro principal ganha nível, e um pouco a cada segundo
    com o tiro principal no nível 3 ou 4;
  - ao pegar uma sub-arma ou subir o nível dela (mais para uma mais
    forte);
  - ao pegar uma medalha (cada medalha da fase vale mais que a
    anterior);
  - um pouco a cada segundo por vida em estoque (até 5);
  - um pouco a cada segundo enquanto você segura o tiro.
Enquanto o NO DEATH ainda está em YES, toda subida é 10% maior.
Cai quando você perde uma vida (de 15% a 25% do valor, mais quando
está alto; o bônus de sobrevivência recomeça) e um pouco (4%) quando
você perde a sub-arma.

O que ele muda:
  - A velocidade dos tiros inimigos acompanha o rank de forma
    contínua, da velocidade da DIFFICULTY escolhida (nunca mais lenta)
    até a velocidade do VERY HARD no topo. Acima de 850 os tiros ficam
    até 15% mais rápidos.
  - A partir de 700, inimigos destruídos soltam tiros de revanche
    (como no SUICIDE BULLETS), mesmo com essa opção desligada. Abaixo
    de 700 volta a valer a opção escolhida.
  - Pontos: cada ponto ganho aumenta até 50% no rank 1000 (metade de
    rank/1000).
  - O quadro LEVEL mostra a faixa do rank (V.EASY a V.HARD, nunca
    abaixo da sua DIFFICULTY) e pisca quando sobe de faixa.
A cadência e a quantidade de tiros dos inimigos não mudam: nos dados
do jogo o tempo de tiro de cada inimigo é um contador próprio dentro
do script dele, e não dá para mudar isso para todos sem alterar os
padrões.
Com LOOP, cada loop novo começa o rank 100 mais alto e permite 50 a
mais (no máximo 1000), e o rank sobe o número do loop vezes mais
rápido (veja LOOP abaixo).
Não vale no Training. RANK OFF desliga tudo isso.

O LOOP (opção do GAMEPLAY, ON por padrão) é uma adição deste port.
Com LOOP ON, vencer a fase 5 não mostra o final: a fase 1 começa de
novo como loop 2, depois loop 3 e assim por diante, e você mantém
pontuação, vidas e armas. Em cada loop novo:

  - A dificuldade base fica um nível acima da do loop anterior, a
    partir da DIFFICULTY escolhida (VERY EASY -> EASY -> NORMAL ->
    HARD -> VERY HARD). Em VERY HARD ela fica em VERY HARD e só o
    multiplicador de loop abaixo continua crescendo. O nível novo, com
    os ajustes do próprio jogo para ele (tiros e resistência dos
    inimigos), vale a partir do começo da fase 1 do loop.
  - O rank recomeça no ponto inicial da base nova. O teto de subida,
    os limites do rank e a aceleração no teto seguem a base do mesmo
    jeito que seguem a sua DIFFICULTY no loop 1 (por exemplo, uma
    partida em NORMAL fica HARD no loop 2, e lá pode subir até VERY
    HARD).
  - Todo valor controlado pelo rank é multiplicado pelo número do
    loop: x1 no loop 1, x2 no loop 2, x3 no loop 3 e assim por diante.
    Isso vale para a velocidade com que o rank sobe, para quanto o
    rank soma à velocidade dos tiros inimigos e para os passos de +1%
    da aceleração no teto. Limites: o multiplicador para em x8 (loop 8
    em diante); a velocidade dos tiros inimigos nunca passa do valor
    VERY HARD do próprio jogo; a aceleração no teto vai até +25% vezes
    o multiplicador, mas nunca passa de +50%.
  - A partir do loop 2 (fora do Training), o ganho de rank é
    multiplicado pelo número do loop nos dois modos de rank, e os
    tiros inimigos ficam mais rápidos com o rank dentro do loop: a
    faixa de rank do loop (do início ao teto) é dividida em 10
    passos, e cada passo soma 5% de velocidade vezes (loop - 1): +5%
    por passo no loop 2, +10% no loop 3 e assim por diante, até no
    máximo +100%.
  - A partir do loop 2, SUICIDE BULLETS fica sempre ON (a opção do
    menu é mantida e volta a valer no loop 1 / numa partida nova).
  - A partir do loop 2, os chefes têm mais HP: cada chefe de fase
    +50% por loop extra (x1,5 no loop 2, x2 no loop 3 ...) e o chefe
    final (a fortaleza da fase 5) +100% por loop extra (x2 no loop 2,
    x3 no loop 3 ...). O multiplicador de loop para em x8 e o HP tem
    limite, então nunca estoura.
  - Perder uma vida volta para a dificuldade base do loop, com o rank
    inicial dela.
  - A partir do loop 2, o título do quadro RANK no painel lateral
    mostra o loop (LOOP 2, LOOP 3 ...); fica oculto no loop 1. A
    contagem para em 99.
  - Game over encerra a partida; uma partida nova começa no loop 1 com
    a DIFFICULTY escolhida.
  - No Training, LOOP ON ainda começa a fase 1 de novo depois da fase
    5, mas a dificuldade e o multiplicador não mudam.

Com LOOP OFF o jogo termina depois da fase 5 exatamente como no
original.

O cheat de debug original continua no código. Durante a partida,
pause com o Esc e digite N E K O C O. Depois:

  2   liga/desliga a invencibilidade
  3   liga/desliga a tela de debug
  1   segure para rodar sem a espera de quadro (avanço rápido)
  4   pula para o final

Esse é o recurso escondido do original e ficou como era.


7. RITMO DOS QUADROS E VSYNC
----------------------------

A lógica original do jogo roda a cerca de 30,67 quadros por segundo.
Essa velocidade é mantida em qualquer PC, então o jogo nunca fica
mais rápido que o original.

O port espera a atualização do seu monitor (vsync) quando o driver de
vídeo permite:

  SMOOTH (padrão)   Num monitor de 60, 120 ou 240 Hz, cada quadro do
                    jogo aparece por exatamente 2, 4 ou 8
                    atualizações. O movimento fica perfeitamente
                    uniforme. O jogo então roda a 30,00 quadros por
                    segundo, cerca de 2% mais lento que o original, o
                    que você não vai notar.

  EXACT             O jogo roda a exatamente 30,67 quadros por segundo
                    e cada atualização mostra o quadro mais novo. O
                    movimento tem uma leve irregularidade periódica em
                    telas de 60 Hz.

Em outras taxas (75, 144, 165 Hz...) o jogo sempre usa EXACT. Se não
houver vsync, um timer mantém o jogo a 30,67.

O F8 mostra uma pequena sobreposição com o modo, a taxa de
atualização medida e estatísticas de tempo. As mesmas informações
vão para o pace.txt, junto do programa, quando o jogo começa e quando
você sai. É útil se você relatar um problema de fluidez.


8. OPÇÕES ORIGINAIS REMOVIDAS DE PROPÓSITO
------------------------------------------

O menu de opções original tinha alguns itens que este port não
mostra:

  AUTO SHOT, WAIT, PRIORITY, FRAME SKIP, SWAP KEYS

Eles serviam para contornar a velocidade e os limites do hardware do
PC-98, ou já são cobertos pelos controles do PC. Um PC moderno roda o
jogo em velocidade total, sem lentidão nem pulo de quadros, a
velocidade é fixada pelo ritmo do port (seção 7), e o DEFINE KEYS
substitui a troca de teclas. Os itens separados INVINCIBLE e NAME
ENTRY do original foram substituídos pelo TRAINING.


9. LIMITAÇÕES CONHECIDAS
------------------------

  - O vsync e o ritmo SMOOTH ainda não foram testados em todo tipo de
    monitor e placa de vídeo. Se o movimento não ficar fluido, tente
    EXACT e mande o pace.txt.
  - Esta é uma build de teste. Por favor, avise de qualquer coisa que
    pareça ou soe diferente do original.


10. COMO RELATAR BUGS
---------------------

Quando algo parecer, soar ou responder errado, por favor mande:

  1. O que aconteceu e o que você esperava.
  2. Os passos para fazer acontecer de novo (fase, modo, menu, o que
     você apertou). Diga se acontece sempre ou só às vezes.
  3. Um print ou um vídeo curto, se puder.
  4. A sua versão do Windows ou do Linux, a placa de vídeo, a taxa de
     atualização do monitor e o controle que você usa (teclado ou
     qual controle).
  5. A opção de PACING (SMOOTH ou EXACT) e o arquivo pace.txt da pasta
     do jogo (ele é reescrito toda vez que você sai, então copie logo
     depois do problema).
  6. O nome da build: Rude Breaker 30th AE pr9ze.

Mande para Filipe Gracioli / shmupsbr.


11. CRÉDITOS
------------

  Rude Breaker (c) Compile, 1996. Jogo, gráficos, música e programa
               originais.
  ymfm         Emulação do YM2203 por Aaron Giles, licença BSD
               3-Clause (veja LICENSE-ymfm.txt).
  SDL2         Simple DirectMedia Layer, licença zlib.
  Port para PC Filipe Gracioli / shmupsbr.
  30th AE      (c) ShmupsBR 2026.

Fonte dos painéis: Robotica, de Courtney Novits (fonte gratuita,
dafont.com), embutida no programa.

RUDE BREAKER 30th AE - PORT PARA PC - MANUAL DO USUÁRIO
=======================================================

Build pr9zc - 30/10/2026

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

Cada partida começa na MOVE SPEED escolhida em GAMEPLAY. A mudança
feita pelo botão vale até o fim da partida; ela não altera a opção
MOVE SPEED.

PAUSA: o Esc (ou Start) só pausa enquanto você está jogando. No
título, na demo e nos menus ele não pausa. Nos menus, o Esc (teclado)
ou o botão B (controle) volta uma página.

A janela pode ser redimensionada. A imagem mantém a proporção
original de 640x400.


4. TELA DE TÍTULO E MENU CONFIG
-------------------------------

O logo, o título, a tela de ranking e a introdução são as telas
originais. Antes deles roda um cartão sem som: partículas voam e
formam o logo do Rude Breaker, ele fica um instante, e então
"30TH ANNIVERSARY EDITION" (em branco, espalhado pela largura do logo,
com um brilho suave quando assenta) e a linha de crédito sobem de
baixo; depois de uns 2,5 segundos ao todo o cartão some e começam o
logo original da Compile e a música dele. Qualquer tecla ou botão do
controle pula o cartão e vai direto para o logo da Compile. Com SIDE
PANELS ligado, a linha de pontuação no alto da área de jogo fica uns
pixels para dentro de cada lado, para não ser cortada pelas molduras
dos painéis.
A tela de título mostra "30TH ANNIVERSARY EDITION" em branco,
centralizado sob o logo, com um brilho suave em volta das letras a
cada 5 segundos (o mesmo brilho aparece sob o logo no painel lateral
esquerdo), o PUSH TRIGGER mudando suavemente entre amarelo e verde a
cada 10 segundos, e as linhas de crédito (c) COMPILE 1996 e (c) 2026
SHMUPSBR.
Aperte Tiro no título para começar, ou abra o CONFIG pelo menu do
título.

O CONFIG tem estas páginas:

GAMEPLAY

  DIFFICULTY       VERY EASY, EASY, NORMAL, HARD, VERY HARD. São os
                   próprios níveis de dificuldade do jogo original.
                   NORMAL é o padrão original. Salvo no RBRANK.DAT.

  SUICIDE BULLETS  ON: os inimigos soltam tiros de volta quando são
                   destruídos (tiros suicidas), como na opção
                   original. OFF: não soltam.

  SHIP SELECT      A opção de mesmo nome do jogo original. Ela define
                   como as armas funcionam:
                   ON: no começo você escolhe uma arma (VULCAN, LASER
                   ou MISSILE) na tela "PLEASE SELECT YOUR OPTION".
                   Depois disso toda cápsula de arma é dessa arma, e
                   você fica com ela a partida inteira.
                   OFF (padrão): não há escolha. As cápsulas vêm como
                   V, L e M, e você troca de arma durante a partida
                   pegando uma cápsula de outra letra.

  MOVE SPEED       LOW, MID (padrão), MAX. A velocidade da nave, como
                   no original. Cada partida começa nela.

  RANK             ON (padrão): os tiros inimigos ficam mais rápidos ou
                   mais lentos conforme você joga (veja a seção 6).
                   OFF: a velocidade dos tiros fica fixa, como no
                   original.

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

  PALETTE          MODERN (padrão): o rebalanceamento de cores do
                   jogo feito pelo Filipe. ORIGINAL: as cores do
                   PC-98.

  TRAINING         ON: modo de treino (veja a seção 5). OFF: jogo
                   normal.

  FRAME PACING     SMOOTH ou EXACT (veja a seção 7).

  SIDE PANELS      ON (padrão): numa janela 16:9 ou em tela cheia, a
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
                   mostra RANK SYSTEM / OFFLINE, com uma linha de
                   varredura lenta, estilo CRT, subindo por ele a cada
                   15 segundos, e o número do rank fica "/ 159  OFF".
                   O rank passa de uma fase para a outra; ele só
                   recomeça numa partida nova. O pequeno "x1.05" ao
                   lado é o multiplicador atual de subida do rank.
                   Nesse layout a área de jogo é mostrada no formato
                   4:3 de um monitor PC-98 (pixels 1,2 vez mais altos
                   que largos); o visual clássico usa pixels
                   quadrados.
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
                   depois NO com o número de vidas perdidas ("NO x2").
                   Continua NO pelas cinco fases e num continue, e só
                   volta a YES numa partida nova. No Training o
                   cabeçalho do quadro também mostra TRAINING.

  BACK

CONTROLS

  JOYSTICK         Mostra se há um controle em uso, e o nome dele.
                   Troque com esquerda/direita.
  DEFINE KEYS      Remapeia UP, DOWN, LEFT, RIGHT, FIRE, MODE, SHIFT,
                   PAUSE e SPEED. Escolha uma ação e aperte a tecla
                   ou o botão novo. RESTORE DEFAULTS volta as teclas
                   listadas na seção 3.
  BACK

SOUND TEST

  SOUND            Música e efeitos sonoros ON ou OFF.
  BGM              Escolha uma música (0-47) com esquerda/direita; o
                   Tiro toca.
  SE               Escolha um efeito sonoro (0-47) com
                   esquerda/direita; o Tiro toca.
  PLAY / PAUSE / STOP   Controles do player.
  As barras embaixo são um VU meter do que está tocando.
  (Alguns números são mudos: estão vazios nos dados originais.)

RANKING            Abre a tabela de recordes original.

PASSWORD           Abre a tela de senha original.

BACK               Volta ao título.


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

O RANK (opção de GAMEPLAY, ligado por padrão) é um acréscimo deste
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
  6. O nome da build: Rude Breaker 30th AE pr9zc.

Mande para Filipe Gracioli / shmupsbr.


11. CRÉDITOS
------------

  Rude Breaker (c) Compile, 1996. Jogo, gráficos, música e programa
               originais.
  ymfm         Emulação do YM2203 por Aaron Giles, licença BSD
               3-Clause (veja LICENSE-ymfm.txt).
  SDL2         Simple DirectMedia Layer, licença zlib.
  Port para PC Filipe Gracioli / shmupsbr.
  30th AE      (c) 2026 ShmupsBR.

Fonte dos painéis: Robotica, de Courtney Novits (fonte gratuita,
dafont.com), embutida no programa.

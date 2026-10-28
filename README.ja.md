# Rude Breaker — PC版移植（エンジンのみ）

🇬🇧 [English](README.md#english) · 🇧🇷 [Português](README.md#português) · 🇯🇵 [日本語](README.ja.md)

---

## 日本語

コンパイルが *ディスクステーション Vol.10*（PC-98、1996年）で発表した縦スクロールシューティング **Rude Breaker（ルードブレイカー）** の非公式PC（Windows・Linux）移植版です。オリジナルのx86ゲームコードをインタプリタ上で動かし、最新のSDL2で表示・入力を行います：垂直同期、ゲームパッド対応、キー割り当ての変更、VUメーター付きサウンドテスト、トレーニングモード、ハイスコアの保存など。音楽はオリジナルのドライバーを、PC-98のFM音源チップYM2203のエミュレーションで再生しています。詳しくは `docs/MANUAL.md` をご覧ください。

### ゲームファイルが含まれていない理由
このリポジトリに含まれているのは **エンジンのみ**（この移植のために書かれたコード）です。オリジナルのゲームファイルは一切含まれておらず、今後も含まれることはありません：

- **Rude Breaker はフリーソフトでもシェアウェアでもありません。** コンパイルの市販タイトルであり、権利は現在も有効です。2014年にはD4エンタープライズが **プロジェクトEGG** で再配信しています。
- ゲームのグラフィック、音楽、効果音、ステージデータ、プログラム（`SGM1.DAT`、`SONG.DAT`、`FPLAY.COM`）は権利者に帰属します。無料であっても再配布は認められていません。

これらのファイルは **ご自身が正規に入手したゲーム** から用意してください。

### プレイ用パッケージの作り方（Windows）
1. ゲームを正規に入手してください。例：[プロジェクトEGG](https://www.amusement-center.com/)、またはお手持ちの *ディスクステーション Vol.10* のディスク。
2. Rude Breaker のディスクから次のファイルを取り出します：`SGM1.DAT`、`SONG.DAT`、`FPLAY.COM`。
3. このリポジトリの `releases/RudeBreaker-PC-pr9o.zip` をダウンロード（または `rudebreaker.exe` を自分でビルド）して展開し、ゲームファイルを加えて次のようなフォルダにします：
   ```
   Rude Breaker PC/
     rudebreaker.exe
     SDL2.dll
     SGM1.DAT
     SONG.DAT
     FPLAY.COM
   ```
4. `rudebreaker.exe` を実行します。

必要なのはこの3つのファイルだけです。BIOSや音源ROMなど、ほかのファイルは要りません。音楽はYM2203のエミュレーションで鳴らしており、ROMは不要です。

### Linux
Linux x86_64版は `releases/RudeBreaker-Linux-pr9o.zip` にあります。`libSDL2-2.0.so.0` を同梱しており、glibc 2.17以降のほとんどのディストリビューションで動作します（X11またはWayland、PulseAudio・PipeWire・ALSA）。

1. 展開して同じゲームファイルを加え、次のようなフォルダにします：
   ```
   Rude Breaker Linux/
     rudebreaker
     libSDL2-2.0.so.0
     SGM1.DAT
     SONG.DAT
     FPLAY.COM
   ```
   ファイル名は大文字も含めて正確に一致させてください（`sgm1.dat` ではなく `SGM1.DAT`）。
2. そのフォルダ内でターミナルから：
   ```
   chmod +x rudebreaker
   ./rudebreaker
   ```

自分でビルドする場合は、C/C++コンパイラ、`make`、`pkg-config`、SDL2の開発パッケージをインストールし（Debian/Ubuntu：`sudo apt install build-essential pkg-config libsdl2-dev`、Fedora：`sudo dnf install gcc-c++ make SDL2-devel`、Arch：`sudo pacman -S base-devel sdl2`）、次を実行します：
```
make linux
```
バイナリは `build/rudebreaker` に作られます。`make linux` でビルドしたバイナリは、まず自分と同じフォルダの `libSDL2-2.0.so.0` を探し、なければシステムのライブラリを使います。C++ランタイムは静的リンクされます。

### 配布に関する注意
ゲームファイルは **配布しないでください**。ゲームファイルを使用できるのは、ゲームを正規に所有している方（*ディスクステーション Vol.10* のオリジナルディスク、またはプロジェクトEGGでの購入・会員登録）のみです。ゲームを所有していない場合は、**テスト目的に限り最長24時間** までプレイでき、その後はゲームファイルを削除しなければなりません。

### 法的事項
本プロジェクトは非営利のファンプロジェクトであり、コンパイルおよびD4エンタープライズとは一切関係ありません。サードパーティのライセンス：SDL2（zlib）、ymfm（BSD-3-Clause）。

TheFear
=======

■ プレイ動画

https://youtu.be/-nILsROeQO4

■ ゲーム概要

TheFear は、C++20 と DirectX 11 で制作したウェーブ制の 3D アクションゲームです。
近接攻撃と飛翔する剣を使って敵を倒し、次のウェーブへ進みます。
画面には体力、現在のウェーブ、残りの敵の数が表示されます。

飛翔する剣は、待機中は刃先を下に向けてプレイヤーの背後に浮かびます。
発射すると最も近い敵を追尾し、命中地点で三角形の破片に分解します。
その後、プレイヤーの背後で自動的に再構成されます。

■ 起動方法

対象環境は Windows 10 / 11（64 ビット）です。
配布用 ZIP を展開し、TheFear.exe を起動してください。
TheFear.exe、data フォルダー、shader.hlsl は同じフォルダーに置いてください。
配布版の起動に Visual Studio や旧 DirectX SDK のインストールは不要です。

■ 操作方法

  W / A / S / D、矢印キー    移動
  マウス                     視点操作
  左クリック                 近接攻撃
  Space                      剣を発射
  F2                         デバッグパネルの開閉
  Esc                        ゲーム終了

タイトル画面では W / S または上下の矢印キーで項目を選び、Enter で決定します。
死亡時には到達したウェーブとスコアが表示されます。
Enter で再挑戦、M でタイトル画面に戻ります。画面上のボタンでも操作できます。

F2 のパネルでは、当たり判定、敵の追跡方向を示す破線、地面との接地点を
それぞれ表示・非表示にできます。

■ ソースからビルドする場合

Visual Studio 2026 の C++ ビルドツール、CMake、Windows SDK 10.0.26100.0 を使用します。
プロジェクトのルートフォルダーで次のコマンドを実行してください。

  cmake -S . -B build -G "Visual Studio 18 2026" -A x64
  cmake --build build --config Release
  cmake --install build --config Release --prefix build/package

配布用ファイルは build/package にまとまります。配布する際はフォルダー全体を ZIP にしてください。

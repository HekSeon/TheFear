<h1 align="center">🎮 TheFear — DX11 3D C++ ゲーム</h1>



<p align="center">

<b>C++・DirectX11・HLSL / 個人制作プロジェクト</b><br>

低レベルグラフィック × ゲームロジック × 手動エンジン構造

</p>

## プレイ動画

[YouTube でプレイ動画を見る](https://youtu.be/-nILsROeQO4)



<p align="center">

<img src="https://img.shields.io/badge/言語-C++-00599C?logo=c%2B%2B\&style=for-the-badge">	<img src="https://img.shields.io/badge/グラフィック-DirectX11-239120?style=for-the-badge">
<img src="https://img.shields.io/badge/プラットフォーム-PC-blue?style=for-the-badge">
<img src="https://img.shields.io/badge/ステータス-アクティブ-success?style=for-the-badge">

</p>



---



\## 🎮 ゲーム概要



TheFearは、C++とDirectX11を使用して制作した実験的プロジェクトです。  

ゲームエンジンを使用せずに、レンダリング、シェーダー、入力、ゲームメカニクスを手動で実装しています。  

低レベルグラフィックの学習と、ゲームループ設計の理解を目的としています。



---



\## 🧩 主な特徴



\### 💻 レンダリングとシェーダー

\- DirectX11レンダリングパイプラインを自作  

\- Vertex/Indexバッファ管理、コンスタントバッファの使用  

\- HLSLシェーダーによるマテリアルシステム (`shader.hlsl`)



\### 🎮 ゲームロジックと入力

\- キーボード入力処理 (W/A/S/D で移動、左クリックで近接攻撃、Space で剣を発射、F2 でデバッグ表示)

\- 手動でのゲームループ設計（UpdateとRenderの分離）  

\- ゲーム内エンティティの状態管理



\### 🧱 ゲームシステム

\- シンプルな衝突判定とレスポンス  

\- シーン管理（基本レベル設定）  

\- オブジェクト管理（スポーン / デスポーン）



\### ⚡ 最適化と設計

\- 軽量アーキテクチャでスムーズなフレームレート  

\- GPUオブジェクトのメモリ・リソース管理  

\- 無駄な計算を避ける効率的なアップデートループ



---



\## 🛠 使用技術



| 分類 | 内容 |

|------|------|

| 言語 | C++ |

| グラフィック | DirectX11, HLSL Shader |

| ビルド | CMake / Visual Studio |

| 入力 | Win32 Keyboard / Mouse |

| アーキテクチャ | 手動ゲームループ、状態管理、オブジェクトプーリング |

| バージョン管理 | Git / GitHub |



---



\## 📂 プロジェクト構造



TheFear/

├── Source/

│ ├── main.cpp # エントリーポイント / ゲームループ

│ ├── Game.h/.cpp # コアゲームクラス

│ ├── Renderer.h/.cpp # レンダリング管理

│ ├── Input.h/.cpp # 入力管理

│ ├── Entity.h/.cpp # ゲームエンティティロジック

│ └── ...

├── Data/

│ ├── textures/ # テクスチャ

│ └── models/ # 3Dモデル（オプション）
├── shader.hlsl # HLSLシェーダー

├── CMakeLists.txt # CMake ビルド設定

└── README.md



---



\## 🚀 実行方法



### ソースからビルド

ビルドには Visual Studio 2026 の C++ ツールと Windows SDK 10.0.26100.0 が必要です。

~~~powershell
cmake -S . -B build -G "Visual Studio 18 2026" -A x64
cmake --build build --config Release
~~~

`build/Release` に `TheFear.exe`、`data/`、`shader.hlsl` が出力されます。実行ファイルのあるフォルダーを作業ディレクトリにして実行してください。

🔹 注意事項



Windows 10、DirectX11対応環境で動作確認済み



プレイには Windows 10 または Windows 11 が必要です。Visual Studio や June 2010 DirectX ランタイムのインストールは不要です。

配布用フォルダーは次のコマンドで作成できます。作成されたフォルダー全体を配布してください。

~~~powershell
cmake --install build --config Release --prefix build/package
~~~



🧠 学んだこと



DirectX11による低レベルレンダリングとシェーダー制御



手動でのゲームループ設計と状態管理



GPUバッファのメモリ・リソース管理



入力、ゲームロジック、レンダリング統合



C++プロジェクト構造の整理と可読性確保



🔮 今後の改善予定



衝突判定・物理システムの強化



複数ステージ対応のシーン管理



ライティング・シャドウなどのシェーダー拡張



UI / HUDシステムの追加



ゲームパッド / 複数入力対応



👤 作者



BAYAR SEMIH (HekSeon)

HAL東京 ゲームプログラマー学科 / 新卒ゲームプログラマー

DirectX11、Unity、Unrealを活用したゲーム開発を学習中




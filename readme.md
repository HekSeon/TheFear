<h1 align="center">🎮 TheFear — DX11 3D C++ ゲーム</h1>



<p align="center">

<b>C++・DirectX11・HLSL / 個人制作プロジェクト</b><br>

低レベルグラフィック × ゲームロジック × 手動エンジン構造

</p>



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

\- キーボード入力処理 (W/A/S/D + Spaceでジャンプ)  

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

| ビルド | Visual Studio Solution (.sln) |

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

├── TheFear.sln # Visual Studioソリューション

└── README.md



---



\## 🚀 実行方法



\### 🔹 ソースからビルド

```bash

git clone https://github.com/HekSeon/TheFear.git

```

Visual Studioで TheFear.sln を開く



ビルド構成を選択（Debug / Release）



ビルドして実行



注意: shader.hlsl は実行ファイルと同じパスに配置してください



🔹 注意事項



Windows 10、DirectX11対応環境で動作確認済み



外部依存関係は不要



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




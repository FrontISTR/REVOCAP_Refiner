================================================================================

  README of REVOCAP_Refiner

  2010.06.01  version 0.4

  The University of Tokyo, CISS Project
  "次世代ものづくりシミュレーションシステム" Group.

  お問い合わせ先
    e-mail:  software@ciss.iis.u-tokyo.ac.jp

================================================================================

このファイルは REVOCAP_Refiner_0_4.tgz を展開してできるフォルダとファイルの説明書です。

== フォルダの構造

 +- README_Refiner.txt     ( このファイル )
 +- Geometry               ( 幾何処理用ライブラリ )
 +- MeshDB                 ( メッシュ処理用ライブラリ )
 +- MeshGen                ( メッシュ生成用ライブラリ )
 +- RevocapIO              ( メッシュ入出力用ライブラリ )
 +- Refiner                ( メッシュ細分化モジュール本体 )
 +- Makefile               ( 実行体作成用のメイクファイル )
 +- OPTIONS                ( Makefile のオプション指定用のファイル )
 +- Documents              ( ドキュメント )

REVOCAP_Refiner のチュートリアルとマニュアルは Documents フォルダの中に入っています。

ご使用の前に、使用許諾条件に関する文書
Documents/CISS_lic_agree201005_ja.pdf (英語版Documents/CISS_lic_agree201005_en.pdf)
の内容をご確認ください。

REVOCAP_Refiner は幾何処理、メッシュ処理、メッシュ生成、メッシュ入出力ライブラリは
REVOCAP_Mesh と共通のものを使っています。

Makefile は REVOCAP_Mesh と共通になっているので注意してください。

ライブラリの作り方とテスト

(1) OPTIONS ファイルを見て、コンパイラやマクロ変数を編集してください
(2) make Refiner          細分化モジュールをメイクします
(3) make RefinerSampleC   C言語で書いたサンプルプログラムをメイクします
(3) make RefinerSampleF   Fortran言語で書いたサンプルプログラムをメイクします
(4) make RefinerClean     生成されたファイルを削除する


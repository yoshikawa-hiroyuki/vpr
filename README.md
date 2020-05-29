# vpreduce … <small>V-PolygonReducer app</small>

### Overview
vpreduceは、Wavefront OBJ形式のポリゴンデータを読み込み、ポリゴンリダクションを行うプログラムです。

vpreduceはc++で記述され、[wxWidgets](https://www.wxwidgets.org)と[OpenGL](https://www.opengl.org)を使用しています。

また、ポリゴンリダクション機能は[Michael Garland](https://mgarland.org)氏が開発した[QSlim](https://mgarland.org/software/qslim.html)を、
理化学研究所/VCADシステム研究会で開発された[V-Xgen](http://vcad-hpsv.riken.jp/jp/release_software/V-Xgen/)に含まれるポリゴンリダクションモジュールインターフェース(vpr_qslim)を介して使用しています。

---

### Pre-Compiled binary
bin/ 配下に、コンパイル済みの実行ファイルが含まれます。
- bin/Windows  Windows10 64bit
- bin/macOS  macOS 10.15

---

### Author
_Yoshikawa, Hiroyuki_

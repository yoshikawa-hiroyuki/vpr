# vpreduce (<small>V-PolygonReducer app</small>)

### Overview
vpreduceは、Wavefront OBJ形式のポリゴンデータを読み込み、ポリゴンリダクションを行うプログラムです。

vpreduceはC++で記述され、[wxWidgets](https://www.wxwidgets.org)と[OpenGL](https://www.opengl.org)を使用しています。

また、ポリゴンリダクション機能は[Michael Garland](https://mgarland.org)氏が開発した[QSlim](https://mgarland.org/software/qslim.html)を、
理化学研究所/VCADシステム研究会で開発された[V-Xgen](http://vcad-hpsv.riken.jp/jp/release_software/V-Xgen/)に含まれるポリゴンリダクションモジュールインターフェース(vpr_qslim)を介して使用しています。

---

### Pre-Compiled binary
bin/ 配下に、コンパイル済みの実行ファイルが含まれます。
- bin/Windows/vpreduce.exe  (Windows10 64bit)
- bin/macOS/vpreduce.app  (macOS 10.15)

---

### Operation
<dl>
<dt>Open OBJ file</dt>
<dd>FileメニューからOpenを選択すると表示されるファイル選択ダイアログでOBJファイルを選択します。</dd>
<dt>View control</dt>
<dd>マウス左ボタンドラッグで回転、Shift+マウス左ボタンドラッグで平行移動、Ctrl(macOSではCommand)+マウス左ボタン上下ドラッグで拡大・縮小します。マウスホイール回転でも拡大・縮小します。
また、Viewメニューから「Reset View」を選択するか、Spaceバーを押すと初期状態に戻ります。</dd>
<dt>Polygon Reduction</dt>
<dd>ウインドウ下部の「Faces(%)」欄に、オリジナルに対して何%まで削減するかを入力します。また、「#of Faces」欄に直接ポリゴン数で指定することもできます。
いずれの場合も、指定した規模までポリゴンリダクションが行われた状態のモデルがウインドウに3D表示されます。
</dd>
<dt>Save OBJ file</dt>
<dd>FileメニューからSaveを選択すると表示されるファイル選択ダイアログでOBJファイルを指定すると、現在のポリゴン数の状態でOBJファイルを保存します。</dd>
<dt>Quit</dt>
<dd>FileメニューからQuitを選択するか、Escキーを押すとプログラムを終了します。</dd>
</dl>

---

### Build from source
#### Qslim

#### vpr_qslim

#### vpr_app

---

### Author
_Yoshikawa, Hiroyuki_

[toc]



## 说明

本项目旨在于实现天凤牌谱的压缩存储、提取分析，编程语言要求C++>=c++17。目前功能并不完善，只实现了基本的算法，有待后续开发。



### TODO

1. 将本README文档翻译为英文 (todo: translate this README document into english)
2. 完善统计算法
3. 实现向听数计算/听牌检测模块（可以参考 [https://tomohxx.github.io/mahjong-algorithm-book/illustration/](https://tomohxx.github.io/mahjong-algorithm-book/illustration/)），并将这部分预处理集成到统计算法中
4. 实现默听统计模块/听牌种类统计模块（依赖TODO-3）
5. 实现役种分析/过滤模块



### 版本

#### v1.2.0

- 新增高效听牌判断模块（算法设计理论见 `doc/tenpai_algorithm.md` ，代码见 `mjlog_tenpai.*` ），即 `TenPaiCheck` 类。该模块可以独立于本项目工程使用。
-  `TenPaiCheck` 类提供 `get_all_ten_pai` 接口判断当前是否听牌，并返回所有待牌。注意，**调用该接口前必须手动执行**过 `preprocess` ！ `preprocess` 会进行预计算，通过空间+单次预计算时间（release模式下10-20ms）换每次计算的时间开销。
- 新增 `RoundTracer` 模块，能够高效追踪每小局的每个Action执行后的状态。
- 新增立直听牌分布的算法 `stats_richi_tenpai_content` 。

#### v1.1.1

- 新增庄家/闲家立直的局收支/结局相关算法

#### v1.1.0

- **修复一个严重bug**：from_xml对EndAgari的宝牌处理有bug，导致v1.0.0的`.hskmjlog`数据宝牌内容错误

- **新增如下算法**

  - stats_game_round

  - stats_round_continue_oya

  - stats_round_end_type
  - stats_agari_richi_ok_dora_num/stats_agari_first_richi_ok_dora_num/stats_agari_chasing_richi_ok_dora_num/stats_agari_be_chased_richi_ok_dora_num

- 新增`YakuType`

- 新增副露解码接口`flatten_meld`和宝牌计算接口get_dora_count_of_hai/is_aka_dora

- 为`EndData`提供to_agari/to_ryuukyoku方法

- 为`EndAgari`提供is_tsumo/is_meld/is_yakuman/is_richi/get_score方法，以及若干手牌/宝牌计算接口

#### v1.0.1

新增了若干算法，通过宏和模板减少重复代码

新增了`stats.ipynb`，对`output/output.json`进行结果分析（硬编码的路径，vibe coding+微调的结果，我懒得做一般性适配）

#### v1.0.0

实现了基本的算法，开源到github



### 输入依赖

需要从天凤官网爬取的xml牌谱，这部分代码使用**python编写**，没有优化性能（读太快了也有被天凤ban的可能）。你需要将牌谱整理为两级目录的形式，例如：

```
├── tenhou_logs
│   ├── 20250101
│   │   ├── 2025010100gm-00a9-0000-0c603e8f.xml
│   │   ├── 2025010100gm-00a9-0000-1c1838ca.xml
│   │   └── ...
│   ├── 20250102
│   │   ├── 2025010200gm-00a9-0000-2ed1bbfe.xml
│   │   ├── 2025010200gm-00a9-0000-3ad30fdd.xml
│   │   └── ...
│   ├── ...
│   │   └── ...
```

天凤牌谱下载可以参考 [https://github.com/Apricot-S/houou-logs](https://github.com/Apricot-S/houou-logs) ， `hskmjlog_data/` 文件夹存放了我处理好的2025四人南凤凰桌牌谱，数据见 [https://github.com/LiuZJ2019/mjlog-parser-and-statistics/releases/tag/hskmjlog-v1.1.0](https://github.com/LiuZJ2019/mjlog-parser-and-statistics/releases/tag/hskmjlog-v1.1.0) 

> 目前天凤似乎不支持 scraw*.zip 2025年及以前的牌谱下载了，我本地保存了处理好的2025年牌谱（仅含凤凰桌四人南）。考虑到中国网络问题，也扔一份百度网盘版本 [https://pan.baidu.com/s/1RowQq39nq09YaAHyjGacjQ?pwd=0721](https://pan.baidu.com/s/1RowQq39nq09YaAHyjGacjQ?pwd=0721) 



### 编译

要求: C++>=C++17

#### Debug

```
$ mkdir build
$ cd build
$ cmake .. -DCMAKE_BUILD_TYPE=Debug
$ make
```

#### Release

```
$ mkdir build
$ cd build
$ cmake .. -DCMAKE_BUILD_TYPE=Release
$ make
```



## 项目功能

### 功能1：预处理，将xml转换为hskmjlog格式

将xml（例如上tenhou_logs/）转换为hskmjlog格式（例如下hskmjlog_data/），后者为二进制格式，空间压缩为原本的1/3，读入时间压缩为原本的1/100。

```
├── hskmjlog_data
│   ├── 20250101.hskmjlog
│   ├── 20250102.hskmjlog
│   ├── ...
```

命令行参数格式

```
./tenhou_paipu_parser preprocess <input_dir> <output_dir> [check=true|false]
```

使用示例

```bash
./tenhou_paipu_parser preprocess "F:/Desktop/rust/tenhou_logs/" "F:/Desktop/C++/46.mjlogParser/hskmjlog_data/" false
```

- 输入参数1：mode，取值为"preprocess"代表**预处理模式**
- 输入参数2：input_dir，为**输入**的xml文件保存目录（可参考“输入依赖”小节）
- 输入参数3：output_dir，为**输出**的hskmjlog文件保存目录
- 输入参数4（可选）：check，为true代表启用**校验功能**，会校验xml和hskmjlog是否保持一致性，**正常情况下总是能校验通过的**，如果校验不通过说明解析程序有bug，请在github上贴issue。
  - 注意：**启用校验功能会将将预处理速度降低为**未校验版本的60%左右，如果你对本程序正确性具有信任，建议关闭校验以提升性能。

下面是我本地的输出（对2025年所有四人南凤凰桌牌谱的处理结果）：

```
convert_xml_to_hskmjlog success. Game: 178897
convert_xml_to_hskmjlog time: 258.65s
```



### 功能2：执行算法，读入xml或hskmjlog格式的数据，基于算法输出json处理结果

程序可以读入hskmjlog/xml格式的数据（建议使用hskmjlog，读入速度极快，只需2s即可读入10万+四人南牌谱）。

命令行参数格式

```
./tenhou_paipu_parser stats <input_dir> <output_json> [suffix=hskmjlog|xml]
```

使用示例

```bash
./tenhou_paipu_parser stats "F:/Desktop/C++/46.mjlogParser/hskmjlog_data" "F:/Desktop/C++/46.mjlogParser/output.json" "hskmjlog"
```

- 输入参数1：mode，取值为"stats"代表**算法模式**
- 输入参数2：input_dir，为**输入**的hskmjlog文件保存目录（只测试过hskmjlog，理论上xml也是可以的）
- 输入参数3：output_json，为**输出**的结果报表文件保存路径
- 输入参数4（可选）：suffix，为**输入文件格式**，默认为hskmjlog，只支持hskmjlog和xml。

下面是我本地的输出（对2025年所有四人南凤凰桌牌谱的处理结果），采用release模式：

```
Data loaded success. Games: 178897
Data load time: 2.09623s
do_stats_algorithm success. Algorithm: 94
do_stats_algorithm time: 4.29476s
```



### 功能3：结果分析

`stats.ipynb`文件编写了输出的`output/output.json`文件的使用demo，没有做适配，但是python代码很好懂，如果想用稍微改改就行



## FAQ

Q: 为什么要做本项目？
A: 为了提取牌谱并进行统计分析。

Q: 为什么不做多线程版本？
A: xml到hskmjlog预处理是一次性的，而基于hskmjlog的处理程序运行速度已经够快了，不需要多线程。

Q: 为什么要压缩存储？
A: 天凤牌谱是xml格式的，一场四人南大概在15KB左右，不便于统计。压缩为存储后能够节省空间，并且能够更高效地分析牌谱。此外，经过测试，基于tinyxml2库读取压缩前的xml文件（10万个）耗时大概在300s，而读取压缩后的。

Q: 注意到rust有一个天凤牌谱处理库，为什么不用成熟的rust库？
A: rust版本每个Action的空间开销太大，我这个版本对空间进行了极致的优化，每个Action只占2字节。

Q: 为什么要用C++？
A: 原本计划用python的，但是python太慢了，数据量太大的话撑不住。

Q: 为什么C++版本>=c++17？
A: 我用了std::filesystem，当然最低可以降到c++11，但作者懒。



## 解析规则

本规则整理参考了下列资料：

[[心得\] 天鳳牌譜解析#1　牌譜的歷史 - 看板MJ_JP | PTT遊戲區](https://pttgamer.com/MJ_JP/1ax1rZit)

[【天凤麻将】对于天凤麻将的牌谱数据解析-CSDN博客](https://blog.csdn.net/qq_44951699/article/details/157648709)

[天凤牌谱采集及分析](https://notoootori.github.io/2020/07/28/天凤牌谱采集及分析.html)

### 场次信息

前面的标签`<GO>/<UN>/<TAIKYOKU>`为场次信息，本代码除了`GO.type`都不解析。

### 对局段

中间是若干对局段，以`INIT`开始，以`AGARI/RYUUKYOKU`结尾。

#### INIT标签

局数、本场、供托、宝牌、庄家编号：`round, honba, kyotaku, dora, oya = seed[0], seed[1], seed[2], seed[5], oya`

开始时点数除以100：`ten[0], ten[1], ten[2], ten[3]`

起手牌：`hai0, hai1, hai2, hai3`

#### (T|U|V|W|D|E|F|G)(num)

摸牌/打牌，type=4-11，value=num

#### N who=num1 m=num2

副露，比较复杂，懒得写文档，看代码吧，type=12-143（映射规则详见mjlog_meta_type.h）

#### REACH

立直，一阶段立直的type=144-147，二阶段立直的type=148-151

#### BYE/RECONNECT

玩家断线/重连，type=144-147/148-151，注意：如果玩家对局前就掉线，目前版本不会记录。

#### DORA

翻宝牌，type=160

#### AGARI

上一行为：切牌或摸牌或AGARI

下一行为：AGARI或结束

```xml
    <AGARI ba="0,0" hai="8,9,10,27,28,32,87,90,93,120,121" m="48747" machi="10" ten="30,1500,0" yaku="18,1" doraHai="18,127" who="1" fromWho="0" sc="240,-15,270,15,250,0,240,0" />
    <AGARI ba="0,0" hai="10,12,16,24,25,54,59,62" m="25063,56671" machi="10" ten="30,2000,0" yaku="8,1,54,1" doraHai="18,127" who="2" fromWho="0" sc="225,-20,285,0,250,20,240,0" />
    <INIT seed="1,1,0,3,2,92" ten="205,285,270,240" oya="1" hai0="10,102,128,96,115,78,99,44,22,85,103,13,129" hai1="30,123,82,51,56,125,15,91,135,76,20,74,133" hai2="94,108,93,75,47,16,107,72,69,6,104,63,109" hai3="67,71,21,132,70,9,23,95,79,110,38,43,66"/>
    <U57/>
```

- ba:本场数/供托数
- hai(**此处hai-x可以不是13张**！)
- m:副露编码
- machi:胡牌
- ten:符/点/级别(0: 满贯未满, 1: 满贯, 2: 跳满, 3: 倍满, 4: 三倍满, 5: 役满)
- yaku(如果非役满):非役满的编号/番数
- yakuman(如果役满):役满编号
- doraHai:宝牌
- doraHaiUra(如果立直):里宝牌
- who:和牌者
- fromWho:铳者
- sc:0号点数/0号点数变化/1号点数/1号点数变化/2号点数/2号点数变化/3号点数/3号点数变化
- owari(如果终局):持有点数/段位分(有小数点)

#### RYUUKYOKU(普通流局)

上一行为：切牌

下一行为：结束

```xml
<RYUUKYOKU ba="0,2" sc="250,-15,250,-15,240,15,240,15" hai2="6,8,13,15,19,22,36,37,52,58,78,83,84" hai3="2,7,10,12,18,23,30,32,41,46,50,65,66" />
```

- ba:本场数/供托数
- sc:0号点数/0号点数变化/1号点数/1号点数变化/2号点数/2号点数变化/3号点数/3号点数变化
- hai-x(x家的牌，因为前面2/3家听牌了，需要展示手牌，**此处hai-x可以不是13张**！)
- owari(如果终局):持有点数/段位分(有小数点)

#### RYUUKYOKU(流局满贯)

上一行为：切牌或流局满贯

下一行为：流局满贯或结束

```xml
<RYUUKYOKU type="nm" ba="0,2" sc="245,-40,247,120,237,-40,251,-40" hai2="4,5,18,21,24,51,55,79,81,87,90,94,96" hai3="23,26,28,50,52,56,61,65,80,82,88,89,91" />
```

- ba:本场数/供托数
- sc:0号点数/0号点数变化/1号点数/1号点数变化/2号点数/2号点数变化/3号点数/3号点数变化
- hai-x(x家的牌，因为前面2/3家听牌了，需要展示手牌，**此处hai-x可以不是13张**！)
- owari(如果终局):持有点数/段位分(有小数点)

#### RYUUKYOKU(九种九牌)

上一行为：摸牌

下一行为：结束

```xml
<RYUUKYOKU type="yao9" ba="0,0" sc="218,0,37,0,363,0,382,0" hai1="0,12,43,62,70,72,95,102,105,114,119,121,128,134" />
```

- ba:本场数/供托数
- sc:0号点数/0号点数变化/1号点数/1号点数变化/2号点数/2号点数变化/3号点数/3号点数变化
- hai-x(流局家的牌)

#### RYUUKYOKU(四风连打)

上一行为：切牌

下一行为：结束

```xml
<RYUUKYOKU type="kaze4" ba="0,0" sc="292,0,267,0,237,0,204,0" />
```

- ba:本场数/供托数
- sc:0号点数/0号点数变化/1号点数/1号点数变化/2号点数/2号点数变化/3号点数/3号点数变化

#### RYUUKYOKU(四家立直)

上一行为：REACH-step 2

下一行为：结束

```xml
<RYUUKYOKU type="reach4" ba="0,4" sc="174,0,388,0,184,0,214,0" hai0="20,21,22,36,43,46,55,57,58,59,61,66,67" hai1="7,11,14,56,62,82,85,90,99,101,104,117,118" hai2="15,16,23,40,41,47,51,54,68,70,71,84,88" hai3="9,13,17,38,39,50,53,63,65,69,109,110,111" />
```

- ba:本场数/供托数
- sc:0号点数/0号点数变化/1号点数/1号点数变化/2号点数/2号点数变化/3号点数/3号点数变化
- hai-x

#### RYUUKYOKU(三家和了)

上一行为：切牌

下一行为：结束

```xml
<RYUUKYOKU type="ron3" ba="2,0" sc="196,0,231,0,329,0,244,0" hai0="7,8,9,10,11,13,52,58,61,93,94,96,99" hai1="14,16,23,76,79,83,85,91,98,101" hai2="47,49,55,60,62,86,89" />
```

- ba:本场数/供托数
- sc:0号点数/0号点数变化/1号点数/1号点数变化/2号点数/2号点数变化/3号点数/3号点数变化
- hai-x（注意，**此处hai-x可以不是13张**！）

#### RYUUKYOKU(四杠散了)

上一行为：切牌

下一行为：结束

- 明杠(加杠)->摸牌->DORA->切牌->流局
- 暗杠->DORA->摸牌->切牌->流局

```xml
<RYUUKYOKU type="kan4" ba="1,2" sc="235,0,235,0,255,0,255,0" />
```

- ba:本场数/供托数
- sc:0号点数/0号点数变化/1号点数/1号点数变化/2号点数/2号点数变化/3号点数/3号点数变化



### 关于.hskmj

```
ROUNDx:
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
+ 0 + 1 + 2 + 3 + 4 + 5 + 6 + 7 + 08-15 + 16-28 + 29-41 + 42-54 + 55-67 + 68-69 +  ...  + ? +  ...  +
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
+ R +  LEN  + R + H + K + D + O +  TEN  + HAI0  + HAI1  + HAI2  + HAI3  + ACTION+ ITEMx + E +  ENDx +
+ O +       + O + O + Y + O + Y +       +       +       +       +       + COUNT +       + N +       +
+ U +       + U + N + O + R + A +       +       +       +       +       +       +       + D +       +
+ N +       + N + B + T + A +   +       +       +       +       +       +       +       + C +       +
+ D +       + D + A + A +   +   +       +       +       +       +       +       +       + O +       +
+ T +       +   +   + K +   +   +       +       +       +       +       +       +       + U +       +
+ A +       +   +   + U +   +   +       +       +       +       +       +       +       + N +       +
+ G +       +   +   +   +   +   +       +       +       +       +       +       +       + T +       +
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
GAME
+---+---+---+---+---+---+---+---+---+
+ 0 +     01-04     + 05-06 +  ...  +
+---+---+---+---+---+---+---+---+---+
+ G +    GAME LEN   +  TYPE + ROUNDx+
+ A +               +       +       +
+ M +               +       +       +
+ E +               +       +       +
+ T +               +       +       +
+ A +               +       +       +
+ G +               +       +       +
+   +               +       +       +
+---+---+---+---+---+---+---+---+---+
```


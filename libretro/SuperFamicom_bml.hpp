static std::string SuperFamicom_bml(R"(//database
//  revision: 2020-01-21

//Prototypes (JPN)

//database
//  revision: 2018-04-14

game
  sha256:   182cd72c2ef57119b56bef1f7c18660498422a912f1bb652771d465cd183b04e
  label:    From TV Animation - Slam Dunk - 集英社Limited
  name:     From TV Animation - Slam Dunk - Shuueisha Limited
  region:   JPN
  revision: 1.0
  board:    SHVC-4PV5B-01
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   bcba4ca39f0279f7a52657bccbffa84564eaea455e2565597b93942ec245fdb1
  label:    くにおくんのドッジボールだよ 全員集合! トーナメントスペシャル
  name:     Kunio-kun no Dodgeball da yo - Zen'in Shuugou! - Tournament Special
  region:   JPN
  revision: 1.0
  board:    SHVC-2P3B-01
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   4362afe51da8a01a67a1c2d6c2545d9e7cbaf8c42fd8f319dd8b61fe328512e1
  name:     Super FX voxel demo
  title:    Super FX voxel demo
  label:    Voxels in progress
  region:   USA
  revision: 1.0
  board:    GSU-RAM
    memory
      type: ROM
      size: 0x60000
      content: Program
    memory
      type: RAM
      size: 0x10000
      content: Save
    oscillator
      frequency: 21440000

//Super Comboy (KOR)

//database
//  revision: 2018-04-14

game
  sha256:   b820ef79c16b3f43139cc9622c685020db3e87364c3a0f3946242bff93b787c8
  label:    드래곤볼Z 초무투전3
  name:     Dragon Ball Z - Chomutujeon
  region:   SNSN-AZ4K-KOR
  revision: SNS-AZ4K-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   9170aacd61bd2f936818db901451e44c18f3e88fd8d534553c2650ccad082466
  label:    한국프로야구
  name:     Hanguk Pro Yagu
  region:   SNSN-3D-KOR
  revision: SKOR-3D-0
  board:    SHVC-2B3B-01
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save
    memory
      type: ROM
      size: 0x1800
      content: Program
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1
    memory
      type: ROM
      size: 0x800
      content: Data
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1
    memory
      type: RAM
      size: 0x200
      content: Data
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1
      volatile
    oscillator
      frequency: 7600000

game
  sha256:   1bb9ba72dfec638ed4cc7c721e31fde2f3e08d160b266a62b6e3997f711cf7cd
  label:    태권도
  name:     Taekwondo
  region:   SNSN-II-KOR
  revision: SKOR-II-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

//Super Famicom (HKG)

//database
//  revision: 2018-04-14

game
  sha256:   bd763c1a56365c244be92e6cffefd318780a2a19eda7d5baf1c6d5bd6c1b3e06
  label:    Super Mario World 2: Yoshi's Island
  name:     Super Mario World 2 - Yoshi's Island
  region:   SNSN-YI-HKG
  revision: SNS-YI-1
  board:    SHVC-1CB5B-20
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save
    oscillator
      frequency: 21440000

// Super Famicom (Competition Carts)

//database
//  revision: 2024-09-21

game
  sha256:   67c1a4917f4cd0fd704b83330a7e91763736c2d2a10a7e12318fdac54cd9c6c6
  label:    Super Nintendo Campus Challenge 1992 (Japan) (En)
  name:     Super Nintendo Campus Challenge 1992 (Japan) (En)
  title:    Campus Challenge '92
  region:   NTSC
  revision: 1.0
  board:    EVENT-CC92
    memory
      type: ROM
      size: 0x40000
      content: Program
    memory
      type: ROM
      size: 0x80000
      content: Level-1
    memory
      type: ROM
      size: 0x80000
      content: Level-2
    memory
      type: ROM
      size: 0x80000
      content: Level-3
    memory
      type: RAM
      size: 0x2000
      content: Save
      volatile
    memory
      type: ROM
      size: 0x1800
      content: Program
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1
    memory
      type: ROM
      size: 0x800
      content: Data
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1
    memory
      type: RAM
      size: 0x200
      content: Data
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1
      volatile
    oscillator
      frequency: 8000000

game
  sha256:   5443a97e9c40e25821a8fb8c91b63c7bd8c3060d9ff888ee6c573b87b53935f4
  label:    PowerFest 94 (USA)
  name:     PowerFest 94 (USA)
  title:    PowerFest '94
  region:   NTSC
  revision: 1.0
  board:    EVENT-PF94
    memory
      type: ROM
      size: 0x40000
      content: Program
    memory
      type: ROM
      size: 0x80000
      content: Level-1
    memory
      type: ROM
      size: 0x80000
      content: Level-2
    memory
      type: ROM
      size: 0x100000
      content: Level-3
    memory
      type: RAM
      size: 0x2000
      content: Save
      volatile
    memory
      type: ROM
      size: 0x1800
      content: Program
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1
    memory
      type: ROM
      size: 0x800
      content: Data
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1
    memory
      type: RAM
      size: 0x200
      content: Data
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1
      volatile
    oscillator
      frequency: 8000000

//Super Famicom (JPN)

//database
//  revision: 2020-01-21

game
  sha256:   5c4e283efc338958b8dd45ebd6daf133a9eb280420a98e2e1df358ae0242c366
  label:    スパイダーマン リーサルフォーズ
  name:     Amazing Spider-Man, The - Lethal Foes
  region:   SHVC-ASPJ-JPN
  revision: SHVC-ASPJ-0
  board:    SHVC-2J0N-11
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   6f6bacdd73aef29ff6a015c25db4a5cd8ba31142b2cc3fe56261d23bbf8329ea
  label:    アンジェリーク プレミアムBox
  name:     Angelique - Premium Box
  region:   SHVC-AAZJ-JPN
  revision: SHVC-AAZJ-0
  board:    SHVC-1J5M-01
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save

game
  sha256:   1f7619ea0d02f16e2b2dcbb36013bb3405eb791885f23884b583eb63768614c4
  label:    アンジェリーク ヴォイス・ファンタジー
  name:     Angelique - Voice Fantasy
  region:   SHVC-AQLJ-JPN
  revision: SHVC-AQLJ-0
  board:    SHVC-1J5M-01
    memory
      type: ROM
      size: 0x300000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save

game
  sha256:   a98eb5f0521746e6ce6d208591e86d366b6e0479d96474bfff43856fe8cfec12
  label:    バハムートラグーン
  name:     Bahamut Lagoon
  region:   SHVC-AXBJ-JPN
  revision: SHVC-AXBJ-0
  board:    SHVC-1J3M-20
    memory
      type: ROM
      size: 0x300000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   47779500c43a0c2e75d7684078489a17baea31170a123063b8ece6ce77359413
  label:    ボール・ブレット・ガン
  name:     Ball Bullet Gun
  region:   SHVC-AAGJ-JPN
  revision: SHVC-AAGJ-0
  board:    SHVC-1A3M-30
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   a05f76cbceffc7b015491a1c4d4913758484d392471bca20af1dce6fd62d878b
  label:    バトルトードインバトルマニアック
  name:     Battletoads in Battlemaniacs
  region:   SHVC-8T
  revision: SHVC-8T-0
  board:    SHVC-YA0N-01
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   af12d61733f120ef25cac2c095e0152345143025eb9d8ef820c7f857207d46ac
  label:    バイオメタル
  name:     Bio Metal
  region:   SHVC-BV
  revision: SHVC-BV-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   ecd772c4a21101d079a795e47abbe00052bef69cc1c854a328f0077016c53311
  label:    ボンバーマン ビーダマン
  name:     Bomberman B-Daman
  region:   SHVC-AH9J-JPN
  revision: SHVC-AH9J-0
  board:    SHVC-1J3M-20
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   3ce321496edc5d77038de2034eb3fb354d7724afd0bc7fd0319f3eb5d57b984d
  label:    BS-X それは名前を盗まれた街の物語
  name:     BS-X - Sore wa Namae o Nusumareta Machi no Monogatari
  region:   SHVC-ZBSJ-JPN
  revision: SHVC-ZBSJ-1
  board:    BSC-1A5B9P-01
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save
    memory
      type: RAM
      size: 0x80000
      content: Download

game
  sha256:   6e7dcbb4df32903d6ff5da1e308342c0a72f5af3f11479cf49391dc3a17d5d7b
  label:    キャプテンコマンドー
  name:     Captain Commando
  region:   SHVC-QM
  revision: SHVC-QM-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   0938ff33f5bab359e383bb5499f4fcc2a488fe49747026db355c2d3d5c7c2fdb
  label:    クロノ・トリガー
  name:     Chrono Trigger
  region:   SHVC-ACTJ-JPN
  revision: SHVC-ACTJ-0
  board:    SHVC-BJ3M-20
    memory
      type: ROM
      size: 0x400000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   0a8e5b78caf79f4710de3ccc41e2d1975cff3a6cb4146be9ed1c8767be1b0c5d
  label:    クロノ・トリガー 体験版サンプルROM
  name:     Chrono Trigger - Taikenban Sample ROM
  region:   SHVC-AC9J-JPN
  revision: SHVC-AC9J-0
  board:    SHVC-1J3M-11
    memory
      type: ROM
      size: 0x400000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save
      volatile
  note: No battery on PCB

game
  sha256:   c195641a1b472590cb3d0be0c48d682b9fee94d7b700dd7bd3297bb995b49307
  label:    Conveni Wars Barcode Battler 戦記 スーパー戦士 出撃せよ!
  name:     Conveni Wars - Barcode Battler Senki - Super Senshi Shutsugeki seyo!
  region:   SHVC-B5
  revision: SHVC-B5-0
  board:    SHVC-1J3B-01
    memory
      type: ROM
      size: 0x80000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   7fccf61a698b250f01b548fd20a7c133ef1c6dbb8172d3ee6c24657b60820a00
  label:    であえ殿さま あっぱれ一番
  name:     Deae Tonosama Appare Ichiban
  region:   SHVC-ADTJ-JPN
  revision: SHVC-ADTJ-0
  board:    SHVC-2A0N-20
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   248217975279bbf9db8e74da11a906a6dd867a3ec88441b0b031ecf900466618
  label:    デモンズ・ブレイゾン 魔界村紋章編
  name:     Demon's Blazon - Makaimura Monshou Hen
  region:   SHVC-3Z
  revision: SHVC-3Z-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   700e5d0a79a46d343216b822e28e6f3d0f33f68906f59b1c719735365c129553
  label:    デア ラングリッサー
  name:     Der Langrisser
  region:   SHVC-ALGJ-JPN
  revision: SHVC-ALGJ-1
  board:    SHVC-1A3M-30
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   38a855229eab468c3ede7573db73082c66b157adfc7af787ccac50559b747f5f
  label:    ダービースタリオン96
  name:     Derby Stallion '96
  region:   SHVC-ZDBJ-JPN
  revision: SHVC-ZDBJ-0
  board:    BSC-1A5M-02
    memory
      type: ROM
      size: 0x300000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save

game
  sha256:   b7209ec3a5a0d28724f5867343195aef7cb85aeb453aa84a6cbe201b61b0d083
  label:    ドレミファンタジー ミロンのドキドキ大冒険
  name:     DoReMi Fantasy - Milon no Dokidoki Daibouken
  region:   SHVC-AM4J-JPN
  revision: SHVC-AM4J-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   dcb14c95f058a32f40cc329793f5d95fd6cf1755cffe02c0594d1c583a06d356
  label:    エミット Vol. 1 時の迷子
  name:     Emit Vol. 1 - Toki no Maigo
  region:   SHVC-AEMJ-JPN
  revision: SHVC-AEMJ-0
  board:    SHVC-1J3M-20
    memory
      type: ROM
      size: 0x300000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   dc1ecf27d9ce4fdf674c9405339016f5a812f7c4687e588cc6404e2b3b92541a
  label:    エミット Vol. 2 命がけの旅
  name:     Emit Vol. 2 - Inochigake no Tabi
  region:   SHVC-AEIJ-JPN
  revision: SHVC-AEIJ-0
  board:    SHVC-1J3M-20
    memory
      type: ROM
      size: 0x300000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   f49417cb8759a30e12439e846f7d581afd1519c625e6a0522876666098521fcc
  label:    エミット Vol. 3 私にさよならを
  name:     Emit Vol. 3 - Watashi ni Sayonara o
  region:   SHVC-AETJ-JPN
  revision: SHVC-AETJ-0
  board:    SHVC-1J3M-20
    memory
      type: ROM
      size: 0x300000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   69d06a3f3a4f3ba769541fe94e92b42142e423e9f0924eab97865b2d826ec82d
  label:    Far East of Eden 天外魔境Zero 少年ジャンプの章
  name:     Far East of Eden - Tengai Makyou Zero - Shounen Jump no Shou
  region:   SHVC-AZQJ-JPN
  revision: SHVC-AZQJ-0
  board:    SHVC-LDH3C-01
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: ROM
      size: 0x400000
      content: Data
    memory
      type: RAM
      size: 0x2000
      content: Save
    memory
      type: RTC
      size: 0x10
      content: Time
      manufacturer: Epson
      identifier: RTC4513

game
  sha256:   74aa3a26b66f34819fbbdcdb2475cf9161cc2590fb1ec89fb24940ef10e44332
  label:    ファイナルファンタジーIV
  name:     Final Fantasy IV
  region:   SHVC-F4
  revision: SHVC-F4-0
  board:    SHVC-1A3B-11
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   32125257a20c4a6495131f5df79128d189cf6dfb6d45e5314f8b0173ac6f6ebd
  label:    ファイナルファイト タフ
  name:     Final Fight Tough
  region:   SHVC-AFZJ-JPN
  revision: SHVC-AFZJ-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x300000
      content: Program

game
  sha256:   2d0e06e970ad7a1305db754a3a92b6e07e2eab848be196c8182b48dc416f1762
  label:    ファイアーエムブレム トラキア776
  name:     Fire Emblem - Thracia 776
  region:   SHVC-BFRJ-JPN
  revision: SHVC-BFRJ-0
  board:    SHVC-1A5M-20
    memory
      type: ROM
      size: 0x400000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save

game
  sha256:   db337a2e8cf6de653d092ba3489cabc658f91c63ec8a9db4e1866400aadf913f
  label:    ゲットインザホール
  name:     Get in the Hole
  region:   SHVC-AGHJ-JPN
  revision: SHVC-AGHJ-0
  board:    SHVC-2J3M-20
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   b248b2122a0caf99298ebd9a4f66ad8047dbfce1e4bbac8219ba3ea9fb7488b5
  label:    ゴーストチェイサー電精
  name:     Ghost Chaser Densei
  region:   SHVC-ET
  revision: SHVC-ET-0
  board:    SHVC-1J0N-10
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   3a59d7d8df992e9e19c3944f1e17b8a3e9500b266412f51af306e14ff543ab45
  label:    グール・パトロール
  name:     Ghoul Patrol
  region:   SHVC-AGJJ-JPN
  revision: SHVC-AGJJ-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   4e790991a1dc4a9df209f68895412597b4069b42971683c3a3092a160556f305
  label:    ごきんじょ冒険隊
  name:     Gokinjo Boukentai
  region:   SHVC-A3LJ-JPN
  revision: SHVC-A3LJ-0
  board:    SHVC-1J1M-20
    memory
      type: ROM
      size: 0x280000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   8d4e736e1876182a5cc6d9dcc3ca4eb36b16485bc35e2f40e750d023138ada43
  label:    美食戦隊 薔薇野郎
  name:     Gourmet Sentai - Bara Yarou
  region:   SHVC-AV6J-JPN
  revision: SHVC-AV6J-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   4a910531609abb6e0a8bab15c1c6269b608eb72cb2fbf227c0e706d0d6f6fe5b
  label:    ザ・グレイトバトルV
  name:     Great Battle V, The
  region:   SHVC-AG5J-JPN
  revision: SHVC-AG5J-0
  board:    SHVC-2A0N-20
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   83a9d5c7732677838fd4812071aacf04e513e43b3a52e68bd7242db23b2ecc95
  label:    鋼
  name:     Hagane
  region:   SHVC-AHGJ-JPN
  revision: SHVC-AHGJ-0
  board:    SHVC-1J0N-10
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   8e0d620a307a225a757bbc9ef2a2a666792e5d533aa0279d3c0060a1b93ead82
  label:    アイアンコマンドー 鋼鉄の戦士
  name:     Iron Commando - Koutetsu no Senshi
  region:   SHVC-AICJ-JPN
  revision: SHVC-AICJ-0
  board:    SHVC-2A0N-20
    memory
      type: ROM
      size: 0x140000
      content: Program

game
  sha256:   4dfba33201de6b5dec952d0f327aeb44ed784c025a72c982356dd41b52efc219
  label:    糸井重里のバス釣りNo. 1
  name:     Itoi Shigesato no Bass Tsuri No. 1
  region:   SHVC-ZBPJ-JPN
  revision: SHVC-ZBPJ-0
  board:    BSC-1L3B-01
    memory
      type: ROM
      size: 0x400000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save
    memory
      type: RAM
      size: 0x800
      content: Internal
      volatile

game
  sha256:   23b320be74b9fc20de512080be3051575ba36c3246d5c4ee224f31a2fa7808f5
  label:    常勝麻雀天牌
  name:     Joushou Mahjong Tenpai
  region:   SHVC-ZTMJ-JPN
  revision: SHVC-ZTMJ-0
  board:    BSC-1J3M-01
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   ff19a9c3f5a1ccb3ac6261886f54870ac910b0f25df9e46a436e4a621f8a0a59
  label:    化学者ハリーの波乱万丈
  name:     Kagakusha Harley no Haran Banjou
  region:   SHVC-HV
  revision: SHVC-HV-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   f9ec39546e18b15b8f6a738204d0227c1542cd8157e3e0ea16934e76f39e288c
  label:    迦楼羅王
  name:     Karuraou
  region:   SHVC-OH
  revision: SHVC-OH-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   1281841d3b9624e3b4cc3234f46350ce65473fba8d963b30a9f25b5385b8dd83
  label:    奇々怪界 謎の黒マント
  name:     Kiki Kaikai - Nazo no Kuro Manto
  region:   SHVC-KK
  revision: SHVC-KK-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   374e411f64e4fd581a32296c90a5c74c0adf2936003077565e0672d0a91affdf
  label:    奇々快界 月夜草子
  name:     Kiki Kaikai - Tsukiyo Soushi
  region:   SHVC-3N
  revision: SHVC-3N-0
  board:    SHVC-2A0N-11
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   1906b351d51fc0fc6d5a3cfa0fe7cb45b10d09aba256c411f5abad827bce95c6
  label:    カービイのきらきらきっず
  name:     Kirby no Kirakira Kids
  region:   SHVC-BKKJ-JPN
  revision: SHVC-BKKJ-0
  board:    SHVC-1A1M-20
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   0266520683f2df2179a2e6abf62754d0e8c7d82d12e59d3d3cbf7ae403a2625f
  label:    レナスII 封印の使徒
  name:     Lennus II - Fuuin no Shito
  region:   SHVC-ALNJ-JPN
  revision: SHVC-ALNJ-0
  board:    SHVC-1J3M-20
    memory
      type: ROM
      size: 0x400000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   fe44f9d0db9f04f704764577b94e5bf2e18bc7a1c4ff1e6bdaca06d49ed6813c
  label:    リーサルエンフォーサーズ
  name:     Lethal Enforcers
  region:   SHVC-LK
  revision: SHVC-LK-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   106c8158a10f493e0f57bd66ee3b3db36af01964bc44a48819498bf02cb1af7c
  label:    マジカルドロップ2 文化放送スペシャルバージョン
  name:     Magical Drop 2 - Bunka Housou Special Version
  region:   SHVC-AOQJ-JPN
  revision: SHVC-AOQJ-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   ed617ad12c865fc9c9c5c75de840d3afeded57d13ca3a3062bf8e30095629414
  label:    マジカルポップン
  name:     Magical Pop'n
  region:   SHVC-AIAJ-JPN
  revision: SHVC-AIAJ-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   78d0f6dd9ce0813e0532c7b25c7fa0b6b945d12a4ace21aa940e98babf4dacb1
  label:    魔獣王
  name:     Majuuou
  region:   SHVC-AOHJ-JPN
  revision: SHVC-AOHJ-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   c51c5930b344f553415d54c3c964c050e1eb6355b10f5966deabb686e70e1750
  label:    マリオとワリオ
  name:     Mario & Wario
  region:   SHVC-WE
  revision: SHVC-WE-0
  board:    SHVC-YA0N-01
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   e842cac1a4301be196f1e137fbd1a16866d5c913f24dbca313f4dd8bd7472f45
  label:    マリオペイント
  name:     Mario Paint
  region:   SHVC-MP
  revision: SHVC-MP-0
  board:    SHVC-1A5B-04
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save

game
  sha256:   dd314086a62e587bc6ad50c84a38a6ff9082b2d2d06dc50be5fa4c096bed5da3
  label:    マイティ・モーフィン・パワーレンジャー
  name:     Mighty Morphin Power Rangers
  region:   SHVC-52
  revision: SHVC-52-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   2298d92acdfecc7270a6c9a57a6ddc55d7fa841fe9c0e7c0d64e33682fffa429
  label:    ミニ四駆シャイニングスコーピオン レッツ&ゴー!!
  name:     Mini Yonku Shining Scorpion - Let's & Go!!
  region:   SHVC-A4WJ-JPN
  revision: SHVC-A4WJ-0
  board:    SHVC-1L5B-20
    memory
      type: ROM
      size: 0x400000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save
    memory
      type: RAM
      size: 0x800
      content: Internal
      volatile

game
  sha256:   a5bfdaaf490d834917f7ac23ec115147b4c94bf4c18c62e18c64431d7cc79b01
  label:    もと子ちゃんのワンダーキッチン
  name:     Motoko-chan no Wonder Kitchen
  region:   SHVC-WK
  revision: SHVC-WK-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   97c5dcdc457fb29e6294ed93bc2a74117126c45eb399c7caf5920eca36fb63ec
  label:    忍者龍剣伝巴
  name:     Ninja Ryuukenden Tomoe
  region:   SHVC-ANRJ-JPN
  revision: SHVC-ANRJ-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   c48a4ca22d001eb269fd85110fbe459034bf89f14b5b2733ee70b2d10c0687c0
  label:    ザ・ニンジャウォリアーズアゲイン
  name:     Ninja Warriors, The - Again
  region:   SHVC-NI
  revision: SHVC-NI-0
  board:    SHVC-2A0N-11
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   b41126e52ce1696da2efe524f7fd7467d76dcc3fbfa52ff6666671137587b089
  label:    ノーマーク暴牌党 史上最強の雀士達
  name:     Nomark Baku Haitou - Shijou Saikyou no Janshi-tachi
  region:   SHVC-AAPJ-JPN
  revision: SHVC-AAPJ-0
  board:    SHVC-1J3M-20
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   d712adecbde70a74c4a580fe90a45d0d19f2641d1b4e091d507bddeec9601de1
  label:    音楽ツクールかなでーる
  name:     Ongaku Tsukuuru Kanadeeru
  region:   SHVC-ZMCJ-JPN
  revision: SHVC-ZMCJ-0
  board:    BSC-1J5M-01
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save

game
  sha256:   18434c35fe5196bf699494bb5dbabda23019cfea099f4ca638437fda43133b39
  label:    パチンコファン 勝利宣言
  name:     Pachinko Fan - Shouri Sengen
  region:   SHVC-APSJ-JPN
  revision: SHVC-APSJ-1
  board:    SHVC-1A3M-30
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   87e8e3f8b4fc83d2e56662c8b6844b104f6b562744c090d96cdacfcb9523af39
  label:    パチンコ鉄人 七番勝負
  name:     Pachinko Tetsujin - Nanaban Shoubu
  region:   SHVC-A77J-JPN
  revision: SHVC-A77J-0
  board:    SHVC-2A3M-20
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   e9a1f5cf088e060293b5566e7f518b0da89aeaebffb1c3f73fcc3f36fac036ae
  label:    ピットフォール マヤの大冒険
  name:     Pitfall - Maya no Daibouken
  region:   SHVC-APAJ-JPN
  revision: SHVC-APAJ-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   96af51216ee7d2978265a474e382835a88b78d12b456a57f39f944a048e95574
  label:    プロック
  name:     Plok!
  region:   SHVC-P4
  revision: SHVC-P4-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   e98821977389fe0ae16ec22e1f63d226698b5f869b2228885bd231a8a551c003
  label:    ポコニャン! へんぽこりんアドベンチャー
  name:     Pokonyan! - Henpokorin Adventure
  region:   SHVC-APOJ-JPN
  revision: SHVC-APOJ-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   82a9ee11b5640409c67772363f1148517b26127cef13aa2a8ffc2480b487d81f
  label:    レンダリング・レンジャーR²
  name:     Rendering Ranger R2
  region:   SHVC-AVCJ-JPN
  revision: SHVC-AVCJ-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   4fc2832e7aa01d105ca67977b38840ec1188869b5e74d20e58613c1cd127d78f
  label:    ロックマン&フォルテ
  name:     Rockman & Forte
  region:   SHVC-AR6J-JPN
  revision: SHVC-AR6J-0
  board:    SHVC-1J1M-20
    memory
      type: ROM
      size: 0x400000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   2626625f29e451746c8762f9e313d1140457fe68b27d36ce0cbee9b5c5be9743
  label:    ロックマンエックス
  name:     Rockman X
  region:   SHVC-RX
  revision: SHVC-RX-0
  board:    SHVC-2A0N-01#A
    memory
      type: ROM
      size: 0x180000
      content: Program
  note: Custom wiring on PCB

game
  sha256:   76f80cdf704a0e1daf1af5bbf564e427b425a5ee42329417de6f29219fe63e5f
  label:    ロックマンエックス
  name:     Rockman X
  region:   SHVC-RX
  revision: SHVC-RX-1
  board:    SHVC-2A0N-11
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   6dfc016c571a16e5d42045060b1a88b6f3da5831e05b33c22035e1d990deccf3
  label:    ロマンシング サ・ガ3 体験版サンプルROM
  name:     Romancing SaGa 3 - Taikenban Sample ROM
  region:   SHVC-AL9J-JPN
  revision: SHVC-AL9J-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x400000
      content: Program

game
  sha256:   d1e0d1c930011d22423cb7cde8feac445a00705da8067a4e53a735b08389a19d
  label:    RPGツクール2
  name:     RPG Tsukuuru 2
  region:   SHVC-ZR2J-JPN
  revision: SHVC-ZR2J-0
  board:    BSC-1A7M-01
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x10000
      content: Save

game
  sha256:   00e78318926e5cae79bce0535fddd3dccaa732f5c70e43acefc2769a9899eaed
  label:    ラッシング・ビート修羅
  name:     Rushing Beat Shura
  region:   SHVC-R6
  revision: SHVC-R6-0
  board:    SHVC-1J0N-01
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   3a709383208d8258dceb20a5e566903326515ba42931bf97fd389a415a13a72d
  label:    鮫亀
  name:     Same Game
  region:   SHVC-ZS5J-JPN
  revision: SHVC-ZS5J-0
  board:    BSC-1J3M-01
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   208f5ecb7a82b33629e67f39e96b7aa7a992cb2fcc3d1e4eb959abb0a8b7dd95
  label:    SDガンダム ジーネクスト
  name:     SD Gundam G Next
  region:   SHVC-ZX3J-JPN
  revision: SHVC-ZX3J-0
  board:    BSC-1L5B-01
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save
    memory
      type: RAM
      size: 0x800
      content: Internal
      volatile

game
  sha256:   f1acb9fb0c73b4799b28c26d339823a8c6713106fdf1a15260e75ebb47b376f7
  label:    聖剣伝説3 体験版サンプルROM
  name:     Seiken Densetsu 3 - Taikenban Sample ROM
  region:   SHVC-A3EJ-JPN
  revision: SHVC-A3EJ-0
  board:    SHVC-1J3M-20
    memory
      type: ROM
      size: 0x400000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save
      volatile
  note: No battery on PCB

game
  sha256:   601161a459e68824a24e635190e9e786dc93081803b4db66ac5a4744bb422841
  label:    少年忍者サスケ
  name:     Shounen Ninja Sasuke
  region:   SHVC-EO
  revision: SHVC-EO-0
  board:    SHVC-2A3M-11
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   302ba8a084081bbdd4f1f25bb473fe072b07d1514716c5a3ffd258595e9a176d
  label:    ソニックブラストマンII
  name:     Sonic Blast Man II
  region:   SHVC-2C
  revision: SHVC-2C-0
  board:    SHVC-2J0N-11
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   7be858fc681df6728650f460d67fe6c80d816d5fbfc530c11153f652f8b1878e
  label:    サウンドノベルツクール
  name:     Sound Novel Tsukuuru
  region:   SHVC-ZSNJ-JPN
  revision: SHVC-ZSNJ-0
  board:    BSC-1A7M-10
    memory
      type: ROM
      size: 0x300000
      content: Program
    memory
      type: RAM
      size: 0x10000
      content: Save

game
  sha256:   efae37be832d0ea1490784d57bef00761a8bf0b5bcef9c23f558e063441c3876
  label:    スターオーシャン
  name:     Star Ocean
  region:   SHVC-ARFJ-JPN
  revision: SHVC-ARFJ-0
  board:    SHVC-LN3B-01
    memory
      type: ROM
      size: 0x600000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   e722d20b9c264f81bbdb37d77338c5767f6b549af489486e75a648a0a65d5bfc
  label:    ストーンプロテクターズ
  name:     Stone Protectors
  region:   SHVC-ASOJ-JPN
  revision: SHVC-ASOJ-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   f15731675e22dbf3882b777b2d8cd541a637dfdf5d8880c83903cf1e0b64590e
  label:    ストリートファイターZero 2
  name:     Street Fighter Zero 2
  region:   SHVC-AUZJ-JPN
  revision: SHVC-AUZJ-0
  board:    SHVC-1N0N-10
    memory
      type: ROM
      size: 0x400000
      content: Program

game
  sha256:   edacb453da14f825f05d1134d6035f4bf034e55f7cfb97c70c4ee107eabc7342
  label:    スーファミターボ
  name:     Sufami Turbo
  region:   SHVC-A9PJ-JPN
  revision: SHVC-A9PJ-0
  board:    BANDAI-PT-923
    memory
      type: ROM
      size: 0x40000
      content: Program

game
  sha256:   e57aa265b2fbfb7ee7f5488a3df06ae771db202d59ebbd13df8fc2db80a856f3
  label:    スーパー・バック・トゥ・ザ・フューチャーII
  name:     Super Back to the Future - Part II
  region:   SHVC-B2
  revision: SHVC-B2-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   442397be57b3740ca236cfb37633b95f88a2c80dafc94b56a805229793563ce1
  label:    スーパーボンバーマン2 体験版
  name:     Super Bomberman 2 - Taikenban
  region:   SHVC-9B
  revision: SHVC-9B-0
  board:    SHVC-1J0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   5f8e6894f71c62bc5e70485715dbd2e2d8f3c0383ec54211dc5fe180098d0e3f
  label:    スーパーボンバーマン5 コロコロコミック
  name:     Super Bomberman 5 - CoroCoro Comic
  region:   SHVC-AK8J-JPN
  revision: SHVC-AK8J-0
  board:    SHVC-1J3M-20
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   f73238b97807fc37911d9e94ad7b671be2746baf11200974e12aeb089b7f3c35
  label:    スーパーフォーメーションサッカー95 デッラセリエA ザクア
  name:     Super Formation Soccer '95 - della Serie A - Xaqua
  region:   SHVC-ADEJ-JPN
  revision: SHVC-ADEJ-0
  board:    SHVC-2J3M-20
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   4d7fc331a811b8dc630b469262fd6f45e289243cef83101f32038158967d1b28
  label:    スーパーゲームボーイ
  name:     Super Game Boy
  region:   SHVC-SGB
  revision: SYS-SGB-2
  board:    SGB-R-10
    memory
      type: ROM
      size: 0x40000
      content: Program
    memory
      type: ROM
      size: 0x100
      content: Boot
      manufacturer: Nintendo
      architecture: SM83
      identifier: SGB1

game
  sha256:   e1db895a9da7ce992941c1238f711437a9110c2793083bb04e0b4d49b7915916
  label:    スーパーゲームボーイ2
  name:     Super Game Boy 2
  region:   SHVC-SGB2-JPN
  revision: SYS-SGB2-10
  board:    SHVC-SGB2-01
    memory
      type: ROM
      size: 0x80000
      content: Program
    memory
      type: ROM
      size: 0x100
      content: Boot
      manufacturer: Nintendo
      architecture: SM83
      identifier: SGB2
    oscillator
      frequency: 20971520

game
  sha256:   f56c083e54bc94efdc46c5224b3ceebc56c8ba7a0c38ee8620b0f73cd1317677
  label:    スーパー麻雀大会
  name:     Super Mahjong Taikai
  region:   SHVC-IQ
  revision: SHVC-IQ-0
  board:    SHVC-1A3B-13
    memory
      type: ROM
      size: 0x80000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   96c0cb3c670c36e068c36b38d1edae145f24f60159537da4988bf956eee58d59
  label:    スーパー麻雀大会
  name:     Super Mahjong Taikai
  region:   SHVC-IQ
  revision: SHVC-IQ-1
  board:    SHVC-1A3B-13
    memory
      type: ROM
      size: 0x80000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   b3204162def67b0dc40097344074e9b660ed296e5b5e22e778f373f0b985645b
  label:    スーパー麻雀大会
  name:     Super Mahjong Taikai
  region:   SHVC-IQ
  revision: SHVC-IQ-2
  board:    SHVC-1A3B-13
    memory
      type: ROM
      size: 0x80000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   15d1187d17fa10c77152f691197d33674e64c33a1f8ceb37e8570588be507b89
  label:    スーパー麻雀大会
  name:     Super Mahjong Taikai
  region:   SHVC-IQ
  revision: SHVC-IQ-3
  board:    SHVC-1A3B-13
    memory
      type: ROM
      size: 0x80000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   68def3754ee32d6bce8f7e346e4d46dc2861c2c590835f8151c291c78f37b21b
  label:    スーパー桃太郎電鉄DX JR西日本Presents
  name:     Super Momotarou Dentetsu DX - JR Nishi Nihon Presents
  region:   SHVC-ANWJ-JPN
  revision: SHVC-ANWJ-0
  board:    SHVC-1J3M-20
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   3972e52c1a6641842160ed70bac59c51d0d68a234deb8b2ad3b1e5daea38446e
  label:    す～ぱ～忍者くん
  name:     Super Ninja-kun
  region:   SHVC-8Q
  revision: SHVC-8Q-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   ecd462c64516169cc83dd266af354fe676fcf53811863a361d78cc918619da0d
  label:    スーパー三国志II 復刻版
  name:     Super Sangokushi II
  region:   SHVC-XL
  revision: SHVC-XL-1
  board:    SHVC-1A5M-20
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save

game
  sha256:   fde83367c1caf6edfb41a0c609bacc90084e9808b32ba52b13d204eb59535ab5
  label:    スーパースコープ6
  name:     Super Scope 6
  region:   SHVC-LR
  revision: SHVC-LR-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   8dda3b0888a32005041f2feb9be4e14807d40291f951a4612461cf41dac9cb78
  label:    Super Tetris 2 + Bombliss
  name:     Super Tetris 2 + Bombliss
  region:   SHVC-T2
  revision: SHVC-T2-0
  board:    SHVC-1A1B-06
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   a72b6f6722decc1c9e3f979f3d637794d3016b525822dd97f2aeba88696959aa
  label:    スーパータリカン
  name:     Super Turrican
  region:   SHVC-T9
  revision: SHVC-T9-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   db04fad1cfb1b8a58cb05ce62ae9e66532923699b54499344869cf8143f06098
  label:    すってはっくん
  name:     Sutte Hakkun
  region:   SHVC-BSHJ-JPN
  revision: SHVC-BSHJ-0
  board:    SHVC-1J3M-20
    memory
      type: ROM
      size: 0x2e0000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   77b2d5450ce3c87185f913c2584673530c13dfbe8cc433b1e9fe5e9a653bf7d5
  label:    テイルズオブファンタジア
  name:     Tales of Phantasia
  region:   SHVC-ATVJ-JPN
  revision: SHVC-ATVJ-0
  board:    SHVC-LJ3M-01
    memory
      type: ROM
      size: 0x600000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   c9002e77bcc656e033c35e2574ee6067c4c0d070943359a850806c123a558949
  label:    戦え原始人3 主役はやっぱりジョーアンドマック
  name:     Tatakae Genshijin 3 - Shuyaku wa Yappari Joe & Mac
  region:   SHVC-J3
  revision: SHVC-J3-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   b97f1bb3c7258d500660757a57c1c7e1d90a719dfcf7b3de5e8a0d06f7f8e814
  label:    ティーンエージ ミュータント ニンジャ タートルズ／タートルズ イン タイム
  name:     Teenage Mutant Ninja Turtles - Turtles in Time
  region:   SHVC-TM
  revision: SHVC-TM-0
  board:    SHVC-1A0N-02
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   d844223275a9f8d428c73d5540c6e9ddd2781c53ba8a8d733bde5835ebfa4e25
  label:    UFO仮面ヤキソバン ケトラーの黒い陰謀 景品版
  name:     UFO Kamen Yakisoban - Kettler no Kuroi Inbou - Keihinban
  region:   SHVC-Y7
  revision: SHVC-Y7-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   b66da2a23f249e525b1dd444596a3f10559cb3c30fa3c0bca83ed8f4405fcfcf
  label:    Undake 30 鮫亀大作戦 マリオバージョン
  name:     Undake 30 - Same Game Daisakusen - Mario Version
  region:   SHVC-ANZJ-JPN
  revision: SHVC-ANZJ-0
  board:    SHVC-1J3M-20
    memory
      type: ROM
      size: 0x60000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   cb8073cf95eace56ba4324a2106164fa540900c2de083aff490c4afe91ae95f7
  label:    アンダーカバーコップス
  name:     Undercover Cops
  region:   SHVC-AUCJ-JPN
  revision: SHVC-AUCJ-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   6b37c3bd79db2553bf71b79efbb131d96c40462cffac54c40dbc783f1ef44428
  label:    ワイルドガンズ
  name:     Wild Guns
  region:   SHVC-4W
  revision: SHVC-4W-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   491b5d20c0e00eaa7ae89e9f4cde044408258aa31042bb3ed16dff082e01a7a7
  label:    レッキングクルー'98
  name:     Wrecking Crew '98
  region:   SHVC-BWCJ-JPN
  revision: SHVC-BWCJ-0
  board:    SHVC-1J3M-20
    memory
      type: ROM
      size: 0x1e0000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   c06e3c14a2f73180a7a6ff50ee76790c4542d58e8994bb3fae06412f7303d089
  label:    やまねこバブジーの大冒険
  name:     Yamaneko Bubsy no Daibouken
  region:   SHVC-UY
  revision: SHVC-UY-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   2b2fe61ac7a79c3cfaa0bc16f1b1f4da544fcc37cfdf3c18879d31b8b9f87941
  label:    ヨッシーのクッキー クルッポンオーブンでクッキー
  name:     Yoshi no Cookie - Kuruppon Oven de Cookie
  region:   SHVC-YO
  revision: SHVC-YO-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   64a124461cc12dd073191563729f559d0cf8911890ca7ede051e32024f6d9695
  label:    妖怪バスター ルカの大冒険
  name:     Youkai Buster - Ruka no Daibouken
  region:   SHVC-AYOJ-JPN
  revision: SHVC-AYOJ-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

//Super Famicom (ROC)

//database
//  revision: 2018-04-14

game
  sha256:   1cf13dac329f83a7fe5347dcc20f92c3a09b3eab1511dd461f9cec90e9258403
  label:    三国志III
  name:     Sanguozhi III
  region:   SNSN-S3-ROC
  revision: SCHN-S3-0
  board:    SHVC-1J5M-20
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save

//Super Nintendo (AUS)

//database
//  revision: 2018-04-14

game
  sha256:   7dbaebb1007984610623cb0b571f0e7167d73d89274598bfffc845cfb2de4aac
  label:    Lufia
  name:     Lufia
  region:   SNSP-ANIP-AUS
  revision: SPAL-ANIP-0
  board:    SHVC-1A3M-30
    memory
      type: ROM
      size: 0x280000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   0483780080d10bae49f62c3e4bf6954f891a29c0b1356b3a073cc894784593b8
  label:    Space Invaders
  name:     Space Invaders
  region:   SNSP-IC-AUS
  revision: SPAL-IC-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x40000
      content: Program

game
  sha256:   a1daaaaacf723ddfa4fcb406d0218a045d8f8a78674b6449a2322a0306e0af9f
  label:    Super Chase HQ
  name:     Super Chase HQ
  region:   SNSP-QT-AUS
  revision: SPAL-QT-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

//Super Nintendo (BRA)

//database
//  revision: 2018-04-14

game
  sha256:   1edcceab07d1544dcbd0fd681148b0fbefeea58b7077136fa0c3011973bf34df
  label:    FIFA: A Caminho da Copa '98
  name:     FIFA - A Caminho da Copa '98
  region:   SNS-A8FB-BRA
  revision: SNS-A8FB-0
  board:    EA-1A3M-30
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save
  note: Serial# is from ROM data; it is not present on cartridge or ROM chip

//Super Nintendo (CAN)

//database
//  revision: 2018-04-14

game
  sha256:   dd499445275fca6692c0487a8bd70a23f6c8e78e766df0e3c58fbbe53f8adb06
  label:    The Legend of Zelda: A Link to the Past
  name:     Legend of Zelda, The - A Link to the Past
  region:   SNS-ZF-CAN
  revision: SNS-ZF-0
  board:    SHVC-1A3B-13
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

//Super Nintendo (ESP)

//database
//  revision: 2018-09-21

game
  sha256:   bd5e7a6bc08f64d39c54204b82c6c156f144c03e13c890128588c5faa560659c
  label:    The Addams Family: Pugsley's Scavenger Hunt
  name:     Addams Family, The - Pugsley's Scavenger Hunt
  region:   SNSP-AH-ESP
  revision: SPAL-AH-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   7a75021fb390d04645b654a3dbd986c82cee1dbf34018e7ff7bf4b6ea125fa35
  label:    Bram Stoker's Dracula
  name:     Bram Stoker's Dracula
  region:   SNSP-5D-ESP
  revision: SPAL-5D-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   6582c8f0ebec73d9dee61c1ff95b7b8e7c753a30c7bdd5dab560025dc531a43e
  label:    Cool World
  name:     Cool World
  region:   SNSP-CD-ESP
  revision: SPAL-CD-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   4a4803056afb251b06a2e9fa68d8062b6999156535bb66ff9fb069de180f32c1
  label:    Jack Nicklaus Golf
  name:     Jack Nicklaus Golf
  region:   SNSP-JN-ESP
  revision: SPAL-JN-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   929e15c8439b3beea249730e598e72cb192a3a70af0624ab7f91300f8f786a13
  label:    Jurassic Park
  name:     Jurassic Park
  region:   SNSP-J8-ESP
  revision: SESP-J8-0
  board:    SHVC-2A0N-01
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   bd7e98db82d6b52307be1f3e1fd171e1e7204dc1f8810a95ee2cc64757087e4a
  label:    The Lost Vikings
  name:     Lost Vikings, The
  region:   SNSP-LV-ESP
  revision: SESP-LV-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   6eecabd46305ac95d9cf3a17e1392c24a1b68a7a313173ef0c5b5a3a24cf3353
  label:    Lufia
  name:     Lufia
  region:   SNSP-ANIS-ESP
  revision: SPAL-ANIS-0
  board:    SHVC-1A3M-30
    memory
      type: ROM
      size: 0x300000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   d70bc7916ed5132c3b0053f2adbb5004d78ccb986210c9440fedf642cac68554
  label:    MechWarrior
  name:     MechWarrior
  region:   SNSP-WM-ESP
  revision: SESP-WM-0
  board:    SHVC-1A1M-10
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   d2233d6310522bbf183b6ca9bbe3e2afaf24de0cc4304bff6d0d547d678aed6f
  label:    Sonic Blast Man
  name:     Sonic Blast Man
  region:   SNSP-SK-ESP
  revision: SPAL-SK-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   a20d346da18ddabf70dc43f5095c4189c4a646ca8e6d4ed6c68c20e380f50332
  label:    Super Battletank 2
  name:     Super Battletank 2
  region:   SNSP-2X-ESP
  revision: SESP-2X-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   9eaf1c46d8a068c910d66f582e23b1155882ddfa4b9fd0813819fc5c008167e2
  label:    Super James Pond
  name:     Super James Pond
  region:   SNSP-J5-ESP
  revision: SPAL-J5-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   bd9cbb4c115716ec0f5d2d438bb812d02d496c41b7677a1d9520ff454254247b
  label:    Syvalion
  name:     Syvalion
  region:   SNSP-SY-ESP
  revision: SPAL-SY-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   7f731f4bb620e682132660da39641dda5762211dca4732f8192dd2411211b822
  label:    Terranigma
  name:     Terranigma
  region:   SNSP-AQTS-ESP
  revision: SPAL-AQTS-0
  board:    SHVC-1J3M-20
    memory
      type: ROM
      size: 0x400000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   981128c93f0753dec7af29ec084f13e704cc5d02414be55bb477fc4b2fef5e58
  label:    Tiny Toon Adventures: Buster Busts Loose!
  name:     Tiny Toon Adventures - Buster Busts Loose!
  region:   SNSP-TA-ESP
  revision: SESP-TA-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   ce2445ecd0a43f6025dc80857d91dae7c46d33f7821bf98232c2894ca1959da2
  label:    Turn and Burn: No-Fly Zone
  name:     Turn and Burn - No-Fly Zone
  region:   SNSP-ZN-ESP
  revision: SESP-ZN-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   9ed876a632aa699047e9efba8a64ab57abc55086a0aab6b5fa67d87ea4647f3f
  label:    Whirlo
  name:     Whirlo
  region:   SNSP-SH-ESP
  revision: SPAL-SH-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   ffcdce24171d9dc225a8a8b52e7d24a7832873f85135767359952537a8b9f8f1
  label:    World Cup USA '94
  name:     World Cup USA '94
  region:   SNSP-U4-ESP
  revision: SPAL-U4-0
  board:    SHVC-2J3M-11
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   d897f80465d02a92f17e1eecb727b99211ec2fb9c58cbabfa76b35c554096ea6
  label:    Young Merlin
  name:     Young Merlin
  region:   SNSP-Y6-ESP
  revision: SPAL-Y6-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

//Super Nintendo (EUR)

//database
//  revision: 2018-09-21

game
  sha256:   ec3e81d628a293514e303b44e3b1ac03461ddd1da32764b10b7fab1e507602df
  label:    Aaahh!!! Real Monsters
  name:     Aaahh!!! Real Monsters
  region:   SNSP-ANNP-EUR
  revision: SPAL-ANNP-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   a0baba78c9cad02957b80ed74fc8d09fac3c77e131e47333ef42ba471dc61228
  label:    The Adventures of Batman & Robin
  name:     Adventures of Batman & Robin, The
  region:   SNSP-ABTP-EUR
  revision: SPAL-ABTP-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   e0c065aecfb6167cdb6dee8368b5f36d52fe55131ed4f6adbb3b2c62a39aafca
  label:    The Adventures of Mighty Max
  name:     Adventures of Mighty Max, The
  region:   SNSP-AMOP-EUR
  revision: SPAL-AMOP-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   ffd634dbfa9ad88a81cfc59adcc889c15e03730536c171d358bf58b37c6bca6a
  label:    Air Cavalry
  name:     Air Cavalry
  region:   SNSP-ACCP-EUR
  revision: SPAL-ACCP-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   65feac0ff8bf834bdb3f1eade09102496e1bd021a261ca05fc3a75983c357c84
  label:    Alien 3
  name:     Alien 3
  region:   SNSP-A3-EUR
  revision: SPAL-A3-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   79ff74b758bb5df40d9fb6f830e9370de799c7a1423c482b2cc74eee78c55127
  label:    Animaniacs
  name:     Animaniacs
  region:   SNSP-ANCP-EUR
  revision: SPAL-ANCP-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   41084f83e269d46b9d589f11c802a15e84fede57d604c7986053f2858f757adc
  label:    Archer Maclean's Super Dropzone
  name:     Archer Maclean's Super Dropzone
  region:   SNSP-ASDP-EUR
  revision: SPAL-ASDP-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   ea4240989114fd2c6dbbf2bfcafb2047ab482ebc4aa276f30f3dc7b551197808
  label:    Ardy Lightfoot
  name:     Ardy Lightfoot
  region:   SNSP-A9-EUR
  revision: SPAL-A9-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   f0cfaab9a4be5b2bac0cb3dafea14cea4cf8d7cbfa562323ab3026466985c9e1
  label:    Bass Masters Classic: Pro Edition
  name:     Bass Masters Classic - Pro Edition
  region:   SNSP-A9BP-EUR
  revision: SPAL-A9BP-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   a10f7406d76a85e27ae340ed2dd2379897321ed388b439e247b3437fa07806cb
  label:    Batman Forever
  name:     Batman Forever
  region:   SNSP-A3BP-EUR
  revision: SPAL-A3BP-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x300000
      content: Program

game
  sha256:   4d347d60795008852ec11bc8e0256a3d1f159bf48c130b9798cb2961e560f319
  label:    Beavis and Butt-Head
  name:     Beavis and Butt-Head
  region:   SNSP-ABUP-EUR
  revision: SPAL-ABUP-0
  board:    SHVC-2A0N-20
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   a73c8864743dd64a61d756ebe001a1244d6ae387621a46f9da4421d061c6b7ac
  label:    Boogerman: A Pick and Flick Adventure
  name:     Boogerman - A Pick and Flick Adventure
  region:   SNSP-AB4P-EUR
  revision: SPAL-AB4P-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x300000
      content: Program

game
  sha256:   597894334926c326e332ecdea0d4ad61430ab6af03830a9beeb2c007c280c843
  label:    Breath of Fire II
  name:     Breath of Fire II
  region:   SNSP-AF2P-EUR
  revision: SPAL-AF2P-0
  board:    SHVC-1J3M-20
    memory
      type: ROM
      size: 0x300000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   a057383e41bd5735b1c554c555b89fe0ff9b7eb7e9f9d46dbefbdd749c8d2181
  label:    Brutal: Paws of Fury
  name:     Brutal - Paws of Fury
  region:   SNSP-ABLP-EUR
  revision: SPAL-ABLP-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   be86b8b9c390f1a85384117ce19170567e1234ddb08b538e1361af2feee63869
  label:    Bubsy II
  name:     Bubsy II
  region:   SNSP-ABBP-EUR
  revision: SPAL-ABBP-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   3545306505c463c627bc4ced91ff4413481c945a658860ddc1f6e8e7b4cc6144
  label:    Cannon Fodder
  name:     Cannon Fodder
  region:   SNSP-ACNP-EUR
  revision: SPAL-ACNP-0
  board:    SHVC-2A0N-11
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   04a347e7d600c8aff435e769f72273ac1ae8160838905f325f03ef4c5aa93bbe
  label:    Captain Commando
  name:     Captain Commando
  region:   SNSP-QM-EUR
  revision: SPAL-QM-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   0f37da5beb0beb5e8f8c34443bf0734575649f8222074e3394926c3b697589cc
  label:    Carrier Aces
  name:     Carrier Aces
  region:   SNSP-ACAP-EUR
  revision: SPAL-ACAP-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   ca99787148aac7053ae9d1a1b06c8a5f42f2bfdd70c2942282a9dd0c1c0fda4f
  label:    Castlevania: Vampire's Kiss
  name:     Castlevania - Vampire's Kiss
  region:   SNSP-ADZP-EUR
  revision: SPAL-ADZP-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   e1e2cc6b7ef58b512bb8aa6848dd67fce9e5d092ea67e6b4f31f156b060cc2b1
  label:    Clay Fighter 2: Judgment Clay
  name:     Clay Fighter 2 - Judgment Clay
  region:   SNSP-ACZP-EUR
  revision: SPAL-ACZP-0
  board:    SHVC-BJ0N-01
    memory
      type: ROM
      size: 0x300000
      content: Program

game
  sha256:   b9d6cf21e7a280b83fadcf513029158be6f4dcdc51f73b699b7c215a5150be42
  label:    CutThroat Island
  name:     CutThroat Island
  region:   SNSP-AC8P-EUR
  revision: SPAL-AC8P-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   3a158e49478bd9a25b487a347c25f401cd0ed1cd1ccf72d8010752139a2143dc
  label:    Demolition Man
  name:     Demolition Man
  region:   SNSP-AD6P-EUR
  revision: SPAL-AD6P-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   f6afe40ced7033c845df84b895230fd26ea0b48e6a58d6b6e18beee9b594ad6e
  label:    Demon's Crest
  name:     Demon's Crest
  region:   SNSP-3Z-EUR
  revision: SPAL-3Z-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   a2dc5ffc82e8d055d030c3f021d8df3ae8b08571c8301cdd1d7652248d6f9b55
  label:    Dino Dini's Soccer
  name:     Dino Dini's Soccer
  region:   SNSP-ADSP-EUR
  revision: SPAL-ADSP-0
  board:    SHVC-1A3M-21
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   574c61ab1a670a79b8dc69445b8f55aa5b4caa95ed0e1504fae8a1e3d336e7f1
  label:    Donkey Kong Country 3: Dixie Kong's Double Trouble
  name:     Donkey Kong Country 3 - Dixie Kong's Double Trouble
  region:   SNSP-A3CP-EUR
  revision: SPAL-A3CP-0
  board:    SHVC-1J1M-20
    memory
      type: ROM
      size: 0x400000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   753467f4e343a563a1e4f5da43b731968a47cf3fc035f98575134e680a596f27
  label:    Dragon: The Bruce Lee Story
  name:     Dragon - The Bruce Lee Story
  region:   SNSP-4N-EUR
  revision: SPAL-4N-0
  board:    SHVC-1J0N-10
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   89dddd495d29db4542d45bf4a89d387bf2465eb8d2bd5b300c5397d8b24dc4af
  label:    Dragon Ball Z: Hyper Dimension
  name:     Dragon Ball Z - Hyper Dimension
  region:   SNSP-AZIF-EUR
  revision: SPAL-AZIF-0
  board:    SNSP-1L0N3S-01
    memory
      type: ROM
      size: 0x300000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save
      volatile
    memory
      type: RAM
      size: 0x800
      content: Internal
      volatile

game
  sha256:   4a96f3c0a7694a1d8235e2bf03b583ed2e99488d1dc0e26691db003fd23191f7
  label:    Earthworm Jim
  name:     Earthworm Jim
  region:   SNSP-AEJP-EUR
  revision: SPAL-AEJP-0
  board:    SHVC-BJ0N-01
    memory
      type: ROM
      size: 0x300000
      content: Program

game
  sha256:   df2b34467581f8ffdad24f59119d54a03875f355d33ab552459b60f8f0de3e78
  label:    Earthworm Jim 2
  name:     Earthworm Jim 2
  region:   SNSP-A2EP-EUR
  revision: SPAL-A2EP-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x300000
      content: Program

game
  sha256:   f4cabd80dd20b076ff90bf1f394b5dff5800900fa1fe178a42b6af967fd80c25
  label:    Fatal Fury 2
  name:     Fatal Fury 2
  region:   SNSP-DJ-EUR
  revision: SPAL-DJ-0
  board:    SHVC-BJ0N-01
    memory
      type: ROM
      size: 0x280000
      content: Program

game
  sha256:   a12391775fa9770f85f383ffaec6441a686c33b2e1800de7c01a79a0b7c93154
  label:    Fatal Fury Special
  name:     Fatal Fury Special
  region:   SNSP-3R-EUR
  revision: SPAL-3R-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x300000
      content: Program

game
  sha256:   20af141280d75c29a0981c6a4decfa1835c3613a47636a4ae0967948a7878f7c
  label:    Fever Pitch Soccer
  name:     Fever Pitch Soccer
  region:   SNSP-AVSP-EUR
  revision: SPAL-AVSP-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   ea769b616ad5d6f7b0a5e42951c31096191800347dc9fd6c8c9845f2afa95f5b
  label:    FIFA '97
  name:     FIFA '97
  region:   SNSP-A7IP-EUR
  revision: SPAL-A7IP-0
  board:    EA-1A3M-30
    memory
      type: ROM
      size: 0x1e0000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   14e8f7963bd71a3d8792952ae0c0def733178ac720417b954ea5cb12cc76dece
  label:    Final Fight 3
  name:     Final Fight 3
  region:   SNSP-AFZP-EUR
  revision: SPAL-AFZP-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x300000
      content: Program

game
  sha256:   a0106f9cff7abbf25e081e2531f6d4b4aedf6f0dc8d155a66506817bff267d12
  label:    The Firemen
  name:     Firemen, The
  region:   SNSP-AFMP-EUR
  revision: SPAL-AFMP-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   21d79cd5382ad5503fdee1c44416a7a425904cebe37bb531d508ef62aa4f2ed0
  label:    Frank Thomas' Big Hurt Baseball
  name:     Frank Thomas' Big Hurt Baseball
  region:   SNSP-AFKP-EUR
  revision: SPAL-AFKP-0
  board:    SHVC-1A3M-30
    memory
      type: ROM
      size: 0x300000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   6a6a95528e4c8cfdf46cade2a354f3e28f63b55bba002e10af0f60227ec4c833
  label:    Frantic Flea
  name:     Frantic Flea
  region:   SNSP-AF8P-EUR
  revision: SPAL-AF8P-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   0d7a1649915d45b4c6e0752ea06ad353c2b1a590370912c18deeb42986821624
  label:    Ghoul Patrol
  name:     Ghoul Patrol
  region:   SNSP-AGJP-EUR
  revision: SPAL-AGJP-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   ee4ab0df49db4024b5da62d04c18c8c866830b54d286dec69440af7285ec2854
  label:    Hagane
  name:     Hagane
  region:   SNSP-AHGP-EUR
  revision: SPAL-AHGP-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   d98487781da9307309eec8830c6a28c907b0f1848e351df7f5b6d005c653b4ac
  label:    Hurricanes
  name:     Hurricanes
  region:   SNSP-AHUP-EUR
  revision: SPAL-AHUP-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   21e8632a6da085cae5d4bcf7334c63fbda0028c2a37c0b0c3041ab59f307ebd3
  label:    Incantation
  name:     Incantation
  region:   SNSP-AIYP-EUR
  revision: SPAL-AIYP-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   09d937a73595df67c473f1a4beba2f302570c1434d8093051d027ec1e440b984
  label:    Indiana Jones' Greatest Adventures
  name:     Indiana Jones' Greatest Adventures
  region:   SNSP-AIJP-EUR
  revision: SPAL-AIJP-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   6fe482f91a59a71992e14014a0a4e23fb866cf4e870c10d57c81b0c20ae6688e
  label:    International Superstar Soccer
  name:     International Superstar Soccer
  region:   SNSP-3U-EUR
  revision: SPAL-3U-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   686f2507e9969fd7617fc544c139e124668294d102e79a0eb34478c8deb75271
  label:    Izzy's Quest for the Olympic Rings
  name:     Izzy's Quest for the Olympic Rings
  region:   SNSP-AIZP-EUR
  revision: SPAL-AIZP-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   02cb423199be3368fc2b40148f83b7a48900394983e04d43f94bb7d76ce44e94
  label:    Judge Dredd
  name:     Judge Dredd
  region:   SNSP-AJDP-EUR
  revision: SPAL-AJDP-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   2516843fa405ab1aa1f242b57f19977519aefb68599474d2c7065aaef88ecb88
  label:    Jungle Strike
  name:     Jungle Strike
  region:   SNSP-AJGP-EUR
  revision: SPAL-AJGP-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   9b1dbcac063b524eef533e78cf7051e3f566a49d5ac13d23474dc6afb293d6be
  label:    Jurassic Park Part 2: The Chaos Continues
  name:     Jurassic Park Part 2 - The Chaos Continues
  region:   SNSP-A2JP-EUR
  revision: SPAL-A2JP-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   07386ef7dfcc70a67beb01fa7e2300249914b2ce0b010a74cbfbf0714c32fcf1
  label:    Justice League Task Force
  name:     Justice League Task Force
  region:   SNSP-AJLP-EUR
  revision: SPAL-AJLP-0
  board:    SHVC-BA0N-01
    memory
      type: ROM
      size: 0x280000
      content: Program

game
  sha256:   51364bc86ba8f00f0eeb2dcd59c68b42f94595af5ce272e914dedb5fc4ffa191
  label:    Lemmings 2: The Tribes
  name:     Lemmings 2 - The Tribes
  region:   SNSP-L2-EUR
  revision: SPAL-L2-0
  board:    SHVC-1A1M-11
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   bf0d2739c2bd1aeba9cc979928e0702341cb290023dbc39bd25f18c1412fe674
  label:    Marko's Magic Football
  name:     Marko's Magic Football
  region:   SNSP-AMRP-EUR
  revision: SPAL-AMRP-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   4f83fc7c1fa4fae99568ae8e049a45e6e65176761fe3ac74315bee8eff846fd4
  label:    Marvel Super Heroes in War of the Gems
  name:     Marvel Super Heroes in War of the Gems
  region:   SNSP-AHZP-EUR
  revision: SPAL-AHZP-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   9da7274457995b39ae7b00387c1eaab92f1fdb0beac55372726c3a3af1cb8f7e
  label:    MechWarrior 3050
  name:     MechWarrior 3050
  region:   SNSP-A35P-EUR
  revision: SPAL-A35P-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   81f90a36c07221546c441d67689d951864f071f3218aa96994c0c54e93998a67
  label:    Mega Man 7
  name:     Mega Man 7
  region:   SNSP-A7RP-EUR
  revision: SPAL-A7RP-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   ae6fb12b5077eaa9c13c6124368343bfef076bc1e42f53f442973576266255c4
  label:    Mega Man X2
  name:     Mega Man X2
  region:   SNSP-ARXP-EUR
  revision: SPAL-ARXP-0
  board:    SHVC-2DC0N-01
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: ROM
      size: 0xc00
      content: Data
      manufacturer: Hitachi
      architecture: HG51BS169
      identifier: Cx4
    memory
      type: RAM
      size: 0xc00
      content: Data
      manufacturer: Hitachi
      architecture: HG51BS169
      identifier: Cx4
      volatile
    oscillator
      frequency: 20000000

game
  sha256:   14cd1e5d7a810426c566c1c4eff16398cace6e7a4c0d7f048fdb1f4c22c1c135
  label:    Mega Man X3
  name:     Mega Man X3
  region:   SNSP-AR3P-EUR
  revision: SPAL-AR3P-0
  board:    SHVC-1DC0N-01
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: ROM
      size: 0xc00
      content: Data
      manufacturer: Hitachi
      architecture: HG51BS169
      identifier: Cx4
    memory
      type: RAM
      size: 0xc00
      content: Data
      manufacturer: Hitachi
      architecture: HG51BS169
      identifier: Cx4
      volatile
    oscillator
      frequency: 20000000

game
  sha256:   02a85c9e783add4e3d44f8f4718c014743f252585fedd9ae5583458237122b35
  label:    Mohawk & Headphone Jack
  name:     Mohawk & Headphone Jack
  region:   SNSP-AJYP-EUR
  revision: SPAL-AJYP-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   b7fa4b79590ecdef2d0303b5cbccf1e1a3af31fca289652acbdb3d5381137f24
  label:    Mortal Kombat 3
  name:     Mortal Kombat 3
  region:   SNSP-A3MP-EUR
  revision: SPAL-A3MP-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x400000
      content: Program

game
  sha256:   964f8391806bc8d674f24aa2f201b20982d2f2f3e3efb2f730c4a34a289c3007
  label:    Mortal Kombat II
  name:     Mortal Kombat II
  region:   SNSP-28-EUR
  revision: SPAL-28-1
  board:    SHVC-BJ0N-01
    memory
      type: ROM
      size: 0x300000
      content: Program

game
  sha256:   bfec7d5afdf0d41cfce7f4e73dc20d91981db0edc2a0f41c5737d861efd4ab2a
  label:    Mr. Do!
  name:     Mr. Do!
  region:   SNSP-AUNP-EUR
  revision: SPAL-AUNP-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x40000
      content: Program

game
  sha256:   65377b1f6dbf1b3258c0554fe54ab0fbe59d6c3f07d3cad1c69ff692e0872b9a
  label:    NBA Jam: Tournament Edition
  name:     NBA Jam - Tournament Edition
  region:   SNSP-AJTP-EUR
  revision: SPAL-AJTP-0
  board:    SHVC-BA1M-01
    memory
      type: ROM
      size: 0x300000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   aa3f0608fa8723a21a4a5121f04098c53a76b5188f4dc30fcc26db9232c734d8
  label:    NHL '96
  name:     NHL '96
  region:   SNSP-A6HP-EUR
  revision: SPAL-A6HP-0
  board:    SHVC-1J3M-20
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   25d04bba3f3b12c63ae7ee4450e1d924df7aff077515d634721e22e6420c09b9
  label:    Ninja Warriors: The New Generation
  name:     Ninja Warriors - The New Generation
  region:   SNSP-NI-EUR
  revision: SPAL-NI-0
  board:    SHVC-2A0N-20
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   d9860f4f9b5e71290c5419b88c49b545f947a35cfe0549c2f32e54f05bc55815
  label:    Olympic Summer Games
  name:     Olympic Summer Games
  region:   SNSP-AO9P-EUR
  revision: SPAL-AO9P-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   2ec71aca4efc3791b6b3e65956df3eafd2a46e223d5ea71aead07d30ca48b6c9
  label:    Operation Starfi5h
  name:     Operation Starfi5h
  region:   SNSP-AOSP-EUR
  revision: SPAL-AOSP-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   908b1b18403a1330e1af5844f8c9d11a66279ff107cf14b56e6be849fbd4a7f9
  label:    Phantom 2040
  name:     Phantom 2040
  region:   SNSP-A24P-EUR
  revision: SPAL-A24P-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   166fe888ac03f88cce5759c1008021082bd81254aab8fea8203cdb13080709d7
  label:    Pinball Fantasies
  name:     Pinball Fantasies
  region:   SNSP-APFP-EUR
  revision: SPAL-APFP-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   1744a87d4817be07948e1b37b59c4de49c85e6aec2cf49879c7b9649d1b1fc90
  label:    Primal Rage
  name:     Primal Rage
  region:   SNSP-AR9P-EUR
  revision: SPAL-AR9P-0
  board:    SHVC-BJ0N-20
    memory
      type: ROM
      size: 0x300000
      content: Program

game
  sha256:   00e2c72fd82d78c5bb6f144101df6145d0a7f68dd77632f49969f13c5623c59a
  label:    Realm
  name:     Realm
  region:   SNSP-ARQP-EUR
  revision: SPAL-ARQP-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   862a53905b9e2ed33bae8e01e755ab6ca9fe2f181567510cb0840ed9a19f20d8
  label:    Rise of the Robots
  name:     Rise of the Robots
  region:   SNSP-AROP-EUR
  revision: SPAL-AROP-0
  board:    SHVC-BJ0N-01
    memory
      type: ROM
      size: 0x400000
      content: Program

game
  sha256:   2d5e3091ce4912e2325dedf99b439c5b4cb6ba6899e25bf8957b19eda942de88
  label:    RoboCop vs. The Terminator
  name:     RoboCop vs. The Terminator
  region:   SNSP-VR-EUR
  revision: SPAL-VR-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   09fdef0c959e7275563f65d8267d95a20104f054e9438e9ac739ef7f46120a2d
  label:    Smash Tennis
  name:     Smash Tennis
  region:   SNSP-JA-EUR
  revision: SPAL-JA-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   9638d4daa7bc63fd178b77c4487c3b080def5b808c0b7ccc959101f33e221b42
  label:    Spider-Man & Venom: Maximum Carnage
  name:     Spider-Man & Venom - Maximum Carnage
  region:   SNSP-AMCP-EUR
  revision: SPAL-AMCP-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   4001418ceb26f38efbea10b7bcc8233adc7bfcaa7e11dfb401d820f3254f06ef
  label:    Street Fighter Alpha 2
  name:     Street Fighter Alpha 2
  region:   SNSP-AUZP-EUR
  revision: SPAL-AUZP-0
  board:    SNSP-1N0N-01
    memory
      type: ROM
      size: 0x400000
      content: Program

game
  sha256:   4ef0053b294be64353df47e4f7d079cb97044e18263b55d46b6e4f9adda55766
  label:    Super B.C. Kid
  name:     Super BC Kid
  region:   SNSP-ZH-EUR
  revision: SPAL-ZH-0
  board:    SHVC-2J0N-20
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   640acb63dae038ad6f0ae65e103416f5a1f84d4a37ddaeeab5046122def774d5
  label:    Super Metroid
  name:     Super Metroid
  region:   SNSP-RI-EUR
  revision: SPAL-RI-0
  board:    SHVC-1A3M-30
    memory
      type: ROM
      size: 0x300000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   148f6bbf0578ff6ec62e6dacb9e3d266f6d7a427baa104e8ecd3cb2df64bca14
  label:    Super Soccer
  name:     Super Soccer
  region:   SNSP-FS-EUR
  revision: SPAL-FS-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   92946358ee4fbc5cb1df81a26ebd323c2f4e8cc76acd038e1191b8aa31ad1c24
  label:    Super Star Wars III: Return of the Jedi
  name:     Super Star Wars III - Return of the Jedi
  region:   SNSP-ARJP-EUR
  revision: SPAL-ARJP-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   a52b98da8f65bf2210b7d2e931548c672838fa7e44d852f2e3c6f3cd2ba950d6
  label:    Super Street Fighter II: The New Challengers
  name:     Super Street Fighter II - The New Challengers
  region:   SNSP-XW-EUR
  revision: SPAL-XW-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x400000
      content: Program

game
  sha256:   299984b4148ee4503d67cba8469c5023f7ecb204949bc70c3271cc56b117bb8e
  label:    Super Tennis
  name:     Super Tennis
  region:   SNSP-ST-EUR
  revision: SPAL-ST-1
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   93ba50d853e98e1ca227a2ca72389c0e3ac18d6b50c946b3f618c16c2d3edd38
  label:    Terranigma
  name:     Terranigma
  region:   SNSP-AQTP-EUR
  revision: SPAL-AQTP-0
  board:    SHVC-1J3M-20
    memory
      type: ROM
      size: 0x400000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   41648e6d73bd1b998f5c0737a4e61cd603e574ce4a3439e579d2b74b14390159
  label:    Tiny Toon Adventures: Wild & Wacky Sports
  name:     Tiny Toon Adventures - Wild & Wacky Sports
  region:   SNSP-5Z-EUR
  revision: SPAL-5Z-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   4ad736a9e1c7f34740afaa7777b8f1a31da4bb4a021e7ae341d1dafd74fa0acc
  label:    True Lies
  name:     True Lies
  region:   SNSP-ATLP-EUR
  revision: SPAL-ATLP-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   dbf11d4c77b9aa3416f687201d57d71a23bb8fb0b8fe5e9e8212db3fac036631
  label:    Turbo Toons
  name:     Turbo Toons
  region:   SNSP-ATTP-EUR
  revision: SPAL-ATTP-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   03540084486c21724819515d1cb967c658f7f08ec180855fa7c191ca13d8bef1
  label:    Ultimate Mortal Kombat 3
  name:     Ultimate Mortal Kombat 3
  region:   SNSP-A3ZP-EUR
  revision: SPAL-A3ZP-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x400000
      content: Program

game
  sha256:   fc8c611bc46c850515a933d4388e2123e20ab24a3a3d3c1ac95afdc76c82c3d4
  label:    Urban Strike
  name:     Urban Strike
  region:   SNSP-AUSP-EUR
  revision: SPAL-AUSP-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   1217ddf2fe475661a54f50e111864102faf854397ce5aceea4297204ebd6cbb6
  label:    Val d'isère Championship
  name:     Val d'isere Championship
  region:   SNSP-8Z-EUR
  revision: SPAL-8Z-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   150073c04e9c0e8b283a8eb237acea8fda3268c722f922ee42642009e17dc729
  label:    Vortex
  name:     Vortex
  region:   SNSP-4V-EUR
  revision: SPAL-4V-0
  board:    SHVC-1CA0N5S-01
    memory
      type: ROM
      size: 0x80000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save
      volatile
    oscillator
      frequency: 21440000

game
  sha256:   f47f1665d97d350dda8bf968543ed38b1daf63081d6f71e517867a5533ce4776
  label:    Wild Guns
  name:     Wild Guns
  region:   SNSP-4W-EUR
  revision: SPAL-4W-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   aceb9ac7cfd68e6740de47ad1a19be70d4efe3974caa4aa8ec50b0a0a6672e47
  label:    Winter Gold
  name:     Winter Gold
  region:   SNSP-AXSP-EUR
  revision: SPAL-AXSP-0
  board:    SHVC-1CB7B-01
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x10000
      content: Save
    oscillator
      frequency: 21440000

game
  sha256:   142b9b3f99811c2314a94d2c4b66aa9d434f5bdc9ccbb1574e3a6cbf2176b378
  label:    X-Kaliber 2097
  name:     X-Kaliber 2097
  region:   SNSP-X7-EUR
  revision: SPAL-X7-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

//Super Nintendo (FAH)

//database
//  revision: 2018-09-21

game
  sha256:   0aafd04a43ae29266e43920a7f9954d4a49f6fe43a5abffecc9c2fd5ad7d6cea
  label:    Aero the Acro-Bat 2
  name:     Aero the Acro-Bat 2
  region:   SNSP-AE2P-FAH
  revision: SPAL-AE2P-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   a4b1e125b421c5a58e2fd73edc4d58b31d7a596c07dee263c565f00ee712223f
  label:    Alfred Chicken
  name:     Alfred Chicken
  region:   SNSP-8A-FAH
  revision: SPAL-8A-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   d54d2703e474d7f469766d2d095ffcbbcab893e3fe58bbbcc57e24082a44bb40
  label:    Astérix
  name:     Asterix
  region:   SNSP-XE-FAH
  revision: SPAL-XE-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   d9127808fb02c47dd74fd22f39582c69f19936a794a8efc153cc0e51a0d4d782
  label:    Astérix & Obelix
  name:     Asterix & Obelix
  region:   SNSP-AXOP-FAH
  revision: SPAL-AXOP-0
  board:    SHVC-2A0N-20
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   e6adeeee287204b06300b790fe048358f54e322306806142fb4e667ae8b57378
  label:    Le Cobaye
  name:     Cobaye, Le
  region:   SNSP-LW-FAH
  revision: SPAL-LW-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   dde314fea056445685b97f9c8b554d2be81ea1fe6ace935934592464908d05fb
  label:    Donkey Kong Country
  name:     Donkey Kong Country
  region:   SNSP-8X-FAH
  revision: SPAL-8X-0
  board:    SHVC-BJ1M-10
    memory
      type: ROM
      size: 0x400000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   259537561fc1a0ddf0beacf9169ccb5fbe8ad88a322c07fccb862a39b379ae62
  label:    Donkey Kong Country 2: Diddy's Kong Quest
  name:     Donkey Kong Country 2 - Diddy's Kong Quest
  region:   SNSP-ADNP-FAH
  revision: SPAL-ADNP-1
  board:    SHVC-1J1M-20
    memory
      type: ROM
      size: 0x400000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   826a328f401cdf5a9ee87aaa7a2784cbb21813b165a6c7ca3f702fe6ba8c0804
  label:    Eric Cantona: Football Challenge
  name:     Eric Cantona - Football Challenge
  region:   SNSP-EC-FAH
  revision: SPAL-EC-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   ce09743d44a54f64862d8c53c11c2c84f2f861ec74c778bd8b05b0a3b07708d6
  label:    FIFA International Soccer
  name:     FIFA International Soccer
  region:   SNSP-84-FAH
  revision: SPAL-84-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   5a181f25055298579d24571b4f1cd21dd81583fb41a235fc4f226fa28d9df916
  label:    Jack Nicklaus Golf
  name:     Jack Nicklaus Golf
  region:   SNSP-JN-FAH
  revision: SFRA-JN-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   9b52a2848b78aa7ecb7cac295bce7ae033089af6099e127c2c904f518c6d4b89
  label:    The Lost Vikings
  name:     Lost Vikings, The
  region:   SNSP-LV-FAH
  revision: SFRA-LV-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   8510491f99400115ccf33570269bc4e484fb56370f7ac36f12e73eec19d342da
  label:    Lucky Luke
  name:     Lucky Luke
  region:   SNSP-ALYP-FAH
  revision: SPAL-ALYP-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   1c41bb11c9df5aa8a6ca98caa5215418f37025f7a5e88ff62188b257338af3ab
  label:    Mega Man X
  name:     Mega Man X
  region:   SNSP-RX-FAH
  revision: SPAL-RX-0
  board:    SHVC-2A0N-11
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   a6772914f3d0c2fda5bfde855a41f211c89d9452b7bd437b41d69b46727ed6e8
  label:    Nintendo Scope 6
  name:     Nintendo Scope 6
  region:   SNSP-LR-FAH
  revision: SPAL-LR-0
  board:    SHVC-1A0N-02
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   558b437e10915c2ca79f376634fa4623c87efdb9292a5878b886c7a6fbef61e2
  label:    Parodius
  name:     Parodius
  region:   SNSP-PD-FAH
  revision: SPAL-PD-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   034850392507ec2ea7219970a8be7ad4a4418a11eccbda9df7a5bbf4c74f0287
  label:    Plok
  name:     Plok
  region:   SNSP-P4-FAH
  revision: SFRA-P4-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   3a30a6aff66e1ab44b7f742eafde45710cd5a7165681649470c3f04afa579927
  label:    R-Type III: The Third Lightning
  name:     R-Type III - The Third Lightning
  region:   SNSP-ER-FAH
  revision: SPAL-ER-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   553940ad4b0bbcf375cf2a06f092b44fcd880db820828c8c900c4cd9d4f5753f
  label:    Starwing
  name:     Starwing
  region:   SNSP-FO-FAH
  revision: SPAL-FO-0
  board:    SNSP-1C0N5S-01
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save
      volatile

game
  sha256:   35c1d6623748f66e254843cf340121e3e268a8301951f35c7ba3ef666bc293bf
  label:    Street Fighter II: The World Warrior
  name:     Street Fighter II - The World Warrior
  region:   SNSP-S2-FAH
  revision: SPAL-S2-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   54ac1f0c99f1521e4c390f43af5da2138ec0526a97912790bdb53f9ab1b10b63
  label:    Street Fighter II Turbo: Hyper Fighting
  name:     Street Fighter II Turbo - Hyper Fighting
  region:   SNSP-TI-FAH
  revision: SPAL-TI-1
  board:    SHVC-BJ0N-01
    memory
      type: ROM
      size: 0x280000
      content: Program

game
  sha256:   abe8db202f9bd12d845a4c7ecb97d85fb149d7f17a608b3eae015d92f52f2c04
  label:    Stunt Race FX
  name:     Stunt Race FX
  region:   SNSP-CQ-FAH
  revision: SPAL-CQ-0
  board:    SHVC-1CA6B-01
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x10000
      content: Save
    oscillator
      frequency: 21440000

game
  sha256:   2b097fb72be6aff2030a488df87a349667eb204c2281fb0a2bb7eeafca33d5ed
  label:    Super Hockey
  name:     Super Hockey
  region:   SNSP-NH-FAH
  revision: SFRA-NH-0
  board:    SHVC-1A1M-11
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   9760fb407282d91005044fb08f9c15dc3e0ae65063a02eedfbbd285566501fd0
  label:    Super Mario All-Stars
  name:     Super Mario All-Stars
  region:   SNSP-4M-FAH
  revision: SPAL-4M-0
  board:    SHVC-1A3B-20
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   12d04359329bd646fc65c62db6121b4a7e4ece68556d68252e81ced421069f4c
  label:    Super Mario Kart
  name:     Super Mario Kart
  region:   SNSP-MK-FAH
  revision: SPAL-MK-0
  board:    SHVC-1K1B-01
    memory
      type: ROM
      size: 0x80000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save
    memory
      type: ROM
      size: 0x1800
      content: Program
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1
    memory
      type: ROM
      size: 0x800
      content: Data
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1
    memory
      type: RAM
      size: 0x200
      content: Data
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1
      volatile
    oscillator
      frequency: 7600000

game
  sha256:   5cc54b1e5c8d3c7701a5e20514145c3b36f15f26fe0a4fe6d2e43677e4b4eda9
  label:    Super Mario World
  name:     Super Mario World
  region:   SNSP-MW-FAH
  revision: SPAL-MW-1
  board:    SHVC-1A1B-06
    memory
      type: ROM
      size: 0x80000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   824f07e93c9ad38fe408af561e8979e3c0211f0c6c98aeb6e6bc85cd6f9edc91
  label:    Super Mario World 2: Yoshi's Island
  name:     Super Mario World 2 - Yoshi's Island
  region:   SNSP-YI-FAH
  revision: SPAL-YI-1
  board:    SHVC-1CB5B-20
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save
    oscillator
      frequency: 21440000

game
  sha256:   640acb63dae038ad6f0ae65e103416f5a1f84d4a37ddaeeab5046122def774d5
  label:    Super Metroid
  name:     Super Metroid
  region:   SNSP-RI-FAH
  revision: SPAL-RI-0
  board:    SHVC-BA3M-10
    memory
      type: ROM
      size: 0x300000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   71de4dec6a8e8be685627b6e929317f7cefb836997059bbd3b438ccc07a60044
  label:    Tintin: Le Temple du Soleil
  name:     Tintin - Le Temple du Soleil
  region:   SNSP-A2VP-FAH
  revision: SPAL-A2VP-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   da8b8bccebf51b70213adecda37387d1bdb55aeb7bc0805bb1be1cd9b514daf6
  label:    Tintin au Tibet
  name:     Tintin au Tibet
  region:   SNSP-AT6P-FAH
  revision: SPAL-AT6P-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   5a9103b04b9246f63af9018cbbd7934c6b79076dd9b0062887bd16077cd37c81
  label:    Val d'Isère Championship
  name:     Val d'Isere Championship
  region:   SNSP-8V-FAH
  revision: SPAL-8V-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

//Super Nintendo (FRA)

//database
//  revision: 2018-09-21

game
  sha256:   65df600780021f13ced52e7fbc507b7b2e6491b2c5c25fe78d0515dcbe669403
  label:    Aladdin
  name:     Aladdin
  region:   SNSP-RJ-FRA
  revision: SFRA-RJ-0
  board:    SHVC-2A0N-11
    memory
      type: ROM
      size: 0x140000
      content: Program

game
  sha256:   7baaa65ced8fed14e161615e1fffff971f10be4b723523b4a7302891db02ba09
  label:    Blanco World Class Rugby
  name:     Blanco World Class Rugby
  region:   SNSP-WY-FRA
  revision: SFRA-WY-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   dd920a8f29dde8712a246a56d28ac0825d15c041d7f77f97dbeb37680751ce68
  label:    Dragon Ball Z: La Légende Saien
  name:     Dragon Ball Z - La Legende Saien
  region:   SNSP-EF-FRA
  revision: SFRA-EF-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   e135511707c68c16d07b656f63e2b9dc42f436a37b0c85a949fc9e75e1111b34
  label:    Dragon Ball Z: Super Butouden
  name:     Dragon Ball Z - Super Butouden
  region:   SNSP-Z2-FRA
  revision: SFRA-Z2-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   663116d015cad3398644608ca6f2506071ab23d38b03b4890e1a6caecb1a55aa
  label:    Dragon Ball Z: Ultime Menace
  name:     Dragon Ball Z - Ultime Menace
  region:   SNSP-AZ4F-FRA
  revision: SPAL-AZ4F-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   a97a5ffdef892d7574e5cb807ec657059fd65490186feae975079cd11baa2df7
  label:    Goof Troop
  name:     Goof Troop
  region:   SNSP-G6-FRA
  revision: SFRA-G6-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   b2a44f5bb49f6a033e5c82f05356246f5918cacf6f026b88cab5a0b410659b01
  label:    Illusion of Time
  name:     Illusion of Time
  region:   SNSP-JG-FRA
  revision: SFRA-JG-0
  board:    SHVC-1J3M-20
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   8dc982c6dd0dfd094b74e4af94a2b10ddc51d334f7f0fa77a9f70917fa324e84
  label:    Jimmy Connors Pro Tennis Tour
  name:     Jimmy Connors Pro Tennis Tour
  region:   SNSP-JC-FRA
  revision: SFRA-JC-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   7ba2709cffa654f73b3b1364c13d6a5b595b820629102fe3d51c10bca30d0e4e
  label:    Jurassic Park
  name:     Jurassic Park
  region:   SNSP-J8-FRA
  revision: SFRA-J8-0
  board:    SHVC-2A0N-01
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   29a0d5812ccbb4b11bdb55d8f751a2a797b4110bf402ca8ba15eb2bf85db7b39
  label:    The Legend of Zelda: A Link to the Past
  name:     Legend of Zelda, The - A Link to the Past
  region:   SNSP-ZL-FRA
  revision: SFRA-ZL-0
  board:    SHVC-1A3B-13
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   6d37b2ed794b2f3e5f3d28490ffe89a0ff3c8a0cfc0418bd2aa8e0c66d4868ff
  label:    Le Livre de la Jungle
  name:     Livre de la Jungle, Le
  region:   SNSP-7K-FRA
  revision: SPAL-7K-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   18f5666860c36fabebc909d262e234bb0bf51a87b55945c134118a4c36b49b52
  label:    Mario is Missing!
  name:     Mario is Missing!
  region:   SNSP-MU-FRA
  revision: SFRA-MU-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   600df7da79c95e14c1b9e6e54e0b3a061d079831d015ef6f31bc3772c1d0efc6
  label:    MechWarrior
  name:     MechWarrior
  region:   SNSP-WM-FRA
  revision: SFRA-WM-0
  board:    SHVC-1A1M-01
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   1432590492b63d7103caa715e10e3c3ebbc77b63b6472ee9dee0e5afda311551
  label:    Mystic Quest Legend
  name:     Mystic Quest Legend
  region:   SNSP-MQ-FRA
  revision: SFRA-MQ-0
  board:    SHVC-1A3M-20
    memory
      type: ROM
      size: 0x80000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   a4621118ef174d2c8a9ffe934e31ff3a00f66a3aaa13340983eec43625a773f6
  label:    Pac-Man 2: The New Adventures
  name:     Pac-Man 2 - The New Adventures
  region:   SNSP-25-FRA
  revision: SFRA-25-0
  board:    SHVC-2A0N-20
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   febcb3325240293cc82b4038630b89bbd936f092cb172107eb82452a5555859e
  label:    Ranma ½
  name:     Ranma Half
  region:   SNSP-R2-FRA
  revision: SFRA-R2-0
  board:    SHVC-2J0N-01
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   896e09a0d24bfec0412aa75d121063b015153a754ed542f7db7d66366b555de4
  label:    Le Roi Lion
  name:     Roi Lion, Le
  region:   SNSP-ALKP-FRA
  revision: SPAL-ALKP-0
  board:    SHVC-BJ0N-01
    memory
      type: ROM
      size: 0x300000
      content: Program

game
  sha256:   0aa16d6b588ba05ab00936201e68a694746fc5e1b2e4f2dbf7cda09265a81379
  label:    Sailor Moon
  name:     Sailor Moon
  region:   SNSP-AE-FRA
  revision: SFRA-AE-0
  board:    SHVC-2J0N-11
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   b730adcbb34a19f8fd1c2abe27455cc3256329a9b8a021291e3009ea33004127
  label:    Secret of Mana
  name:     Secret of Mana
  region:   SNSP-K2-FRA
  revision: SFRA-K2-1
  board:    SHVC-1J3M-11
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   f73e6da9e979c839c7c22ec487bea6667d3e65e7d8f9fcc97a2bcdeb4487cddf
  label:    SimCity
  name:     SimCity
  region:   SNSP-SC-FRA
  revision: SFRA-SC-0
  board:    SHVC-1A5M-01
    memory
      type: ROM
      size: 0x80000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save

game
  sha256:   1f226553ba05fe738d085a88154469bbc9f9058f7dfc320a327259d84ae5f393
  label:    Soul Blazer
  name:     Soul Blazer
  region:   SNSP-SO-FRA
  revision: SFRA-SO-0
  board:    SHVC-1J3M-20
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   5d0a234a2fcb343d169206d9d7d578507c44f800ead9cc9ccfa0b1d4cb1cc9e5
  label:    Terranigma
  name:     Terranigma
  region:   SNSP-AQTF-FRA
  revision: SPAL-AQTF-0
  board:    SHVC-1J3M-20
    memory
      type: ROM
      size: 0x400000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

//Super Nintendo (FRG)

//database
//  revision: 2018-04-14

game
  sha256:   f415cafaaac4d5fe062b61be35e64ee6b5e8b925f12b9c82777b4566d31de8f4
  label:    Choplifter III
  name:     Choplifter III
  region:   SNSP-3C-FRG
  revision: SPAL-3C-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   5e1cce5ccedb20eff9d188eca445e54bd413db63570fd281f76b3ab56abffd82
  label:    Claymates
  name:     Claymates
  region:   SNSP-Y5-FRG
  revision: SPAL-Y5-0
  board:    SHVC-1J0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   ddad4a3708b8cf760e520b784f42d7085154b0699f3824b8d722512ccab687cb
  label:    Killer Instinct
  name:     Killer Instinct
  region:   SNSP-AKLP-FRG
  revision: SPAL-AKLP-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x400000
      content: Program

//Super Nintendo (GPS)

//database
//  revision: 2018-04-14

game
  sha256:   0d4c05f75c95d5b8c0fc00e61027ce3fda2dd288fcf6695656232176fce7843e
  label:    Super Hockey
  name:     Super Hockey
  region:   SNSP-NH-GPS
  revision: SPAL-NH-0
  board:    SHVC-1A1M-20
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

//Super Nintendo (HOL)

//database
//  revision: 2018-04-14

game
  sha256:   dc8b3144329a23459bfcce93c89e0e561d133709aca6bfc74f9e0755f1a04b91
  label:    Lufia
  name:     Lufia
  region:   SNSP-ANIH-HOL
  revision: SPAL-ANIH-0
  board:    SHVC-1A3M-30
    memory
      type: ROM
      size: 0x300000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

//Super Nintendo (ITA)

//database
//  revision: 2018-09-21

game
  sha256:   deab7aad7c168423e43eae14e9e31efa29c7341ab84f936be508911ce508b372
  label:    MechWarrior
  name:     MechWarrior
  region:   SNSP-WM-ITA
  revision: SITA-WM-0
  board:    SHVC-1A1M-01
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   aafbae4c2a7a5a35c81a183df0470027b4b5690f836592af21c15af6b259328d
  label:    Super Strike Gunner
  name:     Super Strike Gunner
  region:   SNSP-SG-ITA
  revision: SPAL-SG-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

//Super Nintendo (LTN)

//database
//  revision: 2018-04-14

game
  sha256:   e678a29a93111cf2016c487ba9977b14de8f719040651a42c74bd74eea2d0d81
  label:    The Death and Return of Superman
  name:     Death and Return of Superman, The
  region:   SNS-9D-LTN
  revision: SNS-9D-1
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

//Super Nintendo (NOE)

//database
//  revision: 2020-01-01

game
  sha256:   b342d12d71729edebc1911725ea23d58c1a397b27253a5c8cd96cfb58af242a9
  label:    Addams Family Values
  name:     Addams Family Values
  region:   SNSP-VY-NOE
  revision: SPAL-VY-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   475c601df7384621539ebcf2586a50720cc8de5d089b71fc65ff3f605cff7c8f
  label:    The Addams Family
  name:     Addams Family, The
  region:   SNSP-AF-NOE
  revision: SPAL-AF-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   12c00475e5d881c26fbdce025d05ca32d4f5534bfa441d9887624a843df74222
  label:    Aero the Acro-Bat
  name:     Aero the Acro-Bat
  region:   SNSP-XB-NOE
  revision: SPAL-XB-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   a4b1e125b421c5a58e2fd73edc4d58b31d7a596c07dee263c565f00ee712223f
  label:    Alfred Chicken
  name:     Alfred Chicken
  region:   SNSP-8A-NOE
  revision: SPAL-8A-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   b2df16101a2daa0f718853be92e3cf5d88f8e8843d04962e4b403d13769c1119
  label:    Alien vs. Predator
  name:     Alien vs. Predator
  region:   SNSP-AP-NOE
  revision: SPAL-AP-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   daa9a6a3872f252d74d7112094b6c97409fc166dae564e032b5158d12443b68c
  label:    An American Tail: Fievel Goes West
  name:     American Tail, An - Fievel Goes West
  region:   SNSP-9W-NOE
  revision: SPAL-9W-0
  board:    SHVC-1J0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   85b88af9b1f2071804e27c3ff9d42da6b26417c26b6f8a9e878733b8c35724ab
  label:    Another World
  name:     Another World
  region:   SNSP-TW-NOE
  revision: SPAL-TW-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   d54d2703e474d7f469766d2d095ffcbbcab893e3fe58bbbcc57e24082a44bb40
  label:    Asterix
  name:     Asterix
  region:   SNSP-XE-NOE
  revision: SPAL-XE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   bb81d1730222c518084d06c5ce456ee860d5ccb6a410f14a73e68971305bdd12
  label:    Axelay
  name:     Axelay
  region:   SNSP-AX-NOE
  revision: SPAL-AX-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   ffd6668607ee279d79ef3f2b35713b99f8b046a53c608a82df9d37ef39bb079b
  label:    Batman Returns
  name:     Batman Returns
  region:   SNSP-BJ-NOE
  revision: SPAL-BJ-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   716214f0b17233021f8ee07500530947655c4c871cd041290adb204523984821
  label:    Battletoads in Battlemaniacs
  name:     Battletoads in Battlemaniacs
  region:   SNSP-NX-NOE
  revision: SPAL-NX-0
  board:    SHVC-YA0N-01
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   13cd21706cbb02536714c9f0a4d14b37d59a9161abe155e043fd74ba4d5dea15
  label:    Beethoven: The Ultimate Canine Caper!
  name:     Beethoven - The Ultimate Canine Caper!
  region:   SNSP-2V-NOE
  revision: SPAL-2V-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   08e02381cf59bdb3ad4d561587c8ccd001c722c7731705a9d84b39cb0337ca53
  label:    Blackhawk
  name:     Blackhawk
  region:   SNSP-6Z-NOE
  revision: SPAL-6Z-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   34221406999172e342b9442e30d12c467fbcd9afa86bedb5ec5ddb5ebc2ee8a1
  label:    The Blues Brothers
  name:     Blues Brothers, The
  region:   SNSP-B6-NOE
  revision: SPAL-B6-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   5a054466ffe0694599498b9d94427b25d4f4d55ab4fc1542335f69025e817a3f
  label:    B.O.B.
  name:     BOB
  region:   SNSP-B4-NOE
  revision: SPAL-B4-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   6df9b702c65183ef13dd2a66d1bc95e7ec1452e088e9c19a0e3beb9ef952e939
  label:    Brawl Brothers: Rival Turf! 2
  name:     Brawl Brothers - Rival Turf! 2
  region:   SNSP-RE-NOE
  revision: SPAL-RE-0
  board:    SHVC-2A0N-11
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   0a49023824d812c64cd262db6e85ce9900c88af7ac6dab3e47078ab0dd1e546c
  label:    Bubsy in Claws Encounters of the Furred Kind
  name:     Bubsy in Claws Encounters of the Furred Kind
  region:   SNSP-YN-NOE
  revision: SPAL-YN-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   6795e22c6ddcbfa48de1e5b1b22ad72890214533a12e59763cb4b8d5b2535aef
  label:    Bugs Bunny: Rabbit Rampage
  name:     Bugs Bunny - Rabbit Rampage
  region:   SNSP-R7-NOE
  revision: SPAL-R7-0
  board:    SHVC-2A0N-11
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   a20bdbdafccee20bf17eae28fdb0b79fced5b53f90a1cad259461a37903f9ad1
  label:    California Games II
  name:     California Games II
  region:   SNSP-C2-NOE
  revision: SPAL-C2-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   a2e3a81f7dfbd50423bebfce8c2f6b85d58f7336810ee6c813fb74ac7d4427a3
  label:    Captain America and The Avengers
  name:     Captain America and The Avengers
  region:   SNSP-6A-NOE
  revision: SPAL-6A-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   a60886ea6459861bb8d149023a975c4e83c172847264756840ca0754eb9f1f15
  label:    Championship Pool
  name:     Championship Pool
  region:   SNSP-5P-NOE
  revision: SPAL-5P-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   fd7e471b7855614f1a4782c2194e6d268406c694d0f674350317a3efed26c4aa
  label:    The Chaos Engine
  name:     Chaos Engine, The
  region:   SNSP-UD-NOE
  revision: SPAL-UD-0
  board:    SHVC-2J0N-11
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   9f072794e9f379e35dabd50a714e08b98deab61e3dd97ef982d7504b85b28d24
  label:    The Chessmaster
  name:     Chessmaster, The
  region:   SNSP-CH-NOE
  revision: SPAL-CH-0
  board:    SHVC-1A0N-02
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   9ae517aef04a7157f9e8e13c874a9ac5f452d02ad6343e03fed9cce0aa515171
  label:    Chuck Rock
  name:     Chuck Rock
  region:   SNSP-CK-NOE
  revision: SPAL-CK-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   f000606a504c7a51617c0e32865924a68bf899170aea2647cf403fede8491c0e
  label:    Clay Fighter
  name:     Clay Fighter
  region:   SNSP-8C-NOE
  revision: SPAL-8C-0
  board:    SHVC-1J0N-10
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   1ee967210f980b76b6c810a8b130049405183d20e4c308b17c7ef982912fc628
  label:    Cliffhanger
  name:     Cliffhanger
  region:   SNSP-6C-NOE
  revision: SPAL-6C-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   e0c51ca00f22716531cb9138cfe8d5af4680cde1c95bfbfcd52c35f246763e65
  label:    Congo's Caper
  name:     Congo's Caper
  region:   SNSP-J2-NOE
  revision: SPAL-J2-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   81220ffa1f167d9b5b909b0315a6734fae8957b8d708b3887fb0e9105f816edb
  label:    Cool Spot
  name:     Cool Spot
  region:   SNSP-C8-NOE
  revision: SPAL-C8-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   5d42cef66c529939b6038f4ecaf1aeb06acda2dabbf7bcf4e7203f3cb6b43f7a
  label:    Cybernator
  name:     Cybernator
  region:   SNSP-AV-NOE
  revision: SPAL-AV-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   a9ababea552f9d7f756e99bfc0f7f88b30898297c762d35d603837475e713f86
  label:    Daffy Duck: The Marvin Missions
  name:     Daffy Duck - The Marvin Missions
  region:   SNSP-YF-NOE
  revision: SPAL-YF-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   6ce516e3d1a7068cc9732cd3517cfd1e92179f2340c63a244625a1ff49815085
  label:    Daze Before Christmas
  name:     Daze Before Christmas
  region:   SNSP-ADCP-NOE
  revision: SPAL-ADCP-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   3e4e16a5d47197a2f7bc724e3a6875b682f738800c7377ea655a6d7f54156a0e
  label:    Dennis
  name:     Dennis
  region:   SNSP-4D-NOE
  revision: SPAL-4D-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   a7612dbb5fd122090d4fcbffec81d295c0911ff5c83fa865c441c7b3996fcc92
  label:    DinoCity
  name:     DinoCity
  region:   SNSP-DW-NOE
  revision: SPAL-DW-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   984107f2dbccbcaec1f7e74b62af82785918438144221bd0228fa36419a11ed8
  label:    Dragon's Lair
  name:     Dragon's Lair
  region:   SNSP-DI-NOE
  revision: SPAL-DI-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   dddacae010766c1201f50810fcf15dff7c0f6d41ac1a1004c8eea4873a71db12
  label:    F-Zero
  name:     F-Zero
  region:   SNSP-FZ-NOE
  revision: SPAL-FZ-0
  board:    SHVC-1A1B-06
    memory
      type: ROM
      size: 0x80000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   450df78c9b7c92e9f8ce5c2ee0e1dbf939031c1e4f9e10c52c8d8f874364d1d6
  label:    Fatal Fury
  name:     Fatal Fury
  region:   SNSP-GN-NOE
  revision: SPAL-GN-0
  board:    SHVC-2A0N-10
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   c7abc997e0f685a726dacd82e2734f557028490c1c9b8e575bc6cbc18de243a4
  label:    Final Fight 2
  name:     Final Fight 2
  region:   SNSP-F2-NOE
  revision: SPAL-F2-0
  board:    SHVC-2A0N-11
    memory
      type: ROM
      size: 0x140000
      content: Program

game
  sha256:   b7ab3afb4f63b95c2ceafcaf03fd691fe9fc53b173d7fd22a8a111b40f8713b3
  label:    First Samurai
  name:     First Samurai
  region:   SNSP-FK-NOE
  revision: SPAL-FK-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   6cce3d0c8b2eec350475e3c274d7ad80c9a208ba101d9cf9ac637c5d09760ab7
  label:    Flashback
  name:     Flashback
  region:   SNSP-5F-NOE
  revision: SPAL-5F-0
  board:    SHVC-2J0N-01
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   c7935e5e147a8c1b2646fbb7776cf73198967bd053f2a58f4f01e9a3a5c2911a
  label:    The Flintstones: The Treasure of Sierra Madrock
  name:     Flintstones, The - The Treasure of Sierra Madrock
  region:   SNSP-9F-NOE
  revision: SPAL-9F-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   6d0095422fe380202e62ed5b34f038681f777dcd9a943cf3534645068e118fb2
  label:    George Foreman's KO Boxing
  name:     George Foreman's KO Boxing
  region:   SNSP-GK-NOE
  revision: SPAL-GK-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   ddda02bc9fff44c702ce6b2d043c8c2dc520c568a1df221de0bde24e16e1618d
  label:    Harley's Humongous Adventure
  name:     Harley's Humongous Adventure
  region:   SNSP-HV-NOE
  revision: SPAL-HV-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   320c9a88c2fa3221d8e8d2dc8eaae9d6143753b0b881166b5376bcd50031e1da
  label:    Home Alone
  name:     Home Alone
  region:   SNSP-HA-NOE
  revision: SPAL-HA-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   ca2d67f208e3dc7aaeea88b60d7008b6e00b525bf935c259b642ad5cad783bb1
  label:    Home Alone 2: Lost in New York
  name:     Home Alone 2 - Lost in New York
  region:   SNSP-HN-NOE
  revision: SPAL-HN-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   e4b315f714529a9ba593ffa16ac7456db178294405e26811313c08177969a05f
  label:    Hook
  name:     Hook
  region:   SNSP-HO-NOE
  revision: SPAL-HO-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   a9dffa5d97855733f14b1b888bc478e8e0630107812b7b3df729c499e0e0734f
  label:    Hyper Zone
  name:     Hyper Zone
  region:   SNSP-HZ-NOE
  revision: SPAL-HZ-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   039beb46f81ad9e0844ecec420cc78bfdbf2b1ae940adb4fdf08dbf1b55ac7ed
  label:    Illusion of Time
  name:     Illusion of Time
  region:   SNSP-JG-NOE
  revision: SFRG-JG-1
  board:    SHVC-1J3M-20
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   42bc8987dda4f8e4a1d135f3327ef89fb51b9bea97a79dba0060f6fdf9f4c0ba
  label:    The Incredible Crash Dummies
  name:     Incredible Crash Dummies, The
  region:   SNSP-C7-NOE
  revision: SPAL-C7-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   2c08b32ecbba97bea0fbb569186294f9ad738a301d90245d9729046ba0ecabab
  label:    James Bond Jr.
  name:     James Bond Jr.
  region:   SNSP-JJ-NOE
  revision: SPAL-JJ-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   09299d142e485ba2fcdbd9b3a6d1a5acfbc7fc70b06cf22be28479686419a7a9
  label:    Jimmy Connors Pro Tennis Tour
  name:     Jimmy Connors Pro Tennis Tour
  region:   SNSP-JC-NOE
  revision: SFRG-JC-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   74c55ea3c9733bf263628a260df7492fc840d7de1c3fceebb7bcf6d99a8c81d6
  label:    Joe & Mac: Caveman Ninja
  name:     Joe & Mac - Caveman Ninja
  region:   SNSP-JT-NOE
  revision: SPAL-JT-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   3fa82117fe88b0b5398995b68624f3027184259456123f2a61d55f668326c769
  label:    Kevin Keegan's Player Manager
  name:     Kevin Keegan's Player Manager
  region:   SNSP-PJ-NOE
  revision: SFRG-PJ-0
  board:    SHVC-1A3M-10
    memory
      type: ROM
      size: 0x80000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   53ccf6970d4577608893f38633cc0609ba0eec1b393f891df31e2e5c2f903a9c
  label:    Kid Klown in Crazy Chase
  name:     Kid Klown in Crazy Chase
  region:   SNSP-ZI-NOE
  revision: SPAL-ZI-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   11a6c5de89b25836c8b66d3e3077bacdcaf52faab5a0f7fe2bc751aa85a8dd68
  label:    King of the Monsters
  name:     King of the Monsters
  region:   SNSP-KM-NOE
  revision: SPAL-KM-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   9bc7afdb9ffaca90db4390697e80a75bf4889cd8529e21c28ad42c41171e2630
  label:    Knights of the Round
  name:     Knights of the Round
  region:   SNSP-LO-NOE
  revision: SPAL-LO-0
  board:    SHVC-2A0N-11
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   9ed408bb30b32c0a86605ea80e2b431563a33d54354a4b68e8b7d4eedde25295
  label:    Last Action Hero
  name:     Last Action Hero
  region:   SNSP-L5-NOE
  revision: SPAL-L5-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   4a7444780a750f97943d974589586d4cf89d8957e396cc5a7ad565cd4c1b70a7
  label:    The Legend of the Mystical Ninja
  name:     Legend of the Mystical Ninja, The
  region:   SNSP-GG-NOE
  revision: SFRG-GG-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   5ec66298ddb579b35cc5d3df5bfeeee05bdf71347565c7c5f5f3869bf4f1e469
  label:    Looney Tunes Basketball
  name:     Looney Tunes Basketball
  region:   SNSP-ALTP-NOE
  revision: SPAL-ALTP-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   0ecdd9d6fc78c0bdbc2f684c682ec834cda1148ed2e675cc783a95c601000d3b
  label:    Lothar Matthäus Super Soccer
  name:     Lothar Matthaus Super Soccer
  region:   SNSP-AOMD-NOE
  revision: SPAL-AOMD-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   8510491f99400115ccf33570269bc4e484fb56370f7ac36f12e73eec19d342da
  label:    Lucky Luke
  name:     Lucky Luke
  region:   SNSP-ALYP-NOE
  revision: SPAL-ALYP-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   be9c42913fee053a8d854054a27a0fb4d1cf6c092b46c7291d2743e94204a323
  label:    Magic Boy
  name:     Magic Boy
  region:   SNSP-YG-NOE
  revision: SPAL-YG-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   8f6920549b28a065a030fbdbe2ea2e9d966d42ab5ac1ef0e9dabc99875a51df2
  label:    MechWarrior
  name:     MechWarrior
  region:   SNSP-WM-NOE
  revision: SFRG-WM-0
  board:    SHVC-1A1M-10
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   9283ac563e3d8244fb103db13256c669063e67be5eaf6e82b94527f079c9d8a3
  label:    Michael Jordan: Chaos in the Windy City
  name:     Michael Jordan - Chaos in the Windy City
  region:   SNSP-AWCP-NOE
  revision: SPAL-AWCP-0
  board:    SHVC-2J0N-11
    memory
      type: ROM
      size: 0x140000
      content: Program

game
  sha256:   4703cb071611874b0d9e9555f102278e33dd494202875dc994a232029148bf67
  label:    Mortal Kombat
  name:     Mortal Kombat
  region:   SNSP-KX-NOE
  revision: SPAL-KX-0
  board:    SHVC-2A0N-01
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   41803805d28cad8873029c4b648cc6dd7503b7a33afa8ff584434a970dc9d8c1
  label:    Mr. Nutz
  name:     Mr. Nutz
  region:   SNSP-N8-NOE
  revision: SPAL-N8-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   558b437e10915c2ca79f376634fa4623c87efdb9292a5878b886c7a6fbef61e2
  label:    Parodius
  name:     Parodius
  region:   SNSP-PD-NOE
  revision: SPAL-PD-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   b8fcbad3c712a2ff69a5f9bb9fbe4c4284f91bbe96fe849275a8bcfcb497d204
  label:    Phalanx: The Enforce Fighter A-144
  name:     Phalanx - The Enforce Fighter A-144
  region:   SNSP-PH-NOE
  revision: SPAL-PH-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   ec6f444dcab84d251c12eb4f47aeac23ed997bf0416c5a537bac6bb9d40b725d
  label:    The Pirates of Dark Water
  name:     Pirates of Dark Water, The
  region:   SNSP-8P-NOE
  revision: SPAL-8P-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   9932ed1419bc606ea19337b74a8ef17adaa6b31be5fca8d2b6b266b3f6383e39
  label:    Revolution X
  name:     Revolution X
  region:   SNSP-AXRD-NOE
  revision: SPAL-AXRD-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   b2717bec03b627c4f02bd6dd77cfa790ea4eab91f0f47626ea452c50369d35d4
  label:    Rival Turf
  name:     Rival Turf
  region:   SNSP-RB-NOE
  revision: SPAL-RB-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   a0888a9c02b0ca0c9e95246c9ea60407f7f9c4dfde1ff1c15b7f6d5bd4ea5b85
  label:    Samurai Shodown
  name:     Samurai Shodown
  region:   SNSP-A7SP-NOE
  revision: SPAL-A7SP-0
  board:    SHVC-BJ0N-01
    memory
      type: ROM
      size: 0x400000
      content: Program

game
  sha256:   c341668847170d36fa5cfb720568b0b1ecbb24fc426a821f665f1d3853a46a6d
  label:    Secret of Evermore
  name:     Secret of Evermore
  region:   SNSP-AEOD-NOE
  revision: SPAL-AEOD-0
  board:    SHVC-1J3M-20
    memory
      type: ROM
      size: 0x300000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   1444ab11f96f7750db992e9a4160532b27abede8a7054128c09f448300c91ebf
  label:    Secret of Mana
  name:     Secret of Mana
  region:   SNSP-K2-NOE
  revision: SFRG-K2-0
  board:    SHVC-1J3M-11
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   bc819454a2082f93b03ad3dc11795eb8cd4ccec72a41560462a91b6f0edd432f
  label:    Starwing
  name:     Starwing
  region:   SNSP-FO-NOE
  revision: SFRG-FO-0
  board:    SHVC-1C0N
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save
      volatile

game
  sha256:   e4df822294a12919b35cd73a1ca24105418ca2c2bc5e08f887fe1d10b3e4f200
  label:    Starwing Competition
  name:     Starwing Competition
  region:   SNSP-FU-NOE
  revision: SFRG-FU-0
  board:    SHVC-1C0N
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save
      volatile

game
  sha256:   35c1d6623748f66e254843cf340121e3e268a8301951f35c7ba3ef666bc293bf
  label:    Street Fighter II: The World Warrior
  name:     Street Fighter II - The World Warrior
  region:   SNSP-S2-NOE
  revision: SPAL-S2-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   54ac1f0c99f1521e4c390f43af5da2138ec0526a97912790bdb53f9ab1b10b63
  label:    Street Fighter II Turbo: Hyper Fighting
  name:     Street Fighter II Turbo - Hyper Fighting
  region:   SNSP-TI-NOE
  revision: SPAL-TI-1
  board:    SHVC-BJ0N-01
    memory
      type: ROM
      size: 0x280000
      content: Program

game
  sha256:   30291b60cb7bb0bf92dc48b922daff71f6bc4b29200bef5540a522bcb0a64cee
  label:    Super Air Diver
  name:     Super Air Diver
  region:   SNSP-AZ-NOE
  revision: SPAL-AZ-0
  board:    SHVC-1K0N-01
    memory
      type: ROM
      size: 0x80000
      content: Program
    memory
      type: ROM
      size: 0x1800
      content: Program
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1
    memory
      type: ROM
      size: 0x800
      content: Data
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1
    memory
      type: RAM
      size: 0x200
      content: Data
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1
      volatile
    oscillator
      frequency: 7600000

game
  sha256:   4dcf9213cf22c9e28e58b42ca7808224399d89b9b33f1fd592be6866db42755d
  label:    Super Battleship
  name:     Super Battleship
  region:   SNSP-8B-NOE
  revision: SPAL-8B-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   3b8e4da1435a548927a120a827eac01fef7d3636f3f763923063e5613adad42f
  label:    Super Castlevania IV
  name:     Super Castlevania IV
  region:   SNSP-AD-NOE
  revision: SPAL-AD-0
  board:    SHVC-1A0N-02
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   bbedc3b9263993c1294793895b5901973fbb159157db4c6d1b79ee8245007791
  label:    Super Double Dragon
  name:     Super Double Dragon
  region:   SNSP-WD-NOE
  revision: SPAL-WD-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   4d7fc331a811b8dc630b469262fd6f45e289243cef83101f32038158967d1b28
  label:    Super Game Boy
  name:     Super Game Boy
  region:   SNSP-A-SG-NOE
  revision: SYS-SGB-2
  board:    SGB-R-10
    memory
      type: ROM
      size: 0x40000
      content: Program
    memory
      type: ROM
      size: 0x100
      content: Boot
      manufacturer: Nintendo
      architecture: SM83
      identifier: SGB1

game
  sha256:   4c5b426ea5950b66098ba4377f6a86d091d7af2f4783895086a621aa98811596
  label:    Super Ice Hockey
  name:     Super Ice Hockey
  region:   SNSP-OX-NOE
  revision: SPAL-OX-0
  board:    SHVC-1J0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   2b1ca521975c3010650fd2055be8c6b964ea4eff765ad03198ac71995285fee7
  label:    Super Probotector: Alien Rebels
  name:     Super Probotector - Alien Rebels
  region:   SNSP-CS-NOE
  revision: SPAL-CS-0
  board:    SHVC-1A0N-02
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   7c7e90fb7c762769219234baf7b5fa6bf574fff7dc63b7134d49ec7c8b38ea7e
  label:    Super R-Type
  name:     Super R-Type
  region:   SNSP-SR-NOE
  revision: SPAL-SR-0
  board:    SHVC-1A0N-02
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   70c07b2c7bdff9353d3849e2d4bde2bfa631b29e0743862635c2212ac551cb27
  label:    Super SWIV
  name:     Super SWIV
  region:   SNSP-WV-NOE
  revision: SPAL-WV-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   1bc2b230d6bb6091412fcc9b957192d9a729496f9f2123449d968afb088fc525
  label:    Super Turrican
  name:     Super Turrican
  region:   SNSP-TU-NOE
  revision: SPAL-TU-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   454dbc5383f36e1d611fcc76a23162ae03e269c76c98919b1e7505ea4c7c2402
  label:    Super Turrican 2
  name:     Super Turrican 2
  region:   SNSP-A2TP-NOE
  revision: SPAL-A2TP-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   eea3584fe574b1e7ba6c559ef32f86a990f54c7fa36b25242656b4c6dc18e6f0
  label:    Syndicate
  name:     Syndicate
  region:   SNSP-AFYP-NOE
  revision: SPAL-AFYP-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   3e26fbb8004635f4128f8e989d96f89fddc0a2cf85a8ede6b93ae6648bd6a717
  label:    Teenage Mutant Hero Turtles V: Tournament Fighters
  name:     Teenage Mutant Hero Turtles V - Tournament Fighters
  region:   SNSP-8F-NOE
  revision: SPAL-8F-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   d6968f79ec66cec751dcf54a1fd0c7321a8e1b69722d81b10d1969a8415412a6
  label:    Terminator 2: The Arcade Game
  name:     Terminator 2 - The Arcade Game
  region:   SNSP-XV-NOE
  revision: SPAL-XV-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   2465335e0a16e65d41d42067f53d97ce0929d003ef41ec605d160cfb30c20df7
  label:    Terranigma
  name:     Terranigma
  region:   SNSP-AQTD-NOE
  revision: SPAL-AQTD-0
  board:    SHVC-1J3M-20
    memory
      type: ROM
      size: 0x400000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   613cd1a31eaded18648168bd7453a57830ca9a6f3c10de5154625436fbd49556
  label:    Terranigma
  name:     Terranigma
  region:   SNSP-AQTD-NOE
  revision: SPAL-AQTD-1
  board:    SHVC-1J3M-20
    memory
      type: ROM
      size: 0x400000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   3c94c600fefcf34e5d251885c209d6b61b40de119589b67badec38deeffb46f9
  label:    Tiny Toon Adventures: Buster Busts Loose!
  name:     Tiny Toon Adventures - Buster Busts Loose!
  region:   SNSP-TA-NOE
  revision: SPAL-TA-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   94e6fe78bb1a1d89ccfd74ad92e2a489f8e2e257d6dfe62404155741763f962f
  label:    True Lies
  name:     True Lies
  region:   SNSP-ATLD-NOE
  revision: SPAL-ATLD-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   a1105819d48c04d680c8292bbfa9abbce05224f1bc231afd66af43b7e0a1fd4e
  label:    Unirally
  name:     Unirally
  region:   SNSP-4L-NOE
  revision: SPAL-4L-0
  board:    SHVC-1A3M-30
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   847c9189580fc92e785c8124cbde4f1d72be75e1941b35021f6b159e0470c1b0
  label:    WeaponLord
  name:     WeaponLord
  region:   SNSP-ADWP-NOE
  revision: SPAL-AWDP-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x300000
      content: Program

game
  sha256:   8315596b4fe517b970d004336c86ed2bc74e167692ffaa51c529a41e2197519e
  label:    Winter Olympics: Lillehammer '94
  name:     Winter Olympics - Lillehammer '94
  region:   SNSP-W4-NOE
  revision: SPAL-W4-0
  board:    SHVC-1J0N-10
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   9eb2f90cae9958ae7f387d797cb28797b5ccaf520c41d4c5ca9494c74a87c422
  label:    World Heroes
  name:     World Heroes
  region:   SNSP-WZ-NOE
  revision: SPAL-WZ-0
  board:    SHVC-1J0N-01
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   92dd66642e86b9b4156352a583bd479c200c342820a8b6cf906cca6bb923cf25
  label:    Worms
  name:     Worms
  region:   SNSP-AW3P-NOE
  revision: SPAL-AW3P-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x1e0000
      content: Program

game
  sha256:   24aad9739f8ffe9319f20d4fa1c4f58108e73843d20d65296926e00ba9c456be
  label:    Zombies
  name:     Zombies
  region:   SNSP-ZA-NOE
  revision: SPAL-ZA-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

//Super Nintendo (SCN)

//database
//  revision: 2018-09-21

game
  sha256:   beb379ba48f63561c0f939ecd8f623ec06c1b5e06976eef9887e5c62f3df2766
  label:    Gods
  name:     Gods
  region:   SNSP-GZ-SCN
  revision: SPAL-GZ-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   896e09a0d24bfec0412aa75d121063b015153a754ed542f7db7d66366b555de4
  label:    The Lion King
  name:     Lion King, The
  region:   SNSP-ALKP-SCN
  revision: SPAL-ALKP-0
  board:    SHVC-BJ0N-01
    memory
      type: ROM
      size: 0x300000
      content: Program

game
  sha256:   4fb9eb8fa4d9c3a0b6c24bac5b0a0b0f079f083f5e6dfa937a161c8f4bcde853
  label:    Shadowrun
  name:     Shadowrun
  region:   SNSP-WR-SCN
  revision: SSWE-WR-0
  board:    SHVC-1A3M-20
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   e15247495311e91db9431d61777a264d4b42def011291d512b273fc8acd1cbfa
  label:    Soul Blazer
  name:     Soul Blazer
  region:   SNSP-SO-SCN
  revision: SPAL-SO-0
  board:    SHVC-1A3B-20
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   687c4f9a14cc16605f5e92aa0fe33bf083fe8e39ba781676259fadf932480890
  label:    Tintin i Tibet
  name:     Tintin i Tibet
  region:   SNSP-AT6X-SCN
  revision: SPAL-AT6X-0
  board:    SHVC-2A0N-20
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   a6297356fb06f1575b432fae463171f53e3b786fd77b841557547a9117fb52fe
  label:    X-Zone
  name:     X-Zone
  region:   SNSP-XZ-SCN
  revision: SPAL-XZ-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

//Super Nintendo (UKV)

//database
//  revision: 2018-04-14

game
  sha256:   38e050ac3ffec01031c6dd8ead20676aacec78ebf8d890a3a00d1badaaba3d3b
  label:    The Adventures of Dr. Franken
  name:     Adventures of Dr. Franken, The
  region:   SNSP-6F-UKV
  revision: SPAL-6F-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   8c5f2f8d45d86b27e48313d136477ba6f30989c93748d800ad6bf82f14ecd4a1
  label:    Battletoads & Double Dragon
  name:     Battletoads & Double Dragon
  region:   SNSP-UL-UKV
  revision: SPAL-UL-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   0a49023824d812c64cd262db6e85ce9900c88af7ac6dab3e47078ab0dd1e546c
  label:    Bubsy in Claws Encounters of the Furred Kind
  name:     Bubsy in Claws Encounters of the Furred Kind
  region:   SNSP-YN-UKV
  revision: SPAL-YN-0
  board:    SHVC-2A0N-10
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   536f9c2ff7dfdc6e5b51389142151b1c9e9d73f1c2451eafe16d0224d15ad35f
  label:    Desert Fighter
  name:     Desert Fighter
  region:   SNSP-OS-UKV
  revision: SPAL-OS-0
  board:    SHVC-1A3M-20
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   1c12660c99571692d2fba4ba871a1086b115486697e789f85fb939c55eeec7c7
  label:    Doom
  name:     Doom
  region:   SNSP-AD8P-UKV
  revision: SPAL-AD8P-0
  board:    SHVC-1CB0N7S-01
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x10000
      content: Save
      volatile
    oscillator
      frequency: 21440000

game
  sha256:   3d4715ce44045fbb27b72c0925020a52b748c9236db1e7782ee62f74615182fc
  label:    Eek! The Cat
  name:     Eek! The Cat
  region:   SNSP-E7-UKV
  revision: SPAL-E7-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   dddacae010766c1201f50810fcf15dff7c0f6d41ac1a1004c8eea4873a71db12
  label:    F-Zero
  name:     F-Zero
  region:   SNSP-FZ-UKV
  revision: SPAL-FZ-0
  board:    SHVC-1A1B-05
    memory
      type: ROM
      size: 0x80000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   b0d2439944b6ae85d4ec6f39c91e3c8b9c740a43d077f4d0675dc2c2235204df
  label:    Family Dog
  name:     Family Dog
  region:   SNSP-D8-UKV
  revision: SPAL-D8-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   7d423c7d5ac4fef6ae608ae87e77608294ef7e51a6f0afff1042ed66a1c639fe
  label:    The Flintstones
  name:     Flintstones, The
  region:   SNSP-AFNP-UKV
  revision: SPAL-AFNP-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   e9e2152411fec3bd10e8cd4587b62717169a25a4cd28f491f8e477b9aae2fcee
  label:    fun 'n games
  name:     Fun & Games
  region:   SNSP-7N-UKV
  revision: SPAL-7N-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   94b2e68503514afe956cbc8ef4fb2cb93a323761e9b7d205e44e544049a4ae3b
  label:    The Incredible Hulk
  name:     Incredible Hulk, The
  region:   SNSP-8U-UKV
  revision: SPAL-8U-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   9d936f3b0b5bea0b7c4588e65fa147fff1108d0e630337dd75eb16133a55e317
  label:    International Sensible Soccer: World Champions
  name:     International Sensible Soccer - World Champions
  region:   SNSP-8S-UKV
  revision: SPAL-8S-1
  board:    SHVC-1A3M-20
    memory
      type: ROM
      size: 0x80000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   975397a09ec1dfa518f1f0a2029ecb74e5b4c1113bf3376de94711672ff4e054
  label:    James Pond's Crazy Sports
  name:     James Pond's Crazy Sports
  region:   SNSP-JX-UKV
  revision: SPAL-JX-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   f5e74f09c485d58b444ef2b168d041a1d451056b5feb295901974ca73190dbdb
  label:    Jelly Boy
  name:     Jelly Boy
  region:   SNSP-AJBP-UKV
  revision: SPAL-AJBP-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   1be41a3a2d490be97b98f2ab8934d8e8812d7d2596598a7841e3027b23e95261
  label:    Jurassic Park
  name:     Jurassic Park
  region:   SNSP-J8-UKV
  revision: SPAL-J8-0
  board:    SHVC-2A0N-11
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   ddad4a3708b8cf760e520b784f42d7085154b0699f3824b8d722512ccab687cb
  label:    Killer Instinct
  name:     Killer Instinct
  region:   SNSP-AKLP-UKV
  revision: SPAL-AKLP-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x400000
      content: Program

game
  sha256:   6379d7d476a29ae2655e30fc20699772af81abefc903f5816872dd46e4e88242
  label:    Legend
  name:     Legend
  region:   SNSP-6L-UKV
  revision: SPAL-6L-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   654183585e88abf965b19570c194f3d69ef209a7f2d32f71881eceaea6a3487e
  label:    The Legend of Zelda: A Link to the Past
  name:     Legend of Zelda, The - A Link to the Past
  region:   SNSP-ZL-UKV
  revision: SPAL-ZL-0
  board:    SHVC-1A3B-13
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   0c4038eb0ee37c0faac6a04928b37b5c2f1047ab59c5345da16de48c92db5021
  label:    Lethal Enforcers
  name:     Lethal Enforcers
  region:   SNSP-LK-UKV
  revision: SPAL-LK-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   528f9697cdb5b50504aa4f6d6f7882c4997e7141898f9a00a630692b964204eb
  label:    Lethal Weapon
  name:     Lethal Weapon
  region:   SNSP-L3-UKV
  revision: SPAL-L3-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   1c41bb11c9df5aa8a6ca98caa5215418f37025f7a5e88ff62188b257338af3ab
  label:    Mega Man X
  name:     Mega Man X
  region:   SNSP-RX-UKV
  revision: SPAL-RX-0
  board:    SHVC-2A0N-11
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   d33f682605b3d6c8a162506ef333a24933ae26a32f10ff8e49fc113bcd189137
  label:    Mighty Morphin Power Rangers: The Fighting Edition
  name:     Mighty Morphin Power Rangers - The Fighting Edition
  region:   SNSP-A3RP-UKV
  revision: SPAL-A3RP-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   d147bfe392e8c2129fb887379410804d79be426bbffdd01cec7bd2332e03f39e
  label:    Pilotwings
  name:     Pilotwings
  region:   SNSP-PW-UKV
  revision: SPAL-PW-0
  board:    SHVC-1B0N-10
    memory
      type: ROM
      size: 0x80000
      content: Program
    memory
      type: ROM
      size: 0x1800
      content: Program
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1B
    memory
      type: ROM
      size: 0x800
      content: Data
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1B
    memory
      type: RAM
      size: 0x200
      content: Data
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1B
      volatile
    oscillator
      frequency: 7600000

game
  sha256:   5702fb6978229e398075cf0d20927b47a5881b572532813557e86b8e9cf63db0
  label:    Secret of Mana
  name:     Secret of Mana
  region:   SNSP-K2-UKV
  revision: SPAL-K2-1
  board:    SHVC-1J3M-20
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   e95278ec68bf983111a52e38d5e6031c41141858e87e2cb8ef92fdfe17e41a15
  label:    Sensible Soccer: European Champions
  name:     Sensible Soccer - European Champions
  region:   SNSP-8S-UKV
  revision: SPAL-8S-0
  board:    SHVC-1A3M-20
    memory
      type: ROM
      size: 0x80000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   21f4b54ed90de7bb0f5f02d64d11f80ba9f2147197b7a3bd09fc7aa1858ba0f5
  label:    Side Pocket
  name:     Side Pocket
  region:   SNSP-4P-UKV
  revision: SPAL-4P-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   d02f8e6b75f9b9ede20a32b8ec93c06475f18160ced1eb069cd6a3cbbc3cba2e
  label:    Spectre
  name:     Spectre
  region:   SNSP-7Q-UKV
  revision: SPAL-7Q-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0xe0000
      content: Program

game
  sha256:   8c7a8aa1d16aeef31244b016ac951eab0b9ccd46daea61adbe890e5c5daa29c6
  label:    Star Trek: The Next Generation - Future's Past
  name:     Star Trek - The Next Generation - Future's Past
  region:   SNSP-XN-UKV
  revision: SPAL-XN-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   80e7ba7c756c70eedd55ca4548c1965f84f9ef01d3a5ca91a2e5465a6930c49a
  label:    Starwing
  name:     Starwing
  region:   SNSP-FO-UKV
  revision: SPAL-FO-1
  board:    SNSP-1C0N5S-01
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save
      volatile

game
  sha256:   2a25bf4c1a4fc64f9312ddf039f5b3e8949054cbaeb4f86d1ccd9ea2bb779643
  label:    Starwing Competition
  name:     Starwing Competition
  region:   SNSP-FU-UKV
  revision: SPAL-FU-0
  board:    SHVC-1C0N
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save
      volatile

game
  sha256:   b8ee1b5b9deae5c84fa209815515030109cc271b645a18de882aaf1b254cda1f
  label:    Street Fighter II Turbo: Hyper Fighting
  name:     Street Fighter II Turbo - Hyper Fighting
  region:   SNSP-TI-UKV
  revision: SPAL-TI-0
  board:    SHVC-BJ0N-01
    memory
      type: ROM
      size: 0x280000
      content: Program

game
  sha256:   df43b3a4f511401a4d162ee6e7e3b08485533600dc44a29ee0a829b937b144d4
  label:    Striker
  name:     Striker
  region:   SNSP-KE-UKV
  revision: SPAL-KE-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   f1a75578e6711716340bb26ea93bf05d5762bc7da21dbc19576fc65de1e885b5
  label:    Super Aleste
  name:     Super Aleste
  region:   SNSP-AT-UKV
  revision: SPAL-AT-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   e8b1639acd44a536f060d6d8dbcc4ef368279e3e17e1e3862a463d3ebf07ea14
  label:    Super Battletank 2
  name:     Super Battletank 2
  region:   SNSP-2X-UKV
  revision: SPAL-2X-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   8bc56d4d23638ff592715e089dfd697fe7884a388c5ac95e147973bc2ff71e72
  label:    Super International Cricket
  name:     Super International Cricket
  region:   SNSP-ACIP-UKV
  revision: SPAL-ACIP-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   9760fb407282d91005044fb08f9c15dc3e0ae65063a02eedfbbd285566501fd0
  label:    Super Mario All-Stars
  name:     Super Mario All-Stars
  region:   SNSP-4M-UKV
  revision: SPAL-4M-0
  board:    SHVC-2A3M-01
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   12d04359329bd646fc65c62db6121b4a7e4ece68556d68252e81ced421069f4c
  label:    Super Mario Kart
  name:     Super Mario Kart
  region:   SNSP-MK-UKV
  revision: SPAL-MK-0
  board:    SHVC-1K1B-01
    memory
      type: ROM
      size: 0x80000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save
    memory
      type: ROM
      size: 0x1800
      content: Program
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1
    memory
      type: ROM
      size: 0x800
      content: Data
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1
    memory
      type: RAM
      size: 0x200
      content: Data
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1
      volatile
    oscillator
      frequency: 7600000

game
  sha256:   640acb63dae038ad6f0ae65e103416f5a1f84d4a37ddaeeab5046122def774d5
  label:    Super Metroid
  name:     Super Metroid
  region:   SNSP-RI-UKV
  revision: SPAL-RI-0
  board:    SHVC-BA3M-10
    memory
      type: ROM
      size: 0x300000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   3438e78d35634d0ed7f58cb395c77da548fb601248725a18365edaed38a565d5
  label:    Super Punch-Out!!
  name:     Super Punch-Out!!
  region:   SNSP-4Q-UKV
  revision: SPAL-4Q-0
  board:    SHVC-1A3M-30
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   7c7e90fb7c762769219234baf7b5fa6bf574fff7dc63b7134d49ec7c8b38ea7e
  label:    Super R-Type
  name:     Super R-Type
  region:   SNSP-SR-UKV
  revision: SPAL-SR-0
  board:    SHVC-1A0N-02
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   a52b98da8f65bf2210b7d2e931548c672838fa7e44d852f2e3c6f3cd2ba950d6
  label:    Super Street Fighter II: The New Challengers
  name:     Super Street Fighter II - The New Challengers
  region:   SNSP-XW-UKV
  revision: SPAL-XW-0
  board:    SHVC-BJ0N-20
    memory
      type: ROM
      size: 0x400000
      content: Program

game
  sha256:   d92b74678e58360db9b47a7044cedd6c57f191570a5677b1a1bf5e476f92721d
  label:    Terminator 2: Judgment Day
  name:     Terminator 2 - Judgment Day
  region:   SNSP-TP-UKV
  revision: SPAL-TP-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   c0a7e27131a7d8c9ef52a5227329e6de5846c045a9da1f3f84845e3be8e4efba
  label:    Theme Park
  name:     Theme Park
  region:   SNSP-ATQP-UKV
  revision: SPAL-ATQP-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   f899b083b29f34bee62cc022262ab4ff6aad9b16423011faff37f2c21a45fd89
  label:    Total Carnage
  name:     Total Carnage
  region:   SNSP-XC-UKV
  revision: SPAL-XC-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   717fe462a79c298581423c614f62e22dbf6a33e0bf75e691d96848086869418e
  label:    Tuff E Nuff
  name:     Tuff E Nuff
  region:   SNSP-TE-UKV
  revision: SPAL-TE-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

//Super Nintendo (USA)

//database
//  revision: 2020-01-01

game
  sha256:   2ffe8828480f943056fb1ab5c3c84d48a0bf8cbe3ed7c9960b349b59adb07f3b
  label:    3 Ninjas Kick Back
  name:     3 Ninjas Kick Back
  region:   SNS-A3NE-USA
  revision: SNS-A3NE-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   4dd631433c867ba920997fd3add2c838b62e70e06e0ef55c53884b8b68b0dd27
  label:    The 7th Saga
  name:     7th Saga, The
  region:   SNS-EL-USA
  revision: SNS-EL-0
  board:    SHVC-2J3M-01
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   ce164872c4f5814bce04cf0565edcdb5b7969ae95a3b5cd515cfb626b5cde7b3
  label:    Aaahh!!! Real Monsters
  name:     Aaahh!!! Real Monsters
  region:   SNS-ANNE-USA
  revision: SNS-ANNE-0
  board:    MAXI-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   bb83f982961c33b81fefc1f545e18ab572d1c43cf6c241948544f05a1a71f2ba
  label:    ABC Monday Night Football
  name:     ABC Monday Night Football
  region:   SNS-N5-USA
  revision: SNS-N5-0
  board:    SHVC-1A3M-10
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   d07e8802a6d9c777247874e05ec08fce7e0fa1bf122cc1ab9913f7d828e4072b
  label:    ACME Animation Factory
  name:     ACME Animation Factory
  region:   SNS-ACME-USA
  revision: SNS-ACME-0
  board:    SHVC-1A5M-11
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save

game
  sha256:   b8055844825653210d252d29a2229f9a3e7e512004e83940620173c57d8723f0
  label:    ActRaiser
  name:     ActRaiser
  region:   SNS-AR-USA
  revision: SNS-AR-0
  board:    SHVC-1A3B-11
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   71bdd448a30b88725864e55594ebb67a118b1f197a3f9e5dd39dbf23399efa15
  label:    ActRaiser 2
  name:     ActRaiser 2
  region:   SNS-A8-USA
  revision: SNS-A8-0
  board:    SHVC-2J0N-01
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   f59a0a8ed11ea2ba6217b1640e74bab8d8d8161a4585f5ae4a02edd7958ad9a3
  label:    Addams Family Values
  name:     Addams Family Values
  region:   SNS-VY-USA
  revision: SNS-VY-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   e645310d2406ace85523ed91070ee7ff6aa245217267dacb158ae9fc75109692
  label:    The Addams Family
  name:     Addams Family, The
  region:   SNS-AF-USA
  revision: SNS-AF-0
  board:    SHVC-1A0N-01
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   b6957bae7fd97ba681afbf8962fe2138e209649fd88ed9add2d5233178680aaa
  label:    The Addams Family: Pugsley's Scavenger Hunt
  name:     Addams Family, The - Pugsley's Scavenger Hunt
  region:   SNS-AH-USA
  revision: SNS-AH-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   0a8cd5101f849ccd4e40d55fdc4edce914b2825b69eb76ec31cf53b59719e79e
  label:    Advanced Dungeons & Dragons: Eye of the Beholder
  name:     Advanced Dungeons & Dragons - Eye of the Beholder
  region:   SNS-IB-USA
  revision: SNS-IB-0
  board:    SHVC-1A5M-01
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save

game
  sha256:   8083307f3f4b7df9e5bf53d5f25877c2e548f0f677540d4ee62d60ccca3098f8
  label:    The Adventures of Batman & Robin
  name:     Adventures of Batman & Robin, The
  region:   SNS-ABTE-USA
  revision: SNS-ABTE-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   ecd964ae44e61203bc8759cfc6441365bf0c6e7bae6ad2a0fd553d4c7efab71e
  label:    The Adventures of Dr. Franken
  name:     Adventures of Dr. Franken, The
  region:   SNS-6F-USA
  revision: SNS-6F-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   670d898bdcf97d7ca3aab6c2dd1641f1270fcc2a070bbd3028ab413aef2b2ecc
  label:    The Adventures of Kid Kleets
  name:     Adventures of Kid Kleets, The
  region:   SNS-YK-USA
  revision: SNS-YK-0
  board:    SHVC-2A0N-11
    memory
      type: ROM
      size: 0x140000
      content: Program

game
  sha256:   889beb58d2a48a05a6230cabda14555cb030e2e986c0293bdf396e85af5c6798
  label:    The Adventures of Mighty Max
  name:     Adventures of Mighty Max, The
  region:   SNS-AMOE-USA
  revision: SNS-AMOE-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   b70099186d3774355ac5db370240e370c73f9ce5341f6c805cf9f771374b43ae
  label:    The Adventures of Rocky and Bullwinkle and Friends
  name:     Adventures of Rocky and Bullwinkle and Friends, The
  region:   SNS-RZ-USA
  revision: SNS-RZ-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   8049175767eddbc3e21ca5b94ee23fc225f834ccfab4ded30d2e981b0ef73ab6
  label:    Adventures of Yogi Bear
  name:     Adventures of Yogi Bear
  region:   SNS-Y8-USA
  revision: SNS-Y8-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   b737de81315eef8ded7cfd5df6b37aba01d9e6e14566486db7ec83eb0c1aa85e
  label:    Aero Fighters
  name:     Aero Fighters
  region:   SNS-AERE-USA
  revision: SNS-AERE-0
  board:    SHVC-2A0N-11
    memory
      type: ROM
      size: 0x140000
      content: Program

game
  sha256:   18a553dafd4980cc2869180b05f9fdf6e980bf092cc683e498ec6373c9701c6e
  label:    Aero the Acro-Bat
  name:     Aero the Acro-Bat
  region:   SNS-XB-USA
  revision: SNS-XB-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   fc5df5db0a96d39d3df651f63993adf0f5cb5a6b92a36211f3a05d460d92c72d
  label:    Aero the Acro-Bat 2
  name:     Aero the Acro-Bat 2
  region:   SNS-AE2E-USA
  revision: SNS-AE2E-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   d5f0fbeed3774bbccbd769698fc4051487a0a5eb699790a8a094451595600f60
  label:    Aerobiz
  name:     Aerobiz
  region:   SNS-AL-USA
  revision: SNS-AL-0
  board:    SHVC-1A3B-13
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   6b6921d7aba9ba353e96e39c8d79d24eef1b84eb5c7fa54edaa83d2447dcd954
  label:    Aerobiz Supersonic
  name:     Aerobiz Supersonic
  region:   SNS-AG-USA
  revision: SNS-AG-0
  board:    SHVC-1A3M-21
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   1f5738552c51de25ffe8aa44ff396c1ab238435296f1e8f99f8cf335483c03d5
  label:    Air Cavalry
  name:     Air Cavalry
  region:   SNS-ACCE-USA
  revision: SNS-ACCE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   69c5805ad0494703e7d636d3d40d615d33e79bebef9d2cdb4a23b73d44c7b6f9
  label:    Air Strike Patrol
  name:     Air Strike Patrol
  region:   SNS-4A-USA
  revision: SNS-4A-0
  board:    SHVC-1A3M-21
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   0099bdb56e4190f81440c8c29395ecc15d78eeabfc38a93dca4773817d6f720f
  label:    Al Unser Jr.'s Road to the Top
  name:     Al Unser Jr.'s Road to the Top
  region:   SNS-AUJE-USA
  revision: SNS-AUJE-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   aa768b8b00123717c8d49f2c6731cdbfd80ab6a54128bae7594e93f45e38a19e
  label:    Aladdin
  name:     Aladdin
  region:   SNS-RJ-USA
  revision: SNS-RJ-0
  board:    MAXI-1A0N-30
    memory
      type: ROM
      size: 0x140000
      content: Program

game
  sha256:   e637b8241d55ee334a3452888df5e30d72c520dbb55c498db1a984438bab3e55
  label:    Alien 3
  name:     Alien 3
  region:   SNS-A3-USA
  revision: SNS-A3-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   05eb897d7696555790591c431c9d55a43ff9a1c12162443c17c5fcddfa5eb3c5
  label:    Alien vs. Predator
  name:     Alien vs. Predator
  region:   SNS-AP-USA
  revision: SNS-AP-0
  board:    SHVC-YA0N-01
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   dc9cefb4dd50dce2e9d626ac71d4a06306516cba4bc784c90e4a30fe4e7ff4ef
  label:    American Gladiators
  name:     American Gladiators
  region:   SNS-AA-USA
  revision: SNS-AA-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   6931a3eacb7ab3c2f2fb57ba7d59c6da56fe6c7d60484cebec9332e6caca3ae9
  label:    An American Tail: Fievel Goes West
  name:     American Tail, An - Fievel Goes West
  region:   SNS-9W-USA
  revision: SNS-9W-0
  board:    SHVC-1J0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   9abb426bac62e03e4437f5a9a8455c3000042f339125cc60ae29382ae89d8493
  label:    Andre Agassi Tennis
  name:     Andre Agassi Tennis
  region:   SNS-7A-USA
  revision: SNS-7A-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   626f1fe52b0df0f8ede23f0062cd842321c8dabf14aefdca12e526876ecf383a
  label:    Animaniacs
  name:     Animaniacs
  region:   SNS-ANCE-USA
  revision: SNS-ANCE-0
  board:    MJSC-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   31569bef662bc438194726967970bf19f504101060763cbd649350362fb6ef2f
  label:    Arcade's Greatest Hits: The Atari Collection 1
  name:     Arcade's Greatest Hits - The Atari Collection 1
  region:   SNS-AW7E-USA
  revision: SNS-AW7E-0
  board:    MAXI-1A0N-30
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   aac9d6f2b8408e4bbdc83ebbba755428caf8021fefbfac7220fb4c772abd9944
  label:    Arcana
  name:     Arcana
  region:   SNS-RF-USA
  revision: SNS-RF-0
  board:    SHVC-1A3B-13
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   0f474dafe5a26f3dea491d18073dd490d2f1f91313a7e91086565510d38d9a09
  label:    Ardy Light Foot
  name:     Ardy Light Foot
  region:   SNS-A9-USA
  revision: SNS-A9-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   14d3ece30898587eda20c661a4a55ec595ba6352ca1f0bfc177542aa0eef0039
  label:    Arkanoid: Doh It Again!
  name:     Arkanoid - Doh It Again!
  region:   SNS-A6-USA
  revision: SNS-A6-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   55e57c5e012583ff0fafd1aee16b3f8269ee2b34fe10f10b93ba0dde72f2b78d
  label:    Art of Fighting
  name:     Art of Fighting
  region:   SNS-RW-USA
  revision: SNS-RW-0
  board:    SHVC-1J0N-01
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   2431f8dc067ba27c6c3a846929f3deac6a45aa53a9a9ac20ede8ec5ca6854ea2
  label:    Axelay
  name:     Axelay
  region:   SNS-AX-USA
  revision: SNS-AX-0
  board:    SHVC-1A0N-02
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   a3213e40d7abbc25a5b909642433103b06d8f90500c930bf64093ade0329da78
  label:    Ballz 3D
  name:     Ballz 3D
  region:   SNS-ABZE-USA
  revision: SNS-ABZE-0
  board:    SHVC-1K0N-01
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: ROM
      size: 0x1800
      content: Program
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1B
    memory
      type: ROM
      size: 0x800
      content: Data
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1B
    memory
      type: RAM
      size: 0x200
      content: Data
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1B
      volatile
    oscillator
      frequency: 7600000

game
  sha256:   865919b25a9d241c907bcda18b380e3c704f33f4997ad44559046f0f08c4968b
  label:    Barbie Super Model
  name:     Barbie Super Model
  region:   SNS-8L-USA
  revision: SNS-8L-0
  board:    SHVC-YA0N-01
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   fe1ad128313b2b9a47f89cf0d95d4c0cc2cb35a817ac5d915ee6c4d98d47d675
  label:    Barkley Shut Up and Jam!
  name:     Barkley Shut Up and Jam!
  region:   SNS-U5-USA
  revision: SNS-U5-0
  board:    SHVC-2A0N-11
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   c670e16c076f6d92ba581b78769b604e79ad567c04c647dac557f45a48c3448f
  label:    Bass Masters Classic
  name:     Bass Masters Classic
  region:   SNS-ABAE-USA
  revision: SNS-ABAE-0
  board:    SHVC-2A0N-20
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   db1ac03cc8b7daaa812da239029bcf999b30b2afe1c03d51f7ae849a796617ea
  label:    Bass Masters Classic: Pro Edition
  name:     Bass Masters Classic - Pro Edition
  region:   SNS-A9BE-USA
  revision: SNS-A9BE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   e2be173c77bd1957787be36d13334f655e14d32dad99cacb0fd5e5fc65d96fa1
  label:    Bassin's Black Bass
  name:     Bassin's Black Bass
  region:   SNS-AB2E-USA
  revision: SNS-AB2E-0
  board:    SHVC-1A3M-21
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   e36aaba64be016cabc33a2dcf88913341e2edacc722e2a1ebe04eccda2a5d6e7
  label:    Batman Forever
  name:     Batman Forever
  region:   SNS-A3BE-USA
  revision: SNS-A3BE-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x300000
      content: Program

game
  sha256:   f8d5c51f74df33edc827fbf8df7aab70160770ab0a896db6e59438ad9208cc6e
  label:    Batman Returns
  name:     Batman Returns
  region:   SNS-BJ-USA
  revision: SNS-BJ-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   983022203546e031773db0d1af7552c489187954882843f13ff123f09064c1d3
  label:    Battle Blaze
  name:     Battle Blaze
  region:   SNS-BZ-USA
  revision: SNS-BZ-0
  board:    SHVC-YA0N-01
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   d19113964c4d268e986018e256e9358cde9a23a05e6053b54c0f2d950dcdf395
  label:    Battle Cars
  name:     Battle Cars
  region:   SNS-4B-USA
  revision: SNS-4B-0
  board:    SHVC-YA0N-01
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   32f42fda0667d9435a2de84c7ce7b067bcbab1164f8f6d837992ad6cfac4f8de
  label:    Battle Clash
  name:     Battle Clash
  region:   SNS-BT-USA
  revision: SNS-BT-0
  board:    SHVC-1A0N-02
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   6e436020d967d35a0f7ce43e38e83a616b70696ae7d35b37cd56601cfb3822ba
  label:    Battle Grand Prix
  name:     Battle Grand Prix
  region:   SNS-BG-USA
  revision: SNS-BG-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   c7f0269498d190e4fd0f6f880a148fbe3713cd3a632083bac1e5bd18f8172371
  label:    Battletoads & Double Dragon
  name:     Battletoads & Double Dragon
  region:   SNS-UL-USA
  revision: SNS-UL-0
  board:    MAXI-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   b0dbd4d7e5689c32234e80b0c5362ef67c425ab72d6ddb49d1cb1133ef630ef7
  label:    Battletoads in Battlemaniacs
  name:     Battletoads in Battlemaniacs
  region:   SNS-NX-USA
  revision: SNS-NX-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   0b0f991da7ce4b3c2974d6adf62191ec554db9793c5def14cdfb62b7ae28cc98
  label:    Bazooka Blitzkrieg
  name:     Bazooka Blitzkrieg
  region:   SNS-BY-USA
  revision: SNS-BY-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   d7271ca08400bbf5ae165b0aaa6e8a8a1b266f72e6e0ae10aae529732a472f7c
  label:    Beauty and the Beast
  name:     Beauty and the Beast
  region:   SNS-EW-USA
  revision: SNS-EW-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   15d4fc90cb202a9391718cd40b9f0384165aef03018ed932540e8f7c18b397dd
  label:    Beavis and Butt-Head
  name:     Beavis and Butt-Head
  region:   SNS-ABUE-USA
  revision: SNS-ABUE-0
  board:    MAXI-1A0N-30
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   4958eda26f2419f473449017c64121caee5e49c480ffa205422e7dd45cd23e31
  label:    Bebe's Kids
  name:     Bebe's Kids
  region:   SNS-6B-USA
  revision: SNS-6B-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   4d22279e534848012e0f1595468687742ae18cabc3fe44eeef938bc3a4dd08bf
  label:    Beethoven: The Ultimate Canine Caper!
  name:     Beethoven - The Ultimate Canine Caper!
  region:   SNS-2V-USA
  revision: SNS-2V-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   e4e9beaeeb3e968af772d1c4c9e4c1b4dfdba4e47c0205b458e1ab3a62a96060
  label:    Best of the Best: Championship Karate
  name:     Best of the Best - Championship Karate
  region:   SNS-BE-USA
  revision: SNS-BE-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   4cb601168c91fa0608c16a8cf2f292d991c6a5615d51861dee2f9b91c8d6bb19
  label:    Big Sky Trooper
  name:     Big Sky Trooper
  region:   SNS-AB9E-USA
  revision: SNS-AB9E-0
  board:    SHVC-1A1M-20
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   91ba5691dea3cdf103177ae5779110fc372fce8229cf91f263073667e7a8b5b7
  label:    Biker Mice from Mars
  name:     Biker Mice from Mars
  region:   SNS-ABME-USA
  revision: SNS-ABME-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   6fa6b8a8804ff6544bdedf94339a86ba64ce0b6dbf059605abb1cd6f102d3483
  label:    Bill Laimbeer's Combat Basketball
  name:     Bill Laimbeer's Combat Basketball
  region:   SNS-CB-USA
  revision: SNS-CB-0
  board:    SHVC-1A3B-12
    memory
      type: ROM
      size: 0x80000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   ec2d91e238c26a5ddf7067d104b3b3e2eaee89255377e1eb6c4df8f301300e64
  label:    Bill Walsh College Football
  name:     Bill Walsh College Football
  region:   SNS-7F-USA
  revision: SNS-7F-0
  board:    SHVC-1J0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   de1de85ad549a6aaf0431cceb47cbd07e1f6e81f9e16fd62575305e2c1f06240
  label:    Bio Metal
  name:     Bio Metal
  region:   SNS-BV-USA
  revision: SNS-BV-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   328c8f57e2ea371f6fd5b8a9834c56e35eb3bfe710502dd80f370739f9ccb7e1
  label:    Blackthorne
  name:     Blackthorne
  region:   SNS-6Z-USA
  revision: SNS-6Z-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   0d4e0d134396bd1c7254cdc1da356eb944ca14910b6690f484a75a9c3a8106e7
  label:    BlaZeon: The Bio-Cyborg Challenge
  name:     BlaZeon - The Bio-Cyborg Challenge
  region:   SNS-BL-USA
  revision: SNS-BL-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   99f40f06fa4dbeeea4fe67d2de5b4c1bf301bedac1958ba1c239dcaf39b0a998
  label:    The Blues Brothers
  name:     Blues Brothers, The
  region:   SNS-B6-USA
  revision: SNS-B6-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   9e6ebebcf14609c2a38a5f4409d0c8c859949cded70c5b6fd16fd15d9983d9d3
  label:    B.O.B.
  name:     BOB
  region:   SNS-B4-USA
  revision: SNS-B4-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   854d2492d1cb059749bb0904ca5f92a5eeec09167abf84f7cca4023b1819e4f0
  label:    Bonkers
  name:     Bonkers
  region:   SNS-ABNE-USA
  revision: SNS-ABNE-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   8f131182b286bd87f12cf4f00453336538ce690d0e1f0972ac0be98df4d48987
  label:    Boogerman: A Pick and Flick Adventure
  name:     Boogerman - A Pick and Flick Adventure
  region:   SNS-AB4E-USA
  revision: SNS-AB4E-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x300000
      content: Program

game
  sha256:   e67940a2106c1507f3a8d38790f263bbbf814578ebf3dbc4e3eb6007d310793c
  label:    Boxing Legends of the Ring
  name:     Boxing Legends of the Ring
  region:   SNS-LL-USA
  revision: SNS-LL-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   f4666355e7fea434843dc6d5119673bd6c23e69b884aac0b382ff036997e52b5
  label:    Brain Lord
  name:     Brain Lord
  region:   SNS-3B-USA
  revision: SNS-3B-0
  board:    SHVC-2J3M-11
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   9885ca148d32c4df6230642bcfa153f7e51b9559415042a831db14d07b3e6c3d
  label:    The Brainies
  name:     Brainies, The
  region:   SNS-B7-USA
  revision: SNS-B7-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   bbde8b46c7262f9d4a5b3926a00850cb00b4f7711f6421f0adf4e2b0c847a5d6
  label:    Bram Stoker's Dracula
  name:     Bram Stoker's Dracula
  region:   SNS-5D-USA
  revision: SNS-5D-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   130a74e76369b0ec4d6378a014550921433f1ae1ac1dddffb51f77c9f21a818f
  label:    Brandish
  name:     Brandish
  region:   SNS-QF-USA
  revision: SNS-QF-0
  board:    SHVC-2J5M-01
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save

game
  sha256:   044b61613ed66eae08abd5fa5dcd13b24aab11a942e3309cdff624d198c47440
  label:    Brawl Brothers
  name:     Brawl Brothers
  region:   SNS-RE-USA
  revision: SNS-RE-0
  board:    SHVC-2A0N-10
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   aad8c9be1b7a9662256b0c3d76f5b7a273bcd497aa838232d307e9f2e80cf699
  label:    BreakThru!
  name:     BreakThru!
  region:   SNS-ABXE-USA
  revision: SNS-ABXE-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   cbc496a7879ba78f32c51c3df4ba1a1a42f17d78d48a39ea9c709d1ad18cb8df
  label:    Breath of Fire
  name:     Breath of Fire
  region:   SNS-BF-USA
  revision: SNS-BF-0
  board:    SHVC-2A3M-11
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   fede9d4aec8c35ed11e2868c3c517bce53ee3e6af724085c92500e99e43e63de
  label:    Breath of Fire II
  name:     Breath of Fire II
  region:   SNS-AF2E-USA
  revision: SNS-AF2E-0
  board:    SHVC-1J3M-20
    memory
      type: ROM
      size: 0x300000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   99450b45ccc70a1ed387f968c8f863a3c7f6a4b86809e841c25a71e1e904ac61
  label:    Brett Hull Hockey
  name:     Brett Hull Hockey
  region:   SNS-5Y-USA
  revision: SNS-5Y-0
  board:    SHVC-2A0N-01
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   a427db4860cb5140935f263ba872fe14949c3548db644fed46b2edf3dff3d4a8
  label:    Brett Hull Hockey '95
  name:     Brett Hull Hockey '95
  region:   SNS-ABHE-USA
  revision: SNS-ABHE-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   f2bc4cb460dfc5d5288065a2f529190b72e69d4e02634246086244c20f30521c
  label:    Bronkie the Bronchiasaurus
  name:     Bronkie the Bronchiasaurus
  region:   SNS-AB6E-USA
  revision: SNS-AB6E-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   ee00c57ddfa9b967d1857acb518df039ba73055bdebe78db014de0f5da262fd9
  label:    Brunswick World Tournament of Champions
  name:     Brunswick World Tournament of Champions
  region:   SNS-AWUE-USA
  revision: SNS-AWUE-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   9e3ad5e521e759a2e2260ea8072eb3314b6fcf67a3b7247357a5de9bcb24eeaa
  label:    Brutal: Paws of Fury
  name:     Brutal - Paws of Fury
  region:   SNS-ABLE-USA
  revision: SNS-ABLE-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   2357d344af77d25dda030520ce203045fd9060f83e3b9609a228dba859d9017b
  label:    Bubsy II
  name:     Bubsy II
  region:   SNS-ABBE-USA
  revision: SNS-ABBE-0
  board:    SHVC-1J0N-10
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   811cbc3287c0959e8eb242e817684d36de664ebebc5873a1fa9958693857c438
  label:    Bubsy in Claws Encounters of the Furred Kind
  name:     Bubsy in Claws Encounters of the Furred Kind
  region:   SNS-UY-USA
  revision: SNS-UY-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   49020695a017acc3dfadea97a60e28609e583571f69c5abeb3c6b1c2db8113fa
  label:    Bugs Bunny: Rabbit Rampage
  name:     Bugs Bunny - Rabbit Rampage
  region:   SNS-R7-USA
  revision: SNS-R7-0
  board:    SHVC-2A0N-11
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   ba4f31353e0e1233b574391ad97a80901d7de212e2c55d7be2af11a9a57c8225
  label:    Bulls vs. Blazers and the NBA Playoffs
  name:     Bulls vs. Blazers and the NBA Playoffs
  region:   SNS-BU-USA
  revision: SNS-BU-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   d8b3d9267470adb8ef2d197ade44476e504c0823f8fe9d2cf2883a03aa75bd49
  label:    Bulls vs. Blazers and the NBA Playoffs
  name:     Bulls vs. Blazers and the NBA Playoffs
  region:   SNS-BU-USA
  revision: SNS-BU-1
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   d6f6c30732dae8d00cd83628c3156acbdf26f99df701f779522e21de74dae5fe
  label:    Bust-a-Move
  name:     Bust-a-Move
  region:   SNS-AYKE-USA
  revision: SNS-AYKE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   a8294d449bbb8370816efe0374704e8af20dbbde9c19afe969d898528bc293d0
  label:    Cacoma Knight in Bizyland
  name:     Cacoma Knight in Bizyland
  region:   SNS-CC-USA
  revision: SNS-CC-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   75993076c8773e7c7d555ef290be10590def40ca7b83307b8dc86556b04a6565
  label:    Cal Ripken Jr. Baseball
  name:     Cal Ripken Jr. Baseball
  region:   SNS-CJ-USA
  revision: SNS-CJ-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   19f4a1f08aa55ff3cc8ff7ae60ffe03f0e436e8d8901455f1311f2276497a492
  label:    California Games II
  name:     California Games II
  region:   SNS-C2-USA
  revision: SNS-C2-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   6cdec3cb9c91aa23feb13005f874bda580c2f548302874491a31951031c9dbbd
  label:    Cannondale Cup
  name:     Cannondale Cup
  region:   SNS-ASCE-USA
  revision: SNS-ASCE-0
  board:    SHVC-2A1M-01
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   9590110a990e90f525d5c8d70fc2a3da10879378003173b6761afb8bf042ee0d
  label:    Capcom's MVP Football
  name:     Capcom's MVP Football
  region:   SNS-NL-USA
  revision: SNS-NL-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   fd5761f9dd1f2b87ad11df6085046d0dfcdc3a79139263e47b0cff707966ba51
  label:    Capcom's Soccer Shootout
  name:     Capcom's Soccer Shootout
  region:   SNS-JL-USA
  revision: SNS-JL-0
  board:    SHVC-2A0N-11
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   2a117951adcfbc4298763673a834d502c3f7a3964db1e59650f113c07bb831fb
  label:    Captain America and The Avengers
  name:     Captain America and The Avengers
  region:   SNS-6A-USA
  revision: SNS-6A-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   d9b7f9356be0780f0037093a86ef8450f15e569cbd3680073d1cd345dfadb709
  label:    Captain Commando
  name:     Captain Commando
  region:   SNS-QM-USA
  revision: SNS-QM-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   8784614896e2b3e8d98c8166613ca5d2329643795a4dc107791c58c6c51e1268
  label:    Captain Novolin
  name:     Captain Novolin
  region:   SNS-CP-USA
  revision: SNS-CP-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   ee5fc27dd19a2ecb3c3c7c73d558a18ffd5ff365710c18b88150e277f08d587e
  label:    Carrier Aces
  name:     Carrier Aces
  region:   SNS-ACAE-USA
  revision: SNS-ACAE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   b9b982cd8f91c51089d49b550f11882b1ee785ebddcb7355cfc465916d61a042
  label:    Casper
  name:     Casper
  region:   SNS-AXCE-USA
  revision: SNS-AXCE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   367725a149a471411e4f72ad77603b61fb101c9cab4521be5647e13708cc97ba
  label:    Castlevania: Dracula X
  name:     Castlevania - Dracula X
  region:   SNS-ADZE-USA
  revision: SNS-ADZE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   32008c429eafe7eb59aff7a89f77e1a267f02f9061ff8830ade7b99081e27f7c
  label:    Champions World Class Soccer
  name:     Champions World Class Soccer
  region:   SNS-8W-USA
  revision: SNS-8W-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   e6c4f468b39f2dea3436b63758db8ac9b29731357b982ec373334a36f202623f
  label:    Championship Pool
  name:     Championship Pool
  region:   SNS-5P-USA
  revision: SNS-5P-0
  board:    MAXI-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   fdd763dffc6fb38975d4c6d6e3123f9738122781b7d13e1fc7f9820464cbaeb5
  label:    Championship Soccer '94
  name:     Championship Soccer '94
  region:   SNS-67-USA
  revision: SNS-67-0
  board:    SHVC-1A3M-20
    memory
      type: ROM
      size: 0x80000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   aa69d4e19c2eb206fe88eba65994c830256c220e5506f59824aefa0a75dd44d5
  label:    Chavez
  name:     Chavez
  region:   SNS-ZV-USA
  revision: SNS-ZV-0
  board:    SHVC-1A1M-01
    memory
      type: ROM
      size: 0x80000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   39de64101cf7f25863ce55e03a27d13422ea572ee996746578b5936fea80228b
  label:    Chavez II
  name:     Chavez II
  region:   SNS-AC2E-USA
  revision: SNS-AC2E-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   ee0e51d922d1cf8abe3dfc6b0d84a988a6635dc96b2a96962007c41aaa542774
  label:    The Chessmaster
  name:     Chessmaster, The
  region:   SNS-CH-USA
  revision: SNS-CH-0
  board:    SHVC-1A0N-01
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   c7e7df8932bf0056aa530f3dc3c913c1171a359af4c197094c2b972946dc6051
  label:    Chester Cheetah: Too Cool to Fool
  name:     Chester Cheetah - Too Cool to Fool
  region:   SNS-CE-USA
  revision: SNS-CE-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   21a2aa488cb8140ca318f7d1f513103d14e758181aa336a594097d32ba0a7587
  label:    Chester Cheetah: Wild, Wild Quest
  name:     Chester Cheetah - Wild, Wild Quest
  region:   SNS-7C-USA
  revision: SNS-7C-0
  board:    SHVC-2A0N-11
    memory
      type: ROM
      size: 0x140000
      content: Program

game
  sha256:   9a064b67f522b75b82d0857519c0e33b4dbbe494c2ef79a44fdc913d605d0b26
  label:    Choplifter III
  name:     Choplifter III
  region:   SNS-3C-USA
  revision: SNS-3C-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   06d1c2b06b716052c5596aaa0c2e5632a027fee1a9a28439e509f813c30829a9
  label:    Chrono Trigger
  name:     Chrono Trigger
  region:   SNS-ACTE-USA
  revision: SNS-ACTE-0
  board:    SHVC-1J3M-20
    memory
      type: ROM
      size: 0x400000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   63ab79e86ea13e2cf9bb67aec971febb68450db9865b00b5f412610653822393
  label:    Chuck Rock
  name:     Chuck Rock
  region:   SNS-CK-USA
  revision: SNS-CK-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   8b7525b2aa30cbea9e3deee601dd26e0100b8169c1948f19866be15cae0ac00d
  label:    Clay Fighter
  name:     Clay Fighter
  region:   SNS-8C-USA
  revision: SNS-8C-0
  board:    SHVC-1J0N-01
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   ea52b39a8e1ea15bfad6b92883c9a5fe744cecd7c0e175aa3bd40280cf7a966e
  label:    Clay Fighter: Tournament Edition
  name:     Clay Fighter - Tournament Edition
  region:   SNS-7E-USA
  revision: SNS-7E-0
  board:    SHVC-BJ0N-01
    memory
      type: ROM
      size: 0x300000
      content: Program

game
  sha256:   2d40c86bc19d85555bf2672acf515b04dbf56a6a59b29ad503e672310b0fae3b
  label:    Clay Fighter 2: Judgment Clay
  name:     Clay Fighter 2 - Judgment Clay
  region:   SNS-ACZE-USA
  revision: SNS-ACZE-0
  board:    SHVC-BJ0N-20
    memory
      type: ROM
      size: 0x300000
      content: Program

game
  sha256:   6d9cd7f0cda3c0a82ed3f6a92cbbba4fe8365438e0a7867ad1cae2044d1738eb
  label:    Claymates
  name:     Claymates
  region:   SNS-Y5-USA
  revision: SNS-Y5-0
  board:    SHVC-1J0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   e5980b990605a9c91fa89101c440b2ec9993329296ba09a9538042d724a080fb
  label:    Cliffhanger
  name:     Cliffhanger
  region:   SNS-6C-USA
  revision: SNS-6C-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   03f6c69aef92d36b5ea25a6023368da0e1da9fa160e8316ebd533d4f358ffacf
  label:    Clue
  name:     Clue
  region:   SNS-CL-USA
  revision: SNS-CL-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   5536cea2da39f2572abe3b0fcf71f8fcd981376b470b174969772aae4a7a1845
  label:    College Football USA '97
  name:     College Football USA '97
  region:   SNS-AC7E-USA
  revision: SNS-AC7E-0
  board:    SHVC-1A3M-30
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   b0be35a0d5e500f4fffca5f2940e0ec52c81ce99dacd773c3ca9cf92f592d943
  label:    College Slam
  name:     College Slam
  region:   SNS-ANYE-USA
  revision: SNS-ANYE-0
  board:    SHVC-1A3M-30
    memory
      type: ROM
      size: 0x300000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   c88a882ad72dfa07a9b1eb8a2183aa10057d60877a02edf90cf2cd8c78abe65e
  label:    The Combatribes
  name:     Combatribes, The
  region:   SNS-CR-USA
  revision: SNS-CR-0
  board:    SHVC-2A0N-01
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   26e09f5bc2bde28d57aeca0bf5be7f7fb8e3b3887af975bcbf2e6f29b07df56f
  label:    Congo's Caper
  name:     Congo's Caper
  region:   SNS-J2-USA
  revision: SNS-J2-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   a93ea87fc835c530b5135c5294433d15eef6dbf656144b387e89ac19cf864996
  label:    Contra III: The Alien Wars
  name:     Contra III - The Alien Wars
  region:   SNS-CS-USA
  revision: SNS-CS-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   c7d622391f7699fb0dc6367e141c894e700cc9bd8abca69f36785e7bc2f42a49
  label:    Cool Spot
  name:     Cool Spot
  region:   SNS-C8-USA
  revision: SNS-C8-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   9674cc269d89a52d1719a487b44acf004fb777cbd58d76b19a2cd25749728215
  label:    Cool World
  name:     Cool World
  region:   SNS-CD-USA
  revision: SNS-CD-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   7c722f9941957630467c1784d0eb3f92fbfc0a2a1da3c8f5c27f593eca2a5a04
  label:    CutThroat Island
  name:     CutThroat Island
  region:   SNS-AC8E-USA
  revision: SNS-AC8E-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   c4ae2797fac2586b8640064be6398f2b4f2b3158a07f26c66912b29f7fd197de
  label:    Cyber Spin
  name:     Cyber Spin
  region:   SNS-CF-USA
  revision: SNS-CF-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   ad31b94ce928ecff605e2b89082154671691509e95d38370ed381437f2c36698
  label:    Cybernator
  name:     Cybernator
  region:   SNS-AV-USA
  revision: SNS-AV-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   337e643d3e63915de06429992f306e8d2b73aed6849b795f9c855c2d03c18180
  label:    D-Force
  name:     D-Force
  region:   SNS-DF-USA
  revision: SNS-DF-0
  board:    SHVC-1A0N-01
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   4068add412571bd85adac32dff9835e4a4886077d752adb104fee3908e42b7ef
  label:    Daffy Duck: The Marvin Missions
  name:     Daffy Duck - The Marvin Missions
  region:   SNS-YF-USA
  revision: SNS-YF-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   ceb470157576eac3b8b8c16e8ab6b59672409681ffb4232e4ec39dd0cb37ef91
  label:    Darius Twin
  name:     Darius Twin
  region:   SNS-DT-USA
  revision: SNS-DT-0
  board:    SHVC-1A0N-01
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   e6efb6361af04963f22c772f879a466543f56b3b6a084204fef2dcb4659d82d9
  label:    David Crane's Amazing Tennis
  name:     David Crane's Amazing Tennis
  region:   SNS-ZT-USA
  revision: SNS-ZT-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   300c1937e4b68108302e9b0f49974d1ec6b6c45dd8da69dddc19443f9562ecf4
  label:    The Death and Return of Superman
  name:     Death and Return of Superman, The
  region:   SNS-9D-USA
  revision: SNS-9D-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   752d24fab240f4dd1dfbfea5ec83438998316806ad44488bf8c84430ca5a2cd0
  label:    Demolition Man
  name:     Demolition Man
  region:   SNS-AD6E-USA
  revision: SNS-AD6E-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   18d40a807d5f88c5b6a1ad849eec7e0f189225d9a1586037c850f6680b5844de
  label:    Demon's Crest
  name:     Demon's Crest
  region:   SNS-3Z-USA
  revision: SNS-3Z-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   a362033d0d7e754d79202b255679186ad799b9e784719614b913ec8c9857ae33
  label:    Dennis the Menace
  name:     Dennis the Menace
  region:   SNS-4D-USA
  revision: SNS-4D-0
  board:    SHVC-YA0N-01
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   606abf536440173ae36466db360c7db6b474beb7a105c8a62bc74a54cbe1c38b
  label:    Desert Strike: Return to the Gulf
  name:     Desert Strike - Return to the Gulf
  region:   SNS-RG-USA
  revision: SNS-RG-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   9c4721339a197185d61e0697ea0149db143a27ddb2f57ebd398f18bcf4d7724b
  label:    Dig & Spike Volleyball
  name:     Dig & Spike Volleyball
  region:   SNS-VH-USA
  revision: SNS-VH-0
  board:    SHVC-1A3M-20
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   7dbfc44d28a46e6d399628e43086aa9fd0b2abeda4c108751a5ad91c102c3aaf
  label:    DinoCity
  name:     DinoCity
  region:   SNS-DW-USA
  revision: SNS-DW-0
  board:    SHVC-1A0N-02
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   c5474de3592e2a99aac0e5511516658f9f0b50167c754a018409842ec35bcd45
  label:    Dirt Trax FX
  name:     Dirt Trax FX
  region:   SNS-AF9E-USA
  revision: SNS-AF9E-0
  board:    SHVC-1CA0N6S-01
    memory
      type: ROM
      size: 0x80000
      content: Program
    memory
      type: RAM
      size: 0x10000
      content: Save
      volatile
    oscillator
      frequency: 21440000

game
  sha256:   fa8cacf5bbfc39ee6bbaa557adf89133d60d42f6cf9e1db30d5a36a469f74d15
  label:    Donkey Kong Country
  name:     Donkey Kong Country
  region:   SNS-8X-USA
  revision: SNS-8X-0
  board:    SHVC-1J1M-11
    memory
      type: ROM
      size: 0x400000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   df2644d435330192a13768cc1f79c5aa3084a64217a5250c6dd4ffdbe2175be4
  label:    Donkey Kong Country
  name:     Donkey Kong Country
  region:   SNS-8X-USA
  revision: SNS-8X-1
  board:    SHVC-1J1M-11
    memory
      type: ROM
      size: 0x400000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   628147468c3539283197f58f03b94df49758a332831857481ea9cc31645f0527
  label:    Donkey Kong Country
  name:     Donkey Kong Country
  region:   SNS-8X-USA
  revision: SNS-8X-2
  board:    SHVC-1J1M-20
    memory
      type: ROM
      size: 0x400000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   07ff03fa8c8e31d2f8277ef2a9785022edebf7f79b694c66a00c66d8e563bce5
  label:    Donkey Kong Country: Competition Cartridge
  name:     Donkey Kong Country - Competition Cartridge
  region:   SNS-8E-USA
  revision: SNS-8E-0
  board:    SHVC-1J1M-20
    memory
      type: ROM
      size: 0x400000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   35421a9af9dd011b40b91f792192af9f99c93201d8d394026bdfb42cbf2d8633
  label:    Donkey Kong Country 2: Diddy's Kong Quest
  name:     Donkey Kong Country 2 - Diddy's Kong Quest
  region:   SNS-ADNE-USA
  revision: SNS-ADNE-0
  board:    SHVC-1J1M-20
    memory
      type: ROM
      size: 0x400000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   b79c2bb86f6fc76e1fc61c62fc16d51c664c381e58bc2933be643bbc4d8b610c
  label:    Donkey Kong Country 2: Diddy's Kong Quest
  name:     Donkey Kong Country 2 - Diddy's Kong Quest
  region:   SNS-ADNE-USA
  revision: SNS-ADNE-1
  board:    SHVC-BJ1M-20
    memory
      type: ROM
      size: 0x400000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   2277a2d8dddb01fe5cb0ae9a0fa225d42b3a11adccaeafa18e3c339b3794a32b
  label:    Donkey Kong Country 3: Dixie Kong's Double Trouble
  name:     Donkey Kong Country 3 - Dixie Kong's Double Trouble
  region:   SNS-A3CE-USA
  revision: SNS-A3CE-0
  board:    SHVC-1J1M-20
    memory
      type: ROM
      size: 0x400000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   d45e26eb10c323ecd480e5f2326b223e29264c3adde67f48f0d2791128e519e8
  label:    Doom
  name:     Doom
  region:   SNS-AD8E-USA
  revision: SNS-AD8E-0
  board:    SHVC-1CB0N7S-01
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x10000
      content: Save
      volatile
    oscillator
      frequency: 21440000

game
  sha256:   bb915b286b33842e39e9022366169233a4041515c7ecc60ac428420b28e48dd5
  label:    Doomsday Warrior
  name:     Doomsday Warrior
  region:   SNS-DM-USA
  revision: SNS-DM-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   b32aa9cbf8f6baacc84f6418fa6fbc33b9ce71458fecc91275aafdbf6f743a99
  label:    Double Dragon V: The Shadow Falls
  name:     Double Dragon V - The Shadow Falls
  region:   SNS-5E-USA
  revision: SNS-5E-0
  board:    SHVC-BJ0N-01
    memory
      type: ROM
      size: 0x300000
      content: Program

game
  sha256:   d98d7da1e7636e067563e2e480d7dfbc013b7e9bdf1329fd61c5cacac0293e1d
  label:    Dragon: The Bruce Lee Story
  name:     Dragon - The Bruce Lee Story
  region:   SNS-AD5E-USA
  revision: SNS-AD5E-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   a3b1cae3fe55fe52c035ab122e7f850640b0935524496d45b1915ca3c8a934f4
  label:    Dragon View
  name:     Dragon View
  region:   SNS-ADVE-USA
  revision: SNS-ADVE-0
  board:    SHVC-1A3M-21
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   49a1f9f5e6084b3daa1b13ab5a37ebe8bd3cf20e1c7429fbf722298092893e81
  label:    Dragon's Lair
  name:     Dragon's Lair
  region:   SNS-DI-USA
  revision: SNS-DI-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   74910ce01d19d25cb97a243a51f21c3d522f02fb116682f60440da3292bb14f7
  label:    Drakkhen
  name:     Drakkhen
  region:   SNS-DK-USA
  revision: SNS-DK-0
  board:    SHVC-1A3B-11
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   1a79d51a2ad7dd4848205a07ff8e5d873b155dc420de5e52158c9bab935e05c3
  label:    Dream T.V.
  name:     Dream TV
  region:   SNS-VE-USA
  revision: SNS-VE-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   03d25227fb88899d0f3234c4a3f76f1dbe8d582cb6429454fd6f1c4cf14d5c6e
  label:    Dungeon Master
  name:     Dungeon Master
  region:   SNS-V2-USA
  revision: SNS-V2-0
  board:    SHVC-1B5B-02
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save
    memory
      type: ROM
      size: 0x1800
      content: Program
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP2
    memory
      type: ROM
      size: 0x800
      content: Data
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP2
    memory
      type: RAM
      size: 0x200
      content: Data
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP2
      volatile
    oscillator
      frequency: 7600000

game
  sha256:   0408e3d9f2259044344a3bfbd7a7ca3c3427f82108fbecd6e5c4c41e80cd303b
  label:    Earth Defense Force
  name:     Earth Defense Force
  region:   SNS-ED-USA
  revision: SNS-ED-0
  board:    SHVC-1A0N-01
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   a8fe2226728002786d68c27ddddf0b90a894db52e4dfe268fdf72a68cae5f02e
  label:    EarthBound
  name:     EarthBound
  region:   SNS-MB-USA
  revision: SNS-MB-0
  board:    SHVC-1J3M-20
    memory
      type: ROM
      size: 0x300000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   4579e437279f79eedd6b9cf648a814df2ab2c83d937a1bcec1578d28965fb9a0
  label:    Earthworm Jim
  name:     Earthworm Jim
  region:   SNS-AEJE-USA
  revision: SNS-AEJE-0
  board:    SHVC-BJ0N-01
    memory
      type: ROM
      size: 0x300000
      content: Program

game
  sha256:   10eadaab168707829418702386e1bcedd2619d9bbefc37cf31c4118313bcf6de
  label:    Earthworm Jim 2
  name:     Earthworm Jim 2
  region:   SNS-A2EE-USA
  revision: SNS-A2EE-0
  board:    MJSC-1J0N-20
    memory
      type: ROM
      size: 0x300000
      content: Program

game
  sha256:   5d658b63d35e2e0baf48ae3bb04ea5e1553855b34bb39fc2c7ca41fbd3894d52
  label:    Eek! The Cat
  name:     Eek! The Cat
  region:   SNS-E7-USA
  revision: SNS-E7-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   b296054effb1948039635044bc905fdf8ff53e7a73038fd5d8436a913ea5ad8a
  label:    Elite Soccer
  name:     Elite Soccer
  region:   SNS-L7-USA
  revision: SNS-L7-0
  board:    SHVC-1J0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   f14e30ee452ec930a6d08094094b287d0c40c8108f2017c418015242987649b3
  label:    Emmitt Smith Football
  name:     Emmitt Smith Football
  region:   SNS-AESE-USA
  revision: SNS-AESE-0
  board:    SHVC-1A3M-30
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   cfd666f0bbabec59613d9fe189db7d0a060a78047bc084c0c365840769047bf2
  label:    Equinox
  name:     Equinox
  region:   SNS-EX-USA
  revision: SNS-EX-0
  board:    SHVC-1A3M-20
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   c702c3860e9dffdaa1917d013ecbcefdd2c47f89726992f7f810d879772dcc4d
  label:    ESPN Baseball Tonight
  name:     ESPN Baseball Tonight
  region:   SNS-EV-USA
  revision: SNS-EV-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   3326061160ad12c7aef5176544ec1c8ef2f534a51960ca882dbc7fcb9b1a7384
  label:    ESPN National Hockey Night
  name:     ESPN National Hockey Night
  region:   SNS-AEHE-USA
  revision: SNS-AEHE-0
  board:    SHVC-1A3M-21
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   7b2fdab972e393395f5379d7fb13e40464db152f6acf58b2d2a6a18f81cefecb
  label:    ESPN Speed World
  name:     ESPN Speed World
  region:   SNS-ASWE-USA
  revision: SNS-ASWE-0
  board:    SHVC-1A3M-21
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   faf595b4671f005fa9367ba3fcd30dbff371bc7a9ca8bbfbc0ebfcc5826b60f8
  label:    ESPN Sunday Night NFL
  name:     ESPN Sunday Night NFL
  region:   SNS-ASNE-USA
  revision: SNS-ASNE-0
  board:    SHVC-1A3M-21
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   8481e47381bd98c27b9782b5727a5d5f0976fbb3aa3df25c2c42aa37e0586815
  label:    E.V.O.: Search for Eden
  name:     EVO - Search for Eden
  region:   SNS-46-USA
  revision: SNS-46-0
  board:    SHVC-2A3M-01#A
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   f2455253db316b6ccd0c5c686d1f1e2d94cd5e37534e70a3a07a409120d58df6
  label:    Exertainment Mountain Bike Rally
  name:     Exertainment Mountain Bike Rally
  region:   SNS-9X-USA
  revision: SNS-9X-0
  board:    SHVC-1A1M-11
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   a0521f50b0d0bff6666bfb712498476eb8d5974ef38caf157e2f67cbce5475bb
  label:    Exertainment Mountain Bike Rally + Speed Racer
  name:     Exertainment Mountain Bike Rally + Speed Racer
  region:   SNS-ALFE-USA
  revision: SNS-ALFE-0
  board:    SHVC-1A3M-30
    memory
      type: ROM
      size: 0x400000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   1576066e0cb771a91caf79e7d4f6dc00eb0daa47f0786f1604b32537429a7f45
  label:    Extra Innings
  name:     Extra Innings
  region:   SNS-GL-USA
  revision: SNS-GL-0
  board:    SHVC-1A3B-12
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   bf16c3c867c58e2ab061c70de9295b6930d63f29f81cc986f5ecae03e0ad18d2
  label:    F-Zero
  name:     F-Zero
  region:   SNS-FZ-USA
  revision: SNS-FZ-0
  board:    SHVC-1A1B-04
    memory
      type: ROM
      size: 0x80000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   55c33efb514568c9946c4b866c160ed190fe436faee265ee2fb107f7fe94d524
  label:    F1 Pole Position
  name:     F1 Pole Position
  region:   SNS-6P-USA
  revision: SNS-6P-0
  board:    SHVC-1A1M-01
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   1d38e3af9e3a6409e93f4705b68c42558f558c68f3e83ef2a40e46cf560b26cc
  label:    F1 Race of Champions
  name:     F1 Race of Champions
  region:   SNS-EH-USA
  revision: SNS-EH-0
  board:    SHVC-1A3B-13
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   3ae7bfd38a3dc273f4d387af3b15621beefebf5056731af06e3822f5e57db5c5
  label:    F1 Race of Champions II
  name:     F1 Race of Champions II
  region:   SNS-E2-USA
  revision: SNS-E2-0
  board:    SHVC-1DS0B-20
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: ROM
      size: 0xc000
      content: Program
      manufacturer: NEC
      architecture: uPD96050
      identifier: ST010
    memory
      type: ROM
      size: 0x1000
      content: Data
      manufacturer: NEC
      architecture: uPD96050
      identifier: ST010
    memory
      type: RAM
      size: 0x1000
      content: Data
      manufacturer: NEC
      architecture: uPD96050
      identifier: ST010
    oscillator
      frequency: 11000000

game
  sha256:   d689392884df91c2bb84b1411a96f3919b6c9cc8a583dff901a98f0d86d31c30
  label:    Faceball 2000
  name:     Faceball 2000
  region:   SNS-2F-USA
  revision: SNS-2F-0
  board:    SHVC-1A0N-02
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   2891f1eab285133364ecc379a5c9e1d0026d60f425f1a458d149014f386cfa50
  label:    Family Dog
  name:     Family Dog
  region:   SNS-D8-USA
  revision: SNS-D8-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   4f43ce12e1d8cd195468d7048494ad2930721e5bf9e69bfd86eeee707ffc634b
  label:    Family Feud
  name:     Family Feud
  region:   SNS-FN-USA
  revision: SNS-FN-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   d5c651d726dd957b80d03ab6fdbed4cdd26a3cccf5ec9d91af67251b3ec26a3c
  label:    Family Feud
  name:     Family Feud
  region:   SNS-FN-USA
  revision: SNS-FN-1
  board:    MAXI-1A0N-30
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   c92f389d25870aada3002775838ec9f69a988120c0238af885fd08d46bd94930
  label:    Fatal Fury
  name:     Fatal Fury
  region:   SNS-GN-USA
  revision: SNS-GN-0
  board:    SHVC-2A0N-01
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   a0c554d46034caef231c36dd6849828ca39703678fb7fdd15a11f292b93bcd6b
  label:    Fatal Fury 2
  name:     Fatal Fury 2
  region:   SNS-DJ-USA
  revision: SNS-DJ-0
  board:    SHVC-BJ0N-01
    memory
      type: ROM
      size: 0x280000
      content: Program

game
  sha256:   410e90db3d38507ccc85ad3bca6b27a080123fd5160e82b5de4d914d4b6d6e61
  label:    Fatal Fury Special
  name:     Fatal Fury Special
  region:   SNS-3R-USA
  revision: SNS-3R-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x400000
      content: Program

game
  sha256:   1f454f2ce16fb96ba0b950ceaa098fe6eabed9e4a0e512252debad8fa6bc5ef5
  label:    FIFA International Soccer
  name:     FIFA International Soccer
  region:   SNS-84-USA
  revision: SNS-84-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   26ff143c2f1a4a16ca838cc4b5555e39bbe7208515dca8f1c4b1a00dec61cf09
  label:    FIFA Soccer '96
  name:     FIFA Soccer '96
  region:   SNS-A6SE-USA
  revision: SNS-A6SE-0
  board:    SHVC-1A3M-30
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   c514f855ad377242582952eee95513a2152ebbf6bb5b06bdf9f031fc5ac748ab
  label:    FIFA Soccer '97
  name:     FIFA Soccer '97
  region:   SNS-A7IE-USA
  revision: SNS-A7IE-0
  board:    EA-1A3M-30
    memory
      type: ROM
      size: 0x1e0000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   b9594d588816ae570ea5fea14577ed47de4db9ac9a40a116c84e0ad7a2ce58f8
  label:    Fighter's History
  name:     Fighter's History
  region:   SNS-YH-USA
  revision: SNS-YH-1
  board:    SHVC-BJ0N-01
    memory
      type: ROM
      size: 0x280000
      content: Program

game
  sha256:   6151389f33ce2e53db3cd99592440c0020f5f4668f581ce3bd615bc92077f255
  label:    Final Fantasy: Mystic Quest
  name:     Final Fantasy - Mystic Quest
  region:   SNS-MQ-USA
  revision: SNS-MQ-0
  board:    SHVC-1A3B-13
    memory
      type: ROM
      size: 0x80000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   680535dc1c4196c53b40dc9c2c9bc159a77802ab8d4b474bef5dc0281c15ad06
  label:    Final Fantasy II
  name:     Final Fantasy II
  region:   SNS-F4-USA
  revision: SNS-F4-0
  board:    SHVC-1A3B-12
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   0f51b4fca41b7fd509e4b8f9d543151f68efa5e97b08493e4b2a0c06f5d8d5e2
  label:    Final Fantasy III
  name:     Final Fantasy III
  region:   SNS-F6-USA
  revision: SNS-F6-0
  board:    SHVC-BJ3M-10
    memory
      type: ROM
      size: 0x300000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   60cca2592d0756b8c4c7a0a254fafa5ac47aa752521fd1f77dcbf4b6ee1bee90
  label:    Final Fight
  name:     Final Fight
  region:   SNS-FT-USA
  revision: SNS-FT-0
  board:    SHVC-1A0N-01
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   744d1a16c3f99970283597ae8f43b7c3621c5f995c4566ef24b8d70b0692007b
  label:    Final Fight 2
  name:     Final Fight 2
  region:   SNS-F2-USA
  revision: SNS-F2-0
  board:    SHVC-2A0N-01
    memory
      type: ROM
      size: 0x140000
      content: Program

game
  sha256:   f9dac709b2c2859f828103a0dd980716817e2bde3b9d7e2fdea36e9bb9bed7f2
  label:    Final Fight 3
  name:     Final Fight 3
  region:   SNS-AFZE-USA
  revision: SNS-AFZE-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x300000
      content: Program

game
  sha256:   8c47f9dc79748c0ae6c648f8480614d22eaefade065612ad1fe749fc3db4d1bc
  label:    Final Fight Guy
  name:     Final Fight Guy
  region:   SNS-FY-USA
  revision: SNS-FY-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   003dba0193acc5336840307194643ca3b1f848dcfc77580b4e17c605105b27f5
  label:    Firepower 2000
  name:     Firepower 2000
  region:   SNS-FW-USA
  revision: SNS-FW-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   6f32355bef68d4ad58822f50074b46bcc9a68357cd2c6a5470c96bf5344f84d8
  label:    Firestriker
  name:     Firestriker
  region:   SNS-3S-USA
  revision: SNS-3S-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   4c1354337efa788169387458fa6bdbcf4be0c98369920af2bd876ad98d29070f
  label:    First Samurai
  name:     First Samurai
  region:   SNS-FK-USA
  revision: SNS-FK-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   064a880a8dfcf576f74ae8a17c3ec7b0a27e8cb0300a5e5959452fcc30422f14
  label:    Flashback: The Quest for Identity
  name:     Flashback - The Quest for Identity
  region:   SNS-5F-USA
  revision: SNS-5F-0
  board:    SHVC-1J0N-10
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   ff09d72d6241b331dc429d1cf27c04c26546f23312a22c7a14e6a4bf41ed1069
  label:    The Flintstones
  name:     Flintstones, The
  region:   SNS-AFNE-USA
  revision: SNS-AFNE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   3d5bbc06e7e9797d937c803610c40b262c14c3f0a39e8048dd6b0b052a040fc1
  label:    The Flintstones: The Treasure of Sierra Madrock
  name:     Flintstones, The - The Treasure of Sierra Madrock
  region:   SNS-9F-USA
  revision: SNS-9F-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   40b55ee44bc6f1c83daac3e1806dcf7ecd5b35280cdb6a63c7a5e52fbd2115e6
  label:    Football Fury
  name:     Football Fury
  region:   SNS-UF-USA
  revision: SNS-UF-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   bc6b0344a434644c391ac69a53c7720c51e2d3c5c082b8d78598ae4df100c464
  label:    Foreman For Real
  name:     Foreman For Real
  region:   SNS-AFEE-USA
  revision: SNS-AFEE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x300000
      content: Program

game
  sha256:   ba93721bdb0f19506e722bc580d0b4dad6e8648dddbc52e3ce45dd75ea165f72
  label:    Frank Thomas' Big Hurt Baseball
  name:     Frank Thomas' Big Hurt Baseball
  region:   SNS-AFKE-USA
  revision: SNS-AFKE-0
  board:    SHVC-1A3M-30
    memory
      type: ROM
      size: 0x300000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   25b380f529f5a9295df7c0adcc5213d67f131f552d078a3d8f545f988047c90a
  label:    Frantic Flea
  name:     Frantic Flea
  region:   SNS-AF8E-USA
  revision: SNS-AF8E-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   1ce72767795f41049a1f4d2829e837a8885eb91e1c14faf1ca62d05839ebe3c9
  label:    Frogger
  name:     Frogger
  region:   SNS-AF7E-USA
  revision: SNS-AF7E-0
  board:    MAXI-1A0N-30
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   3db8a496a49d47b1ac2a893feaf682722765b2cde63c22af3aa68212dcfa975f
  label:    Full Throttle: All-American Racing
  name:     Full Throttle - All-American Racing
  region:   SNS-AFTE-USA
  revision: SNS-AFTE-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   dcceb5be536c9b91bf34f65e7fcec4b55a78af0192323cf86f3b9555072037ee
  label:    fun 'n games
  name:     Fun & Games
  region:   SNS-7N-USA
  revision: SNS-7N-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   78e2b8210d8dd8776ee23153eb3dce2cbf7d8ddc3e8a5d25b580428f59d98bdb
  label:    Gemfire
  name:     Gemfire
  region:   SNS-RL-USA
  revision: SNS-RL-0
  board:    SHVC-1A3B-13
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   05e9404bfe0689e484a34991f22712c9da718cf000b0f748378af5758b654a3d
  label:    Genghis Khan II: Clan of the Gray Wolf
  name:     Genghis Khan II - Clan of the Gray Wolf
  region:   SNS-6G-USA
  revision: SNS-6G-0
  board:    SHVC-1J3B-01
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   fc993c122079b94ebc9b9452b46a55d5ddcd3715a4b97af795222b264827e90f
  label:    George Foreman's KO Boxing
  name:     George Foreman's KO Boxing
  region:   SNS-GK-USA
  revision: SNS-GK-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   a4ceb31b82ea532e6eb640fa2eda61625758e72251efa5f0ae9a984f4a98a8a0
  label:    Ghoul Patrol
  name:     Ghoul Patrol
  region:   SNS-AGJE-USA
  revision: SNS-AGJE-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   feff03cfa195512ad87dc876012d70520cf192e2874c518b6dbdf3d876ea60c4
  label:    Goal!
  name:     Goal!
  region:   SNS-SU-USA
  revision: SNS-SU-0
  board:    SHVC-1A3B-13
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   8796ca4de3aeffd3a494fe28e7d7e2aeb220ca652b43684f29e2cc94f02c20c4
  label:    Gods
  name:     Gods
  region:   SNS-GZ-USA
  revision: SNS-GZ-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   2bb368c47189ce813ad716eef16c01cd47685cb98e2c1cb35fa6f0173c97dd7c
  label:    Goof Troop
  name:     Goof Troop
  region:   SNS-G6-USA
  revision: SNS-G6-0
  board:    MAXI-1A0N-30
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   342ddf438aec3f0696de8f2cd74bb7f48a956f488f1246eeccaff5ef246ca50b
  label:    GP-1
  name:     GP-1
  region:   SNS-G7-USA
  revision: SNS-G7-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   2e478569949c3935529c956448947ef90bae64abaf3d523bd89e7f4cf5e83702
  label:    GP-1: Part II
  name:     GP-1 - Part II
  region:   SNS-AGRE-USA
  revision: SNS-AGRE-0
  board:    SHVC-2J0N-11
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   93da752a0c76167d0907efa832367e5d14aab8e835b864f345c386071a9af718
  label:    Gradius III
  name:     Gradius III
  region:   SNS-G3-USA
  revision: SNS-G3-0
  board:    SHVC-1A0N-01
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   b5492aea296ee4bfcd2c677fbec5153fd4c4db758c835b372ef750cf2399649b
  label:    The Great Circus Mystery: Starring Micky & Minnie
  name:     Great Circus Mystery, The - Starring Micky & Minnie
  region:   SNS-4C-USA
  revision: SNS-4C-0
  board:    SHVC-2J0N-11
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   3ab1ca181000f10cb7c2ae8dc9fafeecd5d77314ee92960e26dff0d6a1fd11ee
  label:    The Great Waldo Search
  name:     Great Waldo Search, The
  region:   SNS-GW-USA
  revision: SNS-GW-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   f921297c361f16ad3f1257e91829638fc795f9323172015d7237ed648c8f7515
  label:    GunForce
  name:     GunForce
  region:   SNS-GU-USA
  revision: SNS-GU-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   5a6deb5617e86a9f4b981031e939510e30c5c8ad047f5f012e40442113fd28c2
  label:    Hagane
  name:     Hagane
  region:   SNS-AHGE-USA
  revision: SNS-AHGE-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   de1cf1512e48473b03adb87b7504f394e8b330b346bac24044f833d83609799a
  label:    Hal's Hole in One Golf
  name:     Hal's Hole in One Golf
  region:   SNS-JO-USA
  revision: SNS-JO-0
  board:    SHVC-1A0N-01
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   21cd6d749674c55342f5f1a895642d4b58b9bd7eb211f5df3a35dc6c2f5d4501
  label:    Hammer Lock Wrestling
  name:     Hammer Lock Wrestling
  region:   SNS-LJ-USA
  revision: SNS-LJ-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   4fb9b010e4b1dc166ab7995a6f9491114063b68aac344004b1edfc555951d959
  label:    Hard Ball III
  name:     Hard Ball III
  region:   SNS-3Y-USA
  revision: SNS-3Y-0
  board:    SHVC-1J0N-10
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   6e2a02a8944c19db3da76d2646f232fbe3ed039bc7d44cc03910814fa77a7acf
  label:    Harley's Humongous Adventure
  name:     Harley's Humongous Adventure
  region:   SNS-HV-USA
  revision: SNS-HV-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   73a3aa87ddd5ce5df5ba51292316f42b6e128280179b0a1b11b4dcddc17d4163
  label:    Harvest Moon
  name:     Harvest Moon
  region:   SNS-AYWE-USA
  revision: SNS-AYWE-0
  board:    SHVC-1A3M-30
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   9ff6bbcce7dc1f3bded96860a86698cab161d13ee167c57b5b114ac646eea0ea
  label:    Head-On Soccer
  name:     Head-On Soccer
  region:   SNS-AVSE-USA
  revision: SNS-AVSE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   fa418618787145291201b3d9247048b2d7dfba37f6556dcb1d40db499124dd60
  label:    Hit the Ice
  name:     Hit the Ice
  region:   SNS-HC-USA
  revision: SNS-HC-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   7c34908526db2a634216fab0276c42a8e4e22d59c728cd586200142a12dd2c2c
  label:    Home Alone
  name:     Home Alone
  region:   SNS-HA-USA
  revision: SNS-HA-0
  board:    SHVC-1A0N-01
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   27eaccb4eea93832639565a664bf3561ed5a11ac025d377a3f33262d851c1b2b
  label:    Home Alone 2: Lost in New York
  name:     Home Alone 2 - Lost in New York
  region:   SNS-HN-USA
  revision: SNS-HN-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   48a3ac52e2c9128abc2dc60e07817a51898e8a93be0d51d6f541a8635263a089
  label:    Home Improvement
  name:     Home Improvement
  region:   SNS-AHIE-USA
  revision: SNS-AHIE-0
  board:    SHVC-2A0N-11
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   acad4c594f156e0f30dec2532b35fcb3bab800e08263377634e2d96dfd055f3e
  label:    Hook
  name:     Hook
  region:   SNS-HO-USA
  revision: SNS-HO-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   a03528344d40bf800cdbde2dd240b30442cec7aea6026fbbe312a7c36bf0f74a
  label:    The Hunt for Red October
  name:     Hunt for Red October, The
  region:   SNS-RO-USA
  revision: SNS-RO-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   41cc900d2461a8dc4706640e493885ddf85db04d8dffceebf7a0ed89cc983d8d
  label:    Hurricanes
  name:     Hurricanes
  region:   SNS-AHUE-USA
  revision: SNS-AHUE-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   db57f45e113127b7148c1b0add2da888e16e16b3e46749f95973b3ef497ae90b
  label:    Hyper V-Ball
  name:     Hyper V-Ball
  region:   SNS-HQ-USA
  revision: SNS-HQ-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   f57c49cc3e4c5e34929e12658db0de8794265c517e42f3c518bb1466ba46f14a
  label:    Hyper Zone
  name:     Hyper Zone
  region:   SNS-HZ-USA
  revision: SNS-HZ-0
  board:    SHVC-1A0N-01
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   fa143784fd20721d61101920e6aae9b7f5012b8157b1ce0c7ea83ea6c875d84d
  label:    The Ignition Factor
  name:     Ignition Factor, The
  region:   SNS-AIFE-USA
  revision: SNS-AIFE-0
  board:    SHVC-1J0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   32adc048fd89228a4310c03df243e83de7d436cdb2460b4cc83ade20d359b2bd
  label:    Illusion of Gaia
  name:     Illusion of Gaia
  region:   SNS-JG-USA
  revision: SNS-JG-0
  board:    SHVC-1J3B-01
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   4dc2b5de98e9109583d9b61b38d26a8216af4dba246ef71350122213630562d0
  label:    Imperium
  name:     Imperium
  region:   SNS-DN-USA
  revision: SNS-DN-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   c41150c0e84055801377fb7cb273cc51ca442b269ca6287cadf514f553e34750
  label:    Incantation
  name:     Incantation
  region:   SNS-AIYE-USA
  revision: SNS-AIYE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   0415adfe80977485f84cdffaaa79f18c91c004c7dba202b4cf9a94eb11adeada
  label:    The Incredible Crash Dummies
  name:     Incredible Crash Dummies, The
  region:   SNS-C7-USA
  revision: SNS-C7-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   8d170628d2d2fdf142bb82ad80e908bba54c45fa33241c779b8eafaf1b21171f
  label:    The Incredible Hulk
  name:     Incredible Hulk, The
  region:   SNS-8U-USA
  revision: SNS-8U-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   cf611b40f9570e8fcfdc21db623965da62561e8ca82ded59e432ba6fb0bfb562
  label:    Indiana Jones' Greatest Adventures
  name:     Indiana Jones' Greatest Adventures
  region:   SNS-AIJE-USA
  revision: SNS-AIJE-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   5e5cac64fdcf09a52a9c0ab1ebc8bd309dcb1256faf1405284443569b5284fe5
  label:    Inindo: Way of the Ninja
  name:     Inindo - Way of the Ninja
  region:   SNS-IN-USA
  revision: SNS-IN-0
  board:    SHVC-1A3B-13
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   c7b1706a0ee96f3e0d65cd043c05966bfe3d5c57d08bbd2df3118817424adf82
  label:    Inspector Gadget
  name:     Inspector Gadget
  region:   SNS-5G-USA
  revision: SNS-5G-0
  board:    SHVC-YJ0N-01
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   6443fecebcdc2ff7061a40432f3ad1db6dfd354909a5f306972cf6afa122752c
  label:    International Superstar Soccer
  name:     International Superstar Soccer
  region:   SNS-3U-USA
  revision: SNS-3U-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   d2fe66c1ce66c65ce14e478c94be2e616f9e2cad374b5783a6a64d3c1a99cfa9
  label:    International Superstar Soccer Deluxe
  name:     International Superstar Soccer Deluxe
  region:   SNS-AWJE-USA
  revision: SNS-AWJE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   4e8044b1e7a779685d6751351eef2391272ac8b2bd909edeecfc3d3c5a594bef
  label:    International Tennis Tour
  name:     International Tennis Tour
  region:   SNS-IT-USA
  revision: SNS-IT-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   8304d8bc55aa9e64bdd144d384f4b185af2426e7d64888c6c23dd41366a53981
  label:    The Irem Skins Game
  name:     Irem Skins Game, The
  region:   SNS-MT-USA
  revision: SNS-MT-0
  board:    SHVC-1A0N-02
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   39bfe828571cdb23e7c85c23cf5b175979dcc2042c5841add58f5ae6492168a9
  label:    The Itchy & Scratchy Game
  name:     Itchy & Scratchy Game, The
  region:   SNS-AISE-USA
  revision: SNS-AISE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   183af7d642b55d52cd594786ec2f031d033cc6c8c1a2a84a834e4ada04301b26
  label:    Izzy's Quest for the Olympic Rings
  name:     Izzy's Quest for the Olympic Rings
  region:   SNS-AIZE-USA
  revision: SNS-AIZE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   3ffbb6e0ccf8e9f5e4c72d9fe526a16371e562b71a91d6430e562bf358a5126b
  label:    Jack Nicklaus Golf
  name:     Jack Nicklaus Golf
  region:   SNS-JN-USA
  revision: SNS-JN-0
  board:    SHVC-1A0N-02
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   9cf82dd9a851cdc38bf2afc286c077ff18a0a5d3bb301eba606cc52db62f8965
  label:    James Bond Jr.
  name:     James Bond Jr.
  region:   SNS-JJ-USA
  revision: SNS-JJ-0
  board:    SHVC-1A0N-02
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   8b7b33f3d2edb844f1d43cfdd595b8c2cb6fc005d59acb899a1afda999e47638
  label:    Jammit
  name:     Jammit
  region:   SNS-J6-USA
  revision: SNS-J6-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   85e5f6fedc420925557092d728e1dc6b4e2042368fb78bf93c0df447f8c9c0c0
  label:    Jeopardy!
  name:     Jeopardy!
  region:   SNS-JY-USA
  revision: SNS-JY-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   76d760da59aa7fc2fd4426c1d1aec57412703ab901b9df60ac2be16ede80b0e7
  label:    Jeopardy! - Deluxe Edition
  name:     Jeopardy! - Deluxe Edition
  region:   SNS-7J-USA
  revision: SNS-7J-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   d7110ddc0b34aa4ecd5e55f7b846657edb982db82cf0ba340fe0464daf0ca9be
  label:    Jeopardy! - Sports Edition
  name:     Jeopardy! - Sports Edition
  region:   SNS-8Y-USA
  revision: SNS-8Y-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   ee7a29eb9c302ea2bb235ef990fd8344a6beb9817499941c40a8a94ad5f7c964
  label:    The Jetsons: Invasion of the Planet Pirates
  name:     Jetsons, The - Invasion of the Planet Pirates
  region:   SNS-8J-USA
  revision: SNS-8J-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   a314583b11d594b8245b5177aa64a4d3b7497d096edabbea7c1842c57aa2ad2b
  label:    Jim Lee's Wild Covert Action Teams
  name:     Jim Lee's Wild Covert Action Teams
  region:   SNS-AWIE-USA
  revision: SNS-AWIE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   6f0bec87ece503b0fbe108cd159ed6f5fa7711b1c4fe31e982af41ad5c638093
  label:    Jim Power: The Lost Dimension in 3D
  name:     Jim Power - The Lost Dimension in 3D
  region:   SNS-6J-USA
  revision: SNS-6J-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   205890f94e27e7c1b381124cc35c866351bafa906383e507cd17ccb9d1b68ffd
  label:    Jimmy Connors' Pro Tennis Tour
  name:     Jimmy Connors' Pro Tennis Tour
  region:   SNS-JC-USA
  revision: SNS-JC-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   6abe908b4005a68d1f6e9a54339c94a33db41801890d8c058ce974fdab3d0bcd
  label:    Jimmy Houston's Bass Tournament, U.S.A.
  name:     Jimmy Houston's Bass Tournament, USA
  region:   SNS-AFUE-USA
  revision: SNS-AFUE-0
  board:    SHVC-1A3M-30
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   3b2b8b813b58049a4a86ce1c42d2a651f19fd9bbab2407a494e20cf343d3c1a4
  label:    Joe & Mac
  name:     Joe & Mac
  region:   SNS-JT-USA
  revision: SNS-JT-0
  board:    SHVC-1A0N-01
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   c9889799a9ae8558f26d66ae546db930c99acb78d921b4395afbbc38da6272aa
  label:    Joe & Mac 2: Lost in the Tropics
  name:     Joe & Mac 2 - Lost in the Tropics
  region:   SNS-J3-USA
  revision: SNS-J3-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   5a76347672ea7d27bb334b1e30bbc73e06f92373883bed499245377327a8f0cf
  label:    John Madden Football
  name:     John Madden Football
  region:   SNS-JM-USA
  revision: SNS-JM-0
  board:    SHVC-1A0N-01
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   3e62872bf69ea90dd7093608268f8defa2c6016adb1011745dab3c2af45d69b7
  label:    John Madden Football '93
  name:     John Madden Football '93
  region:   SNS-MF-USA
  revision: SNS-MF-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   67fa7115eb6bf292c024c3a8b06ce9bd457c4d46de182a06a573afff968cc0f1
  label:    Judge Dredd
  name:     Judge Dredd
  region:   SNS-AJDE-USA
  revision: SNS-AJDE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   771a0322d9f5f8e13a52d01e80257a1f75cc693cf4abf74793520eb5f8b5580e
  label:    The Jungle Book
  name:     Jungle Book, The
  region:   SNS-7K-USA
  revision: SNS-7K-0
  board:    MAXI-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   8d812ea4fa897274116f7f43bc689e110f1cfbd3f7eb3a1737c2a85d36369159
  label:    Jungle Strike
  name:     Jungle Strike
  region:   SNS-AJGE-USA
  revision: SNS-AJGE-0
  board:    MJSC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   fe91d45201753ae9655d5ce38838e352f478b26b2d933c1bcb5bd8330121f9ff
  label:    Jurassic Park
  name:     Jurassic Park
  region:   SNS-J8-USA
  revision: SNS-J8-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   0a4e9d6fa2ac16aa51da5538d93280734de480e44c430173ed14826c84553c7d
  label:    Jurassic Park
  name:     Jurassic Park
  region:   SNS-J8-USA
  revision: SNS-J8-1
  board:    MAXI-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   5eff7c27f69b3ebc1ec1294ffcd1debf3512bc3e6c1c75fbdc5e778dcaaba1c9
  label:    Jurassic Park 2: The Chaos Continues
  name:     Jurassic Park 2 - The Chaos Continues
  region:   SNS-A2JE-USA
  revision: SNS-A2JE-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   7f05959f423bc656091ea3bddfbc89c877ae243dca346f63233e27973f34e0eb
  label:    Justice League Task Force
  name:     Justice League Task Force
  region:   SNS-AJLE-USA
  revision: SNS-AJLE-0
  board:    SHVC-BA0N-10
    memory
      type: ROM
      size: 0x280000
      content: Program

game
  sha256:   05152bcf9bf086e7bcdbfa7dd8edfe2085f1339c4d7e193e0071c49a10471ef4
  label:    Ka-Blooey
  name:     Ka-Blooey
  region:   SNS-BB-USA
  revision: SNS-BB-0
  board:    SHVC-1A0N-02
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   7cc3693cc5e1e834d57795f04b048fab27864a898a9507e7ca383771e2035414
  label:    Kawasaki Caribbean Challenge
  name:     Kawasaki Caribbean Challenge
  region:   SNS-KC-USA
  revision: SNS-KC-0
  board:    SHVC-2A0N-10
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   fec6dd097e378e795dd164be658b10b1c60672b2351a7f4a7722d1fe5736410e
  label:    Kawasaki Superbike Challenge
  name:     Kawasaki Superbike Challenge
  region:   SNS-AKEE-USA
  revision: SNS-AKEE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   3104d6c06c8909c56f6adb2faecf1b4382f2490370798b605631da926c5306d8
  label:    Ken Griffey Jr. Presents Major League Baseball
  name:     Ken Griffey Jr. Presents Major League Baseball
  region:   SNS-JR-USA
  revision: SNS-JR-0
  board:    SHVC-1A3B-20
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   f9f7a2de8cbafd6f9042841dfc42529f8245d75f436bed1c23f9ba1663182e61
  label:    Ken Griffey Jr. Presents Major League Baseball
  name:     Ken Griffey Jr. Presents Major League Baseball
  region:   SNS-JR-USA
  revision: SNS-JR-1
  board:    SHVC-1A3M-21
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   18b5276be764c06531219c1757d40214aeec06fae68f4ce3ec78b58ee750a43e
  label:    Ken Griffey Jr.'s Winning Run
  name:     Ken Griffey Jr.'s Winning Run
  region:   SNS-A9GE-USA
  revision: SNS-A9GE-0
  board:    SHVC-1J3M-20
    memory
      type: ROM
      size: 0x400000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   b16661d5151d73cea5ac46d7c899531c7b2cdee2558092c23a5460c8092b80b8
  label:    Kendo Rage
  name:     Kendo Rage
  region:   SNS-M7-USA
  revision: SNS-M7-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   914652f72d6ceb83b8b43414d6c42581ec12e9b3f45259b8b2768d26b8d4f073
  label:    Kid Klown in Crazy Chase
  name:     Kid Klown in Crazy Chase
  region:   SNS-ZI-USA
  revision: SNS-ZI-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   618a23636e07110e094277ec1d1e60c3620a6e9a5f386292808267593fa803ad
  label:    Killer Instinct
  name:     Killer Instinct
  region:   SNS-AKLE-USA
  revision: SNS-AKLE-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x400000
      content: Program

game
  sha256:   1b58d0aed4510811c73d267244a7e915aa0b334c86e68f3fa5883f5cb534e4d7
  label:    King Arthur & The Knights of Justice
  name:     King Arthur & The Knights of Justice
  region:   SNS-AAKE-USA
  revision: SNS-AAKE-0
  board:    SHVC-BJ0N-20
    memory
      type: ROM
      size: 0x280000
      content: Program

game
  sha256:   aca9eb59d6783e2cae3787c69053888fea98f5dfe4c8af8b5a6360e0afb3b5d7
  label:    King Arthur's World
  name:     King Arthur's World
  region:   SNS-RC-USA
  revision: SNS-RC-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   6638b5541059814d4c34574e5e277ef613aebf81c91d3def557a7642fb5840e1
  label:    King of Dragons
  name:     King of Dragons
  region:   SNS-EI-USA
  revision: SNS-EI-0
  board:    SHVC-1J0N-10
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   1135858a1ce686a0a163bb0e6f3a4d7cd71c0cd56efbc79677372f2624cf2306
  label:    King of the Monsters
  name:     King of the Monsters
  region:   SNS-KM-USA
  revision: SNS-KM-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   a9729d049ce580839bbfef1836a13dc16f2fc934d203ebf7390e0b1c47ea9a2d
  label:    King of the Monsters 2
  name:     King of the Monsters 2
  region:   SNS-KT-USA
  revision: SNS-KT-0
  board:    SHVC-1J0N-10
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   4e095fbbdec4a16b075d7140385ff68b259870ca9e3357f076dfff7f3d1c4a62
  label:    Kirby Super Star
  name:     Kirby Super Star
  region:   SNS-AKFE-USA
  revision: SNS-AKFE-0
  board:    SHVC-1L3B-11
    memory
      type: ROM
      size: 0x400000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save
    memory
      type: RAM
      size: 0x800
      content: Internal
      volatile

game
  sha256:   67937dd7a29a93b1aaabb6df89f0748369ff47f3f6c655a402c00d5657973140
  label:    Kirby's Avalanche
  name:     Kirby's Avalanche
  region:   SNS-PQ-USA
  revision: SNS-PQ-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   0f984dc5fe8293f75e3b8fad98b0cb564706d9b1e3902b56415aa399c2d4df2b
  label:    Kirby's Dream Course
  name:     Kirby's Dream Course
  region:   SNS-CG-USA
  revision: SNS-CG-0
  board:    SHVC-1A3M-30
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   b50bf9d95485e8aeb7a6730e9f9f9c9c4ec23a85b336a4fb2e3b63034531e36f
  label:    Kirby's Dream Land 3
  name:     Kirby's Dream Land 3
  region:   SNS-AFJE-USA
  revision: SNS-AFJE-0
  board:    SHVC-1L5B-20
    memory
      type: ROM
      size: 0x400000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save
    memory
      type: RAM
      size: 0x800
      content: Internal
      volatile

game
  sha256:   fb601ead645edce139b0483d3155b4e3d7ab245bf87a3a66cb88c0a617c0a526
  label:    Knights of the Round
  name:     Knights of the Round
  region:   SNS-LO-USA
  revision: SNS-LO-0
  board:    SHVC-2A0N-11
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   bed18c968aee0eb0c866c1964c28135364cd6d65fff7bcb5873342c04e63750d
  label:    Krusty's Super Fun House
  name:     Krusty's Super Fun House
  region:   SNS-FH-USA
  revision: SNS-FH-0
  board:    SHVC-1A0N-02
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   e36322697c48baae6492db91e6cbf3844a420f6e0cc8a75f3a73556026ddbbb8
  label:    Krusty's Super Fun House
  name:     Krusty's Super Fun House
  region:   SNS-FH-USA
  revision: SNS-FH-1
  board:    SHVC-1A0N-02
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   daf3e45bafbec81ffa5911b94810b2cd267574de717292360f9940f41fb2a6a9
  label:    Kyle Petty's No Fear Racing
  name:     Kyle Petty's No Fear Racing
  region:   SNS-AKPE-USA
  revision: SNS-AKPE-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x300000
      content: Program

game
  sha256:   46c811f0cacffe8f20e1d63072d25d7c47e9bb3fd5124851fd05aca9884d21fb
  label:    Lagoon
  name:     Lagoon
  region:   SNS-LA-USA
  revision: SNS-LA-0
  board:    SHVC-1A3B-12
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   48cd9476fef1ed685b9c30dd1669b46048f7295cbbb2abcfa5b1a48699346ea3
  label:    Lamborghini American Challenge
  name:     Lamborghini American Challenge
  region:   SNS-L8-USA
  revision: SNS-L8-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   314d53f409b66ba3f4426a6f1bb7c69f6779aeed277ce2e19535f94d7c8ca586
  label:    Last Action Hero
  name:     Last Action Hero
  region:   SNS-L5-USA
  revision: SNS-L5-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   a179a1188220b59787c49a78a0dde79b89380e3a8a8a0ab558f0102c5796f873
  label:    The Lawnmower Man
  name:     Lawnmower Man, The
  region:   SNS-LW-USA
  revision: SNS-LW-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   c7814cee0fc95d6422cf19a3dc8c9a65b60f6f56da75f09cebea02cc5f99261b
  label:    Legend
  name:     Legend
  region:   SNS-6L-USA
  revision: SNS-6L-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   c865fb17e8c59a21d32b9a605319241fa74ec732e3f0ee58f5d7fcbd8ee57c6b
  label:    The Legend of the Mystical Ninja
  name:     Legend of the Mystical Ninja, The
  region:   SNS-GG-USA
  revision: SNS-GG-0
  board:    SHVC-1A0N-01
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   66871d66be19ad2c34c927d6b14cd8eb6fc3181965b6e517cb361f7316009cfb
  label:    The Legend of Zelda: A Link to the Past
  name:     Legend of Zelda, The - A Link to the Past
  region:   SNS-ZL-USA
  revision: SNS-ZL-0
  board:    SHVC-1A3B-12
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   4b28d2ddab405976bb56e41a40ec1ea11d7362a8f398d5f8c117d715a15719ca
  label:    Lemmings
  name:     Lemmings
  region:   SNS-LE-USA
  revision: SNS-LE-0
  board:    SHVC-1A0N-01
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   cd016c41c7ef9f4f243d57c2b1564b4ceb3b4c38cc165cd02ab6c8e35c93aa2e
  label:    Lemmings
  name:     Lemmings
  region:   SNS-LE-USA
  revision: SNS-LE-1
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   63ecdca7f89b2432ccd57bdb81c0875f6336353f5897f756ef162ab2ec2ee707
  label:    Lemmings 2: The Tribes
  name:     Lemmings 2 - The Tribes
  region:   SNS-L2-USA
  revision: SNS-L2-0
  board:    SHVC-1A1M-11
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   a2c1970670e2831e47e24ced01bf4ba5aba05cac3773bf524c62d689c35687e1
  label:    Lester the Unlikely
  name:     Lester the Unlikely
  region:   SNS-LY-USA
  revision: SNS-LY-0
  board:    SHVC-1J0N-01
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   3bc5f296c3dbee012e93a5cf25568f9288ce87b34d74085401a560350eaca03f
  label:    Lethal Enforcers
  name:     Lethal Enforcers
  region:   SNS-LK-USA
  revision: SNS-LK-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   80c22cc92d51a54de9cd9fd00db5ff58a35fff35e822169c94e445d50834fba3
  label:    Lethal Weapon
  name:     Lethal Weapon
  region:   SNS-L3-USA
  revision: SNS-L3-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   8fcb9c34aea863d49ff08a0ace3f83d301b81f01e2ede28bee7e6d778878d0cc
  label:    Liberty or Death
  name:     Liberty or Death
  region:   SNS-7L-USA
  revision: SNS-7L-0
  board:    SHVC-2J3M-01
    memory
      type: ROM
      size: 0x140000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   457abe634e0a8be03e29513a3dca8f3e9d0ddc6bf97d8931f2316094260f3712
  label:    The Lion King
  name:     Lion King, The
  region:   SNS-ALKE-USA
  revision: SNS-ALKE-0
  board:    MJSC-1J0N-20
    memory
      type: ROM
      size: 0x300000
      content: Program

game
  sha256:   0e2ba574ff73587f211c8f818d444631584832e9240548f003171c11b898ad62
  label:    Lock On
  name:     Lock On
  region:   SNS-AZ-USA
  revision: SNS-AZ-0
  board:    SHVC-1K0N-01
    memory
      type: ROM
      size: 0x80000
      content: Program
    memory
      type: ROM
      size: 0x1800
      content: Program
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1
    memory
      type: ROM
      size: 0x800
      content: Data
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1
    memory
      type: RAM
      size: 0x200
      content: Data
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1
      volatile
    oscillator
      frequency: 7600000

game
  sha256:   76ba0fc1f5c1f39354bb3173a600f23915f1191f400f7d525d220b4b3c8d958d
  label:    Looney Tunes B-Ball
  name:     Looney Tunes B-Ball
  region:   SNS-ALTE-USA
  revision: SNS-ALTE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   62557ee2a3fc3b5a3f59431f966eb61bb380ba983ef6c7742cb55cf075f15f6c
  label:    The Lord of the Rings: Volume 1
  name:     Lord of the Rings, The - Volume 1
  region:   SNS-64-USA
  revision: SNS-64-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   ab3d97c1a3a979e1680a428ec65df54cfb72997bbfe2173292248a4fa8c51ba1
  label:    Lost Vikings 2
  name:     Lost Vikings 2
  region:   SNS-ALVE-USA
  revision: SNS-ALVE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   9f7782a92fda789f9d119b1f0a2f7da0f35606357556a48eca9487797ee1a888
  label:    The Lost Vikings
  name:     Lost Vikings, The
  region:   SNS-LV-USA
  revision: SNS-LV-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   73731a5a7932965de02a9e98055dcf88b4d17b8f710a6ecfde3e36a1f248773b
  label:    Lufia & The Fortress of Doom
  name:     Lufia & The Fortress of Doom
  region:   SNS-ES-USA
  revision: SNS-ES-0
  board:    SHVC-1A3M-20
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   7c34ecb16c10f551120ed7b86cfbc947042f479b52ee74bb3c40e92fdd192b3a
  label:    Lufia II: Rise of the Sinistrals
  name:     Lufia II - Rise of the Sinistrals
  region:   SNS-ANIE-USA
  revision: SNS-ANIE-0
  board:    SHVC-1A3M-30
    memory
      type: ROM
      size: 0x280000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   7e77e196db47e87a5b297e60f0dfa7ce41df8d2d1fdd9152e06628d0b0e586af
  label:    Madden NFL '94
  name:     Madden NFL '94
  region:   SNS-9M-USA
  revision: SNS-9M-0
  board:    SHVC-1J0N-01
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   0ad77ae7af231313e1369a52d1622b88e3751aa5ec774628df7071f9e4244abc
  label:    Madden NFL '95
  name:     Madden NFL '95
  region:   SNS-ANLE-USA
  revision: SNS-ANLE-0
  board:    SHVC-1J3M-11
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   3059d86cdc383985c564a7a891fe18e08f5222ead7ede9fa309159d60cde13a1
  label:    Madden NFL '96
  name:     Madden NFL '96
  region:   SNS-A6FE-USA
  revision: SNS-A6FE-0
  board:    SHVC-1J3M-20
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   6874568d985f65dd817d4b03998e71c8cbacc8d8707411fde7bffee350605a88
  label:    Madden NFL '97
  name:     Madden NFL '97
  region:   SNS-A7NE-USA
  revision: SNS-A7NE-0
  board:    EA-1J3M-20
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   e3c62c9fe55d2311aa6a264f41b45d6cbc7b1b069ed3aa82ee57d381c062547d
  label:    Madden NFL '98
  name:     Madden NFL '98
  region:   SNS-A8NE-USA
  revision: SNS-A8NE-0
  board:    EA-1J3M-20
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   c01fb8989d391d3e343003934937f02bd8ef9aacdad68c32c3d3f56feb72f5b0
  label:    Magic Boy
  name:     Magic Boy
  region:   SNS-YG-USA
  revision: SNS-YG-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   1d3cceaa05e054b002caeb09fd5fb9e718ec446764f4169d97bc185da76fdf4d
  label:    Magic Sword
  name:     Magic Sword
  region:   SNS-MD-USA
  revision: SNS-MD-0
  board:    SHVC-1A0N-02
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   f301bb8ea867e530ecb64e8eff504ed5b9697cf076c70e2036ecf2ffbe6c487a
  label:    The Magical Quest Starring Mickey Mouse
  name:     Magical Quest Starring Mickey Mouse, The
  region:   SNS-MI-USA
  revision: SNS-MI-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   8267e2f092c86d5a29c9a826db82c7473638e28e2507cdaf5c86981f07cd0bef
  label:    Mario is Missing!
  name:     Mario is Missing!
  region:   SNS-MU-USA
  revision: SNS-MU-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   e842cac1a4301be196f1e137fbd1a16866d5c913f24dbca313f4dd8bd7472f45
  label:    Mario Paint
  name:     Mario Paint
  region:   SNS-MP-USA
  revision: SHVC-MP-0
  board:    SHVC-1A5M-20
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save

game
  sha256:   22b21fb39d40447846f6ff77a07f7b4aba2a7473941ba50c787aae6153b1fb5e
  label:    Mario's Early Years: Fun with Letters
  name:     Mario's Early Years - Fun with Letters
  region:   SNS-AMYE-USA
  revision: SNS-AMYE-0
  board:    SHVC-1J0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   57b94f5f576abbe8bd8f5a4eeb6cf927300ec1b5f0596610f3539ba733505c12
  label:    Mario's Early Years: Fun with Numbers
  name:     Mario's Early Years - Fun with Numbers
  region:   SNS-YR-USA
  revision: SNS-YR-0
  board:    SHVC-1J0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   c646b1e60e2cd5a2741915f5d3dfe3c17f45ff7283f8052e840bd4354b0990e1
  label:    Mario's Early Years: Preschool Fun
  name:     Mario's Early Years - Preschool Fun
  region:   SNS-AMEE-USA
  revision: SNS-AMEE-0
  board:    SHVC-1J0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   4b69d4958e099c3b3f6ae45e153ced9b24755d8c161dfee06c9f67886a7c0f09
  label:    Mario's Time Machine
  name:     Mario's Time Machine
  region:   SNS-8M-USA
  revision: SNS-8M-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   908440f96fd0df14602fc6d1daee8519fc31f765ad00bf64aaba35c2c6ef0b56
  label:    Mark Davis' The Fishing Master
  name:     Mark Davis' The Fishing Master
  region:   SNS-AOAE-USA
  revision: SNS-AOAE-0
  board:    SHVC-1J3M-20
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   49dd77b310b476c875633335243553be59ecfb0bffae62e46f2e53ff05c20fcd
  label:    Marvel Super Heroes in War of the Gems
  name:     Marvel Super Heroes in War of the Gems
  region:   SNS-AHZE-USA
  revision: SNS-AHZE-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   2731f0bd1c87e75121f41d1ed5cc9fbf177f414b8bf831c76fd9c4b58c86ed08
  label:    Mary Shelley's Frankenstein
  name:     Mary Shelley's Frankenstein
  region:   SNS-AFRE-USA
  revision: SNS-AFRE-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   44cc113ce1e7616cc737adea9e8f140436c9f1c3fba57e8e9db48025d4ace632
  label:    The Mask
  name:     Mask, The
  region:   SNS-AMGE-USA
  revision: SNS-AMGE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   22453182d44380f08004a12c6492a0c4b2e1f584e268dcc3296a03ea03ae0909
  label:    Math Blaster: Episode One
  name:     Math Blaster - Episode One
  region:   SNS-AMME-USA
  revision: SNS-AMME-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   a203a13870eaec92095daef1196a0c9fe8416e600504d55dd0dc724d4f5f5cb0
  label:    Maui Mallard in Cold Shadow
  name:     Maui Mallard in Cold Shadow
  region:   SNS-AZBE-USA
  revision: SNS-AZBE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x300000
      content: Program

game
  sha256:   c3c4afdeb67136619c643bd9a9f3fe90337541a40745503b5d4eb9b9e6e64b67
  label:    Mecarobot Golf
  name:     Mecarobot Golf
  region:   SNS-TS-USA
  revision: SNS-TS-0
  board:    SHVC-1A3B-13
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   2a08704748f5ef6488348c4099729feca600412d331bda3756e51efd8b94e113
  label:    MechWarrior
  name:     MechWarrior
  region:   SNS-WM-USA
  revision: SNS-WM-0
  board:    SHVC-1A1B-06
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   7bffa1dc31604fa3d61e06ce2c59168098cc8dd7e59998e1d5f30c49bdf8d617
  label:    MechWarrior 3050
  name:     MechWarrior 3050
  region:   SNS-A35E-USA
  revision: SNS-A35E-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   a255fec32453739903a1954149f19bc9658f4a415600b44badf1d4e5e13a16f9
  label:    Mega Man 7
  name:     Mega Man 7
  region:   SNS-A7RE-USA
  revision: SNS-A7RE-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   cf4d603dc0a3759da571224c671a9bfd29f9e52ca8dbb61bcc8ac8be5481e9b2
  label:    Mega Man Soccer
  name:     Mega Man Soccer
  region:   SNS-RQ-USA
  revision: SNS-RQ-0
  board:    SHVC-2A0N-11
    memory
      type: ROM
      size: 0x140000
      content: Program

game
  sha256:   3e1209f473bff8cd4bcbf71d071e7f8df17a2d564e9a5c4c427ee8198cebb615
  label:    Mega Man X
  name:     Mega Man X
  region:   SNS-RX-USA
  revision: SNS-RX-0
  board:    SHVC-2A0N-01
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   b8f70a6e7fb93819f79693578887e2c11e196bdf1ac6ddc7cb924b1ad0be2d32
  label:    Mega Man X
  name:     Mega Man X
  region:   SNS-RX-USA
  revision: SNS-RX-1
  board:    MAXI-1A0N-30
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   da484f2a636b8d692840f40e5468e992c5f2af26d365c69fbc12ef2923588d23
  label:    Mega Man X2
  name:     Mega Man X2
  region:   SNS-ARXE-USA
  revision: SNS-ARXE-0
  board:    SHVC-2DC0N-01
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: ROM
      size: 0xc00
      content: Data
      manufacturer: Hitachi
      architecture: HG51BS169
      identifier: Cx4
    memory
      type: RAM
      size: 0xc00
      content: Data
      manufacturer: Hitachi
      architecture: HG51BS169
      identifier: Cx4
      volatile
    oscillator
      frequency: 20000000

game
  sha256:   b2aa2c0a621dfbed3b528de93282fc91abb16325d358680d34753d43629263cf
  label:    Mega Man X3
  name:     Mega Man X3
  region:   SNS-AR3E-USA
  revision: SNS-AR3E-0
  board:    SHVC-1DC0N-01
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: ROM
      size: 0xc00
      content: Data
      manufacturer: Hitachi
      architecture: HG51BS169
      identifier: Cx4
    memory
      type: RAM
      size: 0xc00
      content: Data
      manufacturer: Hitachi
      architecture: HG51BS169
      identifier: Cx4
      volatile
    oscillator
      frequency: 20000000

game
  sha256:   d4f2cb6b209db29f7aec62e5a23846681c14665fb007e94d7bcfc7b5611e938b
  label:    Metal Combat: Falcon's Revenge
  name:     Metal Combat - Falcon's Revenge
  region:   SNS-KD-USA
  revision: SNS-KD-0
  board:    SHVC-2E3M-01
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   0a9609a505dd1555006b16f53d961b3ce50c518aa1597a77dcd46e55ecc716ff
  label:    Metal Marines
  name:     Metal Marines
  region:   SNS-6M-USA
  revision: SNS-6M-0
  board:    SHVC-2A0N-01
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   057484558ebd18165f98e556b994080535e31cefdd98b5edb190516f7040fc9d
  label:    Metal Morph
  name:     Metal Morph
  region:   SNS-AMHE-USA
  revision: SNS-AMHE-0
  board:    SHVC-2A0N-11
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   0d7f875877fe856066cfb39b4ecdbbe7d48393a75770720876c94419f809bb1c
  label:    Metal Warriors
  name:     Metal Warriors
  region:   SNS-AWME-USA
  revision: SNS-AWME-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   88545d24c60c18c695cc3fce8c4074f46ad1bac905e8a39a61d8a6ae6b608621
  label:    Michael Andretti's Indy Car Challenge
  name:     Michael Andretti's Indy Car Challenge
  region:   SNS-AMAE-USA
  revision: SNS-AMAE-0
  board:    SHVC-1K0N-01
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: ROM
      size: 0x1800
      content: Program
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1
    memory
      type: ROM
      size: 0x800
      content: Data
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1
    memory
      type: RAM
      size: 0x200
      content: Data
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1
      volatile
    oscillator
      frequency: 7600000

game
  sha256:   1b425ea5a883b7464637b74c2937fde699ffff52b53ad6940a66285e0663194a
  label:    Michael Jordan: Chaos in the Windy City
  name:     Michael Jordan - Chaos in the Windy City
  region:   SNS-AWCE-USA
  revision: SNS-AWCE-0
  board:    SHVC-2J0N-11
    memory
      type: ROM
      size: 0x140000
      content: Program

game
  sha256:   0773eb741ce28f963f767fc7dd44678eb3d37ed4dc7fc82bb9cce7d55f1cfc64
  label:    Mickey Mania: The Timeless Adventures of Mickey Mouse
  name:     Mickey Mania - The Timeless Adventures of Mickey Mouse
  region:   SNS-AMIE-USA
  revision: SNS-AMIE-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   add082caffe707df2c0259489c3e272d6557ab07ba3ff856cbc0adba0d7db6a5
  label:    Mickey's Ultimate Challenge
  name:     Mickey's Ultimate Challenge
  region:   SNS-6U-USA
  revision: SNS-6U-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   a2adeb4bf0e7cc943611ac726e5578da404373a79e91436c9bbd15480688b15c
  label:    Micro Machines
  name:     Micro Machines
  region:   SNS-AH3E-USA
  revision: SNS-AH3E-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   835ded9479f0e7babae00405d85233d767e82fa247aa1a9cdc14fd1f147b62ef
  label:    Might & Magic III: Isles of Terra
  name:     Might & Magic III - Isles of Terra
  region:   SNS-3H-USA
  revision: SNS-3H-0
  board:    SHVC-2A3M-20
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   624a66607caef2ca34920ea15b84b28cdd1916ee089d496cec4f1d43621fdbb3
  label:    Mighty Morphin Power Rangers
  name:     Mighty Morphin Power Rangers
  region:   SNS-52-USA
  revision: SNS-52-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   1b85c0690aa156a255c7f79e133e453345452698fa98abf8df744c262d0cf865
  label:    Mighty Morphin Power Rangers: The Fighting Edition
  name:     Mighty Morphin Power Rangers - The Fighting Edition
  region:   SNS-A3RE-USA
  revision: SNS-A3RE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   c706b70097c679f6f0ced6f77a30620807d0b2555fc3c683c0ec2fc791176039
  label:    Mighty Morphin Power Rangers: The Movie
  name:     Mighty Morphin Power Rangers - The Movie
  region:   SNS-A2RE-USA
  revision: SNS-A2RE-0
  board:    SHVC-2A0N-20
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   8546dfc91f6256df8b21083531457e3edf0021da12fce6858e2c59ff239c31da
  label:    The Miracle Piano Teaching System
  name:     Miracle Piano Teaching System, The
  region:   SNS-MR-USA
  revision: SNS-MR-0
  board:    SHVC-1A0N-01
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   d32f194e27d177e64bf3bda046889b8198276fca2e8772e4b02a17152e9273e4
  label:    MLBPA Baseball
  name:     MLBPA Baseball
  region:   SNS-XH-USA
  revision: SNS-XH-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   8715a641f2e4dd8b6066be7f2683d9129fff3fcccaf0a09cc8bdd2aa56460764
  label:    Mohawk & Headphone Jack
  name:     Mohawk & Headphone Jack
  region:   SNS-AJYE-USA
  revision: SNS-AJYE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   480ae7186fd5b28200cd88e136b9cd3b6600d32508e280a0bc27ea0ed8d3c0bb
  label:    Monopoly
  name:     Monopoly
  region:   SNS-ML-USA
  revision: SNS-ML-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   6b0ac4d52d24536cdb7d9d0dc7d19ee30d08ac34363983290c5912ccc850fa0d
  label:    Monopoly
  name:     Monopoly
  region:   SNS-ML-USA
  revision: SNS-ML-1
  board:    MAXI-1A0N-30
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   3c6d3e4a9c9af160f1c1cf11ce4ead531d9500c1f58f1cbe682c90a5eaa3efb2
  label:    Mortal Kombat
  name:     Mortal Kombat
  region:   SNS-KX-USA
  revision: SNS-KX-0
  board:    SHVC-2A0N-10
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   417874aa57856fe93eefdb24066fa1a9ca3f23c72c09d5247ae2b3ab4b3d09d1
  label:    Mortal Kombat 3
  name:     Mortal Kombat 3
  region:   SNS-A3ME-USA
  revision: SNS-A3ME-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x400000
      content: Program

game
  sha256:   43e36a74fb73a7efc46b380599e269b1fff8f55ecf80f5cf50c34d02ceda041a
  label:    Mortal Kombat II
  name:     Mortal Kombat II
  region:   SNS-28-USA
  revision: SNS-28-0
  board:    SHVC-BJ0N-01
    memory
      type: ROM
      size: 0x300000
      content: Program

game
  sha256:   ca2f86ca77f822fcd8e86f5a287f2a76d0becbb81a7bce73ae22909beb2f834c
  label:    Mortal Kombat II
  name:     Mortal Kombat II
  region:   SNS-28-USA
  revision: SNS-28-1
  board:    SHVC-BJ0N-01
    memory
      type: ROM
      size: 0x300000
      content: Program

game
  sha256:   340293c06536d7b6981ad7c681e404f4390ff6c62340f844a4558877c1b82af0
  label:    Mr. Do!
  name:     Mr. Do!
  region:   SNS-AUNE-USA
  revision: SNS-AUNE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x40000
      content: Program

game
  sha256:   3472dd574b50aed2fa998f464398db4fbb00f5a300a672c3737ee9336a008a16
  label:    Mr. Nutz
  name:     Mr. Nutz
  region:   SNS-N8-USA
  revision: SNS-N8-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   007735e68a91cab403f1c955d9d562e9311124e660fa5b32e5c5d0a2e052160e
  label:    Ms. Pac-Man
  name:     Ms. Pac-Man
  region:   SNS-AN8E-USA
  revision: SNS-AN8E-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x40000
      content: Program

game
  sha256:   f292598ac462fdfcd32ad9b6b35ac01d4bab020391dff92bfe94780ec604289a
  label:    Musya: The Classic Japanese Tale of Horror
  name:     Musya - The Classic Japanese Tale of Horror
  region:   SNS-MY-USA
  revision: SNS-MY-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   8d4ada4f98464d176ae7f0fb8a20032056680f3241637a0f903f23f31f41ff36
  label:    Mutant Chronicles: Doom Troopers
  name:     Mutant Chronicles - Doom Troopers
  region:   SNS-AM9E-USA
  revision: SNS-AM9E-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   22d2260de02552b1205aac2ff5a202a4c80532ac28045ef5a058d88279ab758e
  label:    Natsume Championship Wrestling
  name:     Natsume Championship Wrestling
  region:   SNS-7W-USA
  revision: SNS-7W-0
  board:    SHVC-1J0N-10
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   c70b812a9d2df7f95b279e4050e03a4b8a68588a370816e645f378296b84e5d1
  label:    NBA All-Star Challenge
  name:     NBA All-Star Challenge
  region:   SNS-NB-USA
  revision: SNS-NB-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   5874b942f974bb906d8cbc33b601a1faf8f14aee8d0995124c8dc84bb4973808
  label:    NBA Give 'n Go
  name:     NBA Give & Go
  region:   SNS-ANJE-USA
  revision: SNS-ANJE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   af2fe0627de2bb2672f4f65dcbdaaee22b211275f679f123d5fa5d37fd699363
  label:    NBA Hang Time
  name:     NBA Hang Time
  region:   SNS-AXGE-USA
  revision: SNS-AXGE-0
  board:    SHVC-1J3M-20
    memory
      type: ROM
      size: 0x300000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   0f18c496426bb97fe5e8b91ad5299f0b1c3898ac17047b745c86b167c212ab7a
  label:    NBA Jam
  name:     NBA Jam
  region:   SNS-8N-USA
  revision: SNS-8N-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   b257cffb3484e6be051a56268cb99ee888bd6d3e9c0e8d6d0779ff66c411f6ba
  label:    NBA Jam: Tournament Edition
  name:     NBA Jam - Tournament Edition
  region:   SNS-AJTE-USA
  revision: SNS-AJTE-0
  board:    SHVC-BA1M-01
    memory
      type: ROM
      size: 0x300000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   2115c39f0580ce19885b5459ad708eaa80cc80fabfe5a9325ec2280a5bcd7870
  label:    NBA Live '95
  name:     NBA Live '95
  region:   SNS-ANBE-USA
  revision: SNS-ANBE-0
  board:    SHVC-2A3M-20
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   2d6fc4214245684a8f8f9bb553de637b7c660919ec775bfe3baaf74060c9157e
  label:    NBA Live '96
  name:     NBA Live '96
  region:   SNS-A6BE-USA
  revision: SNS-A6BE-0
  board:    SHVC-2A3M-20
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   4b945493d28653d5d047a375136ec9792f27e652be4af00e2c03c40369bd6ecf
  label:    NBA Live '97
  name:     NBA Live '97
  region:   SNS-A7LE-USA
  revision: SNS-A7LE-0
  board:    EA-1A3M-30
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   1d1b257dcf859cc412d3a9521fc58dc876a5917e6a69cd1d960a8e9840454bb4
  label:    NBA Live '98
  name:     NBA Live '98
  region:   SNS-A8LE-USA
  revision: SNS-A8LE-0
  board:    EA-1A3M-30
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   6a7324734004d99206439430243b51a05fa8c25ffa314dafc7f127235d1a730f
  label:    NBA Showdown
  name:     NBA Showdown
  region:   SNS-6N-USA
  revision: SNS-6N-0
  board:    SHVC-1A3M-10
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   351587366eca8aeb38236e4ad6cbd4a1b6e37b8cc592725a79249c3a054fa3a7
  label:    NCAA Basketball
  name:     NCAA Basketball
  region:   SNS-DU-USA
  revision: SNS-DU-0
  board:    SHVC-1A1B-06
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   8ef5d5c50ffeca1e62e88e4fe2909eaf191e28fbb5a9faf98b7b10bea72c9ed9
  label:    NCAA Basketball
  name:     NCAA Basketball
  region:   SNS-DU-USA
  revision: SNS-DU-1
  board:    SHVC-1A1B-06
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   d297c76efbcd9a528d8ee51025f8774ab2cb6bbc676c24e28592c50f47e71bfc
  label:    NCAA Final Four Basketball
  name:     NCAA Final Four Basketball
  region:   SNS-AFIE-USA
  revision: SNS-AFIE-0
  board:    SHVC-2A3M-20
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   8c43b7c2f2ef1fca9237c64c2f4c9e98d1a48940dae500ce8eac71278d34efb3
  label:    NCAA Football
  name:     NCAA Football
  region:   SNS-AFBE-USA
  revision: SNS-AFBE-0
  board:    SHVC-1A1M-11
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   489d6d704d3d917d9b46ce382230700419983a8d0d61a98fe07e08c935522dde
  label:    Newman-Haas IndyCar featuring Nigel Mansell
  name:     Newman-Haas IndyCar featuring Nigel Mansell
  region:   SNS-ANME-USA
  revision: SNS-ANME-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   f651a42bb351f7691b880827178c36fcf6c265c7833c6de1d94f7ed69bac0517
  label:    NFL Football
  name:     NFL Football
  region:   SNS-NF-USA
  revision: SNS-NF-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   6a59115a9958d4a9fa167095505a2ddf222ca6291209d07618319e39a2be8b61
  label:    NFL Quarterback Club
  name:     NFL Quarterback Club '95
  region:   SNS-Q9-USA
  revision: SNS-Q9-0
  board:    SHVC-1A1M-11
    memory
      type: ROM
      size: 0x300000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   f43f8ec546b8060e9d191fca860c38caf5a43eda86a304f0073647c6fad7b2c9
  label:    NFL Quarterback Club '96
  name:     NFL Quarterback Club '96
  region:   SNS-AQBE-USA
  revision: SNS-AQBE-0
  board:    SHVC-1A1M-20
    memory
      type: ROM
      size: 0x300000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   5132e1c0d466963e6adc09e8a608ebd90619ab94f7fc908d626bbaf6a99dfa19
  label:    NHL '94
  name:     NHL '94
  region:   SNS-4H-USA
  revision: SNS-4H-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   01c0b58d5fd5d5484fea988455a55a71ed9e606538d2b3ce3f216159cc6929b0
  label:    NHL '95
  name:     NHL '95
  region:   SNS-ANHE-USA
  revision: SNS-ANHE-0
  board:    SHVC-1J3M-11
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   d24c0175ee4eafed88e277691c5f5dafd4e197723097e2eb68aa6b40f449fff2
  label:    NHL '96
  name:     NHL '96
  region:   SNS-A6HE-USA
  revision: SNS-A6HE-0
  board:    SHVC-1J3M-20
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   2a2dc2ef84efd9a773d1e8231b7e3e57f0de7e4528968670963f2f1f358eef39
  label:    NHL '97
  name:     NHL '97
  region:   SNS-AH7E-USA
  revision: SNS-AH7E-0
  board:    SHVC-1J3M-20
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   0d933149242a2a5278b9ada9294481db5b30aaa134c660951dc340bf8ab441e8
  label:    NHL '97
  name:     NHL '97
  region:   SNS-AH7E-USA
  revision: SNS-AH7E-1
  board:    SHVC-1J3M-20
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   8113c2cedafc8fd5a56c8638ae340fb275f263ff5c5e18d04dc6c3ebc5cfffee
  label:    NHL '98
  name:     NHL '98
  region:   SNS-A8HE-USA
  revision: SNS-A8HE-0
  board:    SHVC-1J5M-20
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save

game
  sha256:   d44f487d84f5bb761955b7b70a5464b2f094e199875f595f312c88e04ac647ff
  label:    NHL Stanley Cup
  name:     NHL Stanley Cup
  region:   SNS-NH-USA
  revision: SNS-NH-0
  board:    SHVC-1A1M-01
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   55f3432a130085c112d65aa6443c41eb7a8aeec59aad2c2b4b2ac536b604b449
  label:    NHLPA Hockey '93
  name:     NHLPA Hockey '93
  region:   SNS-HY-USA
  revision: SNS-HY-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   d7ad6f67860da78fe25d9e79dd13af7ac7efaa0c8e0103898a4849ab4af9e438
  label:    Nickelodeon GUTS
  name:     Nickelodeon GUTS
  region:   SNS-ANGE-USA
  revision: SNS-ANGE-0
  board:    SHVC-2A0N-11
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   ce9c819d6496e58901b39d9b04558a89e09ccc3aac33690b8d02bb0406682a57
  label:    Nigel Mansell's World Championship Racing
  name:     Nigel Mansell's World Championship Racing
  region:   SNS-M8-USA
  revision: SNS-M8-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   fccc96af24a2463b1c53253e1c5c8ef63641355fae53c0fb410427f29743262b
  label:    Ninja Gaiden Trilogy
  name:     Ninja Gaiden Trilogy
  region:   SNS-ANRE-USA
  revision: SNS-ANRE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   3c109e50b91ec6df3bb8509778ae544c99433fb40dda9b801178dfe513053618
  label:    Ninja Warriors
  name:     Ninja Warriors
  region:   SNS-NI-USA
  revision: SNS-NI-0
  board:    SHVC-2A0N-11
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   f099937ac4c8afb38c517c5d85475224985fb8f345dacb44994a617ea05bf4e5
  label:    No Escape
  name:     No Escape
  region:   SNS-ANOE-USA
  revision: SNS-ANOE-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   437ea48711d54c2275286170a17948cb57ba9d961ba475715c0dba5fbd61a2db
  label:    Nobunga's Ambition
  name:     Nobunga's Ambition
  region:   SNS-NZ-USA
  revision: SNS-NZ-0
  board:    SHVC-1J3M-01
    memory
      type: ROM
      size: 0x80000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   ae572a51ebe429cfc06a3b75d7db9581f2884f9adc78827dc162b4c4ddc6ce2d
  label:    Nobunga's Ambition: Lord of Darkness
  name:     Nobunga's Ambition - Lord of Darkness
  region:   SNS-IZ-USA
  revision: SNS-IZ-0
  board:    SHVC-1A5M-11
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save

game
  sha256:   7f3d0ebac6ecfb99cfd1d5b13210e989df9e8b2f2319a63c42faef8ad115a964
  label:    Nolan Ryan's Baseball
  name:     Nolan Ryan's Baseball
  region:   SNS-NR-USA
  revision: SNS-NR-0
  board:    SHVC-1A3B-12
    memory
      type: ROM
      size: 0x80000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   9712829b38f23229d4e3d65da78237659c790235f425c6b12487e4d9e9a37ae9
  label:    Nosferatu
  name:     Nosferatu
  region:   SNS-NS-USA
  revision: SNS-NS-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   31bc862ab0a8eabf23b5124e13940cb3501e7ecdd3f15e34142248ceb4aa139a
  label:    Obitus
  name:     Obitus
  region:   SNS-7B-USA
  revision: SNS-7B-0
  board:    SHVC-1A3M-21
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   b766c26498d0afd63f44aefdef42642d2483b54f18d2b81a4f1d67a57f641044
  label:    Ogre Battle: The March of the Black Queen
  name:     Ogre Battle - The March of the Black Queen
  region:   SNS-OB-USA
  revision: SNS-OB-0
  board:    SHVC-2A3M-20
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   a137f4fc1c635f706d10e0c7927e71f52da171ce2d27394ce0deb451b5bed8ae
  label:    Olympic Summer Games
  name:     Olympic Summer Games
  region:   SNS-AO9E-USA
  revision: SNS-AO9E-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   e153195de7b59dd5b9854952cccca6bb93164e5fdff8292124bee6bbe5dbf16f
  label:    On the Ball
  name:     On the Ball
  region:   SNS-CT-USA
  revision: SNS-CT-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   9e1a39aaf3585af0d6a5d79de3c35fcfe030c34bd4b09606a6fb8c7def19340b
  label:    Operation Europe: Path to Victory 1939-45
  name:     Operation Europe - Path to Victory 1939-45
  region:   SNS-YP-USA
  revision: SNS-YP-0
  board:    SHVC-2J5M-01
    memory
      type: ROM
      size: 0x140000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save

game
  sha256:   190742792a950a112f893cba0e083eb787cf24293f698967defff929635ba0e7
  label:    Operation Logic Bomb
  name:     Operation Logic Bomb
  region:   SNS-IY-USA
  revision: SNS-IY-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   5cbed0401734142184166917427d24f9e5f107a7adea665e2f4b4101491ad54b
  label:    Operation Thunderbolt
  name:     Operation Thunderbolt
  region:   SNS-36-USA
  revision: SNS-36-0
  board:    SHVC-2J0N-11
    memory
      type: ROM
      size: 0x140000
      content: Program

game
  sha256:   0c08e6b817e4d0b333acb910a0bde3d79bd2dc188defc5df9a7c1233fa81c98d
  label:    Oscar
  name:     Oscar
  region:   SNS-AOZE-USA
  revision: SNS-AOZE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   582548dc86598a3557e9e3c27285c81964b006a954affe5c73948da5375ea11c
  label:    Out of This World
  name:     Out of This World
  region:   SNS-TW-USA
  revision: SNS-TW-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   54b2f03393109ac7fd36d8c7752f15a44d9607ab0187a371b853191db3592c01
  label:    Out to Lunch
  name:     Out to Lunch
  region:   SNS-P8-USA
  revision: SNS-P8-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   db44f8b58a31b640a47aa4390101c3c6a5f613e4e49c636d44786278033dec19
  label:    Outlander
  name:     Outlander
  region:   SNS-LD-USA
  revision: SNS-LD-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   10c8abce67b49f8afbe880d2f13e0fd6d5efc162df34d5941e4a94851f23b2ff
  label:    Pac-Attack
  name:     Pac-Attack
  region:   SNS-P9-USA
  revision: SNS-P9-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   7fe4cb9c294d66589ff78e225774471ecb7db80df25f2b6199ca25671358072b
  label:    Pac-in-Time
  name:     Pac-in-Time
  region:   SNS-APTE-USA
  revision: SNS-APTE-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   4cb52ba751c42d9e12ca429e5d657622a370b608002880a997f64de453f0de20
  label:    Pac-Man 2: The New Adventures
  name:     Pac-Man 2 - The New Adventures
  region:   SNS-25-USA
  revision: SNS-25-0
  board:    SHVC-2A0N-11
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   83727f6954bce024edac7f3fd18a6fbf63d05b580f692d96faa37e60893c91cd
  label:    P.T.O.: Pacific Theater of Operations
  name:     Pacific Theater of Operations
  region:   SNS-TK-USA
  revision: SNS-TK-0
  board:    SHVC-2A5M-01
    memory
      type: ROM
      size: 0x140000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save

game
  sha256:   2c6119fbe40d23576adde568d60ffae793bbf6b512f8fea8dcd0b1cd2623ef02
  label:    P.T.O. II: Pacific Theater of Operations
  name:     Pacific Theater of Operations II
  region:   SNS-ATEE-USA
  revision: SNS-ATEE-0
  board:    SHVC-1J5M-20
    memory
      type: ROM
      size: 0x300000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save

game
  sha256:   b1586c8ece4da9697f0602a684d7a9108027247a34652c3771831d31f82ee078
  label:    Packy & Marlon
  name:     Packy & Marlon
  region:   SNS-APYE-USA
  revision: SNS-APYE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   ecaf51852e111512385c5f209d51578e65b45fcaa17b295ca06120c8d1464520
  label:    The Pagemaster
  name:     Pagemaster, The
  region:   SNS-APME-USA
  revision: SNS-APME-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   46286d0839a4397fc4c067b39783f98d2aefeca870a468bae601a1434f1dde90
  label:    Paladin's Quest
  name:     Paladin's Quest
  region:   SNS-LN-USA
  revision: SNS-LN-0
  board:    SHVC-2A3M-01
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   7cec4ffc3eda0441561717cf55927901b5fbbd669c254079f78ca74c67c4a17b
  label:    Paperboy 2
  name:     Paperboy 2
  region:   SNS-P2-USA
  revision: SNS-P2-0
  board:    SHVC-1A0N-01
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   c414a4084b3d03aba19496d2efdd68fcf826194d8f1308f5c98e3a7af2fcc063
  label:    The Peace Keepers
  name:     Peace Keepers, The
  region:   SNS-R6-USA
  revision: SNS-R6-0
  board:    SHVC-1J0N-10
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   5abfb974ca0e56aabb3f4126817d14a546c57a5a5c6042d31196063d80996698
  label:    PGA European Tour
  name:     PGA European Tour
  region:   SNS-AEPE-USA
  revision: SNS-AEPE-0
  board:    SHVC-1L3B-11
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save
    memory
      type: RAM
      size: 0x800
      content: Internal
      volatile

game
  sha256:   571d3129350bfb7cca2285499cf31dda48d1047cf9eaef122c8c33dffa9ad296
  label:    PGA Tour '96
  name:     PGA Tour '96
  region:   SNS-A3GE-USA
  revision: SNS-A3GE-0
  board:    SHVC-1L3B-02
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save
    memory
      type: RAM
      size: 0x800
      content: Internal
      volatile

game
  sha256:   5c0b5266a191852ca593235f07180e673cb79e3f0b0dd31f65808eef83bf6e90
  label:    PGA Tour Golf
  name:     PGA Tour Golf
  region:   SNS-PG-USA
  revision: SNS-PG-0
  board:    SHVC-1A3B-12
    memory
      type: ROM
      size: 0x80000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   0663330bc061f4b768fa1806610878ef6e6cf546f36041ae087c8e55703693b8
  label:    Phalanx: The Enforce Fighter A-144
  name:     Phalanx - The Enforce Fighter A-144
  region:   SNS-PH-USA
  revision: SNS-PH-0
  board:    SHVC-1A0N-02
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   b7291088f5c49e1fc55bf932076ec03f7b39f6e409ae06e884b57024c56cdc87
  label:    Phantom 2040
  name:     Phantom 2040
  region:   SNS-A24E-USA
  revision: SNS-A24E-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   a0b39d7fd7c39c5b0f41f3542fb8d2887530ded1c111b4ffb2a863845e704ecc
  label:    Pieces
  name:     Pieces
  region:   SNS-Z5-USA
  revision: SNS-Z5-0
  board:    SHVC-1J0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   724144e34990069213591b5df067dd437a613b30f669840e9098db1dce626d2d
  label:    Pilotwings
  name:     Pilotwings
  region:   SNS-PW-USA
  revision: SNS-PW-0
  board:    SHVC-1B0N-02
    memory
      type: ROM
      size: 0x80000
      content: Program
    memory
      type: ROM
      size: 0x1800
      content: Program
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1
    memory
      type: ROM
      size: 0x800
      content: Data
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1
    memory
      type: RAM
      size: 0x200
      content: Data
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1
      volatile
    oscillator
      frequency: 7600000

game
  sha256:   3a52bf09850aa054dca443f7ea74d43f201dffecc40326924ecba9b0f1450e43
  label:    Pinball Dreams
  name:     Pinball Dreams
  region:   SNS-7D-USA
  revision: SNS-7D-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   0888d20ab2f834c77b0a2dc2162c43890a1640adc78c6b0bb5892ca8d5008ad3
  label:    Pinball Fantasies
  name:     Pinball Fantasies
  region:   SNS-APFE-USA
  revision: SNS-APFE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   d0f4a5040ecf96dc49aa0084160e291a38f2ee75319750db4d6687ab36828da9
  label:    Pink Goes to Hollywood
  name:     Pink Goes to Hollywood
  region:   SNS-YW-USA
  revision: SNS-YW-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   98c51c3bb577600fe79577c323333a791baa30904f37c695890e6e380b75e3c8
  label:    Pinocchio
  name:     Pinocchio
  region:   SNS-ACGE-USA
  revision: SNS-ACGE-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x300000
      content: Program

game
  sha256:   447dfa710e69479159e9d407474fbf5f67d3a3330ab0c7627afd123ded3fdb3a
  label:    The Pirates of Dark Water
  name:     Pirates of Dark Water, The
  region:   SNS-8P-USA
  revision: SNS-8P-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   c2a1a66648a0a0bfe2f201cf4f926d138e410fbf85ecf436ccb9aac70c0df3de
  label:    Pit-Fighter
  name:     Pit-Fighter
  region:   SNS-PF-USA
  revision: SNS-PF-0
  board:    SHVC-1A0N-01
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   e03d117d8b3093b0bbad5224638f85378b254b81eb304e506a732b4338802e0f
  label:    Pitfall: The Mayan Adventure
  name:     Pitfall - The Mayan Adventure
  region:   SNS-APAE-USA
  revision: SNS-APAE-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   0b3b0fd42bbe06a812053df376b183412fc0de335c4b7cb8e45f3fe47b0044e9
  label:    Plok
  name:     Plok
  region:   SNS-P4-USA
  revision: SNS-P4-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   72b2b3bead3fcd27a1610ad5d4d8be3235efeaff96df2e7858911992a5892d21
  label:    Pocky & Rocky
  name:     Pocky & Rocky
  region:   SNS-KK-USA
  revision: SNS-KK-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   cc33ae02114ea18a86592de327b2b4bcc80728b11a5e4c61666dca71480d4169
  label:    Pocky & Rocky 2
  name:     Pocky & Rocky 2
  region:   SNS-29-USA
  revision: SNS-29-0
  board:    SHVC-2A0N-11
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   5e580f220ed16281df8ee9a5f450b553f39f8c4078d3f3048d66bda15f98e19f
  label:    Populous
  name:     Populous
  region:   SNS-PO-USA
  revision: SNS-PO-0
  board:    SHVC-1A0N-01
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   ee9759fdb590ba908f569c2bb8a63703d282b58b84bd1fe0a472ea47685acdc5
  label:    Porky Pig's Haunted Holiday
  name:     Porky Pig's Haunted Holiday
  region:   SNS-APPE-USA
  revision: SNS-APPE-0
  board:    MAXI-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   06c8fc466805f97c9147711b2d8370d4f4d05d9fa3a916f17aa1682f73c9a63b
  label:    Power Instinct
  name:     Power Instinct
  region:   SNS-AGKE-USA
  revision: SNS-AGKE-0
  board:    SHVC-BJ0N-20
    memory
      type: ROM
      size: 0x280000
      content: Program

game
  sha256:   42ee7829d3db0213b718168c29674879bb4532573e9fb3450a5b417174a16ed0
  label:    Power Moves
  name:     Power Moves
  region:   SNS-P3-USA
  revision: SNS-P3-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   0288ec049723cd0c7f1148cdc1aef0b6922b8a756affe373c99d5690e0dfceaa
  label:    Power Piggs of the Dark Age
  name:     Power Piggs of the Dark Age
  region:   SNS-AOTE-USA
  revision: SNS-AOTE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   2e2a8c0da7f6def617077a25b222062f6988ef14b36b2edfe10d47c6a942d023
  label:    Power Rangers Zeo: Battle Racers
  name:     Power Rangers Zeo - Battle Racers
  region:   SNS-A4RE-USA
  revision: SNS-A4RE-0
  board:    SHVC-1L3B-11
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save
    memory
      type: RAM
      size: 0x800
      content: Internal
      volatile

game
  sha256:   8f387d083de1399bb79e5312c31a6f1757f2a536bfa25cecf1aea77bfd77058b
  label:    Prehistorik Man
  name:     Prehistorik Man
  region:   SNS-APUE-USA
  revision: SNS-APUE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   55376715f243b1bacd9aeecf1092bbc7837fe512592a2c1703d24b0829fc1934
  label:    Primal Rage
  name:     Primal Rage
  region:   SNS-AR9E-USA
  revision: SNS-AR9E-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x300000
      content: Program

game
  sha256:   494190cd6d7fd68882cbe255a6e237d9c4bdaf3988615ede0297a5e285ad5dd9
  label:    Prince of Persia
  name:     Prince of Persia
  region:   SNS-PR-USA
  revision: SNS-PR-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   04ca1a481093c4a7e12f18b33697d6e05e50e30e0f5b1655aa265abd14719bba
  label:    Prince of Persia 2: The Shadow and the Flame
  name:     Prince of Persia 2 - The Shadow and the Flame
  region:   SNS-AUPE-USA
  revision: SNS-AUPE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   85e2e58e1f99fce366c85d49f77446395584ca4959ef772a707fe454ed46c68f
  label:    Pro Quarterback
  name:     Pro Quarterback
  region:   SNS-QB-USA
  revision: SNS-QB-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   00b5b3d3febd1a6f28ceb49e39cdd9476a944fe62ea3850808cdeafaaaa63040
  label:    Pro Sport Hockey
  name:     Pro Sport Hockey
  region:   SNS-UI-USA
  revision: SNS-UI-0
  board:    SHVC-1A3M-20
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   475c9baa1c2b76a6b3119e47d32814dc1c08e84e23250ae015bb0bccea915637
  label:    Push-Over
  name:     Push-Over
  region:   SNS-PV-USA
  revision: SNS-PV-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   89d57bf308033ef17f770a80080cbeed2d112244635d5b5f860f2016398cd2f6
  label:    Q*bert 3
  name:     Q-bert 3
  region:   SNS-Q3-USA
  revision: SNS-Q3-0
  board:    SHVC-1A0N-02
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   4d6c7d6d2693d8d43bafaff7582f9a94885362dadd9ee4012bbbdce1ba10c30e
  label:    R-Type III: The Third Lightning
  name:     R-Type III - The Third Lightning
  region:   SNS-ER-USA
  revision: SNS-ER-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   dd0feb78e2d5d81f59241baf3bca5e2edaebbe98f0ac860a4eb6d448718f1ca5
  label:    Race Drivin'
  name:     Race Drivin'
  region:   SNS-RV-USA
  revision: SNS-RV-0
  board:    SHVC-1A0N-02
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   edf990e502c646a2fe83fcd1d359ca0ed5003ace06cb4c3de5a51a0c56d6ec54
  label:    Radical Psycho Machine Racing
  name:     Radical Psycho Machine Racing
  region:   SNS-RP-USA
  revision: SNS-RP-0
  board:    SHVC-1A1B-05
    memory
      type: ROM
      size: 0x80000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   1869c0faf93bf21b7ff965f1925fad4b2924a64b1e48f4837221eebdf741226c
  label:    Radical Rex
  name:     Radical Rex
  region:   SNS-ARRE-USA
  revision: SNS-ARRE-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   5fd7666e509f9d3cf1fd6b209dc22f2f3848f74eae7b83239d1090e031fc6df2
  label:    Raiden Trad
  name:     Raiden Trad
  region:   SNS-RD-USA
  revision: SNS-RD-0
  board:    SHVC-1A0N-02
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   e19f7d8d5c3e4cefeff5889380d8780495e01f0553d13be4182a15a5d4b615bb
  label:    Rampart
  name:     Rampart
  region:   SNS-RM-USA
  revision: SNS-RM-0
  board:    SHVC-1A0N-02
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   32d32ef56af83887cdc2c04b3da4be1cd82a473988deaa2e7dd50d38ef79c3a1
  label:    Ranma ½: Hard Battle
  name:     Ranma Half - Hard Battle
  region:   SNS-R2-USA
  revision: SNS-R2-0
  board:    SHVC-2J0N-01
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   920bbaaf6a32cab5feabb5bc8b2b427dccd53bfd31d0da8acb7ea4e819139e4f
  label:    Rap Jam: Volume One
  name:     Rap Jam - Volume One
  region:   SNS-ARVE-USA
  revision: SNS-ARVE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   097cbe9720903bc14599158b80e0cc314ef2fe8a585d6d0a8962eb1626031492
  label:    Realm
  name:     Realm
  region:   SNS-ARQE-USA
  revision: SNS-ARQE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   549f2e5b17f685cad25ba71ce7bc6e004e7bfd09e6be12a827af9a9a26556fff
  label:    Red Line: F-1 Racer
  name:     Red Line - F1 Racer
  region:   SNS-6R-USA
  revision: SNS-6R-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   b2221d0a59a399e437710e1191a37071d0a59c72d8e62427cd499de8d8fd7b61
  label:    Relief Pitcher
  name:     Relief Pitcher
  region:   SNS-5R-USA
  revision: SNS-5R-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   d322ce440076be0e3678277596acee8089098f4397b35ac8b3df88be5ce5e02f
  label:    The Ren & Stimpy Show: Buckeroo$!
  name:     Ren & Stimpy Show, The - Buckeroo$!
  region:   SNS-ARBE-USA
  revision: SNS-ARBE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   71e7083cfcf32b738f60f5eeffd4f9d1fd9250afbde0c56e22a4b97abac377a1
  label:    The Ren & Stimpy Show: Fire Dogs
  name:     Ren & Stimpy Show, The - Fire Dogs
  region:   SNS-6Y-USA
  revision: SNS-6Y-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   ad7dd4efb8836d4009f6c76bd21865d8f5dcf9c3cbd8fa7bb32d686488847120
  label:    The Ren & Stimpy Show: Time Warp
  name:     Ren & Stimpy Show, The - Time Warp
  region:   SNS-ARTE-USA
  revision: SNS-ARTE-0
  board:    SHVC-2A0N-11
    memory
      type: ROM
      size: 0x140000
      content: Program

game
  sha256:   ba54d715abf100b94fee801351986fa818e4309730cefbacf9b4fad36e284c1c
  label:    The Ren & Stimpy Show: Veediots!
  name:     Ren & Stimpy Show, The - Veediots!
  region:   SNS-V8-USA
  revision: SNS-V8-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   5fb072c3c2e9d8e7f84bea9c4bf2253e6868eb2b1f13e35a7d75fdf05896d873
  label:    Revolution X: Music is the Weapon
  name:     Revolution X - Music is the Weapon
  region:   SNS-AXRE-USA
  revision: SNS-AXRE-0
  board:    SHVC-2A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   f44482e2cccd9fcfd5875d84ff700f6e783f3bd8abd1ac4d939074cd6ad3fe65
  label:    Rex Ronan: Experimental Surgeon
  name:     Rex Ronan - Experimental Surgeon
  region:   SNS-XR-USA
  revision: SNS-XR-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   e3ea89964ee82d2ae1e5739b8503acf62732aedef28d1b3d5caa9ebae0feec06
  label:    Riddick Bowe Boxing
  name:     Riddick Bowe Boxing
  region:   SNS-XG-USA
  revision: SNS-XG-0
  board:    SHVC-1A1M-01
    memory
      type: ROM
      size: 0x80000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   cce455b7074f62ffdb41863ee8c160ea06f7acd028837666329bc1e0c3567ad0
  label:    Rise of the Phoenix
  name:     Rise of the Phoenix
  region:   SNS-QG-USA
  revision: SNS-QG-0
  board:    SHVC-2J3M-11
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   38be8013bbe07b2020ba30031fb0a2c77bad8a3eb61fac8217adfe82d6c402af
  label:    Rise of the Robots
  name:     Rise of the Robots
  region:   SNS-AROE-USA
  revision: SNS-AROE-0
  board:    SHVC-BJ0N-01
    memory
      type: ROM
      size: 0x400000
      content: Program

game
  sha256:   3f59cc687d22cd1b23cc33ae6e4518234c9da813c01f79f4c43716e12d32a12d
  label:    Rival Turf!
  name:     Rival Turf!
  region:   SNS-RB-USA
  revision: SNS-RB-0
  board:    SHVC-1A0N-01
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   864aa9068fb23cd20022a9ac36fb9082299278ea0cb07a20deec2b6a1c6cbc70
  label:    Road Riot 4WD
  name:     Road Riot 4WD
  region:   SNS-RR-USA
  revision: SNS-RR-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   2e8203e421f97cf165f03a5d4f69dadf0bcca18c42c6a1dfe79c8705c522cc54
  label:    Road Runner's Death Valley Rally
  name:     Road Runner's Death Valley Rally
  region:   SNS-DV-USA
  revision: SNS-DV-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   055d9c6311a663af7c899a6f76a419c274c57baada3ef64c52fadb1c676b1446
  label:    RoboCop 3
  name:     RoboCop 3
  region:   SNS-R3-USA
  revision: SNS-R3-0
  board:    SHVC-1A0N-02
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   a2115e7576dec06e0de613efb89de861815a78ef72e78a3784be09fb7541928f
  label:    RoboCop versus The Terminator
  name:     RoboCop vs. The Terminator
  region:   SNS-VR-USA
  revision: SNS-VR-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   1e2ded7b1e350449b7a99b7ec414525e4b9b086c416deeee5eb3e48e032c46bd
  label:    Robotrek
  name:     Robotrek
  region:   SNS-E9-USA
  revision: SNS-E9-0
  board:    SHVC-2J3M-11
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   9d721753301278325c851f1843d669a697aed757dcf6495a31fc31ddf664b182
  label:    Rock n' Roll Racing
  name:     Rock & Roll Racing
  region:   SNS-RN-USA
  revision: SNS-RN-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   b072fd9b08042e3262446fdf418a41848251072a32bd7f8335cc03543c4ae6c8
  label:    The Rocketeer
  name:     Rocketeer, The
  region:   SNS-RK-USA
  revision: SNS-RK-0
  board:    SHVC-1A0N-02
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   cde8876b99c337ff932322506ceef05519e5882b853c54bb8c650d9500cd5957
  label:    Rocko's Modern Life: Spunky's Dangerous Day
  name:     Rocko's Modern Life - Spunky's Dangerous Day
  region:   SNS-8D-USA
  revision: SNS-8D-0
  board:    SHVC-2A0N-11
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   7c0f915b581796e5b6dd384ecdc0dad8af4d956492fbcedec628c8845d911d7e
  label:    Rocky Rodent
  name:     Rocky Rodent
  region:   SNS-NP-USA
  revision: SNS-NP-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   e21300c8c4170e084fd83ce4c842dd73f939fbd10ddfe47c9a49b6e291dcd52d
  label:    Roger Clemens' MVP Baseball
  name:     Roger Clemens' MVP Baseball
  region:   SNS-VP-USA
  revision: SNS-VP-0
  board:    SHVC-1A0N-02
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   f7e3c3012af2dbad350646b6ef3470f0b4c42e4a2873109f7aa6c81d7157c887
  label:    Roger Clemens' MVP Baseball
  name:     Roger Clemens' MVP Baseball
  region:   SNS-VP-USA
  revision: SNS-VP-1
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   815bfcf4fd6eb23a20c2e50dde023c210b273ffb6cd86a93909d803c3643ce46
  label:    Romance of the Three Kingdoms II
  name:     Romance of the Three Kingdoms II
  region:   SNS-XL-USA
  revision: SNS-XL-0
  board:    SHVC-1A5B-04
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save

game
  sha256:   88fa0a78ca98c6386e086c7fa9e81a2625bdecc55115dde6c6f4770b0670aa40
  label:    Romance of the Three Kingdoms III: Dragon of Destiny
  name:     Romance of the Three Kingdoms III - Dragon of Destiny
  region:   SNS-S3-USA
  revision: SNS-S3-0
  board:    SHVC-2J5M-01
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save

game
  sha256:   9670c67eeeb5581fa5345025a2e2ee875179e0da9f5be4bf78641e477d785f17
  label:    Romance of the Three Kingdoms IV: Wall of Fire
  name:     Romance of the Three Kingdoms IV - Wall of Fire
  region:   SNS-AS4E-USA
  revision: SNS-AS4E-0
  board:    SHVC-1J5M-20
    memory
      type: ROM
      size: 0x300000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save

game
  sha256:   4158e3e8890a52f0b12dc9ad5a29276058a247ff41e9f1d22897ebde1eb11269
  label:    Run Saber
  name:     Run Saber
  region:   SNS-RU-USA
  revision: SNS-RU-0
  board:    SHVC-YA0N-01
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   5db804171fca42486485ed85e4afe45b29e6d01304bdf75d520bfc42429739e3
  label:    Samurai Shodown
  name:     Samurai Shodown
  region:   SNS-A7SE-USA
  revision: SNS-A7SE-0
  board:    SHVC-BJ0N-01
    memory
      type: ROM
      size: 0x400000
      content: Program

game
  sha256:   34e1af0642c85148c5a3dc3c7ab4bcbda13a9fea190934b5526c555fff035651
  label:    Saturday Night Slam Masters
  name:     Saturday Night Slam Masters
  region:   SNS-ZW-USA
  revision: SNS-ZW-0
  board:    SHVC-BJ0N-01
    memory
      type: ROM
      size: 0x300000
      content: Program

game
  sha256:   7fb5236d10852125f0f37c2188b907d636647400a57bccbdb2f63098ffae8b2d
  label:    Scooby-Doo Mystery
  name:     Scooby-Doo Mystery
  region:   SNS-AXDE-USA
  revision: SNS-AXDE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   a4ab8cfad2f236675b1c0124f8484688e149f38e8628a3b38e9ec14d491ec07e
  label:    SeaQuest DSV
  name:     SeaQuest DSV
  region:   SNS-ASQE-USA
  revision: SNS-ASQE-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   17c864a76d498feb6479eee8e7d6807b951c66225033228622bb66754baab1db
  label:    Secret of Evermore
  name:     Secret of Evermore
  region:   SNS-AEOE-USA
  revision: SNS-AEOE-0
  board:    SHVC-1J3M-20
    memory
      type: ROM
      size: 0x300000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   4c15013131351e694e05f22e38bb1b3e4031dedac77ec75abecebe8520d82d5f
  label:    Secret of Mana
  name:     Secret of Mana
  region:   SNS-K2-USA
  revision: SNS-K2-0
  board:    SHVC-1J3M-01
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   e6bc0a595d5c7c4bc0bbb61ffe35a70288a77eb78544ed74682d489a9e6f07f4
  label:    Shadowrun
  name:     Shadowrun
  region:   SNS-WR-USA
  revision: SNS-WR-0
  board:    SHVC-1A3M-10
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   bdbcc53b266159b5a08e307e5b60fdb0cb5a1ba8f8c1c6c7f89d81eaf5133e85
  label:    Shanghai II: Dragon's Eye
  name:     Shanghai II - Dragon's Eye
  region:   SNS-DE-USA
  revision: SNS-DE-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   c73757eea258e169e506eaef989227a59918060f94117917f338183db14c50b6
  label:    Shaq-Fu
  name:     Shaq-Fu
  region:   SNS-AQFE-USA
  revision: SNS-AQFE-0
  board:    SHVC-1J0N-10
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   454c580498a7d317ed61cb5863b1999eff57ad440ecd30ebb76e193c9c52f3ac
  label:    Shien's Revenge
  name:     Shien's Revenge
  region:   SNS-OO-USA
  revision: SNS-OO-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   de2d5a952096c5f50368b9270d342aa6e7a39007ffbec27117e182e30ef4cf32
  label:    Sid Meier's Civilization
  name:     Sid Meier's Civilization
  region:   SNS-EQ-USA
  revision: SNS-EQ-0
  board:    SHVC-1J5M-20
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save

game
  sha256:   d09ca5adaee65cfd686742482bc55b1a3ce9bc5ebed61f24c5631555151a7fc7
  label:    Side Pocket
  name:     Side Pocket
  region:   SNS-4P-USA
  revision: SNS-4P-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   c0bd1b378337c32047a6b7122a3813beb646e496fbdb1fa5c87ab9856271e4c5
  label:    SimAnt: The Electronic Ant Colony
  name:     SimAnt - The Electronic Ant Colony
  region:   SNS-AN-USA
  revision: SNS-AN-0
  board:    SHVC-1A5M-01
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save

game
  sha256:   e9c0bc05511e05a0d7c3e7cc42e761e1e8e532d46f59b9854b6902e1a2e9dd0a
  label:    SimCity
  name:     SimCity
  region:   SNS-SC-USA
  revision: SNS-SC-0
  board:    SHVC-1A5B-02
    memory
      type: ROM
      size: 0x80000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save

game
  sha256:   bf74c58e4190faca2f3a967dc190fe529d13887d1262b72e057b5353e43cf67f
  label:    SimCity 2000
  name:     SimCity 2000
  region:   SNS-AWWE-USA
  revision: SNS-AWWE-0
  board:    SHVC-1J5M-20
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save

game
  sha256:   446a1036d036986fdea7906c83832d3ba79ef63a6ed8c4e88b89ab9cb25daded
  label:    SimEarth: The Living Planet
  name:     SimEarth - The Living Planet
  region:   SNS-SE-USA
  revision: SNS-SE-0
  board:    SHVC-1A3B-20
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   f0d98e9061d0f6a193bb856de8a592f336dada97c41966e8d03119ba97465413
  label:    The Simpsons: Bart's Nightmare
  name:     Simpsons, The - Bart's Nightmare
  region:   SNS-BN-USA
  revision: SNS-BN-0
  board:    SHVC-1A0N-02
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   29c28188234ddbb0b72fc84253dcd3514e23034779c773db8097b073b73390c8
  label:    The Simpsons: Virtual Bart
  name:     Simpsons, The - Virtual Bart
  region:   SNS-AVBE-USA
  revision: SNS-AVBE-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   70008efe51185eb0a2f8d8d8ac2bdbb99bd3dfcc169dcc474962f82692998051
  label:    S.O.S.: Sink or Swim
  name:     Sink or Swim
  region:   SNS-9J-USA
  revision: SNS-9J-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   e10070f01845505ae8bfdf7b5b492e7209c2ae876f169fb6ff420dea269f4da3
  label:    Skuljagger: Revolt of the Westicans
  name:     Skuljagger - Revolt of the Westicans
  region:   SNS-SL-USA
  revision: SNS-SL-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   a4ba1483db79c3f6278082387bce216d8f3e3b11ca32d49516d27f5ac07135a5
  label:    Skyblazer
  name:     Skyblazer
  region:   SNS-LZ-USA
  revision: SNS-LZ-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   cbca00fa5dfd6c72db2f21d010255657c33f7ac48de2554262035ead11bdf314
  label:    Smart Ball
  name:     Smart Ball
  region:   SNS-JB-USA
  revision: SNS-JB-0
  board:    SHVC-1A0N-01
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   f653b8308b617e1cba1caf004e5a670088b882e320fa6afcc7a29dd3b2dd0ea0
  label:    Snow White in Happily Ever After
  name:     Snow White in Happily Ever After
  region:   SNS-ASHE-USA
  revision: SNS-ASHE-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   6fe7c8d39fcfab7f0a18e837a7ee0dd162e0557d6989c6e0d10c81616d3a0b8b
  label:    Soldiers of Fortune
  name:     Soldiers of Fortune
  region:   SNS-UD-USA
  revision: SNS-UD-0
  board:    SHVC-2J0N-01
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   75a7b5b8ad0329dc828d3201089e125fd55fdfc99d4cec704ffcd7e3036c2410
  label:    Sonic Blast Man
  name:     Sonic Blast Man
  region:   SNS-SK-USA
  revision: SNS-SK-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   efe78f6fc68ddd0f6ef0ad9e0223d9417c14fcadece987dc8f50423fd6723b27
  label:    Sonic Blast Man II
  name:     Sonic Blast Man II
  region:   SNS-2C-USA
  revision: SNS-2C-0
  board:    SHVC-2J0N-11
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   8f0a7670fe53d30f721784e5fff78349eb157a9f0eb2246206f9d7db478b7c56
  label:    SOS
  name:     SOS
  region:   SNS-TT-USA
  revision: SNS-TT-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   8438da09de8ce9aded3bb08644543f7b60fb60cffc68ce2d67d6a0643f2ecfc2
  label:    Soul Blazer
  name:     Soul Blazer
  region:   SNS-SO-USA
  revision: SNS-SO-0
  board:    SHVC-1A3B-13
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   85887dfa92374048fb20809c00eabea428992024cf875d287d0431b9767cc7cb
  label:    Space Ace
  name:     Space Ace
  region:   SNS-5A-USA
  revision: SNS-5A-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   24f3f22949f36ebf8ab4beaa8cba22db107efe7a7585f749343f2860bf041fe1
  label:    Space Football: One on One
  name:     Space Football - One on One
  region:   SNS-FL-USA
  revision: SNS-FL-0
  board:    SHVC-1A0N-02
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   dc5353ddc350816619230f25f8c51bddabf7438e6dfba21662eb1c4794856735
  label:    Space Invaders
  name:     Space Invaders
  region:   SNS-IC-USA
  revision: SNS-IC-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x40000
      content: Program

game
  sha256:   f5b7418c00ccac44615cfc57c7e17d57533837056886f6d733e6b714c36dec1f
  label:    Space Megaforce
  name:     Space Megaforce
  region:   SNS-AT-USA
  revision: SNS-AT-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   14dc44687c8da35aec63b9edadbaac21bf7293f5171646f614139192e82ab071
  label:    Spanky's Quest
  name:     Spanky's Quest
  region:   SNS-HJ-USA
  revision: SNS-HJ-0
  board:    SHVC-1A0N-02
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   eaa06470734ea57eff9b888137aa468fcb7bb149a0870a85e68c9db123de4670
  label:    Sparkster
  name:     Sparkster
  region:   SNS-ASSE-USA
  revision: SNS-ASSE-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   32d0f1ca5b91fd9b2caf81422fb9e8fb30bc091f0b2a429b9269dd307fcba4fd
  label:    Spawn
  name:     Spawn
  region:   SNS-A9WE-USA
  revision: SNS-A9WE-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x300000
      content: Program

game
  sha256:   22d0b7687d66436abaf447bb29558e43f562283ec4dbe744df7d79a3e27a2577
  label:    Spectre
  name:     Spectre
  region:   SNS-7Q-USA
  revision: SNS-7Q-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0xe0000
      content: Program

game
  sha256:   54046ec1fc57f6165ad33080502f72809d1b06b438a5a0d0a80ffa2bb3df8b0b
  label:    Speed Racer in My Most Dangerous Adventures
  name:     Speed Racer in My Most Dangerous Adventures
  region:   SNS-9S-USA
  revision: SNS-9S-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   df02d0f4f40e2732138309d38e91b48aef482490979007ecb63359a35115dfd4
  label:    Speedy Gonzales: Los Gatos Bandidos
  name:     Speedy Gonzales - Los Gatos Bandidos
  region:   SNS-ASEE-USA
  revision: SNS-ASEE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   68a51b7a06b6a9e7100a89521e52b5c467c46c828c0f6504bee677beac2aa6fd
  label:    Speedy Gonzales: Los Gatos Bandidos
  name:     Speedy Gonzales - Los Gatos Bandidos
  region:   SNS-ASEE-USA
  revision: SNS-ASEE-1
  board:    MAXI-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   964d21996e385e032b5d18baf716692ba1db780245cd71956c212045c1b8eb9a
  label:    Spider-Man & Venom: Maximum Carnage
  name:     Spider-Man & Venom - Maximum Carnage
  region:   SNS-AMCE-USA
  revision: SNS-AMCE-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   b72fbbfe737eff49f59dcef9f13b963e50c5bc322d7eb0e7b4c25f3a71aa0815
  label:    Spider-Man & Venom: Separation Anxiety
  name:     Spider-Man & Venom - Separation Anxiety
  region:   SNS-A2CE-USA
  revision: SNS-A2CE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x300000
      content: Program

game
  sha256:   63210a91573fa8e19592f2e6c746a400831d804c00453739447d2df32e731df7
  label:    Spider-Man & X-Men: Arcade's Revenge
  name:     Spider-Man & X-Men - Arcade's Revenge
  region:   SNS-MN-USA
  revision: SNS-MN-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   f05d777e3de69aab18d336cac0af07f794f8d00090d085f86cebaed3679cabad
  label:    Spider-Man: The Animated Series
  name:     Spider-Man - The Animated Series
  region:   SNS-ADME-USA
  revision: SNS-ADME-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   fe10238ae42ed9eb4d906a81dd50ebe585140982cdfe266308ce1f16e78e6903
  label:    Spindizzy Worlds
  name:     Spindizzy Worlds
  region:   SNS-SX-USA
  revision: SNS-SX-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   8d3f20af0be588cf2edc39aee0d96d4085512919a05c0e90a7848e414cc20e42
  label:    The Sporting News Baseball
  name:     Sporting News Baseball, The
  region:   SNS-AWBE-USA
  revision: SNS-AWBE-0
  board:    SHVC-1J3M-20
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   254d17a82a32d8bd231ca3a87d356b65e65cb0229902a69a57c21a4c99bbba1f
  label:    Sports Illustrated: Championship Football & Baseball
  name:     Sports Illustrated - Championship Football & Baseball
  region:   SNS-LU-USA
  revision: SNS-LU-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   3857b5294ea8f7468849437bb2d8271564e8a0ff30774622e9c872bcbd53a84d
  label:    Star Fox
  name:     Star Fox
  region:   SNS-FO-USA
  revision: SNS-FO-0
  board:    SHVC-1C0N5S-01
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save
      volatile

game
  sha256:   82e39dfbb3e4fe5c28044e80878392070c618b298dd5a267e5ea53c8f72cc548
  label:    Star Fox
  name:     Star Fox
  region:   SNS-FO-USA
  revision: SNS-FO-2
  board:    SHVC-1C0N
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save
      volatile

game
  sha256:   2c0bac12a7866fad1cb306da768a201c12f2520332df1ef51cba1576db21ff06
  label:    Star Fox: Super Weekend
  name:     Star Fox - Super Weekend
  region:   SNS-FU-USA
  revision: SNS-FU-0
  board:    SHVC-1C0N
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save
      volatile

game
  sha256:   3a16ad45ae3d89b13c9e53e21c2a4c725ff7cec7fbe7896d538d163f92cb4aac
  label:    Star Trek: Deep Space Nine - Crossroads of Time
  name:     Star Trek - Deep Space Nine - Crossroads of Time
  region:   SNS-A9DE-USA
  revision: SNS-A9DE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   b4d0b4a3fd73469f5469dfcc2316e50848ebf0630a225df2969c740759e321f4
  label:    Star Trek: Starfleet Academy - Starship Bridge Simulator
  name:     Star Trek - Starfleet Academy - Starship Bridge Simulator
  region:   SNS-ASTE-USA
  revision: SNS-ASTE-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   22c907b56ac6f414365801ed375c3fbf75696ce7f34ec89e1724628dc5622957
  label:    Star Trek: The Next Generation - Future's Past
  name:     Star Trek - The Next Generation - Future's Past
  region:   SNS-XN-USA
  revision: SNS-XN-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   9ffd95486904804aca1f7068c99f1c9e91c2c0e5474ec758df1a913393571cc7
  label:    Stargate
  name:     Stargate
  region:   SNS-AGTE-USA
  revision: SNS-AGTE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   91f938b4989215b1cd39635797f23b59b9d7b6d36e583f9eb69d022abe537bfc
  label:    Steel Talons
  name:     Steel Talons
  region:   SNS-5S-USA
  revision: SNS-5S-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   e3b07a59f969ced91c4579bb459f2c747a6c3f12c45ae4776483ef4830f5f00f
  label:    Sterling Sharpe: End 2 End
  name:     Sterling Sharpe - End 2 End
  region:   SNS-AS2E-USA
  revision: SNS-AS2E-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   dad9c116283322d5a13fd659874c681582abdff3df182cc4c90511d33fb7110a
  label:    Stone Protectors
  name:     Stone Protectors
  region:   SNS-ASOE-USA
  revision: SNS-ASOE-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   b4626cf0c876a124b50f9421c48a7d762e9ed808ad336c799d543d60b484897c
  label:    Street Combat
  name:     Street Combat
  region:   SNS-RA-USA
  revision: SNS-RA-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   910a29f834199c63c22beddc749baba746da9922196a553255deade59f4fc127
  label:    Street Fighter Alpha 2
  name:     Street Fighter Alpha 2
  region:   SNS-AUZE-USA
  revision: SNS-AUZE-0
  board:    SHVC-1N0N-01
    memory
      type: ROM
      size: 0x400000
      content: Program

game
  sha256:   2b34161e96ef3f0f48cecd67e531a9bb94310652d8686f301bac426e4ab97e77
  label:    Street Fighter II: The World Warrior
  name:     Street Fighter II - The World Warrior
  region:   SNS-S2-USA
  revision: SNS-S2-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   3e487f8ba48c0b5e31744e3281d6bce375089db6075c8eb3d9a929376b817381
  label:    Street Fighter II Turbo: Hyper Fighting
  name:     Street Fighter II Turbo - Hyper Fighting
  region:   SNS-TI-USA
  revision: SNS-TI-0
  board:    SHVC-3J0N-01
    memory
      type: ROM
      size: 0x280000
      content: Program

game
  sha256:   6756f92fe8e066be4b204cfdc94c1615ba6ece7e78fb5d2c77c81a9d298aa744
  label:    Street Hockey '95
  name:     Street Hockey '95
  region:   SNS-AHYE-USA
  revision: SNS-AHYE-0
  board:    SHVC-BJ0N-01
    memory
      type: ROM
      size: 0x300000
      content: Program

game
  sha256:   d1f61b6bb4bb6879a4fbd2c82d77390c546ee7f821edddc884fb9cc7463ad79b
  label:    Street Racer
  name:     Street Racer
  region:   SNS-ASRE-USA
  revision: SNS-ASRE-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   05f14e6ed3394d9273e2397769a8acf1a9db646be6066e82269521e8eec53562
  label:    Strike Gunner S.T.G.
  name:     Strike Gunner STG
  region:   SNS-SG-USA
  revision: SNS-SG-0
  board:    SHVC-1A0N-02
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   c04d80b84514202ff319384ca20641eb0189e975eed5612915bd9c224b2ab30a
  label:    Stunt Race FX
  name:     Stunt Race FX
  region:   SNS-CQ-USA
  revision: SNS-CQ-1
  board:    SHVC-1CA6B-01
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x10000
      content: Save
    oscillator
      frequency: 21440000

game
  sha256:   e9c406d4f773697b9b671e7ddf2207c9d0ab242d7f23e502cdd453fbb264d392
  label:    Sunset Riders
  name:     Sunset Riders
  region:   SNS-6S-USA
  revision: SNS-6S-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   190999122aacc2cff20c5677b3f60ed938d8a36b696d16cc1bf416705efe151e
  label:    Super Adventure Island
  name:     Super Adventure Island
  region:   SNS-H2-USA
  revision: SNS-H2-0
  board:    SHVC-1A0N-01
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   eaf1b83e95d8a04f9a84d4960cf87cc182fc60ef07be35eb8929c4033d6fef67
  label:    Super Adventure Island II
  name:     Super Adventure Island II
  region:   SNS-E4-USA
  revision: SNS-E4-0
  board:    SHVC-2J3M-11
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   0deb7a91fbe5848f1733ce668daaa49b0dad3d821bacc0791837c1ba15e60d7c
  label:    Super Alfred Chicken
  name:     Super Alfred Chicken
  region:   SNS-8A-USA
  revision: SNS-8A-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   b64f9f95fb244feddb3ff50bf0972af88f5d006e9b09050b930374fef8b37862
  label:    The Super Aquatic Games starring the Aquabats
  name:     Super Aquatic Games starring the Aquabats, The
  region:   SNS-AU-USA
  revision: SNS-AU-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   8808783f789ca6413364a7abea240f6f7291b5906026f360ba8cfdd2791fc179
  label:    Super Baseball 2020
  name:     Super Baseball 2020
  region:   SNS-SA-USA
  revision: SNS-SA-0
  board:    SHVC-2J0N-01
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   1622371a5a4001fff9690323e89b7a8d449cdc3cae6dcd1249f0c7dc8c651d33
  label:    Super Baseball Simulator 1.000
  name:     Super Baseball Simulator 1.000
  region:   SNS-UB-USA
  revision: SNS-UB-0
  board:    SHVC-1A3B-12
    memory
      type: ROM
      size: 0x80000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   a8239355631d303ecebfd43fc14e80f148e4ac9937234e29cc87d6f939b033a0
  label:    Super Bases Loaded
  name:     Super Bases Loaded
  region:   SNS-SP-USA
  revision: SNS-SP-0
  board:    SHVC-1A0N-01
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   327745aceb708d82419c3d2cffdffa78f55df0878bb1786e1b7fc294385f20ba
  label:    Super Bases Loaded 2
  name:     Super Bases Loaded 2
  region:   SNS-3D-USA
  revision: SNS-3D-0
  board:    SHVC-2B3B-01
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save
    memory
      type: ROM
      size: 0x1800
      content: Program
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1
    memory
      type: ROM
      size: 0x800
      content: Data
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1
    memory
      type: RAM
      size: 0x200
      content: Data
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1
      volatile
    oscillator
      frequency: 7600000

game
  sha256:   b21a161fed748920e54cd72c54095416b1d999636e0388d7d147884779c52833
  label:    Super Bases Loaded 3: License to Steal
  name:     Super Bases Loaded 3 - License to Steal
  region:   SNS-AB3E-USA
  revision: SNS-AB3E-0
  board:    SHVC-1A3M-30
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   165938810948f3226f7446978fa36ae8bc781616d95b39cd126d5c8afbf6e2ee
  label:    Super Batter Up
  name:     Super Batter Up
  region:   SNS-FA-USA
  revision: SNS-FA-0
  board:    SHVC-1A0N-02
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   1d6573e3db2efab0c98d1940b4171d569348d27a1cc20e80a83094e6c0e66e35
  label:    Super Battleship
  name:     Super Battleship
  region:   SNS-8B-USA
  revision: SNS-8B-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   94496e73fc7fdf2f72f16bf2becb0c3935db2ebd97555eac73b63400acbceec6
  label:    Super Battletank: War in the Gulf
  name:     Super Battletank - War in the Gulf
  region:   SNS-SB-USA
  revision: SNS-SB-0
  board:    SHVC-1A0N-02
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   b68e865b0b5fe6af421a171e94fb1cb0006ae3e412b6361f6f858c44adaa304b
  label:    Super Battletank 2
  name:     Super Battletank 2
  region:   SNS-2X-USA
  revision: SNS-2X-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   f0913358cb0870c3dcc7f0c0d2d69704874c31a113150eda668c95024c0d6fd9
  label:    Super Black Bass
  name:     Super Black Bass
  region:   SNS-BQ-USA
  revision: SNS-BQ-0
  board:    MAXI-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   4efab3f49cbe91ec77b6cba747ddfedfdc0b080c755a8b6ba51234f0676c000f
  label:    Super Bomberman
  name:     Super Bomberman
  region:   SNS-H6-USA
  revision: SNS-H6-0
  board:    SHVC-1J0N-10
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   0a4b4a783a7faf6ada3e1326ecf85de77e8c2a171659b42a78a1fae43f806ca6
  label:    Super Bomberman 2
  name:     Super Bomberman 2
  region:   SNS-M4-USA
  revision: SNS-M4-0
  board:    SHVC-1J0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   21d4a72461d8680cf75cf3b8eba42e13127815bc17b6249d89a5e39beb3f1406
  label:    Super Bonk
  name:     Super Bonk
  region:   SNS-ANKE-USA
  revision: SNS-ANKE-0
  board:    SHVC-2J0N-11
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   946de556b4f877e54e16b5c828db89c038e50349cfd0ddf8ea96b6541f9d70fa
  label:    Super Bowling
  name:     Super Bowling
  region:   SNS-BW-USA
  revision: SNS-BW-0
  board:    SHVC-1A0N-02
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   5965bde449ff775c1a0d9fd3cf2fb8c51a86b44ad1942dfb5c14a91f103be030
  label:    Super Buster Brothers
  name:     Super Buster Brothers
  region:   SNS-SN-USA
  revision: SNS-SN-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   d42f8c7969b4c434f9ca04ce0080d897877a5e71c2926d309ef5dae93ba25548
  label:    Super Caesers Palace
  name:     Super Caesers Palace
  region:   SNS-C6-USA
  revision: SNS-C6-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   0ef6f4cce5a2273fa49fe1ce724e0048a8e39c91da6b00dbb693fe1ba909177d
  label:    Super Castlevania IV
  name:     Super Castlevania IV
  region:   SNS-AD-USA
  revision: SNS-AD-0
  board:    MAXI-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   b839253b878821ff00847491d11452e933baaf303f49dd39d22e3a524ea1ff81
  label:    Super Chase H.Q.
  name:     Super Chase HQ
  region:   SNS-QT-USA
  revision: SNS-QT-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   6cbc4db85cb8420020f9edbfd290c0f435b313a59577aca65314d31f8b816620
  label:    Super Conflict
  name:     Super Conflict
  region:   SNS-CN-USA
  revision: SNS-CN-0
  board:    SHVC-1A1B-06
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   db7d727de86a1ac886ce3c243bf5941910154eaef3be50209187b9711592830d
  label:    Super Copa
  name:     Super Copa
  region:   SNS-75-USA
  revision: SNS-75-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   bcced1be76ef920b562a555696bcb4583d1c8cea4d4b057cab6e0e09be8ef8c4
  label:    Super Double Dragon
  name:     Super Double Dragon
  region:   SNS-WD-USA
  revision: SNS-WD-0
  board:    SHVC-1A0N-02
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   4d7fc331a811b8dc630b469262fd6f45e289243cef83101f32038158967d1b28
  label:    Super Game Boy
  name:     Super Game Boy
  region:   SNS-A-SG-USA
  revision: SYS-SGB-2
  board:    SGB-R-10
    memory
      type: ROM
      size: 0x40000
      content: Program
    memory
      type: ROM
      size: 0x100
      content: Boot
      manufacturer: Nintendo
      architecture: SM83
      identifier: SGB1

game
  sha256:   7468c271d7240cf4e0d08c16e9969a1b1b1caf5adc0e5adc568d93c92651a057
  label:    Super Ghouls 'n Ghosts
  name:     Super Ghouls & Ghosts
  region:   SNS-CM-USA
  revision: SNS-CM-0
  board:    SHVC-1A0N-01
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   d4caa6683ee1b6c70c10fd0555ade33dadcc6551d94e791586e64fd683d8f3a8
  label:    Super Goal! 2
  name:     Super Goal! 2
  region:   SNS-JV-USA
  revision: SNS-JV-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   33dda5838264c93341ef865512e4b86e16fd4a0387eda04e331517bfaecf1c0b
  label:    Super Godzilla
  name:     Super Godzilla
  region:   SNS-7G-USA
  revision: SNS-7G-0
  board:    MAXI-1A0N-30
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   d78ff42efdbb0e180777d17b5f7a96e08530ab77f15a99237f60714f7cfe2b74
  label:    Super High Impact
  name:     Super High Impact
  region:   SNS-HX-USA
  revision: SNS-HX-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   3f8efb19eae68f24feb42c018b7dc7a819bfd8d993ab36899681caa7ee94b06e
  label:    Super James Pond
  name:     Super James Pond
  region:   SNS-J5-USA
  revision: SNS-J5-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   a9e3e57d591e995e8e0dd228b619b6aed42205eaf55316fa8ff33f236b3a32b3
  label:    Super Mario All-Stars
  name:     Super Mario All-Stars
  region:   SNS-4M-USA
  revision: SNS-4M-0
  board:    SHVC-2A3B-01
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   a8806bfe07cd3c9945d9fd3fcea932ae1cd671cab5cae12bb7a2ae726cbf9175
  label:    Super Mario All-Stars + Super Mario World
  name:     Super Mario All-Stars + Super Mario World
  region:   SNS-5M-USA
  revision: SNS-5M-0
  board:    SHVC-BA3M-01
    memory
      type: ROM
      size: 0x280000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   89ad4ba02a2518ca792cf96b61b36613f86baac92344c9c10d7fab5433bebc16
  label:    Super Mario Kart
  name:     Super Mario Kart
  region:   SNS-MK-USA
  revision: SNS-MK-0
  board:    SHVC-1K1B-01
    memory
      type: ROM
      size: 0x80000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save
    memory
      type: ROM
      size: 0x1800
      content: Program
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1
    memory
      type: ROM
      size: 0x800
      content: Data
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1
    memory
      type: RAM
      size: 0x200
      content: Data
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1
      volatile
    oscillator
      frequency: 7600000

game
  sha256:   76d293e5a772eb2f326e173eac62ca323873b1f329f9b935a97ba86974e1fcd5
  label:    Super Mario Kart
  name:     Super Mario Kart
  region:   SNS-MK-USA
  revision: SNS-MK-0
  board:    SHVC-1K1X-10
    memory
      type: ROM
      size: 0x80000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save
    memory
      type: ROM
      size: 0x1800
      content: Program
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1B
    memory
      type: ROM
      size: 0x800
      content: Data
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1B
    memory
      type: RAM
      size: 0x200
      content: Data
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1B
      volatile
    oscillator
      frequency: 7600000

game
  sha256:   740646f3535bfb365ca44e70d46ab433467b142bd84010393070bd0b141af853
  label:    Super Mario RPG: Legend of the Seven Stars
  name:     Super Mario RPG - Legend of the Seven Stars
  region:   SNS-ARWE-USA
  revision: SNS-ARWE-0
  board:    SHVC-1L5B-11
    memory
      type: ROM
      size: 0x400000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save
    memory
      type: RAM
      size: 0x800
      content: Internal
      volatile

game
  sha256:   0838e531fe22c077528febe14cb3ff7c492f1f5fa8de354192bdff7137c27f5b
  label:    Super Mario World
  name:     Super Mario World
  region:   SNS-MW-USA
  revision: SNS-MW-0
  board:    SHVC-1A1B-06
    memory
      type: ROM
      size: 0x80000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   9b4957466798bbdb5b43a450bbb60b2591ae81d95b891430f62d53ca62e8bc7b
  label:    Super Mario World 2: Yoshi's Island
  name:     Super Mario World 2 - Yoshi's Island
  region:   SNS-YI-USA
  revision: SNS-YI-0
  board:    SHVC-1CB5B-01
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save
    oscillator
      frequency: 21440000

game
  sha256:   bd763c1a56365c244be92e6cffefd318780a2a19eda7d5baf1c6d5bd6c1b3e06
  label:    Super Mario World 2: Yoshi's Island
  name:     Super Mario World 2 - Yoshi's Island
  region:   SNS-YI-USA
  revision: SNS-YI-1
  board:    SHVC-1CB5B-20
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save
    oscillator
      frequency: 21440000

game
  sha256:   12b77c4bc9c1832cee8881244659065ee1d84c70c3d29e6eaf92e6798cc2ca72
  label:    Super Metroid
  name:     Super Metroid
  region:   SNS-RI-USA
  revision: SHVC-RI-0
  board:    SHVC-BA3M-01
    memory
      type: ROM
      size: 0x300000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   40b46bb29785fb431b325f22377faa8b099be4d77aecc1f03000b8a4cb589b63
  label:    Super Ninja Boy
  name:     Super Ninja Boy
  region:   SNS-CW-USA
  revision: SNS-CW-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   a8acbbd6f8afbf289a6b837da6cd8bd109a00cd38625c956ab8ff739732d9e4f
  label:    Super Nova
  name:     Super Nova
  region:   SNS-DH-USA
  revision: SNS-DH-0
  board:    SHVC-1J0N-01
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   24b687f95bb9737d223738f13c00aeaa7d697fa9e2fd50597b81d0cfa2160daa
  label:    Super Off Road
  name:     Super Off Road
  region:   SNS-OR-USA
  revision: SNS-OR-0
  board:    SHVC-1A0N-01
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   16f9c90d75bd23d0620be00ecf818fcb25c5935d4ee26b1fe17b926f8987aa65
  label:    Super Off Road: The Baja
  name:     Super Off Road - The Baja
  region:   SNS-R8-USA
  revision: SNS-R8-0
  board:    MAXI-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   6bf0e7a7b95613b9d0e5c8cc98eee5d0ac200e88b5d4444ad5cf93d3e8265118
  label:    Super Pinball: Behind the Mask
  name:     Super Pinball - Behind the Mask
  region:   SNS-XP-USA
  revision: SNS-XP-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   65aff1403838c938d7c6170f3044a2b0317cd2ee13ad79353a4cdd2aa82f4e87
  label:    Super Pinball: Behind the Mask
  name:     Super Pinball - Behind the Mask
  region:   SNS-XP-USA
  revision: SNS-XP-1
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   5a4b0c89606f71182fa5552ac476cc3bbda5ddc7d44e33f9184114aaea38020d
  label:    Super Play Action Football
  name:     Super Play Action Football
  region:   SNS-SF-USA
  revision: SNS-SF-0
  board:    SHVC-1A3B-13
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   a3d803b8c6b0b6ac55085671040b840f993543915c7f802e14fb651beabe9b63
  label:    Super Punch-Out!!
  name:     Super Punch-Out!!
  region:   SNS-4Q-USA
  revision: SNS-4Q-0
  board:    SHVC-1A3M-21
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   3dff3513ae6acb85a02b00b9f5adb4757ab97661993bded4f329127a792cef87
  label:    Super Putty
  name:     Super Putty
  region:   SNS-YU-USA
  revision: SNS-YU-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   05c7f6461209020785fba33007e1830820aa44ada4b1a6f991d936bf2335b15b
  label:    Super R-Type
  name:     Super R-Type
  region:   SNS-SR-USA
  revision: SNS-SR-0
  board:    SHVC-1A0N-01
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   607bfa0f74e57298f59ebe237c2e7b59364024d844534f3befb9ad1301bbedf8
  label:    Super R.B.I Baseball
  name:     Super RBI Baseball
  region:   SNS-ARLE-USA
  revision: SNS-ARLE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   7a8ffaf8bb549b400ec2f0bda9f3c0dbf5852c38618cdb21cd783c368383e2c7
  label:    Super Scope 6
  name:     Super Scope 6
  region:   SNS-LR-USA
  revision: SNS-LR-0
  board:    SHVC-1A0N-01
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   e987ceb53cc1407b1ba612b13a7b5391db6c41ea14552c165ae62ad42eeaa0ae
  label:    Super Slam Dunk
  name:     Super Slam Dunk
  region:   SNS-D9-USA
  revision: SNS-D9-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   c39ebf88af138e87ca5d963ed71a8b7defba3e2bc271d72186d00056e8225721
  label:    Super Slap Shot
  name:     Super Slap Shot
  region:   SNS-ZX-USA
  revision: SNS-ZX-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   8b75ab4bb1b63c45a56e7a4c5a37c0864f14375ab3131edc33489764426ad888
  label:    Super Smash T.V.
  name:     Super Smash TV
  region:   SNS-TV-USA
  revision: SNS-TV-0
  board:    SHVC-1A0N-01
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   694ad2b34ab808bd8a1aa9dda359594cfd3a5fd9d95d4cf262ccaa0eb57eef67
  label:    Super Soccer
  name:     Super Soccer
  region:   SNS-FS-USA
  revision: SNS-FS-0
  board:    SHVC-1A0N-01
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   5eb9736d66b3ac730ebbfdd19ef2397282b5f1a62dc0048093e041e23b807741
  label:    Super Soccer Champ
  name:     Super Soccer Champ
  region:   SNS-HT-USA
  revision: SNS-HT-0
  board:    SHVC-1A0N-02
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   5985cdad45958a5b8c5967ad39efe51569f560bf237a4e9864f21111082a8500
  label:    Super Solitaire
  name:     Super Solitaire
  region:   SNS-LT-USA
  revision: SNS-LT-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   c6bd7239cb2074ff1c471d14535bead6290bba2d7c75b4f03209cb114877b4c8
  label:    Super Star Wars: A New Hope
  name:     Super Star Wars - A New Hope
  region:   SNS-V4-USA
  revision: SNS-V4-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   1dd01da68cb61def641389bf3fa87b7a29113c9c8dd172cf303c0f7e03ba6e47
  label:    Super Star Wars II: The Empire Strikes Back
  name:     Super Star Wars II - The Empire Strikes Back
  region:   SNS-E5-USA
  revision: SNS-E5-0
  board:    SHVC-2A0N-10
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   46370b3bd6ff9ee04c425eef0f482f521f3b61bd4b058f2f4e9d322253d7b5de
  label:    Super Star Wars II: The Empire Strikes Back
  name:     Super Star Wars II - The Empire Strikes Back
  region:   SNS-E5-USA
  revision: SNS-E5-1
  board:    MAXI-1A0N-30
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   c49be867cdd276703a3ae46a6b17673b9e359af4f9af2329c8eb747120245817
  label:    Super Star Wars III: Return of the Jedi
  name:     Super Star Wars III - Return of the Jedi
  region:   SNS-ARJE-USA
  revision: SNS-ARJE-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   f7df5cd16ce6624567d1a24e9b9c0b9050ea9b6a9fe5a7973484589637756596
  label:    Super Star Wars III: Return of the Jedi
  name:     Super Star Wars III - Return of the Jedi
  region:   SNS-ARJE-USA
  revision: SNS-ARJE-1
  board:    MAXI-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   d17cb5c73174060fcbd9cba6c705643f19c3b8be24d0f7ee43aee674ff1ea38e
  label:    Super Street Fighter II: The New Challengers
  name:     Super Street Fighter II - The New Challengers
  region:   SNS-XW-USA
  revision: SNS-XW-0
  board:    SHVC-BJ0N-01
    memory
      type: ROM
      size: 0x400000
      content: Program

game
  sha256:   830c900083cccc6ded74c167c4e257db34df4786ecd6e2f053de454db7d31fe5
  label:    Super Strike Eagle
  name:     Super Strike Eagle
  region:   SNS-EG-USA
  revision: SNS-EG-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   6e45a80ea148654514cb4e8604a0ffcbc726946e70f9e0b9860e36c0f3fa4877
  label:    Super Tennis
  name:     Super Tennis
  region:   SNS-ST-USA
  revision: SNS-ST-0
  board:    SHVC-1A0N-01
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   02cb7f1ed21ed6bfb9eaa0e91df2adb063a9bf4cbd6feb6cd024d676829e227e
  label:    Super Troll Islands
  name:     Super Troll Islands
  region:   SNS-5L-USA
  revision: SNS-5L-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   056ac8160363d577e3ffc2f593801b8bb3d024fe4f3a3331b711dc556204949d
  label:    Super Turrican
  name:     Super Turrican
  region:   SNS-TU-USA
  revision: SNS-TU-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   96da3512a1aa05a40f1e5d61c48932b0d55d9f136d6418b848153a9fecab06de
  label:    Super Turrican 2
  name:     Super Turrican 2
  region:   SNS-A2TE-USA
  revision: SNS-A2TE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   792e615a34ceae5a1b9a4f54c5a5d9b53e39299332fece83e4bceca2c22efb21
  label:    Super Valis IV
  name:     Super Valis IV
  region:   SNS-VA-USA
  revision: SNS-VA-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   62d3d650e956f23f3b221f83c5c5bd204afd0e4f4d9c47a3251962323de96089
  label:    Super Widget
  name:     Super Widget
  region:   SNS-WI-USA
  revision: SNS-WI-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   dfada65f4497f482d5f81964322d9952f678f50e65418d307d4503f9bf9a32d9
  label:    Suzuka 8 Hours
  name:     Suzuka 8 Hours
  region:   SNS-8H-USA
  revision: SNS-8H-0
  board:    SHVC-1K0N-01
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: ROM
      size: 0x1800
      content: Program
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1
    memory
      type: ROM
      size: 0x800
      content: Data
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1
    memory
      type: RAM
      size: 0x200
      content: Data
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP1
      volatile
    oscillator
      frequency: 7600000

game
  sha256:   d802715fb4f09d7e499b5b3e577af641598a723dae7cedeaa93943bb53c6edbb
  label:    SWAT Kats: The Radical Squadron
  name:     SWAT Kats - The Radical Squadron
  region:   SNS-AK9E-USA
  revision: SNS-AK9E-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   c2015d03dd3db08069b2a6ea1ed6b3e1ac1e3a5f804b02295c3cd3717add91ef
  label:    Syndicate
  name:     Syndicate
  region:   SNS-AFYE-USA
  revision: SNS-AFYE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   1711fe9010232b41ec406900e5b4ef528dd2beaa97b27d94ed7eef57d63904a0
  label:    Taz-Mania
  name:     Taz-Mania
  region:   SNS-TZ-USA
  revision: SNS-TZ-1
  board:    MAXI-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   7da4a3cfa5de4bb4722a6e2a42c26aae322b5e668f1645d8c870fb99e6080600
  label:    Tecmo Secret of the Stars
  name:     Tecmo Secret of the Stars
  region:   SNS-AQ-USA
  revision: SNS-AQ-0
  board:    SHVC-2A3M-20
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   c3cb1d2fd9775aa9d15b7eedd45ad82519b73d47ca166737ed398c748717bcc4
  label:    Tecmo Super Baseball
  name:     Tecmo Super Baseball
  region:   SNS-ATBE-USA
  revision: SNS-ATBE-0
  board:    SHVC-2A3M-11
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   35dd020cf57fc402417ab6e4a6c49866c5a86bba25218c0aaf7ce85cb134bcf8
  label:    Tecmo Super Bowl
  name:     Tecmo Super Bowl
  region:   SNS-7T-USA
  revision: SNS-7T-0
  board:    SHVC-2A3B-01
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   2972057a0087f8239d2523eaa995405f06e5d5ba3a3203b6b50d401379c8ebde
  label:    Tecmo Super Bowl II: Special Edition
  name:     Tecmo Super Bowl II - Special Edition
  region:   SNS-ASBE-USA
  revision: SNS-ASBE-0
  board:    SHVC-1A5M-11
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save

game
  sha256:   8cfd4c5525f4bd4bba5af7e2323f1e61f27ce97c6d5617cfa685c9276fbf6407
  label:    Tecmo Super Bowl III: Final Edition
  name:     Tecmo Super Bowl III - Final Edition
  region:   SNS-AW4E-USA
  revision: SNS-AW4E-0
  board:    SHVC-1A5M-20
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save

game
  sha256:   14bce564f976d1431127259edbcb23c52c79361fed9814d307d627c4650e800e
  label:    Tecmo Super NBA Basketball
  name:     Tecmo Super NBA Basketball
  region:   SNS-XM-USA
  revision: SNS-XM-0
  board:    SHVC-1A3B-13
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   5b82cdd6f2da56f43680d6a5021faebe2e06036d30602c1a7917aa414cf8b5f4
  label:    Teenage Mutant Ninja Turtles IV: Turtles in Time
  name:     Teenage Mutant Ninja Turtles IV - Turtles in Time
  region:   SNS-TM-USA
  revision: SNS-TM-0
  board:    SHVC-1A0N-02
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   849141370f164d6db3e5709da670726f958ce13ffef69319564db3fb0b12c69d
  label:    Teenage Mutant Ninja Turtles V: Tournament Fighters
  name:     Teenage Mutant Ninja Turtles V - Tournament Fighters
  region:   SNS-KY-USA
  revision: SNS-KY-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   bd7074ef4a05d790646abe145ffd2587fb48044e4b730286d807abe102841177
  label:    The Terminator
  name:     Terminator
  region:   SNS-TN-USA
  revision: SNS-TN-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   06db3be569a587d79b51bfc684fd2ebdea977863875aedec88218fbb4169c21b
  label:    Terminator 2: Judgment Day
  name:     Terminator 2 - Judgment Day
  region:   SNS-TP-USA
  revision: SNS-TP-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   365f10f9d9f68cc59e769eeb451c417e1ff7415022a625de9976a3b924c0bd61
  label:    Terminator 2: The Arcade Game
  name:     Terminator 2 - The Arcade Game
  region:   SNS-XV-USA
  revision: SNS-XV-0
  board:    MAXI-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   c9563cb57314588aa3db22b76dc8acfba3e73733dd3538edd90af5a15595830e
  label:    Test Drive II: The Duel
  name:     Test Drive II - The Duel
  region:   SNS-DL-USA
  revision: SNS-DL-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   3cdebbd8adc4bb6773a7995f542fdac49adefca71cba583255a1c1bf37ac3946
  label:    Tetris & Dr. Mario
  name:     Tetris & Dr. Mario
  region:   SNS-ATFE-USA
  revision: SNS-ATFE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   accc836c3adabadc810fbe35702c6a64d50a09f4c672d2734fa58b251d7a20aa
  label:    Tetris 2
  name:     Tetris 2
  region:   SNS-27-USA
  revision: SNS-27-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   70dea29a928c1625def31c862dc74960e39e587e416b45829efc21f13ebd9630
  label:    Tetris 2
  name:     Tetris 2
  region:   SNS-27-USA
  revision: SNS-27-1
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   579bf1a1988d4af06a69cc1d82a2478ebe51940c5ced7f97e83029a24e6aa778
  label:    Tetris Attack
  name:     Tetris Attack
  region:   SNS-AYLE-USA
  revision: SNS-AYLE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   1121df3f05dd9dbb749f6b785eb9b3eb69968004f9d6ceffa6f84757af2f3702
  label:    Thomas the Tank Engine & Friends
  name:     Thomas the Tank Engine & Friends
  region:   SNS-6T-USA
  revision: SNS-6T-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   d50aaa41e153ca356eee16a9deccb1a763ee56ebbe6c80cd28c5ad1db66a5316
  label:    Thunder Spirits
  name:     Thunder Spirits
  region:   SNS-TH-USA
  revision: SNS-TH-0
  board:    SHVC-1A0N-02
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   eb958801fd1f08771e5a0933f7701d633262efbfe8d47de21dda18e3b77670de
  label:    The Tick
  name:     Tick, The
  region:   SNS-ATHE-USA
  revision: SNS-ATHE-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   16fb965130e57f37dda2466f23820f091f8b96758aa7e30ba4fd63cb618e5ddb
  label:    Time Cop
  name:     Time Cop
  region:   SNS-ATCE-USA
  revision: SNS-ATCE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   8e82f98d2e62bc1e5fcf2386c2b5ca54998398220efcedd67858aaaa92289a42
  label:    Time Slip
  name:     Time Slip
  region:   SNS-XT-USA
  revision: SNS-XT-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   fa7e2b40093f0cc7233cc77e95bbbea2144c8183dec10446590396fffd7cda37
  label:    Time Trax
  name:     Time Trax
  region:   SNS-X8-USA
  revision: SNS-X8-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   271a67b32b3bb00ceb0f4e7d81494888d0d821635f0f936d481dfbe671567c08
  label:    Timon & Pumbaa's Jungle Games
  name:     Timon & Pumbaa's Jungle Games
  region:   SNS-AJ9E-USA
  revision: SNS-AJ9E-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   0503cd93b4d211a825acd47ff3813668b4ce68890c8be2fbfe5ac2b46882dfcf
  label:    Tin Star
  name:     Tin Star
  region:   SNS-9N-USA
  revision: SNS-9N-0
  board:    SHVC-1A1M-11
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   ba679a11264e9695895a6c17358a41e8459be06166d056811df9c2738fef3d0d
  label:    Tiny Toon Adventures: Buster Busts Loose!
  name:     Tiny Toon Adventures - Buster Busts Loose!
  region:   SNS-TA-USA
  revision: SNS-TA-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   f753de4a38bd83f7d937fc7bf5565a3c351a794c113dead8fdee6d86c85633e8
  label:    Tiny Toon Adventures: Wacky Sports Challenge
  name:     Tiny Toon Adventures - Wacky Sports Challenge
  region:   SNS-5Z-USA
  revision: SNS-5Z-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   08d808e9c5851e4301a38a56b350a20ea9e3adbef51546e87e1785d691d0f2d5
  label:    TKO Super Championship Boxing
  name:     TKO Super Championship Boxing
  region:   SNS-BX-USA
  revision: SNS-BX-0
  board:    SHVC-1A0N-02
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   ced8c0bc2791ffe47cb9eec03db67567945af8c58b5330326722f1cfca41bf51
  label:    TNN Bass Tournament of Champions
  name:     TNN Bass Tournament of Champions
  region:   SNS-ATNE-USA
  revision: SNS-ATNE-0
  board:    SHVC-2J3M-20
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   4f500da19dbb1557a7bc0ce14437098c1402478d573fb569303b81c011f86fbf
  label:    Tom and Jerry
  name:     Tom and Jerry
  region:   SNS-TJ-USA
  revision: SNS-TJ-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   d99008d2181986dc7f65228696d940b5d31be8471f166d1ab9b1c14f1503bcfb
  label:    Tommy Moe's Winter Extreme: Skiing and Snowboarding
  name:     Tommy Moe's Winter Extreme - Skiing and Snowboarding
  region:   SNS-XS-USA
  revision: SNS-XS-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   e0fbf731550266d79a0b6ca460afd04c8c312f7023b2c9882c4fc3acc3e7932f
  label:    Tony Meola's Sidekicks Soccer
  name:     Tony Meola's Sidekicks Soccer
  region:   SNS-6K-USA
  revision: SNS-6K-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   ca9889f17f184b3d99a2eaaa82af73e366f03ed00313fdd369e5e023b208e788
  label:    Top Gear
  name:     Top Gear
  region:   SNS-TR-USA
  revision: SNS-TR-0
  board:    SHVC-1A0N-02
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   76b2702c4be8b668c1017f2817c280283c275eaa41535bf6ffa2b8d2220b68c6
  label:    Top Gear 2
  name:     Top Gear 2
  region:   SNS-2P-USA
  revision: SNS-2P-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   ede60f0d283b6ea3a45308981d99e7c422a0296e5fadde51c6bafd4dcc789ca2
  label:    Top Gear 3000
  name:     Top Gear 3000
  region:   SNS-A3TE-USA
  revision: SNS-A3TE-0
  board:    SHVC-1B0N-03
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: ROM
      size: 0x1800
      content: Program
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP4
    memory
      type: ROM
      size: 0x800
      content: Data
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP4
    memory
      type: RAM
      size: 0x200
      content: Data
      manufacturer: NEC
      architecture: uPD7725
      identifier: DSP4
      volatile
    oscillator
      frequency: 7600000

game
  sha256:   9bf884be5627d38f060ad7f3a61ea1fea1474d416e1d037d33014ca9d5205c1d
  label:    Total Carnage
  name:     Total Carnage
  region:   SNS-XC-USA
  revision: SNS-XC-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   345e795000e74f51704774edfc8049473461761a65eb47cab710caa29e09897b
  label:    Toy Story
  name:     Toy Story
  region:   SNS-AQHE-USA
  revision: SNS-AQHE-0
  board:    SHVC-BJ0N-20
    memory
      type: ROM
      size: 0x400000
      content: Program

game
  sha256:   7a6e5da46b026900fba4584a32ad40d940b9ecf9fccfb11f96a205a914014784
  label:    Toys: Let the Toy Wars Begin!
  name:     Toys - Let the Toy Wars Begin!
  region:   SNS-YT-USA
  revision: SNS-YT-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   4070a7702dc506a1ceb6f65b5c330b3a162df6f01735c8f206934fd47b810ed4
  label:    Troddlers
  name:     Troddlers
  region:   SNS-TX-USA
  revision: SNS-TX-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   46fcca4ce29f472afa8519958d256eec347c2dc2da154c95f263a051c5c02dbb
  label:    Troy Aikman Football
  name:     Troy Aikman Football
  region:   SNS-YQ-USA
  revision: SNS-YQ-0
  board:    SHVC-2A3M-11
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   dd96a8f4f9c8988301ff710a4c70ebe3bf7914901f3547abe1d5f0dd5c0b921b
  label:    True Golf: Wicked 18
  name:     True Golf - Wicked 18
  region:   SNS-W8-USA
  revision: SNS-W8-0
  board:    SHVC-1A3M-10
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   5c7b28bb24bad697156ad444ff23bd15ad6744dbf9899b3cccf2aa36d559d825
  label:    True Golf Classics: Pebble Beach Golf Links
  name:     True Golf Classics - Pebble Beach Golf Links
  region:   SNS-GB-USA
  revision: SNS-GB-0
  board:    SHVC-1A3B-12
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   72088194a65fc057f2910945a33d9f071682a4cecac8996f0bdabcdb5ef39962
  label:    True Golf Classics: Waialae Country Club
  name:     True Golf Classics - Waialae Country Club
  region:   SNS-TG-USA
  revision: SNS-TG-0
  board:    SHVC-1A3B-11
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   47dd8ea2d12a6bb2a9774de1d492259fc4fb9f8ec7976383bbfd922532671f6b
  label:    True Lies
  name:     True Lies
  region:   SNS-ATLE-USA
  revision: SNS-ATLE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   8f62d014f513a7dcbca5aa76cbe476c3e4526100f34913af831bc05dab029bd1
  label:    Tuff E Nuff
  name:     Tuff E Nuff
  region:   SNS-TE-USA
  revision: SNS-TE-0
  board:    SHVC-2A0N-10
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   e4343c0fadc00ffdc3dc31345068d751eea5d639f826731f08cb81673d508c40
  label:    Turn and Burn: No-Fly Zone
  name:     Turn and Burn - No-Fly Zone
  region:   SNS-ZN-USA
  revision: SNS-ZN-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   259c25d4613f97f5fa7992900fb583625d7fb912c7ae09fa9def2e682834dc9f
  label:    The Twisted Tales of Spike McFang
  name:     Twisted Tales of Spike McFang, The
  region:   SNS-83-USA
  revision: SNS-83-0
  board:    SHVC-1A1M-11
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   094555d5720158ee60c9d5ab9a13110192db5ebf0f6cf69abbb59a00bc470345
  label:    Ultima: Runes of Virtue II
  name:     Ultima - Runes of Virtue II
  region:   SNS-7U-USA
  revision: SNS-7U-0
  board:    SHVC-1A1M-11
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   11659bd8dd620d50400d16042aeb2d0ddb00c7183fc1ecb95b1a34f07db0431b
  label:    Ultima VI: The False Prophet
  name:     Ultima VI - The False Prophet
  region:   SNS-U6-USA
  revision: SNS-U6-0
  board:    SHVC-1A3M-20
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   a31af0e39afb55bbc92a5543b504327fbe7e8cd0a5e08626976bed7b65376737
  label:    Ultima VII: The Black Gate
  name:     Ultima VII - The Black Gate
  region:   SNS-7I-USA
  revision: SNS-7I-0
  board:    SHVC-1A3M-21
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   78bf82963cded9162e25035053c8b1a9f760748ff0beacc230d005204992737d
  label:    Ultimate Fighter
  name:     Ultimate Fighter
  region:   SNS-HP-USA
  revision: SNS-HP-0
  board:    SHVC-2A0N-11
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   cb2fdfce61858063bf4c9da4228381c3ec3abe423f4d378cddd174ae4adb261e
  label:    Ultimate Mortal Kombat 3
  name:     Ultimate Mortal Kombat 3
  region:   SNS-A3ZE-USA
  revision: SNS-A3ZE-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x400000
      content: Program

game
  sha256:   e9fae4c2e171a1fc4f2bd800abd9e42750aaf7a4db9e40c5b9142e15029500bd
  label:    Ultraman: Towards the Future
  name:     Ultraman - Towards the Future
  region:   SNS-UM-USA
  revision: SNS-UM-0
  board:    SHVC-1A0N-01
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   0b155a54b6134601fc0791252a63ca73efd522667c3d6fd7a44f5b3c500039d7
  label:    U.N. Squadron
  name:     UN Squadron
  region:   SNS-E8-USA
  revision: SNS-E8-0
  board:    SHVC-1A0N-01
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   794152fc6f55cb15a0b203fa645ac9fa314a293da999d8ec8b3dda080434d175
  label:    Uncharted Waters
  name:     Uncharted Waters
  region:   SNS-QK-USA
  revision: SNS-QK-0
  board:    SHVC-1A3B-13
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   64bc4707f422661a66618088887e2363a5f896ea683c58984fffd96dd21ab5f0
  label:    Uncharted Waters: New Horizons
  name:     Uncharted Waters - New Horizons
  region:   SNS-QL-USA
  revision: SNS-QL-0
  board:    SHVC-1J5M-11
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save

game
  sha256:   859ec99fdc25dd9b239d9085bf656e4f49c93a32faa5bb248da83efd68ebd478
  label:    Uniracers
  name:     Uniracers
  region:   SNS-4L-USA
  revision: SNS-4L-0
  board:    SHVC-1A3M-21
    memory
      type: ROM
      size: 0x200000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   ecefb4117a6aae117e033c8cc07f0db2797d6be93dd5cdcefc23692a21fae02e
  label:    The Untouchables
  name:     Untouchables, The
  region:   SNS-UC-USA
  revision: SNS-UC-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   dcb33a89fcb8d8ca8f3a467034728ad6375273d8eb51a60418ca51ef745e9b38
  label:    Urban Strike
  name:     Urban Strike
  region:   SNS-AUSE-USA
  revision: SNS-AUSE-0
  board:    MJSC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   2500d6c846c78bcb729f15535bf2b852a120411891cabaaaa6fc407906d0214e
  label:    Utopia: The Creation of a Nation
  name:     Utopia - The Creation of a Nation
  region:   SNS-UP-USA
  revision: SNS-UP-0
  board:    SHVC-1A3M-10
    memory
      type: ROM
      size: 0x80000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   78bf9d79fb2ff3f9d03ecc1176d070e53ddaca2c6b0cda69e74c19a4e50b195b
  label:    Vegas Stakes
  name:     Vegas Stakes
  region:   SNS-VS-USA
  revision: SNS-VS-0
  board:    SHVC-1A1B-06
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   41b5561de9e4984276e52987ea46c5f4fa8526d8141c70c738875a9eb9fe9d70
  label:    Vortex
  name:     Vortex
  region:   SNS-4V-USA
  revision: SNS-4V-0
  board:    SHVC-1CA0N5S-01
    memory
      type: ROM
      size: 0x80000
      content: Program
    memory
      type: RAM
      size: 0x8000
      content: Save
      volatile
    oscillator
      frequency: 21440000

game
  sha256:   b0e74f0fe8d1e7fe2fe404341fea7c68e28f3a0ab78552d5092d413f2ecec417
  label:    Wanderers from Ys III
  name:     Wanderers from Ys III
  region:   SNS-YS-USA
  revision: SNS-YS-0
  board:    SHVC-1A3B-12
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   82bba8ae2bb4dbc74a18af31aaec19c368576e4369dd70b396caa5e8729540bb
  label:    War 2410
  name:     War 2410
  region:   SNS-A2AE-USA
  revision: SNS-A2AE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   8b12e5e4553fc921c23739d1aec2ed517535ec239daef800f39f602d8473847f
  label:    War 3010: The Revolution
  name:     War 3010 - The Revolution
  region:   SNS-AZNE-USA
  revision: SNS-AZNE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   be420715152394e0e4a25ab10931b67f92f910cfcf7793e31dfee1c6334808e5
  label:    Wario's Woods
  name:     Wario's Woods
  region:   SNS-65-USA
  revision: SNS-65-0
  board:    SHVC-1A3M-21
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   8579dd352d20589072ed5c026bde7adadd6229d18a022e7cb47cf5602b54015e
  label:    Warlock
  name:     Warlock
  region:   SNS-AWKE-USA
  revision: SNS-AWKE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   36ec2f409f08a08f8327570eadcd8960b6a47bf5797441c2df05fcc50c5b762b
  label:    Warp Speed
  name:     Warp Speed
  region:   SNS-WP-USA
  revision: SNS-WP-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   dd73690dd3165a16580e191c92a497102758f312c759353f685e371755c663a8
  label:    Wayne Gretzky and the NHLPA All-Stars
  name:     Wayne Gretzky and the NHLPA All-Stars
  region:   SNS-AWZE-USA
  revision: SNS-AWZE-0
  board:    SHVC-2A3M-20
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   56ba3d585bf6b701e342d86a0bd164ab0a97dfbd5df46b3a964506842633459c
  label:    Wayne's World
  name:     Wayne's World
  region:   SNS-WW-USA
  revision: SNS-WW-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   215ab7e576b31462e3284d035006dc638cae06bbfb1af2c5617403b15ab2b25a
  label:    WCW Super Brawl Wrestling
  name:     WCW Super Brawl Wrestling
  region:   SNS-AWRE-USA
  revision: SNS-AWRE-0
  board:    SHVC-2A0N-11
    memory
      type: ROM
      size: 0x180000
      content: Program

game
  sha256:   e931c3c08f20f78e3a43ad92d16eb472be619abaa17c2d8e2b0fcd5d05dbd74d
  label:    We're Back!: A Dinosaur's Story
  name:     We're Back! - A Dinosaur's Story
  region:   SNS-6D-USA
  revision: SNS-6D-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   0b1ba31ae95b61d7d9a0f5509b5836fff84f60915802e3b3ba1170a5c50a4b71
  label:    WeaponLord
  name:     WeaponLord
  region:   SNS-AWDE-USA
  revision: SNS-AWDE-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x300000
      content: Program

game
  sha256:   12abf1ba063c120c1a98495a1c85e67a0007aff771ef92adcb94b4a0a2fd5adb
  label:    Wheel of Fortune
  name:     Wheel of Fortune
  region:   SNS-WL-USA
  revision: SNS-WL-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   c7af9e7f3243ba1b5dd81f32f710e60c2ae1d443ecaf7140bf42d71a1b69d8a2
  label:    Wheel of Fortune: Deluxe Edition
  name:     Wheel of Fortune - Deluxe Edition
  region:   SNS-XF-USA
  revision: SNS-XF-0
  board:    MAXI-1A0N-30
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   1bd3af0125743bf9bbbac6e7dc215fe32c4ff7043a2ee034d56b627e3f6875f0
  label:    Where in the World is Carmen Sandiego?
  name:     Where in the World is Carmen Sandiego
  region:   SNS-WX-USA
  revision: SNS-WX-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   e3177f066bf004fd99aef31f2692ba0b412dd280d613dc3f4cf334d97f4c9af8
  label:    Where in Time is Carmen Sandiego?
  name:     Where in Time is Carmen Sandiego
  region:   SNS-WN-USA
  revision: SNS-WN-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   0a52dc1e7820f5541f53ce0e1e96144fe079af0efe3dae5c2c89d86365feb8b1
  label:    Whizz
  name:     Whizz
  region:   SNS-AZWE-USA
  revision: SNS-AZWE-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   c8f159e2625ac8078535c06857ea28475706da45df494de8e46f50888272cf71
  label:    Wild Guns
  name:     Wild Guns
  region:   SNS-4W-USA
  revision: SNS-4W-0
  board:    SHVC-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   39bb99eddb224de5f0789f807ffef06b9efa2efb7962dced31fb272f986699cd
  label:    Wild Snake
  name:     Wild Snake
  region:   SNS-AWSE-USA
  revision: SNS-AWSE-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   60d01a3f499463156546ecdee18ee3e946b95302ee0b1569decb97f52372d2eb
  label:    Williams Arcade's Greatest Hits
  name:     Williams Arcade's Greatest Hits
  region:   SNS-AW8E-USA
  revision: SNS-AW8E-0
  board:    MAXI-1A0N-30
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   2167fc7c5447b2287668d2f3e4ef1a285361b2292ecc8a4cbd9f966a460ad7a2
  label:    Wing Commander
  name:     Wing Commander
  region:   SNS-WC-USA
  revision: SNS-WC-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   132ca0b6a4888edf7de785d48f4417fac28522646e6c7514f80c5e9ff1438d5f
  label:    Wing Commander: The Secret Missions
  name:     Wing Commander - The Secret Missions
  region:   SNS-2W-USA
  revision: SNS-2W-0
  board:    SHVC-YA0N-01
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   c3bcd5c716f96e6359ebcfd85c3e9b07b46c5124bf4010d89ceef5b6f2f868f6
  label:    Wings 2: Aces High
  name:     Wings 2 - Aces High
  region:   SNS-WG-USA
  revision: SNS-WG-0
  board:    SHVC-1A0N-02
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   0e834647669783c2b79cc1120c057c870c541079a7abd1eee3f787d59dc3c3eb
  label:    Winter Olympic Games: Lillehammer '94
  name:     Winter Olympic Games - Lillehammer '94
  region:   SNS-W4-USA
  revision: SNS-W4-0
  board:    SHVC-1J0N-10
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   025dd3047c474d879e69b91a3918add9cdabedf4182e1c1e10e5f0c13a124bf9
  label:    The Wizard of Oz
  name:     Wizard of Oz, The
  region:   SNS-W6-USA
  revision: SNS-W6-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   bc68f40075f9983f5027fe264c3037d1a831e8e76a6c9adb60d10226f6ef005b
  label:    Wizardry V: Heart of the Maelstrom
  name:     Wizardry V - Heart of the Maelstrom
  region:   SNS-W5-USA
  revision: SNS-W5-0
  board:    SHVC-1A3M-20
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   e0165bafeb8d65be08a5a4079f8651104471f450c60794b761b1255853ca2d98
  label:    Wolf Child
  name:     Wolf Child
  region:   SNS-WH-USA
  revision: SNS-WH-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   9c2b458e8fda5cb437a4c6d28fb430e45c4cfef98420c40546b8e08563a4fc7d
  label:    Wolfenstein 3D
  name:     Wolfenstein 3D
  region:   SNS-6W-USA
  revision: SNS-6W-0
  board:    SHVC-1J0N-10
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   44428a3d1c796fbd41da7620e321c45f11cd80a0e5f4ab8c48177106cb960d77
  label:    Wolverine: Adamantium Rage
  name:     Wolverine - Adamantium Rage
  region:   SNS-AWXE-USA
  revision: SNS-AWXE-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   82e2b636e66c4edbae27a5be91a61194ef2881ec93f40d1de93a6153617b12f2
  label:    Wordtris
  name:     Wordtris
  region:   SNS-WT-USA
  revision: SNS-WT-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   86e9d76a8834732e511253d8092727bdbfb409b3d0ff1c06b6b6e481c9a039ef
  label:    World Cup USA '94
  name:     World Cup USA '94
  region:   SNS-U4-USA
  revision: SNS-U4-0
  board:    SHVC-2J3M-11
    memory
      type: ROM
      size: 0x180000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   ea76cfdbb2a555a7b6eff8b466a879f9a9189639416e8c2fb45bf074e695105f
  label:    World Heroes
  name:     World Heroes
  region:   SNS-WZ-USA
  revision: SNS-WZ-0
  board:    SHVC-2J0N-10
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   159d5341d13d6801324e8271f7191c0223617c9d30984676319b2df7937c78c0
  label:    World Heroes 2
  name:     World Heroes 2
  region:   SNS-JI-USA
  revision: SNS-JI-0
  board:    SHVC-BA0N-01
    memory
      type: ROM
      size: 0x300000
      content: Program

game
  sha256:   d4d9f1b41dad7e7a126a9adbe8d86c4b339e120c866156796de1cb0c9a214189
  label:    World League Soccer
  name:     World League Soccer
  region:   SNS-WS-USA
  revision: SNS-WS-0
  board:    SHVC-1A1B-05
    memory
      type: ROM
      size: 0x80000
      content: Program
    memory
      type: RAM
      size: 0x800
      content: Save

game
  sha256:   2143bbd87ea1c5cfe5eaf46ae39e3ebb11a2e929d05cbb929904037f4d72acfe
  label:    World Soccer '94: Road to Glory
  name:     World Soccer '94 - Road to Glory
  region:   SNS-WO-USA
  revision: SNS-WO-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   0af7b0d3022acd24a1fb15865a076519f7f56e7a4b33f12b6d851b3a91e5388c
  label:    WWF Raw
  name:     WWF Raw
  region:   SNS-AWFE-USA
  revision: SNS-AWFE-0
  board:    SHVC-BJ0N-01
    memory
      type: ROM
      size: 0x300000
      content: Program

game
  sha256:   51c53e36ed0b959b0695fc6ef036fa7302d1c995eca35c28261d6f3cb77df0ca
  label:    WWF Royal Rumble
  name:     WWF Royal Rumble
  region:   SNS-WU-USA
  revision: SNS-WU-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   0b9abf2fc25a5f07c71f9d8efbb0d0e616c1494060138fbb63f7398e9c26198e
  label:    WWF Super Wrestlemania
  name:     WWF Super Wrestlemania
  region:   SNS-WF-USA
  revision: SNS-WF-0
  board:    MAXI-1A0N-30
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   67faa6ed3406a5ab0d7224b811c0960bb36560040ee959bb3304c9293ceaa093
  label:    WWF Wrestlemania: The Arcade Game
  name:     WWF Wrestlemania - The Arcade Game
  region:   SNS-AWVE-USA
  revision: SNS-AWVE-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x300000
      content: Program

game
  sha256:   dc3792e9fe7ef7aaea4ac675a48ad06129dd3ebdd4b96a513bc8241549cbd579
  label:    X-Kaliber 2097
  name:     X-Kaliber 2097
  region:   SNS-X7-USA
  revision: SNS-X7-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   65fe17fd6b297f52df6ce9812ecb02c3bb1bfda3ebc05a19c4a8decbf9a446ae
  label:    X-Men: Mutant Apocalypse
  name:     X-Men - Mutant Apocalypse
  region:   SNS-AXME-USA
  revision: SNS-AXME-0
  board:    MAXI-1J0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   93272180090e8418582f69b79c5cee6b28638b9a93192cc4bcd96291a4fca02d
  label:    X-Zone
  name:     X-Zone
  region:   SNS-XZ-USA
  revision: SNS-XZ-0
  board:    SHVC-1A0N-10
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   71b69490c78d0bbaf47da25217c5dae295190311aa5df75653c3fac0a1b45358
  label:    Xardion
  name:     Xardion
  region:   SNS-XA-USA
  revision: SNS-XA-0
  board:    SHVC-1A3B-12
    memory
      type: ROM
      size: 0x100000
      content: Program
    memory
      type: RAM
      size: 0x2000
      content: Save

game
  sha256:   90ad69a489194aca7ef7b7fd1d30e0105da4934a81ac8b0333ea20f9248df92d
  label:    Yoshi's Cookie
  name:     Yoshi's Cookie
  region:   SNS-YC-USA
  revision: SNS-YC-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program

game
  sha256:   12fba2aff04c8e39968e828629ebd16caa314bca397a9418d35fdaffe8188e20
  label:    Yoshi's Safari
  name:     Yoshi's Safari
  region:   SNS-RH-USA
  revision: SNS-RH-0
  board:    SHVC-YA0N-01
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   fbe8926fc0149d3e8e2aec20f15640ea6814f4f4b01c3960f3c477f5f17e890f
  label:    Young Merlin
  name:     Young Merlin
  region:   SNS-Y6-USA
  revision: SNS-Y6-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   7d414b7f5941f1eddc35259a22accbbbd7b47c517dfcf8bad86c4dcfa9e50b1e
  label:    Zero the Kamikaze Squirrel
  name:     Zero the Kamikaze Squirrel
  region:   SNS-AZKE-USA
  revision: SNS-AZKE-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x200000
      content: Program

game
  sha256:   b27e2e957fa760f4f483e2af30e03062034a6c0066984f2e284cc2cb430b2059
  label:    Zombies Ate My Neighbors
  name:     Zombies Ate My Neighbors
  region:   SNS-ZA-USA
  revision: SNS-ZA-0
  board:    SHVC-YA0N-01
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   25414de02c6805ca62574cfb39c23bf292b3d8c4ff33eb8f212ccdbcd61c5ae3
  label:    Zool: Ninja of the "Nth" Dimension
  name:     Zool - Ninja of the 'Nth' Dimension
  region:   SNS-Z8-USA
  revision: SNS-Z8-0
  board:    SHVC-1A0N-20
    memory
      type: ROM
      size: 0x100000
      content: Program

game
  sha256:   a3f1abf2740ff64042d82f7a940fb4269ece57d7c6967571549d8b60b20f305a
  label:    Zoop
  name:     Zoop
  region:   SNS-AZPE-USA
  revision: SNS-AZPE-0
  board:    SHVC-1J0N-20
    memory
      type: ROM
      size: 0x80000
      content: Program
)");
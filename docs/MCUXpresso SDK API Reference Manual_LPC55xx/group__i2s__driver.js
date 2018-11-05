var group__i2s__driver =
[
    [ "i2s_config_t", "group__i2s__driver.html#structi2s__config__t", [
      [ "masterSlave", "group__i2s__driver.html#aafc62e3f728b969836765543c64e51cc", null ],
      [ "mode", "group__i2s__driver.html#a0cad4b688fbf5a86c693ecdf38fae82d", null ],
      [ "rightLow", "group__i2s__driver.html#af91414d28248e4e84925e32ebb21be1f", null ],
      [ "leftJust", "group__i2s__driver.html#a1bd30d4ba4204ceff694883e65b0f658", null ],
      [ "sckPol", "group__i2s__driver.html#a06daafd9d6be910c620dc34b27f09cf1", null ],
      [ "wsPol", "group__i2s__driver.html#a7ef75e30977450dee75c44174e932069", null ],
      [ "divider", "group__i2s__driver.html#a124c2290d1b6a65c1ed7b9862c5751e0", null ],
      [ "oneChannel", "group__i2s__driver.html#a31f47bae11e9bd15ddcf145f63487844", null ],
      [ "dataLength", "group__i2s__driver.html#addfc45dd466680d9a2074dd5c69dc4e5", null ],
      [ "frameLength", "group__i2s__driver.html#a5c5c1aa5b38146f0658a096063541705", null ],
      [ "position", "group__i2s__driver.html#a6fb24ddef6640286ba084acb69740f77", null ],
      [ "watermark", "group__i2s__driver.html#a1246195e9b532c8b337d8f95ee6aad9b", null ],
      [ "txEmptyZero", "group__i2s__driver.html#a4af539301a365a0fd0870aa2f10f8a8b", null ],
      [ "pack48", "group__i2s__driver.html#a05e651099ce845bcba1144d2f3e8b6ca", null ]
    ] ],
    [ "i2s_transfer_t", "group__i2s__driver.html#structi2s__transfer__t", [
      [ "data", "group__i2s__driver.html#a8cc69942922b3f5130fd326f9b6b778b", null ],
      [ "dataSize", "group__i2s__driver.html#ac229187780b18f60d4616e9b9d10b527", null ]
    ] ],
    [ "i2s_handle_t", "group__i2s__driver.html#struct__i2s__handle", [
      [ "state", "group__i2s__driver.html#ac71e8b4cb176f3ee47962cd0c3a8990b", null ],
      [ "completionCallback", "group__i2s__driver.html#a95bd70e36257e8fe9406d759d0ceb433", null ],
      [ "userData", "group__i2s__driver.html#a06376f0e14c8243247b13ee278933182", null ],
      [ "oneChannel", "group__i2s__driver.html#a7ca7ac0f7335eb401e1617bc4b6b784d", null ],
      [ "dataLength", "group__i2s__driver.html#aabe3a7f62d8eb02f47ac57c4840f0adb", null ],
      [ "pack48", "group__i2s__driver.html#a6665cfa3da935e04753a0a298583773d", null ],
      [ "useFifo48H", "group__i2s__driver.html#a89aaff656f75bcf502d27acb3141b8cb", null ],
      [ "i2sQueue", "group__i2s__driver.html#a756e104fab56948d94a5098cf6cd2ec1", null ],
      [ "queueUser", "group__i2s__driver.html#a9f0c6aaa58ac324d2fd09972170f0b39", null ],
      [ "queueDriver", "group__i2s__driver.html#abfe39b2170ac906b2b86387b456ad272", null ],
      [ "errorCount", "group__i2s__driver.html#ab0ca51adf36c91f7b63138f617fba694", null ],
      [ "transferCount", "group__i2s__driver.html#a4b5c6c53947f229fac4a8b473a0254c0", null ],
      [ "watermark", "group__i2s__driver.html#abe5af63056b023f9b790c69c2a466635", null ]
    ] ],
    [ "FSL_I2S_DRIVER_VERSION", "group__i2s__driver.html#gaf19f85eb3c13d6b8b839a62c30009710", null ],
    [ "I2S_NUM_BUFFERS", "group__i2s__driver.html#gae17f5f49178cc3ab239917f6454792d0", null ],
    [ "i2s_transfer_callback_t", "group__i2s__driver.html#gaf04bcafbb89c4d0a47ac0acfcc958faf", null ],
    [ "_i2s_status", "group__i2s__driver.html#ga696c1fb76e6e1c506ff62e8572b342fd", [
      [ "kStatus_I2S_BufferComplete", "group__i2s__driver.html#gga696c1fb76e6e1c506ff62e8572b342fda634ef87c7515bb4037dfaaea2d75b6e1", null ],
      [ "kStatus_I2S_Done", "group__i2s__driver.html#gga696c1fb76e6e1c506ff62e8572b342fdaf123c761ef2616e6f48031b3f1103e48", null ],
      [ "kStatus_I2S_Busy", "group__i2s__driver.html#gga696c1fb76e6e1c506ff62e8572b342fdaf73f348e9a8fe5cdf3c1ad097378d5b2", null ]
    ] ],
    [ "i2s_flags_t", "group__i2s__driver.html#ga7806dd07d6393101b44143e4707dfcf1", [
      [ "kI2S_TxErrorFlag", "group__i2s__driver.html#gga7806dd07d6393101b44143e4707dfcf1a4de8d6343bcebc3f15243732806fc564", null ],
      [ "kI2S_TxLevelFlag", "group__i2s__driver.html#gga7806dd07d6393101b44143e4707dfcf1ac3daa0c88fabb995e62e7f2803db4155", null ],
      [ "kI2S_RxErrorFlag", "group__i2s__driver.html#gga7806dd07d6393101b44143e4707dfcf1af26884847511b34b0197bb70c7c36989", null ],
      [ "kI2S_RxLevelFlag", "group__i2s__driver.html#gga7806dd07d6393101b44143e4707dfcf1ac29fb67960b694a776c8159a7b4487db", null ]
    ] ],
    [ "i2s_master_slave_t", "group__i2s__driver.html#gadebb589e2ab53e2443229481d9047b47", [
      [ "kI2S_MasterSlaveNormalSlave", "group__i2s__driver.html#ggadebb589e2ab53e2443229481d9047b47a84bedf24f43a46b87a774e479da5b82b", null ],
      [ "kI2S_MasterSlaveWsSyncMaster", "group__i2s__driver.html#ggadebb589e2ab53e2443229481d9047b47a2d7515b2fc53986d17636f3e16412b08", null ],
      [ "kI2S_MasterSlaveExtSckMaster", "group__i2s__driver.html#ggadebb589e2ab53e2443229481d9047b47ad0b9c4656a3d40c936a7369d75c8d346", null ],
      [ "kI2S_MasterSlaveNormalMaster", "group__i2s__driver.html#ggadebb589e2ab53e2443229481d9047b47a13ab2820eda5d509bb3ceff6fb28758b", null ]
    ] ],
    [ "i2s_mode_t", "group__i2s__driver.html#gacad3e5bee8b5bde941b18e4e244f7127", [
      [ "kI2S_ModeI2sClassic", "group__i2s__driver.html#ggacad3e5bee8b5bde941b18e4e244f7127ac0e4769e50d6318229c2dc31cd99bae3", null ],
      [ "kI2S_ModeDspWs50", "group__i2s__driver.html#ggacad3e5bee8b5bde941b18e4e244f7127a11364b25e57662ef4d1fc288ab12ddb1", null ],
      [ "kI2S_ModeDspWsShort", "group__i2s__driver.html#ggacad3e5bee8b5bde941b18e4e244f7127a6a42cbdc569ac3c30706fb6ba61db9d9", null ],
      [ "kI2S_ModeDspWsLong", "group__i2s__driver.html#ggacad3e5bee8b5bde941b18e4e244f7127a3bb54e6d9b9d1793637e1b82e323e829", null ]
    ] ],
    [ "I2S_TxInit", "group__i2s__driver.html#ga5d68b2f69b625a753d498dab6758c5b9", null ],
    [ "I2S_RxInit", "group__i2s__driver.html#gaa71e9b55c275d5469c1e3f723dc55213", null ],
    [ "I2S_TxGetDefaultConfig", "group__i2s__driver.html#ga905cb886175d4a1f9ac2fbf8d2722cf6", null ],
    [ "I2S_RxGetDefaultConfig", "group__i2s__driver.html#ga8356de62fea66f67a9803ee89a78805a", null ],
    [ "I2S_Deinit", "group__i2s__driver.html#ga68e9243ae7345a144668c7e04f1f52ac", null ],
    [ "I2S_TxTransferCreateHandle", "group__i2s__driver.html#ga0e630bdd0adb5a39b52e374ede7bc07f", null ],
    [ "I2S_TxTransferNonBlocking", "group__i2s__driver.html#ga8e341c9ce48e5be03e120ea332758048", null ],
    [ "I2S_TxTransferAbort", "group__i2s__driver.html#ga25659519f70757bf2db3831f7e807bd8", null ],
    [ "I2S_RxTransferCreateHandle", "group__i2s__driver.html#gabd8c54c9689fd89024c83bb883693c16", null ],
    [ "I2S_RxTransferNonBlocking", "group__i2s__driver.html#ga454cba72bec3ebeecf865570ce6ed900", null ],
    [ "I2S_RxTransferAbort", "group__i2s__driver.html#ga32eb018b43745a695ef26cec1ff73a73", null ],
    [ "I2S_TransferGetCount", "group__i2s__driver.html#ga14ef749f4300fe7b1fd38277f0ef167f", null ],
    [ "I2S_TransferGetErrorCount", "group__i2s__driver.html#gae3d210a830859e93103931b4baf83586", null ],
    [ "I2S_Enable", "group__i2s__driver.html#gac109aadc23d6f31e6f8420fa3f4018fd", null ],
    [ "I2S_Disable", "group__i2s__driver.html#ga4b393781ae83a07d4d3e99972429cfa1", null ],
    [ "I2S_EnableInterrupts", "group__i2s__driver.html#ga435263725c056eccaa82eb1947e162aa", null ],
    [ "I2S_DisableInterrupts", "group__i2s__driver.html#gaa6f8ed52f3cd5c63b3a3716a70c77223", null ],
    [ "I2S_GetEnabledInterrupts", "group__i2s__driver.html#ga2d96a8f63e454de7225a98541b6461b9", null ],
    [ "I2S_TxHandleIRQ", "group__i2s__driver.html#ga2423f95dc2c00bce6a9de6d868d1fde2", null ],
    [ "I2S_RxHandleIRQ", "group__i2s__driver.html#gaeff61f8cbd780c881c593ad059f98338", null ]
];
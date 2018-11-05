var group__i2c__master__driver =
[
    [ "i2c_master_config_t", "group__i2c__master__driver.html#structi2c__master__config__t", [
      [ "enableMaster", "group__i2c__master__driver.html#a2419131a10906475fd31950f17ac8895", null ],
      [ "baudRate_Bps", "group__i2c__master__driver.html#a2186844dc87bcde999fc12005f4c550a", null ],
      [ "enableTimeout", "group__i2c__master__driver.html#a1c6831135c83c013d2a3cfc3e7210357", null ]
    ] ],
    [ "i2c_master_transfer_t", "group__i2c__master__driver.html#struct__i2c__master__transfer", [
      [ "flags", "group__i2c__master__driver.html#a8835787e1b0f9a4b8868e7cbe53e45d5", null ],
      [ "slaveAddress", "group__i2c__master__driver.html#a97891bd050609c6dcd1276714277e480", null ],
      [ "direction", "group__i2c__master__driver.html#ac5b89dc9115b7760431c981e0c1fd443", null ],
      [ "subaddress", "group__i2c__master__driver.html#ae7facb612714785d4e143e57d47a5af3", null ],
      [ "subaddressSize", "group__i2c__master__driver.html#aeec8dccf4a49f03ff9a40f5982a24796", null ],
      [ "data", "group__i2c__master__driver.html#a8c7f63bd1e7de04e40f5cd733f5c7388", null ],
      [ "dataSize", "group__i2c__master__driver.html#a68e2dd69e1e34b1b505270e1e07e8a34", null ]
    ] ],
    [ "i2c_master_handle_t", "group__i2c__master__driver.html#struct__i2c__master__handle", [
      [ "state", "group__i2c__master__driver.html#add7ec18bc8239c5c87ffcec2fbcf5dd8", null ],
      [ "transferCount", "group__i2c__master__driver.html#a5943d16f5ed6e7c4ebd334cdbc0e2afc", null ],
      [ "remainingBytes", "group__i2c__master__driver.html#aca481b5d3ca6dce53cf440b534ff80c2", null ],
      [ "buf", "group__i2c__master__driver.html#a9577474c0c4395355174df2b016108de", null ],
      [ "transfer", "group__i2c__master__driver.html#a6858d3525f762d7aded20e6c95eb19fc", null ],
      [ "completionCallback", "group__i2c__master__driver.html#a15b84b8a94c2b2e5ace0a695c79edd84", null ],
      [ "userData", "group__i2c__master__driver.html#aad7df570c53adb2e80acd2ba0d39d109", null ]
    ] ],
    [ "i2c_master_transfer_callback_t", "group__i2c__master__driver.html#gad292a48f957a9b76593c1779d9dce497", null ],
    [ "_i2c_master_flags", "group__i2c__master__driver.html#gac938392418ba1a891983d11ef5c9f2a3", [
      [ "kI2C_MasterPendingFlag", "group__i2c__master__driver.html#ggac938392418ba1a891983d11ef5c9f2a3a9cc49a1ca21e87058a7fbf733cce0e62", null ],
      [ "kI2C_MasterArbitrationLostFlag", "group__i2c__master__driver.html#ggac938392418ba1a891983d11ef5c9f2a3a7b487779b58af99a7a8c179545f78ccd", null ],
      [ "kI2C_MasterStartStopErrorFlag", "group__i2c__master__driver.html#ggac938392418ba1a891983d11ef5c9f2a3ab5cec1d9be7dc5fb2c106e2b87889517", null ]
    ] ],
    [ "i2c_direction_t", "group__i2c__master__driver.html#gab49c827b45635206f06e5737606e4611", [
      [ "kI2C_Write", "group__i2c__master__driver.html#ggab49c827b45635206f06e5737606e4611a93b476b469c2a4bfed5916b458ae9bb0", null ],
      [ "kI2C_Read", "group__i2c__master__driver.html#ggab49c827b45635206f06e5737606e4611a3a7bb24dc8d1c1be8925603eeafe9b30", null ]
    ] ],
    [ "_i2c_master_transfer_flags", "group__i2c__master__driver.html#ga87ea07668194cfb46c7c368d2cb42433", [
      [ "kI2C_TransferDefaultFlag", "group__i2c__master__driver.html#gga87ea07668194cfb46c7c368d2cb42433ae80f7b768b1621e42ac965d3d23de5e2", null ],
      [ "kI2C_TransferNoStartFlag", "group__i2c__master__driver.html#gga87ea07668194cfb46c7c368d2cb42433a5cb44bf860c0482c0ca0165cf0d31d8a", null ],
      [ "kI2C_TransferRepeatedStartFlag", "group__i2c__master__driver.html#gga87ea07668194cfb46c7c368d2cb42433aadf112471ef12f194985a093cd9b9721", null ],
      [ "kI2C_TransferNoStopFlag", "group__i2c__master__driver.html#gga87ea07668194cfb46c7c368d2cb42433afb8aeea71b5b7475e3d3df86220db566", null ]
    ] ],
    [ "_i2c_transfer_states", "group__i2c__master__driver.html#gab08c1a0d50859637b4305687278941ee", null ],
    [ "I2C_MasterGetDefaultConfig", "group__i2c__master__driver.html#gad69f2d63ea756dda60749ff6b09f587d", null ],
    [ "I2C_MasterInit", "group__i2c__master__driver.html#ga29f967b3ae8487a36ba2a58deb01ccae", null ],
    [ "I2C_MasterDeinit", "group__i2c__master__driver.html#gadcf7122f0a38d4d9da0f052fcb167957", null ],
    [ "I2C_GetInstance", "group__i2c__master__driver.html#ga07cc6bd20f700249c335893427bf462a", null ],
    [ "I2C_MasterReset", "group__i2c__master__driver.html#ga09137caccbe9a57f6b2ca5e1ce9bdc50", null ],
    [ "I2C_MasterEnable", "group__i2c__master__driver.html#ga164d97e40eb5ad625a41ad43cf938153", null ],
    [ "I2C_GetStatusFlags", "group__i2c__master__driver.html#ga441017241044ffdb828385e473f5fb58", null ],
    [ "I2C_MasterClearStatusFlags", "group__i2c__master__driver.html#ga437271ffb955b77df0a5dee9ea80cc63", null ],
    [ "I2C_EnableInterrupts", "group__i2c__master__driver.html#ga0f6f1f2fe150661f8eb0a072665a9020", null ],
    [ "I2C_DisableInterrupts", "group__i2c__master__driver.html#gac3e053611231cc886bfa38831a1ac26e", null ],
    [ "I2C_GetEnabledInterrupts", "group__i2c__master__driver.html#gae652236a25f1b4828d596f32190655f8", null ],
    [ "I2C_MasterSetBaudRate", "group__i2c__master__driver.html#gafeab1d5249a8b39c8d9e1a54a85c23f0", null ],
    [ "I2C_MasterGetBusIdleState", "group__i2c__master__driver.html#ga4b7617034ab51d77786141873fd6ee1c", null ],
    [ "I2C_MasterStart", "group__i2c__master__driver.html#ga7664234966e4162e952b6d57efcaa8a6", null ],
    [ "I2C_MasterStop", "group__i2c__master__driver.html#gaf46a8cc094fc18c6cadea8de71448723", null ],
    [ "I2C_MasterRepeatedStart", "group__i2c__master__driver.html#ga8137f7b333aafe1e3ff6c9d3852b7dbd", null ],
    [ "I2C_MasterWriteBlocking", "group__i2c__master__driver.html#ga3b4d71b59e118adc63afbc5434e5e7dd", null ],
    [ "I2C_MasterReadBlocking", "group__i2c__master__driver.html#gaba04c62ee636dd8317c2235c3c82aa3b", null ],
    [ "I2C_MasterTransferBlocking", "group__i2c__master__driver.html#ga35cae3a5b4aa50ce8db28e3eb703a027", null ],
    [ "I2C_MasterTransferCreateHandle", "group__i2c__master__driver.html#ga31e3de02b57801c6896e6045c4a31268", null ],
    [ "I2C_MasterTransferNonBlocking", "group__i2c__master__driver.html#gac7f76a04b6f6873c52ad176d58901fe9", null ],
    [ "I2C_MasterTransferGetCount", "group__i2c__master__driver.html#ga7d720842e68161d2d52d56d119f20665", null ],
    [ "I2C_MasterTransferAbort", "group__i2c__master__driver.html#gabb386e53b2bc64e29b2915bdfac36800", null ],
    [ "I2C_MasterTransferHandleIRQ", "group__i2c__master__driver.html#gafbf792484d11820561fa22bb27f73284", null ]
];
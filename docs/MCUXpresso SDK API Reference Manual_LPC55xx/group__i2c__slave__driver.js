var group__i2c__slave__driver =
[
    [ "i2c_slave_address_t", "group__i2c__slave__driver.html#structi2c__slave__address__t", [
      [ "address", "group__i2c__slave__driver.html#ad7af2a58faba6a178daa97bd06ebce36", null ],
      [ "addressDisable", "group__i2c__slave__driver.html#aca3dcdb3ab2710d991ada52d64bf102c", null ]
    ] ],
    [ "i2c_slave_config_t", "group__i2c__slave__driver.html#structi2c__slave__config__t", [
      [ "address0", "group__i2c__slave__driver.html#a4738c7cd55260f7e8a3825d0b2278a34", null ],
      [ "address1", "group__i2c__slave__driver.html#ae19c45c96699bb3a6821150ab820b029", null ],
      [ "address2", "group__i2c__slave__driver.html#ae855ba5c53f7e585c44eae8bada85e9d", null ],
      [ "address3", "group__i2c__slave__driver.html#a213d1737a633686701581a09859213a6", null ],
      [ "qualMode", "group__i2c__slave__driver.html#a51577d06d8812b0c999957898ec016b5", null ],
      [ "qualAddress", "group__i2c__slave__driver.html#a4c9d09c35c4641d38bf565de31b07320", null ],
      [ "busSpeed", "group__i2c__slave__driver.html#a14acc40a290c779fde0825f3a8bdbb25", null ],
      [ "enableSlave", "group__i2c__slave__driver.html#a92d20835618a946b8f7702455877ef7a", null ]
    ] ],
    [ "i2c_slave_transfer_t", "group__i2c__slave__driver.html#structi2c__slave__transfer__t", [
      [ "handle", "group__i2c__slave__driver.html#ab74516c1edb1424ddb1554de7cae69bc", null ],
      [ "event", "group__i2c__slave__driver.html#ad0a9e837e9df16d89bab98a78af5bd10", null ],
      [ "receivedAddress", "group__i2c__slave__driver.html#ad6f3b291ee81b69cf91c161ae26d65ae", null ],
      [ "eventMask", "group__i2c__slave__driver.html#a1be21257ec375e09106dfe242c02beea", null ],
      [ "rxData", "group__i2c__slave__driver.html#a3e497078c6038baa8748ce5ecebc7e3d", null ],
      [ "txData", "group__i2c__slave__driver.html#afaff4f7ef2ae4ed49eff607a58db18c2", null ],
      [ "txSize", "group__i2c__slave__driver.html#a3d2ef683b0439ce83e3d54e8823ebc38", null ],
      [ "rxSize", "group__i2c__slave__driver.html#a75e06358c0d5840a1dfc19a029e10ba8", null ],
      [ "transferredCount", "group__i2c__slave__driver.html#a0394563c8d0f9eeeecd242a65cee2ad4", null ],
      [ "completionStatus", "group__i2c__slave__driver.html#a35adbf64ca65dd2c1b52f9260f5b9e90", null ]
    ] ],
    [ "i2c_slave_handle_t", "group__i2c__slave__driver.html#struct__i2c__slave__handle", [
      [ "transfer", "group__i2c__slave__driver.html#a9934cd1a4d4b4e02ac5856e28f86a229", null ],
      [ "isBusy", "group__i2c__slave__driver.html#a81ece18a362fc9779750be91f7cc6b30", null ],
      [ "slaveFsm", "group__i2c__slave__driver.html#a18589a5de9b06b2eaf7d4260eac03a58", null ],
      [ "callback", "group__i2c__slave__driver.html#a7229e894f762ead4bd08b4add49e6bc2", null ],
      [ "userData", "group__i2c__slave__driver.html#a98ea5e99278b386e2ddb99d45a9750ee", null ]
    ] ],
    [ "i2c_slave_transfer_callback_t", "group__i2c__slave__driver.html#ga0704c8f14da38feb11555e4127d86a18", null ],
    [ "_i2c_slave_flags", "group__i2c__slave__driver.html#ga419f2fc2e0684671c00ee97e962bbe0c", [
      [ "kI2C_SlavePendingFlag", "group__i2c__slave__driver.html#gga419f2fc2e0684671c00ee97e962bbe0cabd274f2defb8f0f8d2f6c2fcdd71c63c", null ],
      [ "kI2C_SlaveNotStretching", "group__i2c__slave__driver.html#gga419f2fc2e0684671c00ee97e962bbe0caf7ad09504bcf02f9c1177431ee44562e", null ],
      [ "kI2C_SlaveSelected", "group__i2c__slave__driver.html#gga419f2fc2e0684671c00ee97e962bbe0ca2d0c7dc4bf2815768510b916b7c1d54e", null ],
      [ "kI2C_SaveDeselected", "group__i2c__slave__driver.html#gga419f2fc2e0684671c00ee97e962bbe0cad834aafd823863602d905d4b3d8c9430", null ]
    ] ],
    [ "i2c_slave_address_register_t", "group__i2c__slave__driver.html#ga833a7311515f1a3bf5cb8da2355cc661", [
      [ "kI2C_SlaveAddressRegister0", "group__i2c__slave__driver.html#gga833a7311515f1a3bf5cb8da2355cc661a39e0e158674bd8a6e254a6a42b5ce50f", null ],
      [ "kI2C_SlaveAddressRegister1", "group__i2c__slave__driver.html#gga833a7311515f1a3bf5cb8da2355cc661a42a589b5e31f29ddc41088c3fb4bfcf1", null ],
      [ "kI2C_SlaveAddressRegister2", "group__i2c__slave__driver.html#gga833a7311515f1a3bf5cb8da2355cc661a8381dd229f545df5e6a32ee5f0aac871", null ],
      [ "kI2C_SlaveAddressRegister3", "group__i2c__slave__driver.html#gga833a7311515f1a3bf5cb8da2355cc661a9e73a2b09fe3943d51849a7d87fbe8a3", null ]
    ] ],
    [ "i2c_slave_address_qual_mode_t", "group__i2c__slave__driver.html#ga719dc02b99647eb8f08a05d4d6066c51", [
      [ "kI2C_QualModeMask", "group__i2c__slave__driver.html#gga719dc02b99647eb8f08a05d4d6066c51af542f0269c83549e7b492911e281a902", null ],
      [ "kI2C_QualModeExtend", "group__i2c__slave__driver.html#gga719dc02b99647eb8f08a05d4d6066c51a6b755a1d30a626380730ce947d09335f", null ]
    ] ],
    [ "i2c_slave_bus_speed_t", "group__i2c__slave__driver.html#ga5f368505586dd356fc680711023ace7f", null ],
    [ "i2c_slave_transfer_event_t", "group__i2c__slave__driver.html#gac53e5c96a2eed1b5a95b7d84be48f4ac", [
      [ "kI2C_SlaveAddressMatchEvent", "group__i2c__slave__driver.html#ggac53e5c96a2eed1b5a95b7d84be48f4acacb1ae174dd6798a6fd79800a9e39a3c6", null ],
      [ "kI2C_SlaveTransmitEvent", "group__i2c__slave__driver.html#ggac53e5c96a2eed1b5a95b7d84be48f4aca2f85039a57379838909876a1d509b7aa", null ],
      [ "kI2C_SlaveReceiveEvent", "group__i2c__slave__driver.html#ggac53e5c96a2eed1b5a95b7d84be48f4acaac8593e808a8137d1b5d3a51640779de", null ],
      [ "kI2C_SlaveCompletionEvent", "group__i2c__slave__driver.html#ggac53e5c96a2eed1b5a95b7d84be48f4aca38cca3cd668e73f51b8e574835d419df", null ],
      [ "kI2C_SlaveDeselectedEvent", "group__i2c__slave__driver.html#ggac53e5c96a2eed1b5a95b7d84be48f4aca5e765c1c316a51908b68f1f32a7b524d", null ],
      [ "kI2C_SlaveAllEvents", "group__i2c__slave__driver.html#ggac53e5c96a2eed1b5a95b7d84be48f4aca94b030ebdac378a84961893ae567bfbc", null ]
    ] ],
    [ "i2c_slave_fsm_t", "group__i2c__slave__driver.html#gad6564299304730416461f62255fcd37c", null ],
    [ "I2C_SlaveGetDefaultConfig", "group__i2c__slave__driver.html#ga7115f80e28b62bbbd3be1a2a918529ba", null ],
    [ "I2C_SlaveInit", "group__i2c__slave__driver.html#ga175a81b5ab74dd78469d6cdea690d8cc", null ],
    [ "I2C_SlaveSetAddress", "group__i2c__slave__driver.html#gacf721065f83982e3df651111183bf7b2", null ],
    [ "I2C_SlaveDeinit", "group__i2c__slave__driver.html#gadb50014db29dd75ea2001fb7bc837e77", null ],
    [ "I2C_SlaveEnable", "group__i2c__slave__driver.html#gacbb93d5ea4f4b40038f41e0e942a9ee1", null ],
    [ "I2C_SlaveClearStatusFlags", "group__i2c__slave__driver.html#ga545c1a43d8d49c4675de493d45a4063a", null ],
    [ "I2C_SlaveWriteBlocking", "group__i2c__slave__driver.html#ga280404c6b1ede542f2c1c20bd382869c", null ],
    [ "I2C_SlaveReadBlocking", "group__i2c__slave__driver.html#ga89baaf8bc82b19c837032665e89a1707", null ],
    [ "I2C_SlaveTransferCreateHandle", "group__i2c__slave__driver.html#gac9b90f575d92ff8ad3cc350a5c8ad1b9", null ],
    [ "I2C_SlaveTransferNonBlocking", "group__i2c__slave__driver.html#gaa387d81ea08a9d4b39cfeb3dfec505f7", null ],
    [ "I2C_SlaveSetSendBuffer", "group__i2c__slave__driver.html#gac7602b6897596348fe190e4fc63f1381", null ],
    [ "I2C_SlaveSetReceiveBuffer", "group__i2c__slave__driver.html#ga25b9c35089f0fc966facec3bf874c840", null ],
    [ "I2C_SlaveGetReceivedAddress", "group__i2c__slave__driver.html#ga4c9fc06eeb8a3beb024f6525e4d99a0b", null ],
    [ "I2C_SlaveTransferAbort", "group__i2c__slave__driver.html#ga5ae9f5e6e854d14d33ba3029128bd6f0", null ],
    [ "I2C_SlaveTransferGetCount", "group__i2c__slave__driver.html#gabf38a498bd3527ea0b7cc947ae71e17e", null ],
    [ "I2C_SlaveTransferHandleIRQ", "group__i2c__slave__driver.html#ga3bc939e5dbdc165169f58b8d47d1742d", null ]
];
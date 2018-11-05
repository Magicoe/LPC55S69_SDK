var group__spi__driver =
[
    [ "spi_delay_config_t", "group__spi__driver.html#structspi__delay__config__t", [
      [ "preDelay", "group__spi__driver.html#a24a2584817f03d1ace0a6cecc718bc09", null ],
      [ "postDelay", "group__spi__driver.html#a960b87887e431dabbb5641109cb56d90", null ],
      [ "frameDelay", "group__spi__driver.html#a9609e5c510bf5d0c120a403ed40aed42", null ],
      [ "transferDelay", "group__spi__driver.html#a7ea0733fc746e2bafe1c0999db7d4804", null ]
    ] ],
    [ "spi_master_config_t", "group__spi__driver.html#structspi__master__config__t", [
      [ "enableLoopback", "group__spi__driver.html#a384bc00ef4cd5b4e9cb6a4d48ec336bd", null ],
      [ "enableMaster", "group__spi__driver.html#aa033bd20cfbb1a14f0fd43f4b31bb27e", null ],
      [ "polarity", "group__spi__driver.html#ac8d9a8c7a526b96cbe3105d6e06b1cce", null ],
      [ "phase", "group__spi__driver.html#a21b6f74dbeb89a81460cd618b7da042b", null ],
      [ "direction", "group__spi__driver.html#aace9261acfad8526f3d5b0b59346629a", null ],
      [ "baudRate_Bps", "group__spi__driver.html#ae7695987e044d80983fd98a43812b1ea", null ],
      [ "dataWidth", "group__spi__driver.html#abd75f6a0a3d5a102c0f3239df709b3a5", null ],
      [ "sselNum", "group__spi__driver.html#af1b1d0e86acdcb17ddf8bce7bfbb39b3", null ],
      [ "sselPol", "group__spi__driver.html#a56f6eaccdde14024b8799a4f62c78f6e", null ],
      [ "txWatermark", "group__spi__driver.html#a63118dfe890616b78343c46c4ea899b1", null ],
      [ "rxWatermark", "group__spi__driver.html#adce084d034655724c6ae61ab3836eb2e", null ],
      [ "delayConfig", "group__spi__driver.html#a787477e7b0ae7833dcbdb1c85fef361c", null ]
    ] ],
    [ "spi_slave_config_t", "group__spi__driver.html#structspi__slave__config__t", [
      [ "enableSlave", "group__spi__driver.html#a9fcd3fae6d886c7dfd0c3fd4cd51e6fe", null ],
      [ "polarity", "group__spi__driver.html#a32c240620581fcb133877d4fbda21226", null ],
      [ "phase", "group__spi__driver.html#ac6d76d6b354491d5b43756fdf8ef07d6", null ],
      [ "direction", "group__spi__driver.html#ad4eb458a0c1a0c0934eec53d71b78ffb", null ],
      [ "dataWidth", "group__spi__driver.html#a52fca395fc7960baf31a25faac7924ef", null ],
      [ "sselPol", "group__spi__driver.html#a4b0ff89b5c80c79219f79c92cd1606a9", null ],
      [ "txWatermark", "group__spi__driver.html#a1a5ea2f93ef1f280d53f821c2f67995a", null ],
      [ "rxWatermark", "group__spi__driver.html#a909bef9a8771b2735d1792546bc1b45d", null ]
    ] ],
    [ "spi_transfer_t", "group__spi__driver.html#structspi__transfer__t", [
      [ "txData", "group__spi__driver.html#addc4f20760a1a62c4d20cdf9443f3af3", null ],
      [ "rxData", "group__spi__driver.html#a28b878b99d5ec790b5c8f3bafe140da0", null ],
      [ "configFlags", "group__spi__driver.html#a582eea734badd0049c98ea3cf89b3e4b", null ],
      [ "dataSize", "group__spi__driver.html#ae003337c9398e5f5e3189f5b2ba6335b", null ]
    ] ],
    [ "spi_half_duplex_transfer_t", "group__spi__driver.html#structspi__half__duplex__transfer__t", [
      [ "txData", "group__spi__driver.html#a720d55f285c43087c893c2f48018e0da", null ],
      [ "rxData", "group__spi__driver.html#a93f6fd7e0e297152075af5a3a2c6929c", null ],
      [ "txDataSize", "group__spi__driver.html#a3b296a6f72288eb7fa86067e51dc231d", null ],
      [ "rxDataSize", "group__spi__driver.html#afe4c4f18c78df9435f464ba94cefd052", null ],
      [ "configFlags", "group__spi__driver.html#a98635c2168a95161626ecdc0d22dab4d", null ],
      [ "isPcsAssertInTransfer", "group__spi__driver.html#a23f645fd24ca70055b770082586b1a59", null ],
      [ "isTransmitFirst", "group__spi__driver.html#a1cd036b1eb498f75d8988916dbea3345", null ]
    ] ],
    [ "spi_config_t", "group__spi__driver.html#structspi__config__t", null ],
    [ "spi_master_handle_t", "group__spi__driver.html#struct__spi__master__handle", [
      [ "txData", "group__spi__driver.html#a6cb4626f6b63c70fa21fe2e3338dc915", null ],
      [ "rxData", "group__spi__driver.html#a69f260bde2e1728233f835a148b2f51b", null ],
      [ "txRemainingBytes", "group__spi__driver.html#a62fa101ddfc970e7b6bcba0b2eb6b869", null ],
      [ "rxRemainingBytes", "group__spi__driver.html#a23b2531ea36b53e42cd6b2a3780d3017", null ],
      [ "toReceiveCount", "group__spi__driver.html#aa56c1826cc161d7b3dbc22ffcd9fb6a9", null ],
      [ "totalByteCount", "group__spi__driver.html#a4e19f9299f123f181536c6bd4456e50a", null ],
      [ "state", "group__spi__driver.html#ae7933252a37be998d127217f34f6fd16", null ],
      [ "callback", "group__spi__driver.html#a445e70d437c917e6af1b4037bdbb6a3f", null ],
      [ "userData", "group__spi__driver.html#ab8d01b85149d749ab1c748bb5116b90e", null ],
      [ "dataWidth", "group__spi__driver.html#a11b51e4d88d3f8d9137a13ce301ed46c", null ],
      [ "sselNum", "group__spi__driver.html#a9009d89fcc80d56cfb96bd7953e791c4", null ],
      [ "configFlags", "group__spi__driver.html#a798a5bb17e5685378c9f81ac945b0135", null ],
      [ "txWatermark", "group__spi__driver.html#a9ca76c0d294769733dc3d8af89354d8d", null ],
      [ "rxWatermark", "group__spi__driver.html#af4e445d0b89ce27e12a1ea4282742d74", null ]
    ] ],
    [ "FSL_SPI_DRIVER_VERSION", "group__spi__driver.html#gaeb6046de02dffe9248de71332fac246e", null ],
    [ "SPI_DUMMYDATA", "group__spi__driver.html#ga1541129ba8262e1649592b5109e2074c", null ],
    [ "spi_slave_handle_t", "group__spi__driver.html#gad267cfee3a876b2860217ff94f03f574", null ],
    [ "spi_master_callback_t", "group__spi__driver.html#gae9bd140aeb645efab6c7552b3994e01a", null ],
    [ "spi_slave_callback_t", "group__spi__driver.html#ga86b45b85e036adc762eed5bcd2a0491d", null ],
    [ "spi_xfer_option_t", "group__spi__driver.html#ga632c5d421df2f4ef489e8a56e8545408", [
      [ "kSPI_FrameDelay", "group__spi__driver.html#gga632c5d421df2f4ef489e8a56e8545408a368ad1fd03f7af182af9a61c7f4967ed", null ],
      [ "kSPI_FrameAssert", "group__spi__driver.html#gga632c5d421df2f4ef489e8a56e8545408adbcf2b399fc2e5f1ad5c2586117a1ae9", null ]
    ] ],
    [ "spi_shift_direction_t", "group__spi__driver.html#gaa68518c16202382c2e1f1c7c66a9d53d", [
      [ "kSPI_MsbFirst", "group__spi__driver.html#ggaa68518c16202382c2e1f1c7c66a9d53dae7b997e8cff761aab865cbd42c4c3989", null ],
      [ "kSPI_LsbFirst", "group__spi__driver.html#ggaa68518c16202382c2e1f1c7c66a9d53da20ac53684c3e1ad338553cecbfab94a9", null ]
    ] ],
    [ "spi_clock_polarity_t", "group__spi__driver.html#ga3e5a7cd043c9596779bc23b34cb3d1f9", [
      [ "kSPI_ClockPolarityActiveHigh", "group__spi__driver.html#gga3e5a7cd043c9596779bc23b34cb3d1f9a031c0ee2693cf1fd621e8b8cba676629", null ],
      [ "kSPI_ClockPolarityActiveLow", "group__spi__driver.html#gga3e5a7cd043c9596779bc23b34cb3d1f9acfc8257079c9604086622df6de326aea", null ]
    ] ],
    [ "spi_clock_phase_t", "group__spi__driver.html#ga9ad313685ade497f5cbcb71c74a1b4dc", [
      [ "kSPI_ClockPhaseFirstEdge", "group__spi__driver.html#gga9ad313685ade497f5cbcb71c74a1b4dcad15d61c6fd642f13101edbf401d0c72e", null ],
      [ "kSPI_ClockPhaseSecondEdge", "group__spi__driver.html#gga9ad313685ade497f5cbcb71c74a1b4dca18a98985c1f7dd56175e4a2724db3675", null ]
    ] ],
    [ "spi_txfifo_watermark_t", "group__spi__driver.html#ga9a479127a2051ded7945115657adc162", [
      [ "kSPI_TxFifo0", "group__spi__driver.html#gga9a479127a2051ded7945115657adc162ab00ed44c27cc2f5c8be6a37a7e54bbb0", null ],
      [ "kSPI_TxFifo1", "group__spi__driver.html#gga9a479127a2051ded7945115657adc162ad84aa8fe681fb1922108dd71035a1e7a", null ],
      [ "kSPI_TxFifo2", "group__spi__driver.html#gga9a479127a2051ded7945115657adc162af0f35b4313810fb0a721f6d0c96e361d", null ],
      [ "kSPI_TxFifo3", "group__spi__driver.html#gga9a479127a2051ded7945115657adc162a583e03e419cab47b3a257f39a96f9098", null ],
      [ "kSPI_TxFifo4", "group__spi__driver.html#gga9a479127a2051ded7945115657adc162add72390240691eb878a8bc236b0ae0f1", null ],
      [ "kSPI_TxFifo5", "group__spi__driver.html#gga9a479127a2051ded7945115657adc162a2d97d5d6a34b441ebdd1a5dcf9cd0d49", null ],
      [ "kSPI_TxFifo6", "group__spi__driver.html#gga9a479127a2051ded7945115657adc162a50675672a2887ec04a4a2d11c183958b", null ],
      [ "kSPI_TxFifo7", "group__spi__driver.html#gga9a479127a2051ded7945115657adc162a4f8e84453e704f97a592944ff3dc5c50", null ]
    ] ],
    [ "spi_rxfifo_watermark_t", "group__spi__driver.html#ga79f862820ec2bcbb8184ce9de72949a6", [
      [ "kSPI_RxFifo1", "group__spi__driver.html#gga79f862820ec2bcbb8184ce9de72949a6a2d49ceb52f1bf6c51e2514b35e5d9804", null ],
      [ "kSPI_RxFifo2", "group__spi__driver.html#gga79f862820ec2bcbb8184ce9de72949a6a90eb93fdd80d4baf71ff3d75d4df2454", null ],
      [ "kSPI_RxFifo3", "group__spi__driver.html#gga79f862820ec2bcbb8184ce9de72949a6ab27946413b575e3dd27e0a37c8ccb17b", null ],
      [ "kSPI_RxFifo4", "group__spi__driver.html#gga79f862820ec2bcbb8184ce9de72949a6af6dcb50b13a1d8208d1d977ce79ff662", null ],
      [ "kSPI_RxFifo5", "group__spi__driver.html#gga79f862820ec2bcbb8184ce9de72949a6a2c961e2e2c24014368dfe867d2295f3d", null ],
      [ "kSPI_RxFifo6", "group__spi__driver.html#gga79f862820ec2bcbb8184ce9de72949a6ad8bce70560016fbd76c7e1a9e5a2369f", null ],
      [ "kSPI_RxFifo7", "group__spi__driver.html#gga79f862820ec2bcbb8184ce9de72949a6a24b9a255d74d6a5b47d59b82717586dc", null ],
      [ "kSPI_RxFifo8", "group__spi__driver.html#gga79f862820ec2bcbb8184ce9de72949a6aba4452e3c770cac6e822a0f85e2b53f3", null ]
    ] ],
    [ "spi_data_width_t", "group__spi__driver.html#gafa691c5329a7325ee57c9f06fa295731", [
      [ "kSPI_Data4Bits", "group__spi__driver.html#ggafa691c5329a7325ee57c9f06fa295731a0cb8b30c67c53c6d53712d5bacc818ce", null ],
      [ "kSPI_Data5Bits", "group__spi__driver.html#ggafa691c5329a7325ee57c9f06fa295731a7232b4afb5bd82503252ce608638e6eb", null ],
      [ "kSPI_Data6Bits", "group__spi__driver.html#ggafa691c5329a7325ee57c9f06fa295731a6604fcf5c8bebf904c5b072295027bba", null ],
      [ "kSPI_Data7Bits", "group__spi__driver.html#ggafa691c5329a7325ee57c9f06fa295731a2c62d3d7d92866c97bb513d4e603518e", null ],
      [ "kSPI_Data8Bits", "group__spi__driver.html#ggafa691c5329a7325ee57c9f06fa295731ad6b98474a7f14ae7665b7b6d3355657c", null ],
      [ "kSPI_Data9Bits", "group__spi__driver.html#ggafa691c5329a7325ee57c9f06fa295731a4b864a74545165c2cf2dd74134fdadd1", null ],
      [ "kSPI_Data10Bits", "group__spi__driver.html#ggafa691c5329a7325ee57c9f06fa295731a2b2887663c9d025a4137263fc7984024", null ],
      [ "kSPI_Data11Bits", "group__spi__driver.html#ggafa691c5329a7325ee57c9f06fa295731a91e908a7a4725ab9df05549a2b034dec", null ],
      [ "kSPI_Data12Bits", "group__spi__driver.html#ggafa691c5329a7325ee57c9f06fa295731ad1cfe18fd912c0d25dc32e6ca9fa8f97", null ],
      [ "kSPI_Data13Bits", "group__spi__driver.html#ggafa691c5329a7325ee57c9f06fa295731ad8c2fa2e2f5bf137176dc736cc6a2531", null ],
      [ "kSPI_Data14Bits", "group__spi__driver.html#ggafa691c5329a7325ee57c9f06fa295731ae83435f494d1b9f3ce2f2da0ac03fc38", null ],
      [ "kSPI_Data15Bits", "group__spi__driver.html#ggafa691c5329a7325ee57c9f06fa295731add11012a8887066fad29e23c20d66af8", null ],
      [ "kSPI_Data16Bits", "group__spi__driver.html#ggafa691c5329a7325ee57c9f06fa295731abbc2566ba4eb2b5e5dc259a7c4f80bcf", null ]
    ] ],
    [ "spi_ssel_t", "group__spi__driver.html#ga420a3d3f841957068648585b89fa66d1", [
      [ "kSPI_Ssel0", "group__spi__driver.html#gga420a3d3f841957068648585b89fa66d1aba6e24e125d6e2623fdf66179d8d3595", null ],
      [ "kSPI_Ssel1", "group__spi__driver.html#gga420a3d3f841957068648585b89fa66d1a6a919364d43781f9da7c826c53008863", null ],
      [ "kSPI_Ssel2", "group__spi__driver.html#gga420a3d3f841957068648585b89fa66d1a42cddec913f2774241d7d962143e1588", null ],
      [ "kSPI_Ssel3", "group__spi__driver.html#gga420a3d3f841957068648585b89fa66d1a242faca0999052891ca8ed24a116e466", null ]
    ] ],
    [ "spi_spol_t", "group__spi__driver.html#gafd7e7bd8e060742346806ed8c8f1db4c", null ],
    [ "_spi_status", "group__spi__driver.html#ga3fa79a6717ea4e1e74de2dadaa468edd", [
      [ "kStatus_SPI_Busy", "group__spi__driver.html#gga3fa79a6717ea4e1e74de2dadaa468edda703abdf7900047c4d13536480f3463ab", null ],
      [ "kStatus_SPI_Idle", "group__spi__driver.html#gga3fa79a6717ea4e1e74de2dadaa468edda4e32c5b06dccaf4b81e2fd1679e1b560", null ],
      [ "kStatus_SPI_Error", "group__spi__driver.html#gga3fa79a6717ea4e1e74de2dadaa468edda2ff91d774e93aed936b87ffaa18aaf9e", null ],
      [ "kStatus_SPI_BaudrateNotSupport", "group__spi__driver.html#gga3fa79a6717ea4e1e74de2dadaa468eddab0f4d4de5f92524c9d33524f22a04e03", null ]
    ] ],
    [ "_spi_interrupt_enable", "group__spi__driver.html#gaedd690a0f91a0a9eb0fd573b57e31f67", [
      [ "kSPI_RxLvlIrq", "group__spi__driver.html#ggaedd690a0f91a0a9eb0fd573b57e31f67a81a10bf611827929fcf8eed0468aea7a", null ],
      [ "kSPI_TxLvlIrq", "group__spi__driver.html#ggaedd690a0f91a0a9eb0fd573b57e31f67a13d4e9159c4ba5d49adef5e7e5f5b1dd", null ]
    ] ],
    [ "_spi_statusflags", "group__spi__driver.html#ga88b3958a041355f297213b5c407dded2", [
      [ "kSPI_TxEmptyFlag", "group__spi__driver.html#gga88b3958a041355f297213b5c407dded2a243d09d83665d0ab9deefcb8e14ea1ae", null ],
      [ "kSPI_TxNotFullFlag", "group__spi__driver.html#gga88b3958a041355f297213b5c407dded2a3892d0ae82fadd910932410fb86b0415", null ],
      [ "kSPI_RxNotEmptyFlag", "group__spi__driver.html#gga88b3958a041355f297213b5c407dded2a5dffa125a4a171d539f4d1df9abf6e8a", null ],
      [ "kSPI_RxFullFlag", "group__spi__driver.html#gga88b3958a041355f297213b5c407dded2a24187382b09195b069e487ed1e29cbcd", null ]
    ] ],
    [ "SPI_GetInstance", "group__spi__driver.html#gad823d00c394f6ca35c391dc178a25334", null ],
    [ "SPI_MasterGetDefaultConfig", "group__spi__driver.html#ga45c08fc078ae334b79fb844379140838", null ],
    [ "SPI_MasterInit", "group__spi__driver.html#gab36e8463576abeded221a1e5a1eec01c", null ],
    [ "SPI_SlaveGetDefaultConfig", "group__spi__driver.html#gac000b63ed033f57a9eee342a8c8e67f2", null ],
    [ "SPI_SlaveInit", "group__spi__driver.html#gae40850ce14ba74ea75b3eef79beecf8a", null ],
    [ "SPI_Deinit", "group__spi__driver.html#gaac0bc2b87ea2eb7eeba78d9449d4dbbf", null ],
    [ "SPI_Enable", "group__spi__driver.html#ga6d32506242b9596989efbbb2d030e997", null ],
    [ "SPI_GetStatusFlags", "group__spi__driver.html#ga9e7412ad45aeb7f620fe34559034ad2e", null ],
    [ "SPI_EnableInterrupts", "group__spi__driver.html#gab87ea500d8f74369882afe52a81d1199", null ],
    [ "SPI_DisableInterrupts", "group__spi__driver.html#ga6289b192b135592b7b0996c05422be4d", null ],
    [ "SPI_EnableTxDMA", "group__spi__driver.html#ga740b73126a29d9183e7cfef3aced8d58", null ],
    [ "SPI_EnableRxDMA", "group__spi__driver.html#ga6ac4c0b002368cf796219f5a42b8bef4", null ],
    [ "SPI_GetConfig", "group__spi__driver.html#ga51a0ccd48b439eb429540a82217481cb", null ],
    [ "SPI_MasterSetBaud", "group__spi__driver.html#ga36abdd3a2b7464d49ea52f03060a662d", null ],
    [ "SPI_WriteData", "group__spi__driver.html#gaccb3f06e4ea07f7f462965a7e6be6d9b", null ],
    [ "SPI_ReadData", "group__spi__driver.html#ga08dec9d9d6abf910589a591f1cc52f58", null ],
    [ "SPI_SetTransferDelay", "group__spi__driver.html#gadc3b8e0ffc6b6daa99e4db0bc83b25e0", null ],
    [ "SPI_SetDummyData", "group__spi__driver.html#gaa381d274dba2ce9257cdf4be5ec17662", null ],
    [ "SPI_MasterTransferCreateHandle", "group__spi__driver.html#gad0d8eaa132d8c882d8a42286ecfbab64", null ],
    [ "SPI_MasterTransferBlocking", "group__spi__driver.html#ga5b05095245ecf01258e64cc0458fb2e9", null ],
    [ "SPI_MasterTransferNonBlocking", "group__spi__driver.html#ga820c78a32fa29735168d2e517f4881a1", null ],
    [ "SPI_MasterHalfDuplexTransferBlocking", "group__spi__driver.html#ga60e534dca8be43db9a7b796862e3cf65", null ],
    [ "SPI_MasterHalfDuplexTransferNonBlocking", "group__spi__driver.html#gaf83d46b95046bb7e8adf975a077466b7", null ],
    [ "SPI_MasterTransferGetCount", "group__spi__driver.html#gabde0dccfb0783d103b9cf57e0202582f", null ],
    [ "SPI_MasterTransferAbort", "group__spi__driver.html#gae90962a54978acf05ca7780489301eb4", null ],
    [ "SPI_MasterTransferHandleIRQ", "group__spi__driver.html#ga645d041a92bde312d6cf64517c618c41", null ],
    [ "SPI_SlaveTransferCreateHandle", "group__spi__driver.html#gab81dfc439169aa1f6de97cb3803fa25a", null ],
    [ "SPI_SlaveTransferNonBlocking", "group__spi__driver.html#gae37917cb354e36acd1b1ca906d2ffe1c", null ],
    [ "SPI_SlaveTransferGetCount", "group__spi__driver.html#ga0dcd2ed8373d992ff93bb500e1442815", null ],
    [ "SPI_SlaveTransferAbort", "group__spi__driver.html#gae719048431a3e4fb334f83bd824a407b", null ],
    [ "SPI_SlaveTransferHandleIRQ", "group__spi__driver.html#gabf454f42532e8a41340efd398f9868fb", null ],
    [ "s_dummyData", "group__spi__driver.html#ga9d35e904f2864488f65fddcfb5277f44", null ]
];
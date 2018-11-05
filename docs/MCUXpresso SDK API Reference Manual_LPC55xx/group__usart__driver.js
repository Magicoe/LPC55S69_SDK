var group__usart__driver =
[
    [ "usart_config_t", "group__usart__driver.html#structusart__config__t", [
      [ "baudRate_Bps", "group__usart__driver.html#a5d2631bc772901b4114b01770f9bb337", null ],
      [ "parityMode", "group__usart__driver.html#aff1d7e368b35ae89db8aa393207e7ccc", null ],
      [ "stopBitCount", "group__usart__driver.html#aeb0b8a38f5d7e0def2aa1b079643682f", null ],
      [ "bitCountPerChar", "group__usart__driver.html#ab964b3fbce4b824beff770a138fd4b6e", null ],
      [ "loopback", "group__usart__driver.html#a9892d7a138f2245bc9b7fe4e6c1652fb", null ],
      [ "enableRx", "group__usart__driver.html#a8b9813693fdfd2116e0d6019ea39b41d", null ],
      [ "enableTx", "group__usart__driver.html#a2c3b5faf44be34a152232b9dfaaf064d", null ],
      [ "txWatermark", "group__usart__driver.html#af7968e954beeaf72f99ebd1243e6e7f5", null ],
      [ "rxWatermark", "group__usart__driver.html#a29a5d5a30658b044a09ad86373dae217", null ]
    ] ],
    [ "usart_transfer_t", "group__usart__driver.html#structusart__transfer__t", [
      [ "data", "group__usart__driver.html#acad21ced979ce0c2af98f0b1837863c9", null ],
      [ "dataSize", "group__usart__driver.html#a605c0a56df2815ffb3461aaaf116752a", null ]
    ] ],
    [ "usart_handle_t", "group__usart__driver.html#struct__usart__handle", [
      [ "txData", "group__usart__driver.html#a7d9c7451de6d684ad9b5fef4def5fd60", null ],
      [ "txDataSize", "group__usart__driver.html#aafb090dd1831c9e83df2d60cf3cd25f0", null ],
      [ "txDataSizeAll", "group__usart__driver.html#add7cbfa096ebb8bfe5084c9940d0a9d8", null ],
      [ "rxData", "group__usart__driver.html#a7fcedbe0ea6e380084a05ba1bb06e38d", null ],
      [ "rxDataSize", "group__usart__driver.html#a94a60ffa711d28e3c9151c9990a9145c", null ],
      [ "rxDataSizeAll", "group__usart__driver.html#a35109a019b73257d24021ad79d951e64", null ],
      [ "rxRingBuffer", "group__usart__driver.html#a2cac1523ac58da64d4076c6155309d2f", null ],
      [ "rxRingBufferSize", "group__usart__driver.html#ae1c5e6a8ccdd56a678e8dda316e43845", null ],
      [ "rxRingBufferHead", "group__usart__driver.html#afb838e91205d1aa1a2b064591bc11c4e", null ],
      [ "rxRingBufferTail", "group__usart__driver.html#a197ce470fd9b42e53a867ef84eb2b6dd", null ],
      [ "callback", "group__usart__driver.html#a50fd5afc23c86d872ee2a1d46bd4145e", null ],
      [ "userData", "group__usart__driver.html#adbecb8574e5d62ee38761a3b4c30e4c3", null ],
      [ "txState", "group__usart__driver.html#a30a2856c0e736ad39fe44c015bd54ca4", null ],
      [ "rxState", "group__usart__driver.html#a7f621935f46f5f1bdd10ca755e1b51a7", null ],
      [ "txWatermark", "group__usart__driver.html#aaa356523348face14f76a10a7bb74da1", null ],
      [ "rxWatermark", "group__usart__driver.html#a5929107158c0819e39234da21707dc20", null ]
    ] ],
    [ "FSL_USART_DRIVER_VERSION", "group__usart__driver.html#gad737c0eee502276b7615e83ec3a79be4", null ],
    [ "usart_transfer_callback_t", "group__usart__driver.html#ga9688f27725349ed0dd7a37c9a75eccc0", null ],
    [ "_usart_status", "group__usart__driver.html#ga71888f404b377409a54c2bc3bfb0ba8b", [
      [ "kStatus_USART_TxBusy", "group__usart__driver.html#gga71888f404b377409a54c2bc3bfb0ba8ba78157ec0658118f9205521c03da99093", null ],
      [ "kStatus_USART_RxBusy", "group__usart__driver.html#gga71888f404b377409a54c2bc3bfb0ba8ba38225397e3744ef1b4995041ce80d839", null ],
      [ "kStatus_USART_TxIdle", "group__usart__driver.html#gga71888f404b377409a54c2bc3bfb0ba8ba539a0ebaa0137fdf6d7cc9b984acb13f", null ],
      [ "kStatus_USART_RxIdle", "group__usart__driver.html#gga71888f404b377409a54c2bc3bfb0ba8ba3cb7112c821c89d7fbb1c048a86756aa", null ],
      [ "kStatus_USART_TxError", "group__usart__driver.html#gga71888f404b377409a54c2bc3bfb0ba8babf010640733ea1ae8c97d7a0b83ea11b", null ],
      [ "kStatus_USART_RxError", "group__usart__driver.html#gga71888f404b377409a54c2bc3bfb0ba8ba888280fbe6cf9b4b0647db78f0578ff5", null ],
      [ "kStatus_USART_RxRingBufferOverrun", "group__usart__driver.html#gga71888f404b377409a54c2bc3bfb0ba8ba256720c63c378f57791e946c0473290e", null ],
      [ "kStatus_USART_NoiseError", "group__usart__driver.html#gga71888f404b377409a54c2bc3bfb0ba8ba3f21596ffe8300152d7f9f5587402e19", null ],
      [ "kStatus_USART_FramingError", "group__usart__driver.html#gga71888f404b377409a54c2bc3bfb0ba8ba9e2f548075400b4c0a719ee1b0a534a9", null ],
      [ "kStatus_USART_ParityError", "group__usart__driver.html#gga71888f404b377409a54c2bc3bfb0ba8ba579872694419f8708e38935593105868", null ],
      [ "kStatus_USART_BaudrateNotSupport", "group__usart__driver.html#gga71888f404b377409a54c2bc3bfb0ba8bae427c8491ce7294dfeaa3b87e506bd81", null ]
    ] ],
    [ "usart_parity_mode_t", "group__usart__driver.html#ga9b5ca9521874092ccb637a02d7b26ba2", [
      [ "kUSART_ParityDisabled", "group__usart__driver.html#gga9b5ca9521874092ccb637a02d7b26ba2a46309b174047a84a78c77b7648bdf21b", null ],
      [ "kUSART_ParityEven", "group__usart__driver.html#gga9b5ca9521874092ccb637a02d7b26ba2aad8d786301da1bb92e2b911c386d39eb", null ],
      [ "kUSART_ParityOdd", "group__usart__driver.html#gga9b5ca9521874092ccb637a02d7b26ba2a64df3b823d2ab5f3f56ffb3f520eafb1", null ]
    ] ],
    [ "usart_stop_bit_count_t", "group__usart__driver.html#ga58ab07609b094f719f903475de6e57b4", [
      [ "kUSART_OneStopBit", "group__usart__driver.html#gga58ab07609b094f719f903475de6e57b4aa637b8f5f0edd262181a20c1fbed7c12", null ],
      [ "kUSART_TwoStopBit", "group__usart__driver.html#gga58ab07609b094f719f903475de6e57b4a3aee3195dc850778e33f2ebacf8847d3", null ]
    ] ],
    [ "usart_data_len_t", "group__usart__driver.html#ga28e46a3538cf5f5140523132a963283c", [
      [ "kUSART_7BitsPerChar", "group__usart__driver.html#gga28e46a3538cf5f5140523132a963283ca9562d6cdd240e9808a9a1d1946f400b7", null ],
      [ "kUSART_8BitsPerChar", "group__usart__driver.html#gga28e46a3538cf5f5140523132a963283cad71aee19329a4127614132d30e50ad8a", null ]
    ] ],
    [ "usart_txfifo_watermark_t", "group__usart__driver.html#ga51645e760853f2899afe701fd8bafad6", [
      [ "kUSART_TxFifo0", "group__usart__driver.html#gga51645e760853f2899afe701fd8bafad6a4c06b40c032f50878b21a2d50ada6dce", null ],
      [ "kUSART_TxFifo1", "group__usart__driver.html#gga51645e760853f2899afe701fd8bafad6a1d9f1d7d066818718be802be7d375672", null ],
      [ "kUSART_TxFifo2", "group__usart__driver.html#gga51645e760853f2899afe701fd8bafad6a4e893f1b0acc108a4438e5e0246cfd98", null ],
      [ "kUSART_TxFifo3", "group__usart__driver.html#gga51645e760853f2899afe701fd8bafad6a956a6639ce7a77854b2b8818600056d1", null ],
      [ "kUSART_TxFifo4", "group__usart__driver.html#gga51645e760853f2899afe701fd8bafad6a125165264a20e9a99ebe8809c537519d", null ],
      [ "kUSART_TxFifo5", "group__usart__driver.html#gga51645e760853f2899afe701fd8bafad6aba62dcf8bffa3fea75659cd15d85f5f5", null ],
      [ "kUSART_TxFifo6", "group__usart__driver.html#gga51645e760853f2899afe701fd8bafad6ad5789e845dda660016f5ad20d44543c5", null ],
      [ "kUSART_TxFifo7", "group__usart__driver.html#gga51645e760853f2899afe701fd8bafad6a2d5f3f3e606d0234022ba79c51006c79", null ]
    ] ],
    [ "usart_rxfifo_watermark_t", "group__usart__driver.html#gadc4d91bd718e2b3748ec626875703f15", [
      [ "kUSART_RxFifo1", "group__usart__driver.html#ggadc4d91bd718e2b3748ec626875703f15ae4c7d10afbc0733440891ebe03b16b3f", null ],
      [ "kUSART_RxFifo2", "group__usart__driver.html#ggadc4d91bd718e2b3748ec626875703f15a0234bec87111ef277cf08bf1cdacf081", null ],
      [ "kUSART_RxFifo3", "group__usart__driver.html#ggadc4d91bd718e2b3748ec626875703f15a1b8335b872c2a509fd2a623b161975cc", null ],
      [ "kUSART_RxFifo4", "group__usart__driver.html#ggadc4d91bd718e2b3748ec626875703f15a6ea188f34c9f1c2ff50b71526e42a687", null ],
      [ "kUSART_RxFifo5", "group__usart__driver.html#ggadc4d91bd718e2b3748ec626875703f15aa1868626ee2cca1ca8cc225b71c7a7bf", null ],
      [ "kUSART_RxFifo6", "group__usart__driver.html#ggadc4d91bd718e2b3748ec626875703f15a4819c5fd1c6c274e2e6f08b8aa2bad19", null ],
      [ "kUSART_RxFifo7", "group__usart__driver.html#ggadc4d91bd718e2b3748ec626875703f15a9bb99b104055213d54116ea79f0a2586", null ],
      [ "kUSART_RxFifo8", "group__usart__driver.html#ggadc4d91bd718e2b3748ec626875703f15a6e09aeee58582acc9b9baeb10315f024", null ]
    ] ],
    [ "_usart_interrupt_enable", "group__usart__driver.html#ga78b340bf3b25bfae957d0c5532b9b853", null ],
    [ "_usart_flags", "group__usart__driver.html#gaa6a81e7e82e236bb1c3c5ba306466610", [
      [ "kUSART_TxError", "group__ostimer.html#ggaa6a81e7e82e236bb1c3c5ba306466610a88f35fb71e2d5a7bea4f8eed7b6a050c", null ],
      [ "kUSART_RxError", "group__ostimer.html#ggaa6a81e7e82e236bb1c3c5ba306466610a8aa9eb127699721457f3ec9e1025246b", null ],
      [ "kUSART_TxFifoEmptyFlag", "group__ostimer.html#ggaa6a81e7e82e236bb1c3c5ba306466610af11f0420c46f24642414bb66d2e389c7", null ],
      [ "kUSART_TxFifoNotFullFlag", "group__ostimer.html#ggaa6a81e7e82e236bb1c3c5ba306466610a584ded6df831827495ab206619c81174", null ],
      [ "kUSART_RxFifoNotEmptyFlag", "group__ostimer.html#ggaa6a81e7e82e236bb1c3c5ba306466610a39952b4f24141f895bab7822dc330330", null ],
      [ "kUSART_RxFifoFullFlag", "group__ostimer.html#ggaa6a81e7e82e236bb1c3c5ba306466610a519cf0825b102a001c88bfba6506824c", null ],
      [ "kOSTIMER_MatchInterruptFlag", "group__ostimer.html#ggaa6a81e7e82e236bb1c3c5ba306466610a55d291c3a4ec78cbc55f05847146e8b4", null ]
    ] ],
    [ "USART_GetInstance", "group__usart__driver.html#ga0433386bbc6c2ac7e2ec7925fea263db", null ],
    [ "USART_Init", "group__usart__driver.html#ga2aeb4e11fdf0eb515d090865ffcf2ba2", null ],
    [ "USART_Deinit", "group__usart__driver.html#ga80892980b702b0b7614691014a5aaaca", null ],
    [ "USART_GetDefaultConfig", "group__usart__driver.html#ga74464f9357e585e42e5f3c909eb6e9b3", null ],
    [ "USART_SetBaudRate", "group__usart__driver.html#gabdcaf539e6c95903c779f4538b1f422b", null ],
    [ "USART_GetStatusFlags", "group__usart__driver.html#ga33f5cdc2918edf02b9a4ef8a12e27442", null ],
    [ "USART_ClearStatusFlags", "group__usart__driver.html#ga235a301d8f22d6ce3a6f77dbdb76cfe3", null ],
    [ "USART_EnableInterrupts", "group__usart__driver.html#ga9132cf15ff5fd2ac007a88f9400bcf30", null ],
    [ "USART_DisableInterrupts", "group__usart__driver.html#ga091d6509100a6e6206483b2f41f16d6c", null ],
    [ "USART_GetEnabledInterrupts", "group__usart__driver.html#ga49c6966b2af5f0de0fbf49240acc1856", null ],
    [ "USART_EnableTxDMA", "group__usart__driver.html#ga1bdec9d5c2cfeb180579b9bb730ff3a3", null ],
    [ "USART_EnableRxDMA", "group__usart__driver.html#ga5a227e096aa1f7e50129f2adf35bf5a4", null ],
    [ "USART_WriteByte", "group__usart__driver.html#gae7a92a20789cf111acadb543916aac91", null ],
    [ "USART_ReadByte", "group__usart__driver.html#gaaa9dc0edc5edacd55bb88ff7b9b55f98", null ],
    [ "USART_WriteBlocking", "group__usart__driver.html#ga3c3b2f0fb397c466dfecfb977e135ceb", null ],
    [ "USART_ReadBlocking", "group__usart__driver.html#ga09c3b588e2099ffa6b999c5ca7fb8d58", null ],
    [ "USART_TransferCreateHandle", "group__usart__driver.html#ga9aed876794d5c2ab2e37196242602b89", null ],
    [ "USART_TransferSendNonBlocking", "group__usart__driver.html#ga7f5a4e7e831ab0dc869637494a57362e", null ],
    [ "USART_TransferStartRingBuffer", "group__usart__driver.html#ga8500107ace3179c85e66d73fa1f41d16", null ],
    [ "USART_TransferStopRingBuffer", "group__usart__driver.html#gaedb2f678849734999e373a8b00a5248c", null ],
    [ "USART_TransferGetRxRingBufferLength", "group__usart__driver.html#gac0c8e42457c41efd02a4310423804e77", null ],
    [ "USART_TransferAbortSend", "group__usart__driver.html#ga095596e064fa8aa6bac0927e71b0329c", null ],
    [ "USART_TransferGetSendCount", "group__usart__driver.html#ga56acdf4c0046516f59c04d89e8e988e6", null ],
    [ "USART_TransferReceiveNonBlocking", "group__usart__driver.html#ga34c6c819c9e2d8608a6d7e63103fa542", null ],
    [ "USART_TransferAbortReceive", "group__usart__driver.html#ga2de9bf9557d2f7bc6fe0236608cb958e", null ],
    [ "USART_TransferGetReceiveCount", "group__usart__driver.html#ga43cc02b4521f48ae0b5626a1e8f87a0e", null ],
    [ "USART_TransferHandleIRQ", "group__usart__driver.html#gae955a5e336b3cb3276d5cdd6f1f6feda", null ]
];
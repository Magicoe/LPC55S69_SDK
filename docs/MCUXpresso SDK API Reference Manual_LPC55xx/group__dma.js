var group__dma =
[
    [ "dma_descriptor_t", "group__dma.html#structdma__descriptor__t", [
      [ "xfercfg", "group__dma.html#ad49be57eb231061b32b021a8854fe425", null ],
      [ "srcEndAddr", "group__dma.html#aabb52c29097e7d9eedb34f5421740f04", null ],
      [ "dstEndAddr", "group__dma.html#ab0a39d7a7b627b1a714117f2d150bac0", null ],
      [ "linkToNextDesc", "group__dma.html#a8b4151dcf43270fbbeff39334048e7e1", null ]
    ] ],
    [ "dma_xfercfg_t", "group__dma.html#structdma__xfercfg__t", [
      [ "valid", "group__dma.html#a4a97a76d0d7266ee9cc8de82d19e5d81", null ],
      [ "reload", "group__dma.html#a6387faccdc3dcd079d22b56b1c4806fd", null ],
      [ "swtrig", "group__dma.html#a69253cda0502dcfa038c5d71a3cac593", null ],
      [ "clrtrig", "group__dma.html#ab30c2a4b2d436b966ba948edb010688f", null ],
      [ "intA", "group__dma.html#ae2c9381d6fc00cee3491c5a8217c30a1", null ],
      [ "intB", "group__dma.html#a7827b3fe247d5d7218b0263ecbb0aede", null ],
      [ "byteWidth", "group__dma.html#a93c1b2f32e5e046cf10ba7e8b1c215ce", null ],
      [ "srcInc", "group__dma.html#ab21f9a2d11b2ce02da230adfd32b789e", null ],
      [ "dstInc", "group__dma.html#a674be34352d78e2029f8b36f0664cf6f", null ],
      [ "transferCount", "group__dma.html#a11b16354c24a04222507d6508a475bab", null ]
    ] ],
    [ "dma_channel_trigger_t", "group__dma.html#structdma__channel__trigger__t", [
      [ "type", "group__dma.html#a80afb7ab2642836025d063ac8b0c9738", null ],
      [ "burst", "group__dma.html#a66ad3f0d6159adbb560799bfb93da76a", null ],
      [ "wrap", "group__dma.html#acbcaa81a9d2806d3fa021b5ad27fea6f", null ]
    ] ],
    [ "dma_transfer_config_t", "group__dma.html#structdma__transfer__config__t", [
      [ "srcAddr", "group__dma.html#a2a315d0141311dd82dd6ac1a3523671b", null ],
      [ "dstAddr", "group__dma.html#a8eb88e8dcaea9033a76c0003fcabadd8", null ],
      [ "nextDesc", "group__dma.html#a24f716a2b7775c1cb9a59b7c2374508b", null ],
      [ "xfercfg", "group__dma.html#a365dbf9376f6927bc8b6527ce136914c", null ],
      [ "isPeriph", "group__dma.html#a3d7f9ccb3edc3a6b1dbf2feb5a8b8f93", null ]
    ] ],
    [ "dma_handle_t", "group__dma.html#structdma__handle__t", [
      [ "callback", "group__dma.html#a47a5c6af4c934cc9db355d394bb94f46", null ],
      [ "userData", "group__dma.html#a2a10a0701fc2085ce58a0e35032e8a8e", null ],
      [ "base", "group__dma.html#a84d667acc1301d6d56ae52573e4b6b87", null ],
      [ "channel", "group__dma.html#a7c068d330cc60423ee3fd86821221b85", null ]
    ] ],
    [ "FSL_DMA_DRIVER_VERSION", "group__dma.html#gac68c8082b53756a7e58ec6d5f25117d2", null ],
    [ "dma_callback", "group__dma.html#gab844237884d5badd07ac902a9be34275", null ],
    [ "dma_priority_t", "group__dma.html#ga63f28310491d665df0ad9a99dc22a77f", [
      [ "kDMA_ChannelPriority0", "group__dma.html#gga63f28310491d665df0ad9a99dc22a77fafaf9bf976bb9d2db3bd2f629cd545f45", null ],
      [ "kDMA_ChannelPriority1", "group__dma.html#gga63f28310491d665df0ad9a99dc22a77fa6ff9161839b0b5e675b01353a684a63f", null ],
      [ "kDMA_ChannelPriority2", "group__dma.html#gga63f28310491d665df0ad9a99dc22a77fa259e4e6cf7b6b79608abbe1769e262fe", null ],
      [ "kDMA_ChannelPriority3", "group__dma.html#gga63f28310491d665df0ad9a99dc22a77fa8615d84237aa24c16fdc1675cc932efb", null ],
      [ "kDMA_ChannelPriority4", "group__dma.html#gga63f28310491d665df0ad9a99dc22a77fae7f6c72a518bc4c68b3be696da1a872d", null ],
      [ "kDMA_ChannelPriority5", "group__dma.html#gga63f28310491d665df0ad9a99dc22a77fa08b83d1381be3c10688f449c9bf90fc4", null ],
      [ "kDMA_ChannelPriority6", "group__dma.html#gga63f28310491d665df0ad9a99dc22a77facc5b892b2bcf8a632acb64e9c5645273", null ],
      [ "kDMA_ChannelPriority7", "group__dma.html#gga63f28310491d665df0ad9a99dc22a77faf238cfde678fdbd26a054fc055f40a4d", null ]
    ] ],
    [ "dma_irq_t", "group__dma.html#ga5658ee3bb7fbf1da24d997d9bc341e11", [
      [ "kDMA_IntA", "group__dma.html#gga5658ee3bb7fbf1da24d997d9bc341e11ab39861058eb1c9573babd0752286ea48", null ],
      [ "kDMA_IntB", "group__dma.html#gga5658ee3bb7fbf1da24d997d9bc341e11a50cabcc60fb8dc805c62070318fe0c97", null ],
      [ "kDMA_IntError", "group__dma.html#gga5658ee3bb7fbf1da24d997d9bc341e11a28e17f00c59b3dde88943be07a8902bd", null ]
    ] ],
    [ "dma_trigger_type_t", "group__dma.html#ga0468cf171e413581e9bba9803df91427", [
      [ "kDMA_NoTrigger", "group__dma.html#gga0468cf171e413581e9bba9803df91427ae45693316c3b50f21a0fa93e539e9ae5", null ],
      [ "kDMA_LowLevelTrigger", "group__dma.html#gga0468cf171e413581e9bba9803df91427aca6b545d2f8661404c932a70b8a5abcf", null ],
      [ "kDMA_HighLevelTrigger", "group__dma.html#gga0468cf171e413581e9bba9803df91427a4636793fc071ed9f8e753e84a6e12f85", null ],
      [ "kDMA_FallingEdgeTrigger", "group__dma.html#gga0468cf171e413581e9bba9803df91427a9b9f27849a8e3cf715e30a29b4ba1d04", null ],
      [ "kDMA_RisingEdgeTrigger", "group__dma.html#gga0468cf171e413581e9bba9803df91427aa4d1b3c95a9ca3d756d2f68a3bb0afc8", null ]
    ] ],
    [ "dma_trigger_burst_t", "group__dma.html#ga776b1091528ddc2571284f481ddde830", [
      [ "kDMA_SingleTransfer", "group__dma.html#gga776b1091528ddc2571284f481ddde830aa5f7edec7506a38b3b57d04dd774b0b0", null ],
      [ "kDMA_LevelBurstTransfer", "group__dma.html#gga776b1091528ddc2571284f481ddde830aa6cc38822f852f1ddd1f22c5b79949b8", null ],
      [ "kDMA_EdgeBurstTransfer1", "group__dma.html#gga776b1091528ddc2571284f481ddde830af06ebd49d4066394642c80ece815d55b", null ],
      [ "kDMA_EdgeBurstTransfer2", "group__dma.html#gga776b1091528ddc2571284f481ddde830af0ab73a40234a51b14203e7d83621ac9", null ],
      [ "kDMA_EdgeBurstTransfer4", "group__dma.html#gga776b1091528ddc2571284f481ddde830a42eb0fe4fd6b2030e64b5d0173c93c68", null ],
      [ "kDMA_EdgeBurstTransfer8", "group__dma.html#gga776b1091528ddc2571284f481ddde830a4858703ddbc176a60c3015f23a490e17", null ],
      [ "kDMA_EdgeBurstTransfer16", "group__dma.html#gga776b1091528ddc2571284f481ddde830a0641c94c5e494890f587f3cb60ed7800", null ],
      [ "kDMA_EdgeBurstTransfer32", "group__dma.html#gga776b1091528ddc2571284f481ddde830af1764eeb6c94e0e1f86529bf6210c524", null ],
      [ "kDMA_EdgeBurstTransfer64", "group__dma.html#gga776b1091528ddc2571284f481ddde830a8dec3f95cf71d668439e065b17f27235", null ],
      [ "kDMA_EdgeBurstTransfer128", "group__dma.html#gga776b1091528ddc2571284f481ddde830af2c60c5f0dcbd8bd7d782a7c08c87f0b", null ],
      [ "kDMA_EdgeBurstTransfer256", "group__dma.html#gga776b1091528ddc2571284f481ddde830a8640003c589e5d3ffd94d4cd962b8456", null ],
      [ "kDMA_EdgeBurstTransfer512", "group__dma.html#gga776b1091528ddc2571284f481ddde830a0ef881d258e59978faca416630fb1a74", null ],
      [ "kDMA_EdgeBurstTransfer1024", "group__dma.html#gga776b1091528ddc2571284f481ddde830a6aae117efdce18b2c054ddae971559da", null ]
    ] ],
    [ "dma_burst_wrap_t", "group__dma.html#gacea88ecaac2447ba3c9f2157e40b9a82", [
      [ "kDMA_NoWrap", "group__dma.html#ggacea88ecaac2447ba3c9f2157e40b9a82a99b1a9eb98902b9277c2ae64f48e5a88", null ],
      [ "kDMA_SrcWrap", "group__dma.html#ggacea88ecaac2447ba3c9f2157e40b9a82ac0ab79249ad0e6bf887498d0f3e3db90", null ],
      [ "kDMA_DstWrap", "group__dma.html#ggacea88ecaac2447ba3c9f2157e40b9a82afaae8afb83546031e25b0ac5e6494f2b", null ],
      [ "kDMA_SrcAndDstWrap", "group__dma.html#ggacea88ecaac2447ba3c9f2157e40b9a82a543352fa3896623517af17829598eb0c", null ]
    ] ],
    [ "dma_transfer_type_t", "group__dma.html#ga9cb7087af6efc80106c1033f80d60219", [
      [ "kDMA_MemoryToMemory", "group__dma.html#gga9cb7087af6efc80106c1033f80d60219a24392a93deeb55e04559201839343f3c", null ],
      [ "kDMA_PeripheralToMemory", "group__dma.html#gga9cb7087af6efc80106c1033f80d60219a7bf1938ae68b1f494aade7a0f4189303", null ],
      [ "kDMA_MemoryToPeripheral", "group__dma.html#gga9cb7087af6efc80106c1033f80d60219aae1c6bab576d7a25d2fa1249f9cd4ee9", null ],
      [ "kDMA_StaticToStatic", "group__dma.html#gga9cb7087af6efc80106c1033f80d60219af59909afb24e786824cfecdb6afd0da8", null ]
    ] ],
    [ "_dma_transfer_status", "group__dma.html#ga63c0c8f218fd21a01a4996032b1f6ee0", [
      [ "kStatus_DMA_Busy", "group__dma.html#gga63c0c8f218fd21a01a4996032b1f6ee0a9b95dc670083f57c33f96b0c10d0c8ba", null ]
    ] ],
    [ "DMA_Init", "group__dma.html#gade1b5efa61054ce538b37b181dd075bb", null ],
    [ "DMA_Deinit", "group__dma.html#ga634ced9b86d7dc9543e0b4387123fcac", null ],
    [ "DMA_ChannelIsActive", "group__dma.html#ga0e6279e44ebd6778355f6e9e26e9a8b6", null ],
    [ "DMA_EnableChannelInterrupts", "group__dma.html#ga02671643755a7f9395ce1f151a914630", null ],
    [ "DMA_DisableChannelInterrupts", "group__dma.html#gac9873a0205538dffcba7bb59ed1450c5", null ],
    [ "DMA_EnableChannel", "group__dma.html#gaa0293771d55ee1a7c42a8ffecb0728e7", null ],
    [ "DMA_DisableChannel", "group__dma.html#ga8025eebc80c23fa9b5a3a4454855e347", null ],
    [ "DMA_EnableChannelPeriphRq", "group__dma.html#gaa706c05b4875366e7f84c83b8d09ae29", null ],
    [ "DMA_DisableChannelPeriphRq", "group__dma.html#gab2d2f88ed16097b2974d1955c725a5dc", null ],
    [ "DMA_ConfigureChannelTrigger", "group__dma.html#ga04d2004d7d054b4f36676304bc2cd8ad", null ],
    [ "DMA_GetRemainingBytes", "group__dma.html#gaf0197c674ce188bf214c13bc7be43e14", null ],
    [ "DMA_SetChannelPriority", "group__dma.html#gad8ae76fc5d858b97de3362100af86df6", null ],
    [ "DMA_GetChannelPriority", "group__dma.html#ga1a5311b6ad708e230db517fb04f98a5f", null ],
    [ "DMA_CreateDescriptor", "group__dma.html#gaf2955b2c961a33fe24d3154ec801f9fd", null ],
    [ "DMA_AbortTransfer", "group__dma.html#ga3c61d6121d88d2cdf287fc1cd72912fd", null ],
    [ "DMA_CreateHandle", "group__dma.html#ga5354ff2c0c1ec53e2cbd712169fc5558", null ],
    [ "DMA_SetCallback", "group__dma.html#ga2eb9a831b9a84c5108097f770dbe90db", null ],
    [ "DMA_PrepareTransfer", "group__dma.html#ga01ed2edd4ac04c70f83c7c3091ec4b65", null ],
    [ "DMA_SubmitTransfer", "group__dma.html#gade169c9d37d6a2ff949546268ba1c382", null ],
    [ "DMA_StartTransfer", "group__dma.html#gac6fb3f30e13c937cc00fe11218e6ec76", null ],
    [ "DMA_HandleIRQ", "group__dma.html#ga894a184da7122e1e400f05515259093e", null ]
];
/*
* The Clear BSD License
* Copyright (c) 2008-2016, Freescale Semiconductor, Inc.
* Copyright 2016-2018 NXP
* All rights reserved.
*
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted (subject to the limitations in the disclaimer below) provided
*  that the following conditions are met:
*
* o Redistributions of source code must retain the above copyright notice, this list
*   of conditions and the following disclaimer.
*
* o Redistributions in binary form must reproduce the above copyright notice, this
*   list of conditions and the following disclaimer in the documentation and/or
*   other materials provided with the distribution.
*
* o Neither the name of the copyright holder nor the names of its
*   contributors may be used to endorse or promote products derived from this
*   software without specific prior written permission.
*
* NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _AUTHIMGGEN_
#define _AUTHIMGGEN_

#include <string>
#include <vector>
#include "mbedtls/sha256.h"
#define MBEDTLS_FS_IO
#include "mbedtls/x509_crt.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/gcm.h"
#include "elftosb.h"

using namespace std;

/*comming from skboot_image.h*/
/*! @brief Offset to Image Length and Image Link Address info */
#define RT6XX "rt6xx"
#define LPC55XX "lpc55xx"
#define SKBOOT_IMG_HDR_OFFSET(offset) ((offset & 0x0000FFFFu) << 16)

/* bits to be combined with image type */
#define SKBOOT_IMG_HDR_KEY_STORE_INCLUDED_MASK (0x00008000u)
#define SKBOOT_IMG_HDR_TZM_IMAGE_TYPE_MASK (0x00004000u) /* TZM image type */
#define SKBOOT_IMG_HDR_TZM_PRESET_MASK (0x00002000u)     /* TZM preset */

	typedef enum _skboot_image_type
	{
		kSKBOOT_ImageTypeInvalid = 0x0,

		/* Image types with image header compatible with ARM-v7m and ARM-v8m vector table */
		kSKBOOT_ImageTypePlainSigned = SKBOOT_IMG_HDR_OFFSET(0) | 0x1u,
		kSKBOOT_ImageTypePlainCrc = SKBOOT_IMG_HDR_OFFSET(0) | 0x2u,
		kSKBOOT_ImageTypeEncryptedSigned = SKBOOT_IMG_HDR_OFFSET(0) | 0x3u,
		kSKBOOT_ImageTypeXipPlainSigned = SKBOOT_IMG_HDR_OFFSET(0) | 0x4u,
		kSKBOOT_ImageTypeXipPlainCrc = SKBOOT_IMG_HDR_OFFSET(0) | 0x5u,
		kSKBOOT_ImageTypeEncryptedSignedKeyStoreIncluded = kSKBOOT_ImageTypeEncryptedSigned | SKBOOT_IMG_HDR_KEY_STORE_INCLUDED_MASK,
		kSKBOOT_ImageTypePlainSignedKeyStoreIncluded = kSKBOOT_ImageTypePlainSigned | SKBOOT_IMG_HDR_KEY_STORE_INCLUDED_MASK,

		/* Note this concept allows for future image types like
		kSKBOOT_ImageTypePlainSigned_0x400 = SKBOOT_IMG_HDR_OFFSET(0x400) | 0x1u,
		in case if we need to move Image Length and Image Link Address fields to some fixed offset, because some
		future version of ARM-vX might make use of those reserved fields in ARM-v7m and ARM-v8m vector table
		*/

	} skboot_image_type_t;

	/*comming from authentication.h*/

//! @brief Build a 32-bit code from four character values.
//!
//! The resulting value is built with a byte order such that the string
//! being readable in expected order when viewed in a hex editor, if the value
//! is treated as a 32-bit little endian value.
#define FOUR_CHAR_CODE(a, b, c, d) (((d) << 24) | ((c) << 16) | ((b) << 8) | ((a)))


//! @brief Define the length of SHA256 hash.
#define SHA256_HASH_LENGTH_IN_BYTES (32)

//! @brief Tag constants for the certificate block header.
#define SIGNATURE_TAG FOUR_CHAR_CODE('c', 'e', 'r', 't')

//! @brief Define the format related constants.
	enum _bootloader_signed_image_format_constants
	{
		kCertBlockHeaderOffsetAddress = 0x28,   //!< The address of the certificate block header offset.
		kCertBlockHeaderOffsetMinValue = 0x40,  //!< The min value of the certificate block header offset.
		kCertMaxSupportedSizeInBytes = 4096,    //!< The maximum size of certificate, supported by bootloader.
		kRootPublicKeyHashTableMaxEntries = 4,  //!< The maxinum count of RK table entries.
		kImageSessionsAlignmentSizeInBytes = 4, //!< The alignment bytes request by signed image format.
	};

	/*! @brief root public key hash entry definition. */
	//size 32 bytes
	typedef struct _rkh_entry
	{
		uint8_t rkh[SHA256_HASH_LENGTH_IN_BYTES];
	} rkh_entry_t;

	/*! @brief root public key hash table definition. */
	//size 128bytes
	typedef struct _rkh_table
	{
		rkh_entry_t entries[kRootPublicKeyHashTableMaxEntries];
	} rkh_table_t;

	/*! @brief signature block header definition. */
	typedef struct _certificate_block_header
	{
		uint32_t signature;           //!< Always set to 'cert'.
		uint16_t headerMajorVersion;  //!< Major Version.
		uint16_t headerMinorVersion;  //!< Minor Version.
		uint32_t headerLengthInBytes; //!< Number of bytes long the header is, starting from the signature. Does not include
									  //!< the certificate table.
		uint32_t flags;               //!< Reserved for future use.
		uint32_t buildNumber; //!< User specified build number for the signed image. Allows user to prevent reverting to old
							  //! versions.
		uint32_t totalImageLengthInBytes; //!< Length in bytes of the signed data. Does not include RSASSA-PKCS1-v1_5 Signature
		uint32_t certificateCount;   //!< Certificate number in the certificate table. Must be greater than 0.
		uint32_t certificateTableLengthInBytes; //!< Certificate table length.
	} certificate_block_header_t;

	/*comming from key_store.h*/
	//56 bytes
	typedef struct _key_item
	{
		uint32_t type; //4
		union _key_code {
			uint32_t w[52 / sizeof(uint32_t)]; //52
			uint8_t b[52];
		} keyCode;
	} skboot_key_code_item;

	//1536 bytes
	typedef struct _key_store_lpc55xx
	{
		uint32_t header;//4
		uint32_t pufDischargeTime_ms;//4
		union _activation_code {
			uint32_t w[1192 / sizeof(uint32_t)]; //1192
			uint8_t b[1192];
		} activationCode;

		skboot_key_code_item sbkek; //336
		skboot_key_code_item userkek;
		skboot_key_code_item uds;
		skboot_key_code_item encryptedRegion0;
		skboot_key_code_item encryptedRegion1;
		skboot_key_code_item encryptedRegion2;
	} skboot_key_store_lpc55xx_t;
	//1424 bytes
	typedef struct _key_store_lpc68xx
	{
		uint32_t header;
		uint32_t pufDischargeTime_ms;
		union _activation_code {
			uint32_t w[1192 / sizeof(uint32_t)];
			uint8_t b[1192];
		} activationCode;

		skboot_key_code_item sbkek;
		skboot_key_code_item userkek;
		skboot_key_code_item uds;
		skboot_key_code_item otfadkek;
	} skboot_key_store_lpc68xx_t;

	union keyStore_t {
		_key_store_lpc68xx lpc68xx;
		_key_store_lpc55xx lpc55xx;
	};

	//size 56 bytes
	typedef struct _es_header
	{
		uint8_t gmac[16];
		uint8_t iv_gmac[12];
		uint8_t reserved_00[4];
		uint32_t imageLength;
		uint32_t imageType;
		uint32_t offsetToCertHeader;
		uint8_t reserved_01[8];
		uint32_t imageLinkAddress;
	} es_header_t;
	//size 32 bytes
	typedef struct _headerMac
	{
		uint8_t b[32];
	} _headerMac_t;

	/*!
	* \brief Generator of Master Boot images for lpc55xx and rt6xx.
	*
	* Takes the JSON image configuration and creating bin file.
	*/
	class AuthImageGenerator {
	public:
		AuthImageGenerator();
		//! \brief Starting point of Master Boot image generation.
		void execute(const string &confFilePath, const chip_family_t family);
		struct configuration {
			string family;
			string imageFile;
			string imageType;
			string imageEncryptionKeyFile;
			uint32_t imageLinkAddress;
			bool imageLinkAddressFromImage;
			bool trustZone;
			string trustZonePresetFile;
			bool generateTrustZone;
			bool useKeyStore;
			string keyStoreFile;
			vector<string> rootCertFiles;
			vector<string> chainCertFiles;
			uint8_t mainCertId;
			string mainCertPrivateKeyFile;
			string masterBootOutputFile;
		};
	private:
		//! \brief Parrent function to create image.
		bool createImage(const configuration &conf, const chip_family_t family);
		//! \brief Parrent function to parse json image configuration.
		bool parseConfiguration(const std::string &confFilePath, configuration &conf, const chip_family_t family);
		//! \brief Parsing and validating input JSON configuration file for lpc55xx/rt6xx.
		bool parseConfigurationLpcSboot(const std::string &confFilePath, configuration &conf);
		//! \brief Procesing and creating image according provided configuration for lpc55xx/rt6xx.
		int createImageLpcSboot(const configuration &conf);
		//! \brief Freeing reserved resources in createImage function for lpc55xx/rt6xx.
		void cleanUpLpcSboot(uint8_t *userKey, uint8_t * keyStore, FILE * imageFile, FILE * usedRootCrtFile, FILE * outputFile, FILE * trustZonePresetFile, uint8_t * buffer, uint8_t * signature, mbedtls_x509_crt * trustCrt, mbedtls_rsa_context * rsa_priv_key_ctx);
		//! \brief Parsing and validating input JSON configuration file for k3s.
		bool parseConfigurationK3Sboot(const std::string &confFilePath, configuration &conf);
		//! \brief Procesing and creating image according provided configuration for K3S.
		int createImageK3Sboot(const configuration &conf);
		//! \brief Freeing reserved resources in createImage function for k3s.
		void cleanUpK3Sboot(FILE * imageFile, FILE * usedRootCrtFile, FILE * outputFile, uint8_t * buffer, uint8_t * signature, mbedtls_x509_crt * trustCrt, mbedtls_rsa_context * rsa_priv_key_ctx);
	};
#endif /* _AUTHIMGGEN_ */
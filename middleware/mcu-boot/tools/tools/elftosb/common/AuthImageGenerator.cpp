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

#include <stdint.h>
#include <stdio.h>
#include <algorithm>
#include <sstream>

#include "AESKey.h"
#include "crc.h"
#include "json.h"
#include "TrustZonePresetGenerator.h"
#include "Logging.h"

#include "AuthImageGenerator.h"

#define FAIL_RETURN false;
#define SUCCESS_RETURN true;
#define MAX_PATH 150
#define AUTH_IMAGE_ALIGNMENT 4
#define ES_IMG_TAIL_BYTELEN (sizeof(es_header_t) + sizeof(ctrInitVector))
#define MAX_ROOT_CERT_COUNT 4
#define HMACHEADEROFSET 64

AuthImageGenerator::AuthImageGenerator() {}

void AuthImageGenerator::execute(const string &confFilePath, const chip_family_t family) {
	configuration conf{};
	if (parseConfiguration(confFilePath, conf, family)) {
		if (!createImage(conf, family))
			throw runtime_error("Creation of authenticated image file failed. Please see the previous output to identify possible problem.");
	}
	else
		throw runtime_error(("Parsing of configuration file " + confFilePath + " failed. Please see the previous output to identify possible problem.\nTIP: Use json validator to help with correction json formating problems if there are any.\n").c_str());
}

bool AuthImageGenerator::parseConfiguration(const string &confFilePath, configuration &conf, const chip_family_t family) {
	if (family == kLPC_skbootFamily)
		return parseConfigurationLpcSboot(confFilePath, conf);
	else if (family == kKinetisK3Family)
		return parseConfigurationK3Sboot(confFilePath, conf);
	else
		throw runtime_error("Not supported device family for parsing master boot image json configuration.");
}

bool AuthImageGenerator::createImage(const configuration &conf, const chip_family_t family) {
	if (family == kLPC_skbootFamily)
		return createImageLpcSboot(conf);
	else if (family == kKinetisK3Family)
		return createImageK3Sboot(conf);
	else
		throw runtime_error("Not supported device family for creating master boot image file.");
}

bool AuthImageGenerator::parseConfigurationLpcSboot(const string &confFilePath, configuration &conf)
{
	string tmp;
	auto error = false;
	stringstream ss;
	ss << "Parsing configuration file: " << confFilePath << ".\n";
	Log::log(Logger::INFO, ss.str());
	ss.str("");
	jute::jValue jsonConf;

	try {
		jsonConf = jute::parser::parse_file(confFilePath);
	}
	catch (runtime_error &e) {
		throw runtime_error(("Cannot parse json configuration file: " + confFilePath + " - " + e.what()).c_str());
	}

	if (jsonConf["family"].get_type() == jute::JSTRING) {
		conf.family = jsonConf["family"].as_string();
		transform(conf.family.begin(), conf.family.end(), conf.family.begin(), ::tolower);
		if (conf.family != LPC55XX && conf.family != RT6XX) {
			ss << "\tUnexpected \"family\" value (" << conf.family << ") from configuration file: " << confFilePath << ".\n";
			ss << "\tSupported only " << LPC55XX << " and " << RT6XX << "." << endl;
			Log::log(Logger::ERROR, ss.str());
			ss.str("");
			error = true;
		}
	}
	else {
		ss << "\tCannot read \"family\" from configuration file: " << confFilePath << ".\n";
		Log::log(Logger::ERROR, ss.str());
		ss.str("");
		error = true;
	}

	if (jsonConf["inputImageFile"].get_type() == jute::JSTRING)
		conf.imageFile = jsonConf["inputImageFile"].as_string();
	else {
		ss << "\tCannot read \"inputImageFile\" from configuration file: " << confFilePath << ".\n";
		Log::log(Logger::ERROR, ss.str());
		ss.str("");
		error = true;
	}

	if (jsonConf["outputImageExecutionTarget"].get_type() == jute::JSTRING) {
		string backup = tmp = jsonConf["outputImageExecutionTarget"].as_string();
		transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
		tmp.erase(remove_if(tmp.begin(), tmp.end(), [](char ch) { return isspace(static_cast<unsigned char>(ch)); }), tmp.end());
		if (conf.family == LPC55XX && tmp != "internalflash(xip)") {
			ss << "\tUnsupported value (" << backup << ") of \"outputImageAuthenticationType\" for selcted family " << LPC55XX <<  " from configuration file: " << confFilePath << ".\n";
			ss << "\tExpected values [\"Internal flash (XIP)\"] for " << LPC55XX << " family.\n";
			Log::log(Logger::ERROR, ss.str());
			ss.str("");
			error = true;
		}
		else if (conf.family == RT6XX && tmp != "externalflash(xip)" && tmp != "ram") {
			ss << "\tUnsupported value (" << backup << ") of \"outputImageAuthenticationType\" for selcted family " << RT6XX << " from configuration file: " << confFilePath << ".\n";
			ss << "\tExpected values [\"External flash (XIP)\", \"RAM\"]  for " << RT6XX << " family.\n";
			Log::log(Logger::ERROR, ss.str());
			ss.str("");
			error = true;
		}
		if (tmp == "internalflash(xip)" || tmp == "externalflash(xip)")
			tmp = "xip";
		conf.imageType = tmp;
	}
	else {
		ss << "\tCannot read \"outputImageExecutionTarget\" from configuration file: " << confFilePath << ".\n";
		Log::log(Logger::ERROR, ss.str());
		ss.str("");
		error = true;
	}

	if (jsonConf["outputImageAuthenticationType"].get_type() == jute::JSTRING) {
		string backup = tmp = jsonConf["outputImageAuthenticationType"].as_string();
		transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
		tmp.erase(remove_if(tmp.begin(), tmp.end(), [](char ch) { return isspace(static_cast<unsigned char>(ch)); }), tmp.end());
		if (conf.family == LPC55XX && !(tmp == "crc" || tmp == "signed")) {
			ss << "\tUnsupported value (" << tmp << ") of \"outputImageAuthenticationType\" for selected family " << LPC55XX << " from configuration file: " << confFilePath << ".\n";
			ss << "\tExpected values [\"CRC\", \"Signed\"] " << LPC55XX << " family with \"outputImageExecutionTarget\" set to \"Internal flash(XIP)\".\n";
			Log::log(Logger::ERROR, ss.str());
			ss.str("");
			error = true;
		}
		else if (conf.family == RT6XX && conf.imageType == "xip" && !(tmp == "crc" || tmp == "signed")) {
			ss << "\tUnsupported value (" << tmp << ") of \"outputImageAuthenticationType\" for selected family " << RT6XX << " from configuration file: " << confFilePath << ".\n";
			ss << "\tExpected values [\"CRC\", \"Signed\"] for " << RT6XX << " family with \"outputImageExecutionTarget\" set to \"External flash(XIP)\".\n";
			Log::log(Logger::ERROR, ss.str());
			ss.str("");
			error = true;
		}
		else if (conf.family == RT6XX && conf.imageType == "ram" && !(tmp == "crc" || tmp == "signed" || tmp == "encrypted+signed")) {
			ss << "\tUnsupported value (" << tmp << ") of \"outputImageAuthenticationType\" for selected family " << RT6XX << " from configuration file: " << confFilePath << ".\n";
			ss << "\tExpected values [\"CRC\", \"Signed\", \"Encryped + Signed\"] for " << RT6XX << " family with \"outputImageExecutionTarget\" set to \"RAM\".\n";
			Log::log(Logger::ERROR, ss.str());
			ss.str("");
			error = true;
		}
		conf.imageType += tmp;
	}
	else {
		ss << "\tCannot read \"outputImageAuthenticationType\" from configuration file: " << confFilePath << ".\n";
		Log::log(Logger::ERROR, ss.str());
		ss.str("");
		error = true;
	}

	if (conf.imageType == "ramsigned" || conf.imageType == "ramencrypted+signed") {
		if (jsonConf["useKeyStore"].get_type() == jute::JBOOLEAN)
			conf.useKeyStore = jsonConf["useKeyStore"].as_bool();
		else {
			ss << "\tCannot read \"useKeyStore\" from configuration file: " << confFilePath << ".\n\tKey store disable by default.\n";
			Log::log(Logger::INFO, ss.str());
			ss.str("");
		}

		if (conf.useKeyStore) {
			if (jsonConf["keyStoreFile"].get_type() == jute::JSTRING)
				conf.keyStoreFile = jsonConf["keyStoreFile"].as_string();
			else {
				ss << "\tCannot read \"keyStoreFile\" from configuration file: " << confFilePath << ".\n\tKey store will be only reserved.\n";
				Log::log(Logger::INFO, ss.str());
				ss.str("");
			}
		}
	}

	if (conf.imageType == "ramsigned" || conf.imageType == "ramencrypted+signed" || conf.imageType == "xipsigned" ) {		
		if (conf.imageType != "xipsigned") {
			if (jsonConf["outputImageEncryptionKeyFile"].get_type() == jute::JSTRING)
				conf.imageEncryptionKeyFile = jsonConf["outputImageEncryptionKeyFile"].as_string();
			else {
				ss << "\tCannot read \"outputImageEncryptionKeyFile\" from configuration file: " << confFilePath << ".\n";
				Log::log(Logger::ERROR, ss.str());
				ss.str("");
				error = true;
			}
		}

		for (int i = 0; i < MAX_ROOT_CERT_COUNT; i++) {
			tmp = "rootCertificate" + to_string(i) + "File";
			if (jsonConf[tmp].get_type() == jute::JSTRING)
				conf.rootCertFiles.push_back(string(jsonConf[tmp].as_string()));
			else {
				ss << "\tCannot read \"" << tmp << "\" from configuration file: " << confFilePath << ".\n\tSkiping " << tmp << ".\n";
				Log::log(Logger::INFO, ss.str());
				ss.str("");
				conf.rootCertFiles.push_back(string(""));
			}
		}

		if (jsonConf["mainCertChainId"].get_type() == jute::JNUMBER) {
			conf.mainCertId = jsonConf["mainCertChainId"].as_int();
			if (conf.mainCertId > MAX_ROOT_CERT_COUNT - 1) {
				ss << "\tWrong value of \"mainCertChainId\" loaded from configuration file: " << confFilePath << ".\n";
				ss << "\tLoaded: " << conf.mainCertId << " value should be from 0  to " << MAX_ROOT_CERT_COUNT - 1 << ", using 0 as default.\n";
				Log::log(Logger::INFO, ss.str());
				ss.str("");
				conf.mainCertId = 0;
			}
		}
		else {
			ss << "\tCannot read \"mainCertChainId\" from configuration file: " << confFilePath << ".\n\tUsing 0 as default.\n";
			Log::log(Logger::INFO, ss.str());
			ss.str("");
		}

		if (conf.rootCertFiles[conf.mainCertId] == "") {
			ss << "\tSelected main root certificate \"mainCertChainId\" (" << (int)conf.mainCertId << ") from configuration file: " << confFilePath <<
				" don't have set corresponding value for \"rootCertificate" << (int)conf.mainCertId << "\".\n";
			Log::log(Logger::ERROR, ss.str());
			ss.str("");
			error = true;
		}

		short i = 0;
		tmp = "chainCertificate" + to_string(conf.mainCertId) + "File";
		while (jsonConf[tmp + to_string(i)].get_type() == jute::JSTRING) {
			conf.chainCertFiles.push_back(string(jsonConf[tmp + to_string(i)].as_string()));
			i++;
		}

		ss << "\tMain certificate chain " << (int)conf.mainCertId << " with " << i + 1 << " certificate(s) was fetched from configuration file: " << confFilePath << ".\n";
		Log::log(Logger::DEBUG, ss.str());
		ss.str("");

		if (jsonConf["mainCertPrivateKeyFile"].get_type() == jute::JSTRING)
			conf.mainCertPrivateKeyFile = jsonConf["mainCertPrivateKeyFile"].as_string();
		else {
			ss << "\tCannot read \"mainCertPrivateKeyFile\" from configuration file: " << confFilePath << ".\n";
			Log::log(Logger::ERROR, ss.str());
			ss.str("");
			error = true;
		}
	}

	if (jsonConf["imageLinkAdressFromImage"].get_type() == jute::JBOOLEAN)
		conf.imageLinkAddressFromImage = jsonConf["imageLinkAdressFromImage"].as_bool();
	else {
		ss << "\tCannot read \"imageLinkAdressFromImage\" from configuration file: " << confFilePath << ".\n\tDisabled by default and image load address fetched from \"imageLinkAddress\".\n";
		Log::log(Logger::INFO, ss.str());
		ss.str("");
		conf.imageLinkAddressFromImage = false;
	}

	if(!conf.imageLinkAddressFromImage){
		if (jsonConf["imageLinkAddress"].get_type() != jute::JUNKNOWN) {
			string tmp(jsonConf["imageLinkAddress"].as_string());
			try {
				conf.imageLinkAddress = stoul(tmp, nullptr, 16);
			}
			catch (invalid_argument) {
				ss << "\tCannot parse \"imageLinkAddress\" value: " << jsonConf["imageLinkAddress"].as_string() << " from configuration file: " << confFilePath << ".\n";
				ss << "\tExpected hexadecimal string in format \"0xHEXVALUE\" or only \"HEXVALUE\".\n";
				Log::log(Logger::ERROR, ss.str());
				ss.str("");
				error = true;
			}
			catch (out_of_range) {
				ss << "\tValue of \"imageLinkAddress\" is too big: " << jsonConf["imageLinkAddress"].as_string() << " from configuration file: " << confFilePath << ".\n";
				ss << "\tUse max value as 0xffffffff.\n";
				Log::log(Logger::ERROR, ss.str());
				ss.str("");
				error = true;
			}
			
			ss << "\tUsed \"imageLinkAddress\" value: " << conf.imageLinkAddress << "." << endl;
			Log::log(Logger::INFO2, ss.str());
			ss.str("");

		}
		else {
			ss << "\tCannot read \"imageLinkAddress\" from configuration file: " << confFilePath << ".\n";
			Log::log(Logger::ERROR, ss.str());
			ss.str("");
			error = true;
		}
	}

	if (jsonConf["enableTrustZone"].get_type() == jute::JBOOLEAN)
		conf.trustZone = jsonConf["enableTrustZone"].as_bool();
	else {
		ss << "\tCannot read \"enableTrustZone\" from configuration file: " << confFilePath << ".\n\tTrust zone disabled by default.\n";
		Log::log(Logger::INFO, ss.str());
		ss.str("");
	}

	if (conf.trustZone) {
		if (jsonConf["trustZonePresetFile"].get_type() == jute::JSTRING) {
			conf.trustZonePresetFile = jsonConf["trustZonePresetFile"].as_string();
			if (conf.trustZonePresetFile.length() > 0) {
				auto pos = conf.trustZonePresetFile.find_last_of('.');
				if (pos != string::npos) {
					auto type = conf.trustZonePresetFile.substr(pos);
					transform(type.begin(), type.end(), type.begin(), ::tolower);
					if (type == ".bin")
						conf.generateTrustZone = false;
					else if (type == ".json")
						conf.generateTrustZone = true;
					else
						pos = -1;					
				}
				if(pos == string::npos) {
					ss << "\tUnexpected value of \"trustZonePresetFile\" (" << conf.trustZonePresetFile << ") from configuration file: " << confFilePath << ".\n";
					ss << "\tPlease specify valid path to TZ-Preset .bin file or TZ-Preset .json configuration file or keep it empty to disable TZ-preset configuration in image.\n" << "\tTZ-Preset disabled in image.\n";
					Log::log(Logger::ERROR, ss.str());
					ss.str("");
					error = true;
				}
			}
		}
		else {
			ss << "\tCannot read \"trustZonePresetFile\" from configuration file: " << confFilePath << ".\n\tTZ-Preset disabled in image.\n";
			Log::log(Logger::INFO, ss.str());
			ss.str("");
		}
	}

	if (jsonConf["masterBootOutputFile"].get_type() == jute::JSTRING)
		conf.masterBootOutputFile = jsonConf["masterBootOutputFile"].as_string();
	else {
		ss << "\tCannot read \"masterBootOutputFile\" from configuration file: " << confFilePath << ".\n";
		Log::log(Logger::ERROR, ss.str());
		ss.str("");
		error = true;
	}
	if (error)
		Log::log(Logger::WARNING, "\tParsed with error(s).\n" );
	else 
		Log::log(Logger::INFO, "\tSuccess.\n");

	return !error;
}

int AuthImageGenerator::createImageLpcSboot(const configuration &conf)
{
	FILE *imageFile = NULL, *configFile = NULL, *usedRootCrtFile = NULL, *outputFile = NULL, *trustZonePresetFile = NULL;
	uint32_t signatureSizeInBytes = 0, rootCertCount = 0, usedRootCrtAlignedSize = 0, certCount = 0, rkhOffset = 0, status = SUCCESS_RETURN;
	uint8_t *buffer = NULL, *signature = NULL, ctrInitVector[16] = { 0 }, *userKey = NULL, *keyStore = NULL;
	const char* configdata = NULL;
	vector<string> crtList;
	string usedRootCertPath;
	int ret = -1, step = 1;
	size_t totalImageSize = 0, crtTableAlignedSize = 0, usedRootCrtSize = 0, crtTableSize = 0, userKeySize = 0, keyStoreSize = 0;
	mbedtls_ctr_drbg_context ctr_drbg;
	mbedtls_rsa_context *rsa_priv_key_ctx = NULL;
	rkh_table_t rkh;
	mbedtls_x509_crt *trustCrt = NULL;
	_headerMac_t s_headerMac{};



	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Check the image file.
	////////////////////////////////////////////////////////////////////////////////////////////////////
	Log::log(Logger::INFO, "Starting processing image....\n");
	Log::log(Logger::INFO2, "%d. Check of the image file.\n", step);
	configdata = conf.imageFile.c_str();
	imageFile = fopen(configdata, "rb");
	if (imageFile == NULL)
	{
		Log::log(Logger::ERROR, "\tCannot open image file %s.\n", configdata);
		cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
		return FAIL_RETURN;
	}
	// Get image length in bytes.
	fseek(imageFile, 0, SEEK_END);
	size_t imageSize = ftell(imageFile);
	fseek(imageFile, 0, SEEK_SET);
	// Get the word aligned length.
	size_t imageAlignedSize = 0;
	if (imageSize % AUTH_IMAGE_ALIGNMENT)
	{
		imageAlignedSize = imageSize + (AUTH_IMAGE_ALIGNMENT - (imageSize % AUTH_IMAGE_ALIGNMENT));
	}
	else
	{
		imageAlignedSize = imageSize;
	}
	Log::log(Logger::INFO2, "\tSuccess. (File %s: Size = %lu bytes, AlignedSize = %lu bytes)\n", configdata, imageSize, imageAlignedSize);
	step++; //2
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Read image type
	////////////////////////////////////////////////////////////////////////////////////////////////////
	Log::log(Logger::INFO2, "%d. Fetching of image configuration: execution target and security.\n", step);
	uint32_t imageType = kSKBOOT_ImageTypePlainSigned;
	configdata = conf.imageType.c_str();
	if (strncmp("ramsigned", configdata, MAX_PATH) == 0)
	{
		imageType = kSKBOOT_ImageTypePlainSigned;
		Log::log(Logger::INFO2, "\tRAM - plain signed: image will be signed based on provided configuration.\n");
	}
	else if (strncmp("ramcrc", configdata, MAX_PATH) == 0)//CRC
	{
		imageType = kSKBOOT_ImageTypePlainCrc;
		Log::log(Logger::INFO2, "\tRAM - plain with CRC: security configuration will be skipped.\n");
	}
	else if (strncmp("ramencrypted+signed", configdata, MAX_PATH) == 0)
	{
		imageType = kSKBOOT_ImageTypeEncryptedSigned;
		Log::log(Logger::INFO2, "\tRAM - encrypted signed: image will be encrypted and signed based on provided configuration.\n");
	}
	else if (strncmp("xipsigned", configdata, MAX_PATH) == 0)
	{
		imageType = kSKBOOT_ImageTypeXipPlainSigned;
		
		Log::log(Logger::INFO2, "\t%s flash (XIP) - plain signed: image will be signed based on provided configuration.\n", conf.family == LPC55XX ? "External": "Internal");
	}
	else if (strncmp("xipcrc", configdata, MAX_PATH) == 0) //CRC
	{
		imageType = kSKBOOT_ImageTypeXipPlainCrc;
		Log::log(Logger::INFO2, "\t%s flash (XIP) - plain with CRC: security configuration will be skipped.\n", conf.family == LPC55XX ? "External" : "Internal");
	}
	else
	{
		Log::log(Logger::ERROR, "Unexpected image type value: %s.\n", configdata);
		cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
		return FAIL_RETURN;
	}
	Log::log(Logger::INFO2, "\tSuccess.\n");

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Check Image Link Address
	////////////////////////////////////////////////////////////////////////////////////////////////////
	uint32_t imageLinkAddress;
	Log::log(Logger::INFO2, "%d.1 Checking image link address configuration.\n", step);
	if (conf.imageLinkAddressFromImage)
		Log::log(Logger::INFO2, "\tImage link address will be used from input image itself.\n");
	else {
		Log::log(Logger::INFO2, "\tImage link address will be set to: 0x%08x\n", conf.imageLinkAddress);
		imageLinkAddress = conf.imageLinkAddress;
	}
	Log::log(Logger::INFO2, "\tSuccess.\n");

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Checking trust zone configuration
	//////////////////////////////////////////////////////////////////////////////////////////////////// 
	Log::log(Logger::INFO2, "%d.2 Checking image trust zone configuration.\n", step);
	uint32_t imageTypeTZM = 0;
	size_t trustZonePresetFileSize = 0;
	unique_ptr<vector<uint32_t>> trustZonedata;
	if (conf.trustZone == false) //TZ-M Disabled
	{
		imageTypeTZM |= SKBOOT_IMG_HDR_TZM_IMAGE_TYPE_MASK;
		Log::log(Logger::INFO2, "\tTrust zone disabled image -> configuration of TZM-M_Preset and TZM-M_PresetFile is ignored.\n");
	}
	else
	{
		if (conf.trustZonePresetFile.length() > 0)
		{
			Log::log(Logger::INFO2, "\tTrust zone enabled image: configuration of TZM-M_Preset enabled -> TZM-M_PresetFile will be loaded and included in image.\n");
			imageTypeTZM |= SKBOOT_IMG_HDR_TZM_PRESET_MASK;
			if (conf.generateTrustZone) {
				Log::log(Logger::INFO2, "\tGenerating TZ-Preset configuration based on %s configuration file:.\n", conf.trustZonePresetFile.c_str());
				try {
					auto tpg = TrustZonePresetGenerator();
					trustZonedata = tpg.getTzpPreset(conf.trustZonePresetFile, conf.family);
					trustZonePresetFileSize = (*trustZonedata).size() * sizeof(uint32_t);
				}
				catch (runtime_error e) {
					Log::log(Logger::ERROR, "\tCannot generate TZM-M_Preset configuration: %s\n", e.what());
					cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
					return FAIL_RETURN;
				}
				catch (...) {
					Log::log(Logger::ERROR, "\tCannot generate TZM-M_Preset configuration: Unexpected error.\n");
					cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
					return FAIL_RETURN;
				}
			}
			else {
				Log::log(Logger::INFO2, "%d.2.1 Checking trust zone preset file %s.\n",step,  conf.trustZonePresetFile.c_str());
				const auto tZPFileName = conf.trustZonePresetFile.c_str();
				trustZonePresetFile = fopen(tZPFileName, "rb");
				if (NULL == trustZonePresetFile)
				{
					Log::log(Logger::ERROR, "\tCannot open TZM-M_PresetFile file %s.\n", tZPFileName);
					cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
					return FAIL_RETURN;
				}
				fseek(trustZonePresetFile, 0, SEEK_END);
				trustZonePresetFileSize = ftell(trustZonePresetFile);
				fseek(trustZonePresetFile, 0, SEEK_SET);
				if (trustZonePresetFileSize % AUTH_IMAGE_ALIGNMENT)
				{
					Log::log(Logger::ERROR, "Unexpected size of TZM-M_PresetFile: %lu bytes, byte count should be divisible by %d without the remainder.\n", trustZonePresetFileSize, AUTH_IMAGE_ALIGNMENT);
					cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
					return FAIL_RETURN;
				}
				Log::log(Logger::INFO2, "\tSuccess: Trust zone preset file %s verified. (size: %lu bytes)\n", tZPFileName, trustZonePresetFileSize);
			}
		}
		else
		{
			Log::log(Logger::INFO2, "\tTrust zone enabled image: configuration of TZM-M_Preset disabled -> TZM-M_PresetFile is ignored and not used.\n");
		}
	}


	if (imageType == kSKBOOT_ImageTypeXipPlainCrc || imageType == kSKBOOT_ImageTypePlainCrc)
	{
		Log::log(Logger::INFO2, "Start to generate CRC image!\n");
		totalImageSize = imageAlignedSize;
	}
	else
	{
		Log::log(Logger::INFO2, "Start to generate signed image!\n");
		////////////////////////////////////////////////////////////////////////////////////////////////////
		// Checking key store configuration
		//////////////////////////////////////////////////////////////////////////////////////////////////// 
		if (imageType == kSKBOOT_ImageTypePlainSigned || imageType == kSKBOOT_ImageTypeEncryptedSigned /*|| imageType ==  kSKBOOT_ImageTypePlainCrc*/)
		{
			Log::log(Logger::INFO2, "%d.3 Checking key store configuration.\n", step);
			if (conf.useKeyStore)
			{
				imageType |= SKBOOT_IMG_HDR_KEY_STORE_INCLUDED_MASK;
				Log::log(Logger::INFO2, "\tKey store enabled image.\n");
			}
			else
			{
				Log::log(Logger::INFO2, "\tKey store disabled image.\n");
			}
			Log::log(Logger::INFO2, "\tSuccess.\n");
		}
		step++;
		
		////////////////////////////////////////////////////////////////////////////////////////////////////
		// Load the Root certificate files.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Log::log(Logger::INFO2, "%d. Load the root certificates.\n", step);
		////////////////////////////////////////////////////////////////////////////////////////////////////
		// Load the count of root certificate files.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Log::log(Logger::INFO2, "%d.1 Load the count of root certificates.\n", step);
		rootCertCount = conf.rootCertFiles.size();
		if ((rootCertCount > MAX_ROOT_CERT_COUNT) || (rootCertCount == 0))
		{
			Log::log(Logger::ERROR, "Invalid count of root certificates. Should between 1 - 4.\n");
			cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
			return FAIL_RETURN;
		}
		Log::log(Logger::INFO2, "\tSuccess. (Root Certification Count = %d)\n", rootCertCount);

		////////////////////////////////////////////////////////////////////////////////////////////////////
		// Load the Index of used root certificate files.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Log::log(Logger::INFO2, "%d.2 Load selected certificate chain id, used to sign this image.\n", step);
		uint32_t usedRootCertIndex = conf.mainCertId;
		if ((usedRootCertIndex >= MAX_ROOT_CERT_COUNT) || (usedRootCertIndex >= rootCertCount))
		{
			Log::log(Logger::INFO2, "Invalid index of selected certificate chain. Should between 0 to %d.\n", rootCertCount - 1);
			cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
			return FAIL_RETURN;
		}
		Log::log(Logger::INFO2, "\tSuccess. (Selected certificatate chain index = %d)\n", usedRootCertIndex);

		////////////////////////////////////////////////////////////////////////////////////////////////////
		// Load all the Root certificate files.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Log::log(Logger::INFO2, "%d.3 Load all root certificates.\n", step);
		memset(&rkh, 0, sizeof(rkh_table_t));

		for (int i = 0; i < MAX_ROOT_CERT_COUNT; i++) {
			if (conf.rootCertFiles[i].size() == 0) {
				if (usedRootCertIndex == i) 
				{
					Log::log(Logger::ERROR, "\tRoot certificate %d, which is part of selected certificate chain, cannot be empty.\n", i);
					cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
					return FAIL_RETURN;
				}
				else
				{
					continue;
				}
			}

			const char* rootCertFile = (conf.rootCertFiles[i]).c_str();
			FILE *rootCrtFile = fopen(rootCertFile, "rb");
			if (rootCrtFile == NULL)
			{
				Log::log(Logger::INFO2, "\tCannot open root certificate file %d (%s).\n", i, rootCertFile);
				cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
				return FAIL_RETURN;
			}
			fclose(rootCrtFile);
			if (conf.mainCertId == i)
			{
				usedRootCertPath = rootCertFile;
			}
			mbedtls_x509_crt rootCrt;
			mbedtls_x509_crt_init(&rootCrt);

			ret = mbedtls_x509_crt_parse_file(&rootCrt, rootCertFile);
			if (ret != 0)
			{
				Log::log(Logger::ERROR, "\tFailed to parse root certificate file %d(%s)(error code = %d).\nExpected X.509 certificate in DER format.\n", i, rootCertFile, ret);
				cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
				return FAIL_RETURN;
			}

			uint32_t flags;
			ret = mbedtls_x509_crt_verify(&rootCrt, &rootCrt, NULL, NULL, &flags, NULL, NULL);
			if (ret != 0)
			{
				Log::log(Logger::ERROR, "\tRoot certificate %d must be selfsigned.\n", i);
				cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
				return FAIL_RETURN;
			}
			else
			{
				Log::log(Logger::INFO2, "\tRoot certificate %d is self signed.\n", i);
			}

			size_t n_size = mbedtls_mpi_size(&((mbedtls_rsa_context *)rootCrt.pk.pk_ctx)->N);
			size_t e_size = mbedtls_mpi_size(&((mbedtls_rsa_context *)rootCrt.pk.pk_ctx)->E);
			uint8_t *buf = (uint8_t *)calloc(1, n_size + e_size);
			mbedtls_mpi_write_binary(&((mbedtls_rsa_context *)rootCrt.pk.pk_ctx)->N, buf, n_size);
			mbedtls_mpi_write_binary(&((mbedtls_rsa_context *)rootCrt.pk.pk_ctx)->E, buf + n_size, e_size);
			mbedtls_sha256(buf, n_size + e_size, rkh.entries[i].rkh, false);

			mbedtls_x509_crt_free(&rootCrt);
			Log::log(Logger::INFO2, "\tSuccess. (Root Certification %d = %s)\n", i, rootCertFile);
		}
		
		////////////////////////////////////////////////////////////////////////////////////////////////////
		//  Calculate used root certificate size.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Log::log(Logger::INFO2, "%d.4 Calculate size of root certificates.\n", step);
		usedRootCrtFile = fopen(usedRootCertPath.c_str(), "rb");
		if (usedRootCrtFile == NULL)
		{
			Log::log(Logger::ERROR, "\tCannot open the selected root certificate file %s.\n", usedRootCertPath.c_str());
			cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
			return FAIL_RETURN;
		}
		// Get image length in bytes.
		fseek(usedRootCrtFile, 0, SEEK_END);
		usedRootCrtSize = ftell(usedRootCrtFile);
		fseek(usedRootCrtFile, 0, SEEK_SET);

		if (usedRootCrtSize % AUTH_IMAGE_ALIGNMENT)
		{
			usedRootCrtAlignedSize = usedRootCrtSize + (AUTH_IMAGE_ALIGNMENT - (usedRootCrtSize % AUTH_IMAGE_ALIGNMENT));
		}
		else
		{
			usedRootCrtAlignedSize = usedRootCrtSize;
		}

		Log::log(Logger::INFO2, "\tSuccess. (Root Certification Size = %lu bytes, Aligned Size = %d bytes)\n", usedRootCrtSize, usedRootCrtAlignedSize);
		step++;
		////////////////////////////////////////////////////////////////////////////////////////////////////
		// Load all certificates.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Log::log(Logger::INFO2, "%d. Load all certificates in selected certificate chain.\n", step);
		////////////////////////////////////////////////////////////////////////////////////////////////////
		// Load the count of the certificates.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Log::log(Logger::INFO2, "%d.1 Load the count of chained certificates in selected certificate chain.\n", step);
		certCount = conf.chainCertFiles.size();
		if (certCount < 0)
		{
			Log::log(Logger::ERROR, "Invalid count of the certificates chained in selected certificate chain.\n");
			cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
			return FAIL_RETURN;
		}
		Log::log(Logger::INFO2, "\tSuccess. (Certificate count = %d)\n", certCount);
		////////////////////////////////////////////////////////////////////////////////////////////////////
		// Load all certificates.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Log::log(Logger::INFO2, "%d.2 Load and parse certificates in selected certificate chain.\n", step);
		trustCrt = (mbedtls_x509_crt *)malloc(sizeof(mbedtls_x509_crt));
		mbedtls_x509_crt_init(trustCrt);
		ret = mbedtls_x509_crt_parse_file(trustCrt, usedRootCertPath.c_str());
		if (ret != 0)
		{
			Log::log(Logger::ERROR, "\tFailed to parse selected root certificate file (%s)(error code = %d).\n", usedRootCertPath.c_str(), ret);
			cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
			return FAIL_RETURN;
		}
		if (certCount > 0) {
			for (uint32_t i = 0; i < certCount; i++)
			{
				const char* chainCertFile = conf.chainCertFiles[i].c_str();
				FILE *crtFile = fopen(chainCertFile, "rb");
				if (crtFile == NULL)
				{
					Log::log(Logger::INFO2, "\tCannot open certificate file %d(%s)(error code = %d).\n", i, chainCertFile, ret);
					cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
					return FAIL_RETURN;
				}
				fclose(crtFile);

				mbedtls_x509_crt *testCrt = (mbedtls_x509_crt *)malloc(sizeof(mbedtls_x509_crt));
				mbedtls_x509_crt_init(testCrt);
				ret = mbedtls_x509_crt_parse_file(testCrt, chainCertFile);
				if (ret != 0)
				{
					Log::log(Logger::ERROR, "\tFailed to parse certificate file %d(%s)(error code = %d).\n", i, chainCertFile, ret);
					cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
					return FAIL_RETURN;
				}

				uint32_t flags;
				ret = mbedtls_x509_crt_verify(testCrt, trustCrt, NULL, NULL, &flags, NULL, NULL);
				if (ret != 0)
				{
					Log::log(Logger::ERROR, "\tVerify Certification chain failed at index %d(%s)(error code = %d).\n", i, chainCertFile, ret);
					cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
					return FAIL_RETURN;
				}
				mbedtls_x509_crt_free(trustCrt);
				free(trustCrt);
				trustCrt = testCrt;
				testCrt = NULL;

				crtList.push_back(chainCertFile);
				Log::log(Logger::INFO2, "\tSuccess. ( Certification %d = %s)\n", i, chainCertFile);
			}
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////
		// Calculate all the certificates size.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Log::log(Logger::INFO2, "%d.3 Calculate size of all chained certificates.\n", step);
		size_t crtFileTotalSize = 0;

		size_t i = 0;
		for (vector<string>::iterator it = crtList.begin(); it != crtList.end(); it++, i++)
		{
			FILE *crtFile = fopen(it->c_str(), "rb");
			if (crtFile == NULL)
			{
				Log::log(Logger::ERROR, "\tCannot open the used certificate file %s.\n", it->c_str());
				cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
				return FAIL_RETURN;
			}
			fseek(crtFile, 0, SEEK_END);
			size_t crtFileSize = ftell(crtFile);
			fseek(crtFile, 0, SEEK_SET);
			fclose(crtFile);

			uint32_t crtFileAlignedSize;
			if (crtFileSize % AUTH_IMAGE_ALIGNMENT)
			{
				crtFileAlignedSize = crtFileSize + (AUTH_IMAGE_ALIGNMENT - (crtFileSize % AUTH_IMAGE_ALIGNMENT));
			}
			else
			{
				crtFileAlignedSize = crtFileSize;
			}

			crtFileTotalSize += crtFileAlignedSize;

			Log::log(Logger::INFO2, "\tSuccess. (Certification %d size = %lu bytes, Aligned size = %d bytes)\n", i, crtFileSize, crtFileAlignedSize);
		}
		Log::log(Logger::INFO2, "\tSuccess. (Certification total size = %lu bytes)\n", crtFileTotalSize);
		step++;

		////////////////////////////////////////////////////////////////////////////////////////////////////
		// Load and parse the private key.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Log::log(Logger::INFO2, "%d. Load and parse the private key of certificate used for signing of output image.\n", step);
		mbedtls_pk_context pk_ctx;
		mbedtls_pk_init(&pk_ctx);
		if ((ret = mbedtls_pk_parse_keyfile(&pk_ctx, conf.mainCertPrivateKeyFile.c_str(), NULL)) != 0)
		{
			Log::log(Logger::ERROR, "Parse private key failed.(%d)(%s)\n", ret, conf.mainCertPrivateKeyFile.c_str());
			cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
			return FAIL_RETURN;
		}
		// Get the private key.
		rsa_priv_key_ctx = (mbedtls_rsa_context *)(pk_ctx.pk_ctx);
		Log::log(Logger::INFO2, "\tSuccess. (Private Key File Path = %s\n", conf.mainCertPrivateKeyFile.c_str());
		signatureSizeInBytes = rsa_priv_key_ctx->len;
		step++;

		////////////////////////////////////////////////////////////////////////////////////////////////////
		// Generate the entire image data
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Log::log(Logger::INFO2, "%d. Calculate certificate table size.\n", step);

		// Calculate CRT table size and align it to word boundary.
		crtTableSize = usedRootCrtAlignedSize + crtFileTotalSize + 4 * (certCount + 1 /*root*/);
		// Align the crt table size to word boundary.

		if (crtTableSize % AUTH_IMAGE_ALIGNMENT)
		{
			crtTableAlignedSize = crtTableSize + (AUTH_IMAGE_ALIGNMENT - (crtTableSize % AUTH_IMAGE_ALIGNMENT));
		}
		else
		{
			crtTableAlignedSize = crtTableSize;
		}
		Log::log(Logger::INFO2, "\tSuccess.\n");
		// Calculate entire image size including image header and crt.
		totalImageSize = imageAlignedSize + sizeof(certificate_block_header_t) + crtTableAlignedSize + sizeof(rkh_table_t);
		step++;
		////////////////////////////////////////////////////////////////////////////////////////////////////
		// Initialization of random number generator for mbedtls
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Log::log(Logger::INFO2, "%d Initialization of random number generator.\n", step);
		// mbedTLS drbg will be used to generate random numbers
		// Init deterministic random bit generator.
		mbedtls_ctr_drbg_init(&ctr_drbg);
		// Init entropy.
		mbedtls_entropy_context entropy;
		mbedtls_entropy_init(&entropy);

		ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, NULL, 0);
		if (ret != 0)
		{
			Log::log(Logger::INFO2, "Seed random bit generator failed.%d\n", ret);
			cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
			return FAIL_RETURN;
		}
		Log::log(Logger::INFO2, "\tSuccess.\n");
	}
	
	if ((imageType == kSKBOOT_ImageTypeEncryptedSigned) || (imageType == kSKBOOT_ImageTypeEncryptedSignedKeyStoreIncluded))
	{
		// Malloc a buffer to contain the entire image data plus ES image tail.
		size_t allocSize = totalImageSize + ES_IMG_TAIL_BYTELEN + trustZonePresetFileSize;
		buffer = (uint8_t *)malloc(allocSize);
		// Init the buffer. padding bytes for the aligned are 0x00s.
		memset(buffer, 0, allocSize);
	}
	else
	{
		// Malloc a buffer to contain the entire image data.
		size_t allocSize = totalImageSize + trustZonePresetFileSize;
		buffer = (uint8_t *)malloc(allocSize);
		// Init the buffer. padding bytes for the aligned are 0x00s.
		memset(buffer, 0, allocSize);
	}
	step++; //8
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Cp the original image data
	////////////////////////////////////////////////////////////////////////////////////////////////////
	Log::log(Logger::INFO2, "%d. Load the input image data to buffer.\n", step);
	// Load image file to buffer.
	ret = fread(buffer, 1, imageSize, imageFile);
	Log::log(Logger::INFO2, "\tSuccess.\n");
	if (ret != imageSize)
	{
		Log::log(Logger::ERROR, "Fail reading image file.\n", configdata);
		cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
		return FAIL_RETURN;
	}

	/* LoadToRam signed images all have header MAC and optionally key store inserted at offset 64 bytes */
	if ((imageType == kSKBOOT_ImageTypeEncryptedSignedKeyStoreIncluded) || (imageType == kSKBOOT_ImageTypePlainSignedKeyStoreIncluded)
		|| (imageType == kSKBOOT_ImageTypeEncryptedSigned) || (imageType == kSKBOOT_ImageTypePlainSigned))
	{
		step++;
		configdata = conf.imageEncryptionKeyFile.c_str();
		Log::log(Logger::INFO2, "%d. Load the HMAC/image encryption key (%s).\n", step, configdata);
		// Read userkey
		ifstream userKeyFile(conf.imageEncryptionKeyFile, ios_base::in);
		
		if (userKeyFile.fail())
		{
			Log::log(Logger::ERROR, "Cannot open ImageEncryptionKey file %s.\n", configdata);
			cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
			return FAIL_RETURN;
		}
		userKeyFile.seekg(0, ios::end);
		userKeySize = (size_t)userKeyFile.tellg() / 2;
		userKeyFile.seekg(0, ios::beg);
		try {
			if (userKeySize >= 16 && userKeySize < 32) {
				userKeySize = 16;
				AESKey<128> aes(userKeyFile);
				userKey = new uint8_t[userKeySize];
				aes.getKey((AESKey<128>::key_t*)userKey);
			}
			else if (userKeySize >= 32) {
				userKeySize = 32;
				AESKey<256> aes(userKeyFile);
				userKey = new uint8_t[userKeySize];
				aes.getKey((AESKey<256>::key_t*)userKey);
			}
			else {
				Log::log(Logger::INFO2, "ImageEncryptionKey have unexpected size (16/32): %u\n", userKeySize);
				cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
				return FAIL_RETURN;
			}
			userKeyFile.close();
		}
		catch (exception &e) {
			Log::log(Logger::ERROR, "Cannot parse ImageEncryptionKey key: %s.\n", e.what());
			userKeyFile.close();
			cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
			return FAIL_RETURN;
		}
		Log::log(Logger::INFO2, "\tSuccess.\n");
	}

	if ((imageType == kSKBOOT_ImageTypeEncryptedSigned) || (imageType == kSKBOOT_ImageTypeEncryptedSignedKeyStoreIncluded))
	{
		step++;
		Log::log(Logger::INFO2, "%d. Encryption - Generate random IV.\n", step);
		// Generate random ctrInitVector
		if (0 != mbedtls_ctr_drbg_random(&ctr_drbg, ctrInitVector, sizeof(ctrInitVector)))
		{
			Log::log(Logger::ERROR, "Cannot generate random init vector for AES CTR encryption.\n");
			cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
			return FAIL_RETURN;
		}
		Log::log(Logger::INFO2, "\tSuccess. (IV: 0x%08x 0x%08x 0x%08x 0x%08x)\n", ((uint32_t*)ctrInitVector)[0], ((uint32_t*)ctrInitVector)[1], ((uint32_t*)ctrInitVector)[2], ((uint32_t*)ctrInitVector)[3]);

		// Save ctrInitVector to ES_IMG_TAIL
		memcpy(&buffer[totalImageSize + ES_IMG_TAIL_BYTELEN - sizeof(ctrInitVector)], ctrInitVector, sizeof(ctrInitVector));

		Log::log(Logger::INFO2, "%d.1 Encryption - Encrypting input plain image data using %s key.\n", step, conf.imageEncryptionKeyFile.c_str());
		// Encrypt original image data
		size_t nc_off = 0;
		uint8_t stream_block[16] {};
		mbedtls_aes_context aesCtx;
		mbedtls_aes_init(&aesCtx);
		mbedtls_aes_setkey_enc(&aesCtx, userKey, userKeySize * 8);
		mbedtls_aes_crypt_ctr(&aesCtx, totalImageSize, &nc_off, ctrInitVector, stream_block, buffer, buffer);
		mbedtls_aes_free(&aesCtx);

		// Save encrypted vector table to ES_IMG_TAIL
		memcpy(&buffer[totalImageSize], buffer, sizeof(es_header_t));

		totalImageSize += ES_IMG_TAIL_BYTELEN;
		Log::log(Logger::INFO2, "\tSuccess.\n");
	}

	// add header MAC for all LoadToRam images
	size_t headerMacSizeInBytes = 0;
	if ((imageType == kSKBOOT_ImageTypeEncryptedSignedKeyStoreIncluded) || (imageType == kSKBOOT_ImageTypePlainSignedKeyStoreIncluded)
		|| (imageType == kSKBOOT_ImageTypeEncryptedSigned) || (imageType == kSKBOOT_ImageTypePlainSigned))
	{
		headerMacSizeInBytes = sizeof(s_headerMac);
	}
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Add key store
	////////////////////////////////////////////////////////////////////////////////////////////////////
	if ((imageType == kSKBOOT_ImageTypeEncryptedSignedKeyStoreIncluded) || (imageType == kSKBOOT_ImageTypePlainSignedKeyStoreIncluded))
	{
		step++;
		Log::log(Logger::INFO2, "%d. Key store configuration.\n", step);
		if (conf.useKeyStore)
		{
			if (conf.keyStoreFile.length() > 0)
			{
				Log::log(Logger::INFO2, "%d.1 Add key store from specified file.\n", step);
				FILE *keyStoreFile = NULL;
				configdata = conf.keyStoreFile.c_str();
				keyStoreFile = fopen(configdata, "rb");
				if (NULL == keyStoreFile)
				{
					Log::log(Logger::INFO2, "Cannot open key store file %s.\n", configdata);
					cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
					return FAIL_RETURN;
				}

				fseek(keyStoreFile, 0, SEEK_END);
				auto keyStoreFileSize = ftell(keyStoreFile);
				Log::log(Logger::INFO2, "\tKey Store File Path = %s, size = %lu bytes\n", configdata, keyStoreFileSize);
				fseek(keyStoreFile, 0, SEEK_SET);

				if (LPC55XX == conf.family)
				{
					keyStoreSize = sizeof(skboot_key_store_lpc55xx_t);
					keyStore = new uint8_t[keyStoreSize]{};
					if (keyStoreSize != keyStoreFileSize)
					{
						Log::log(Logger::ERROR, "\tWrong size of key store file for %s. (expected: %d bytes, fetched: %lu bytes)\n", LPC55XX, keyStoreSize, keyStoreFileSize);
						cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
						return FAIL_RETURN;
					}
					ret = fread(keyStore, 1, keyStoreSize, keyStoreFile);
				}
				else if (RT6XX == conf.family)
				{
					keyStoreSize = sizeof(skboot_key_store_lpc68xx_t);
					keyStore = new uint8_t[keyStoreSize]{};
					if (keyStoreSize != keyStoreFileSize)
					{
						Log::log(Logger::ERROR, "\tWrong size of key store file for %s. (expected: %d bytes, fetched: %lu bytes)\n", RT6XX, keyStoreSize, keyStoreFileSize);
						cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
						return FAIL_RETURN;
					}
					ret = fread(keyStore, 1, keyStoreSize, keyStoreFile);
				}
				else
				{
					Log::log(Logger::ERROR, "Unsupported device family: %s.\n", conf.family.c_str());
					cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
					return FAIL_RETURN;
				}
				if (ret != keyStoreSize)
				{
					Log::log(Logger::ERROR, "Fail reading key store file.\n");
					cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
					return FAIL_RETURN;
				}
			}
			else
			{
				Log::log(Logger::INFO2, "%d.1 Reserve key store space only.\n", step);
				if (LPC55XX == conf.family)
				{
					keyStoreSize = sizeof(skboot_key_store_lpc55xx_t);
					keyStore = new uint8_t[keyStoreSize] {};
				}
				else if (RT6XX == conf.family)
				{
					keyStoreSize = sizeof(skboot_key_store_lpc68xx_t);
					keyStore = new uint8_t[keyStoreSize] {};
				}
				else
				{
					Log::log(Logger::ERROR, "Unsupported device family: %s.\n", conf.family.c_str());
					cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
					return FAIL_RETURN;
				}
				Log::log(Logger::INFO, "\tKey store with size %lu bytes will be reserved in image from address: 0x%x.\n", keyStoreSize, HMACHEADEROFSET + headerMacSizeInBytes);
			}
		}
		Log::log(Logger::INFO2, "\tSuccess.\n");
	}

	step++;
	Log::log(Logger::INFO2, "%d. Updating image header.\n", step);
	// Modify the header offsize.
	*(uint32_t *)(buffer + 0x28) = imageAlignedSize;
	*(uint32_t *)(buffer + 0x24) = imageType | imageTypeTZM;
	*(uint32_t *)(buffer + 0x20) = totalImageSize + signatureSizeInBytes + keyStoreSize + headerMacSizeInBytes + trustZonePresetFileSize;
	if (!conf.imageLinkAddressFromImage)
		*(uint32_t *)(buffer + 0x34) = imageLinkAddress;

	Log::log(Logger::INFO2, "\tSuccess. (Image Type = 0x%08x, Image load address = 0x%x, Total Image size = %d bytes)\n", *(uint32_t *)(buffer + 0x24), *(uint32_t *)(buffer + 0x34), *(uint32_t *)(buffer + 0x20));
	
	if (imageType != kSKBOOT_ImageTypeXipPlainCrc && imageType != kSKBOOT_ImageTypePlainCrc)
	{
		step++;
		////////////////////////////////////////////////////////////////////////////////////////////////////
		// Init certificate header
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Log::log(Logger::INFO2, "%d. Init certificate header.\n", step);
		certificate_block_header_t *header = (certificate_block_header_t *)(buffer + imageAlignedSize);

		header->signature = SIGNATURE_TAG;
		header->headerMajorVersion = 1;
		header->headerMinorVersion = 0;
		header->headerLengthInBytes = sizeof(certificate_block_header_t);
		header->flags = 0;
		header->buildNumber = FOUR_CHAR_CODE(1, 0, 0, 0);
		header->totalImageLengthInBytes = totalImageSize + trustZonePresetFileSize;
		header->certificateCount = 1 /*root cert*/ + certCount;
		header->certificateTableLengthInBytes = crtTableAlignedSize;

		uint32_t crtTableOffset = imageAlignedSize + sizeof(certificate_block_header_t);
		uint32_t crtOffset = crtTableOffset;
		Log::log(Logger::INFO2, "\tSuccess.\n");
		////////////////////////////////////////////////////////////////////////////////////////////////////
		// Add root certificate
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Log::log(Logger::INFO2, "%d.1 Add root certificate.\n", step);
		*(uint32_t *)(buffer + crtOffset) = usedRootCrtAlignedSize;
		crtOffset += 4;
		fread(buffer + crtOffset, 1, usedRootCrtSize, usedRootCrtFile);
		crtOffset += usedRootCrtAlignedSize;
		Log::log(Logger::INFO2, "\tSuccess.\n");
		////////////////////////////////////////////////////////////////////////////////////////////////////
		// Add certificate
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Log::log(Logger::INFO2, "%d.2 Add chained certificates from selected chain.\n", step);
		int i = 0;
		for (vector<string>::iterator it = crtList.begin(); it != crtList.end(); it++, i++)
		{
			FILE *crtFile = fopen(it->c_str(), "rb");
			if (crtFile == NULL)
			{
				Log::log(Logger::ERROR, "\tCannot open the used certificate file %s.\n", usedRootCertPath.c_str());
				cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
				return FAIL_RETURN;
			}
			fseek(crtFile, 0, SEEK_END);
			size_t crtFileSize = ftell(crtFile);
			fseek(crtFile, 0, SEEK_SET);

			uint32_t crtFileAlignedSize;
			if (crtFileSize % AUTH_IMAGE_ALIGNMENT)
			{
				crtFileAlignedSize = crtFileSize + (AUTH_IMAGE_ALIGNMENT - (crtFileSize % AUTH_IMAGE_ALIGNMENT));
			}
			else
			{
				crtFileAlignedSize = crtFileSize;
			}

			*(uint32_t *)(buffer + crtOffset) = crtFileAlignedSize;
			crtOffset += 4;
			fread(buffer + crtOffset, 1, crtFileSize, crtFile);
			crtOffset += crtFileAlignedSize;
			fclose(crtFile);
		}

		if ((crtOffset - crtTableOffset) != crtTableSize)
		{
			Log::log(Logger::ERROR, "\tgenerate certificate table failed. table size is %d, but should be %lu.\n", crtOffset - crtTableOffset, crtTableSize);
			cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
			return FAIL_RETURN;
		}
		Log::log(Logger::INFO2, "\tSuccess. (Certification table size = %d bytes)\n", crtOffset - crtTableOffset);
		////////////////////////////////////////////////////////////////////////////////////////////////////
		// Add root hash key table
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Log::log(Logger::INFO2, "%d.3 Add root hash key table (RKTH).\n", step);
		rkhOffset = crtOffset;
		memcpy(buffer + rkhOffset, &rkh, sizeof(rkh_table_t));
		Log::log(Logger::INFO2, "\tSuccess.\n");
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Add trust zone preset record if specified.
	////////////////////////////////////////////////////////////////////////////////////////////////////
	if (trustZonePresetFileSize > 0) {
		step++;
		Log::log(Logger::INFO2, "%d. Add trust zone preset data.\n", step);
		if (conf.generateTrustZone) {
			auto k{ 0 };
			for (auto const value : *trustZonedata) {
				*(uint32_t*)(buffer + totalImageSize + k) = value;
				k += sizeof(uint32_t);
			}
		}
		else {
			ret = fread(buffer + totalImageSize, 1, trustZonePresetFileSize, trustZonePresetFile);
			if (ret != trustZonePresetFileSize)
			{
				Log::log(Logger::ERROR, "Fail reading trust zone preset data from file.\n");
				cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
				return FAIL_RETURN;
			}
		}
		Log::log(Logger::INFO2, "\tSuccess.\n");
		totalImageSize += trustZonePresetFileSize;
	}
	if (imageType != kSKBOOT_ImageTypeXipPlainCrc && imageType != kSKBOOT_ImageTypePlainCrc)
	{
		step++;
		////////////////////////////////////////////////////////////////////////////////////////////////////
		// Generate the digest.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Log::log(Logger::INFO2, "%d. Generate output image sha256 digest for image signature.\n", step);
		// Buffer contains digest.
		uint8_t digest[32] = { 0 };

		// Call mbedtls to calculate digest.
		mbedtls_sha256_context sha256_ctx;
		mbedtls_sha256_init(&sha256_ctx);
		mbedtls_sha256_starts(&sha256_ctx, false /*isSHA224 = false*/);
		mbedtls_sha256_update(&sha256_ctx, buffer, totalImageSize);
		mbedtls_sha256_finish(&sha256_ctx, digest);

		Log::log(Logger::INFO2, "\tSuccess.\n");
		Log::log(Logger::INFO2, "\tSHA256 digest:");
		for (size_t i = 0; i < 32; i++)
		{
			Log::log(Logger::INFO2, " %#x", digest[i]);
		}
		Log::log(Logger::INFO2, "\n");
		step++;
		////////////////////////////////////////////////////////////////////////////////////////////////////
		// Generate the signature.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Log::log(Logger::INFO2, "%d. Generate output image signature.\n", step);
		// Buffer contains signature.
		signature = (uint8_t *)malloc(signatureSizeInBytes);
		memset(signature, 0, signatureSizeInBytes);

		ret = mbedtls_rsa_rsassa_pkcs1_v15_sign(rsa_priv_key_ctx, mbedtls_ctr_drbg_random, &ctr_drbg, MBEDTLS_RSA_PRIVATE,
			MBEDTLS_MD_SHA256, 256, digest, signature);
		if (ret != 0)
		{
			Log::log(Logger::ERROR, "Generate signature failed.(%d)\n", ret);
			cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
			return FAIL_RETURN;
		}

		Log::log(Logger::INFO2, "\tSuccess.\n");
		Log::log(Logger::INFO2, "\tSignature:");
		for (size_t i = 0; i < signatureSizeInBytes; i++)
		{
			Log::log(Logger::INFO2, " %#x", signature[i]);
		}
		Log::log(Logger::INFO2, "\n");
		step++;
		////////////////////////////////////////////////////////////////////////////////////////////////////
		// Verify the signature.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Log::log(Logger::INFO2, "%d. Verify signature.\n", step);
		mbedtls_rsa_context *rsa_pub_key_ctx = (mbedtls_rsa_context *)(trustCrt->pk.pk_ctx);
		ret = mbedtls_rsa_rsassa_pkcs1_v15_verify(rsa_pub_key_ctx, NULL, NULL, MBEDTLS_RSA_PUBLIC, MBEDTLS_MD_SHA256, 256,
			digest, signature);
		free(rsa_pub_key_ctx);
		if (ret != 0)
		{
			Log::log(Logger::ERROR, "Verify signature failed.(%d)\n", ret);
			cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
			return FAIL_RETURN;
		}

		Log::log(Logger::INFO2, "\tSuccess.\n");
	}

	if (imageType == kSKBOOT_ImageTypeXipPlainCrc || imageType == kSKBOOT_ImageTypePlainCrc)
	{
		step++; 
		////////////////////////////////////////////////////////////////////////////////////////////////////
		// Generate the CRC.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Log::log(Logger::INFO2, "%d. CRC calculation.\n", step);
		uint32_t crcResult = 0;
		CRC32 crc;
		//skipping 4 bytes from image, because this 4 bytes will be used for storing CRC checksum
		crc.update(buffer, 0x28);
		crc.update(buffer + 0x2C, totalImageSize - 0x2C);
		crc.truncatedFinal(reinterpret_cast<uint8_t *>(&crcResult), sizeof(crcResult));

		Log::log(Logger::INFO2, "\tCRC32 checksum: 0x%X\n", crcResult);
		Log::log(Logger::INFO2, "\tSuccess.\n");
		*(uint32_t *)(buffer + 0x28) = crcResult;
	}
	
	if (imageType == kSKBOOT_ImageTypeEncryptedSignedKeyStoreIncluded || imageType == kSKBOOT_ImageTypePlainSignedKeyStoreIncluded
		|| imageType == kSKBOOT_ImageTypeEncryptedSigned || imageType == kSKBOOT_ImageTypePlainSigned)
	{
		step++;
		Log::log(Logger::INFO2, "%d. Calculate HMAC of output image header using %s key.\n", step, conf.imageEncryptionKeyFile.c_str());
		/* compute MAC of the 1st 64 bytes */
		/* MAC = HMAC-SHA256(key = AES_ECB(SBKEK, zeroAesBlock), buffer, 64) */
		uint8_t zeroKey[16] = { 0 };
		mbedtls_aes_context aesCtx;
		mbedtls_aes_init(&aesCtx);
		mbedtls_aes_setkey_enc(&aesCtx, userKey, userKeySize * 8);
		mbedtls_aes_crypt_ecb(&aesCtx, MBEDTLS_AES_ENCRYPT, zeroKey, zeroKey);
		mbedtls_aes_free(&aesCtx);
		mbedtls_md_hmac(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), zeroKey, sizeof(zeroKey), buffer, HMACHEADEROFSET, (uint8_t*)&s_headerMac);
		Log::log(Logger::INFO2, "\tImage Header MAC:");
		for (size_t i = 0; i < 32; i++)
		{
			Log::log(Logger::INFO2, " %#x", s_headerMac.b[i]);
		}
		Log::log(Logger::INFO2, "\n\tSuccess.\n");
	}

	if (imageType != kSKBOOT_ImageTypeXipPlainCrc && imageType != kSKBOOT_ImageTypePlainCrc) {
		step++;
		////////////////////////////////////////////////////////////////////////////////////////////////////
		// Output the root key hash table SHA256 hash.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Log::log(Logger::INFO2, "%d. Output the root certificates SHA256 hash (RKTH).\n", step);
	
		uint8_t rkhtHash[32];
		mbedtls_sha256_context rkhtHash_sha256_ctx;
		mbedtls_sha256_init(&rkhtHash_sha256_ctx);
		mbedtls_sha256_starts(&rkhtHash_sha256_ctx, false /*isSHA224 = false*/);
		mbedtls_sha256_update(&rkhtHash_sha256_ctx, (const unsigned char *)(buffer + rkhOffset), sizeof(rkh_table_t));
		mbedtls_sha256_finish(&rkhtHash_sha256_ctx, (uint8_t *)rkhtHash);

		Log::log(Logger::INFO2, "\tSuccess.\n");
		Log::log(Logger::INFO, "\tRKTH: ");
		for (size_t i = 0; i < 32; i++)
		{
			Log::log(Logger::INFO, "%02x", rkhtHash[i]);
		}
		Log::log(Logger::INFO, "\n");
	}

	step++;
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Output the image.
	////////////////////////////////////////////////////////////////////////////////////////////////////
	Log::log(Logger::INFO2, "%d. Creating output image file.\n", step);
	configdata = conf.masterBootOutputFile.c_str();
	outputFile = fopen(configdata, "wb");
	if (outputFile == NULL)
	{
		Log::log(Logger::ERROR, "\tCannot open/create output file %s.\n", configdata);
		cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
		return FAIL_RETURN;
	}
	Log::log(Logger::INFO2, "\tSuccess.\n");

	Log::log(Logger::INFO2, "%d.1 Writing output image to file.\n", step);
	if (imageType == kSKBOOT_ImageTypeXipPlainCrc || imageType == kSKBOOT_ImageTypePlainCrc) //CRC
	{
		ret = fwrite(buffer, 1, totalImageSize, outputFile);
		if (ret != totalImageSize)
		{
			Log::log(Logger::ERROR, "\tWrite CRC image to output file failed.\n");
			cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
			return FAIL_RETURN;
		}
		Log::log(Logger::INFO, "\tSuccess. (CRC image file: %s created)\n", conf.masterBootOutputFile.c_str());
		cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
		return SUCCESS_RETURN;
	}/* LoadToRam signed images all have header MAC and optionally key store inserted at offset 64 bytes */
	else if (imageType == kSKBOOT_ImageTypeEncryptedSignedKeyStoreIncluded || imageType == kSKBOOT_ImageTypePlainSignedKeyStoreIncluded
		|| imageType == kSKBOOT_ImageTypeEncryptedSigned || imageType == kSKBOOT_ImageTypePlainSigned)
	{
		/* Now write outputFile */
		ret = fwrite(buffer, 1, HMACHEADEROFSET, outputFile);
		if (ret != HMACHEADEROFSET)
		{
			Log::log(Logger::INFO2, "\tWrite image header data to output file failed..\n");
			cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
			return FAIL_RETURN;
		}
		/* MAC of the initial 64 bytes (header) */
		ret = fwrite(&s_headerMac, 1, headerMacSizeInBytes, outputFile);
		if (ret != headerMacSizeInBytes)
		{
			Log::log(Logger::ERROR, "\tWrite header MAC data to output file failed..\n");
			cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
			return FAIL_RETURN;
		}
		/* optional key store */
		if ((imageType == kSKBOOT_ImageTypeEncryptedSignedKeyStoreIncluded) || (imageType == kSKBOOT_ImageTypePlainSignedKeyStoreIncluded))
		{
			ret = fwrite(keyStore, 1, keyStoreSize, outputFile);
			if (ret != keyStoreSize)
			{
				Log::log(Logger::INFO2, "\tWrite key store data to output file failed..\n");
				cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
				return FAIL_RETURN;
			}
		}
		ret = fwrite(buffer + 64, 1, totalImageSize - HMACHEADEROFSET, outputFile);
		if (ret != (totalImageSize - HMACHEADEROFSET))
		{
			Log::log(Logger::ERROR, "\tWrite total image data to output file failed..\n");
			cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
			return FAIL_RETURN;
		}
		ret = fwrite(signature, 1, signatureSizeInBytes, outputFile);
		if (ret != signatureSizeInBytes)
		{
			Log::log(Logger::ERROR, "\tWrite signature to output file failed..\n");
			cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
			return FAIL_RETURN;
		}
	}
	else
	{
		ret = fwrite(buffer, 1, totalImageSize, outputFile);
		if (ret != totalImageSize)
		{
			Log::log(Logger::ERROR, "\tWrite total image data to output file failed..\n");
			cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
			return FAIL_RETURN;
		}

		ret = fwrite(signature, 1, signatureSizeInBytes, outputFile);
		if (ret != signatureSizeInBytes)
		{
			Log::log(Logger::INFO2, "\tWrite signature to output file failed..\n");
			cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
			return FAIL_RETURN;
		}
	}
	configdata = conf.masterBootOutputFile.c_str();
	Log::log(Logger::INFO, "\tSuccess. (Signed image %s created.)\n", configdata);
	
	cleanUpLpcSboot(userKey, keyStore, imageFile, usedRootCrtFile, outputFile, trustZonePresetFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
	return SUCCESS_RETURN;
}

void AuthImageGenerator::cleanUpLpcSboot(uint8_t *userKey, uint8_t * keyStore, FILE * imageFile, FILE * usedRootCrtFile, FILE * outputFile, FILE * trustZonePresetFile, uint8_t * buffer, uint8_t * signature, mbedtls_x509_crt * trustCrt, mbedtls_rsa_context * rsa_priv_key_ctx)
{
	if (imageFile != NULL)
	{
		fclose(imageFile);
	}
	if (usedRootCrtFile != NULL)
	{
		fclose(usedRootCrtFile);
	}
	if (outputFile != NULL)
	{
		fclose(outputFile);
	}
	if (trustZonePresetFile != NULL)
	{
		fclose(trustZonePresetFile);
	}
	if (buffer != NULL)
	{
		free(buffer);
	}
	if (signature != NULL)
	{
		free(signature);
	}
	if (trustCrt != NULL)
	{
		free(trustCrt);
	}
	if (rsa_priv_key_ctx != NULL)
	{
		free(rsa_priv_key_ctx);
	}
	if (keyStore != NULL)
	{
		delete keyStore;
	}
	if (userKey != NULL)
	{
		delete[] userKey;
	}
}

bool AuthImageGenerator::parseConfigurationK3Sboot(const string &confFilePath, configuration &conf)
{
	string tmp;
	auto error = false;
	stringstream ss;
	ss << "Parsing configuration file: " << confFilePath << ".\n";
	Log::log(Logger::INFO, ss.str());
	ss.str("");
	jute::jValue jsonConf;

	try {
		jsonConf = jute::parser::parse_file(confFilePath);
	}
	catch (runtime_error &e) {
		throw runtime_error(("Cannot parse json configuration file: " + confFilePath + " - " + e.what()).c_str());
	}

	if (jsonConf["family"].get_type() == jute::JSTRING) {
		conf.family = jsonConf["family"].as_string();
		transform(conf.family.begin(), conf.family.end(), conf.family.begin(), ::tolower);
		if (conf.family != "k32w0x") {
			ss << "\tUnexpected \"family\" value (" << conf.family << ") from configuration file: " << confFilePath << ".\n";
			ss << "\tSupported only k32w0x." << endl;
			Log::log(Logger::ERROR, ss.str());
			ss.str("");
			error = true;
		}
	}
	else {
		ss << "\tCannot read \"family\" from configuration file: " << confFilePath << ".\n";
		Log::log(Logger::ERROR, ss.str());
		ss.str("");
		error = true;
	}

	if (jsonConf["inputImageFile"].get_type() == jute::JSTRING)
		conf.imageFile = jsonConf["inputImageFile"].as_string();
	else {
		ss << "\tCannot read \"inputImageFile\" from configuration file: " << confFilePath << ".\n";
		Log::log(Logger::ERROR, ss.str());
		ss.str("");
		error = true;
	}	

	for (int i = 0; i < MAX_ROOT_CERT_COUNT; i++) {
		tmp = "rootCertificate" + to_string(i) + "File";
		if (jsonConf[tmp].get_type() == jute::JSTRING)
			conf.rootCertFiles.push_back(string(jsonConf[tmp].as_string()));
		else {
			ss << "\tCannot read \"" << tmp << "\" from configuration file: " << confFilePath << ".\n\tSkiping " << tmp << ".\n";
			Log::log(Logger::INFO, ss.str());
			ss.str("");
			conf.rootCertFiles.push_back(string(""));
		}
	}

	if (jsonConf["mainCertChainId"].get_type() == jute::JNUMBER) {
		conf.mainCertId = jsonConf["mainCertChainId"].as_int();
		if (conf.mainCertId > MAX_ROOT_CERT_COUNT - 1) {
			ss << "\tWrong value of \"mainCertChainId\" loaded from configuration file: " << confFilePath << ".\n";
			ss << "\tLoaded: " << conf.mainCertId << " value should be from 0  to " << MAX_ROOT_CERT_COUNT - 1 << ", using 0 as default.\n";
			Log::log(Logger::INFO, ss.str());
			ss.str("");
			conf.mainCertId = 0;
		}
	}
	else {
		ss << "\tCannot read \"mainCertChainId\" from configuration file: " << confFilePath << ".\n\tUsing 0 as default.\n";
		Log::log(Logger::INFO, ss.str());
		ss.str("");
	}

	if (conf.rootCertFiles[conf.mainCertId] == "") {
		ss << "\tSelected main root certificate/certificate chain \"mainCertChainId\" (" << (int)conf.mainCertId << ") from configuration file: " << confFilePath <<
			" don't have set corresponding value for \"rootCertificate" << (int)conf.mainCertId << "\".\n";
		Log::log(Logger::ERROR, ss.str());
		ss.str("");
		error = true;
	}

	short i = 0;
	tmp = "chainCertificate" + to_string(conf.mainCertId) + "File";
	while (jsonConf[tmp + to_string(i)].get_type() == jute::JSTRING) {
		conf.chainCertFiles.push_back(string(jsonConf[tmp + to_string(i)].as_string()));
		i++;
	}

	ss << "\tMain certificate chain " << (int)conf.mainCertId << " with " << i + 1 << " certificate(s) was fetched from configuration file: " << confFilePath << ".\n";
	Log::log(Logger::DEBUG, ss.str());
	ss.str("");

	if (jsonConf["mainCertPrivateKeyFile"].get_type() == jute::JSTRING)
		conf.mainCertPrivateKeyFile = jsonConf["mainCertPrivateKeyFile"].as_string();
	else {
		ss << "\tCannot read \"mainCertPrivateKeyFile\" from configuration file: " << confFilePath << ".\n";
		Log::log(Logger::ERROR, ss.str());
		ss.str("");
		error = true;
	}

	if (jsonConf["masterBootOutputFile"].get_type() == jute::JSTRING)
		conf.masterBootOutputFile = jsonConf["masterBootOutputFile"].as_string();
	else {
		ss << "\tCannot read \"masterBootOutputFile\" from configuration file: " << confFilePath << ".\n";
		Log::log(Logger::ERROR, ss.str());
		ss.str("");
		error = true;
	}

	if (error)
		Log::log(Logger::WARNING, "\tParsed with error(s).\n");
	else
		Log::log(Logger::INFO, "\tSuccess.\n");

	return !error;
}

int AuthImageGenerator::createImageK3Sboot(const configuration &conf)
{
	uint32_t status = SUCCESS_RETURN;
	FILE *imageFile = NULL, *configFile = NULL, *usedRootCrtFile = NULL, *outputFile = NULL;
	uint8_t *buffer = NULL, *signature = NULL;
	char *configdata = NULL;
	mbedtls_x509_crt * trustCrt = NULL;
	mbedtls_rsa_context * rsa_priv_key_ctx = NULL;
	std::vector<std::string> crtList;
	std::string usedRootCertPath;
	int ret = -1, step = 1;

	Log::log(Logger::INFO, "Start to generate authenticated image!\n");

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Open image file.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Log::log(Logger::INFO2, "%d. Open the image file.(%s)\n", step, conf.imageFile.c_str());
	imageFile = fopen(conf.imageFile.c_str(), "rb");
	if (imageFile == NULL)
	{
		Log::log(Logger::INFO2, "\tCannot open image file %s.\n", conf.imageFile.c_str());
		return FAIL_RETURN;
		
	}
	// Get image length in bytes.
	fseek(imageFile, 0, SEEK_END);
	size_t imageSize = ftell(imageFile);
	fseek(imageFile, 0, SEEK_SET);
	// Get the word aligned length.
	size_t imageAlignedSize = 0;
	if (imageSize % AUTH_IMAGE_ALIGNMENT)
	{
		imageAlignedSize = imageSize + (AUTH_IMAGE_ALIGNMENT - (imageSize % AUTH_IMAGE_ALIGNMENT));
	}
	else
	{
		imageAlignedSize = imageSize;
	}

	Log::log(Logger::INFO2, "\tSuccess.(Size = %#x, AlignedSize = %#x)\n", imageSize, imageAlignedSize);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Load the Root certificate files.
	////////////////////////////////////////////////////////////////////////////////////////////////////
	Log::log(Logger::INFO2, "3. Load the root certificates.\n");
	configdata = (char *)calloc(1, MAX_PATH);
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Load the count of root certificate files.
	////////////////////////////////////////////////////////////////////////////////////////////////////
	Log::log(Logger::INFO2, "%d.1 Load the count of root certificates.\n", step);
	int rootCertCount = conf.rootCertFiles.size();
	if ((rootCertCount > MAX_ROOT_CERT_COUNT) || (rootCertCount == 0))
	{
		Log::log(Logger::ERROR, "Invalid count of root certificates. Should between 1 - 4.\n");
		cleanUpK3Sboot(imageFile, usedRootCrtFile, outputFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
		return FAIL_RETURN;
	}
	Log::log(Logger::INFO2, "\tSuccess. (Root Certification Count = %d)\n", rootCertCount);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Load the Index of used root certificate files.
	////////////////////////////////////////////////////////////////////////////////////////////////////
	Log::log(Logger::INFO2, "%d.2 Load selected certificate chain id, used to sign this image.\n", step);
	uint32_t usedRootCertIndex = conf.mainCertId;
	if ((usedRootCertIndex >= MAX_ROOT_CERT_COUNT) || (usedRootCertIndex >= rootCertCount))
	{
		Log::log(Logger::INFO2, "Invalid index of selected certificate chain. Should between 0 to %d.\n", rootCertCount - 1);
		cleanUpK3Sboot(imageFile, usedRootCrtFile, outputFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
		return FAIL_RETURN;
	}
	Log::log(Logger::INFO2, "\tSuccess. (Selected certificatate chain index = %d)\n", usedRootCertIndex);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Load all the Root certificate files.
	////////////////////////////////////////////////////////////////////////////////////////////////////
	Log::log(Logger::INFO2, "%d.3 Load all root certificates.\n", step);
	rkh_table_t rkh;
	memset(&rkh, 0, sizeof(rkh_table_t));

	for (int i = 0; i < MAX_ROOT_CERT_COUNT; i++) {
		if (conf.rootCertFiles[i].size() == 0) {
			if (usedRootCertIndex == i)
			{
				Log::log(Logger::ERROR, "\tRoot certificate %d, which is part of selected certificate chain, cannot be empty.\n", i);
				cleanUpK3Sboot( imageFile, usedRootCrtFile, outputFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
				return FAIL_RETURN;
			}
			else
			{
				continue;
			}
		}

		const char* rootCertFile = (conf.rootCertFiles[i]).c_str();
		FILE *rootCrtFile = fopen(rootCertFile, "rb");
		if (rootCrtFile == NULL)
		{
			Log::log(Logger::INFO2, "\tCannot open root certificate file %d (%s).\n", i, rootCertFile);
			cleanUpK3Sboot(imageFile, usedRootCrtFile, outputFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
			return FAIL_RETURN;
		}
		fclose(rootCrtFile);
		if (conf.mainCertId == i)
		{
			usedRootCertPath = rootCertFile;
		}
		mbedtls_x509_crt rootCrt;
		mbedtls_x509_crt_init(&rootCrt);

		ret = mbedtls_x509_crt_parse_file(&rootCrt, rootCertFile);
		if (ret != 0)
		{
			Log::log(Logger::ERROR, "\tFailed to parse root certificate file %d(%s)(error code = %d).\nExpected X.509 certificate in DER format.\n", i, rootCertFile, ret);
			cleanUpK3Sboot(imageFile, usedRootCrtFile, outputFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
			return FAIL_RETURN;
		}

		uint32_t flags;
		ret = mbedtls_x509_crt_verify(&rootCrt, &rootCrt, NULL, NULL, &flags, NULL, NULL);
		if (ret != 0)
		{
			Log::log(Logger::ERROR, "\tRoot certificate %d must be selfsigned.\n", i);
			cleanUpK3Sboot(imageFile, usedRootCrtFile, outputFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
			return FAIL_RETURN;
		}
		else
		{
			Log::log(Logger::INFO2, "\tRoot certificate %d is self signed.\n", i);
		}

		size_t n_size = mbedtls_mpi_size(&((mbedtls_rsa_context *)rootCrt.pk.pk_ctx)->N);
		size_t e_size = mbedtls_mpi_size(&((mbedtls_rsa_context *)rootCrt.pk.pk_ctx)->E);
		uint8_t *buf = (uint8_t *)calloc(1, n_size + e_size);
		mbedtls_mpi_write_binary(&((mbedtls_rsa_context *)rootCrt.pk.pk_ctx)->N, buf, n_size);
		mbedtls_mpi_write_binary(&((mbedtls_rsa_context *)rootCrt.pk.pk_ctx)->E, buf + n_size, e_size);
		mbedtls_sha256(buf, n_size + e_size, rkh.entries[i].rkh, false);

		mbedtls_x509_crt_free(&rootCrt);
		Log::log(Logger::INFO2, "\tSuccess. (Root Certification %d = %s)\n", i, rootCertFile);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//  Calculate used root certificate size.
	////////////////////////////////////////////////////////////////////////////////////////////////////
	Log::log(Logger::INFO2, "%d.4 Calculate size of root certificates.\n", step);
	usedRootCrtFile = fopen(usedRootCertPath.c_str(), "rb");
	int usedRootCrtSize = 0, usedRootCrtAlignedSize = 0;
	if (usedRootCrtFile == NULL)
	{
		Log::log(Logger::ERROR, "\tCannot open the selected root certificate file %s.\n", usedRootCertPath.c_str());
		cleanUpK3Sboot(imageFile, usedRootCrtFile, outputFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
		return FAIL_RETURN;
	}
	// Get image length in bytes.
	fseek(usedRootCrtFile, 0, SEEK_END);
	usedRootCrtSize = ftell(usedRootCrtFile);
	fseek(usedRootCrtFile, 0, SEEK_SET);

	if (usedRootCrtSize % AUTH_IMAGE_ALIGNMENT)
	{
		usedRootCrtAlignedSize = usedRootCrtSize + (AUTH_IMAGE_ALIGNMENT - (usedRootCrtSize % AUTH_IMAGE_ALIGNMENT));
	}
	else
	{
		usedRootCrtAlignedSize = usedRootCrtSize;
	}

	Log::log(Logger::INFO2, "\tSuccess. (Root Certification Size = %lu bytes, Aligned Size = %d bytes)\n", usedRootCrtSize, usedRootCrtAlignedSize);
	step++;

	Log::log(Logger::INFO2, "\t Success.(Used Root Certification Size = %d, Aligned Size = %d)\n", usedRootCrtSize,	usedRootCrtAlignedSize);
	step++;
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Load all certificates.
	////////////////////////////////////////////////////////////////////////////////////////////////////
	Log::log(Logger::INFO2, "%d. Load all certificates in selected certificate chain.\n", step);
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Load the count of the certificates.
	////////////////////////////////////////////////////////////////////////////////////////////////////
	Log::log(Logger::INFO2, "%d.1 Load the count of chained certificates in selected certificate chain.\n", step);
	int certCount = conf.chainCertFiles.size();
	if (certCount < 0)
	{
		Log::log(Logger::ERROR, "Invalid count of the certificates chained in selected certificate chain.\n");
		cleanUpK3Sboot(imageFile, usedRootCrtFile, outputFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
		return FAIL_RETURN;
	}
	Log::log(Logger::INFO2, "\tSuccess. (Certificate count = %d)\n", certCount);
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Load all certificates.
	////////////////////////////////////////////////////////////////////////////////////////////////////
	Log::log(Logger::INFO2, "%d.2 Load and parse certificates in selected certificate chain.\n", step);
	trustCrt = (mbedtls_x509_crt *)malloc(sizeof(mbedtls_x509_crt));
	mbedtls_x509_crt_init(trustCrt);
	ret = mbedtls_x509_crt_parse_file(trustCrt, usedRootCertPath.c_str());
	if (ret != 0)
	{
		Log::log(Logger::ERROR, "\tFailed to parse selected root certificate file (%s)(error code = %d).\n", usedRootCertPath.c_str(), ret);
		cleanUpK3Sboot(imageFile, usedRootCrtFile, outputFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
		return FAIL_RETURN;
	}
	if (certCount > 0) {
		for (uint32_t i = 0; i < certCount; i++)
		{
			const char* chainCertFile = conf.chainCertFiles[i].c_str();
			FILE *crtFile = fopen(chainCertFile, "rb");
			if (crtFile == NULL)
			{
				Log::log(Logger::INFO2, "\tCannot open certificate file %d(%s)(error code = %d).\n", i, chainCertFile, ret);
				cleanUpK3Sboot(imageFile, usedRootCrtFile, outputFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
				return FAIL_RETURN;
			}
			fclose(crtFile);

			mbedtls_x509_crt *testCrt = (mbedtls_x509_crt *)malloc(sizeof(mbedtls_x509_crt));
			mbedtls_x509_crt_init(testCrt);
			ret = mbedtls_x509_crt_parse_file(testCrt, chainCertFile);
			if (ret != 0)
			{
				Log::log(Logger::ERROR, "\tFailed to parse certificate file %d(%s)(error code = %d).\n", i, chainCertFile, ret);
				cleanUpK3Sboot(imageFile, usedRootCrtFile, outputFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
				return FAIL_RETURN;
			}

			uint32_t flags;
			ret = mbedtls_x509_crt_verify(testCrt, trustCrt, NULL, NULL, &flags, NULL, NULL);
			if (ret != 0)
			{
				Log::log(Logger::ERROR, "\tVerify Certification chain failed at index %d(%s)(error code = %d).\n", i, chainCertFile, ret);
				cleanUpK3Sboot(imageFile, usedRootCrtFile, outputFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
				return FAIL_RETURN;
			}
			mbedtls_x509_crt_free(trustCrt);
			free(trustCrt);
			trustCrt = testCrt;
			testCrt = NULL;

			crtList.push_back(chainCertFile);
			Log::log(Logger::INFO2, "\tSuccess. ( Certification %d = %s)\n", i, chainCertFile);
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Calculate all the certificates size.
	////////////////////////////////////////////////////////////////////////////////////////////////////
	Log::log(Logger::INFO2, "%d.3 Calculate size of all chained certificates.\n", step);
	size_t crtFileTotalSize = 0;

	size_t i = 0;
	for (vector<string>::iterator it = crtList.begin(); it != crtList.end(); it++, i++)
	{
		FILE *crtFile = fopen(it->c_str(), "rb");
		if (crtFile == NULL)
		{
			Log::log(Logger::ERROR, "\tCannot open the used certificate file %s.\n", it->c_str());
			cleanUpK3Sboot(imageFile, usedRootCrtFile, outputFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
			return FAIL_RETURN;
		}

		fseek(crtFile, 0, SEEK_END);
		size_t crtFileSize = ftell(crtFile);
		fseek(crtFile, 0, SEEK_SET);
		fclose(crtFile);

		uint32_t crtFileAlignedSize;
		if (crtFileSize % AUTH_IMAGE_ALIGNMENT)
		{
			crtFileAlignedSize = crtFileSize + (AUTH_IMAGE_ALIGNMENT - (crtFileSize % AUTH_IMAGE_ALIGNMENT));
		}
		else
		{
			crtFileAlignedSize = crtFileSize;
		}

		crtFileTotalSize += crtFileAlignedSize;

		Log::log(Logger::INFO2, "\tSuccess. (Certification %d size = %lu bytes, Aligned size = %d bytes)\n", i, crtFileSize, crtFileAlignedSize);
	}
	Log::log(Logger::INFO2, "\tSuccess. (Certification total size = %lu bytes)\n", crtFileTotalSize);
	step++;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Generate the entire image data
	////////////////////////////////////////////////////////////////////////////////////////////////////
	Log::log(Logger::INFO2, "%d. Generate the entire image data.\n", step);

	// Calculate CRT table size and align it to word boundary.
	size_t crtTableSize = usedRootCrtAlignedSize + crtFileTotalSize + 4 * (certCount + 1 /*root*/);
	// Align the crt table size to word boundary.

	size_t crtTableAlignedSize = 0;
	if (crtTableSize % AUTH_IMAGE_ALIGNMENT)
	{
		crtTableAlignedSize = crtTableSize + (AUTH_IMAGE_ALIGNMENT - (crtTableSize % AUTH_IMAGE_ALIGNMENT));
	}
	else
	{
		crtTableAlignedSize = crtTableSize;
	}

	// Calculate entire image size£¬including image£¬ header and crt.
	size_t totalImageSize =
		imageAlignedSize + sizeof(certificate_block_header_t) + crtTableAlignedSize + sizeof(rkh_table_t);

	// Malloc a buffer to contain the entire image data.
	buffer = (uint8_t *)malloc(totalImageSize);
	// Init the buffer. padding bytes for the aligned are 0x00s.
	memset(buffer, 0, totalImageSize);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Cp the original image data
	////////////////////////////////////////////////////////////////////////////////////////////////////
	Log::log(Logger::INFO2, "%d.1 Copy the original image data.\n", step);
	// Load image file to buffer.
	fread(buffer, 1, imageSize, imageFile);

	// Modify the header offsize.
	uint32_t headerOffset = imageAlignedSize;
	*(uint32_t *)(buffer + 0x28) = headerOffset;
	Log::log(Logger::INFO2, "\tSuccess.\n");
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Init certificate header
	////////////////////////////////////////////////////////////////////////////////////////////////////
	Log::log(Logger::INFO2, "%d.2 Init certificate header.\n", step);
	certificate_block_header_t *header = (certificate_block_header_t *)(buffer + imageAlignedSize);

	header->signature = SIGNATURE_TAG;
	header->headerMajorVersion = 1;
	header->headerMinorVersion = 0;
	header->headerLengthInBytes = sizeof(certificate_block_header_t);
	header->flags = 0;
	header->buildNumber = FOUR_CHAR_CODE(1, 0, 0, 0);
	header->totalImageLengthInBytes = totalImageSize;
	header->certificateCount = 1 /*root cert*/ + certCount;
	header->certificateTableLengthInBytes = crtTableAlignedSize;

	uint32_t crtTableOffset = imageAlignedSize + sizeof(certificate_block_header_t);
	uint32_t crtOffset = crtTableOffset;
	Log::log(Logger::INFO2, "\tSuccess.\n");
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Add root certificate
	////////////////////////////////////////////////////////////////////////////////////////////////////
	Log::log(Logger::INFO2, "%d.3 Add root certificate.\n", step);
	*(uint32_t *)(buffer + crtOffset) = usedRootCrtAlignedSize;
	crtOffset += 4;
	fread(buffer + crtOffset, 1, usedRootCrtSize, usedRootCrtFile);
	crtOffset += usedRootCrtAlignedSize;
	Log::log(Logger::INFO2, "\tSuccess.\n");
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Add certificate
	////////////////////////////////////////////////////////////////////////////////////////////////////
	Log::log(Logger::INFO2, "%d.4 Add certificate.\n", step);
	for (std::vector<std::string>::iterator it = crtList.begin(); it != crtList.end(); it++, i++)
	{
		FILE *crtFile = fopen(it->c_str(), "rb");
		if (crtFile == NULL)
		{
			Log::log(Logger::INFO2, "\tCannot open the used certificate file %s.\n", usedRootCertPath.c_str());
			return FAIL_RETURN;			
		}
		fseek(crtFile, 0, SEEK_END);
		size_t crtFileSize = ftell(crtFile);
		fseek(crtFile, 0, SEEK_SET);

		uint32_t crtFileAlignedSize;
		if (crtFileSize % AUTH_IMAGE_ALIGNMENT)
		{
			crtFileAlignedSize = crtFileSize + (AUTH_IMAGE_ALIGNMENT - (crtFileSize % AUTH_IMAGE_ALIGNMENT));
		}
		else
		{
			crtFileAlignedSize = crtFileSize;
		}

		*(uint32_t *)(buffer + crtOffset) = crtFileAlignedSize;
		crtOffset += 4;
		fread(buffer + crtOffset, 1, crtFileSize, crtFile);
		crtOffset += crtFileAlignedSize;
	}

	// crtOffset = crtOffset + (AUTH_IMAGE_ALIGNMENT - (crtOffset % AUTH_IMAGE_ALIGNMENT));

	if ((crtOffset - crtTableOffset) != crtTableSize)
	{
		Log::log(Logger::INFO2, "\tgenerate certificate table failed. table size is %d, but should be %d.\n", crtOffset - crtTableOffset,
			crtTableSize);
		return FAIL_RETURN;		
	}
	Log::log(Logger::INFO2, "\tSuccess.(Certification table size = %d)\n", crtOffset - crtTableOffset);
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Add root hash key table
	////////////////////////////////////////////////////////////////////////////////////////////////////
	Log::log(Logger::INFO2, "%d.5 Add root hash key table.\n", step);
	uint32_t rkhOffset = crtOffset;
	memcpy(buffer + rkhOffset, &rkh, sizeof(rkh_table_t));
	Log::log(Logger::INFO2, "\tSuccess.\n");
	step++;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Load and parse the private key.
	////////////////////////////////////////////////////////////////////////////////////////////////////
	Log::log(Logger::INFO2, "%d. Load and parse the private key of certificate used for signing of output image.\n", step);
	mbedtls_pk_context pk_ctx;
	mbedtls_pk_init(&pk_ctx);
	if ((ret = mbedtls_pk_parse_keyfile(&pk_ctx, conf.mainCertPrivateKeyFile.c_str(), NULL)) != 0)
	{
		Log::log(Logger::ERROR, "Parse private key failed.(%d)(%s)\n", ret, conf.mainCertPrivateKeyFile.c_str());
		cleanUpK3Sboot(imageFile, usedRootCrtFile, outputFile, buffer, signature, trustCrt, rsa_priv_key_ctx);
		return FAIL_RETURN;
	}
	// Get the private key.
	rsa_priv_key_ctx = (mbedtls_rsa_context *)(pk_ctx.pk_ctx);
	Log::log(Logger::INFO2, "\tSuccess. (Private Key File Path = %s\n", conf.mainCertPrivateKeyFile.c_str());
	uint32_t signatureSizeInBytes = rsa_priv_key_ctx->len;
	step++;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Generate the digest.
	////////////////////////////////////////////////////////////////////////////////////////////////////
	Log::log(Logger::INFO2, "%d. Generate ouput image digest.\n", step);
	// Buffer contains digest.
	uint8_t digest[32] = { 0 };

	// Call mbedtls to calculate digest.
	mbedtls_sha256_context sha256_ctx;
	mbedtls_sha256_init(&sha256_ctx);
	mbedtls_sha256_starts(&sha256_ctx, false /*isSHA224 = false*/);
	mbedtls_sha256_update(&sha256_ctx, buffer, totalImageSize);
	mbedtls_sha256_finish(&sha256_ctx, digest);

	Log::log(Logger::INFO2, "\tSuccess.\n");
	Log::log(Logger::INFO2, "\tSHA256 digest:");
	for (size_t i = 0; i < 32; i++)
	{
		Log::log(Logger::INFO2, " %#x", digest[i]);
	}
	Log::log(Logger::INFO2, "\n");
	step++;
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Generate the signature.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Log::log(Logger::INFO2, "%d. Generate output image signature.\n", step);
	// Buffer contains signature.
	signatureSizeInBytes = rsa_priv_key_ctx->len;
	signature = (uint8_t *)malloc(signatureSizeInBytes);
	memset(signature, 0, signatureSizeInBytes);

	// Init deterministic random bit generator.
	mbedtls_ctr_drbg_context ctr_drbg;
	mbedtls_ctr_drbg_init(&ctr_drbg);
	// Init entropy.
	mbedtls_entropy_context entropy;
	mbedtls_entropy_init(&entropy);

	ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, NULL, 0);
	if (ret != 0)
	{
		Log::log(Logger::INFO2, "Seed random bit generator failed.%d\n", ret);
		return FAIL_RETURN;		
	}

	ret = mbedtls_rsa_rsassa_pkcs1_v15_sign(rsa_priv_key_ctx, mbedtls_ctr_drbg_random, &ctr_drbg, MBEDTLS_RSA_PRIVATE,
		MBEDTLS_MD_SHA256, 256, digest, signature);
	if (ret != 0)
	{
		Log::log(Logger::INFO2, "Generate signature failed.(%d)\n", ret);
		return FAIL_RETURN;		
	}

	Log::log(Logger::INFO2, "\tSuccess.\n");
	Log::log(Logger::INFO2, "\tSignature:");
	for (size_t i = 0; i < signatureSizeInBytes; i++)
	{
		Log::log(Logger::INFO2, " %#x", signature[i]);
	}
	Log::log(Logger::INFO2, "\n");
	step++;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Verify the signature.
	////////////////////////////////////////////////////////////////////////////////////////////////////
	Log::log(Logger::INFO2, "%d. Verify signature.\n", step);

	// ret = mbedtls_rsa_rsassa_pkcs1_v15_verify(rsa_pub_key_ctx, mbedtls_ctr_drbg_random, &ctr_drbg,
	// MBEDTLS_RSA_PUBLIC,
	//                                          MBEDTLS_MD_SHA256, 256, digest, signature);
	mbedtls_rsa_context *rsa_pub_key_ctx = (mbedtls_rsa_context *)(trustCrt->pk.pk_ctx);
	ret = mbedtls_rsa_rsassa_pkcs1_v15_verify(rsa_pub_key_ctx, NULL, NULL, MBEDTLS_RSA_PUBLIC, MBEDTLS_MD_SHA256, 256,
		digest, signature);
	if (ret != 0)
	{
		Log::log(Logger::INFO2, "Verify signature failed.(%d)\n", ret);
		return FAIL_RETURN;		
	}

	Log::log(Logger::INFO2, "\tSuccess.\n");
	step++;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Output the root key hash table SHA256 hash.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Log::log(Logger::INFO2, "%d. Output the root certificates SHA256 hash (RKTH).\n", step);

	uint8_t rkhtHash[32];
	mbedtls_sha256_context rkhtHash_sha256_ctx;
	mbedtls_sha256_init(&rkhtHash_sha256_ctx);
	mbedtls_sha256_starts(&rkhtHash_sha256_ctx, false /*isSHA224 = false*/);
	mbedtls_sha256_update(&rkhtHash_sha256_ctx, (const unsigned char *)(buffer + rkhOffset), sizeof(rkh_table_t));
	mbedtls_sha256_finish(&rkhtHash_sha256_ctx, (uint8_t *)rkhtHash);

	Log::log(Logger::INFO2, "\tSuccess.\n");
	Log::log(Logger::INFO, "\tRKTH: ");
	for (size_t i = 0; i < 32; i++)
	{
		Log::log(Logger::INFO, "%02x", rkhtHash[i]);
	}
	Log::log(Logger::INFO, "\n");
	step++;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Output the authenticated image.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Log::log(Logger::INFO2, "%d. Output the authenticated image.\n", step);

	outputFile = fopen(conf.masterBootOutputFile.c_str(), "wb");
	if (outputFile == NULL)
	{
		Log::log(Logger::INFO2, "\tCannot open/create output file %s.\n", conf.masterBootOutputFile.c_str());
		return FAIL_RETURN;		
	}

	ret = fwrite(buffer, 1, totalImageSize, outputFile);
	if (ret != totalImageSize)
	{
		Log::log(Logger::INFO2, "\tWrite total image data to output file failed..\n");
		return FAIL_RETURN;		
	}

	ret = fwrite(signature, 1, signatureSizeInBytes, outputFile);
	if (ret != signatureSizeInBytes)
	{
		Log::log(Logger::INFO2, "\tWrite signature to output file failed..\n");
		return FAIL_RETURN;		
	}
	Log::log(Logger::INFO, "\tSuccess. (Signed image %s created.)\n", conf.masterBootOutputFile.c_str());
	return SUCCESS_RETURN;
}

void AuthImageGenerator::cleanUpK3Sboot(FILE * imageFile, FILE * usedRootCrtFile, FILE * outputFile, uint8_t * buffer, uint8_t * signature, mbedtls_x509_crt * trustCrt, mbedtls_rsa_context * rsa_priv_key_ctx)
{
	if (imageFile != NULL)
	{
		fclose(imageFile);
	}
	if (usedRootCrtFile != NULL)
	{
		fclose(usedRootCrtFile);
	}
	if (outputFile != NULL)
	{
		fclose(outputFile);
	}
	if (buffer != NULL)
	{
		free(buffer);
	}
	if (signature != NULL)
	{
		free(signature);
	}
	if (trustCrt != NULL)
	{
		free(trustCrt);
	}
	if (rsa_priv_key_ctx != NULL)
	{
		free(rsa_priv_key_ctx);
	}
}

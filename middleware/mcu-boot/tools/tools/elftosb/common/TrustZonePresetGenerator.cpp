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

#include <algorithm>
#include <vector>
#include "Logging.h"
#include <stdlib.h>
#include "TrustZonePresetGenerator.h"
#include "AuthImageGenerator.h"

using namespace std;

	TrustZonePresetGenerator::TrustZonePresetGenerator() {}

	unique_ptr<vector<uint32_t>> TrustZonePresetGenerator::getTzpPreset(const string& confFilePath, const string& family) {
		jute::jValue tzp = parseFile(confFilePath);
		if (tzp.get_type() == jute::JUNKNOWN)
			throw runtime_error(("Parsing of configuration file " + confFilePath + " failed. Please see the previous output to identify possible problem.\nUse some json validator to help with json formating problems.\n").c_str());
		if (this->family != family)
			throw runtime_error(("Master boot image generated for " + family + " and TZ-Preset configuration provided for " + this->family + "." ).c_str());
		return generateTzp(tzp, confFilePath);
	}

	void TrustZonePresetGenerator::execute(const string& confFilePath) {
		if (process(confFilePath)) {
			Log::log(Logger::INFO2, "\tProcessing trust zone preset file failed. See the previous output to track possible problem.\n");
			return;
		}
		Log::log(Logger::INFO, "\tSuccess.\n");
	}

	bool TrustZonePresetGenerator::process(const string& confFilePath)
	{
		jute::jValue tzp = parseFile(confFilePath);
		if (tzp.get_type() == jute::JUNKNOWN)
			throw runtime_error(("Parsing of configuration file " + confFilePath + " failed. Please see the previous output to identify possible problem.\nUse some json validator to help with json formating problems.\n").c_str());
		if (saveFile(generateTzp(tzp, confFilePath)))
			throw runtime_error(("Saving of trust zone preset file " + outputFile + " failed.\n").c_str());
		return false;
	}

	bool TrustZonePresetGenerator::saveFile(unique_ptr<vector<uint32_t>> buffer)
	{
		Log::log(Logger::INFO2, "Saving trust zone preset file.\n");
		ofstream outputStream(outputFile, std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
		if (!outputStream)
			return true;
		for (auto const& value : *buffer) {
			outputStream.write(reinterpret_cast<const char *>(&value), 4);
		}
		if (!outputStream)
			return true;
		outputStream.close();
		Log::log(Logger::INFO2, "\tFile "+ outputFile + " created.\n");
		Log::log(Logger::INFO2, "\tSuccess.\n");
		return false;
	}

	unique_ptr<vector<uint32_t>> TrustZonePresetGenerator::generateTzp(jute::jValue tzp, const string& confFilePath) {
		auto vector = getVector(family);
		auto tzpBuffer = std::vector<uint32_t>();
		auto fetched = 0;
		Log::log(Logger::INFO2, "Processing trust zone preset data.\n");
		for (std::vector<pair<string, uint32_t>>::iterator it = vector->begin(); it != vector->end(); ++it) {
			jute::jValue jvalue = tzp[it->first];
			string hexValue;
			stringstream stringStr("");
			if (jvalue.get_type() == jute::JSTRING) {
				hexValue = jvalue.as_string();
				try {
					it->second = stoul(hexValue, nullptr, 16);
					fetched++;
				}
				catch (invalid_argument) {
					string ss = "\tCannot parse \"" + it->first + "\" value: " + hexValue + " from configuration file: " + confFilePath + ".\n";
					ss + "\tExpected hexadecimal string in format \"0xHEXVALUE\" or only \"HEXVALUE\".\n";
					Log::log(Logger::ERROR, ss);
				}
				catch (out_of_range) {
					stringstream ss("");
					ss << "\tValue of \"" << it->first << "\" is too big: " << hexValue << " from configuration file: " << confFilePath << ".\n";
					ss << "\tUse max value as 0xffffffff. Using default value 0x" << std::hex << it->second << ".\n";
					Log::log(Logger::ERROR, ss.str());
				}
			}
			stringStr << "\t" << it->first << " : 0x" << std::hex << it->second << endl;
			Log::log(Logger::INFO2, stringStr.str());
			tzpBuffer.push_back(it->second);
		}
		Log::log(Logger::INFO2, (string)"\tTotal Registers: " + to_string(vector->size()) + ".\n");
		Log::log(Logger::INFO2, "\tRegisters values fetched from conf file: " + to_string(fetched) + ".\n");
		Log::log(Logger::INFO2, "\tSuccess.\n");
		return std::make_unique<std::vector<uint32_t>>(tzpBuffer);
	}
	jute::jValue TrustZonePresetGenerator::parseFile(const string& confFilePath)
	{
		Log::log(Logger::INFO2, "Parsing trust zone json configuration file.\n");
		jute::jValue jsonConf;
		try {
			jsonConf = jute::parser::parse_file(confFilePath);
		}
		catch (runtime_error &e) {
			throw runtime_error(("Cannot parse json configuration file: " + confFilePath + " - " + e.what()).c_str());
		}

		bool error = false;
		if (jsonConf["family"].get_type() == jute::JSTRING) {
			family = jsonConf["family"].as_string();
			transform(family.begin(), family.end(), family.begin(), ::tolower);
			if (family != LPC55XX && family != RT6XX) {
				string ss = "\tUnexpected \"family\" value (" + family + ") from configuration file: " + confFilePath + ".\n";
				ss += (string)"\tSupported only " + LPC55XX + " and " + RT6XX + ".\n";
				Log::log(Logger::ERROR, ss);
				error = true;
			}
			else
				Log::log(Logger::INFO2, "\t\"family\": " + family + ".\n");
		}
		else {
			string ss = "\tCannot read \"family\" from configuration file: " + confFilePath + ".\n";
			Log::log(Logger::ERROR, ss);
			error = true;
		}

		if (jsonConf["tzpOutputFile"].get_type() == jute::JSTRING) {
			outputFile = jsonConf["tzpOutputFile"].as_string();
			Log::log(Logger::INFO2, "\t\"tzpOutputFile\": " + outputFile + ".\n");
		}
		else {
			string ss = "\tCannot read \"tzpOutputFile\" from configuration file: " + confFilePath + ".\n";
			Log::log(Logger::ERROR, ss);
			error = true;
		}

		if (jsonConf["trustZonePreset"].get_type() != jute::JOBJECT){
			string ss = "\tCannot read \"trustZonePreset\" object from configuration file: " + confFilePath + ".\n";
			Log::log(Logger::ERROR, ss);
			error = true;
		}

		if (error)
			return jute::jValue(jute::JUNKNOWN);
		Log::log(Logger::INFO2, "\tSuccess.\n");
		return jsonConf["trustZonePreset"];
	}

	std::vector<pair<string, uint32_t>>* TrustZonePresetGenerator::getVector(const string family) {
		if (family == LPC55XX)
			return getLpc55xxVector();
		else if (family == RT6XX)
			return getRt6xxVector();
		else
			throw runtime_error("Not supported device family: " + family + ".");
	}
	std::vector<pair<string, uint32_t>>* TrustZonePresetGenerator::getRt6xxVector() {
		auto vector = new std::vector<pair<string, uint32_t>>();
		vector->push_back(make_pair("Secure vector table address (vtor_addr)", 0x0u));
		vector->push_back(make_pair("Non-secure vector table address (vtor_ns_addr)", 0x0u));
		vector->push_back(make_pair("Interrupt target non-secure register 0 (nvic_itns0)", 0x0u));
		vector->push_back(make_pair("Interrupt target non-secure register 1 (nvic_itns1)", 0x0u));
		vector->push_back(make_pair("MPU Control Register (mpu_ctrl)", 0x0u));
		vector->push_back(make_pair("MPU Memory Attribute Indirection Register 0 (mpu_mair0)", 0x0u));
		vector->push_back(make_pair("MPU Memory Attribute Indirection Register 1 (mpu_mair1)", 0x0u));
		vector->push_back(make_pair("MPU Region 0 Base Address Register (mpu_rbar0)", 0x0u));
		vector->push_back(make_pair("MPU Region 0 Limit Address Register (mpu_rlar0)", 0x0u));
		vector->push_back(make_pair("MPU Region 1 Base Address Register (mpu_rbar1)", 0x0u));
		vector->push_back(make_pair("MPU Region 1 Limit Address Register (mpu_rlar1)", 0x0u));
		vector->push_back(make_pair("MPU Region 2 Base Address Register (mpu_rbar2)", 0x0u));
		vector->push_back(make_pair("MPU Region 2 Limit Address Register (mpu_rlar2)", 0x0u));
		vector->push_back(make_pair("MPU Region 3 Base Address Register (mpu_rbar3)", 0x0u));
		vector->push_back(make_pair("MPU Region 3 Limit Address Register (mpu_rlar3)", 0x0u));
		vector->push_back(make_pair("MPU Region 4 Base Address Register (mpu_rbar4)", 0x0u));
		vector->push_back(make_pair("MPU Region 4 Limit Address Register (mpu_rlar4)", 0x0u));
		vector->push_back(make_pair("MPU Region 5 Base Address Register (mpu_rbar5)", 0x0u));
		vector->push_back(make_pair("MPU Region 5 Limit Address Register (mpu_rlar5)", 0x0u));
		vector->push_back(make_pair("MPU Region 6 Base Address Register (mpu_rbar6)", 0x0u));
		vector->push_back(make_pair("MPU Region 6 Limit Address Register (mpu_rlar6)", 0x0u));
		vector->push_back(make_pair("MPU Region 7 Base Address Register (mpu_rbar7)", 0x0u));
		vector->push_back(make_pair("MPU Region 7 Limit Address Register (mpu_rlar7)", 0x0u));
		vector->push_back(make_pair("Non-secure MPU Control Register(mpu_ctrl_ns)", 0x0u));
		vector->push_back(make_pair("Non-secure MPU Memory Attribute Indirection Register 0 (mpu_mair0_ns)", 0x0u));
		vector->push_back(make_pair("Non-secure MPU Memory Attribute Indirection Register 1 (mpu_mair1_ns)", 0x0u));
		vector->push_back(make_pair("Non-secure MPU Region 0 Base Address Register (mpu_rbar0_ns)", 0x0u));
		vector->push_back(make_pair("Non-secure MPU Region 0 Limit Address Register (mpu_rlar0_ns)", 0x0u));
		vector->push_back(make_pair("Non-secure MPU Region 1 Base Address Register (mpu_rbar1_ns)", 0x0u));
		vector->push_back(make_pair("Non-secure MPU Region 1 Limit Address Register (mpu_rlar1_ns)", 0x0u));
		vector->push_back(make_pair("Non-secure MPU Region 2 Base Address Register (mpu_rbar2_ns)", 0x0u));
		vector->push_back(make_pair("Non-secure MPU Region 2 Limit Address Register (mpu_rlar2_ns)", 0x0u));
		vector->push_back(make_pair("Non-secure MPU Region 3 Base Address Register (mpu_rbar3_ns)", 0x0u));
		vector->push_back(make_pair("Non-secure MPU Region 3 Limit Address Register (mpu_rlar3_ns)", 0x0u));
		vector->push_back(make_pair("Non-secure MPU Region 4 Base Address Register (mpu_rbar4_ns)", 0x0u));
		vector->push_back(make_pair("Non-secure MPU Region 4 Limit Address Register (mpu_rlar4_ns)", 0x0u));
		vector->push_back(make_pair("Non-secure MPU Region 5 Base Address Register (mpu_rbar5_ns)", 0x0u));
		vector->push_back(make_pair("Non-secure MPU Region 5 Limit Address Register (mpu_rlar5_ns)", 0x0u));
		vector->push_back(make_pair("Non-secure MPU Region 6 Base Address Register (mpu_rbar6_ns)", 0x0u));
		vector->push_back(make_pair("Non-secure MPU Region 6 Limit Address Register (mpu_rlar6_ns)", 0x0u));
		vector->push_back(make_pair("Non-secure MPU Region 7 Base Address Register (mpu_rbar7_ns)", 0x0u));
		vector->push_back(make_pair("Non-secure MPU Region 7 Limit Address Register (mpu_rlar7_ns)", 0x0u));
		vector->push_back(make_pair("SAU Control Register.(sau_ctrl)", 0x0u));
		vector->push_back(make_pair("SAU Region 0 Base Address Register (sau_rbar0)", 0x0u));
		vector->push_back(make_pair("SAU Region 0 Limit Address Register (sau_rlar0)", 0x0u));
		vector->push_back(make_pair("SAU Region 1 Base Address Register (sau_rbar1)", 0x0u));
		vector->push_back(make_pair("SAU Region 1 Limit Address Register (sau_rlar1)", 0x0u));
		vector->push_back(make_pair("SAU Region 2 Base Address Register (sau_rbar2)", 0x0u));
		vector->push_back(make_pair("SAU Region 2 Limit Address Register (sau_rlar2)", 0x0u));
		vector->push_back(make_pair("SAU Region 3 Base Address Register (sau_rbar3)", 0x0u));
		vector->push_back(make_pair("SAU Region 3 Limit Address Register (sau_rlar3)", 0x0u));
		vector->push_back(make_pair("SAU Region 4 Base Address Register (sau_rbar4)", 0x0u));
		vector->push_back(make_pair("SAU Region 4 Limit Address Register (sau_rlar4)", 0x0u));
		vector->push_back(make_pair("SAU Region 5 Base Address Register (sau_rbar5)", 0x0u));
		vector->push_back(make_pair("SAU Region 5 Limit Address Register (sau_rlar5)", 0x0u));
		vector->push_back(make_pair("SAU Region 6 Base Address Register (sau_rbar6)", 0x0u));
		vector->push_back(make_pair("SAU Region 6 Limit Address Register (sau_rlar6)", 0x0u));
		vector->push_back(make_pair("SAU Region 7 Base Address Register (sau_rbar7)", 0x0u));
		vector->push_back(make_pair("SAU Region 7 Limit Address Register (sau_rlar7)", 0x0u));
		vector->push_back(make_pair("ROM Slave Rule Register 0 (bootrom0_slave_rule0)", 0x0u));
		vector->push_back(make_pair("ROM Memory Rule Register 0 (bootrom0_mem_rule0)", 0x0u));
		vector->push_back(make_pair("ROM Memory Rule Register 1 (bootrom0_mem_rule1)", 0x0u));
		vector->push_back(make_pair("ROM Memory Rule Register 2 (bootrom0_mem_rule2)", 0x0u));
		vector->push_back(make_pair("ROM Memory Rule Register 3 (bootrom0_mem_rule3)", 0x0u));
		vector->push_back(make_pair("Quad/Octal SPI Slave Rule Register 0 (qospi_slave_rule0)", 0x0u));
		vector->push_back(make_pair("Quad/Octal SPI 0 Memory Rule Register 0 (qospi0_mem_rule0)", 0x0u));
		vector->push_back(make_pair("Quad/Octal SPI 0 Memory Rule Register 1 (qospi0_mem_rule1)", 0x0u));
		vector->push_back(make_pair("Quad/Octal SPI 0 Memory Rule Register 2 (qospi0_mem_rule2)", 0x0u));
		vector->push_back(make_pair("Quad/Octal SPI 0 Memory Rule Register 3 (qospi0_mem_rule3)", 0x0u));
		vector->push_back(make_pair("Quad/Octal SPI 1 Memory Rule Register 0 (qospi1_mem_rule0)", 0x0u));
		vector->push_back(make_pair("Quad/Octal SPI 1 Memory Rule Register 1 (qospi1_mem_rule1)", 0x0u));
		vector->push_back(make_pair("Quad/Octal SPI 1 Memory Rule Register 2 (qospi1_mem_rule2)", 0x0u));
		vector->push_back(make_pair("Quad/Octal SPI 1 Memory Rule Register 3 (qospi1_mem_rule3)", 0x0u));
		vector->push_back(make_pair("Quad/Octal SPI 2 Memory Rule Register 0 (qospi2_mem_rule0)", 0x0u));
		vector->push_back(make_pair("Quad/Octal SPI 2 Memory Rule Register 1 (qospi2_mem_rule1)", 0x0u));
		vector->push_back(make_pair("Quad/Octal SPI 2 Memory Rule Register 2 (qospi2_mem_rule2)", 0x0u));
		vector->push_back(make_pair("Quad/Octal SPI 2 Memory Rule Register 3 (qospi2_mem_rule3)", 0x0u));
		vector->push_back(make_pair("Quad/Octal SPI 3 Memory Rule Register 0 (qospi3_mem_rule0)", 0x0u));
		vector->push_back(make_pair("Quad/Octal SPI 3 Memory Rule Register 1 (qospi3_mem_rule1)", 0x0u));
		vector->push_back(make_pair("Quad/Octal SPI 3 Memory Rule Register 2 (qospi3_mem_rule2)", 0x0u));
		vector->push_back(make_pair("Quad/Octal SPI 3 Memory Rule Register 3 (qospi3_mem_rule3)", 0x0u));
		vector->push_back(make_pair("RAM0 Slave Rule Register (ram0_slave_rule)", 0x0u));
		vector->push_back(make_pair("RAM00 Memory Rule Register 0 (ram00_mem_rule0)", 0x0u));
		vector->push_back(make_pair("RAM00 Memory Rule Register 1 (ram00_mem_rule1)", 0x0u));
		vector->push_back(make_pair("RAM00 Memory Rule Register 2 (ram00_mem_rule2)", 0x0u));
		vector->push_back(make_pair("RAM00 Memory Rule Register 3 (ram00_mem_rule3)", 0x0u));
		vector->push_back(make_pair("RAM01 Memory Rule Register 0 (ram01_mem_rule0)", 0x0u));
		vector->push_back(make_pair("RAM01 Memory Rule Register 1 (ram01_mem_rule1)", 0x0u));
		vector->push_back(make_pair("RAM01 Memory Rule Register 2 (ram01_mem_rule2)", 0x0u));
		vector->push_back(make_pair("RAM01 Memory Rule Register 3 (ram01_mem_rule3)", 0x0u));
		vector->push_back(make_pair("RAM1 Slave Rule Register (ram1_slave_rule)", 0x0u));
		vector->push_back(make_pair("RAM10 Memory Rule Register 0 (ram10_mem_rule0)", 0x0u));
		vector->push_back(make_pair("RAM10 Memory Rule Register 1 (ram10_mem_rule1)", 0x0u));
		vector->push_back(make_pair("RAM10 Memory Rule Register 2 (ram10_mem_rule2)", 0x0u));
		vector->push_back(make_pair("RAM10 Memory Rule Register 3 (ram10_mem_rule3)", 0x0u));
		vector->push_back(make_pair("RAM11 Memory Rule Register 0 (ram11_mem_rule0)", 0x0u));
		vector->push_back(make_pair("RAM11 Memory Rule Register 1 (ram11_mem_rule1)", 0x0u));
		vector->push_back(make_pair("RAM11 Memory Rule Register 2 (ram11_mem_rule2)", 0x0u));
		vector->push_back(make_pair("RAM11 Memory Rule Register 3 (ram11_mem_rule3)", 0x0u));
		vector->push_back(make_pair("RAM2 Slave Rule Register (ram2_slave_rule)", 0x0u));
		vector->push_back(make_pair("RAM20 Memory Rule Register 0 (ram20_mem_rule0)", 0x0u));
		vector->push_back(make_pair("RAM20 Memory Rule Register 1 (ram20_mem_rule1)", 0x0u));
		vector->push_back(make_pair("RAM20 Memory Rule Register 2 (ram20_mem_rule2)", 0x0u));
		vector->push_back(make_pair("RAM20 Memory Rule Register 3 (ram20_mem_rule3)", 0x0u));
		vector->push_back(make_pair("RAM21 Memory Rule Register 0 (ram21_mem_rule0)", 0x0u));
		vector->push_back(make_pair("RAM21 Memory Rule Register 1 (ram21_mem_rule1)", 0x0u));
		vector->push_back(make_pair("RAM21 Memory Rule Register 2 (ram21_mem_rule2)", 0x0u));
		vector->push_back(make_pair("RAM21 Memory Rule Register 3 (ram21_mem_rule3)", 0x0u));
		vector->push_back(make_pair("RAM22 Memory Rule Register 0 (ram22_mem_rule0)", 0x0u));
		vector->push_back(make_pair("RAM22 Memory Rule Register 1 (ram22_mem_rule1)", 0x0u));
		vector->push_back(make_pair("RAM22 Memory Rule Register 2 (ram22_mem_rule2)", 0x0u));
		vector->push_back(make_pair("RAM22 Memory Rule Register 3 (ram22_mem_rule3)", 0x0u));
		vector->push_back(make_pair("RAM23 Memory Rule Register 0 (ram23_mem_rule0)", 0x0u));
		vector->push_back(make_pair("RAM23 Memory Rule Register 1 (ram23_mem_rule1)", 0x0u));
		vector->push_back(make_pair("RAM23 Memory Rule Register 2 (ram23_mem_rule2)", 0x0u));
		vector->push_back(make_pair("RAM23 Memory Rule Register 3 (ram23_mem_rule3)", 0x0u));
		vector->push_back(make_pair("RAM3 Slave Rule Register (ram3_slave_rule)", 0x0u));
		vector->push_back(make_pair("RAM30 Memory Rule Register 0 (ram30_mem_rule0)", 0x0u));
		vector->push_back(make_pair("RAM30 Memory Rule Register 1 (ram30_mem_rule1)", 0x0u));
		vector->push_back(make_pair("RAM30 Memory Rule Register 2 (ram30_mem_rule2)", 0x0u));
		vector->push_back(make_pair("RAM30 Memory Rule Register 3 (ram30_mem_rule3)", 0x0u));
		vector->push_back(make_pair("RAM31 Memory Rule Register 0 (ram31_mem_rule0)", 0x0u));
		vector->push_back(make_pair("RAM31 Memory Rule Register 1 (ram31_mem_rule1)", 0x0u));
		vector->push_back(make_pair("RAM31 Memory Rule Register 2 (ram31_mem_rule2)", 0x0u));
		vector->push_back(make_pair("RAM31 Memory Rule Register 3 (ram31_mem_rule3)", 0x0u));
		vector->push_back(make_pair("RAM32 Memory Rule Register 0 (ram32_mem_rule0)", 0x0u));
		vector->push_back(make_pair("RAM32 Memory Rule Register 1 (ram32_mem_rule1)", 0x0u));
		vector->push_back(make_pair("RAM32 Memory Rule Register 2 (ram32_mem_rule2)", 0x0u));
		vector->push_back(make_pair("RAM32 Memory Rule Register 3 (ram32_mem_rule3)", 0x0u));
		vector->push_back(make_pair("RAM33 Memory Rule Register 0 (ram33_mem_rule0)", 0x0u));
		vector->push_back(make_pair("RAM33 Memory Rule Register 1 (ram33_mem_rule1)", 0x0u));
		vector->push_back(make_pair("RAM33 Memory Rule Register 2 (ram33_mem_rule2)", 0x0u));
		vector->push_back(make_pair("RAM33 Memory Rule Register 3 (ram33_mem_rule3)", 0x0u));
		vector->push_back(make_pair("RAM4 Slave Rule Register (ram4_slave_rule)", 0x0u));
		vector->push_back(make_pair("RAM40 Memory Rule Register 0 (ram40_mem_rule0)", 0x0u));
		vector->push_back(make_pair("RAM40 Memory Rule Register 1 (ram40_mem_rule1)", 0x0u));
		vector->push_back(make_pair("RAM40 Memory Rule Register 2 (ram40_mem_rule2)", 0x0u));
		vector->push_back(make_pair("RAM40 Memory Rule Register 3 (ram40_mem_rule3)", 0x0u));
		vector->push_back(make_pair("RAM41 Memory Rule Register 0 (ram41_mem_rule0)", 0x0u));
		vector->push_back(make_pair("RAM41 Memory Rule Register 1 (ram41_mem_rule1)", 0x0u));
		vector->push_back(make_pair("RAM41 Memory Rule Register 2 (ram41_mem_rule2)", 0x0u));
		vector->push_back(make_pair("RAM41 Memory Rule Register 3 (ram41_mem_rule3)", 0x0u));
		vector->push_back(make_pair("RAM42 Memory Rule Register 0 (ram42_mem_rule0)", 0x0u));
		vector->push_back(make_pair("RAM42 Memory Rule Register 1 (ram42_mem_rule1)", 0x0u));
		vector->push_back(make_pair("RAM42 Memory Rule Register 2 (ram42_mem_rule2)", 0x0u));
		vector->push_back(make_pair("RAM42 Memory Rule Register 3 (ram42_mem_rule3)", 0x0u));
		vector->push_back(make_pair("RAM43 Memory Rule Register 0 (ram43_mem_rule0)", 0x0u));
		vector->push_back(make_pair("RAM43 Memory Rule Register 1 (ram43_mem_rule1)", 0x0u));
		vector->push_back(make_pair("RAM43 Memory Rule Register 2 (ram43_mem_rule2)", 0x0u));
		vector->push_back(make_pair("RAM43 Memory Rule Register 3 (ram43_mem_rule3)", 0x0u));
		vector->push_back(make_pair("RAM5 Slave Rule Register (ram5_slave_rule)", 0x0u));
		vector->push_back(make_pair("RAM50 Memory Rule Register 0 (ram50_mem_rule0)", 0x0u));
		vector->push_back(make_pair("RAM50 Memory Rule Register 1 (ram50_mem_rule1)", 0x0u));
		vector->push_back(make_pair("RAM50 Memory Rule Register 2 (ram50_mem_rule2)", 0x0u));
		vector->push_back(make_pair("RAM50 Memory Rule Register 3 (ram50_mem_rule3)", 0x0u));
		vector->push_back(make_pair("RAM51 Memory Rule Register 0 (ram51_mem_rule0)", 0x0u));
		vector->push_back(make_pair("RAM51 Memory Rule Register 1 (ram51_mem_rule1)", 0x0u));
		vector->push_back(make_pair("RAM51 Memory Rule Register 2 (ram51_mem_rule2)", 0x0u));
		vector->push_back(make_pair("RAM51 Memory Rule Register 3 (ram51_mem_rule3)", 0x0u));
		vector->push_back(make_pair("RAM52 Memory Rule Register 0 (ram52_mem_rule0)", 0x0u));
		vector->push_back(make_pair("RAM52 Memory Rule Register 1 (ram52_mem_rule1)", 0x0u));
		vector->push_back(make_pair("RAM52 Memory Rule Register 2 (ram52_mem_rule2)", 0x0u));
		vector->push_back(make_pair("RAM52 Memory Rule Register 3 (ram52_mem_rule3)", 0x0u));
		vector->push_back(make_pair("RAM53 Memory Rule Register 0 (ram53_mem_rule0)", 0x0u));
		vector->push_back(make_pair("RAM53 Memory Rule Register 1 (ram53_mem_rule1)", 0x0u));
		vector->push_back(make_pair("RAM53 Memory Rule Register 2 (ram53_mem_rule2)", 0x0u));
		vector->push_back(make_pair("RAM53 Memory Rule Register 3 (ram53_mem_rule3)", 0x0u));
		vector->push_back(make_pair("RAM6 Slave Rule Register (ram6_slave_rule)", 0x0u));
		vector->push_back(make_pair("RAM60 Memory Rule Register 0 (ram60_mem_rule0)", 0x0u));
		vector->push_back(make_pair("RAM60 Memory Rule Register 1 (ram60_mem_rule1)", 0x0u));
		vector->push_back(make_pair("RAM60 Memory Rule Register 2 (ram60_mem_rule2)", 0x0u));
		vector->push_back(make_pair("RAM60 Memory Rule Register 3 (ram60_mem_rule3)", 0x0u));
		vector->push_back(make_pair("RAM61 Memory Rule Register 0 (ram61_mem_rule0)", 0x0u));
		vector->push_back(make_pair("RAM61 Memory Rule Register 1 (ram61_mem_rule1)", 0x0u));
		vector->push_back(make_pair("RAM61 Memory Rule Register 2 (ram61_mem_rule2)", 0x0u));
		vector->push_back(make_pair("RAM61 Memory Rule Register 3 (ram61_mem_rule3)", 0x0u));
		vector->push_back(make_pair("RAM62 Memory Rule Register 0 (ram62_mem_rule0)", 0x0u));
		vector->push_back(make_pair("RAM62 Memory Rule Register 1 (ram62_mem_rule1)", 0x0u));
		vector->push_back(make_pair("RAM62 Memory Rule Register 2 (ram62_mem_rule2)", 0x0u));
		vector->push_back(make_pair("RAM62 Memory Rule Register 3 (ram62_mem_rule3)", 0x0u));
		vector->push_back(make_pair("RAM63 Memory Rule Register 0 (ram63_mem_rule0)", 0x0u));
		vector->push_back(make_pair("RAM63 Memory Rule Register 1 (ram63_mem_rule1)", 0x0u));
		vector->push_back(make_pair("RAM63 Memory Rule Register 2 (ram63_mem_rule2)", 0x0u));
		vector->push_back(make_pair("RAM63 Memory Rule Register 3 (ram63_mem_rule3)", 0x0u));
		vector->push_back(make_pair("RAM7 Slave Rule Register (ram7_slave_rule)", 0x0u));
		vector->push_back(make_pair("RAM70 Memory Rule Register 0 (ram70_mem_rule0)", 0x0u));
		vector->push_back(make_pair("RAM70 Memory Rule Register 1 (ram70_mem_rule1)", 0x0u));
		vector->push_back(make_pair("RAM70 Memory Rule Register 2 (ram70_mem_rule2)", 0x0u));
		vector->push_back(make_pair("RAM70 Memory Rule Register 3 (ram70_mem_rule3)", 0x0u));
		vector->push_back(make_pair("RAM71 Memory Rule Register 0 (ram71_mem_rule0)", 0x0u));
		vector->push_back(make_pair("RAM71 Memory Rule Register 1 (ram71_mem_rule1)", 0x0u));
		vector->push_back(make_pair("RAM71 Memory Rule Register 2 (ram71_mem_rule2)", 0x0u));
		vector->push_back(make_pair("RAM71 Memory Rule Register 3 (ram71_mem_rule3)", 0x0u));
		vector->push_back(make_pair("RAM72 Memory Rule Register 0 (ram72_mem_rule0)", 0x0u));
		vector->push_back(make_pair("RAM72 Memory Rule Register 1 (ram72_mem_rule1)", 0x0u));
		vector->push_back(make_pair("RAM72 Memory Rule Register 2 (ram72_mem_rule2)", 0x0u));
		vector->push_back(make_pair("RAM72 Memory Rule Register 3 (ram72_mem_rule3)", 0x0u));
		vector->push_back(make_pair("RAM73 Memory Rule Register 0 (ram73_mem_rule0)", 0x0u));
		vector->push_back(make_pair("RAM73 Memory Rule Register 1 (ram73_mem_rule1)", 0x0u));
		vector->push_back(make_pair("RAM73 Memory Rule Register 2 (ram73_mem_rule2)", 0x0u));
		vector->push_back(make_pair("RAM73 Memory Rule Register 3 (ram73_mem_rule3)", 0x0u));
		vector->push_back(make_pair("RAM8 Slave Rule Register (ram8_slave_rule)", 0x0u));
		vector->push_back(make_pair("RAM80 Memory Rule Register 0 (ram80_mem_rule0)", 0x0u));
		vector->push_back(make_pair("RAM80 Memory Rule Register 1 (ram80_mem_rule1)", 0x0u));
		vector->push_back(make_pair("RAM80 Memory Rule Register 2 (ram80_mem_rule2)", 0x0u));
		vector->push_back(make_pair("RAM80 Memory Rule Register 3 (ram80_mem_rule3)", 0x0u));
		vector->push_back(make_pair("RAM81 Memory Rule Register 0 (ram81_mem_rule0)", 0x0u));
		vector->push_back(make_pair("RAM81 Memory Rule Register 1 (ram81_mem_rule1)", 0x0u));
		vector->push_back(make_pair("RAM81 Memory Rule Register 2 (ram81_mem_rule2)", 0x0u));
		vector->push_back(make_pair("RAM81 Memory Rule Register 3 (ram81_mem_rule3)", 0x0u));
		vector->push_back(make_pair("HiFi4 DSP Slave Rule Register (pif_hifi4_x_slave_rule0)", 0x0u));
		vector->push_back(make_pair("HiFi4 DSP Memory Rule Register 0 (pif_hifi4_x_mem_rule0)", 0x0u));
		vector->push_back(make_pair("HiFi4 DSP Memory Rule Register 1 (pif_hifi4_x_mem_rule1)", 0x0u));
		vector->push_back(make_pair("HiFi4 DSP Memory Rule Register 2 (pif_hifi4_x_mem_rule2)", 0x0u));
		vector->push_back(make_pair("HiFi4 DSP Memory Rule Register 3 (pif_hifi4_x_mem_rule3)", 0x0u));
		vector->push_back(make_pair("APB Bridge Slave Rule Register (apb_bridge_slave_rule0)", 0x0u));
		vector->push_back(make_pair("APB Bridge Group 0 Memory Rule Register 0 (apb_grp0_mem_rule0)", 0x0u));
		vector->push_back(make_pair("APB Bridge Group 0 Memory Rule Register 1 (apb_grp0_mem_rule1)", 0x0u));
		vector->push_back(make_pair("APB Bridge Group 0 Memory Rule Register 2 (apb_grp0_mem_rule2)", 0x0u));
		vector->push_back(make_pair("APB Bridge Group 0 Memory Rule Register 3 (apb_grp0_mem_rule3)", 0x0u));
		vector->push_back(make_pair("APB Bridge Group 1 Memory Rule Register 0 (apb_grp1_mem_rule0)", 0x0u));
		vector->push_back(make_pair("APB Bridge Group 1 Memory Rule Register 1 (apb_grp1_mem_rule1)", 0x0u));
		vector->push_back(make_pair("APB Bridge Group 1 Memory Rule Register 2 (apb_grp1_mem_rule2)", 0x0u));
		vector->push_back(make_pair("APB Bridge Group 1 Memory Rule Register 3 (apb_grp1_mem_rule3)", 0x0u));
		vector->push_back(make_pair("AHB Peripherals 0 Slave Rule Register 0 (ahb_periph0_slave_rule0)", 0x0u));
		vector->push_back(make_pair("AHB Peripherals 0 Slave Rule Register 1 (ahb_periph0_slave_rule1)", 0x0u));
		vector->push_back(make_pair("AIPS bridge 0 Memory Rule Register 0 (aips_bridge0_mem_rule0)", 0x0u));
		vector->push_back(make_pair("AIPS bridge 0 Memory Rule Register 1 (aips_bridge0_mem_rule1)", 0x0u));
		vector->push_back(make_pair("AHB Peripherals 1 Slave Rule Register (ahb_periph1_slave_rule0)", 0x0u));
		vector->push_back(make_pair("AIPS bridge 1 Slave Rule Register (aips_bridge1_slave_rule0)", 0x0u));
		vector->push_back(make_pair("AIPS bridge 1 Memory Rule Register 0 (aips_bridge1_mem_rule0)", 0x0u));
		vector->push_back(make_pair("AIPS bridge 1 Memory Rule Register 1 (aips_bridge1_mem_rule1)", 0x0u));
		vector->push_back(make_pair("AHB Peripherals 2 Slave Rule Register 0 (ahb_periph2_slave_rule0)", 0x0u));
		vector->push_back(make_pair("AHB Peripherals 3 Slave Rule Register 0 (ahb_periph3_slave_rule0)", 0x0u));
		vector->push_back(make_pair("Secure GPIO Register 0 (sec_gp_reg0)", 0xffffffffu));
		vector->push_back(make_pair("Secure GPIO Register 1 (sec_gp_reg1)", 0xffffffffu));
		vector->push_back(make_pair("Secure GPIO Register 2 (sec_gp_reg2)", 0xffffffffu));
		vector->push_back(make_pair("Secure GPIO Register 3 (sec_gp_reg3)", 0xffffffffu));
		vector->push_back(make_pair("Secure GPIO Register 4 (sec_gp_reg4)", 0xffffffffu));
		vector->push_back(make_pair("Secure GPIO Register 5 (sec_gp_reg5)", 0xffffffffu));
		vector->push_back(make_pair("Secure GPIO Register 6 (sec_gp_reg6)", 0xffffffffu));
		vector->push_back(make_pair("Secure GPIO Register 7 (sec_gp_reg7)", 0xffffffffu));
		vector->push_back(make_pair("Secure GPIO Register 8 for DSP(sec_gp_reg8)", 0xffffffffu));
		vector->push_back(make_pair("Secure GPIO Lock Register (sec_gp_reg_lock)", 0x0000aaaau));
		vector->push_back(make_pair("Master Secure Level Register (master_sec_reg)", 0x80000000u));
		vector->push_back(make_pair("Master Secure Level Anti-pole Register (master_sec_anti_pol_reg)", 0xbfffffffu));
		vector->push_back(make_pair("M33 Lock Control Register (m33_lock_reg)", 0x800002aau));
		vector->push_back(make_pair("Secure Control Duplicate Register (misc_ctrl_dp_reg)", 0x0000aaaau));
		vector->push_back(make_pair("Secure Control Register (misc_ctrl_reg)", 0x0000aaaau));
		return vector;
	}
	std::vector<pair<string, uint32_t>>* TrustZonePresetGenerator::getLpc55xxVector() {
		auto vector = new std::vector<pair<string, uint32_t>>();
		vector->push_back(make_pair("CM33 Secure vector table address (cm33_vtor_addr)", 0x0u));
		vector->push_back(make_pair("CM33 Non-secure vector table address (cm33_vtor_ns_addr)", 0x0u));
		vector->push_back(make_pair("CM33 Interrupt target non-secure register 0 (cm33_nvic_itns0)", 0x0u));
		vector->push_back(make_pair("CM33 Interrupt target non-secure register 1 (cm33_nvic_itns1)", 0x0u));
		vector->push_back(make_pair("MCM33 Secure vector table address (mcm33_vtor_addr)", 0x0u));
		vector->push_back(make_pair("MPU Control Register.(cm33_mpu_ctrl)", 0x0u));
		vector->push_back(make_pair("MPU Memory Attribute Indirection Register 0 (cm33_mpu_mair0)", 0x0u));
		vector->push_back(make_pair("MPU Memory Attribute Indirection Register 1 (cm33_mpu_mair1)", 0x0u));
		vector->push_back(make_pair("MPU Region 0 Base Address Register (cm33_mpu_rbar0)", 0x0u));
		vector->push_back(make_pair("MPU Region 0 Limit Address Register (cm33_mpu_rlar0)", 0x0u));
		vector->push_back(make_pair("MPU Region 1 Base Address Register (cm33_mpu_rbar1)", 0x0u));
		vector->push_back(make_pair("MPU Region 1 Limit Address Register (cm33_mpu_rlar1)", 0x0u));
		vector->push_back(make_pair("MPU Region 2 Base Address Register (cm33_mpu_rbar2)", 0x0u));
		vector->push_back(make_pair("MPU Region 2 Limit Address Register (cm33_mpu_rlar2)", 0x0u));
		vector->push_back(make_pair("MPU Region 3 Base Address Register (cm33_mpu_rbar3)", 0x0u));
		vector->push_back(make_pair("MPU Region 3 Limit Address Register (cm33_mpu_rlar3)", 0x0u));
		vector->push_back(make_pair("MPU Region 4 Base Address Register (cm33_mpu_rbar4)", 0x0u));
		vector->push_back(make_pair("MPU Region 4 Limit Address Register (cm33_mpu_rlar4)", 0x0u));
		vector->push_back(make_pair("MPU Region 5 Base Address Register (cm33_mpu_rbar5)", 0x0u));
		vector->push_back(make_pair("MPU Region 5 Limit Address Register (cm33_mpu_rlar5)", 0x0u));
		vector->push_back(make_pair("MPU Region 6 Base Address Register (cm33_mpu_rbar6)", 0x0u));
		vector->push_back(make_pair("MPU Region 6 Limit Address Register (cm33_mpu_rlar6)", 0x0u));
		vector->push_back(make_pair("MPU Region 7 Base Address Register (cm33_mpu_rbar7)", 0x0u));
		vector->push_back(make_pair("MPU Region 7 Limit Address Register (cm33_mpu_rlar7)", 0x0u));
		vector->push_back(make_pair("Non-secure MPU Control Register.(cm33_mpu_ctrl_ns)", 0x0u));
		vector->push_back(make_pair("Non-secure MPU Memory Attribute Indirection Register 0 (cm33_mpu_mair0_ns)", 0x0u));
		vector->push_back(make_pair("Non-secure MPU Memory Attribute Indirection Register 1 (cm33_mpu_mair1_ns)", 0x0u));
		vector->push_back(make_pair("Non-secure MPU Region 0 Base Address Register (cm33_mpu_rbar0_ns)", 0x0u));
		vector->push_back(make_pair("Non-secure MPU Region 0 Limit Address Register (cm33_mpu_rlar0_ns)", 0x0u));
		vector->push_back(make_pair("Non-secure MPU Region 1 Base Address Register (cm33_mpu_rbar1_ns)", 0x0u));
		vector->push_back(make_pair("Non-secure MPU Region 1 Limit Address Register (cm33_mpu_rlar1_ns)", 0x0u));
		vector->push_back(make_pair("Non-secure MPU Region 2 Base Address Register (cm33_mpu_rbar2_ns)", 0x0u));
		vector->push_back(make_pair("Non-secure MPU Region 2 Limit Address Register (cm33_mpu_rlar2_ns)", 0x0u));
		vector->push_back(make_pair("Non-secure MPU Region 3 Base Address Register (cm33_mpu_rbar3_ns)", 0x0u));
		vector->push_back(make_pair("Non-secure MPU Region 3 Limit Address Register (cm33_mpu_rlar3_ns)", 0x0u));
		vector->push_back(make_pair("Non-secure MPU Region 4 Base Address Register (cm33_mpu_rbar4_ns)", 0x0u));
		vector->push_back(make_pair("Non-secure MPU Region 4 Limit Address Register (cm33_mpu_rlar4_ns)", 0x0u));
		vector->push_back(make_pair("Non-secure MPU Region 5 Base Address Register (cm33_mpu_rbar5_ns)", 0x0u));
		vector->push_back(make_pair("Non-secure MPU Region 5 Limit Address Register (cm33_mpu_rlar5_ns)", 0x0u));
		vector->push_back(make_pair("Non-secure MPU Region 6 Base Address Register (cm33_mpu_rbar6_ns)", 0x0u));
		vector->push_back(make_pair("Non-secure MPU Region 6 Limit Address Register (cm33_mpu_rlar6_ns)", 0x0u));
		vector->push_back(make_pair("Non-secure MPU Region 7 Base Address Register (cm33_mpu_rbar7_ns)", 0x0u));
		vector->push_back(make_pair("Non-secure MPU Region 7 Limit Address Register (cm33_mpu_rlar7_ns)", 0x0u));
		vector->push_back(make_pair("SAU Control Register.(cm33_sau_ctrl)", 0x0u));
		vector->push_back(make_pair("SAU Region 0 Base Address Register (cm33_sau_rbar0)", 0x0u));
		vector->push_back(make_pair("SAU Region 0 Limit Address Register (cm33_sau_rlar0)", 0x0u));
		vector->push_back(make_pair("SAU Region 1 Base Address Register (cm33_sau_rbar1)", 0x0u));
		vector->push_back(make_pair("SAU Region 1 Limit Address Register (cm33_sau_rlar1)", 0x0u));
		vector->push_back(make_pair("SAU Region 2 Base Address Register (cm33_sau_rbar2)", 0x0u));
		vector->push_back(make_pair("SAU Region 2 Limit Address Register (cm33_sau_rlar2)", 0x0u));
		vector->push_back(make_pair("SAU Region 3 Base Address Register (cm33_sau_rbar3)", 0x0u));
		vector->push_back(make_pair("SAU Region 3 Limit Address Register (cm33_sau_rlar3)", 0x0u));
		vector->push_back(make_pair("SAU Region 4 Base Address Register (cm33_sau_rbar4)", 0x0u));
		vector->push_back(make_pair("SAU Region 4 Limit Address Register (cm33_sau_rlar4)", 0x0u));
		vector->push_back(make_pair("SAU Region 5 Base Address Register (cm33_sau_rbar5)", 0x0u));
		vector->push_back(make_pair("SAU Region 5 Limit Address Register (cm33_sau_rlar5)", 0x0u));
		vector->push_back(make_pair("SAU Region 6 Base Address Register (cm33_sau_rbar6)", 0x0u));
		vector->push_back(make_pair("SAU Region 6 Limit Address Register (cm33_sau_rlar6)", 0x0u));
		vector->push_back(make_pair("SAU Region 7 Base Address Register (cm33_sau_rbar7)", 0x0u));
		vector->push_back(make_pair("SAU Region 7 Limit Address Register (cm33_sau_rlar7)", 0x0u));
		vector->push_back(make_pair("FLASH/ROM Slave Rule Register 0 (flash_rom_slave_rule)", 0x0u));
		vector->push_back(make_pair("FLASH Memory Rule Register 0 (flash_mem_rule0)", 0x0u));
		vector->push_back(make_pair("FLASH Memory Rule Register 1 (flash_mem_rule1)", 0x0u));
		vector->push_back(make_pair("FLASH Memory Rule Register 2 (flash_mem_rule2)", 0x0u));
		vector->push_back(make_pair("ROM Memory Rule Register 0 (rom_mem_rule0)", 0x0u));
		vector->push_back(make_pair("ROM Memory Rule Register 1 (rom_mem_rule1)", 0x0u));
		vector->push_back(make_pair("ROM Memory Rule Register 2 (rom_mem_rule2)", 0x0u));
		vector->push_back(make_pair("ROM Memory Rule Register 3 (rom_mem_rule3)", 0x0u));
		vector->push_back(make_pair("RAMX Slave Rule Register (ramx_slave_rule)", 0x0u));
		vector->push_back(make_pair("RAMX Memory Rule Register 0 (ramx_mem_rule0)", 0x0u));
		vector->push_back(make_pair("RAM0 Slave Rule Register (ram0_slave_rule)", 0x0u));
		vector->push_back(make_pair("RAM0 Memory Rule Register 0 (ram0_mem_rule0)", 0x0u));
		vector->push_back(make_pair("RAM0 Memory Rule Register 1 (ram0_mem_rule1)", 0x0u));
		vector->push_back(make_pair("RAM1 Slave Rule Register (ram1_slave_rule)", 0x0u));
		vector->push_back(make_pair("RAM1 Memory Rule Register 0 (ram1_mem_rule0)", 0x0u));
		vector->push_back(make_pair("RAM1 Memory Rule Register 1 (ram1_mem_rule1)", 0x0u));
		vector->push_back(make_pair("RAM2 Slave Rule Register (ram2_slave_rule)", 0x0u));
		vector->push_back(make_pair("RAM2 Memory Rule Register 0 (ram2_mem_rule0)", 0x0u));
		vector->push_back(make_pair("RAM2 Memory Rule Register 1 (ram2_mem_rule1)", 0x0u));
		vector->push_back(make_pair("RAM3 Slave Rule Register (ram3_slave_rule)", 0x0u));
		vector->push_back(make_pair("RAM3 Memory Rule Register 0 (ram3_mem_rule0)", 0x0u));
		vector->push_back(make_pair("RAM3 Memory Rule Register 1 (ram3_mem_rule1)", 0x0u));
		vector->push_back(make_pair("RAM4 Slave Rule Register (ram4_slave_rule)", 0x0u));
		vector->push_back(make_pair("RAM4 Memory Rule Register 0 (ram4_mem_rule0)", 0x0u));
		vector->push_back(make_pair("APB Bridge Group Slave Rule Register (apb_grp_slave_rule)", 0x0u));
		vector->push_back(make_pair("APB Bridge Group 0 Memory Rule Register 0 (apb_grp0_mem_rule0)", 0x0u));
		vector->push_back(make_pair("APB Bridge Group 0 Memory Rule Register 1 (apb_grp0_mem_rule1)", 0x0u));
		vector->push_back(make_pair("APB Bridge Group 0 Memory Rule Register 2 (apb_grp0_mem_rule2)", 0x0u));
		vector->push_back(make_pair("APB Bridge Group 0 Memory Rule Register 3 (apb_grp0_mem_rule3)", 0x0u));
		vector->push_back(make_pair("APB Bridge Group 1 Memory Rule Register 0 (apb_grp1_mem_rule0)", 0x0u));
		vector->push_back(make_pair("APB Bridge Group 1 Memory Rule Register 1 (apb_grp1_mem_rule1)", 0x0u));
		vector->push_back(make_pair("APB Bridge Group 1 Memory Rule Register 2 (apb_grp1_mem_rule2)", 0x0u));
		vector->push_back(make_pair("APB Bridge Group 1 Memory Rule Register 3 (apb_grp1_mem_rule3)", 0x0u));
		vector->push_back(make_pair("AHB Peripherals 0 Slave Rule Register 0 (ahb_periph0_slave_rule0)", 0x0u));
		vector->push_back(make_pair("AHB Peripherals 0 Slave Rule Register 1 (ahb_periph0_slave_rule1)", 0x0u));
		vector->push_back(make_pair("AHB Peripherals 1 Slave Rule Register 0 (ahb_periph1_slave_rule0)", 0x0u));
		vector->push_back(make_pair("AHB Peripherals 1 Slave Rule Register 1 (ahb_periph1_slave_rule1)", 0x0u));
		vector->push_back(make_pair("AHB Peripherals 2 Slave Rule Register 0 (ahb_periph2_slave_rule0)", 0x0u));
		vector->push_back(make_pair("AHB Peripherals 2 Slave Rule Register 1 (ahb_periph2_slave_rule1)", 0x0u));
		vector->push_back(make_pair("AHB Peripherals 2 Memory Rule Register 0 (ahb_periph2_mem_rule0)", 0x0u));
		vector->push_back(make_pair("HS USB Slave Rule Register 0 (usb_hs_slave_rule0)", 0x0u));
		vector->push_back(make_pair("HS USB Memory Rule Register 0 (usb_hs__mem_rule0)", 0x0u));
		vector->push_back(make_pair("Secure GPIO Register 0 (sec_gp_reg0)", 0xffffffffu));
		vector->push_back(make_pair("Secure GPIO Register 1 (sec_gp_reg1)", 0xffffffffu));
		vector->push_back(make_pair("Secure GPIO Register 2 (sec_gp_reg2)", 0xffffffffu));
		vector->push_back(make_pair("Secure GPIO Register 3 (sec_gp_reg3)", 0xffffffffu));
		vector->push_back(make_pair("Secure Interrupt Mask for CPU1 Register 0 (sec_int_reg0)", 0xffffffffu));
		vector->push_back(make_pair("Secure Interrupt Mask for CPU1 Register 1 (sec_int_reg1)", 0xffffffffu));
		vector->push_back(make_pair("Secure GPIO Lock Register (sec_gp_reg_lock)", 0x00000aaau));
		vector->push_back(make_pair("Master Secure Level Register (master_sec_reg)", 0x80000000u));
		vector->push_back(make_pair("Master Secure Level Anti-pole Register (master_sec_anti_pol_reg)", 0xbfffffffu));
		vector->push_back(make_pair("CM33 Lock Control Register (cm33_lock_reg)", 0x800002aau));
		vector->push_back(make_pair("MCM33 Lock Control Register (mcm33_lock_reg)", 0x8000000au));
		vector->push_back(make_pair("Secure Control Duplicate Register (misc_ctrl_dp_reg)", 0x0000aaaau));
		vector->push_back(make_pair("Secure Control Register (misc_ctrl_reg)", 0x0000aaaau));
		return vector;
	}
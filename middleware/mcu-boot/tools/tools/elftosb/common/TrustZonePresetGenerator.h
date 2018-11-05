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

#ifndef _TRUSTZONEGENERATOR_
#define _TRUSTZONEGENERATOR_

#include <string>
#include <memory>
#include <map>
#include "json.h"
using namespace std;

/*!
* \brief Generator for TrustZone-M preset files.
*
* Takes the JSON specififcation of registers values and creating simple bin file.
* Bin file can be later attached into master boot image with TZ-M preset.
* Currently support of lpc55xx and rt6xx 
*/
class TrustZonePresetGenerator {
public:
	TrustZonePresetGenerator();
	//! \brief Used for creation of TZ-M preset data bin file based on provided JSON cofiguration.
	void execute(const string& confFilePath);
	//! \brief Used for on the fly creation of TZ-M preset data during master boot image genaration. Created based on provided JSON cofiguration.
	unique_ptr<vector<uint32_t>> getTzpPreset(const string& confFilePath, const string& family);

private:
	string outputFile;
	string family;
	//! \brief Connecting all particular processing steps in specific order.
	bool process(const string& confFilePath);
	//! \brief Saving TZ-M preset data to bin file.
	bool saveFile(unique_ptr<vector<uint32_t>> buffer);
	/*! 
	* \brief Creating TZ-M preset data vector. If register is missing in JSON specification, default value is used from device specification.
	* Registers are processed in right order based on device specification.
	*/
	unique_ptr<vector<uint32_t>> generateTzp(jute::jValue tzp, const string& confFilePath);
	//! \brief Parsing and validating information from JSON configuration file
	jute::jValue parseFile(const string &confFilePath);
	//! \brief Returns device configuration.
	std::vector<pair<string, uint32_t>>* getVector(const string family);
	//! \brief Returns device configuration for rt6xx.
	std::vector<pair<string, uint32_t>>* getRt6xxVector();
	//! \brief Returns device configuration for lpc55xx.
	std::vector<pair<string, uint32_t>>* getLpc55xxVector();
};

#endif /*_TRUSTZONEGENERATOR_*/
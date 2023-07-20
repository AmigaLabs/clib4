/** PatchForSPE.cpp
 *
 * Patches assembly code generated for the e500 Stream Processing Engine
 * to align the stack to 16 bytes (some naughty code misaligns the stack).
 *
 * How to use:
 * - Either compile this with G++ -o PatchForSPE.exe PatchForSPE.cpp, or add
 *   a rule to your makefile:
 *   PatchForSPE: PatchForSPE.cpp
 *   	$(CXX) -o $@ $<
 * - Generate assembly code instead of the output binary, with the -S option,
 *   and run PatchForSPE on it (hint use -ggdb instead of -gstabs for debug
 *   info, or the GNU assembler might give warnings).
 *   An example makefile rule:
 *  	$(CC) $File_spe.S: File.c File.h PatchForSPE
 *  	$(CC) -c $(CFLAGS) $(CFLAGS_SPE) -S -o $@_in $<
 *  	PatchForSPE $@_in $@
 * - Then, compile the assembly file to an objet file:
 *  File_spe.o: $(CFG)/File_spe.S
 *  	$(CC) -c $(CFLAGS) $(CFLAGS_SPE) -o $@ $<
 *
 * CFLAGS_SPE should contain the following to generate SPE code:
 *    -mspe -mcpu=8540 -mfloat-gprs=double -mabi=spe
 *
 * @author Hans de Ruiter
 *
 * @Copyright (C) 2018 by Kea Sigma Delta Limited, all rights reserved
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <algorithm>

#define DEBUG

using namespace std;

// ----- CONSTANTS -----
#define NUMARGS 2 // Need 2 parameters

// ----- PRIVATE FUNCTIONS -----

static void printUsage(const char *progName) {
    cout << progName << ": Patches GCC generated assembly for the e500 core's SPE to force 16 byte stack alignment"
         << endl << endl;
    cout << "Usage: " << endl;
    cout << progName << " <input filename> <output name>" << endl;
    cout << "This will patch function prologs/epilogues to enforce 16 byte stack alignment. Some naughty PowerPC code"
         << endl
         << "misaligns the stack, causing code compiled for the e500 core's SPE to crash with an alignment exception. "
         << endl
         << "This will add code to make sure it's aligned correctly." << endl
         << endl
         << "Use this when writing library code that may be called from general PowerPC code (i.e., code compiled for "
         << endl
         << "plain PowerPC." << endl
         << endl;
    cout << "Example:" << endl;
    cout << progName << " main.s_in main.s" << endl;
}

inline void assertFileGood(ifstream &file) {
    if (file.bad()) {
        cout << "ERROR: Could not read the file." << endl;
        cout << "Failed." << endl;
        exit(10);
    }
}
// ----- FUNCTIONS -----

int main(int argc, char **argv) {
    // Parse args
    if (argc != NUMARGS + 1) {
        printUsage(argv[0]);
        return 10;
    }
    char *inFileName = argv[1];
    char *outFileName = argv[2];
    ifstream inFile(inFileName);

#ifdef DEBUG
    cout << argv[0] << ": " << endl;
#endif

    if (inFile.fail()) {
        cout << "Could not open " << inFileName << endl;
        cout << "Operation failed." << endl;
        return 10;
    }

    ofstream outFile(outFileName);
    if (outFile.fail()) {
        cout << "Could not open " << outFileName << "for writing." << endl;
        cout << "Operation failed." << endl;
        return 10;
    }

    // Scan through the file
    std::string buffer;
    const std::string prologStackStr("stwu 1,");
    while (!inFile.eof()) {
        assertFileGood(inFile);
        std::getline(inFile, buffer);

        // Is this line one we need to modify?
        size_t loc;
        if ((loc = buffer.find(prologStackStr)) != std::string::npos) {
            // Found a function's prolog. Replace the stack frame allocation code
            loc += prologStackStr.size();
            std::istringstream inStr(buffer);
            inStr.seekg(loc, std::ios_base::beg);
            int frameOffset;
            inStr >> frameOffset;

            outFile << "	rlwinm 11, 1, 0, 0, 27 # r3 = r1 & ~0xF (calc aligned stack pointer)" << endl
                    << "	stwu 1, " << frameOffset
                    << "(11) # write r1 to address r11 - n (so, relative to aligned stack pointer)" << endl
                    << "	mr 1, 11 # move new aligned stack pointer into r1" << endl;
        } else if ((loc = buffer.find("addi 1,1")) != std::string::npos) {
            // Found the stack frame deallocation code in the epilogue. Yes, it
            // needs replacing too (because it uses a static offset instead of the
            // frame pointer)
            outFile << "	lwz 1, 0(1)" << endl;
        } else {
            outFile << buffer << endl;
        }
    }

    inFile.close();
    outFile.close();

    // All done
#ifdef DEBUG
    cout << "Success." << endl;
#endif
}


/** A compare function for case insensitive string comparison
 */
bool caseInsensitive_compare(char ch1, char ch2) {
    return toupper((unsigned char) ch1) == toupper((unsigned char) ch2);
}

/** Strips the white-space from the front and end of a string
 */
void stripWhiteSpace(string &str) {
#ifdef DEBUG
    cout << __func__ << " called." << endl;
#endif
    size_t strsize = str.size();
    if (str[0] == ' ' || str[0] == '\t') {
        size_t n = 0;
        while ((str[n] == ' ' || str[n] == '\t') && n < strsize) {
            ++n;
        }
        str.erase(0, n);
    }

    size_t pos = str.size() - 1;
    if (str[pos] == ' ' || str[pos] == '\t') {
        while ((str[pos] == ' ' || str[pos] == '\t') && pos >= 0) {
            --pos;
        }
        ++pos;
        str.erase(pos);
    }
}

bool
getLayout(ifstream &file, const char *layoutName, string &actualLayoutName, streampos &startPos, streampos &endPos) {
#ifdef DEBUG
    cout << __func__ << " called with layoutName: " << (layoutName ? layoutName : "(anything)") << ", startPos:"
         << startPos << "and endPos:" << endPos << "." << endl;
#endif

    streampos localStartPos = file.tellg();
    string buffer;
    string layoutNameStr;

    if (layoutName) {
        layoutNameStr = layoutName;
    }

    file.clear(); // need to do this or the end-of-file check will fail

    while (!file.eof()) {
        assertFileGood(file);
        std::getline(file, buffer);
#ifdef DEBUG
        cout << "read line:" << buffer << endl;
#endif
        size_t pos = buffer.find("LABEL");
        if (pos == 0) {
            // Have a kickstart label, check if it matches
            actualLayoutName = buffer.substr(sizeof("LABEL ") - 1);
            stripWhiteSpace(actualLayoutName);
            if (!layoutName || std::equal(actualLayoutName.begin(), actualLayoutName.end(), layoutNameStr.begin(),
                                          caseInsensitive_compare) == true) {
                // Found the desired kickstart layout, now find the end
                while (!file.eof()) {
                    assertFileGood(file);
                    startPos = localStartPos;
                    endPos = file.tellg();
                    std::getline(file, buffer);
                    if (buffer.size() == 0) {
                        // found it
                        return true;
                    }
                }
                file.clear(); // need to do this or the end-of-file check will fail
                return true;
            }
        }
        localStartPos = file.tellg();
    }

    return false;
}

bool findModule(ifstream &file, const char *moduleName, streampos startPos, streampos endPos) {
#ifdef DEBUG
    cout << __func__ << " called with moduleName: " << (moduleName ? moduleName : "(anything)") << ", startPos:"
         << startPos << "and endPos:" << endPos << "." << endl;
#endif

    file.clear();
    file.seekg(startPos);
    streampos currPos = file.tellg();
    string buffer;
    string targetModuleName;
    if (moduleName) {
        targetModuleName = moduleName;
    }

    while (currPos < endPos && !file.eof()) {
        assertFileGood(file);
        std::getline(file, buffer);
#ifdef DEBUG
        cout << "read line:" << buffer << endl;
#endif
        size_t pos = buffer.find("MODULE");
        if (pos == 0) {
            // Have a kickstart label, check if it matches
            string actualModuleName = buffer.substr(sizeof("MODULE ") - 1);
            stripWhiteSpace(actualModuleName);
            if (!moduleName || std::equal(actualModuleName.begin(), actualModuleName.end(), targetModuleName.begin(),
                                          caseInsensitive_compare) == true) {
                // found it
                return true;
            }
        }
        startPos = file.tellg();
    }

    file.clear();
    return false;
}



#include "barrel_shifter.hpp"
#include "constants.hpp"
#include "cpu.hpp"

#include <cstring>
#include <fstream>
#include <iostream>

using namespace ARMSimulator;

int readFromElf(const char* buffer, const unsigned int start, const unsigned int size, bool bigEndian) {
    int ret = 0;
    if (bigEndian) {
        for (unsigned int i = 0; i < size; ++i) {
            ret |= (buffer[start + i] & 0xFF) << (size - 1 - i) * 8;
        }
    } else {
        for (int i = size - 1; i >= 0; --i) {
            ret |= (buffer[start + i] & 0xFF) << i * 8;
        }
    }

    return ret;
}

int main(int argc, const char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " ELF-Filename" << '\n';
        return 1;
    }

    std::ifstream elfFile;
    elfFile.open(argv[1], std::ios::in | std::ios::binary);

    char buffer[1024] = {};
    elfFile.read(buffer, ELF_HEADER_SIZE); // read entire file header
    if (memcmp(buffer, ELF_MAGIC_NUMBER, 4) != 0) {
        std::cerr << "ELF file " << argv[1] << " is not a valid ELF File" << '\n';
        return 1;
    }

    if (buffer[4] != 1) {
        std::cerr << "ELF file " << argv[1]
                  << " was compiled for 64bit, which is not supported" << '\n';
        return 1;
    }

    if (!(buffer[0x13] == 0x28 || buffer[0x12] == 0x28)) {
        std::cerr << "ELF file " << argv[1] << " was not built for ARM" << '\n';
        return 1;
    }

    bool bigEndian = buffer[0x5] == 2;

    unsigned long memSize = 4 * 1024 * 1024;
    Cpu           cpu(memSize, bigEndian);

    unsigned int programEntry = readFromElf(buffer, 0x18, 4, bigEndian);
    cpu.setRegister(Register::pc, programEntry);
    cpu.setRegister(Register::sp, 0x10000);

    unsigned int   sectionHeaderStart     = readFromElf(buffer, 0x20, 4, bigEndian);
    unsigned short sectionHeaderEntrySize = readFromElf(buffer, 0x2E, 2, bigEndian);
    unsigned short numSectionHeaders      = readFromElf(buffer, 0x30, 2, bigEndian);

    elfFile.seekg(sectionHeaderStart);

    for (int i = 0; i < numSectionHeaders; ++i) {
        elfFile.read(buffer, sectionHeaderEntrySize);
        int sectionType = readFromElf(buffer, 0x04, 4, bigEndian);
        if (sectionType != 0x01) {
            continue;
        }

        int sectionAddress = readFromElf(buffer, 0x0c, 4, bigEndian);
        int sectionOffset  = readFromElf(buffer, 0x10, 4, bigEndian);
        int sectionSize    = readFromElf(buffer, 0x14, 4, bigEndian);

        auto currentPosition = elfFile.tellg();

        // load section to memory
        elfFile.seekg(sectionOffset);
        char sectionBuffer[sectionSize];
        elfFile.read(sectionBuffer, sectionSize);
        cpu.setMemory(sectionAddress, (unsigned char*)sectionBuffer, sectionSize);

        elfFile.seekg(currentPosition);
    }
    elfFile.close();

    try {
        while (true) {
            cpu.nextInstruction();
        }
    } catch (int returnCode) {
        std::cout << "Internal program exited with code " << returnCode << '\n';
    } catch (std::logic_error& e) {
        const char* message = e.what();

        std::cout << "Internal error:" << '\n'
                  << message << '\n';
    }

    std::cout << "Execution finished" << '\n';
    return 0;
}

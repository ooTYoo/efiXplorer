/*
 *        __ ___   __      _
 *       / _(_) \ / /     | |
 *   ___| |_ _ \ V / _ __ | | ___  _ __ ___ _ __
 *  / _ \  _| | > < | '_ \| |/ _ \| '__/ _ \ '__|
 * |  __/ | | |/ . \| |_) | | (_) | | |  __/ |
 *  \___|_| |_/_/ \_\ .__/|_|\___/|_|  \___|_|
 *                  | |
 *                  |_|
 *
 * efiXplorer
 * Copyright (C) 2020  Binarly
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * efiUtils.cpp
 *
 */

#include "efiUtils.h"
#include "tables/efi_system_tables.h"

static const char plugin_name[] = "efiXplorer";

//--------------------------------------------------------------------------
// Create EFI_GUID structure
void createGuidStructure(ea_t ea) {
    static const char struct_name[] = "_EFI_GUID";
    struc_t *sptr = get_struc(get_struc_id(struct_name));
    if (sptr == nullptr) {
        sptr = get_struc(add_struc(-1, struct_name));
        if (sptr == nullptr)
            return;
        add_struc_member(sptr, "data1", -1, dword_flag(), NULL, 4);
        add_struc_member(sptr, "data2", -1, word_flag(), NULL, 2);
        add_struc_member(sptr, "data3", -1, word_flag(), NULL, 2);
        add_struc_member(sptr, "data4", -1, byte_flag(), NULL, 8);
    }
    asize_t size = get_struc_size(sptr);
    create_struct(ea, size, sptr->id);
}

//--------------------------------------------------------------------------
// Set EFI_GUID type
void setGuidType(ea_t ea) {
    tinfo_t tinfo;
    if (tinfo.get_named_type(get_idati(), "EFI_GUID")) {
        apply_tinfo(ea, tinfo, TINFO_DEFINITE);
    }
}

//--------------------------------------------------------------------------
// Get input file type (X64 or X86)
uint8_t getFileType() {
    char fileType[256] = {};
    get_file_type_name(fileType, 256);
    auto fileTypeStr = static_cast<string>(fileType);
    size_t index = fileTypeStr.find("AMD64");
    if (index != string::npos) {
        /* Portable executable for AMD64 (PE) */
        return X64;
    }
    index = fileTypeStr.find("80386");
    if (index != string::npos) {
        /* Portable executable for 80386 (PE) */
        return X86;
    }
    return 0;
}

//--------------------------------------------------------------------------
// Get boot service description comment
string getBsComment(ea_t offset, uint8_t arch) {
    ea_t offset_arch;
    string cmt = "";
    cmt += "gBS->";
    for (auto i = 0; i < BTABLE_LEN; i++) {
        offset_arch = (ea_t)boot_services_table[i].offset64;
        if (arch == X86) {
            offset_arch = (ea_t)boot_services_table[i].offset86;
        }
        if (offset == offset_arch) {
            cmt += boot_services_table[i].name;
            cmt += "()\n";
            cmt += boot_services_table[i].prototype;
            cmt += "\n";
            cmt += boot_services_table[i].parameters;
            break;
        }
    }
    return cmt;
}

//--------------------------------------------------------------------------
// Get runtime service description comment
string getRtComment(ea_t offset, uint8_t arch) {
    ea_t offset_arch;
    string cmt = "";
    cmt += "gRT->";
    for (auto i = 0; i < RTABLE_LEN; i++) {
        offset_arch = (ea_t)runtime_services_table[i].offset64;
        if (arch == X86) {
            offset_arch = (ea_t)runtime_services_table[i].offset86;
        }
        if (offset == offset_arch) {
            cmt += runtime_services_table[i].name;
            cmt += "()\n";
            cmt += runtime_services_table[i].prototype;
            cmt += "\n";
            cmt += runtime_services_table[i].parameters;
            break;
        }
    }
    return cmt;
}

//--------------------------------------------------------------------------
// Find address of global gBS var for X64 module for each service
ea_t findUnknownBsVarX64(ea_t ea) {
    ea_t resAddr = 0;
    insn_t insn;
    /* 10 instructions below */
    for (int i = 0; i < 10; i++) {
        decode_insn(&insn, ea);
        /* check if insn like 'mov rax, cs:<gBS>' */
        if (insn.itype == NN_mov && insn.ops[0].type == o_reg &&
            insn.ops[0].reg == REG_RAX && insn.ops[1].type == o_mem) {
            DEBUG_MSG("[%s] found gBS at 0x%016X, address = 0x%016X\n",
                      plugin_name, ea, insn.ops[1].addr);
            resAddr = insn.ops[1].addr;
            set_cmt(ea, "EFI_BOOT_SERVICES *gBS", true);
            break;
        }
        ea = prev_head(ea, 0);
    }
    return resAddr;
}

//--------------------------------------------------------------------------
// Get all data xrefs for address
vector<ea_t> getXrefs(ea_t addr) {
    vector<ea_t> xrefs;
    ea_t xref = get_first_dref_to(addr);
    while (xref != BADADDR) {
        xrefs.push_back(xref);
        xref = get_next_dref_to(addr, xref);
    }
    return xrefs;
}

//--------------------------------------------------------------------------
// op_stroff wrapper
bool opStroff(ea_t addr, string type) {
    insn_t insn;
    decode_insn(&insn, addr);
    tid_t struc_id = get_struc_id(type.c_str());
    return op_stroff(insn, 0, &struc_id, 1, 0);
}

//--------------------------------------------------------------------------
// Get pointer to named type and apply it
bool setPtrType(ea_t addr, string type) {
    tinfo_t tinfo;
    if (!tinfo.get_named_type(get_idati(), type.c_str())) {
        return false;
    }
    tinfo_t ptrTinfo;
    ptrTinfo.create_ptr(tinfo);
    apply_tinfo(addr, ptrTinfo, TINFO_DEFINITE);
    return true;
}

//--------------------------------------------------------------------------
// Set name and apply pointer to named type
void setPtrTypeAndName(ea_t ea, string name, string type) {
    set_name(ea, name.c_str(), SN_CHECK);
    setPtrType(ea, type.c_str());
}

//--------------------------------------------------------------------------
// Check for guids.json file exist
bool guidsJsonExists() {
    struct stat buffer;
    /* get guids.json path */
    path guidsJsonPath;
    guidsJsonPath /= idadir("plugins");
    guidsJsonPath /= "guids";
    guidsJsonPath /= "guids.json";
    return (stat(guidsJsonPath.u8string().c_str(), &buffer) == 0);
}

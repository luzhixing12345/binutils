/*
 *Copyright (c) 2023 All rights reserved
 *@description: readelf
 *@author: Zhixing Lu
 *@date: 2023-05-04
 *@email: luzhixing12345@163.com
 *@Github: luzhixing12345
 */

// GNU binutils-readelf:
// https://sourceware.org/git/?p=binutils-gdb.git;a=blob;f=binutils/readelf.c;h=b872876a8b660be19e1ffc66ee300d0bbfaed345;hb=HEAD#l5821

#include <elf.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "xargparse.h"

static const char *VERSION = "v0.0.1";

#define ELF_PRINT_FORMAT "  %-35s%s\n"

static char error_info[1024];
static int display_header = 0;
static int display_section_table = 0;
static int display_symbol_table = 0;
static int display_relocations = 0;
// static bool is_pie(ELF *ELF) {
//     Elf_Internal_Dyn *entry;

//     if (ELF->dynamic_size == 0)
//         locate_dynamic_section(ELF);
//     if (ELF->dynamic_size <= 1)
//         return false;

//     if (!get_dynamic_section(ELF))
//         return false;

//     for (entry = ELF->dynamic_section; entry < ELF->dynamic_section + ELF->dynamic_nent; entry++) {
//         if (entry->d_tag == DT_FLAGS_1) {
//             if ((entry->d_un.d_val & DF_1_PIE) != 0)
//                 return true;
//             break;
//         }
//     }
//     return false;
// }
static int is_pie() {
    return 1;
}

typedef struct ELF {
    void *addr;
    Elf64_Ehdr ehdr;   // ELF头
    Elf64_Shdr *shdr;  // 段表
    Elf64_Off shstrtab_offset;
} ELF;

int get_elf_header(ELF *ELF_file_data) {
    // 具体内容见 man elf
    Elf64_Ehdr ehdr = ELF_file_data->ehdr;
    // #define EI_NIDENT 16

    // typedef struct {
    //     unsigned char e_ident[EI_NIDENT];
    //     uint16_t      e_type;
    //     uint16_t      e_machine;
    //     uint32_t      e_version;
    //     ElfN_Addr     e_entry;
    //     ElfN_Off      e_phoff;
    //     ElfN_Off      e_shoff;
    //     uint32_t      e_flags;
    //     uint16_t      e_ehsize;
    //     uint16_t      e_phentsize;
    //     uint16_t      e_phnum;
    //     uint16_t      e_shentsize;
    //     uint16_t      e_shnum;
    //     uint16_t      e_shstrndx;
    // } ElfN_Ehdr;
    printf("ELF Header:\n");
    printf("  Magic:   ");
    for (int i = 0; i < EI_NIDENT; i++) {
        printf("%2.2x ", ehdr.e_ident[i]);
    }
    printf("\n");
    // EI_CLASS
    //     The fifth byte identifies the architecture for this binary:

    //     ELFCLASSNONE  This class is invalid.
    //     ELFCLASS32    This defines the 32-bit architecture.  It supports machines with files and virtual
    //                 address spaces up to 4 Gigabytes.
    //     ELFCLASS64    This defines the 64-bit architecture.
    char *elf_class_name;
    switch (ehdr.e_ident[EI_CLASS]) {
        case ELFCLASS32: elf_class_name = "ELF32"; break;
        case ELFCLASS64: elf_class_name = "ELF64"; break;
        default:
        case ELFCLASSNONE: elf_class_name = "none"; break;
    }

    // EI_DATA
    //     The sixth byte specifies the data encoding of the processor-specific data  in  the  file.   Cur‐
    //     rently, these encodings are supported:

    //     ELFDATANONE   Unknown data format.
    //     ELFDATA2LSB   Two's complement, little-endian.
    //     ELFDATA2MSB   Two's complement, big-endian.
    char *elf_data_name;
    switch (ehdr.e_ident[EI_DATA]) {
        case ELFDATA2LSB: elf_data_name = "2's complement, little endian"; break;
        case ELFDATA2MSB: elf_data_name = "2's complement, big endian"; break;
        default:
        case ELFDATANONE: elf_data_name = "none"; break;
    }

    // EI_VERSION
    //     The seventh byte is the version number of the ELF specification:

    //     EV_NONE       Invalid version.
    //     EV_CURRENT    Current version.
    char *elf_version_name;
    switch (ehdr.e_ident[EI_VERSION]) {
        case EV_CURRENT: elf_version_name = "current"; break;
        case EV_NONE: elf_version_name = ""; break;
        default: elf_version_name = "unknown";
    }
    // EI_OSABI
    //     The  eighth  byte identifies the operating system and ABI to which the object is targeted.  Some
    //     fields in other ELF structures have flags and values that have platform-specific  meanings;  the
    //     interpretation of those fields is determined by the value of this byte.  For example:

    //     ELFOSABI_NONE        Same as ELFOSABI_SYSV
    //     ELFOSABI_SYSV        UNIX System V ABI
    //     ELFOSABI_HPUX        HP-UX ABI
    //     ELFOSABI_NETBSD      NetBSD ABI
    //     ELFOSABI_LINUX       Linux ABI
    //     ELFOSABI_SOLARIS     Solaris ABI
    //     ELFOSABI_IRIX        IRIX ABI
    //     ELFOSABI_FREEBSD     FreeBSD ABI
    //     ELFOSABI_TRU64       TRU64 UNIX ABI
    //     ELFOSABI_ARM         ARM architecture ABI
    //     ELFOSABI_STANDALONE  Stand-alone (embedded) ABI
    char *elf_osabi_name;
    switch (ehdr.e_ident[EI_OSABI]) {
        case ELFOSABI_NONE: elf_osabi_name = "UNIX - System V"; break;
        case ELFOSABI_HPUX: elf_osabi_name = "UNIX - HP-UX"; break;
        case ELFOSABI_NETBSD: elf_osabi_name = "UNIX - NetBSD"; break;
        case ELFOSABI_GNU: elf_osabi_name = "UNIX - GNU"; break;
        case ELFOSABI_SOLARIS: elf_osabi_name = "UNIX - Solaris"; break;
        case ELFOSABI_AIX: elf_osabi_name = "UNIX - AIX"; break;
        case ELFOSABI_IRIX: elf_osabi_name = "UNIX - IRIX"; break;
        case ELFOSABI_FREEBSD: elf_osabi_name = "UNIX - FreeBSD"; break;
        case ELFOSABI_TRU64: elf_osabi_name = "UNIX - TRU64"; break;
        case ELFOSABI_MODESTO: elf_osabi_name = "Novell - Modesto"; break;
        case ELFOSABI_OPENBSD: elf_osabi_name = "UNIX - OpenBSD"; break;
        case ELFOSABI_ARM: elf_osabi_name = "ARM architecture ABI"; break;
        case ELFOSABI_STANDALONE: elf_osabi_name = "Stand-alone (embedded) ABI"; break;
        default: elf_osabi_name = "unknown";
    }

    printf(ELF_PRINT_FORMAT, "Class:", elf_class_name);
    printf(ELF_PRINT_FORMAT, "Data:", elf_data_name);
    printf("  %-35s%d (%s)\n", "Version:", ehdr.e_version, elf_version_name);
    printf(ELF_PRINT_FORMAT, "OS/ABI:", elf_osabi_name);
    printf("  %-35s%u\n", "ABI Version:", ehdr.e_ident[EI_ABIVERSION]);

    // e_type This member of the structure identifies the object file type:

    //     ET_NONE         An unknown type.
    //     ET_REL          A relocatable file.
    //     ET_EXEC         An executable file.
    //     ET_DYN          A shared object.
    //     ET_CORE         A core file.
    char *elf_type_name;
    switch (ehdr.e_type) {
        case ET_NONE: elf_type_name = "NONE (None)"; break;
        case ET_REL: elf_type_name = "REL (Relocatable file)"; break;
        case ET_EXEC: elf_type_name = "EXEC (Executable file)"; break;
        case ET_DYN:
            // ELF_file_data
            if (is_pie()) {
                elf_type_name = "DYN (Position-Independent Executable file)";
            } else {
                elf_type_name = "DYN (Shared object file)";
            }
            break;
        case ET_CORE: elf_type_name = "CORE (Core file)"; break;
        // Processor Specific
        // OS Specific
        default: elf_type_name = "unknown";
    }
    printf(ELF_PRINT_FORMAT, "Type:", elf_type_name);
    // e_machine
    //     This member specifies the required architecture for an individual file.  For example:

    //     EM_NONE         An unknown machine
    //     EM_M32          AT&T WE 32100
    //     EM_SPARC        Sun Microsystems SPARC
    //     EM_386          Intel 80386
    //     EM_68K          Motorola 68000
    //     EM_88K          Motorola 88000
    //     EM_860          Intel 80860
    //     EM_MIPS         MIPS RS3000 (big-endian only)
    //     EM_PARISC       HP/PA
    //     EM_SPARC32PLUS  SPARC with enhanced instruction set
    //     EM_PPC          PowerPC
    //     EM_PPC64        PowerPC 64-bit
    //     EM_S390         IBM S/390
    //     EM_ARM          Advanced RISC Machines
    //     EM_SH           Renesas SuperH
    //     EM_SPARCV9      SPARC v9 64-bit
    //     EM_IA_64        Intel Itanium
    //     EM_X86_64       AMD x86-64
    //     EM_VAX          DEC Vax
    char *elf_machine_name;
    switch (ehdr.e_machine) {
        default: elf_machine_name = "An unknown machine"; break;
        case EM_M32: elf_machine_name = "AT&T WE 32100"; break;
        case EM_SPARC: elf_machine_name = "Sun Microsystems SPARC"; break;
        case EM_386: elf_machine_name = "Intel 80386"; break;
        case EM_68K: elf_machine_name = "Motorola 68000"; break;
        case EM_88K: elf_machine_name = "Motorola 88000"; break;
        case EM_860: elf_machine_name = "Intel 80860"; break;
        case EM_MIPS: elf_machine_name = "MIPS RS3000 (big-endian only)"; break;
        case EM_PARISC: elf_machine_name = "HP/PA"; break;
        case EM_SPARC32PLUS: elf_machine_name = "SPARC with enhanced instruction set"; break;
        case EM_PPC: elf_machine_name = "PowerPC"; break;
        case EM_PPC64: elf_machine_name = "PowerPC 64-bit"; break;
        case EM_S390: elf_machine_name = "IBM S/390"; break;
        case EM_ARM: elf_machine_name = "Advanced RISC Machines"; break;
        case EM_SH: elf_machine_name = "Renesas SuperH"; break;
        case EM_SPARCV9: elf_machine_name = "SPARC v9 64-bit"; break;
        case EM_IA_64: elf_machine_name = "Intel Itanium"; break;
        case EM_X86_64: elf_machine_name = "Advanced Micro Devices X86-64"; break;
        case EM_VAX: elf_machine_name = "DEC Vax"; break;
    }
    printf(ELF_PRINT_FORMAT, "Machine:", elf_machine_name);
    printf("  Version:                           0x%x\n", ehdr.e_version);
    printf("  Entry point address:               0x%llx\n", (unsigned long long)ehdr.e_entry);
    printf("  Start of program headers:          %lld (bytes into file)\n", (unsigned long long)ehdr.e_phoff);
    printf("  Start of section headers:          %lld (bytes into file)\n", (unsigned long long)ehdr.e_shoff);
    printf("  Flags:                             0x%x\n", ehdr.e_flags);
    printf("  Size of this header:               %d (bytes)\n", ehdr.e_ehsize);
    printf("  Size of program headers:           %d (bytes)\n", ehdr.e_phentsize);
    printf("  Number of program headers:         %d\n", ehdr.e_phnum);
    printf("  Size of section headers:           %d (bytes)\n", ehdr.e_shentsize);
    printf("  Number of section headers:         %d\n", ehdr.e_shnum);
    printf("  Section header string table index: %d\n", ehdr.e_shstrndx);
    return 0;
}

char *getSectionType(Elf64_Word section_type) {
    switch (section_type) {
        case SHT_NULL: return "NULL";
        case SHT_PROGBITS: return "PROGBITS";
        case SHT_SYMTAB: return "SYMTAB";
        case SHT_STRTAB: return "STRTAB";
        case SHT_RELA: return "RELA";
        case SHT_HASH: return "HASH";
        case SHT_DYNAMIC: return "DYNAMIC";
        case SHT_NOTE: return "NOTE";
        case SHT_NOBITS: return "NOBITS";
        case SHT_REL: return "REL";
        case SHT_SHLIB: return "";
        case SHT_DYNSYM: return "DYNSYM";
        default: return "";
    }
}

char *getSectionFlag(Elf64_Xword section_flag) {
    // https://sourceware.org/git?p=binutils-gdb.git;a=blob;f=binutils/readelf.c;h=b872876a8b660be19e1ffc66ee300d0bbfaed345;hb=HEAD#l6812

    // SHF_WRITE:段内容可以被写入.
    // SHF_ALLOC:段在程序执行时被分配内存.
    // SHF_EXECINSTR:段包含可执行指令.
    // SHF_MASKPROC:该位由处理器架构定义.
    static char flags[20];
    char *p = flags;
    if (section_flag & SHF_WRITE)
        *p++ = 'W';
    if (section_flag & SHF_ALLOC)
        *p++ = 'A';
    if (section_flag & SHF_EXECINSTR)
        *p++ = 'X';
    if (section_flag & SHF_MERGE)
        *p++ = 'M';
    if (section_flag & SHF_STRINGS)
        *p++ = 'S';
    if (section_flag & SHF_INFO_LINK)
        *p++ = 'I';
    if (section_flag & SHF_LINK_ORDER)
        *p++ = 'L';
    if (section_flag & SHF_OS_NONCONFORMING)
        *p++ = 'O';
    if (section_flag & SHF_GROUP)
        *p++ = 'G';
    if (section_flag & SHF_TLS)
        *p++ = 'T';
    if (section_flag & SHF_EXCLUDE)
        *p++ = 'E';
    if (section_flag & SHF_COMPRESSED)
        *p++ = 'C';
    *p = 0;
    return flags;
}

int get_elf_section_table(ELF *ELF_file_data) {
    int section_number = ELF_file_data->ehdr.e_shnum;
    // char name_buffer[256];
    printf("There are %d section headers, starting at offset 0x%lx:\n", section_number, ELF_file_data->ehdr.e_shoff);
    if (section_number > 1) {
        printf("\nSection Headers:\n");
    } else {
        printf("\nSection Header:\n");
    }

    printf("  [Nr] Name              Type             Address           Offset\n");
    printf("       Size              EntSize          Flags  Link  Info  Align\n");
    for (int i = 0; i < section_number; i++) {
        Elf64_Shdr *shdr = &ELF_file_data->shdr[i];
        char number[3] = " x";
        if (i / 10) {
            number[0] = '0' + i / 10;
        }
        number[1] = '0' + i % 10;
        char *section_type = getSectionType(shdr->sh_type);
        char *section_flag = getSectionFlag(shdr->sh_flags);
        char *section_name = (char *)(ELF_file_data->addr + ELF_file_data->shstrtab_offset + shdr->sh_name);
        if (strlen(section_name) > 16) {
            char short_section_name[18];
            strncpy(short_section_name, section_name, 12);
            strncpy(short_section_name + 12, "[...]", 6);
            short_section_name[17] = 0;
            section_name = short_section_name;
        }
        printf(
            "  [%2s] %-17s %-16s %016lx  %08lx\n", number, section_name, section_type, shdr->sh_addr, shdr->sh_offset);
        printf("       %016lx  %016lx %3s%8d%6d     %ld\n",
               shdr->sh_size,
               shdr->sh_entsize,
               section_flag,
               shdr->sh_link,
               shdr->sh_info,
               shdr->sh_addralign);
    }

    printf("Key to Flags:\n");
    printf("  W (write), A (alloc), X (execute), M (merge), S (strings), I (info),\n");
    printf("  L (link order), O (extra OS processing required), G (group), T (TLS),\n");
    printf("  C (compressed), x (unknown), o (OS specific), E (exclude),\n");
    printf("  D (mbind), l (large), p (processor specific)\n");
    return 0;
}

char *get_symbol_type(int st_info) {
    switch (st_info) {
        case STT_NOTYPE: return "NOTYPE";
        case STT_OBJECT: return "OBJECT";
        case STT_FUNC: return "FUNC";
        case STT_SECTION: return "SECTION";
        case STT_FILE: return "FILE";
        case STT_COMMON: return "COMMON";
        case STT_TLS: return "TLS";
        default: return "UNKNOWN";
    }
}

char *get_symbol_bind(int st_info) {
    switch (st_info) {
        case STB_LOCAL: return "LOCAL";
        case STB_GLOBAL: return "GLOBAL";
        case STB_WEAK: return "WEAK";
        default: return "UNKNOWN";
    }
}

char *get_symbol_ndx(Elf64_Section st_shndx) {
    static char buf[10];
    memset(buf, 0, 10);
    int i = 8;
    buf[9] = 0;
    switch (st_shndx) {
        case SHN_ABS: return "ABS";
        case SHN_COMMON: return "COM";
        case SHN_UNDEF: return "UND";
        default: {
            // 正常情况
            while (st_shndx) {
                buf[i--] = '0' + st_shndx % 10;
                st_shndx /= 10;
            }
            return buf + i + 1;
        }
    }
}

int get_elf_symbol_table(ELF *ELF_file_data) {
    // typedef struct {
    //     uint32_t      st_name;
    //     unsigned char st_info;
    //     unsigned char st_other;
    //     uint16_t      st_shndx;
    //     Elf64_Addr    st_value;
    //     uint64_t      st_size;
    // } Elf64_Sym;

    int section_number = ELF_file_data->ehdr.e_shnum;
    Elf64_Sym *symtabs;  // 符号表指针
    int symtab_number;   // 符号表表项的个数
    for (int i = 0; i < section_number; i++) {
        Elf64_Shdr *shdr = &ELF_file_data->shdr[i];
        // 判断是符号表
        if ((shdr->sh_type == SHT_SYMTAB) || (shdr->sh_type == SHT_DYNSYM)) {
            // 符号表的段名
            char *section_name = (char *)(ELF_file_data->addr + ELF_file_data->shstrtab_offset + shdr->sh_name);
            // 符号表指向的字符串表
            Elf64_Shdr *strtab = &ELF_file_data->shdr[shdr->sh_link];

            symtabs = ELF_file_data->addr + shdr->sh_offset;
            symtab_number = shdr->sh_size / sizeof(Elf64_Sym);
            printf("\nSymbol table '%s' contains %d entries:\n", section_name, symtab_number);
            printf("   Num:    Value          Size Type    Bind   Vis      Ndx Name\n");
            for (int j = 0; j < symtab_number; j++) {
                // st_info 的低4位用于符号类型 0-3
                // st_info 的高4位用于符号绑定信息 4-7
                char *symbol_type = get_symbol_type(symtabs[j].st_info & 0x0f);
                char *symbol_bind = get_symbol_bind((symtabs[j].st_info & 0xf0) >> 4);
                char *symbol_ndx = get_symbol_ndx(symtabs[j].st_shndx);
                char *symbol_name;
                // 对于 st_name 的值不为0的符号或者 ABS, 去对应的 .strtab 中找
                if (symtabs[j].st_name || symtabs[j].st_shndx == SHN_ABS) {
                    symbol_name = (char *)(ELF_file_data->addr + strtab->sh_offset + symtabs[j].st_name);
                } else {
                    // 为 0 说明是一个特殊符号, 用 symbol_ndx 去段表字符串表中找
                    symbol_name = (char *)(ELF_file_data->addr + ELF_file_data->shstrtab_offset +
                                           ELF_file_data->shdr[symtabs[j].st_shndx].sh_name);
                }
                if (strlen(symbol_name) > 21) {
                    // check_argparse_groups
                    // check_argparse_s[...]
                    char short_symbol_name[22];
                    strncpy(short_symbol_name, symbol_name, 16);
                    strncpy(short_symbol_name + 16, "[...]", 6);
                    short_symbol_name[21] = 0;
                    symbol_name = short_symbol_name;
                }

                // Vis 在 C/C++ 中未使用, 用于控制符号可见性, Rust/Swift 使用到了
                printf("%6d: %016lx %5ld %-8s%-6s %s %4s %s\n",
                       j,
                       symtabs[j].st_value,
                       symtabs[j].st_size,
                       symbol_type,
                       symbol_bind,
                       "DEFAULT",
                       symbol_ndx,
                       symbol_name);
            }
        }
    }
    return 0;
}

char *get_elf_relocation_type_name(int type) {
    switch (type) {
        // x86_64 架构的重定位类型
        case R_X86_64_NONE: return "NONE";
        case R_X86_64_64: return "R_X86_64_64";
        case R_X86_64_PC32: return "R_X86_64_PC32";
        case R_X86_64_PLT32: return "R_X86_64_PLT32";
        case R_X86_64_GOTPCREL: return "R_X86_64_GOTPCREL";
        case R_X86_64_GOTPCRELX: return "R_X86_64_GOTPCRELX";
        case R_X86_64_COPY: return "R_X86_64_COPY";
        case R_X86_64_JUMP_SLOT: return "R_X86_64_JUMP_SLO";
        case R_X86_64_RELATIVE: return "R_X86_64_RELATIVE";
        case R_X86_64_GLOB_DAT: return "R_X86_64_GLOB_DAT";
        // 其他架构的重定位类型
        // ...
        default: return "UNKNOWN";
    }
}

int get_elf_relocation_table(ELF *ELF_file_data) {
    // typedef struct {
    //     Elf64_Addr r_offset;
    //     uint64_t r_info;
    //     int64_t r_addend;
    // } Elf64_Rela;
    int section_number = ELF_file_data->ehdr.e_shnum;
    Elf64_Rela *relocation_table;      // 重定位表
    int relocation_table_item_number;  // 重定位表表项的数量
    
    int matched = 0;
    for (int i = 0; i < section_number; i++) {
        Elf64_Shdr *shdr = &ELF_file_data->shdr[i];
        // 对于重定位表
        if (shdr->sh_type == SHT_RELA) {
            matched = 1;
            // 符号表的段名
            char *section_name = (char *)(ELF_file_data->addr + ELF_file_data->shstrtab_offset + shdr->sh_name);
            // Link 指向的符号表
            Elf64_Shdr *symbol_table = ELF_file_data->addr + ELF_file_data->shdr[shdr->sh_link].sh_offset;
            // 通过符号表的 Link 找到符号表的字符串表
            Elf64_Shdr *strtab = ELF_file_data->addr + ELF_file_data->shdr[ELF_file_data->shdr[shdr->sh_link].sh_link].sh_offset;
            // Info 指向所重定位的段
            // char *relocated_section_name = (char *)(ELF_file_data->addr + ELF_file_data->shstrtab_offset + ELF_file_data->shdr[shdr->sh_info].sh_name);

            relocation_table = ELF_file_data->addr + shdr->sh_offset;
            relocation_table_item_number = shdr->sh_size / sizeof(Elf64_Rela);
            printf("\nRelocation section '%s' at offset 0x%lx contains %d entries:\n",
                   section_name,
                   shdr->sh_offset,
                   relocation_table_item_number);
            printf("  Offset          Info           Type           Sym. Value    Sym. Name + Addend\n");
            for (int j = 0; j < relocation_table_item_number; j++) {
                int relocation_type = ELF64_R_TYPE(relocation_table[j].r_info);
                char *relocation_type_name = get_elf_relocation_type_name(relocation_type);
                int relocation_symbol_index = ELF64_R_SYM(relocation_table[j].r_info);
                Elf64_Sym symtab_item = ((Elf64_Sym *)symbol_table)[relocation_symbol_index];
                char *symbol_name;
                // 对于 st_name 的值不为0的符号或者 ABS, 去对应的 .strtab 中找
                if (symtab_item.st_name || symtab_item.st_shndx == SHN_ABS) {
                    symbol_name = (char *)((char *)strtab + symtab_item.st_name);
                } else {
                    // 为 0 说明是一个特殊符号, 用 symbol_ndx 去段表字符串表中找
                    symbol_name = (char *)(ELF_file_data->addr + ELF_file_data->shstrtab_offset +
                                           ELF_file_data->shdr[symtab_item.st_shndx].sh_name);
                }

                printf("%012lx  %012lx %-18s%016ld %s ",
                       relocation_table[j].r_offset,
                       relocation_table[j].r_info,
                       relocation_type_name,
                       symtab_item.st_value,
                       symbol_name);
                if (relocation_table[j].r_addend >= 0) {
                    printf("+ %ld\n",relocation_table[j].r_addend);
                } else {
                    printf("- %ld\n", -relocation_table[j].r_addend);
                }
            }
        }
    }
    if (!matched) {
        printf("\nThere are no relocations in this file.\n");
    }
    return 0;
}

int main(int argc, const char **argv) {
    char **file_names;
    argparse_option options[] = {
        XBOX_ARG_BOOLEAN(NULL, [-H][--help][help = "show help information"]),
        XBOX_ARG_BOOLEAN(NULL, [-v][--version][help = "show version"]),
        XBOX_ARG_BOOLEAN(&display_header, [-h]["--file-header"][help = "Display the ELF file header"]),
        XBOX_ARG_BOOLEAN(&display_section_table, [-S]["--section-headers"][help = "Display the sections' header"]),
        XBOX_ARG_BOOLEAN(&display_section_table, [--sections][help = "An alias for --section-headers"]),
        XBOX_ARG_BOOLEAN(&display_symbol_table, [-s][--syms][help = "Display the symbol table"]),
        XBOX_ARG_BOOLEAN(&display_symbol_table, [--symbols][help = "An alias for --syms"]),
        XBOX_ARG_BOOLEAN(&display_relocations, [-r][--relocs][help = "Display the relocations (if present)"]),
        XBOX_ARG_STRS_GROUP(&file_names, [name = files]),
        XBOX_ARG_END()};

    XBOX_argparse parser;
    XBOX_argparse_init(&parser, options, XBOX_ARGPARSE_ENABLE_ARG_STICK);
    XBOX_argparse_describe(&parser, "readelf", "Display information about the contents of ELF format files", "");
    XBOX_argparse_parse(&parser, argc, argv);

    if (XBOX_ismatch(&parser, "help")) {
        XBOX_argparse_info(&parser);
        XBOX_free_argparse(&parser);
        return 0;
    }

    if (XBOX_ismatch(&parser, "version")) {
        printf("%s\n", VERSION);
        XBOX_free_argparse(&parser);
        return 0;
    }

    int n = XBOX_ismatch(&parser, "files");
    if (!n) {
        printf("readelf Warning: Nothing to do.\n");
        XBOX_argparse_info(&parser);
    }
    for (int i = 0; i < n; i++) {
        ELF ELF_file_data;
        int fd = open(file_names[i], O_RDONLY);
        if (fd < 0) {
            snprintf(error_info, 1024, "open fail: %s", file_names[i]);
            perror(error_info);
            XBOX_free_argparse(&parser);
            return 1;
        }
        
        // 对 ELF 文件做完整的内存映射, 保存在 ELF_file_data.addr 中, 方便后面寻址
        off_t size = lseek(fd, 0, SEEK_END);
        void *addr = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
        if (addr == MAP_FAILED) {
            snprintf(error_info, 1024, "mmap fail: %s", file_names[i]);
            perror(error_info);
            close(fd);
            XBOX_free_argparse(&parser);
            exit(1);
        }
        ELF_file_data.addr = addr;

        // 读取 ELF 头, 保存在 ELF_file_data.ehdr 中
        lseek(fd, 0, SEEK_SET);
        if (read(fd, &ELF_file_data.ehdr, sizeof(Elf64_Ehdr)) < 0) {
            snprintf(error_info, 1024, "read fail: %s", file_names[i]);
            perror(error_info);
            munmap(addr, size);
            close(fd);
            XBOX_free_argparse(&parser);
            exit(1);
        }

        int section_number = ELF_file_data.ehdr.e_shnum; // 段的数量
        unsigned long long section_table_offset = ELF_file_data.ehdr.e_shoff; // 段表
        lseek(fd, section_table_offset, SEEK_SET);

        ELF_file_data.shdr = malloc(sizeof(Elf64_Shdr) * section_number);
        if (read(fd, ELF_file_data.shdr, sizeof(Elf64_Shdr) * section_number) < 0) {
            perror("read");
            munmap(addr, size);
            close(fd);
            XBOX_free_argparse(&parser);
            free(ELF_file_data.shdr);
            exit(1);
        }

        // 段表字符串表的索引
        int section_string_index = ELF_file_data.ehdr.e_shstrndx;

        // 段表字符串表的偏移量
        Elf64_Off shstrtab_offset = ELF_file_data.shdr[section_string_index].sh_offset;
        ELF_file_data.shstrtab_offset = shstrtab_offset;
        if (display_header) {
            get_elf_header(&ELF_file_data);
        }
        if (display_section_table) {
            get_elf_section_table(&ELF_file_data);
        }
        if (display_symbol_table) {
            get_elf_symbol_table(&ELF_file_data);
        }
        if (display_relocations) {
            get_elf_relocation_table(&ELF_file_data);
        }
        munmap(addr, size);
        close(fd);
        free(ELF_file_data.shdr);
    }
    XBOX_free_argparse(&parser);
    return 0;
}

# readelf

```bash
(base) kamilu@LZX:~/binutils$ ./src/readelf --help
Usage: readelf [OPTION]... [files]...
Display information about the contents of ELF format files

  -H   --help                show help information
  -v   --version             show version
  -h   --file-header         Display the ELF file header
  -S   --section-headers     Display the sections' header
       --sections            An alias for --section-headers
  -s   --syms                Display the symbol table
       --symbols             An alias for --syms
  -r   --relocs              Display the relocations (if present)
  -l   --program-header      Display the program headers
       --segments            An alias for --program-headers
  -T   --silent-truncation   If a symbol name is truncated, do not add [...] suffix
```

## ELF 文件标准历史

20世纪90年代, 一些厂商联合成立了一个委员会, 起草并发布了一个 ELF 文件格式标准供公开使用, 并希望所有人都可以遵循这项标准并从中获益. 1993 年委员会发布了 [ELF 文件标准](https://refspecs.linuxfoundation.org/elf/TIS1.1.pdf), 当时参与该委员会的有来自于编译器的厂商, 比如 Watcom(Watcom C/C++ 编译器) 和 Borland(Borland Turbo Pascal 编译器); 来自 CPU 的厂商比如 IBM 和 Intel; 来自操作系统的厂商比如IBM 和 Microsoft. 1995 年委员会发布了 [ELF1.2标准](https://refspecs.linuxfoundation.org/elf/elf.pdf), 自此委员会完成了自己的使命, 不久就解散了, 所以 ELF 文件格式标准的最新版本也是最后一个版本就是 1.2

> https://refspecs.linuxfoundation.org/

readelf 需要传入一个 ELF 文件名, 以下的内容基于 examples/SimpleSection.c, 会随 make 一同编译, 最后得到 SimpleSection.o

```c
int printf(const char *format, ...);

int global_init_var = 84;
int global_uninit_var;

void func1(int i) {
    printf("%d\n",i);
}

int main(void) {
    static int static_var = 85;
    static int static_var2;
    int a = 1;
    int b;
    func1(static_var + static_var2 + a + b);
}
```

## ELF 文件格式

### 基本结构

ELF 文件的作用有两个,一是用于程序链接(为了生成程序);二是用于程序执行(为了运行程序). 从链接和运行的角度,可以将 ELF 文件的组成部分划分为 链接视图 和 运行试图 这两种格式, 如下所示.

ELF 目标文件格式的最前部是 ELF 文件头(ELF Header), 它包含了描述整个文件的基本属性. 接着是 ELF 文件各个段, 其中 ELF 文件中与段有关的重要结构就是**段表**(Section Header Table), 该表描述了 ELF 包含的所有段的信息, 比如每个段的段名,段长度,文件中的偏移量,读写权限和段的其他属性

![20230819194148](https://raw.githubusercontent.com/learner-lu/picbed/master/20230819194148.png)

其中右侧可执行程序中的 segment 实际上使用的方式是将多个 .o 中各个段合并到一起, 相同性质的段组合为一个大段, 如下所示

![20230515145513](https://raw.githubusercontent.com/learner-lu/picbed/master/20230515145513.png)

> 正常来说 section 应该翻译为节, segment 翻译为段, 由于下文我们讨论单个目标文件的 ELF 格式, section 统称为段

使用 `readelf -S` 可以查看 SimpleSection.o 中所有的段的偏移量和大小, 可以看到一共有 14 个段

```bash
$ readelf -S SimpleSection.o
There are 14 section headers, starting at offset 0x410:

Section Headers:
  [Nr] Name              Type             Address           Offset
       Size              EntSize          Flags  Link  Info  Align
  [ 0]                   NULL             0000000000000000  00000000
       0000000000000000  0000000000000000           0     0     0
  [ 1] .text             PROGBITS         0000000000000000  00000040
       0000000000000064  0000000000000000  AX       0     0     1
  [ 2] .rela.text        RELA             0000000000000000  000002f0
       0000000000000078  0000000000000018   I      11     1     8
  [ 3] .data             PROGBITS         0000000000000000  000000a4
       0000000000000008  0000000000000000  WA       0     0     4
  [ 4] .bss              NOBITS           0000000000000000  000000ac
       0000000000000008  0000000000000000  WA       0     0     4
  [ 5] .rodata           PROGBITS         0000000000000000  000000ac
       0000000000000004  0000000000000000   A       0     0     1
  [ 6] .comment          PROGBITS         0000000000000000  000000b0
       000000000000002e  0000000000000001  MS       0     0     1
  [ 7] .note.GNU-stack   PROGBITS         0000000000000000  000000de
       0000000000000000  0000000000000000           0     0     1
  [ 8] .note.gnu.pr[...] NOTE             0000000000000000  000000e0
       0000000000000020  0000000000000000   A       0     0     8
  [ 9] .eh_frame         PROGBITS         0000000000000000  00000100
       0000000000000058  0000000000000000   A       0     0     8
  [10] .rela.eh_frame    RELA             0000000000000000  00000368
       0000000000000030  0000000000000018   I      11     9     8
  [11] .symtab           SYMTAB           0000000000000000  00000158
       0000000000000138  0000000000000018          12     8     8
  [12] .strtab           STRTAB           0000000000000000  00000290
       0000000000000060  0000000000000000           0     0     1
  [13] .shstrtab         STRTAB           0000000000000000  00000398
       0000000000000074  0000000000000000           0     0     1
Key to Flags:
  W (write), A (alloc), X (execute), M (merge), S (strings), I (info),
  L (link order), O (extra OS processing required), G (group), T (TLS),
  C (compressed), x (unknown), o (OS specific), E (exclude),
  D (mbind), l (large), p (processor specific)
```

其中 ELF Header 对应第 0 个无名段, 1-13 section 依次对应右侧的段. 由于段的大小(Size) 和起始地址 (Offset) 都不确定, 所以我们需要一个结构体数组来保存各个段的信息, 在 ELF 文件中这个数据结构叫做 **(Section Header Table)段表** , 下图左侧最后一个即为整个 ELF 文件的段表, 值得一提的是**段表并不算一个段, 它是独立于段之外的, 用于记录所有段信息的一个数组**

![20230820212800](https://raw.githubusercontent.com/learner-lu/picbed/master/20230820212800.png)

根据输出信息可以画出整个 ELF 文件的排布, Offset 对应每一个段的起始位置, Size 对应段的大小, 0x000 - 0x40c 之间分别对应各个段, 0x40c - 0x410 对齐, 0x410 之后是段表, 段表一共0x380字节所以总字节数为 0x410 + 0x380 = 0x790 = 1936 字节, 也就是整个ELF文件的大小

![20230820224134](https://raw.githubusercontent.com/learner-lu/picbed/master/20230820224134.png)

那么如何找到段表的位置呢? 前面我们提到了 ELF Header 为整个 ELF 文件的文件头, 其中 ELF Header 的结构体如下所示, 相关结构体元素的含义以注释的形式说明. 我们可以使用 `readelf -h` 参数查看 ELF 文件头, 其中的 `e_shoff` 就记录了段表在整个 ELF 文件中的偏移地址, `e_shnum` 记录了段的个数

**因此我们只需要从文件开头开始读取 sizeof(Elf64_Ehdr) 字节, 就可以利用这个地址找到段表的位置了**

```c
typedef struct {
    unsigned char e_ident[EI_NIDENT]; // 一些信息
    uint16_t      e_type;             // 文件类型
    uint16_t      e_machine;          // CPU类型
    uint32_t      e_version;          // ELF版本号
    ElfN_Addr     e_entry;            // 入口地址
    ElfN_Off      e_phoff;            // 程序头入口
    ElfN_Off      e_shoff;            // 段表在文件中的偏移
    uint32_t      e_flags;            // 标志位
    uint16_t      e_ehsize;           // ELF文件头大小
    uint16_t      e_phentsize;        // 程序头大小
    uint16_t      e_phnum;            // 程序头个数
    uint16_t      e_shentsize;        // 段表描述符大小, 等同于 sizeof(ElfN_Ehdr)
    uint16_t      e_shnum;            // 段表描述符数量
    uint16_t      e_shstrndx;         // 段表字符串表的在段表中的索引值
} ElfN_Ehdr;
```

![20230820224330](https://raw.githubusercontent.com/learner-lu/picbed/master/20230820224330.png)

> 其中e_entry入口地址指 ELF 程序的虚拟入口地址, 操作系统在加载完该程序后从这个而地址开始执行进程的指令, 可重定位文件一般没有入口地址, 对于可执行文件来说有 program_header 和 entry 的概念

关于 ELF 的所有定义都在 `/usr/include/elf.h` 中, ELF 兼顾了32/64位机器, 所以在实际编写程序时需要根据目标机器将 N 替换为 32 或 64, 对于 32/64 位机器也有不同的 size 大小, 这里我们只考虑 64 位机的情况

|typedef|type|32-bit size (bytes)|64-bit size (bytes)|
|:--:|:--:|:--:|:--:|
|ElfN_Addr|Unsigned program address, uintN_t|4|8|
|ElfN_Off|Unsigned file offset, uintN_t|4|8|
|ElfN_Section|Unsigned section index, uint16\_t|2|2|
|ElfN_Versym|Unsigned version symbol information, uint16\_t|2|2|
|Elf\_Byte|unsigned char|1|1|
|ElfN\_Half|uint16\_t|2|2|
|ElfN\_Sword|int32\_t|4|4|
|ElfN\_Word|uint32\_t|4|4|
|ElfN\_Sxword|int64\_t|8|8|
|ElfN\_Xword|uint64\_t|8|8|


> 读者可以使用 `man elf` 来查看相关信息

可以看到最前面的四个字节 `7f 45 4c 46` 是所有 ELF 文件都必须相同的标识码, 第一个字节对应 ASCII 的 DEL 控制符, 后面三个字节对应 ELF 三个字符的ASCII码, 这四个字节被称为 ELF 文件的**魔数**, 几乎所有可执行文件的开始的几个字节都是魔数

![20230506004437](https://raw.githubusercontent.com/learner-lu/picbed/master/20230506004437.png)

比如 a.out 开始的为 0x01 0x07, PE/COFF 为 0x4d 0x5a(MZ). 魔数用来确定文件的类型, 操作系统在加载可执行文件的时候会确认魔数是否正确, 如果不正确会拒绝加载

> 见 [fs/binfmt_elf.c:load_elf_library](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/fs/binfmt_elf.c#n1368), 在加载时判断了 if (memcmp(elf_ex.e_ident, ELFMAG, SELFMAG) != 0) goto out;

### 段表

通过 ELF Header 找到了段表, 段表实际上是一个数组, 数组中每一个元素都是 `Elf64_Shdr` 结构体, 用于储存每一个段的信息, 可以计算得到该结构体占据 (4x4+8x6) = 64 个字节, 其定义如下所示

```c
typedef struct {
    uint32_t   sh_name;      // 段名
    uint32_t   sh_type;      // 段类型
    uint64_t   sh_flags;     // 段标志位
    Elf64_Addr sh_addr;      // 段虚拟地址
    Elf64_Off  sh_offset;    // 段偏移
    uint64_t   sh_size;      // 段长度
    uint32_t   sh_link;      // 段链接信息
    uint32_t   sh_info;      // 段链接信息
    uint64_t   sh_addralign; // 段地址对齐
    uint64_t   sh_entsize;   // 段条目的长度
} Elf64_Shdr;
```

当然这里也可以使用 `readelf -h` 列出 ELF Header 查看到一共有段表中有14项,每一项对应每一个段都是64字节, 总大小为 64x14= 896B = 0x380

![20230506010254](https://raw.githubusercontent.com/learner-lu/picbed/master/20230506010254.png)

因此找到每一个段的流程如下, **ELF 文件中的每一个段的设计都是结构体数组**, 段表中为 `Elf64_Shdr` 数组, 重定位表为 `Elf64_Rela` 数组, 符号表为 `Elf64_Sym` 数组, 程序头为 `Elf64_Phdr` 数组:

![20230824083626](https://raw.githubusercontent.com/learner-lu/picbed/master/20230824083626.png)

值得注意的是上文中 Elf64_Shdr 结构体的字段 `sh_name` 的含义是段名, 但是类型是 uint32_t 而非 char*, 这是因为段本身并不记录其名字, 段的名字在 `.shstrtab` (段表字符串表)中统一记录, `sh_name` 是一个索引值. 采用这种方式就可以固定下来 `Elf64_Shdr` 结构体的大小, 因为 ELF 文件中用到了很多字符串, 比如段名,变量名等等, 字符串的长度往往是不确定的, 所以用固定的结构来表示它比较困难. 一种常见的做法就是把字符串集中起来存在一个表中, 这一点和ext文件系统的inode对于文件名的管理方式有些类似

> 0x410 之后是段表, 0x398 开始是 .shstrtab(段表字符串表), 记录了每一个段的名字

![20230506004340](https://raw.githubusercontent.com/learner-lu/picbed/master/20230506004340.png)

因此找到每一个段名字的方法如下, 首先找到 shstrtab, 然后利用每一个段 shdr 中的 sh_name 作为偏移量计算地址

![20230824090541](https://raw.githubusercontent.com/learner-lu/picbed/master/20230824090541.png)

### 符号表和重定位表

符号表(.symtab) 和 重定位表(.rela) 对于 ELF 文件来说是很重要的, 涉及到之后链接器 ld. 符号表的 sh_type 为 `SHT_SYMTAB` 或 `SHT_DYNSYM`, 重定位表的 sh_type 是 `SHT_RELA`, 遍历所有段判断 sh_type 类型即可



如果段的类型是与链接相关的, 比如重定位表(.rela)和符号表(.symtab), 那么 `sh_link` 和 `sh_info` 这两个段就有含义, 否则是无意义的.

对于重定位表 RELA, `sh_link` 代表该段所对应的符号表(.symbol)的下标, `sh_info` 表示它作用的重定位的段. 如下所示

> 见 src/readelf.c 中的 `display_elf_relocation_table`

![20230821095550](https://raw.githubusercontent.com/learner-lu/picbed/master/20230821095550.png)

重定位表中的每一个表项都是一个 Elf64_Rela 结构体, 成员含义如下所示

```c
typedef struct{
    Elf64_Addr r_offset;     /* 地址 */
    Elf64_Xword r_info;      /* 重定位类型和符号索引 */
    Elf64_Sxword r_addend;   /* 偏移量 */
} Elf64_Rela;
```

对于符号表 SYMTAB, `sh_link` 指向该段对应的字符串表(通常是 .strtab), st_info 的低4位用于符号类型, 高4位用于符号绑定信息

> 见 src/readelf.c 中的 `display_elf_symbol_table`

符号表的每一个表项都是一个 Elf64_Sym 结构体, 成员含义如下所示

```c
typedef struct {
    Elf64_Word st_name;      /* 符号名称(字符串表索引) */
    unsigned char st_info;   /* 符号类型和绑定 */
    unsigned char st_other;  /* 符号可见性 */
    Elf64_Section st_shndx;  /* 节索引 */
    Elf64_Addr st_value;     /* 符号值 */
    Elf64_Xword st_size;     /* 符号大小 */
} Elf64_Sym;
```

### 程序头(program header)

program header 在可重定位文件中不存在, 仅存在于已经完成链接的可执行文件, 可以使用 `-l` 选项查看

```c
typedef struct {
    Elf64_Word    p_type;          /* 段类型 */
    Elf64_Word    p_flags;         /* 段标志 */
    Elf64_Off     p_offset;        /* 段在文件中的偏移量 */
    Elf64_Addr    p_vaddr;         /* 段的虚拟地址 */
    Elf64_Addr    p_paddr;         /* 段的物理地址 */
    Elf64_Xword   p_filesz;        /* 段在文件中的大小 */
    Elf64_Xword   p_memsz;         /* 段在内存中的大小 */
    Elf64_Xword   p_align;         /* 段对齐方式 */
} Elf64_Phdr;
```

## 参考

- [ELF 文件解析 1-前述+文件头分析](https://zhuanlan.zhihu.com/p/380908650)
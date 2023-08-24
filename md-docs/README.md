# binutils

GNU binutils 实现

由于涉及到的二进制文件的查看, 推荐一个比较好用的 Vscode 插件: [hexdump](https://marketplace.visualstudio.com/items?itemName=slevesque.vscode-hexdump)

## 编译与使用

```bash
## 编译
make

## 安装
sudo make install

## 卸载
sudo make uninstall
```

## 目录

- [ ] ld - GNU 链接器.
- [ ] as - GNU 汇编器.
- [ ] gold - 一款新的、更快速的、仅支持 ELF 格式的链接器.
- [ ] addr2line - 将地址转换为文件名和行号.
- [ ] ar - 用于创建、修改和从归档中提取的实用工具.
- [ ] c++filt - 用于解码 C++ 符号的过滤器.
- [ ] dlltool - 用于创建用于构建和使用 DLL 的文件.
- [ ] elfedit - 允许对 ELF 格式文件进行修改.
- [ ] gprof - 显示性能分析信息.
- [ ] gprofng - 收集并显示应用程序性能数据.
- [ ] nlmconv - 将目标代码转换为 NLM 格式.
- [ ] nm - 列出目标文件中的符号.
- [ ] objcopy - 复制和转换目标文件.
- [ ] objdump - 显示目标文件中的信息.
- [ ] ranlib - 为归档内容生成索引.
- [x] readelf - 显示任何 ELF 格式目标文件的信息.
- [ ] size - 列出目标或归档文件的各节大小.
- [ ] strings - 列出文件中的可打印字符串.
- [ ] strip - 丢弃符号信息.
- [ ] windmc - 适用于 Windows 的兼容消息编译器.
- [ ] windres - 用于 Windows 资源文件的编译器.

## 参考

- [gnu binutils](https://www.gnu.org/software/binutils/)
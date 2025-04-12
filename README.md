# sFetch - SILVER ~~Simple~~ System Information Fetcher

## Description
sFetch is a lightweight system information tool that displays basic system details in a clean, ASCII-art accompanied format. It shows information such as:
    
- Hostname and username
- OS name and kernel version
- System architecture
- CPU model
- Memory usage (total and free)
- System uptime

## Author
Eduardo O. Santos <eduardooliveira.santos@yahoo.com.br>

## Installation
To install sFetch system-wide:

```bash
make
sudo make install
```

To install to a custom location:

```bash
make PREFIX=/your/custom/prefix
make install PREFIX=/your/custom/prefix
```

## Uninstallation
To remove sFetch:

```bash
sudo make uninstall
```

## Dependencies
sFetch requires:

- A C compiler (gcc or clang)
- Linux system (uses Linux-specific interfaces)
- Standard C library

## Usage
Simply run:

```bash
sFetch
```

## Contributing
Contributions are welcome! Please follow these guidelines:

- Fork the repository
- Create a feature branch
- Submit a pull request

## License
GPL3 - See LICENSE file for details.

## ASCII Art
The program comes with Void Linux ASCII art by default. To add support for other distributions:

- Add your ASCII art file to the res/ directory
- Modify the getASCIIArt() function in src/main.c to detect your OS and load the appropriate art

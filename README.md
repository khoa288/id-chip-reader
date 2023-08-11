# ID Chip Reader

ID Chip Reader is a C/C++ library for reading Vietnamese Identification Card Chips. The library extracts personal information and the identity photo from the chip, following the instructions of Doc 9303 Machine Readable Travel Documents as specified by the International Civil Aviation Organization (ICAO).

## Features

- Extract basic personal details and identity photo from Vietnamese ID card chips using BAC (Basic Access Control)
- Support for SAM and NFC card reading

## Requirements

- CMake version 3.8 or higher
- C99 compatible compiler
- Reader supports SAM or NFC card reading

## Building and Installation

To build and install the ID Chip Reader library, follow these steps:

1. Clone the repository:

```
git clone https://github.com/your_username/id-chip-reader.git
cd id-chip-reader
```

2. Create a build directory:

```
mkdir build && cd build
```

3. Configure the project using CMake:

```
cmake ..
```

4. Build and install the library:

```
make && sudo make install
```

5. (Optional) Build examples:

Enable `ID_CHIP_READER_EXAMPLES` option in `CMakeLists.txt` or pass `-DID_CHIP_READER_EXAMPLES=ON` during configuration step.

## Usage

Here's an example of how to use the ID Chip Reader library in your own project:

```c
#include <chip_reader.h>

int main() {
	unsigned char mrzInformation[] = "<MRZ_INFORMATION>";
	unsigned char imageFilePath[]  = "<IMAGE_FILE_PATH>";

	long res = ReadIdCardChip(mrzInformation, imageFilePath);

	return res;
}
```

Replace `<MRZ_INFORMATION>` with the MRZ information string obtained from the ID card, and `<IMAGE_FILE_PATH>` with the file path where you want to save the extracted identity photo.

## Documentation

The implementation instructions can be found in the [id_chip_reader_instruction.pdf](doc/id-chip-reader-instruction.pdf).

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
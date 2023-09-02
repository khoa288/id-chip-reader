# ID Chip Reader

ID Chip Reader is a C library for reading Vietnamese Identification Card Chips. The library extracts data from the chip, following the instructions of Doc 9303 Machine Readable Travel Documents as specified by the International Civil Aviation Organization (ICAO).

## Features

- Extract data from Vietnamese Identification Card Chips using BAC (Basic Access Control), including:
  - EF.COM: Common's card information
  - Data Group 1: MRZ information
  - Data Group 2: Portrait image
  - Data Group 13: Card ID number, Full name, Date of birth, Gender, Nationality, Ethnicity, Religion, Place of origin, Place of residence, Personal identification, Issued date, Expiration date, Father’s name, Mother’s name, and old ID number
- Support for SAM and NFC card reading

## Requirements

- CMake version 3.8 or higher
- C99 compatible compiler
- Reader supports SAM or NFC card reading

## Building and Installation

To build and install the library, follow these steps:

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

## Usage

Here's an example of how to use the ID Chip Reader library:

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

## Demo

![](doc/id-chip-reader-demo.gif)

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
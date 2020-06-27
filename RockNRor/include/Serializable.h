
#pragma once

#include <stdio.h>


namespace ROCKNROR {
;
namespace SYSTEM {
	
	class SerializeException : public std::exception {
		public:
		SerializeException(const char *m = "Serialize exception") : std::exception(m) {}
		~SerializeException(void) {};
	};


	class Serializable {
	public:

		// Serialize object to output stream (file). May throw SerializeException
		virtual long int Serialize(FILE *outputFile) const { return 0; }
		// Deserialize object from input stream (file). May throw SerializeException
		virtual bool Deserialize(FILE *inputFile) { return true; }

		// Serialize object to a file (path)
		// Returns written bytes, or -1 if failed
		long int SerializeToFile(const char *outputFilePath) {
			FILE *file = nullptr;
			errno_t e = fopen_s(&file, outputFilePath, "wb");
			if ((e != 0) || !file) {
				return -1;
			}
			long int size = 0;
			try {
				size = this->Serialize(file);
			}
			catch (SerializeException) {
				size = -1;
			}
			fclose(file);
			return size;
		}
		// Deserialize object from file (path).
		// Returns true if successful
		bool DeserializeFromFile(const char *inputFilePath) {
			FILE *file = nullptr;
			errno_t e = fopen_s(&file, inputFilePath, "rb");
			if ((e != 0) || !file) {
				return false;
			}
			bool res;
			try {
				res = this->Deserialize(file);
			}
			catch (SerializeException) {
				res = false;
			}
			fclose(file);
			return res;
		}

	protected:
		// Write to output file and return the number of written BYTES
		// Throws SerializeException if failed
		long int WriteBytes(FILE *outputFile, const void *data, size_t size) const {
			// 'struct size' set to 1, 'number' to write set to 'size', so that fwrite returns the number of written bytes
			size_t n = fwrite(data, 1, size, outputFile);
			if (n < size) {
				throw SerializeException("Failed to write bytes to output file");
			}
			return n;
		}
		// Read from input file, writes into outputBuffer, and returns the number of read BYTES
		// Throws SerializeException if failed
		long int ReadBytes(FILE *inputFile, void *inputBuffer, size_t size) {
			size_t n = fread(inputBuffer, 1, size, inputFile);
			if (n < size) {
				throw SerializeException("Failed to read bytes from input file");
			}
			return n;
		}
	};

}
}

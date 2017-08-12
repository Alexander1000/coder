#include <fstream>
#include <unistd.h>

namespace coder
{
  typedef uint8_t BYTE;

    class Buffer
    {
        public:
            virtual int Read(BYTE* buffer, int count) = 0;
            virtual int Write(BYTE* stream, int size) = 0;
    };

    class FileBuffer : Buffer
    {
        public:
            FileBuffer(const char *fileName)
            {
                this->fileName = (char*) fileName;
                this->hFile = NULL;
                this->eof = false;
                this->size = -1;
            }

            FileBuffer(char *fileName)
            {
                this->fileName = fileName;
                this->hFile = NULL;
                this->eof = false;
                this->size = -1;
            }

            ~FileBuffer()
            {
                this->Close();
            }
            
            void Open(const char *mode)
            {
                this->Open((char*) mode);
            }
            
            void Open(char *mode)
            {
                if (this->hFile == NULL) {
                    this->hFile = fopen(this->fileName, mode);
                }
            }

        int Read(BYTE* buffer, int count)
        {
            if (this->hFile == NULL) {
                this->hFile = fopen(this->fileName, "r");
            }

            size_t result = fread(buffer, sizeof(BYTE), count, this->hFile);

            if (result != count) {
                this->eof = true;
            }

            return (int) result;
        }

        int Write(BYTE* stream, int size)
        {
            if (this->hFile == NULL) {
                this->hFile = fopen(this->fileName, "w+");
            }

            size_t count = fwrite(stream, sizeof(BYTE), size, this->hFile);
            return (int) count;
        }
      
        int Size()
        {
            if (this->size > -1) {
                return this->size;
            }
            
            UINT64 curPosition = ftell(this->hFile);
            fseek(this->hFile, 0L, SEEK_END);
            this->size = ftell(this->hFile);
            fseek(this->hFile, curPosition, SEEK_SET);

            return this->size;
        }
        
        void Seek(UINT64 position)
        {
            fseek(this->hFile, position, SEEK_SET);
        }
        
        void SetSize(int size)
        {
            ftruncate(fileno(this->hFile), (off_t) size);
        }
        
        void Close()
        {
            fclose(this->hFile);
        }

        bool IsEof()
        {
            return this->eof;
        }

        protected:
            bool eof;
            FILE *hFile;
            char *fileName;
            int size;
  };

  class StdBuffer : Buffer
  {
    public:
      int Read(BYTE* buffer, int count)
      {
        // BYTE* buf = new BYTE[count];
        for (int i = 0; i < count; ++i) {
          cin >> buffer[i];
        }
        return 0;
      }

      int Write(BYTE* stream, int size)
      {
        return size;
      }
  };
}
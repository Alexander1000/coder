#include "buffer.cpp"

namespace coder
{       
    class Randomizer_2011
    {
	public:
            
            Randomizer_2011()
            {
		this->old_xi = time(0);
                this->old_ci = this->rotate(this->old_xi, this->old_xi >> 24);
            }

            UINT32 randomize(UINT32 max)
            {
                UINT32 A = 4164903690;
                UINT32 M = 0xFFFFFFFF;
                UINT32 Xi = ((A * this->old_xi) + this->old_ci) % M;
                UINT32 Ci = this->rotate(((A * this->old_xi) + this->old_ci), 16);
                this->old_xi = Xi;
                this->old_ci = Ci;
                return (Xi % max);
            }

            UINT32 rotate(UINT32 digit, UINT32 exp)
            {
                for (UINT32 i = 0; i < exp; i++)
                {
                    if ((digit & 0x80000000) == 0x80000000)
                    {
                        digit <<= 1;
                        digit |= 1;
                    }
                    else
                    {
                        digit <<= 1;
                    }
                }
                return digit;
            }

	private:
            UINT32 old_xi;
            UINT32 old_ci;
    };
    
  class Coder
  {
    public:
        Coder() {
        }

        void setInputFile(char *fileName)
        {
          this->fileInputBuffer = new FileBuffer(fileName);
        }

        void setOutputFile(char *fileName)
        {
          this->fileOutputBuffer = new FileBuffer(fileName);
        }
      
        void setKeyFileIn(char *fileName)
        {
            this->keyFileInBuffer = new FileBuffer(fileName);
        }
      
        void setKeyFileOut(char *fileName)
        {
            this->keyFileOutBuffer = new FileBuffer(fileName);
        }
      
        void encode()
        {
            this->fileInputBuffer->Open("r");
            UINT64 size = this->fileInputBuffer->Size();
            int count = 0;
            UINT64 Index = 0;
            UINT64 *key = new UINT64[16];
            
            count = this->keyFileInBuffer->Read((BYTE*) key, sizeof(UINT64) * 16);
          
            do {
		UINT64 A = 0, B = 0, C = 0, D = 0;
		UINT64 temp = 0;

                if (Index > 0) {
                    count = this->fileInputBuffer->Read((BYTE*) &temp, sizeof(UINT64));
                } else {
                    // кодируем в начале файла исходный размер
                    count = sizeof(UINT64);
                    temp = size;
                }
                
                if (count == 0) {
                    break;
                }

                memcpy(&A, &temp, count);

                count = this->fileInputBuffer->Read((BYTE*) &temp, sizeof(UINT64));
                memcpy(&B, &temp, count);
                
                if (count > 0) {
                    count = this->fileInputBuffer->Read((BYTE*) &temp, sizeof(UINT64));
                    memcpy(&C, &temp, count);
                } else {
                    count = 0;
                }

                if (count > 0) {
                    count = this->fileInputBuffer->Read((BYTE*) &temp, sizeof(UINT64));
                    memcpy(&D, &temp, count);
                } else {
                    count = 0;
                }

                for (int i = 0; i < 16; i++) {
                    this->EncodePart(&A, &B, &C, &D, &key[i]);
                }

                this->fileOutputBuffer->Write((BYTE*) &A, sizeof(UINT64));
                this->fileOutputBuffer->Write((BYTE*) &B, sizeof(UINT64));
                this->fileOutputBuffer->Write((BYTE*) &C, sizeof(UINT64));
                this->fileOutputBuffer->Write((BYTE*) &D, sizeof(UINT64));

                Index++;
            } while(count > 0);

            for (int i = 0; i < 16; i++) {
                this->keyFileOutBuffer->Write((BYTE*) &key[i], sizeof(UINT64));
            }
            
            this->fileInputBuffer->Close();
            this->fileOutputBuffer->Close();
            this->keyFileOutBuffer->Close();
            this->keyFileInBuffer->Close();
            free(key);
        }

        void decode()
        {
            UINT64 *key = new UINT64[16];
            int count = this->keyFileInBuffer->Read((BYTE*) key, sizeof(UINT64) * 16);
            this->fileInputBuffer->Open("r");
            UINT64 size = this->fileInputBuffer->Size();
            this->fileOutputBuffer->Open("w+");
            
            int tupleSize = sizeof(UINT64) * 4;
            UINT64 Index = size / tupleSize;
            UINT64 liPointer;
            UINT64 sourceSize = 0;

            do
            {
		UINT64 A = 0, B = 0, C = 0, D = 0;
		UINT64 temp = 0;
                
                liPointer = (Index - 1) * tupleSize;
                
                this->fileInputBuffer->Seek(liPointer);

                count = this->fileInputBuffer->Read((BYTE*) &temp, sizeof(UINT64));
		memcpy(&A, &temp, count);

		if (count > 0) {
                    count = this->fileInputBuffer->Read((BYTE*) &temp, sizeof(UINT64));
                    memcpy(&B, &temp, count);
		} else {
                    if (count == 0) break;
                    count = 0;
		}

                if (count > 0) {
                    count = this->fileInputBuffer->Read((BYTE*) &temp, sizeof(UINT64));
                    memcpy(&C, &temp, count);
                } else {
                    count = 0;
                }

                if (count > 0) {
                    count = this->fileInputBuffer->Read((BYTE*) &temp, sizeof(UINT64));
                    memcpy(&D, &temp, count);
                } else {
                    count = 0;
                }

                for (int i = 0; i < 16; i++) {
                    this->DecodePart(&A, &B, &C, &D, &key[15 - i]);
                }
                
                if (Index > 1) {
                    liPointer -= sizeof(UINT64);
                    this->fileOutputBuffer->Seek(liPointer);
                    this->fileOutputBuffer->Write((BYTE*) &A, sizeof(UINT64));
                } else {
                    this->fileOutputBuffer->Seek(0);
                    sourceSize = A;
                }

                this->fileOutputBuffer->Write((BYTE*) &B, sizeof(UINT64));
                this->fileOutputBuffer->Write((BYTE*) &C, sizeof(UINT64));
                this->fileOutputBuffer->Write((BYTE*) &D, sizeof(UINT64));

                Index--;
            } while (Index != 0);
            
            free(key);

            this->fileInputBuffer->Close();
            this->fileOutputBuffer->Close();
            this->keyFileInBuffer->Close();
        }

    protected:
        Buffer *inputBuffer;
        Buffer *outputBuffer;
        // source files
        FileBuffer *fileInputBuffer;
        FileBuffer *fileOutputBuffer;
        // key files
        FileBuffer *keyFileInBuffer;
        FileBuffer *keyFileOutBuffer;
        
        void EncodePart(UINT64 *A, UINT64 *B, UINT64 *C, UINT64 *D, UINT64 *key)
        {
            *C ^= *B;
            *B = this->F(*B);
            *A ^= *B;
            *D ^= this->Rotate(*C, 32);
            *A ^= *key;
            *B = this->Rotate(*B, 32);
            *C ^= *B;
            *D ^= *C;
            *C ^= *A;
            *B ^= *D;
            *A = this->F(*A);
            *key = this->Rotate(*key, 16);
            *key ^= *B;
            
            {
		UINT64 tempA = *A, tempB = *B, tempC = *C, tempD = *D;
		*B = tempA;
		*C = tempB;
		*D = tempC;
		*A = tempD;
            }
        }

        void DecodePart(UINT64 *A, UINT64 *B, UINT64 *C, UINT64 *D, UINT64 *key)
        {
            *B = G(*B);
            *key = this->Rotate((*key ^ *C), 48);
            *C ^= *A;
            *D ^= *B;
            *A ^= *D;
            *B ^= *key;
            *D ^= *C;
            *C = this->Rotate(*C, 32);
            *B ^= *C;
            *C = this->G(*C);
            *A ^= this->Rotate(*D, 32);
            *D ^= *C;
	
            {
		UINT64 tempA = *A, tempB = *B, tempC = *C, tempD = *D;
		*D = tempA;
		*A = tempB;
		*B = tempC;
		*C = tempD;
            }
        }
        
        UINT64 Rotate(UINT64 digit, UINT8 exp)
        {
            for (UINT8 i = 0; i < exp; i++) {
                if ((digit & 0x8000000000000000) == 0x8000000000000000) {
                    digit <<= 1;
                    digit |= 1;
                } else {
                    digit <<= 1;
                }
            }

            return digit;
        }

        UINT64 G(UINT64 digit)
        {
            union
            {
		UINT64 dig;
		UINT8 d[8];
            };
            
            dig = digit;

            {
		UINT8 t0 = d[0], t1 = d[1], t2 = d[2], t3 = d[3], t4 = d[4], t5 = d[5], t6 = d[6], t7 = d[7];

		d[2] = t0;
		d[3] = t1;
		d[4] = t2;
		d[5] = t3;
		d[6] = t4;
		d[7] = t5;
		d[0] = t6;
		d[1] = t7;
            }

            d[4] ^= d[2];
            d[5] ^= d[7];
            d[5] ^= d[4];
            d[0] ^= d[2];
            d[3] ^= d[6];
            d[1] ^= d[4];
            d[5] ^= d[7];
            d[6] ^= d[5];
            d[2] ^= d[5];
            d[7] ^= d[6];
            d[4] ^= d[3];
            d[0] ^= d[1];
	
            return dig;
        }

        UINT64 F(UINT64 digit)
        {
            union
            {
		UINT64 dig;
		UINT8 d[8];
            };
            
            dig = digit;

            d[0] ^= d[1];
            d[4] ^= d[3];
            d[7] ^= d[6];
            d[2] ^= d[5];
            d[6] ^= d[5];
            d[5] ^= d[7];
            d[1] ^= d[4];
            d[3] ^= d[6];
            d[0] ^= d[2];
            d[5] ^= d[4];
            d[5] ^= d[7];
            d[4] ^= d[2];

            {
		UINT8 t0 = d[0], t1 = d[1], t2 = d[2], t3 = d[3], t4 = d[4], t5 = d[5], t6 = d[6], t7 = d[7];

		d[6] = t0;
		d[7] = t1;
		d[0] = t2;
		d[1] = t3;
		d[2] = t4;
		d[3] = t5;
		d[4] = t6;
		d[5] = t7;
            }
	
            return dig;
        }
    };
}
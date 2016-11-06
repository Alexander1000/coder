#include <iostream>
#include <fstream>
#include <ctime>

using namespace std;

typedef uint64_t UINT64;
typedef uint32_t UINT32;
typedef uint8_t UINT8;

UINT64 F(UINT64 digit);
UINT64 G(UINT64 digit);

UINT64 Rotate(UINT64 digit, UINT8 exp);
void Encode(UINT64 *A, UINT64 *B, UINT64 *C, UINT64 *D, UINT64 *key);
void Decode(UINT64 *A, UINT64 *B, UINT64 *C, UINT64 *D, UINT64 *key);

class Randomizer_2011
{
	public:
    Randomizer_2011()
    {
		old_xi = time(0);
        old_ci = rotate(old_xi, old_xi >> 24);
    }

    UINT32 randomize(UINT32 max)
    {
        UINT32 A = 4164903690;
        UINT32 M = 0xFFFFFFFF;
        UINT32 Xi = ((A * old_xi) + old_ci) % M;
        UINT32 Ci = rotate(((A * old_xi) + old_ci), 16);
        old_xi = Xi;
        old_ci = Ci;
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

Randomizer_2011 randomizer;

int main()
{
	FILE *hFile1, *hFile2;
	char *szFile1 = new char[100];
	memset(szFile1, 0, 100);
	cout << "Then input file name: ";
	cin >> szFile1;
	hFile1 = fopen(szFile1, "w+");
	free(szFile1);
	char *szFile2 = new char[100];
	memset(szFile2, 0, 100);
	cout << "The output file name: ";
	cin >> szFile2;
	hFile2 = fopen(szFile2, "w+");
	free(szFile2);
	int action = 0;

	do {
		cout << "Select action:" << endl;
		cout << "1. Encode." << endl;
		cout << "2. Decode." << endl;
		cout << "> ";
		cin >> action;
	} while (action < 1 || action > 2);

	// encode
	if (action == 1) {
		// Generate 1024-bit key
		UINT64 *key = new UINT64[16];
		memset(key, 0, sizeof(UINT64) * 16);
		cout << "Select action for key: " << endl;
		cout << "1. Generate file-key." << endl;
		cout << "2. Read from file." << endl;
		cin >> action;

		if (action == 1) {
			// generate key
			for (int i = 0; i < 16; i++) {
				key[i] = ((UINT64) randomizer.randomize(0xFFFFFFFF) << 32) + randomizer.randomize(0xFFFFFFFF);
			}
			
			FILE *hKey1 = fopen("key1.bin", "w+");
			fwrite(key, sizeof(UINT64), 16, hKey1);
			fclose(hKey1);
		} else {
			char *szFileKey = new char [100];
			memset(szFileKey, 0, 100);
			cout << "Enter key-file: " << endl;
			cin >> szFileKey;
			FILE *hKey1 = fopen(szFileKey, "w+");
			fread(key, sizeof(UINT64), 16, hKey1);
			fclose(hKey1);
			free(szFileKey);
		}

		UINT64 size = 0;
		fseek(hFile1, 0L, SEEK_END);
		size = ftell(hFile1);
		size_t count = 0;
		UINT64 Index = 0;

		do {
			UINT64 A = 0, B = 0, C = 0, D = 0;
			UINT64 liPointer = Index * 4 * sizeof(UINT64);
			fseek(hFile1, liPointer, SEEK_SET);
			UINT64 temp = 0;
			count = fread(&temp, sizeof(UINT64), 1, hFile1);
			memcpy(&A, &temp, count);

			if(count == sizeof(UINT64)) {
				count = fread(&temp, sizeof(UINT64), 1, hFile1);
				memcpy(&B, &temp, count);
			} else {
				count = 0;
			}

			if(count == sizeof(UINT64)) {
				count = fread(&temp, sizeof(UINT64), 1, hFile1);
				memcpy(&C, &temp, count);
			} else {
				count = 0;
			}

			if(count == sizeof(UINT64)) {
				count = fread(&temp, sizeof(UINT64), 1, hFile1);
				memcpy(&D, &temp, count);
			} else {
				count = 0;
			}

			for (int i = 0; i < 16; i++) {
				Encode(&A, &B, &C, &D, &key[i]);
			}

			liPointer = Index * 4 * sizeof(UINT64);
			fseek(hFile2, liPointer, SEEK_SET);

			fwrite(&A, sizeof(UINT64), 1, hFile2);
			fwrite(&B, sizeof(UINT64), 1, hFile2);
			fwrite(&C, sizeof(UINT64), 1, hFile2);
			fwrite(&D, sizeof(UINT64), 1, hFile2);
			Index++;
		} while(count == sizeof(UINT64));

		FILE *hKey1;
		hKey1 = fopen("key2.bin", "w+");
		fwrite(key, sizeof(UINT64), 16, hKey1);
		fclose(hKey1);
		free(key);
	} else {
		// decode
		FILE *hKey;

		char *pName = new char [100];
		cout << "Enter key-file: ";
		cin >> pName;
		hKey = fopen(pName, "r");
		UINT64 *key = new UINT64[16];
		size_t count = 0;
		count = fread(key, sizeof(UINT64), 16, hKey);
		fclose(hKey);

		fseek(hFile1, 0L, SEEK_END);
		UINT64 size = ftell(hFile1);

		UINT64 liPointer;
		UINT64 Index = size / (sizeof(UINT64) * 4);

		do
		{
			UINT64 A = 0, B = 0, C = 0, D = 0;
			liPointer = (Index - 1) * 4 * sizeof(UINT64);
			fseek(hFile1, liPointer, SEEK_SET);
			UINT64 temp = 0;
			count = fread(&temp, sizeof(UINT64), 1, hFile1);
			memcpy(&A, &temp, count);

			if(count == sizeof(UINT64)) {
				count = fread(&temp, sizeof(UINT64), 1, hFile1);
				memcpy(&B, &temp, count);
			} else {
				if (count == 0) break;
				count = 0;
			}

			if (count == sizeof(UINT64)) {
				count = fread(&temp, sizeof(UINT64), 1, hFile1);
				memcpy(&C, &temp, count);
			} else {
				count = 0;
			}

			if (count == sizeof(UINT64)) {
				count = fread(&temp, sizeof(UINT64), 1, hFile1);
				memcpy(&D, &temp, count);
			} else {
				count = 0;
			}

			for (int i = 0; i < 16; i++) {
				Decode(&A, &B, &C, &D, &key[15 - i]);
			}

			liPointer = (Index - 1) * 4 * sizeof(UINT64);
			fseek(hFile2, liPointer, SEEK_SET);
			fwrite(&A, sizeof(UINT64), 1, hFile2);
			fwrite(&B, sizeof(UINT64), 1, hFile2);
			fwrite(&C, sizeof(UINT64), 1, hFile2);
			fwrite(&D, sizeof(UINT64), 1, hFile2);
			Index--;
		} while (Index != 0);

		free(key);
	}

	fclose(hFile1);
	fclose(hFile2);
	return 0;
}

void Encode(UINT64 *A, UINT64 *B, UINT64 *C, UINT64 *D, UINT64 *key)
{
	*C ^= *B;
	*B = F(*B);
	*A ^= *B;
	*D ^= Rotate(*C, 32);
	*A ^= *key;
	*B = Rotate(*B, 32);
	*C ^= *B;
	*D ^= *C;
	*C ^= *A;
	*B ^= *D;
	*A = F(*A);
	*key = Rotate(*key, 16);
	*key ^= *B;
	{
		UINT64 tempA = *A, tempB = *B, tempC = *C, tempD = *D;
		*B = tempA;
		*C = tempB;
		*D = tempC;
		*A = tempD;
	}
}

void Decode(UINT64 *A, UINT64 *B, UINT64 *C, UINT64 *D, UINT64 *key)
{
	*B = G(*B);
	*key = Rotate((*key ^ *C), 48);
	*C ^= *A;
	*D ^= *B;
	*A ^= *D;
	*B ^= *key;
	*D ^= *C;
	*C = Rotate(*C, 32);
	*B ^= *C;
	*C = G(*C);
	*A ^= Rotate(*D, 32);
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
	/*__asm
	{
		mov cl, exp
		rol digit, cl
	}*/
	asm("movb cl, exp; rol digit, cl;");
	// asm("mov cl, exp");
	// asm("rol digit, cl");
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
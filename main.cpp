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
				key[i] = (randomizer.randomize(0xFFFFFFFF) << 32) + randomizer.randomize(0xFFFFFFFF);
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
		DWORD dwSizeLow, dwSizeHight;
		dwSizeLow = GetFileSize(hFile1, &dwSizeHight);
		size += dwSizeLow;
		size += ((UINT64)dwSizeHight << 32);
		DWORD count = 0;
		UINT64 Index = 0;
		do
		{
			UINT64 A = NULL, B = NULL, C = NULL, D = NULL;
			LARGE_INTEGER liPointer;
			liPointer.QuadPart = Index * 4 * sizeof(UINT64);
			SetFilePointer(hFile1, liPointer.LowPart, &liPointer.HighPart, FILE_BEGIN);
			UINT64 temp = NULL;
			ReadFile(hFile1, &temp, sizeof(UINT64), &count, NULL);
			memcpy(&A, &temp, count);
			if(count == sizeof(UINT64))
			{
				ReadFile(hFile1, &temp, sizeof(UINT64), &count, NULL);
				memcpy(&B, &temp, count);
			}
			else
			{
				count = 0;
			}
			if(count == sizeof(UINT64))
			{
				ReadFile(hFile1, &temp, sizeof(UINT64), &count, NULL);
				memcpy(&C, &temp, count);
			}
			else
			{
				count = 0;
			}
			if(count == sizeof(UINT64))
			{
				ReadFile(hFile1, &temp, sizeof(UINT64), &count, NULL);
				memcpy(&D, &temp, count);
			}
			else
			{
				count = 0;
			}
			for (int i = 0; i < 16; i++)
			{
				Encode(&A, &B, &C, &D, &key[i]);
			}
			DWORD count2 = 0;
			liPointer.QuadPart = Index * 4 * sizeof(UINT64);
			SetFilePointer(hFile2, liPointer.LowPart, &liPointer.HighPart, FILE_BEGIN);
			WriteFile(hFile2, &A, sizeof(UINT64), &count2, NULL);
			WriteFile(hFile2, &B, sizeof(UINT64), &count2, NULL);
			WriteFile(hFile2, &C, sizeof(UINT64), &count2, NULL);
			WriteFile(hFile2, &D, sizeof(UINT64), &count2, NULL);
			Index++;
		}
		while(count == sizeof(UINT64));

		{
			HANDLE hKey1 = CreateFileA("key2.bin",
						GENERIC_READ | GENERIC_WRITE,
						FILE_SHARE_WRITE,
						NULL,
						CREATE_ALWAYS,
						FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
						NULL);
			DWORD count = 0;
			WriteFile(hKey1, key, sizeof(UINT64) * 16, &count, NULL);
			CloseHandle(hKey1);
		}
		delete key;
	}
	else
	{
		// decode
		HANDLE hKey = (HANDLE)0xFFFFFFFF;
		do
		{
			char *pName = new char [100];
			cout << "������� ����-����: ";
			cin >> pName;
			hKey = CreateFileA(pName,
						GENERIC_READ | GENERIC_WRITE,
						FILE_SHARE_WRITE,
						NULL,
						OPEN_EXISTING,
						FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
						NULL);
		}
		while(hKey == (HANDLE)0xFFFFFFFF);
		UINT64 *key = new UINT64[16];
		DWORD count = 0;
		ReadFile(hKey, key, 16 * sizeof(UINT64), &count, NULL);
		CloseHandle(hKey);

		UINT64 size = 0;
		DWORD dwSizeLow, dwSizeHight;
		dwSizeLow = GetFileSize(hFile1, &dwSizeHight);
		size += dwSizeLow;
		size += ((UINT64)dwSizeHight << 32);

		LARGE_INTEGER liPointer;
		UINT64 Index = size / (sizeof(UINT64) * 4);

		do
		{
			UINT64 A = NULL, B = NULL, C = NULL, D = NULL;
			liPointer.QuadPart = (Index - 1) * 4 * sizeof(UINT64);
			SetFilePointer(hFile1, liPointer.LowPart, &liPointer.HighPart, FILE_BEGIN);
			UINT64 temp = NULL;
			ReadFile(hFile1, &temp, sizeof(UINT64), &count, NULL);
			memcpy(&A, &temp, count);
			if(count == sizeof(UINT64))
			{
				ReadFile(hFile1, &temp, sizeof(UINT64), &count, NULL);
				memcpy(&B, &temp, count);
			}
			else
			{
				if (count == 0)break;
				count = 0;
			}
			if(count == sizeof(UINT64))
			{
				ReadFile(hFile1, &temp, sizeof(UINT64), &count, NULL);
				memcpy(&C, &temp, count);
			}
			else
			{
				count = 0;
			}
			if(count == sizeof(UINT64))
			{
				ReadFile(hFile1, &temp, sizeof(UINT64), &count, NULL);
				memcpy(&D, &temp, count);
			}
			else
			{
				count = 0;
			}
			for (int i = 0; i < 16; i++)
			{
				Decode(&A, &B, &C, &D, &key[15 - i]);
			}
			DWORD count2 = 0;
			liPointer.QuadPart = (Index - 1) * 4 * sizeof(UINT64);
			SetFilePointer(hFile2, liPointer.LowPart, &liPointer.HighPart, FILE_BEGIN);
			WriteFile(hFile2, &A, sizeof(UINT64), &count2, NULL);
			WriteFile(hFile2, &B, sizeof(UINT64), &count2, NULL);
			WriteFile(hFile2, &C, sizeof(UINT64), &count2, NULL);
			WriteFile(hFile2, &D, sizeof(UINT64), &count2, NULL);
			Index--;
		} while(Index != 0);
		delete key;
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
	__asm
	{
		mov cl, exp
		rol digit, cl
	}
    return digit;
}

UINT64 G(UINT64 digit)
{
	union
	{
		UINT64 dig;
		BYTE d[8];
	};
	dig = digit;

	{
		BYTE t0 = d[0], t1 = d[1], t2 = d[2], t3 = d[3], t4 = d[4], t5 = d[5], t6 = d[6], t7 = d[7];

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
		BYTE d[8];
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
		BYTE t0 = d[0], t1 = d[1], t2 = d[2], t3 = d[3], t4 = d[4], t5 = d[5], t6 = d[6], t7 = d[7];

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
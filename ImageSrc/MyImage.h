#pragma once
#include <Windows.h>
#include <assert.h>
#include <cstdio>
#define CLIP(x) (x < 0) ? 0 : x > 255 ? 255 : x		// CLIP() : 행 당 몇 바이트를 잡을 것인지 결정하는 함수
													// 아주 많은 BYTE 데이터들을 CPU가 처리하게 하기 위해
													// 32bit 데이터로 만들어 주는 ??? 4의 배수로 ??
													// 2 byte가 남으면 버리고 빠른 처리 속도 위해서 무조건 4 byte 단위로 만든다.
													// 데이터를 저장하다 보면 음수 값이 나올 수 있는데 영상 처리에서는 0 ~ 255의 값만 다루기 때문에
													// 음수면 0을 256 이상이면 255로 취급하여 사용한다.
template <typename T>
class CMyImage
{
protected:
	int m_nChannels;			// 채널 수, 1 byte (8 biy)당 1채널
	int m_nHeight;				// 세로 픽셀 수
	int m_nWidth;				// 가로 픽셀 수
	int m_nWStep;				// 행당 데이터 원소 수, 4의 배수, 무조건 4 byte 씩 할당 (모자라면 추가)
	T* m_pImageData;			// 픽셀 배열 포인터

public:
	CMyImage(void)
		: m_nChannels(0)
		, m_nHeight(0)
		, m_nWidth(0)
		, m_nWStep(0)
		, m_pImageData(NULL)
	{}

	CMyImage(int nWidth, int nHeight, int nChannels = 1)
		: m_nChannels(nChannels)
		, m_nHeight(nHeight)
		, m_nWidth(nWidth)
		, m_nWStep(((nWidth* nChannels * sizeof(T) + 3) & ~3) / sizeof(T))	// & ~3, 비트 연산자 3의 보수, ~0000011 -> 11111100
																			// LSB 두 개, 2^0 2^1 자리 사용 하지 않겠다.
																			// 2^2 = 4, 즉 가장 가까운 4의 배수만 사용하기 위한 연산이다.
																			// 3을 추가 ...
	{
		m_pImageData = new T[m_nHeight * m_nWStep];
	}

	CMyImage(const CMyImage& myImage) {
		m_nChannels = myImage.m_nChannels;
		m_nHeight = myImage.m_nHeight;
		m_nWidth = myImage.m_nWidth;
		m_nWStep = myImage.m_nWStep;
		m_pImageData = new T[m_nHeight * m_nWStep];										// 깊은 복사
		memcpy(m_pImageData, myImage.m_pImageData, m_nHeight * m_nWStep * sizeof(T));	// 메모리 복사
	}
		
	CMyImage& operator = (const CMyImage& myImage) {			// 연산자 중복 정의 -> 대입 연산자 '='
		if (this == &myImage)
			return *this;

		m_nChannels = myImage.m_nChannels;
		m_nHeight = myImage.m_nHeight;
		m_nWidth = myImage.m_nWidth;
		m_nWStep = myImage.m_nWStep;

		if (m_pImageData)
			delete[] m_pImageData;
		if (myImage.m_pImageData != NULL) {						// 저장할 공간에 데이터가 있으면
			m_pImageData = new T[m_nHeight * m_nWStep];
			memcpy(m_pImageData, myImage.m_pImageData, m_nHeight * m_nWStep * sizeof(T));
		}
		return *this;
	}

	template <typename From>							// 객체 리턴 및 대입 연산자 사용 시
	CMyImage(const CMyImage<From>& myImage) {			// 복사 생성자
		m_nChannels = myImage.GetChannel();
		m_nHeight = myImage.GetHeight();
		m_nWidth = myImage.GetWidth();
		m_nWStep = ((m_nWidth * m_nChannels * sizeof(T) + 3) & ~3) / sizeof(T);

		int nWStep = myImage.GetWStep();

		if (sizeof(T) == 1) {
			for (int r = 0; r < m_nHeight; r++) {
				T* pDst = GetPtr(r);					// 복사 해야 할 위치 주소 값, row의 값을 주면 해당 row의 주소 값 리턴
				From* pSrc = myImage.GetPtr(r);			// 원본에 대한 이미지 row의 주소 값 리턴
				for (int c = 0; c < nWStep; c++) {
					pDst[c] = (T)CLIP(pSrc[c]);
				}
			}
		}
		else {
			for (int r = 0; r < m_nHeight; r++) {
				T* pDst = GetPtr(r);
				From* pSrc = myImage.GetPtr(r);
				for (int c = 0; c < nWStep; c++) {
					pDst[c] = (T)pSrc[c];
				}
			}
		}
	}

	~CMyImage(void) {
		if (m_pImageData) delete[] m_pImageData;
	}

	bool LoadImage(const char* filename) {
		assert(sizeof(T) == 1);		// Byte 형의 경우만 가능

		if (!strcmp(".bmp", &filename[strlen(filename) - 4])) {
			FILE* pFile = NULL;
			fopen_s(&pFile, filename, "rb");		// 바이너리 읽기 모드
			if (!pFile) return false;

			BITMAPFILEHEADER fileHeader;

			if (!fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, pFile)) {
				fclose(pFile);
				return false;
			}
			if (fileHeader.bfType != 0x4D42) {
				fclose(pFile);
				return false;
			}

			BITMAPINFOHEADER infoHeader;

			if (!fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, pFile)) {
				fclose(pFile);
				return false;
			}
			if (infoHeader.biBitCount != 8 && infoHeader.biBitCount != 24) {
				fclose(pFile);
				return false;
			}

			if (m_nWidth != infoHeader.biWidth && m_nHeight != infoHeader.biHeight && m_nChannels != infoHeader.biBitCount / 8) {
				if (m_pImageData) delete[] m_pImageData;

				m_nChannels = infoHeader.biBitCount / 8;
				m_nHeight = infoHeader.biHeight;
				m_nWidth = infoHeader.biWidth;
				m_nWStep = (m_nWidth * m_nChannels * sizeof(T) + 3) & ~3;

				m_pImageData = new T[m_nHeight * m_nWStep];
			}

			fseek(pFile, fileHeader.bfOffBits, SEEK_SET);

			int r;
			for (r = m_nHeight - 1; r >= 0; r--) {
				if (!fread(&m_pImageData[r * m_nWStep], sizeof(BYTE), m_nWStep, pFile)) {
					fclose(pFile);
					return false;
				}
			}

			fclose(pFile);
			return true;
		}
		else {
			return false;
		}
	}

	bool SaveImage(const char* filename) {
		assert(sizeof(T) == 1);

		if (!strcmp(".bmp", &filename[strlen(filename) - 4])) {
			FILE* pFile = NULL;
			fopen_s(&pFile, filename, "wb");
			if (!pFile) return false;

			BITMAPFILEHEADER fileHeader; // 크기 : 14
			fileHeader.bfType = 0x4D42; // 'BM'
			fileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + m_nWStep * m_nHeight + (m_nChannels == 1) * 1024;
			fileHeader.bfReserved1 = 0;
			fileHeader.bfReserved2 = 0;
			fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (m_nChannels == 1) * 256 * sizeof(RGBQUAD);

			fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, pFile);

			BITMAPINFOHEADER infoHeader; // 크기 : 40 
			infoHeader.biSize = sizeof(BITMAPINFOHEADER);
			infoHeader.biWidth = m_nWidth;
			infoHeader.biHeight = m_nHeight;
			infoHeader.biPlanes = 1;
			infoHeader.biBitCount = m_nChannels * 8;
			infoHeader.biCompression = BI_RGB;
			infoHeader.biSizeImage = m_nWStep * m_nHeight;
			infoHeader.biClrImportant = 0;
			infoHeader.biClrUsed = 0;
			infoHeader.biXPelsPerMeter = 0;
			infoHeader.biYPelsPerMeter = 0;

			fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, pFile);

			if (m_nChannels == 1) {
				for (int l = 0; l < 256; l++) {
					RGBQUAD GrayPalette = { (BYTE)l, (BYTE)l, (BYTE)l, (BYTE)0 };	// 팔레트 정보 크기 : 1024 (4 * 256)
					fwrite(&GrayPalette, sizeof(RGBQUAD), 1, pFile);
				}
			}

			int r;
			for (r = m_nHeight - 1; r >= 0; r--) {
				fwrite(&m_pImageData[r * m_nWStep], sizeof(BYTE), m_nWStep, pFile);
			}

			fclose(pFile);
			return true;
		}
		else {
			return false;
		}
	}

	bool IsEmpty() const {
		return m_pImageData ? false : true;
	}

	void SetConstValue(T val) {
		if (val == 0) {
			memset(m_pImageData, 0, m_nWStep * m_nHeight * sizeof(T));
			return;
		}
		if (sizeof(T) == 1) {		// 사이즈 == byte 이면 아래 실행
			memset(m_pImageData, val, m_nWStep * m_nHeight);
		}
		else {
			T* pData = m_pImageData;
			for (int r = 0; r < m_nHeight; r++) {
				for (int c = 0; c < m_nWidth; c++) {
					pData[c] = val;
				}
				pData += m_nWStep;
			}
		}
	}

	inline T& GetAt(int x, int y, int c = 0) const {
		assert((x >= 0) && (x < m_nWidth) && (y >= 0) && (y < m_nHeight));
		return m_pImageData[m_nWStep * y + m_nChannels * x + c];
	}

	int GetChannel() const { return m_nChannels; }
	int GetHeight() const { return m_nHeight; }	
	int GetWidth() const { return m_nWidth; }
	int GetWStep() const { return m_nWStep; }
	T* GetPtr(int r = 0, int c = 0) const { return m_pImageData + r * m_nWStep + c; }		//row, colum의 행당 원소수를 곱해서 위치를 반환한다.
};

typedef CMyImage <BYTE	> CByteImage;
typedef CMyImage <int	> CIntImage;
typedef CMyImage <float	> CFloatImage;
typedef CMyImage <double> CDoubleImage;
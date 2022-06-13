#include <iostream>
#include "ImageSrc/MyImage.h"

using namespace std;

int main() {
	CByteImage image1(640, 480);	// 생성자 호출, 디폴트 매개변수(channels = 1 ) : 1 채널 사용 (0 ~ 255 bit만 사용) -> 흑백
	image1.SetConstValue(0);		// 0으로 초기화

	CByteImage image2(image1);		// 복사 생성자 호출
	CByteImage image3;				// 기본 생성자 호출
	image3 = image1;				// 대입 연산자 호출

	CByteImage image4(image1);

	int nWidth = image1.GetWidth();		// 영상의 너비
	int nHeight = image1.GetHeight();	// 영상의 높이
	int nChannel = image1.GetChannel();	// 영상의 채널 수

	double incX = 256.0 / nWidth;		// 비트 256 개를 가로(colum), 640로 쪼갠 값
	double incY = 256.0 / nHeight;

	cout << "incX = " << incX << "  incY = " << incY << endl;
	cout << "nHeight = " << nHeight << "  nWidth = " << nWidth << endl;

	int r, c;
	for (r = 0; r < nHeight; r++) {		// 행 단위 이동
		for (c = 0; c < nWidth; c++) {	// 열 단위 이동
			image2.GetAt(c, r) = (BYTE)(c * incX);	// 가로 그라데이션
			image3.GetAt(c, r) = (BYTE)(r * incY);	// 세로 그라데이션
			image4.GetAt(c, r) = (BYTE)(((c * incX) + (r * incY)) / 2);	// 대각선 그라데이션
			if (c < 10 || c > 630) {
				cout << (int)(((c * incX) + (r * incY)) / 2 )<< " ";
				//cout << (int)(r * incY) << " ";
			}
		}
		cout << endl;
	}

	image1.SaveImage("Black.bmp");
	image2.SaveImage("GradationX.bmp");
	image3.SaveImage("GradationY.bmp");
	image4.SaveImage("GradationDiagonal.bmp");

	return 0;
}

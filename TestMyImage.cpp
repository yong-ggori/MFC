#include <iostream>
#include "ImageSrc/MyImage.h"

using namespace std;

int main() {
	CByteImage image1(640, 480);	// ������ ȣ��, ����Ʈ �Ű�����(channels = 1 ) : 1 ä�� ��� (0 ~ 255 bit�� ���) -> ���
	image1.SetConstValue(0);		// 0���� �ʱ�ȭ

	CByteImage image2(image1);		// ���� ������ ȣ��
	CByteImage image3;				// �⺻ ������ ȣ��
	image3 = image1;				// ���� ������ ȣ��

	CByteImage image4(image1);

	int nWidth = image1.GetWidth();		// ������ �ʺ�
	int nHeight = image1.GetHeight();	// ������ ����
	int nChannel = image1.GetChannel();	// ������ ä�� ��

	double incX = 256.0 / nWidth;		// ��Ʈ 256 ���� ����(colum), 640�� �ɰ� ��
	double incY = 256.0 / nHeight;

	cout << "incX = " << incX << "  incY = " << incY << endl;
	cout << "nHeight = " << nHeight << "  nWidth = " << nWidth << endl;

	int r, c;
	for (r = 0; r < nHeight; r++) {		// �� ���� �̵�
		for (c = 0; c < nWidth; c++) {	// �� ���� �̵�
			image2.GetAt(c, r) = (BYTE)(c * incX);	// ���� �׶��̼�
			image3.GetAt(c, r) = (BYTE)(r * incY);	// ���� �׶��̼�
			image4.GetAt(c, r) = (BYTE)(((c * incX) + (r * incY)) / 2);	// �밢�� �׶��̼�
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

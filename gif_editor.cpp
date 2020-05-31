#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>

#include <fstream>
#include <iostream>
#include <windows.h>
#include <strsafe.h>

using namespace std;
using namespace cv;

struct rect_data{
    int x;
    int y;
    int w;
    int h;
    float weight;
};

static void printLimits(){
    cerr << "Limits of the current interface:" << endl;
    cerr << " - Only handles cascade classifier models, trained with the opencv_traincascade tool, containing stumps as decision trees [default settings]." << endl;
    cerr << " - The image provided needs to be a sample window with the original model dimensions, passed to the --image parameter." << endl;
    cerr << " - ONLY handles HAAR and LBP features." << endl;
}

BOOL CALLBACK EnumFamCallBack(LPLOGFONT lplf, LPNEWTEXTMETRIC lpntm, DWORD FontType, LPVOID aFontCount)
{
	int far * aiFontCount = (int far *) aFontCount;

	// Record the number of raster, TrueType, and vector  
	// fonts in the font-count array.  
	
	if (FontType & RASTER_FONTTYPE) {
		aiFontCount[0]++;
		std::cout << "1." << lplf->lfFaceName << std::endl;
	}
	else if (FontType & TRUETYPE_FONTTYPE) {
		aiFontCount[2]++;
		std::cout << "2." << lplf->lfFaceName << std::endl;
	}
	else {
		aiFontCount[1]++;
		std::cout << "3." << lplf->lfFaceName << std::endl;
	}
	if (aiFontCount[0] || aiFontCount[1] || aiFontCount[2])
		return TRUE;
	else
		return FALSE;

	UNREFERENCED_PARAMETER(lplf);
	UNREFERENCED_PARAMETER(lpntm);
}

int GetAllFont(int fontIdx, char *faceName, int faceNameLen)
{
	UINT uAlignPrev;
	char szCount[8];
	HRESULT hr;
	size_t  pcch;
	int aFontCount[] = { 0, 0, 0 };
	memset(faceName, 0, faceNameLen * sizeof(char));
	HDC hdc = CreateCompatibleDC(0);
	EnumFontFamilies(hdc, (LPCTSTR)NULL, (FONTENUMPROC)EnumFamCallBack, (LPARAM)aFontCount);

	uAlignPrev = SetTextAlign(hdc, TA_UPDATECP);

	MoveToEx(hdc, 10, 50, (LPPOINT)NULL);
	TextOut(hdc, 0, 0, "Number of raster fonts: ", 24);
	itoa(aFontCount[0], szCount, 10);

	hr = StringCchLength(szCount, 9, &pcch);
	if (FAILED(hr))
	{
		// TODO: write error handler 
	}
	TextOut(hdc, 0, 0, szCount, pcch);

	MoveToEx(hdc, 10, 75, (LPPOINT)NULL);
	TextOut(hdc, 0, 0, "Number of vector fonts: ", 24);
	itoa(aFontCount[1], szCount, 10);
	hr = StringCchLength(szCount, 9, &pcch);
	if (FAILED(hr))
	{
		// TODO: write error handler 
	}
	TextOut(hdc, 0, 0, szCount, pcch);

	MoveToEx(hdc, 10, 100, (LPPOINT)NULL);
	TextOut(hdc, 0, 0, "Number of TrueType fonts: ", 26);
	itoa(aFontCount[2], szCount, 10);
	hr = StringCchLength(szCount, 9, &pcch);
	if (FAILED(hr))
	{
		// TODO: write error handler 
	}
	TextOut(hdc, 0, 0, szCount, pcch);

	SetTextAlign(hdc, uAlignPrev);
	return 0;
}

void Wchar_tToString(std::string& szDst, wchar_t *wchar)
{
	// psText为char*的临时数组，作为赋值给std::string的中间变量
	char *psText; 
	wchar_t * wText = wchar;
	// WideCharToMultiByte的运用
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, NULL, 0, NULL, FALSE);
	psText = new char[dwNum];
	// WideCharToMultiByte的再次运用
	WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, psText, dwNum, NULL, FALSE);
	// std::string赋值
	szDst = psText; 
	delete[]psText; 
}

void TextToVcString(std::wstring text, std::vector<std::string>& content)
{
	wchar_t arrWord[2] = { 0 };
	for (int i = 0; i < text.length(); i++)
	{
		string word;
		arrWord[0] = text.at(i);
		arrWord[1] = '\0';
		Wchar_tToString(word, arrWord);
		content.push_back(word);
	}
}

void GetStringSize(HDC hDC, const char* str, int* w, int* h)
{
	SIZE size;
	GetTextExtentPoint32A(hDC, str, strlen(str), &size);
	if (w != 0) *w = size.cx;
	if (h != 0) *h = size.cy;
}

void putTextZH(Mat &dst, const char* str, Point org, Scalar color, int fontSize, const char* fn, bool italic, bool underline)
{
	CV_Assert(dst.data != 0 && (dst.channels() == 1 || dst.channels() == 3));

	int x, y, r, b;
	if (org.x > dst.cols || org.y > dst.rows) return;
	x = org.x < 0 ? -org.x : 0;
	y = org.y < 0 ? -org.y : 0;

	LOGFONTA lf;
	lf.lfHeight = -fontSize;
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = 5;
	lf.lfItalic = italic;   //斜体
	lf.lfUnderline = underline; //下划线
	lf.lfStrikeOut = 0;
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfOutPrecision = 0;
	lf.lfClipPrecision = 0;
	lf.lfQuality = PROOF_QUALITY;
	lf.lfPitchAndFamily = 0;
	int fontIdx = 1;
	char faceName[32];
	GetAllFont(fontIdx, faceName, sizeof(faceName) / sizeof(faceName[0]));
	// strcpy_s(lf.lfFaceName, faceName);
	strcpy_s(lf.lfFaceName, fn);
	HFONT hf = CreateFontIndirectA(&lf);
	HDC hDC = CreateCompatibleDC(0);
	HFONT hOldFont = (HFONT)SelectObject(hDC, hf);

	int strBaseW = 0, strBaseH = 0;
	int singleRow = 0;
	char buf[1 << 12];
	strcpy_s(buf, str);
	char *bufT[1 << 12];  // 这个用于分隔字符串后剩余的字符，可能会超出。
						  //处理多行
	{
		int nnh = 0;
		int cw, ch;

		const char* ln = strtok_s(buf, "\n", bufT);
		while (ln != 0)
		{
			GetStringSize(hDC, ln, &cw, &ch);
			strBaseW = max(strBaseW, cw);
			strBaseH = max(strBaseH, ch);

			ln = strtok_s(0, "\n", bufT);
			nnh++;
		}
		singleRow = strBaseH;
		strBaseH *= nnh;
	}

	if (org.x + strBaseW < 0 || org.y + strBaseH < 0)
	{
		SelectObject(hDC, hOldFont);
		DeleteObject(hf);
		DeleteObject(hDC);
		return;
	}

	r = org.x + strBaseW > dst.cols ? dst.cols - org.x - 1 : strBaseW - 1;
	b = org.y + strBaseH > dst.rows ? dst.rows - org.y - 1 : strBaseH - 1;
	org.x = org.x < 0 ? 0 : org.x;
	org.y = org.y < 0 ? 0 : org.y;

	BITMAPINFO bmp = { 0 };
	BITMAPINFOHEADER& bih = bmp.bmiHeader;
	int strDrawLineStep = strBaseW * 3 % 4 == 0 ? strBaseW * 3 : (strBaseW * 3 + 4 - ((strBaseW * 3) % 4));

	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biWidth = strBaseW;
	bih.biHeight = strBaseH;
	bih.biPlanes = 1;
	bih.biBitCount = 24;
	bih.biCompression = BI_RGB;
	bih.biSizeImage = strBaseH * strDrawLineStep;
	bih.biClrUsed = 0;
	bih.biClrImportant = 0;

	void* pDibData = 0;
	HBITMAP hBmp = CreateDIBSection(hDC, &bmp, DIB_RGB_COLORS, &pDibData, 0, 0);

	CV_Assert(pDibData != 0);
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hDC, hBmp);

	//color.val[2], color.val[1], color.val[0]
	SetTextColor(hDC, RGB(255, 255, 255));
	SetBkColor(hDC, 0);
	//SetStretchBltMode(hDC, COLORONCOLOR);

	strcpy_s(buf, str);
	const char* ln = strtok_s(buf, "\n", bufT);
	int outTextY = 0;
	while (ln != 0)
	{
		TextOutA(hDC, 0, outTextY, ln, strlen(ln));
		outTextY += singleRow;
		ln = strtok_s(0, "\n", bufT);
	}
	uchar* dstData = (uchar*)dst.data;
	int dstStep = dst.step / sizeof(dstData[0]);
	unsigned char* pImg = (unsigned char*)dst.data + org.x * dst.channels() + org.y * dstStep;
	unsigned char* pStr = (unsigned char*)pDibData + x * 3;
	for (int tty = y; tty <= b; ++tty)
	{
		unsigned char* subImg = pImg + (tty - y) * dstStep;
		unsigned char* subStr = pStr + (strBaseH - tty - 1) * strDrawLineStep;
		for (int ttx = x; ttx <= r; ++ttx)
		{
			for (int n = 0; n < dst.channels(); ++n) {
				double vtxt = subStr[n] / 255.0;
				int cvv = vtxt * color.val[n] + (1 - vtxt) * subImg[n];
				subImg[n] = cvv > 255 ? 255 : (cvv < 0 ? 0 : cvv);
			}

			subStr += 3;
			subImg += dst.channels();
		}
	}

	SelectObject(hDC, hOldBmp);
	SelectObject(hDC, hOldFont);
	DeleteObject(hf);
	DeleteObject(hBmp);
	DeleteDC(hDC);
}

#include <map>
#include <vector>

int frame_main(VideoWriter& writer, std::map<int, std::vector<std::string> >& text)
{
	int i, j;
	Mat window;
	string imgPath = "d:\\images\\";
	std::map<int, std::vector<std::string> >::iterator mapIter;
	std::vector<std::string>::iterator vcIter;
	window = imread("1.jpg", true);
	resize(window, window, Size(1024, 768), 0, 0, INTER_LINEAR_EXACT);
	for (i = 0, mapIter = text.begin(); mapIter != text.end(); mapIter++, ++i)
	{
		for (j = 0, vcIter = mapIter->second.begin(); vcIter != mapIter->second.end(); ++vcIter, ++j)
		{
			if (vcIter->length() > 0) {
				// Point pos((j + 1) * 30, (i + 1) * 30); // 横写
				// Point pos((i + 1) * 30, (j + 1) * 30); // 竖写
				// Point pos((i + 1) * 30, (mapIter->second.size() - j) * 30); // 逆古书
				Point pos((text.size() - i) * (32) + 800, (j + 1) * 32); // 古书
				// putText(window, vcIter->c_str(), pos, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255));
				putTextZH(window, vcIter->c_str(), pos, Scalar(255, 255, 255), 28, "隶书", false, true);
				putTextZH(window, "|", Point(955, 588), Scalar(255, 255, 255), 18, "宋体", false, false);
				putTextZH(window, "|", Point(955, 608), Scalar(255, 255, 255), 18, "宋体", false, false);
				putTextZH(window, "麦", Point(950, 628), Scalar(255, 255, 255), 18, "宋体", true, true);
				putTextZH(window, "思", Point(950, 648), Scalar(255, 255, 255), 18, "宋体", true, true);
				// putTextZH(window, "【深】", Point(940, 668), Scalar(255, 255, 255), 18, "宋体", true, true);
				// putTextZH(window, "【圳】", Point(940, 688), Scalar(255, 255, 255), 18, "宋体", true, true);
				imwrite(imgPath + std::to_string(i) + "_" + std::to_string(j) + ".jpg", window);
				writer << window;
			}
		}
	}

	return 0;
}

int fmt_text(std::vector<std::string>& content, int row, int col, std::map<int, std::vector<std::string> >& text)
{
	int i, j;
	std::vector<std::string>::iterator ctxIter;
	std::vector<std::string> line;
	for (i = 0, ctxIter = content.begin(); i < row; i++)
	{
		line.clear();
		for (j = 0; j < col; j++)
		{
			if (ctxIter == content.end()) {
				text[i] = line;
				return 0;
			} else {
				line.push_back(*ctxIter);
				ctxIter++;
			}
		}
		text[i] = line;
	}
}

int gen_content(std::vector<std::string>& content)
{
	std::wstring text = L"今天是公元二零二零年四月四日，近三个月的禁闭终于结束！清明了，给病毒烧点纸............";
	TextToVcString(text, content);
	return 0;
}

//    V_FOURCC('P','I','M','1')     = MPEG-1 codec
//    CV_FOURCC('M','J','P','G')    = motion-jpeg codec (does not work well)
//    CV_FOURCC('M', 'P', '4', '2') = MPEG-4.2 codec
//    CV_FOURCC('D', 'I', 'V', '3') = MPEG-4.3 codec
//    CV_FOURCC('D', 'I', 'V', 'X') = MPEG-4 codec
//    CV_FOURCC('U', '2', '6', '3') = H263 codec
//    CV_FOURCC('I', '2', '6', '3') = H263I codec
//    CV_FOURCC('F', 'L', 'V', '1') = FLV1 codec							    
int video_main()
{
	int resize_factor = 10;
	VideoWriter writer;
	std::string input_pic = "1.jpg";
	stringstream output_video;
	output_video << "model_visualization.avi";
	Mat reference_image = imread(input_pic, true);
	Size s = Size(1024, 768);
	bool ret = writer.open(output_video.str(), VideoWriter::fourcc('M', 'J', 'P', 'G'), 5, s, true);
	if (!ret) {
		return -1;
	}
	std::vector<std::string> content;
	std::map<int, std::vector<std::string> > text;
	gen_content(content);
	fmt_text(content, 18, 18, text);
	frame_main(writer, text);
	writer.release();
	return 0;
}

class ScreenCapture {
	/*  */
	HBITMAP hBitmap;
	HDC hDDC;
	HDC hCDC;
	int nWidth, nHeight;
	int iFwrite;
public:
	ScreenCapture()
	{
		hBitmap = NULL;
		hDDC = NULL;
		hCDC = NULL;
		nWidth = 1024;
		nHeight = 768;
		iFwrite = 200;
	}

	Mat GdiScreenCapture() {
		hBitmap = CreateCompatibleBitmap(hDDC, nWidth, nHeight); // 得到位图    
		SelectObject(hCDC, hBitmap); // 好像总得这么写。             
		BitBlt(hCDC, 0, 0, nWidth, nHeight, hDDC, 0, 0, SRCCOPY);
		Mat dst;
		dst.create(cv::Size(nWidth, nHeight), CV_8UC4);
		GetBitmapBits(hBitmap, nWidth*nHeight * 4, dst.data);
		cvtColor(dst, dst, COLOR_BGRA2BGR);
		return dst;
	}

	void SaveVideo(int iFwrite)
	{
		int iFrame = 1;
		nWidth = GetSystemMetrics(SM_CXSCREEN);//得到屏幕的分辨率的x    
		nHeight = GetSystemMetrics(SM_CYSCREEN);//得到屏幕分辨率的y    
		hDDC = GetDC(GetDesktopWindow());//得到屏幕的dc    
		hCDC = CreateCompatibleDC(hDDC);//    
		Mat img = GdiScreenCapture();
		VideoWriter vw;
		// vw.open("screen.avi", VideoWriter::fourcc('M', 'P', '4', '2'), 25, cv::Size(img.cols, img.rows));
		vw.open("screen.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 5, Size(nWidth, nHeight), true);
		while (iFwrite > iFrame++)
		{
			img = GdiScreenCapture();
			waitKey(25);
			if (!img.data)
				return;
			vw << img;
			// imshow("view", img);
		}
		vw.release();
	}
};


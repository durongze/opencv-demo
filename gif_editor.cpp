#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>

#include <fstream>
#include <iostream>
#include <windows.h>

#include <map>
#include <vector>

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
			// Point pos((j + 1) * 30, (i + 1) * 30); // 横写
			// Point pos((i + 1) * 30, (j + 1) * 30); // 竖写
			// Point pos((i + 1) * 30, (mapIter->second.size() - j) * 30); // 逆古书
			Point pos((text.size() - i) * (32) + 800, (j + 1) * 32); // 古书
			// putText(window, vcIter->c_str(), pos, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255));
			putTextZH(window, vcIter->c_str(), pos, Scalar(0, 255, 0), 28, "宋体", true, true);
			imwrite(imgPath + std::to_string(i) + "_" + std::to_string(j) + ".jpg", window);
			writer << window;
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
	content.push_back("昨");
	content.push_back("天");
	content.push_back("是");
	content.push_back("20");
	content.push_back("年");
	content.push_back("04");
	content.push_back("月");
	content.push_back("01");
	content.push_back("日");
	content.push_back("，");
	content.push_back("天");
	content.push_back("快");
	content.push_back("亮");
	content.push_back("时");
	content.push_back("，");
	content.push_back("我");
	content.push_back("做");
	content.push_back("了");
	content.push_back("个");
	content.push_back("梦");
	content.push_back("，");
	content.push_back("梦");
	content.push_back("到");
	content.push_back("中");
	content.push_back("学");
	content.push_back("里");
	content.push_back("的");
	content.push_back("场");
	content.push_back("景");
	content.push_back("，");
	content.push_back("醒");
	content.push_back("来");
	content.push_back("时");
	content.push_back("有");
	content.push_back("些");
	content.push_back("心");
	content.push_back("痛");
	content.push_back("，");
	content.push_back("任");
	content.push_back("何");
	content.push_back("努");
	content.push_back("力");
	content.push_back("都");
	content.push_back("要");
	content.push_back("在");
	content.push_back("正");
	content.push_back("确");
	content.push_back("的");
	content.push_back("时");
	content.push_back("间");
	content.push_back("，");
	content.push_back("如");
	content.push_back("今");
	content.push_back("时");
	content.push_back("间");
	content.push_back("错");
	content.push_back("了");
	content.push_back("，");
	content.push_back("梦");
	content.push_back("想");
	content.push_back("就");
	content.push_back("这");
	content.push_back("样");
	content.push_back("永");
	content.push_back("远");
	content.push_back("成");
	content.push_back("了");
	content.push_back("梦");
	content.push_back("想");
	content.push_back("！");
	content.push_back("！");
	content.push_back("！");
	content.push_back("！");
	content.push_back("！");
	content.push_back("！");
	content.push_back("！");
	content.push_back("！");
	content.push_back("！");
	content.push_back("！");
	content.push_back("！");
	return 0;
}

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

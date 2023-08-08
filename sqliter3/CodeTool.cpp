#include <atlconv.h>
#include <stdio.h>
#include <time.h>
#include <ShellAPI.h>
#include "CodeTool.h"
#include <strsafe.h>
#include <WinIoCtl.h>
#include <codecvt>
#include <io.h>
#include <ShObjIdl.h>
#include <ShlGuid.h>

#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

#define FILE_DEVICE_UNKNOWN			0x00000022
#define IOCTL_UNKNOWN_BASE			FILE_DEVICE_UNKNOWN
#define IOCTL_PROTECT_PROCESS		CTL_CODE(IOCTL_UNKNOWN_BASE, 0x0201, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

static string _base64_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char _base64_pad = '=';

inline MD5::uint4 MD5::F(uint4 x, uint4 y, uint4 z) {
	return x&y | ~x&z;
}

inline MD5::uint4 MD5::G(uint4 x, uint4 y, uint4 z) {
	return x&z | y&~z;
}

inline MD5::uint4 MD5::H(uint4 x, uint4 y, uint4 z) {
	return x^y^z;
}

inline MD5::uint4 MD5::I(uint4 x, uint4 y, uint4 z) {
	return y ^ (x | ~z);
}

inline MD5::uint4 MD5::rotate_left(uint4 x, int n) {
	return (x << n) | (x >> (32 - n));
}

inline void MD5::FF(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac) {
	a = rotate_left(a + F(b, c, d) + x + ac, s) + b;
}

inline void MD5::GG(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac) {
	a = rotate_left(a + G(b, c, d) + x + ac, s) + b;
}

inline void MD5::HH(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac) {
	a = rotate_left(a + H(b, c, d) + x + ac, s) + b;
}

inline void MD5::II(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac) {
	a = rotate_left(a + I(b, c, d) + x + ac, s) + b;
}

MD5::MD5()
{
	init();
}

MD5::MD5(const string &text)
{
	init();
	update(text.c_str(), text.length());
	finalize();
}

void MD5::init()
{
	finalized = false;

	count[0] = 0;
	count[1] = 0;

	// load magic initialization constants.  
	state[0] = 0x67452301;
	state[1] = 0xefcdab89;
	state[2] = 0x98badcfe;
	state[3] = 0x10325476;
}

void MD5::decode(uint4 output[], const uint1 input[], size_type len)
{
	for (unsigned int i = 0, j = 0; j < len; i++, j += 4)
		output[i] = ((uint4)input[j]) | (((uint4)input[j + 1]) << 8) |
		(((uint4)input[j + 2]) << 16) | (((uint4)input[j + 3]) << 24);
}

void MD5::encode(uint1 output[], const uint4 input[], size_type len)
{
	for (size_type i = 0, j = 0; j < len; i++, j += 4) {
		output[j] = input[i] & 0xff;
		output[j + 1] = (input[i] >> 8) & 0xff;
		output[j + 2] = (input[i] >> 16) & 0xff;
		output[j + 3] = (input[i] >> 24) & 0xff;
	}
}

void MD5::transform(const uint1 block[blocksize])
{
	uint4 a = state[0], b = state[1], c = state[2], d = state[3], x[16];
	decode(x, block, blocksize);

	/* Round 1 */
	FF(a, b, c, d, x[0], S11, 0xd76aa478); /* 1 */
	FF(d, a, b, c, x[1], S12, 0xe8c7b756); /* 2 */
	FF(c, d, a, b, x[2], S13, 0x242070db); /* 3 */
	FF(b, c, d, a, x[3], S14, 0xc1bdceee); /* 4 */
	FF(a, b, c, d, x[4], S11, 0xf57c0faf); /* 5 */
	FF(d, a, b, c, x[5], S12, 0x4787c62a); /* 6 */
	FF(c, d, a, b, x[6], S13, 0xa8304613); /* 7 */
	FF(b, c, d, a, x[7], S14, 0xfd469501); /* 8 */
	FF(a, b, c, d, x[8], S11, 0x698098d8); /* 9 */
	FF(d, a, b, c, x[9], S12, 0x8b44f7af); /* 10 */
	FF(c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
	FF(b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
	FF(a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
	FF(d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
	FF(c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
	FF(b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

	/* Round 2 */
	GG(a, b, c, d, x[1], S21, 0xf61e2562); /* 17 */
	GG(d, a, b, c, x[6], S22, 0xc040b340); /* 18 */
	GG(c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
	GG(b, c, d, a, x[0], S24, 0xe9b6c7aa); /* 20 */
	GG(a, b, c, d, x[5], S21, 0xd62f105d); /* 21 */
	GG(d, a, b, c, x[10], S22, 0x2441453); /* 22 */
	GG(c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
	GG(b, c, d, a, x[4], S24, 0xe7d3fbc8); /* 24 */
	GG(a, b, c, d, x[9], S21, 0x21e1cde6); /* 25 */
	GG(d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
	GG(c, d, a, b, x[3], S23, 0xf4d50d87); /* 27 */
	GG(b, c, d, a, x[8], S24, 0x455a14ed); /* 28 */
	GG(a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
	GG(d, a, b, c, x[2], S22, 0xfcefa3f8); /* 30 */
	GG(c, d, a, b, x[7], S23, 0x676f02d9); /* 31 */
	GG(b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

	/* Round 3 */
	HH(a, b, c, d, x[5], S31, 0xfffa3942); /* 33 */
	HH(d, a, b, c, x[8], S32, 0x8771f681); /* 34 */
	HH(c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
	HH(b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
	HH(a, b, c, d, x[1], S31, 0xa4beea44); /* 37 */
	HH(d, a, b, c, x[4], S32, 0x4bdecfa9); /* 38 */
	HH(c, d, a, b, x[7], S33, 0xf6bb4b60); /* 39 */
	HH(b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
	HH(a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
	HH(d, a, b, c, x[0], S32, 0xeaa127fa); /* 42 */
	HH(c, d, a, b, x[3], S33, 0xd4ef3085); /* 43 */
	HH(b, c, d, a, x[6], S34, 0x4881d05); /* 44 */
	HH(a, b, c, d, x[9], S31, 0xd9d4d039); /* 45 */
	HH(d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
	HH(c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
	HH(b, c, d, a, x[2], S34, 0xc4ac5665); /* 48 */

	/* Round 4 */
	II(a, b, c, d, x[0], S41, 0xf4292244); /* 49 */
	II(d, a, b, c, x[7], S42, 0x432aff97); /* 50 */
	II(c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
	II(b, c, d, a, x[5], S44, 0xfc93a039); /* 52 */
	II(a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
	II(d, a, b, c, x[3], S42, 0x8f0ccc92); /* 54 */
	II(c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
	II(b, c, d, a, x[1], S44, 0x85845dd1); /* 56 */
	II(a, b, c, d, x[8], S41, 0x6fa87e4f); /* 57 */
	II(d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
	II(c, d, a, b, x[6], S43, 0xa3014314); /* 59 */
	II(b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
	II(a, b, c, d, x[4], S41, 0xf7537e82); /* 61 */
	II(d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
	II(c, d, a, b, x[2], S43, 0x2ad7d2bb); /* 63 */
	II(b, c, d, a, x[9], S44, 0xeb86d391); /* 64 */

	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;

	// Zeroize sensitive information.  
	memset(x, 0, sizeof x);
}

void MD5::update(const unsigned char input[], size_type length)
{
	// compute number of bytes mod 64  
	size_type index = count[0] / 8 % blocksize;

	// Update number of bits  
	if ((count[0] += (length << 3)) < (length << 3))
		count[1]++;
	count[1] += (length >> 29);

	// number of bytes we need to fill in buffer  
	size_type firstpart = 64 - index;

	size_type i;

	// transform as many times as possible.  
	if (length >= firstpart)
	{
		// fill buffer first, transform  
		memcpy(&buffer[index], input, firstpart);
		transform(buffer);

		// transform chunks of blocksize (64 bytes)  
		for (i = firstpart; i + blocksize <= length; i += blocksize)
			transform(&input[i]);

		index = 0;
	}
	else
		i = 0;

	// buffer remaining input  
	memcpy(&buffer[index], &input[i], length - i);
}

void MD5::update(const char input[], size_type length)
{
	update((const unsigned char*)input, length);
}

MD5& MD5::finalize()
{
	static unsigned char padding[64] = {
		0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};

	if (!finalized) {
		// Save number of bits  
		unsigned char bits[8];
		encode(bits, count, 8);

		// pad out to 56 mod 64.  
		size_type index = count[0] / 8 % 64;
		size_type padLen = (index < 56) ? (56 - index) : (120 - index);
		update(padding, padLen);

		// Append length (before padding)  
		update(bits, 8);

		// Store state in digest  
		encode(digest, state, 16);

		// Zeroize sensitive information.  
		memset(buffer, 0, sizeof buffer);
		memset(count, 0, sizeof count);

		finalized = true;
	}

	return *this;
}

string MD5::hexdigest() const
{
	if (!finalized)
		return "";

	char buf[33];
	for (int i = 0; i<16; i++)
		sprintf(buf + i * 2, "%02x", digest[i]);
	buf[32] = 0;

	return string(buf);
}

ostream& operator<<(ostream& out, MD5 md5)
{
	return out << md5.hexdigest();
}

string CodeTool::EncodeBase64(const unsigned char * str, int bytes)
{
	int num = 0;
	string _encode_result;
	const unsigned char * current;
	current = str;
	while (bytes > 2) {
		_encode_result += _base64_table[current[0] >> 2];
		_encode_result += _base64_table[((current[0] & 0x03) << 4) + (current[1] >> 4)];
		_encode_result += _base64_table[((current[1] & 0x0f) << 2) + (current[2] >> 6)];
		_encode_result += _base64_table[current[2] & 0x3f];

		current += 3;
		bytes -= 3;
	}
	if (bytes > 0)
	{
		_encode_result += _base64_table[current[0] >> 2];
		if (bytes % 3 == 1) {
			_encode_result += _base64_table[(current[0] & 0x03) << 4];
			_encode_result += "==";
		}
		else if (bytes % 3 == 2) {
			_encode_result += _base64_table[((current[0] & 0x03) << 4) + (current[1] >> 4)];
			_encode_result += _base64_table[(current[1] & 0x0f) << 2];
			_encode_result += "=";
		}
	}
	return _encode_result;
}

string CodeTool::DecodeBase64(const char *str, int length)
{
	//解码表
	const char DecodeTable[] =
	{
		-2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -1, -2, -2, -1, -2, -2,
		-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
		-1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 62, -2, -2, -2, 63,
		52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -2, -2, -2, -2, -2, -2,
		-2, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
		15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -2, -2, -2, -2, -2,
		-2, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
		41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -2, -2, -2, -2, -2,
		-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
		-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
		-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
		-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
		-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
		-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
		-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
		-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2
	};
	int bin = 0, i = 0, pos = 0;
	string _decode_result;
	const char *current = str;
	char ch;
	while ((ch = *current++) != '\0' && length-- > 0)
	{
		if (ch == _base64_pad) { // 当前一个字符是“=”号
			/*
			先说明一个概念：在解码时，4个字符为一组进行一轮字符匹配。
			两个条件：
			1、如果某一轮匹配的第二个是“=”且第三个字符不是“=”，说明这个带解析字符串不合法，直接返回空
			2、如果当前“=”不是第二个字符，且后面的字符只包含空白符，则说明这个这个条件合法，可以继续。
			*/
			if (*current != '=' && (i % 4) == 1) {
				return NULL;
			}
			continue;
		}
		ch = DecodeTable[ch];
		//这个很重要，用来过滤所有不合法的字符
		if (ch < 0) { /* a space or some other separator character, we simply skip over */
			continue;
		}
		switch (i % 4)
		{
		case 0:
			bin = ch << 2;
			break;
		case 1:
			bin |= ch >> 4;
			_decode_result += bin;
			bin = (ch & 0x0f) << 4;
			break;
		case 2:
			bin |= ch >> 2;
			_decode_result += bin;
			bin = (ch & 0x03) << 6;
			break;
		case 3:
			bin |= ch;
			_decode_result += bin;
			break;
		}
		i++;
	}
	return _decode_result;
}

wstring CodeTool::Str2WStr(const string& str)
{
	USES_CONVERSION;
	return A2W(str.c_str());
}

string CodeTool::WStr2Str(const wstring& wstr)
{
	USES_CONVERSION;
	return W2A(wstr.c_str());
}

std::string CodeTool::UnicodeToUtf8(std::wstring& wstr)
{
	std::string ret;
	try {
		std::wstring_convert<std::codecvt_utf8<wchar_t>> wcv;
		ret = wcv.to_bytes(wstr);
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return ret;
}

std::wstring CodeTool::Utf8ToUnicode(std::string& str)
{
	std::wstring ret;
	try {
		std::wstring_convert<std::codecvt_utf8<wchar_t>> wcv;
		ret = wcv.from_bytes(str);
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return ret;
}

string CodeTool::md5(const std::string str)
{
	MD5 md5 = MD5(str);
	return md5.hexdigest();
}

// 检测URL是否包含？选择拼接
const std::string CodeTool::ActivityCheckUrl(const std::string& strUrl)
{
	if (strUrl.empty())
		return "";

	std::string strRetUrl = strUrl;
	const int ilen = strRetUrl.find_last_of("?");
	// 下发URL包含了参数
	if (ilen > 0)
		strRetUrl.append("&");
	else
		strRetUrl.append("?");
	return strRetUrl;
}

unsigned char ToHex(unsigned char x)
{
	return  x > 9 ? x + 55 : x + 48;
}

unsigned char FromHex(unsigned char x)
{
	unsigned char y;
	if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
	else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
	else if (x >= '0' && x <= '9') y = x - '0';
	return y;
}
std::string CodeTool::UrlEncode(const std::string& str)
{
	std::string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
		if (isalnum((unsigned char)str[i]) ||
			(str[i] == '-') ||
			(str[i] == '_') ||
			(str[i] == '.') ||
			(str[i] == '~'))
			strTemp += str[i];
		else if (str[i] == ' ')
			strTemp += "+";
		else
		{
			strTemp += '%';
			strTemp += ToHex((unsigned char)str[i] >> 4);
			strTemp += ToHex((unsigned char)str[i] % 16);
		}
	}
	return strTemp;
}

std::string CodeTool::UrlDecode(const std::string& str)
{
	std::string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
		if (str[i] == '+') strTemp += ' ';
		else if (str[i] == '%')
		{
			//assert(i + 2 < length);
			unsigned char high = FromHex((unsigned char)str[++i]);
			unsigned char low = FromHex((unsigned char)str[++i]);
			strTemp += high * 16 + low;
		}
		else strTemp += str[i];
	}
	return strTemp;
}

void CodeTool::DeletetFile(const std::string& stFilePath) {
	if (stFilePath.empty())
		return;
	DeleteFileA(stFilePath.c_str());
}

const bool CodeTool::ProcessRunStart(const std::wstring& strProcessPath, const std::wstring& strParm, std::wstring& strError)
{
	ShellExecuteW(NULL, NULL, strProcessPath.c_str(), strParm.c_str(), NULL, SW_HIDE);
	return true;
}

const bool CodeTool::CGetCurrentDirectory(std::string& strDirpath)
{
	// 获取当前目录路径
	char szModule[1024] = { 0, };
	GetModuleFileNameA(NULL, szModule, sizeof(szModule) / sizeof(char));
	strDirpath = szModule;
	if (0 >= strDirpath.size())
	{
		return 0;
	}
	int offset = strDirpath.rfind("\\");
	if (0 >= offset)
	{
		return 0;
	}
	strDirpath = strDirpath.substr(0, offset + 1);
	return true;
}

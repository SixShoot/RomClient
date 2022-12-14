#include <windows.h>
#include "ui.h"
#include "uifontstring.h"
int CUiFontString::s_charColor = 0;					// 文字顏色
int CUiFontString::s_linkIndex = -1;				// 索引值
const WCHAR* CUiFontString::s_textBegin = NULL;				// 文字開始位置
inline bool IsNumeric(WCHAR ch, bool isSymbol)
{
	if (ch >= L'0' && ch <= '9')
		return true;
	if (isSymbol)
	{
		switch (ch)
		{
		case L'+':
		case L'-':
		case L'*':
		case L'/':
		case L',':
		case L'.':
			return true;
		}
	}
	return false;
}
inline bool IsNumeric(const WCHAR* text, int length, bool isSymbol)
{
	return IsNumeric(text[0], isSymbol);
}
inline bool IsCharAlphabet(WCHAR ch)
{
	if (ch == L'\n')
		return false;
	if (ch >= 0x4e00 && ch <= 0x9fa5)
		return false;
	if (ch >= 0x0800 && ch <= 0x4e00)
		return false;
	if (ch > 0x9fa5)
		return false;
	if (ch == 0x0401 || ch == 0x0451 || (ch >= 0x0410 && ch <= 0x044f))
		return true;
	if (IsCharAlphaW(ch))
		return true;
	if (ch == L' ')
		return false;
	return true;
}
inline bool IsCharAlphabet(const WCHAR* text, int length)
{
	return IsCharAlphabet(text[0]);
}
CUiFontString::CUiFontString(const char* name, CUiLayout* pParent, CUi* pUi)
	: CUiLayout(name, pParent, pUi)
{
	m_uiType = eUiType_FontString;
	m_text = NULL;
	m_bytes = 255;
	m_pFontType = NULL;
	m_pDotChar = NULL;
	m_pShadow = NULL;
	m_spacing = 0.0f;
	m_justifyVType = eJustifyV_Middle;
	m_justifyHType = eJustifyH_Center;
	m_internalChangedSize = false;
	m_hideLastDot = false;
	m_isRTL = false;
	m_charData.clear();
	m_strData.clear();
	m_hyperlinkData.clear();
	m_caretPosition.clear();
}
CUiFontString::~CUiFontString()
{
	UI_SAFE_DELETE_ARRAY(m_text);
	ClearFontShadow();
	m_charData.clear();
	m_strData.clear();
	m_hyperlinkData.clear();
	m_caretPosition.clear();
}
bool CUiFontString::HyperlinkText(const WCHAR* inStr, int inSize, wstring& outStr)
{
	int i = 0;
	if (inSize < 0)
		inSize = (int)wcslen(inStr);
	while (inStr[i] && i < inSize)
	{
		bool valid = true;
		if (inStr[i] == L'|')
		{
			const wchar_t* next = NULL;
			switch (inStr[i + 1])
			{
			case L'H':
				if (next = wcsstr(&inStr[i + 2], L"|h"))
				{
					const wchar_t* begin = next + 2;
					const wchar_t* end = wcsstr(begin, L"|h");
					if (end != NULL) 
					{
						valid = false;
						HyperlinkText(begin, int(end - begin), outStr);
						i += int(end - (inStr + (INT64)i) + 2);
					}
				}
				break;
			case L'c':
				if (wcslen(&inStr[i + 2]) > 7) {
					valid = false;
					i += 10;
				}
				break;
			case L'o':
			case L'O':
				if (wcslen(&inStr[i + 2]) > 7) {
					valid = false;
					i += 10;
				}
				break;
			case L'r':
				valid = false;
				i += 2;
				break;
			}
		}
		if (valid) {
			outStr.push_back(inStr[i++]);
		}
	}
	return 0;
}
// ----------------------------------------------------------------------------------
bool CUiFontString::HyperlinkTextA(const char* inStr, int inSize, string& outStr)
{
	int i = 0;
	if (inSize < 0)
		inSize = (int)strlen(inStr);
	while (inStr[i] && i < inSize)
	{
		bool valid = true;
		if (inStr[i] == '|')
		{
			const char* next = NULL;
			switch (inStr[i + 1])
			{
				// 連結
			case 'H':
				if (next = strstr(&inStr[i + 2], "|h"))
				{
					// 前段資料
					const char* begin = next + 2;
					// 後段資料
					const char* end = strstr(begin, "|h");
					if (end != NULL) {
						valid = false;
						HyperlinkTextA(begin, int(end - begin), outStr);
						i += int(end - (inStr + (INT64)i) + 2);
					}
				}
				break;
				// 顏色開始
			case 'c':
				if (strlen(&inStr[i + 2]) > 7) {
					valid = false;
					i += 10;			// 2控字元 + 8個色碼
				}
				break;
			case 'o':
			case 'O':
				if (strlen(&inStr[i + 2]) > 7) {
					valid = false;
					i += 10;			// 2控字元 + 8個色碼
				}
				break;
				// 顏色結束,回復預設值
			case 'r':
				valid = false;
				i += 2;
				break;
			}
		}
		// 有效的字元
		if (valid) {
			outStr.push_back(inStr[i++]);
		}
	}
	return 0;
}
// ----------------------------------------------------------------------------------
void CUiFontString::Render(float alpha)
{
	CUiPoint pos = GetPaintPos();
	Render(alpha, pos.m_x, pos.m_y);
}
// ----------------------------------------------------------------------------------
void CUiFontString::Render(float alpha, float x, float y)
{
	if (m_strData.empty() || !m_isVisible || m_pFontType == NULL)
		return;
	float sx, sy, charWidth, dotWidth;
	float scale = GetRealScale();
	float fontHeight = m_pFontType->GetHeight() * scale;
	float spacing = m_spacing * scale;
	CUiSize size = m_size * scale;
	int maxLine = min((int)m_strData.size(), max((int)(size.m_y / fontHeight + 0.1f), 1));
	sy = y;
	m_isDrawDot = false;
	// 跟據對齊方式,取得Y軸座標值
	switch (m_justifyVType)
	{
	case eJustifyV_Top:
		break;
	case eJustifyV_Middle:
		if (size.m_y != fontHeight * maxLine)
			sy += (size.m_y - (fontHeight + m_spacing) * maxLine) / 2.0f;
		break;
	case eJustifyV_Bottom:
		sy += size.m_y - (fontHeight + m_spacing) * maxLine;
		break;
	}
	// 開始繪出字元
	//CRuFontChar::PaintBegin();
	CRuFontString::PaintBegin();
	for (int i = 0; i < maxLine; i++)
	{
		int begin = m_strData[i].begin;
		int end = m_strData[i].end;
		float w = m_strData[i].width * scale;
		sx = x;
		/*
		// 第二行,檢查開始字元是否為空白
		if ( i > 0 )
		{
			while (begin < end)
			{
				CRuFontChar* pChar = m_charData[begin].pChar;
				if ( pChar->GetChar() == == L' ' || pChar->GetChar() == L'\t' )
					begin++;
				else
					break;
			}
		}
		*/
		// 若已經是顯示的最後一行並且還有資料未顯示
		if (i + 1 == maxLine && maxLine < (int)m_strData.size())
		{
			// 找到最後顯示字元
			StringInfo& next = m_strData[i + 1];
			for (int k = next.begin; k < next.end; k++)
			{
				charWidth = m_charData[k].fontString->GetXSize() * scale;
				if (w + charWidth > size.m_x)
					break;
				else
				{
					w += charWidth + spacing;
					end = k + 1;
				}
			}
			end = min(end, (int)(m_charData.size() - 1));
			if (!m_hideLastDot)
			{
				// 在字串最後繪出"..."
				m_isDrawDot = true;
				// 調整"..."加入後大小
				dotWidth = m_pDotChar->GetXSize() * scale;
				w += dotWidth * 3.0f;
				while (w > size.m_x && end > begin)
				{
					charWidth = m_charData[end - 1].fontString->GetXSize() * scale;
					w -= charWidth + spacing;
					end--;
				}
			}
		}
		// 跟據對齊方式,取得X軸座標值
		switch (m_justifyHType)
		{
		case eJustifyH_Left:
			break;
		case eJustifyH_Center:
			if (size.m_x != w)
				sx += (size.m_x - w) / 2;
			break;
		case eJustifyH_Right:
			sx += size.m_x - w;
			break;
		}
		// paint 
		PaintChar(sx, sy, begin, end, alpha, scale);
		if (m_isDrawDot)
		{
			DWORD color = m_color.GetColor();
			sx += w - dotWidth * 3.0f;
			for (int i = 0; i < 3; i++)
			{
				m_pDotChar->Paint(sx, sy, scale, color, m_pShadow);
				sx += dotWidth;
			}
		}
		sy += fontHeight + m_spacing;
	}
}
// ----------------------------------------------------------------------------------
void CUiFontString::Render(float alpha, float x, float y, int begin, int end)
{
	if (m_charData.empty())
		return;
	begin = min(max(0, begin), (int)m_charData.size());
	end = min(max(0, end), (int)m_charData.size());
	// 開始繪出字元
	//CRuFontChar::PaintBegin();
	CRuFontString::PaintBegin();
	// paint 
	PaintChar(x, y, min(begin, end), max(begin, end), alpha, GetRealScale());
}
// ----------------------------------------------------------------------------------
void CUiFontString::SetAnchorSize(CUiSize size)
{
	if (m_anchorSize != size)
	{
		m_anchorSize = size;
		if (m_anchorSize.m_x == 0.0f || m_anchorSize.m_y == 0.0f)
			ModifySize();
		else
			SetSize(m_anchorSize);
	}
}
// ----------------------------------------------------------------------------------
void CUiFontString::SetSize(CUiSize size, bool isReAnchor)
{
	if (m_size != size)
	{
		CUiLayout::SetSize(size);
		//if (!m_internalChangedSize)
		GetLineInfo();
	}
}
// ----------------------------------------------------------------------------------
void CUiFontString::SetFontType(CRuFontType* pFontType)
{
	m_pDotChar = NULL;
	m_pFontType = pFontType;
	SetText(m_text);
	if (m_pFontType)
		m_pDotChar = CRuFontString::GetFontString(m_pFontType, L".", 1);
}
// ----------------------------------------------------------------------------------
bool CUiFontString::CreateFontType(const char* font, int size, RuFontWeight weight, RuFontOutline outline)
{
	if (m_pUi)
	{
		string fontname = m_pUi->GetFontName(font);
		if (fontname.empty() == false)
		{
			CRuFontType* pFontType = CRuFontType::GetFontType(fontname.c_str(), size, weight, outline);
			if (pFontType)
			{
				SetFontType(pFontType);
				return true;
			}
		}
	}
	return false;
}
// ----------------------------------------------------------------------------------
void CUiFontString::SetBytes(int bytes)
{
	if (m_bytes != bytes)
	{
		WCHAR* tempStr = new WCHAR[bytes + 1];
		if (m_text)
		{
			wcsncpy(tempStr, m_text, bytes);
			tempStr[bytes] = 0;
			delete[] m_text;
		}
		m_text = tempStr;
		m_bytes = bytes;
	}
}
// ----------------------------------------------------------------------------------
void CUiFontString::SetColor(float r, float g, float b)
{
	if (m_color.m_r != r || m_color.m_g != g || m_color.m_b != b)
	{
		CUiLayout::SetColor(r, g, b);
		DWORD ch_color = m_color.GetColor();
		for (vector<CharInfo>::iterator iter = m_charData.begin(); iter != m_charData.end(); iter++)
		{
			(*iter).color = ch_color;
		}
	}
}
// ----------------------------------------------------------------------------------
void CUiFontString::SetAlpha(float a)
{
	if (m_color.m_a != a)
	{
		CUiLayout::SetAlpha(a);
		DWORD ch_color = m_color.GetColor();
		for (vector<CharInfo>::iterator iter = m_charData.begin(); iter != m_charData.end(); iter++)
		{
			(*iter).color = ch_color;
		}
	}
}
// ----------------------------------------------------------------------------------
HyperlinkData* CUiFontString::GetHyperlinkText(float x, float y)
{
	if (m_text == NULL || m_pFontType == NULL || x < 0.0f || y < 0.0f)
		return NULL;
	CUiSize realSize = GetRealSize();
	float scale = GetRealScale();
	float charWidth, spacing;
	float fontHeight = m_pFontType->GetHeight() * scale;
	spacing = m_spacing * scale;
	int pos = 0;
	int end = (int)m_charData.size();
	float sy = y;
	if (x < realSize.m_x)
	{
		switch (m_justifyHType)
		{
		case eJustifyH_Right:
			x = realSize.m_x - x;
			break;
		case eJustifyH_Center:
			x = (realSize.m_x - x) / 2;
			break;
		}
		// 取得開始位置
		vector<StringInfo>::iterator iter = m_strData.begin();
		for (; iter != m_strData.end() && sy > fontHeight; iter++)
		{
			pos = iter->end;
			sy -= fontHeight + spacing;
		}
		if (iter != m_strData.end())
		{
			end = iter->end;
		}
		for (; pos < end; pos++)
		{
			charWidth = m_charData[pos].fontString->GetXSize() * scale;
			if (charWidth > x)
			{
				int index = m_charData[pos].hyperlinkIndex;
				if (index < 0 || index >= (int)m_hyperlinkData.size())
					return NULL;
				return &(m_hyperlinkData[index]);
			}
			x -= charWidth + spacing;
		}
	}
	return NULL;
}
// ----------------------------------------------------------------------------------
int CUiFontString::GetStringLines(float height)
{
	int line = 0;
	if (m_text == NULL || m_pFontType == NULL)
		return line;
	if (height < 0.001f)
		return line;
	float scale = GetRealScale();
	float fontHeight = m_pFontType->GetHeight() * scale;
	float spacing = m_spacing * scale;
	// 取得開始位置
	for (vector<StringInfo>::iterator iter = m_strData.begin(); iter != m_strData.end(); iter++)
	{
		line++;
		if (height > fontHeight)
			return line;
		height -= fontHeight + spacing;
	}
	return line;
}
// ----------------------------------------------------------------------------------
int CUiFontString::TextToCaret(int position)
{
	int caret = 0;
	for (vector<int>::iterator iter = m_caretPosition.begin(); iter != m_caretPosition.end(); iter++, caret++)
	{
		if (position == *iter || position < *iter)
			return caret;
	}
	return (int)m_caretPosition.size();
}
// ----------------------------------------------------------------------------------
int CUiFontString::CaretToIndex(int caret)
{
	if (caret < 0 || m_caretPosition.empty())
		return 0;
	if (caret >= m_caretPosition.size())
	{
		int offset = (int)(caret - m_caretPosition.size() + 1);
		return m_caretPosition[m_caretPosition.size() - 1] + offset;
	}
	return m_caretPosition[caret];
}
// ----------------------------------------------------------------------------------
int CUiFontString::GetCharSize(int index, int* beginIndex)
{
	int i = 0;
	while (m_text[i] && i < m_bytes)
	{
		int size = 1;
		if (m_text[i] == L'|')
		{
			const wchar_t* next = NULL;
			switch (m_text[i + 1])
			{
				// 連結
			case L'H':
				if (next = wcsstr(&m_text[i + 2], L"|h"))
				{
					// 前段資料
					const wchar_t* begin = next + 2;
					// 後段資料
					const wchar_t* end = wcsstr(begin, L"|h");
					if (end != NULL) {
						size = int((end - (m_text + i)) + 2);
						if (i <= index && index < i + size)
						{
							if (beginIndex) *beginIndex = i;
							return size;
						}
					}
				}
				break;
				// 顏色開始
			case L'c':
				if (wcslen(&m_text[i + 2]) > 7) {
					size = 10;
					if (i <= index && index < i + size)
					{
						if (beginIndex) *beginIndex = i;
						return size;
					}
				}
				break;
			case L'o':
			case L'O':
				if (wcslen(&m_text[i + 2]) > 7) {
					size = 10;
					if (i <= index && index < i + size)
					{
						if (beginIndex) *beginIndex = i;
						return size;
					}
				}
				break;
				// 顏色結束,回復預設值
			case L'r':
				size = 2;
				if (i <= index && index < i + 2)
				{
					if (beginIndex) *beginIndex = i;
					return size;
				}
				break;
			}
		}
		if (i >= index)
		{
			if (beginIndex) *beginIndex = i;
			return size;
		}
		i += size;
	}
	if (beginIndex) *beginIndex = i;
	return 1;
}
// ----------------------------------------------------------------------------------
// textStr 字串內容
// isParse 字串是否需要解釋,特殊字串
void CUiFontString::SetText(const WCHAR* textStr, bool isParse)
{
	if (textStr)
	{
		if (m_text == NULL)
			m_text = new WCHAR[m_bytes + 1];
		else if (wcsncmp(textStr, m_text, m_bytes) == 0)
			return;
		wcsncpy(m_text, textStr, m_bytes);
		m_text[m_bytes] = 0;
		/*
		list<wstring> strList;
		CRuFontString::SplitString(textStr, wcslen(textStr), strList);
		wstring wText;
		for ( list<wstring>::iterator iter = strList.begin(); iter != strList.end(); iter++ )
			wText += *iter;
		// 重新取得顯示字元
		ResetCharData();
		s_textBegin = wText.c_str();
		SetCharData(isParse, s_textBegin, wText.length());
		s_textBegin = NULL;
		*/
		// 重新取得顯示字元
		ResetCharData();
		s_textBegin = m_text;
		SetCharData(isParse, m_text, (int)wcslen(m_text));
		s_textBegin = NULL;
		// 若顯示面積跟據字串大小而做變動
		ModifySize();
		// 取得每一行秀出文字
		GetLineInfo();
	}
}
// ----------------------------------------------------------------------------------
void CUiFontString::SetTextA(const char* text, bool isParse)
{
	//WCHAR* tmp = MultiByteToWChar(text);
	WCHAR* tmp = Utf8ToWChar(text);
	SetText(tmp, isParse);
	UI_SAFE_DELETE_ARRAY(tmp);
}
// ----------------------------------------------------------------------------------
void CUiFontString::SetFontShadow(RuFontShadow* shadow)
{
	if (shadow)
	{
		if (m_pShadow == NULL)
			m_pShadow = new RuFontShadow;
		*m_pShadow = *shadow;
	}
	else
		ClearFontShadow();
}
// ----------------------------------------------------------------------------------
void CUiFontString::ClearFontShadow()
{
	UI_SAFE_DELETE(m_pShadow);
}
// ----------------------------------------------------------------------------------
void CUiFontString::PaintChar(float x, float y, int begin, int end, float alpha, float scale)
{
	if (begin >= end)
		return;
	int size = end - begin;
	int separatorCount = 0;
	WORD ls[1024];
	BYTE RTLSign[1024];
	BOOL isHaveRTL = false;
	WORD* charType = new WORD[size];
	BOOL isFirstRTL = m_charData[0].fontString->GetFirstWordType() == C2_RIGHTTOLEFT;
	if (m_isRTL)
		isFirstRTL = true;
	// 查詢每一個要顯示的文字類型
	for (int i = 0; i < size; i++)
	{
		charType[i] = m_charData[i + begin].fontString->GetFirstWordType();
		ls[i] = i;
		RTLSign[i] = 0;
		switch (charType[i])
		{
		case C2_RIGHTTOLEFT:
			isHaveRTL = true;
			break;
		}
	}
	if (isHaveRTL || isFirstRTL)
	{
		int r = size - 1;
		int l = 0;
		int i = 0;
		bool isPreRTL = isFirstRTL;
		while (i < size)
		{
			int count = 0;
			while (i + count < size)
			{
				if (charType[i + count] == C2_LEFTTORIGHT)
				{
					if (isPreRTL)
					{
						isPreRTL = false;
						break;
					}
				}
				else if (charType[i + count] == C2_RIGHTTOLEFT)
				{
					if (!isPreRTL)
					{
						isPreRTL = true;
						break;
					}
				}
				else if (charType[i + count] == C2_EUROPESEPARATOR || charType[i + count] == C2_OTHERNEUTRAL)
				{
					wchar_t wch = m_charData[i + count].fontString->GetText()[0];
					switch (wch)
					{
					case L'[':
					case L']':
						separatorCount++;
						break;
					}
					// 目前與開頭都是RTL
					if (isPreRTL && isFirstRTL)
					{
						RTLSign[i + count] = 1;
					}
					// 開頭是RTL
					else if (isFirstRTL)
					{
						bool end = true;
						isPreRTL = true;
						// 檢查下一個字元
						for (int k = i + count + 1; k < size; k++)
						{
							if (charType[k] == C2_LEFTTORIGHT || charType[k] == C2_EUROPENUMBER || charType[k] == C2_ARABICNUMBER)
							{
								if (m_isRTL && (wch == L'[' || wch == L']') && (separatorCount & 1) == 0)
								{
								}
								else
								{
									isPreRTL = false;
									end = false;
								}
								break;
							}
							else if (charType[k] == C2_RIGHTTOLEFT)
							{
								break;
							}
						}
						if (isPreRTL)
							RTLSign[i + count] = 1;
						if (end)
							break;
					}
					else if (isPreRTL)
					{
						bool end = true;
						isPreRTL = false;
						// 檢查下一個字元
						for (int k = i + count + 1; k < size; k++)
						{
							if (charType[k] == C2_LEFTTORIGHT)
							{
								break;
							}
							else if (charType[k] == C2_RIGHTTOLEFT || charType[k] == C2_EUROPENUMBER || charType[k] == C2_ARABICNUMBER)
							{
								isPreRTL = true;
								end = false;
								break;
							}
						}
						if (isPreRTL)
							RTLSign[i + count] = 1;
						if (end)
							break;
					}
				}
				count++;
			}
			// RTL 需要將數字做反向
			vector<int> order;
			int j = 0;
			while (j < count)
			{
				int len = 1;
				if (charType[i] == C2_RIGHTTOLEFT || (isFirstRTL && (charType[i] != C2_LEFTTORIGHT)))
				{
					// 英文做反向
					if (charType[i + (count - j - len)] == C2_LEFTTORIGHT)
					{
						while (len < count - j)
						{
							bool isBreak = true;
							switch (charType[i + (count - j - len - 1)])
							{
							case C2_LEFTTORIGHT:
							case C2_EUROPENUMBER:
							case C2_EUROPESEPARATOR:
							case C2_EUROPETERMINATOR:
							case C2_COMMONSEPARATOR:
							case C2_BLOCKSEPARATOR:
							case C2_WHITESPACE:
							case C2_OTHERNEUTRAL:
								isBreak = false;
								len++;
								break;
							}
							if (isBreak)
								break;
						}
					}
					// 如果是RTL要將數字做反向
					else if (charType[i + (count - j - len)] == C2_EUROPENUMBER)
					{
						while (len < count - j)
						{
							bool isBreak = true;
							DWORD nextPosition = i + (count - j - len - 1);
							switch (charType[nextPosition])
							{
							case C2_EUROPENUMBER:
							case C2_EUROPESEPARATOR:
							case C2_EUROPETERMINATOR:
								isBreak = false;
								len++;
								break;
								// 符號檢查下一個字元類型是否為數字
							case C2_COMMONSEPARATOR:
								if (nextPosition > (DWORD)i &&
									charType[nextPosition - 1] == C2_EUROPENUMBER ||
									charType[nextPosition - 1] == C2_EUROPESEPARATOR)
								{
									isBreak = false;
									len++;
								}
								break;
							}
							if (isBreak)
								break;
						}
					}
					for (int n = len; n > 0; n--)
						order.push_back(i + (count - j - n));
				}
				else
				{
					order.push_back(i + j);
				}
				j += len;
			}
			if (isFirstRTL)
			{
				for (vector<int>::reverse_iterator iter = order.rbegin(); iter != order.rend(); iter++)
				{
					ls[r--] = *iter;
				}
			}
			else
			{
				for (vector<int>::iterator iter = order.begin(); iter != order.end(); iter++)
				{
					ls[l++] = *iter;
				}
			}
			i += count;
		}
	}
	for (int n = 0; n < size; n++)
	{
		CUiRGBA color;
		int index = ls[n] + begin;
		color = m_charData[index].color;
		color.SetAlphaScale(alpha);
		m_charData[index].fontString->Paint(x, y, scale, color.m_value, m_pShadow, 0, m_charData[index].bOutline, RTLSign[ls[n]]);
		x += (m_charData[index].fontString->GetXSize() + m_spacing) * scale;
	}
	/*
	else
	{
		for ( int i = begin; i < end; i++ )
		{
			CUiRGBA color;
			color = m_charData[i].color;
			color.SetAlphaScale(alpha);
			m_charData[i].fontString->Paint(x, y, scale, color.m_value, m_pShadow, 0, m_charData[i].bOutline );
			x += (m_charData[i].fontString->GetXSize() + m_spacing) * scale;
		}
	}
	*/
	delete[] charType;
}
// ----------------------------------------------------------------------------------
float CUiFontString::Separate(int index, int& end, float limitLength)
{
	WCHAR* text = m_charData[index].fontString->GetText();
	int textLength = m_charData[index].fontString->GetTextLength();
	float width = m_charData[index].fontString->GetXSize();
	if (limitLength < 0.0001f) limitLength = 100000.0f;
	index++;
	int begin = index;
	if (wcsncmp(text, L" ", 1) != 0)
	{
		// 文字字元,包函英文字母,控制字元以及特殊字元
		if (IsCharAlphabet(text, textLength))
		{
			while (true)
			{
				if (index >= (int)m_charData.size()) {
					break;
				}
				else if (!IsCharAlphabet(m_charData[index].fontString->GetText(), m_charData[index].fontString->GetTextLength())) {
					break;
				}
				else if (width >= limitLength) {
					break;
				}
				width += m_charData[index].fontString->GetXSize() + m_spacing;
				index++;
			}
		}
		// 數字
		else if (IsNumeric(text, textLength, false))
		{
			while (true)
			{
				if (index >= (int)m_charData.size()) {
					break;
				}
				else if (!IsNumeric(m_charData[index].fontString->GetText(), m_charData[index].fontString->GetTextLength(), true)) {
					break;
				}
				else if (width >= limitLength) {
					break;
				}
				width += m_charData[index].fontString->GetXSize() + m_spacing;
				index++;
			}
		}
	}
	// 最後一個字元超出最大寬度
	if (width > limitLength && index > begin)
	{
		index--;
		width -= m_charData[index].fontString->GetXSize() + m_spacing;
	}
	end = index;
	return width;
}
// ----------------------------------------------------------------------------------
float CUiFontString::CaretToPos(int begin, int caret)
{
	if (m_charData.empty())
		return 0;
	float pos = 0.0f;
	float scale = GetRealScale();
	//begin = TextToCaret(begin);
	//begin = TextToCaret(begin);
	caret = TextToCaret(caret);
	begin = min(max(begin, 0), (int)m_charData.size());
	caret = min(max(caret, 0), (int)m_charData.size());
	for (int i = begin; i < caret; i++)
		pos += (m_charData[i].fontString->GetXSize() + m_spacing) * scale;
	return pos;
}
// ----------------------------------------------------------------------------------
int CUiFontString::PosToCaret(int begin, float width, bool& trail)
{
	float scale = GetRealScale();
	float charWidth, spacing;
	int pos = max(begin, 0);
	spacing = m_spacing * scale;
	for (; pos < (int)m_charData.size(); pos++)
	{
		charWidth = m_charData[pos].fontString->GetXSize() * scale;
		if (charWidth > width)
		{
			trail = ((width / 2.0f) > charWidth);
			break;
		}
		width -= charWidth + spacing;
	}
	return pos;
}
// ----------------------------------------------------------------------------------
void CUiFontString::ResetCharData()
{
	m_charData.clear();
	m_hyperlinkData.clear();
	m_caretPosition.clear();
	s_charColor = m_color.GetColor();
	s_linkIndex = -1;
}
/*
// ----------------------------------------------------------------------------------
void CUiFontString::SetCharData(bool parse, const WCHAR* ws, int size)
{
	WCHAR toneSymbol[1024];
	int symbolCount = 0;
	if ( ws == NULL || m_pFontType == NULL )
		return;
	if ( size < 0 )
		size = wcslen(ws);
	bool bOutline  = false;
	int i = 0;
	while ( ws[i] && i < size )
	{
		bool valid = true;
		HyperlinkData link;
		symbolCount = 0;
		// 控制碼解釋
		if ( parse && ws[i] == L'|' )
		{
			const wchar_t* next = NULL;
			switch (ws[i+1])
			{
			// 連結
			case L'H':
				if ( next = wcsstr(&ws[i+2], L"|h") )
				{
					// 前段資料
					const wchar_t* begin = next + 2;
					// 後段資料
					const wchar_t* end = wcsstr(begin, L"|h");
					if ( end != NULL ) {
						valid = false;
						// 超鏈結關連
						{
							s_linkIndex = m_hyperlinkData.size();
							// 顯示字串
							SetCharData(true, begin, end - begin);
							// 記錄超鏈結字串區段
							link.begin = ws + i;
							link.count = end - (ws + i) + 2;
							m_hyperlinkData.push_back(link);
							s_linkIndex = -1;
						}
						i += end - (ws + i) + 2;
					}
				}
				break;
			// 顏色開始
			case L'c':
				if ( wcslen(&ws[i+2]) > 7 ) {
					valid = false;
					s_charColor = HexStrToInt(&ws[i+2]);
					i += 10;			// 2控字元 + 8個色碼
				}
				break;
			case L'o':
			case L'O':
				if ( wcslen(&ws[i+2]) > 7 ) {
					valid = false;
					s_charColor = HexStrToInt(&ws[i+2]);
					i += 10;			// 2控字元 + 8個色碼
					bOutline = true;
				}
				break;
			// 顏色結束,回復預設值
			case L'r':
				valid = false;
				s_charColor = m_color.GetColor();
				i += 2;
				bOutline = false;
				break;
			}
		}
		BOOL alpha = IsCharAlphaW(ws[i]);
		// 如果不是聲調或特殊符號,需要檢查下一個字元是否為聲調符號
		if ( IsToneSymbol(ws[i]) == false )
		{
			symbolCount = 0;
			while ( IsToneSymbol(ws[i + symbolCount + 1]) )
			{
				toneSymbol[symbolCount] = ws[i + symbolCount + 1];
				symbolCount++;
			}
		}
		toneSymbol[symbolCount] = 0;
		// 有效的字元
		if ( valid )
		{
			m_caretPosition.push_back(&ws[i] - m_text);
			CRuFontChar* ruFontChar = CRuFontChar::GetFontChar(m_pFontType, ws[i], toneSymbol);
			if ( ruFontChar ) {
				m_charData.push_back(CharInfo(ruFontChar, s_charColor, s_linkIndex, bOutline ));
			}
			// 加上聲調符號字數
			i += symbolCount;
			// 下一個字元
			i++;
		}
	}
}
*/
// ----------------------------------------------------------------------------------
void CUiFontString::SetCharData(bool parse, const WCHAR* ws, int size)
{
	if (ws == NULL || m_pFontType == NULL)
		return;
	if (size < 0)
		size = (int)wcslen(ws);
	bool bOutline = false;
	int i = 0;
	while (ws[i] && i < size)
	{
		bool valid = true;
		HyperlinkData link;
		// 控制碼解釋
		if (parse && ws[i] == L'|')
		{
			const wchar_t* next = NULL;
			switch (ws[i + 1])
			{
				// 連結
			case L'H':
				if (next = wcsstr(&ws[i + 2], L"|h"))
				{
					// 前段資料
					const wchar_t* begin = next + 2;
					// 後段資料
					const wchar_t* end = wcsstr(begin, L"|h");
					if (end != NULL) {
						valid = false;
						// 超鏈結關連
						{
							s_linkIndex = (int)m_hyperlinkData.size();
							// 顯示字串
							SetCharData(true, begin, int(end - begin));
							// 記錄超鏈結字串區段
							link.begin = ws + i;
							link.count = int(end - (ws + i) + 2);
							m_hyperlinkData.push_back(link);
							s_linkIndex = -1;
						}
						i += int(end - (ws + i) + 2);
					}
				}
				break;
				// 顏色開始
			case L'c':
				if (wcslen(&ws[i + 2]) > 7) {
					valid = false;
					s_charColor = HexStrToInt(&ws[i + 2]);
					i += 10;			// 2控字元 + 8個色碼
				}
				break;
			case L'o':
			case L'O':
				if (wcslen(&ws[i + 2]) > 7) {
					valid = false;
					s_charColor = HexStrToInt(&ws[i + 2]);
					i += 10;			// 2控字元 + 8個色碼
					bOutline = true;
				}
				break;
				// 顏色結束,回復預設值
			case L'r':
				valid = false;
				s_charColor = m_color.GetColor();
				i += 2;
				bOutline = false;
				break;
			}
		}
		if (valid)
		{
			// 下一個字元位置
			const wchar_t* charNext = CRuFontString::CharNext(&ws[i], size - i);
			int ln = int(charNext - &ws[i]);
			// 座標偏移值
			m_caretPosition.push_back(int(&ws[i] - s_textBegin));
			CRuFontString* ruFontString = CRuFontString::GetFontString(m_pFontType, ws + i, ln);
			if (ruFontString)
			{
				m_charData.push_back(CharInfo(ruFontString, s_charColor, s_linkIndex, bOutline));
			}
			i += ln;
		}
	}
}
/*
// ----------------------------------------------------------------------------------
void CUiFontString::SetCharData(bool parse, wstring& wText, int size)
{
	if ( wText.empty() || m_pFontType == NULL )
		return;
	bool bOutline  = false;
	int i = 0;
	while ( i < size )
	while ( strList.size() > 0 )
	{
		bool valid = true;
		HyperlinkData link;
	}
}
*/
/*
// ----------------------------------------------------------------------------------
void CUiFontString::SetCharData(bool parse, list<wstring>& strList)
{
	if ( strList.empty() || m_pFontType == NULL )
		return;
	bool bOutline  = false;
	while ( strList.size() > 0 )
	{
		bool valid = true;
		HyperlinkData link;
		if ( strList[0].compare(L"|") == 0 && strList.size() > 1 )
		{
			if ( strList[1].compare(L"H") == 0 )
			{
			}
			else if ( strList[1].compare(L"c") == 0 )
			{
				if ( strList.size() > 9 )
				{
					valid = false;
					bOutline = false;
					wstring colorStr;
					for ( int i = 0; i < 8; i++ )
						colorStr += strList[i];
					s_charColor = HexStrToInt(colorStr.c_str());
					// 2控字元 + 8個色碼
					for ( int i = 0; i < 10; i++ )
						strList.pop_front();
				}
			}
			else if ( strList[1].compare(L"o") == 0 )
			{
				if ( strList.size() > 9 )
				{
					valid = false;
					bOutline = true;
					wstring colorStr;
					for ( int i = 0; i < 8; i++ )
						colorStr += strList[i];
					s_charColor = HexStrToInt(colorStr.c_str());
					// 2控字元 + 8個色碼
					for ( int i = 0; i < 10; i++ )
						strList.pop_front();
				}
			}
			else if ( strList[1].compare(L"r") == 0 )
			{
				valid = false;
				bOutline = false;
				s_charColor = m_color.GetColor();
				for ( int i = 0; i < 2; i++)
					strList.pop_front();
			}
		}
	}
}
*/
// ----------------------------------------------------------------------------------
void CUiFontString::GetLineInfo()
{
	StringInfo strInfo;
	float scale = GetRealScale();
	int index = 0;
	int size = (int)m_charData.size();
	strInfo.begin = index;
	strInfo.end = index;
	strInfo.width = 0.0f;
	m_strData.clear();
	while (index < size)
	{
		if (m_charData[index].fontString->Compare(L"\n") == 0)
		{
			m_strData.push_back(strInfo);
			// 下一個開始字元
			while (true) {
				index++;
				if (index >= size || m_charData[index].fontString->Compare(L" ") != 0)
					break;
			}
			strInfo.begin = index;
			strInfo.end = index;
			strInfo.width = 0.0f;
		}
		else
		{
			int start = index;
			int end;
			//float w = Separate(start, end, m_size.m_x - strInfo.width);
			float w = Separate(start, end, m_size.m_x);
			// 兩字串兩加寬度大於元件寬度,則需將後半字串做拆解處理
			if (strInfo.width + w - m_size.m_x > 0.1f)
			{
				//if ( strInfo.end != strInfo.begin )
				if (strInfo.width > 0.0f)
				{
					m_strData.push_back(strInfo);
					// 下一個開始字元
					while (m_charData[index].fontString->Compare(L" ") == 0) {
						index++;
						start++;
						if (index >= size)
							break;
					}
					strInfo.begin = index;
					strInfo.end = index;
					strInfo.width = 0.0f;
				}
				float charWidth;
				for (int i = start; i < end; i++)
				{
					charWidth = m_charData[i].fontString->GetXSize();
					if (strInfo.width + charWidth > m_size.m_x)
					{
						m_strData.push_back(strInfo);
						// 下一個開始字元
						if (m_charData[i].fontString->Compare(L" ") == 0)
						{
							charWidth = 0.0f;
							while (m_charData[i].fontString->Compare(L" ") == 0) {
								i++;
								if (i >= size)
									break;
							}
						}
						if (i >= end)
							index = i;
						strInfo.begin = i;
						strInfo.end = i;
						strInfo.width = charWidth;
					}
					else
					{
						strInfo.width += charWidth + m_spacing;
						strInfo.end = i + 1;
					}
				}
			}
			else
			{
				strInfo.end = end;
				strInfo.width += w + m_spacing;
			}
			if (index < end)
				index = end;
		}
	}
	if (strInfo.end != strInfo.begin)
		m_strData.push_back(strInfo);
}
// ----------------------------------------------------------------------------------
void CUiFontString::ModifySize()
{
	if (m_pFontType == NULL)
		return;
	if (m_anchorSize.m_x == 0.0f || m_anchorSize.m_y == 0.0f)
	{
		CUiSize size = m_anchorSize;
		// 兩層對位,會有調整原本大小,所以以目前大小為準
		if (m_anchors.size() > 1)
		{
			// 必需設定寬高在沒有依據字串做調整
			if (m_anchorSize.m_x != 0.0f)
				size.m_x = m_size.m_x;
			if (m_anchorSize.m_y != 0.0f)
				size.m_y = m_size.m_y;
		}
		// 調整寬度
		if (m_anchorSize.m_x == 0.0f)
		{
			float x = 0.0f;
			int index = 0;
			for (; index < (int)m_charData.size(); index++)
			{
				if (m_charData[index].fontString->Compare(L"\n") == 0)
				{
					if (x > size.m_x)
						size.m_x = x;
					x = 0.0f;
				}
				else
					x += m_charData[index].fontString->GetXSize() + m_spacing;
			}
			if (x > size.m_x)
				size.m_x = x;
		}
		// 調整高度
		if (m_anchorSize.m_y == 0.0f)
		{
			float x = 0.0f;
			int index = 0;
			int length = (int)m_charData.size();
			while (index < length)
			{
				if (m_charData[index].fontString->Compare(L"\n") == 0)
				{
					// 跳掉空白字元,找尋下一個開始字元
					while (true) {
						index++;
						if (index >= length || m_charData[index].fontString->Compare(L" ") != 0)
							break;
					}
					x = 0.0f;
					size.m_y += m_pFontType->GetHeight() + m_spacing;
				}
				else
				{
					int end;
					float w = Separate(index, end, size.m_x);
					if (x + w - size.m_x > 0.1f)
					{
						// 換行
						size.m_y += m_pFontType->GetHeight() + m_spacing;
						x = 0.0f;
						// 下一個開始字元
						while (m_charData[index].fontString->Compare(L" ") == 0) {
							index++;
							if (index >= length)
								break;
						}
						// 分段處理
						while (index < end)
						{
							float charWidth = m_charData[index].fontString->GetXSize();
							if (x + charWidth < size.m_x)
							{
								x += charWidth + m_spacing;
								index++;
							}
							else
							{
								// 換行
								size.m_y += m_pFontType->GetHeight() + m_spacing;
								// 空白
								if (m_charData[index].fontString->Compare(L" ") == 0)
								{
									// 跳掉空白字元,找尋下一個非空白字元
									while (true) {
										index++;
										if (index >= length || m_charData[index].fontString->Compare(L" ") != 0)
											break;
									}
									x = 0.0f;
								}
								else
								{
									x = charWidth;
									index++;
								}
							}
						}
					}
					else
					{
						x += w + m_spacing;
					}
					if (index < end)
						index = end;
				}
			}
			if (x != 0.0f)
				size.m_y += m_pFontType->GetHeight() + m_spacing;
		}
		m_internalChangedSize = true;
		SetSize(size);
		m_internalChangedSize = false;
	}
}
// ----------------------------------------------------------------------------------
int CUiFontString::HexStrToInt(const WCHAR* valueStr)
{
	int value = 0;
	if (valueStr)
	{
		int len = (int)wcslen(valueStr);
		if (len > 8)
			len = 8;
		for (int i = 0; i < len; i++)
		{
			int lo = len - i - 1;
			if (valueStr[lo] >= L'0' && valueStr[lo] <= L'9')
				value += (valueStr[lo] - L'0') * (int)pow(16.0f, i);
			else if (valueStr[lo] >= L'A' && valueStr[lo] <= L'F')
				value += (valueStr[lo] - L'A' + 10) * (int)pow(16.0f, i);
			else if (valueStr[lo] >= L'a' && valueStr[lo] <= L'f')
				value += (valueStr[lo] - L'a' + 10) * (int)pow(16.0f, i);
		}
	}
	return value;
}
// ----------------------------------------------------------------------------------
float CUiFontString::GetDisplayWidth(int mode)
{
	float width = 0.0f;
	// 抓取最大值
	if (mode == 0)
	{
		for (vector<StringInfo>::iterator iter = m_strData.begin(); iter != m_strData.end(); iter++)
		{
			if (iter->width > width)
				width = iter->width;
		}
	}
	// 總長度
	else if (mode == 1)
	{
		for (vector<StringInfo>::iterator iter = m_strData.begin(); iter != m_strData.end(); iter++, width += m_spacing)
		{
			width += iter->width;
		}
	}
	return width;
}
// ----------------------------------------------------------------------------------
int CUiFontString::GetMemSize()
{
	int size = CUiLayout::GetMemSize() + sizeof(CUiFontString) - sizeof(CUiLayout);
	size += sizeof(CharInfo) * (int)m_charData.size();
	size += sizeof(StringInfo) * (int)m_strData.size();
	size += sizeof(HyperlinkData) * (int)m_hyperlinkData.size();
	size += sizeof(int) * (int)m_caretPosition.size();
	if (m_text)
		size += (int)(wcslen(m_text) + 1) * sizeof(WCHAR);
	if (m_pFontType)
		size += sizeof(CRuFontType);
	if (m_pDotChar)
		size += sizeof(CRuFontChar);
	if (m_pShadow)
		size += sizeof(RuFontShadow);
	return size;
}
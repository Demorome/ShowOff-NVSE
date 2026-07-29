#include "GameTypes.h"
#include "GameAPI.h"

String::String() {}

String::~String() {}

void String::Init(uint32_t bufSize)
{
	if (m_data) GameHeapFree(m_data);
	m_bufLen = bufSize;
	m_data = (char*)GameHeapAlloc(m_bufLen + 1);
	*m_data = 0;
	m_dataLen = 0;
}

bool String::Set(const char *src)
{
	m_dataLen = StrLen(src);
	if (!m_dataLen)
	{
		if (!src)
		{
			if (m_data)
			{
				GameHeapFree(m_data);
				m_data = NULL;
			}
			m_bufLen = 0;
		}
		else if (m_data) *m_data = 0;
		return true;
	}
	if (m_bufLen < m_dataLen)
	{
		m_bufLen = m_dataLen;
		if (m_data) GameHeapFree(m_data);
		m_data = (char*)GameHeapAlloc(m_dataLen + 1);
	}
	memcpy(m_data, src, m_dataLen);
	m_data[m_dataLen] = 0;
	return true;
}

bool String::Append(const char *toAppend)
{
	uint16_t length = StrLen(toAppend);
	if (!length) return true;
	uint16_t newLen = m_dataLen + length;
	if (m_bufLen < newLen)
	{
		m_bufLen = newLen;
		char *newStr = (char*)GameHeapAlloc(m_bufLen + 1);
		if (m_data)
		{
			memcpy(newStr, m_data, m_dataLen);
			GameHeapFree(m_data);
		}
		m_data = newStr;
	}
	memcpy(m_data + m_dataLen, toAppend, length);
	m_dataLen = newLen;
	m_data[m_dataLen] = 0;
	return true;
}

void String::AppendChar(char toAppend)
{
	if (m_bufLen == m_dataLen)
	{
		m_bufLen++;
		char *newStr = (char*)GameHeapAlloc(m_bufLen + 1);
		if (m_data)
		{
			memcpy(newStr, m_data, m_dataLen);
			GameHeapFree(m_data);
		}
		m_data = newStr;
	}
	m_data[m_dataLen++] = toAppend;
	m_data[m_dataLen] = 0;
}

void String::InsertChar(char toInsert, uint32_t index)
{
	if (index > m_dataLen) return;
	if (m_bufLen == m_dataLen)
	{
		m_bufLen++;
		char *newStr = (char*)GameHeapAlloc(m_bufLen + 1);
		if (m_data)
		{
			memcpy(newStr, m_data, m_dataLen);
			GameHeapFree(m_data);
		}
		m_data = newStr;
	}
	if (index < m_dataLen)
		memcpy(m_data + index + 1, m_data + index, m_dataLen - index);
	m_dataLen++;
	m_data[index] = toInsert;
	m_data[m_dataLen] = 0;
}

void String::EraseAt(uint32_t index)
{
	if (!m_data || (index >= m_dataLen)) return;
	m_dataLen--;
	if (m_dataLen)
	{
		uint32_t length = m_dataLen - index;
		if (!length)
			m_data[m_dataLen] = 0;
		else
			memmove(m_data + index, m_data + index + 1, length + 1);
	}
	else *m_data = 0;
}

const char *String::CStr()
{
	return (m_data && m_dataLen) ? m_data : "";
}

size_t String::GetLength()
{
	// Copy code from 0x4048E0
	if (m_dataLen == 0xFFFF)
		return m_data ? strlen(m_data) : 0;
	else
		return m_dataLen;
}

#include "Utilities.h"

const std::string & GetFalloutDirectory(void)
{
	static std::string s_falloutDirectory;

	if(s_falloutDirectory.empty())
	{
		// can't determine how many bytes we'll need, hope it's not more than MAX_PATH
		char	falloutPathBuf[MAX_PATH];
		uint32_t	falloutPathLength = GetModuleFileName(GetModuleHandle(NULL), falloutPathBuf, sizeof(falloutPathBuf));

		if(falloutPathLength && (falloutPathLength < sizeof(falloutPathBuf)))
		{
			std::string	falloutPath(falloutPathBuf, falloutPathLength);

			// truncate at last slash
			std::string::size_type	lastSlash = falloutPath.rfind('\\');
			if(lastSlash != std::string::npos)	// if we don't find a slash something is VERY WRONG
		{
				s_falloutDirectory = falloutPath.substr(0, lastSlash + 1);

				_DMESSAGE("fallout root = %s", s_falloutDirectory.c_str());
			}
			else
			{
				_WARNING("no slash in fallout path? (%s)", falloutPath.c_str());
			}
		}
		else
		{
			_WARNING("couldn't find fallout path (len = %d, err = %08X)", falloutPathLength, GetLastError());
		}
	}

	return s_falloutDirectory;
}

char* CopyCString(const char* src) {
	uint32_t length = strlen(src);
	if (!length) return NULL;
	char* result = (char*)GameHeapAlloc(length + 1);
	strcpy(result, src);
	return result;
}

//taken from xNVSE
std::string FormatString(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	char msg[0x400];
	vsprintf_s(msg, 0x400, fmt, args);
	return msg;
}

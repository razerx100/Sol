#ifndef __EXCEPTION_MESSAGE_BOX_HPP__
#define __EXCEPTION_MESSAGE_BOX_HPP__

__declspec(dllimport) void _cdecl ExceptionMessageBox(
	const char* exceptionDetails,
	const char* exceptionType
);

#endif

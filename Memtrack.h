#pragma once 

#ifdef _DEBUG

#include <Windows.h>
#include <list>
#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <vector>

typedef struct alloc_info_t
{
	DWORD address;
	DWORD size;
	char file[128];
	DWORD line;
};

typedef std::list<alloc_info_t *> AllocationTable;

extern AllocationTable *allocationTable;

void * __cdecl operator new(size_t size, char *file, int line);
void __cdecl operator delete(void *p);
void * __cdecl operator new[](size_t size, char *file, int line);
void __cdecl operator delete[](void *p);

#define new new(__FILE__, __LINE__)

void addTrack(DWORD address, DWORD addrSize, char *filename, DWORD lineNumber);
void removeTrack(DWORD address);
void dumpmem();
char *str_replace(char *orig, char *rep, char *with);
std::vector<std::string> getFileNameFromPath(const char *str, char c);
#endif




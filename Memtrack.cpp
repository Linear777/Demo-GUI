#include "Memtrack.h"
#include <new>
#undef new

#ifdef _DEBUG


AllocationTable *allocationTable;

void * operator new(size_t size, char *file, int line)
{
	void *ptr = (void *)malloc(size);
	addTrack((DWORD)ptr, size, file, line);
	return ptr;
}

void operator delete(void *p)
{
	removeTrack((DWORD)p);
	free(p);
	p = NULL;
}

void * operator new[](size_t size, char *file, int line)
{
	void *ptr = (void *)malloc(size);
	addTrack((unsigned int)ptr, size, file, line);
	return(ptr);
};

void operator delete[](void *p)
{
	removeTrack((unsigned int)p);
	free(p);
};

void addTrack(DWORD address, DWORD addrSize, char *filename, DWORD lineNumber)
{
	alloc_info_t *info;

	if (allocationTable == NULL)
	{
		allocationTable = new (AllocationTable);
	}
	info = new (alloc_info_t);
	info->address = address;
	std::vector<std::string> str = getFileNameFromPath(filename, '\\');
	strncpy(info->file,	str.back().c_str(), 128);
	info->line = lineNumber;
	info->size = addrSize;
	allocationTable->insert(allocationTable->begin(), info);
}

void removeTrack(DWORD address)
{
	AllocationTable::iterator it;

	if (allocationTable == NULL)
	{
		return;
	}

	for (it = allocationTable->begin(); it != allocationTable->end(); it++)
	{
		if ((*it)->address == address)
		{
			allocationTable->remove((*it));
			break;
		}
	}
}

void dumpmem()
{
	AllocationTable::iterator it;
	DWORD totalSize = 0;

	if (allocationTable == NULL)
	{
		return;
	}
	if (allocationTable->size() == 0)
	{
		return;
	}
	time_t t = time(0);   // get time now
	struct tm * now = localtime(&t);

	std::string logfileName = std::string("logfiles/memory/memdump-");


	logfileName.append(std::to_string(now->tm_year + 1900) + std::to_string((now->tm_mon + 1)) + std::to_string(now->tm_mday) + "-" + std::to_string(now->tm_hour) + "-" + std::to_string(now->tm_min) + "-" + std::to_string(now->tm_sec) + ".log");

	std::ofstream logfile;
	logfile.open(logfileName, std::fstream::out);

	logfile << "Memory Log Dump by Red Datura Softworks s.r.o\n";
	logfile << "Filename: " << logfileName << "\n";
	logfile << "Date: " << std::to_string((now->tm_mday)) + "." + std::to_string(now->tm_mon + 1) + "." + std::to_string(now->tm_year + 1900) + "\n";
	logfile << "Time: " << std::to_string(now->tm_hour) + ":" + std::to_string(now->tm_min) + ":" + std::to_string(now->tm_sec) + "\n\n";
	logfile << "-----------------------------------------DUMP BEGIN-----------------------------------------------------------------------\n";

	for (it = allocationTable->begin(); it != allocationTable->end(); it++)
	{
		logfile << (*it)->file << ":\tline: " << (*it)->line << ", \tat address: " << (*it)->address << "\t" << (*it)->size << " bytes of unfreed memory\n";
		totalSize += (*it)->size;
	}

	logfile << "----------------------------------------- DUMP END -----------------------------------------------------------------------\n";

	logfile << "Total unfreed memory: " << totalSize << " bytes\n";
	logfile.close();
}

std::vector<std::string> getFileNameFromPath(const char *str, char c)
{
	std::vector<std::string> result;

	do
	{
		const char *begin = str;

		while (*str != c && *str)
		{
			str++;
		}
		result.push_back(std::string(begin, str));
	} while (0 != *str++);

	return result;
}

#endif


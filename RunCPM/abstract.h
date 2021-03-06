#ifndef ABSTRACT_H
#define ABSTRACT_H

/* see main.c for definition */
#include <conio.h>
#include <ctype.h>
#include <dir.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Memory abstraction functions */
/*===============================================================================*/

/* Filesystem (disk) abstraction fuctions */
/*===============================================================================*/
struct ffblk fnd;

typedef struct {
	uint8 dr;
	uint8 fn[8];
	uint8 tp[3];
	uint8 ex, s1, s2, rc;
	uint8 al[16];
	uint8 cr, r0, r1, r2;
} CPM_FCB;

FILE* _sys_fopen_r(uint8 *filename) {
	return(fopen((const char*)filename, "rb"));
}

FILE* _sys_fopen_w(uint8 *filename) {
	return(fopen((const char*)filename, "wb"));
}

FILE* _sys_fopen_rw(uint8 *filename) {
	return(fopen((const char*)filename, "r+b"));
}

FILE* _sys_fopen_a(uint8 *filename) {
	return(fopen((const char*)filename, "a"));
}

int _sys_fseek(FILE *file, long delta, int origin) {
	return(fseek(file, delta, origin));
}

long _sys_ftell(FILE *file) {
	return(ftell(file));
}

long _sys_fread(void *buffer, long size, long count, FILE *file) {
	return(fread(buffer, size, count, file));
}

long _sys_fwrite(const void *buffer, long size, long count, FILE *file) {
	return(fwrite(buffer, size, count, file));
}

int _sys_feof(FILE *file) {
	return(feof(file));
}

int _sys_fclose(FILE *file) {
	return(fclose(file));
}

int _sys_remove(uint8 *filename) {
	return(remove((const char*)filename));
}

int _sys_select(uint8 *disk) {
	uint8 result;
	DIR *d;
	if ((d = opendir((char*)disk)) != NULL) {
		result = 1;
		closedir(d);
	} else {
		result = 0;
	}
	return(result);
}

uint8 _FCBtoHostname(uint16 fcbaddr, uint8 *filename) {
	CPM_FCB *F = (CPM_FCB*)&RAM[fcbaddr];
	uint8 i = 0;
	uint8 unique = TRUE;

	if (F->dr) {
		*(filename++) = (F->dr - 1) + 'A';
	} else {
		*(filename++) = (_RamRead(0x0004) & 0x0f) + 'A';
	}
	*(filename++) = '\\';

	while (i < 8) {
		if (F->fn[i] > 32)
			*(filename++) = F->fn[i];
		if (F->fn[i] == '?')
			unique = FALSE;
		i++;
	}
	*(filename++) = '.';
	i = 0;
	while (i < 3) {
		if (F->tp[i] > 32)
			*(filename++) = F->tp[i];
		if (F->tp[i] == '?')
			unique = FALSE;
		i++;
	}
	*filename = 0x00;

	return(unique);
}

void _HostnameToFCB(uint16 fcbaddr, uint8 *filename) {
	CPM_FCB *F = (CPM_FCB*)&RAM[fcbaddr];
	int32 i = 0;

	while (*filename != 0 && *filename != '.') {
		F->fn[i] = toupper(*filename);
		filename++;
		i++;
	}
	while (i < 8) {
		F->fn[i] = ' ';
		i++;
	}
	if (*filename == '.')
		filename++;
	i = 0;
	while (*filename != 0) {
		F->tp[i] = toupper(*filename);
		filename++;
		i++;
	}
	while (i < 3) {
		F->tp[i] = ' ';
		i++;
	}
}

uint8 _findfirst(void) {
	uint8 result = 0xff;
	uint8 found;

	found = findfirst(filename, &fnd, 0);
	if (found == 0) {
		_HostnameToFCB(dmaAddr, fnd.ff_name);
		_RamWrite(dmaAddr, 0);	// Sets the user of the requested file correctly on DIR entry
		result = 0x00;
	}
	return(result);
}

uint8 _findnext(void) {
	uint8 result = 0xff;
	uint8 more;

	more = findnext(&fnd);
	if (more == 0) {
		_HostnameToFCB(dmaAddr, fnd.ff_name);
		_RamWrite(dmaAddr, 0);	// Sets the user of the requested file correctly on DIR entry
		result = 0x00;
	}
	return(result);
}

uint8 _Truncate(char *fn, uint8 rc) {
	uint8 result = 0x00;
	if (truncate(fn, rc * 128))
		result = 0xff;
	return(result);
}

/* Console abstraction functions */
/*===============================================================================*/
void _console_init(void) {
}

void _console_reset(void) {

}

int _kbhit(void) {
	return kbhit();
}

unsigned char _getch(void) {
	return getch();
}

unsigned char _getche(void) {
	return getche();
}

void _putch(uint8 ch) {
	putch(ch);
}

void _clrscr(void) {
	system("cls");
}

#endif
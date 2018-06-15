#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <salis.h>
#include "printer.h"
#include "handler.h"
#include "tsalis.h"

#define CONSOLE_INPUT_LENGTH 64

static sbyte
symbolToInst(char charSymbol)
{
#define SINST(name, symbol) case symbol: return name;
#define SILST(name, symbol) case symbol: return name;

	switch (charSymbol) {
		SINST_LIST
	}

#undef SINST
#undef SILST
	return (sbyte)(-1);
}

static void
writeInstructions(const char *command)
{
	sword  addr   = atoi(command);
	char  *symbol = strchr(command, '_');

	if (symbol) {
		symbol++;

		while (sm_isValidAt(addr) && *symbol) {
			sbyte svalue = symbolToInst(*symbol);

			if (si_isInst(svalue)) {
				sm_setInstAt(addr, svalue);
			} else {
				sm_setInstAt(addr, 0);
			}

			addr++;
			symbol++;
		}
	}
}

static void
compileGenome(const char *command)
{
	sword  addr     = atoi(command);
	char  *fileName = strchr(command, '_');

	if (fileName) {
		FILE *file = fopen(fileName + 1, "r");

		if (file) {
			while (addr < sm_getSize()) {
				int   symbol = fgetc(file);
				sbyte svalue = symbolToInst((char)symbol);

				if (symbol == EOF) {
					break;
				}

				if (si_isInst(svalue)) {
					sm_setInstAt(addr, svalue);
				} else {
					sm_setInstAt(addr, 0);
				}

				addr++;
			}
		}
	}
}

static void
createOrganism(const char *command)
{
	sword  addr  = atoi(command);
	char  *sizep = strchr(command, '_');

	if (sizep) {
		sword size = atoi(sizep + 1);

		if (sm_isValidAt(addr) && sm_isValidAt(addr + size - 1)) {
			sword offset;

			for (offset = 0; offset < size; offset++) {
				if (sm_isAllocatedAt(addr + offset)) {
					return;
				}
			}

			sp_create(addr, size);
		}
	}
}

static void
killOrganism(void)
{
	sp_kill();
}

static void
moveTo(const char *command)
{
	sword loc = atoi(command);
	tsp_moveTo(loc);
}

static void
selectProc(const char *command)
{
	sword proc = atoi(command);
	tsp_selectProcess(proc);
}

static void
saveSim(void)
{
	s_save(g_simName);
}

static void
renameSim(const char *name)
{
	if (strlen(name) <= NAME_MAX_SIZE) {
		clear();
		strncpy(g_simName, name, NAME_MAX_SIZE);
	}
}

static void
setAutoSave(const char *command)
{
	g_autoSaveInterval = atoi(command);
}

static void
clearConsoleLine(void)
{
	move(LINES - 1, 0);
	clrtoeol();
}

static void
runConsole(void)
{
	char command[CONSOLE_INPUT_LENGTH] = {0};
	clearConsoleLine();
	echo();
	mvprintw(LINES - 1, 1, "$ ");
	curs_set(TRUE);
	getnstr(command, CONSOLE_INPUT_LENGTH - 1);
	curs_set(FALSE);
	noecho();
	clearConsoleLine();

	switch (command[0]) {
	case 'q':
		if (command[1] != '!') {
			s_save(g_simName);
		}

		g_exit = STRUE;
		break;

	case 'i':
		writeInstructions(&command[1]);
		break;

	case 'c':
		compileGenome(&command[1]);
		break;

	case 'n':
		createOrganism(&command[1]);
		break;

	case 'k':
		killOrganism();
		break;

	case 'm':
		moveTo(&command[1]);
		break;

	case 'p':
		selectProc(&command[1]);
		break;

	case 's':
		saveSim();
		break;

	case 'r':
		renameSim(&command[1]);
		break;

	case 'a':
		setAutoSave(&command[1]);
		break;
	}
}

void
tsh_handleEvent(int event)
{
	switch (event) {
	case KEY_RESIZE:
		tsp_onResize();
		break;

	case KEY_LEFT:
		tsp_prevPage();
		break;

	case KEY_RIGHT:
		tsp_nextPage();
		break;

	case 'w':
		tsp_scrollDown();
		break;

	case 'a':
		tsp_scrollLeft();
		break;

	case 's':
		tsp_scrollUp();
		break;

	case 'd':
		tsp_scrollRight();
		break;

	case 'Q':
		tsp_scrollToTop();
		break;

	case 'A':
		tsp_scrollToLeft();
		break;

	case 'z':
		tsp_zoomIn();
		break;

	case 'x':
		tsp_zoomOut();
		break;

	case 'o':
		tsp_prevOrganism();
		break;

	case 'p':
		tsp_nextOrganism();
		break;

	case 'g':
		g_processShowGenes = !g_processShowGenes;
		break;

	case 'c':
		g_running = SFALSE;
		nodelay(stdscr, SFALSE);
		tsp_printData();
		runConsole();
		break;

	case ' ':
		g_running = !g_running;
		nodelay(stdscr, g_running);
		break;

	case 'j':
		if (sp_getCount()) {
			g_selectedProcess = sp_getFirst();
		}

		break;

	case 'l':
		if (sp_getCount()) {
			g_selectedProcess = sp_getLast();
		}

		break;

	case 'k':
		tsp_gotoSelectedProc();
		break;

	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '0':
		if (!g_running) {
			int power  = ((event - '0') ? (event - '0') : 10) - 1;
			int cycles = 1 << power;

			while (cycles--) {
				s_cycle();
			}
		}

		break;
	}
}

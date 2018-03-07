#include <stdarg.h>
#include <curses.h>
#include <salis.h>
#include "printer.h"
#include "tsalis.h"

enum {
	PAIR_NORMAL          =  1,
	PAIR_HEADER          =  2,
	PAIR_SELECTED_PROC   =  3,
	PAIR_FREE_CELL       =  4,
	PAIR_ALLOC_CELL      =  5,
	PAIR_MEM_BLOCK_START =  6,
	PAIR_MEM_BLOCK_2     =  7,
	PAIR_MEM_BLOCK_1     =  8,
	PAIR_SELECTED_SP     =  9,
	PAIR_SELECTED_IP     = 10
};

enum {
	PAGE_MEMORY,
	PAGE_EVOLVER,
	PAGE_PROCESS,
	PAGE_WORLD,
	PAGE_COUNT
};

const char *g_instNames[] = {
#define SINST(name, symbol) #name,
#define SILST(name, symbol) #name
	SINST_LIST
#undef SINST
#undef SILST
};

const char g_instSymbols[] = {
#define SINST(name, symbol) symbol,
#define SILST(name, symbol) symbol
	SINST_LIST
#undef SINST
#undef SILST
};

const char *g_procElems[] = {
	"mb1a",
	"mb1s",
	"mb2a",
	"mb2s",
	"ip",
	"sp",
	"reg[0]",
	"reg[1]",
	"reg[2]",
	"reg[3]",
	"stack[0]",
	"stack[1]",
	"stack[2]",
	"stack[3]",
	"stack[4]",
	"stack[5]",
	"stack[6]",
	"stack[7]"
};

const int PROC_ELEMENT_COUNT = (sizeof(g_procElems) / sizeof(*g_procElems));
const int DATA_WIDTH         = 25;

int   g_currentPage;
sword g_selectedProcess;
sbool g_processShowGenes;
sword g_processVertScroll;
sword g_processDataScroll;
sword g_processGeneScroll;
sword g_worldPos;
sword g_worldZoom;
sword g_worldLineWidth;
sword g_worldLineCoverage;
sword g_worldArea;

static void
adjustWorld(void)
{
	g_worldLineWidth    = COLS - DATA_WIDTH;
	g_worldLineCoverage = g_worldLineWidth * g_worldZoom;
	g_worldArea         = LINES * g_worldLineCoverage;

	while ((g_worldArea > (sm_getSize() * 2)) && (g_worldZoom > 1)) {
		g_worldZoom /= 2;
	}
}

void
tsp_init(void)
{
	initscr();
	cbreak();
	noecho();
	curs_set(0);
	keypad(stdscr, TRUE);
	start_color();
	init_pair(PAIR_NORMAL,          COLOR_WHITE,  COLOR_BLACK);
	init_pair(PAIR_HEADER,          COLOR_CYAN,   COLOR_BLACK);
	init_pair(PAIR_SELECTED_PROC,   COLOR_YELLOW, COLOR_BLACK);
	init_pair(PAIR_FREE_CELL,       COLOR_CYAN,   COLOR_BLUE);
	init_pair(PAIR_ALLOC_CELL,      COLOR_BLUE,   COLOR_CYAN);
	init_pair(PAIR_MEM_BLOCK_START, COLOR_BLUE,   COLOR_WHITE);
	init_pair(PAIR_MEM_BLOCK_1,     COLOR_BLACK,  COLOR_YELLOW);
	init_pair(PAIR_MEM_BLOCK_2,     COLOR_BLACK,  COLOR_GREEN);
	init_pair(PAIR_SELECTED_SP,     COLOR_BLACK,  COLOR_MAGENTA);
	init_pair(PAIR_SELECTED_IP,     COLOR_BLACK,  COLOR_RED);
	g_worldZoom = 1;
	adjustWorld();
}

void
tsp_quit(void)
{
	endwin();
}

void
tsp_onResize(void)
{
	clear();
	adjustWorld();
}

void
tsp_prevPage(void)
{
	clear();
	g_currentPage += (PAGE_COUNT - 1);
	g_currentPage %= PAGE_COUNT;
}

void
tsp_nextPage(void)
{
	clear();
	g_currentPage++;
	g_currentPage %= PAGE_COUNT;
}

void
tsp_scrollUp(void)
{
	switch (g_currentPage) {
	case PAGE_PROCESS:
		if (g_processVertScroll) {
			g_processVertScroll--;
		}

		break;

	case PAGE_WORLD:
		if (g_worldPos >= g_worldLineCoverage) {
			g_worldPos -= g_worldLineCoverage;
		}

		break;
	}

	refresh();
}

void
tsp_scrollDown(void)
{
	switch (g_currentPage) {
	case PAGE_PROCESS:
		if (g_processVertScroll < (sp_getCap() - 1)) {
			g_processVertScroll++;
		}

		break;

	case PAGE_WORLD:
		if ((g_worldPos + g_worldLineCoverage) < sm_getSize()) {
			g_worldPos += g_worldLineCoverage;
		}

		break;
	}

	refresh();
}

void
tsp_scrollLeft(void)
{
	switch (g_currentPage) {
	case PAGE_PROCESS:
		if (g_processShowGenes) {
			if (g_processGeneScroll) {
				g_processGeneScroll--;
			}
		} else {
			if (g_processDataScroll) {
				g_processDataScroll--;
			}
		}

		break;

	case PAGE_WORLD:
		if (g_worldPos >= g_worldZoom) {
			g_worldPos -= g_worldZoom;
		} else {
			g_worldPos = 0;
		}

		break;
	}

	refresh();
}

void
tsp_scrollRight(void)
{
	switch (g_currentPage) {
	case PAGE_PROCESS:
		if (g_processShowGenes) {
			g_processGeneScroll++;
		} else {
			if (g_processDataScroll < (sword)(PROC_ELEMENT_COUNT - 1)) {
				g_processDataScroll++;
			}
		}

		break;

	case PAGE_WORLD:
		if ((g_worldPos + g_worldZoom) < sm_getSize()) {
			g_worldPos += g_worldZoom;
		}

		break;
	}

	refresh();
}

void
tsp_scrollToTop(void)
{
	switch (g_currentPage) {
	case PAGE_PROCESS:
		g_processVertScroll = 0;
		break;

	case PAGE_WORLD:
		g_worldPos = 0;
		break;
	}
}

void
tsp_scrollToLeft(void)
{
	switch (g_currentPage) {
	case PAGE_PROCESS:
		if (g_processShowGenes) {
			g_processGeneScroll = 0;
		} else {
			g_processDataScroll = 0;
		}

		break;
	}
}

void
tsp_zoomIn(void)
{
	if (g_currentPage == PAGE_WORLD) {
		if (g_worldZoom > 1) {
			g_worldZoom /= 2;
		}

		adjustWorld();
	}
}

void
tsp_zoomOut(void)
{
	if (g_currentPage == PAGE_WORLD) {
		if (g_worldArea < sm_getSize()) {
			g_worldZoom *= 2;
			refresh();
		}

		adjustWorld();
	}
}

void
tsp_prevOrganism(void)
{
	switch (g_currentPage) {
	case PAGE_PROCESS:
	case PAGE_WORLD:
		if (g_selectedProcess < sp_getCap()) {
			g_selectedProcess += (sp_getCap() - 1);
			g_selectedProcess %= sp_getCap();
		}

		break;
	}

	refresh();
}

void
tsp_nextOrganism(void)
{
	switch (g_currentPage) {
	case PAGE_PROCESS:
	case PAGE_WORLD:
		if (g_selectedProcess < sp_getCap()) {
			g_selectedProcess++;
			g_selectedProcess %= sp_getCap();
		}

		break;
	}

	refresh();
}

void
tsp_gotoSelectedProc(void)
{
	switch (g_currentPage) {
	case PAGE_PROCESS:
		g_processVertScroll = g_selectedProcess;
		break;

	case PAGE_WORLD:
		g_worldPos = sp_getProc(g_selectedProcess).mb1a;
		break;
	}
}

void
tsp_selectProcess(sword proc)
{
	if (proc < sp_getCap()) {
		g_selectedProcess = proc;
	}
}

void
tsp_moveTo(sword loc)
{
	switch (g_currentPage) {
	case PAGE_PROCESS:
		if (loc < sp_getCap()) {
			g_processVertScroll = loc;
		}

		break;

	case PAGE_WORLD:
		if (loc < sm_getSize()) {
			g_worldPos = loc;
		}

		break;
	}
}

static void
printWidget(int line, const char *format, ...)
{
	if (line < LINES) {
		va_list args;
		char    dataLine[24];
		va_start(args, format);
		vsprintf(dataLine, format, args);
		mvprintw(line, 1, "%.*s", COLS - 1, dataLine);
		va_end(args);
	}
}

static void
printHeader(int line, const char *string)
{
	attron(COLOR_PAIR(PAIR_HEADER));
	printWidget(line, string);
	standend();
}

#define PHEADER(label)       printHeader((*line)++, label)
#define PWIDGET(label, data) printWidget((*line)++, "%-10s : %10u", label, data)
#define PSIDGET(label, data) printWidget((*line)++, "%-10s : %10s", label, data)
#define INCREMENT_LINE       (*line)++

static void
printMemoryPage(int *line)
{
	PHEADER("MEMORY");
	PWIDGET("order",  sm_getOrder());
	PWIDGET("size",   sm_getSize());
	PWIDGET("blocks", sm_getMemBlockCount());
	PWIDGET("alloc",  sm_getAllocated());
	PWIDGET("cap",    sm_getCap());
}

static void
printEvolverPage(int *line)
{
	PHEADER("EVOLVER");
	PWIDGET("lastAddr", se_getLastAddress());
	PSIDGET("lastInst", g_instNames[se_getLastInst()] + 1);
	PWIDGET("state0",   se_getState(0));
	PWIDGET("state1",   se_getState(1));
	PWIDGET("state2",   se_getState(2));
	PWIDGET("state3",   se_getState(3));
}

static void
printField(int y, int x, const char *field, sbool lalign)
{
	if ((y < LINES) && (x < COLS)) {
		if (lalign) {
			mvprintw(y, x, "%-.*s", COLS - x, field);
		} else {
			mvprintw(y, x, "%.*s", COLS - x, field);
		}
	}
}

static void
printSingleProcessGenome(int line, sword pidx)
{
	char  sidx[11];
	SProc proc = sp_getProc(pidx);
	sword gidx = g_processGeneScroll;
	int   xpos = 14;

	if (pidx == g_selectedProcess) {
		attron(COLOR_PAIR(PAIR_SELECTED_PROC));
	} else if (!sp_isFree(pidx)) {
		attron(COLOR_PAIR(PAIR_HEADER));
	}

	sprintf(sidx, "%-10u |", pidx);
	printField(line, 1, sidx, STRUE);
	move(line, xpos);

	while ((gidx < proc.mb1s) && (xpos < COLS)) {
		sword gaddr = proc.mb1a + gidx;

		if (gaddr == proc.ip) {
			attron(COLOR_PAIR(PAIR_SELECTED_IP));
		} else if (gaddr == proc.sp) {
			attron(COLOR_PAIR(PAIR_SELECTED_SP));
		} else {
			attron(COLOR_PAIR(PAIR_MEM_BLOCK_1));
		}

		addch(g_instSymbols[sm_getInstAt(gaddr)]);
		gidx++;
		xpos++;
	}

	if (proc.mb1s < g_processGeneScroll) {
		gidx = g_processGeneScroll - proc.mb1s;
	} else {
		gidx = 0;
	}

	while ((gidx < proc.mb2s) && (xpos < COLS)) {
		sword gaddr = proc.mb2a + gidx;

		if (gaddr == proc.ip) {
			attron(COLOR_PAIR(PAIR_SELECTED_IP));
		} else if (gaddr == proc.sp) {
			attron(COLOR_PAIR(PAIR_SELECTED_SP));
		} else {
			attron(COLOR_PAIR(PAIR_MEM_BLOCK_2));
		}

		addch(g_instSymbols[sm_getInstAt(gaddr)]);
		gidx++;
		xpos++;
	}

	standend();
}

static void
printProcessGenes(int *line)
{
	sword pidx = g_processVertScroll;
	attron(COLOR_PAIR(PAIR_HEADER));
	printField(*line, 1, "pidx", STRUE);

	standend();
	INCREMENT_LINE;

	while ((*line < LINES) && (pidx < sp_getCap())) {
		printSingleProcessGenome(*line, pidx);
		INCREMENT_LINE;
		pidx++;
	}

	standend();
}

static void
printSingleProcessData(int line, sword pidx)
{
	char   sidx[11];
	int    eidx = g_processDataScroll;
	int    xpos = 12;
	SProc  proc = sp_getProc(pidx);
	sword *data = (sword *)&proc;

	if (pidx == g_selectedProcess) {
		attron(COLOR_PAIR(PAIR_SELECTED_PROC));
	} else if (!sp_isFree(pidx)) {
		attron(COLOR_PAIR(PAIR_HEADER));
	}

	sprintf(sidx, "%u", pidx);
	printField(line, 1, sidx, STRUE);

	while (eidx < PROC_ELEMENT_COUNT) {
		char element[13];
		sprintf(element, "| %10u", data[eidx]);
		printField(line, xpos, element, SFALSE);
		eidx++;
		xpos += 13;
	}

	standend();
}

static void
printProcessData(int *line)
{
	sword pidx = g_processVertScroll;
	int   eidx = g_processDataScroll;
	int   xpos = 12;
	attron(COLOR_PAIR(PAIR_HEADER));
	printField(*line, 1, "pidx", STRUE);

	while (eidx < PROC_ELEMENT_COUNT) {
		char element[13];
		sprintf(element, "| %10s", g_procElems[eidx]);
		printField(*line, xpos, element, SFALSE);
		eidx++;
		xpos += 13;
	}

	standend();
	INCREMENT_LINE;

	while ((*line < LINES) && (pidx < sp_getCap())) {
		printSingleProcessData(*line, pidx);
		INCREMENT_LINE;
		pidx++;
	}

	standend();
}

static void
printProcessPage(int *line)
{
	int   cline;
	sbool fnull = (sp_getFirst() == (sword) - 1);
	sbool lnull = (sp_getLast()  == (sword) - 1);
	PHEADER("PROCESS");
	PWIDGET("count", sp_getCount());
	PWIDGET("cap",   sp_getCap());
	fnull ? PSIDGET("first", "---") : PWIDGET("first", sp_getFirst());
	lnull ? PSIDGET("last",  "---") : PWIDGET("last",  sp_getLast());
	PWIDGET("selected", g_selectedProcess);
	INCREMENT_LINE;

	for (cline = *line; cline < LINES; cline++) {
		move(cline, 0);
		clrtoeol();
	}

	if (g_processShowGenes) {
		printProcessGenes(line);
	} else {
		printProcessData(line);
	}
}

static int
getColorOf(sword addr)
{
	if (!sp_isFree(g_selectedProcess)) {
		SProc proc = sp_getProc(g_selectedProcess);

		if (addr == proc.ip) {
			return PAIR_SELECTED_IP;
		} else if (addr == proc.sp) {
			return PAIR_SELECTED_SP;
		} else if ((addr >= proc.mb1a) && (addr < (proc.mb1a + proc.mb1s))) {
			return PAIR_MEM_BLOCK_1;
		} else if ((addr >= proc.mb2a) && (addr < (proc.mb2a + proc.mb2s))) {
			return PAIR_MEM_BLOCK_2;
		}
	}

	if (sm_isMemBlockStartAt(addr)) {
		return PAIR_MEM_BLOCK_START;
	} else if (sm_isAllocatedAt(addr)) {
		return PAIR_ALLOC_CELL;
	} else {
		return PAIR_FREE_CELL;
	}
}

static void
printWorld(void)
{
	sword y;

	for (y = 0; y < (sword)LINES; y++) {
		sword x;

		for (x = 0; x < g_worldLineWidth; x++) {
			sword addr  = g_worldPos + (((y * g_worldLineWidth) + x) * g_worldZoom);
			sbool atEnd = !sm_isValidAt(addr);
			int   xpos  = DATA_WIDTH + x;

			if (atEnd) {
				mvaddch(y, xpos, ' ');
				continue;
			}

			if (g_worldZoom == 1) {
				char symbol = g_instSymbols[sm_getInstAt(addr)];
				attron(COLOR_PAIR(getColorOf(addr)));
				mvaddch(y, xpos, symbol);
			} else {
				sword offset;
				char  symbol;
				sword instSum = 0;
				int   color   = PAIR_FREE_CELL;

				for (offset = 0; offset < g_worldZoom; offset++) {
					int   testColor;
					sword offsetAddr = addr + offset;

					if (!sm_isValidAt(offsetAddr)) {
						break;
					}

					instSum  += sm_getInstAt(offsetAddr);
					testColor = getColorOf(offsetAddr);

					if (testColor > color) {
						color = testColor;
					}
				}

				instSum /= g_worldZoom;

				if (!instSum) {
					symbol = ' ';
				} else if (instSum < 32) {
					symbol = '-';
				} else {
					symbol = '=';
				}

				attron(COLOR_PAIR(color));
				mvaddch(y, xpos, symbol);
			}

			standend();
		}
	}
}

static void
printWorldPage(int *line)
{
	int    eidx;
	SProc  proc = sp_getProc(g_selectedProcess);
	sword *data = (sword *)&proc;
	PHEADER("WORLD");
	PWIDGET("pos",      g_worldPos);
	PWIDGET("zoom",     g_worldZoom);
	PWIDGET("selected", g_selectedProcess);
	INCREMENT_LINE;
	PHEADER("SELECTED");

	if (!sp_isFree(g_selectedProcess)) {
		attron(COLOR_PAIR(PAIR_SELECTED_PROC));
	}

	for (eidx = 0; eidx < PROC_ELEMENT_COUNT; eidx++) {
		PWIDGET(g_procElems[eidx], data[eidx]);
	}

	standend();
	printWorld();
}

void
tsp_printData(void)
{
	int  linev = 1;
	int *line  = &linev;
	PHEADER("SALIS");
	PSIDGET("name",  g_simName);
	PSIDGET("state", g_running ? "running" : "paused");

	if (g_autoSaveInterval) {
		PWIDGET("autosave", g_autoSaveInterval);
	} else {
		PSIDGET("autosave", "---");
	}

	PWIDGET("cycle", s_getCycle());
	PWIDGET("epoch", s_getEpoch());
	INCREMENT_LINE;

	switch (g_currentPage) {
	case PAGE_MEMORY:
		printMemoryPage(line);
		break;

	case PAGE_EVOLVER:
		printEvolverPage(line);
		break;

	case PAGE_PROCESS:
		printProcessPage(line);
		break;

	case PAGE_WORLD:
		printWorldPage(line);
		break;
	}

	refresh();
}

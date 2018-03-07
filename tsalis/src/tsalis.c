#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <curses.h>
#include <salis.h>
#include "printer.h"
#include "handler.h"
#include "tsalis.h"

#define DEFAULT_ORDER 16

sbool g_exit;
sbool g_running;
sword g_autoSaveInterval;
char  g_simName[NAME_MAX_SIZE + 1] = "def.sim";

static void
onDefault(void)
{
	FILE *testFile = fopen(g_simName, "r");

	if (testFile) {
		fclose(testFile);
		s_load(g_simName);
	} else {
		s_init(DEFAULT_ORDER);
	}
}

static void
onLoad(const char *fileName)
{
	FILE *testFile;

	if (strlen(fileName) > NAME_MAX_SIZE) {
		fputs("ERROR: File name too long", stderr);
		exit(1);
	}

	strncpy(g_simName, fileName, NAME_MAX_SIZE);
	testFile = fopen(g_simName, "r");

	if (testFile) {
		fclose(testFile);
		s_load(g_simName);
	} else {
		fputs("ERROR: File does not exist", stderr);
		exit(1);
	}
}

static void
init(int argc, char **argv)
{
	if (argc == 1) {
		onDefault();
	} else if (argc == 2) {
		char  cmd = argv[1][0];
		char *val = &argv[1][1];

		if (cmd == 'n') {
			s_init(atoi(val));
		} else if (cmd == 'l') {
			onLoad(val);
		} else {
			fputs("ERROR: Incorrect arguments", stderr);
			exit(1);
		}
	} else {
		fputs("ERROR: Incorrect argument count", stderr);
		exit(1);
	}

	tsp_init();
}

static void
exec(void)
{
	while (!g_exit) {
		if (g_running) {
			clock_t beg = clock();
			clock_t end;
			float   delay;

			do {
				s_cycle();

				if (g_autoSaveInterval && !(s_getCycle() % g_autoSaveInterval)) {
					s_save(g_simName);
				}

				end   = clock();
				delay = (float)(end - beg) / CLOCKS_PER_SEC;
			} while (delay < (1.0 / 60.0));
		}

		tsp_printData();
		tsh_handleEvent(getch());
	}
}

static void
quit(void)
{
	tsp_quit();
	s_save(g_simName);
	s_quit();
}

int
main(int argc, char **argv)
{
	init(argc, argv);
	exec();
	quit();
	return 0;
}

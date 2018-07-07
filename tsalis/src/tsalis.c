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
		fputs("ERROR: File name too long\n", stderr);
		exit(1);
	}

	strncpy(g_simName, fileName, NAME_MAX_SIZE);
	testFile = fopen(g_simName, "r");

	if (testFile) {
		fclose(testFile);
		s_load(g_simName);
	} else {
		fputs("ERROR: File does not exist\n", stderr);
		exit(1);
	}
}

static void
init(int argc, char **argv)
{
	if (!tsp_check()) {
		fputs("ERROR: Terminal not supported\n", stderr);
		exit(1);
	}

	if (argc == 1) {
		onDefault();
	} else if (argc == 3) {
		if (!strcmp(argv[1], "-n") || !strcmp(argv[1], "--new")) {
			s_init(atoi(argv[2]));
		} else if (!strcmp(argv[1], "-l") || !strcmp(argv[1], "--load")) {
			onLoad(argv[2]);
		} else {
			fputs("ERROR: Incorrect arguments\n", stderr);
			exit(1);
		}
	} else {
		fputs("ERROR: Incorrect argument count\n", stderr);
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
					char extendedName[NAME_MAX_SIZE + 28];
					sprintf(extendedName, "%s.%010u.%010u.auto", g_simName, s_getEpoch(), s_getCycle());
					s_save(extendedName);
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

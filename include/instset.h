#ifndef SALIS_INSTSET_H
#define SALIS_INSTSET_H

#define SINST_COUNT 64

#define SINST_LIST \
	SINST(SNOOP, ' ') \
	SINST(SNOP0, '0') \
	SINST(SNOP1, '1') \
	SINST(SNOP2, '2') \
	SINST(SNOP3, '3') \
	\
	SINST(SJMPB, '(') \
	SINST(SJMPF, ')') \
	SINST(SADRB, '[') \
	SINST(SADRF, ']') \
	SINST(SIFNZ, '?') \
	\
	SINST(SALLB, '{') \
	SINST(SALLF, '}') \
	SINST(SBSWP, '%') \
	SINST(SBCLR, '|') \
	SINST(SSPLT, '$') \
	\
	SINST(SADDN, '+') \
	SINST(SSUBN, '-') \
	SINST(SMULN, '*') \
	SINST(SDIVN, '/') \
	SINST(SINCN, '^') \
	SINST(SDECN, 'v') \
	SINST(SNOTN, '!') \
	SINST(SSHFL, '<') \
	SINST(SSHFR, '>') \
	SINST(SZERO, 'z') \
	SINST(SUNIT, 'u') \
	\
	SINST(SPSHN, '#') \
	SINST(SPOPN, '~') \
	\
	SINST(SLOAD, '.') \
	SINST(SWRTE, ':') \
	SINST(SDUPL, '"') \
	SINST(SSWAP, 'x') \
	\
	SINST(SKEYA, 'a') \
	SINST(SKEYB, 'b') \
	SINST(SKEYC, 'c') \
	SINST(SKEYD, 'd') \
	SINST(SKEYE, 'e') \
	SINST(SKEYF, 'f') \
	SINST(SKEYG, 'g') \
	SINST(SKEYH, 'h') \
	SINST(SKEYI, 'i') \
	SINST(SKEYJ, 'j') \
	SINST(SKEYK, 'k') \
	SINST(SKEYL, 'l') \
	SINST(SKEYM, 'm') \
	SINST(SKEYN, 'n') \
	SINST(SKEYO, 'o') \
	SINST(SKEYP, 'p') \
	\
	SINST(SLOKA, 'A') \
	SINST(SLOKB, 'B') \
	SINST(SLOKC, 'C') \
	SINST(SLOKD, 'D') \
	SINST(SLOKE, 'E') \
	SINST(SLOKF, 'F') \
	SINST(SLOKG, 'G') \
	SINST(SLOKH, 'H') \
	SINST(SLOKI, 'I') \
	SINST(SLOKJ, 'J') \
	SINST(SLOKK, 'K') \
	SINST(SLOKL, 'L') \
	SINST(SLOKM, 'M') \
	SINST(SLOKN, 'N') \
	SINST(SLOKO, 'O') \
	SILST(SLOKP, 'P')

enum sinst {
#define SINST(name, symb) name,
#define SILST(name, symb) name
	SINST_LIST
#undef SINST
#undef SILST
};

sbool si_isInst       (sbyte inst);
sbool si_isMod        (sbyte inst);
sbool si_isKey        (sbyte inst);
sbool si_isLock       (sbyte inst);
sbool si_keyLockMatch (sbyte key, sbyte lock);

#endif

#ifndef TSALIS_PRINTER_H
#define TSALIS_PRINTER_H

extern const int PROC_ELEMENT_COUNT;

extern int   g_currentPage;
extern sword g_selectedProcess;
extern sbool g_processShowGenes;
extern sword g_processVertScroll;
extern sword g_processDataScroll;
extern sword g_processGeneScroll;
extern sword g_worldPos;
extern sword g_worldZoom;

sbool tsp_check            (void);
void  tsp_init             (void);
void  tsp_quit             (void);
void  tsp_onResize         (void);
void  tsp_prevPage         (void);
void  tsp_nextPage         (void);
void  tsp_scrollUp         (void);
void  tsp_scrollDown       (void);
void  tsp_fastScrollUp     (void);
void  tsp_fastScrollDown   (void);
void  tsp_scrollLeft       (void);
void  tsp_scrollRight      (void);
void  tsp_scrollToTop      (void);
void  tsp_scrollToLeft     (void);
void  tsp_zoomIn           (void);
void  tsp_zoomOut          (void);
void  tsp_prevOrganism     (void);
void  tsp_nextOrganism     (void);
void  tsp_gotoSelectedProc (void);
void  tsp_selectProcess    (sword proc);
void  tsp_moveTo           (sword loc);
void  tsp_printData        (void);

#endif

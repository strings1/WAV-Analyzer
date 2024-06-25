/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  ACQ_PANEL                        1       /* callback function: onAcqPanel */
#define  ACQ_PANEL_SWITCH_PANNELS         2       /* control type: command, callback function: onSwitchPanels */
#define  ACQ_PANEL_DOWNSAMPLED_GRAPH      3       /* control type: graph, callback function: (none) */
#define  ACQ_PANEL_DSAMPLED_GRAPH_FILTER  4       /* control type: graph, callback function: (none) */
#define  ACQ_PANEL_SPECTRU_GRAPH_FILTERE  5       /* control type: graph, callback function: (none) */
#define  ACQ_PANEL_SPECTRU_GRAPH          6       /* control type: graph, callback function: (none) */
#define  ACQ_PANEL_DECORATION             7       /* control type: deco, callback function: (none) */
#define  ACQ_PANEL_SAMPLES_RING_FRQ       8       /* control type: slide, callback function: OnRingSamples_Freq */
#define  ACQ_PANEL_START_BUTTON_FR        9       /* control type: pictButton, callback function: OnStartTimer_Freq */
#define  ACQ_PANEL_TESTLED                10      /* control type: LED, callback function: (none) */
#define  ACQ_PANEL_FREQ_PANEL_UPD_TIMER   11      /* control type: timer, callback function: OnTimer_freq */
#define  ACQ_PANEL_NUMERIC_TIMER_SPEED    12      /* control type: numeric, callback function: OnNumericTimerSpeed */
#define  ACQ_PANEL_FILTER_RING            13      /* control type: ring, callback function: onFilterRing */
#define  ACQ_PANEL_RING                   14      /* control type: ring, callback function: (none) */
#define  ACQ_PANEL_SNAP_GRPH_BUTTON_FI_2  15      /* control type: pictButton, callback function: onSnapGraphFilteredFreqButton */
#define  ACQ_PANEL_SNAP_SPEC_BUTTON_FILT  16      /* control type: pictButton, callback function: onSnapSpecFilteredButton */
#define  ACQ_PANEL_FILENAME_GRPH_STRIN_3  17      /* control type: string, callback function: (none) */
#define  ACQ_PANEL_FILENAME_SPEC_STRIN_2  18      /* control type: string, callback function: (none) */
#define  ACQ_PANEL_SNAP_SPEC_BUTTON       19      /* control type: pictButton, callback function: onSnapSpecButton */
#define  ACQ_PANEL_FILENAME_SPEC_STRING   20      /* control type: string, callback function: (none) */
#define  ACQ_PANEL_TEXTMSG                21      /* control type: textMsg, callback function: (none) */
#define  ACQ_PANEL_LABEL_GRAPH_SP_DS      22      /* control type: textMsg, callback function: (none) */
#define  ACQ_PANEL_LABEL_GRAPH_DS         23      /* control type: textMsg, callback function: (none) */
#define  ACQ_PANEL_LABEL_GRAPH_SP_DS_2    24      /* control type: textMsg, callback function: (none) */
#define  ACQ_PANEL_LABEL_GRAPH_SP_DS_3    25      /* control type: textMsg, callback function: (none) */
#define  ACQ_PANEL_DECORATION_2           26      /* control type: deco, callback function: (none) */

#define  MAIN_PANEL                       2       /* callback function: OnMainPanel */
#define  MAIN_PANEL_FILTERED_GRAPH        2       /* control type: graph, callback function: (none) */
#define  MAIN_PANEL_RAW_GRAPH             3       /* control type: graph, callback function: (none) */
#define  MAIN_PANEL_NEXT_BUTTON           4       /* control type: command, callback function: OnNextButton */
#define  MAIN_PANEL_PREV_BUTTON           5       /* control type: command, callback function: OnPrevButton */
#define  MAIN_PANEL_LOAD_BUTTON           6       /* control type: command, callback function: OnLoadButton */
#define  MAIN_PANEL_FILTER_RING           7       /* control type: ring, callback function: OnFilterRing */
#define  MAIN_PANEL_LOAD_FILTER_BUTTON    8       /* control type: command, callback function: OnLoadFilter */
#define  MAIN_PANEL_HISTOGRAM_RAW_DATA    9       /* control type: graph, callback function: (none) */
#define  MAIN_PANEL_THR0_NUMERIC          10      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_MEDIAN_NUMERIC        11      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_MEAN_NUMERIC          12      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_KURTOSIS_NUMERIC      13      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_SKEWENESS_NUMERIC     14      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_STD_DEV_NUMERIC       15      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_MAX_IDX_NUMERIC       16      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_MIN_IDX_NUMERIC       17      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_MIN_NUMERIC           18      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_MAX_NUMERIC           19      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_ALPHA_SLIDE           20      /* control type: scale, callback function: (none) */
#define  MAIN_PANEL_SNAP_RAW_BUTTON       21      /* control type: pictButton, callback function: onSnapRawButton */
#define  MAIN_PANEL_SNAP_FILTER_BUTTON    22      /* control type: pictButton, callback function: onSnapFilterButton */
#define  MAIN_PANEL_DECORATION            23      /* control type: deco, callback function: (none) */
#define  MAIN_PANEL_DECORATION_2          24      /* control type: deco, callback function: (none) */
#define  MAIN_PANEL_RESET_VIEW_BUTTON     25      /* control type: command, callback function: OnResetView */
#define  MAIN_PANEL_START_NUMERIC         26      /* control type: numeric, callback function: onStartSelection */
#define  MAIN_PANEL_ENVELOPE_CHECKBOX     27      /* control type: radioButton, callback function: onShowEnvelope */
#define  MAIN_PANEL_SWITCH_PANNELS        28      /* control type: command, callback function: onSwitchPanels */
#define  MAIN_PANEL_DECORATION_3          29      /* control type: deco, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK onAcqPanel(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnFilterRing(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnLoadButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnLoadFilter(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnMainPanel(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnNextButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnNumericTimerSpeed(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPrevButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnResetView(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnRingSamples_Freq(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK onShowEnvelope(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK onSnapFilterButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK onSnapGraphFilteredFreqButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK onSnapRawButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK onSnapSpecButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK onSnapSpecFilteredButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK onStartSelection(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnStartTimer_Freq(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK onSwitchPanels(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnTimer_freq(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
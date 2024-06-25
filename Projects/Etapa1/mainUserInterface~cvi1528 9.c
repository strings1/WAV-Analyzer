#include <cvirte.h>		
#include <userint.h>
#include <formatio.h>
#include <ansi_c.h>
#include <cvirte.h>
#include <userint.h>
#include <advanlys.h>
#include <utility.h>
#include "mainUserInterface.h"
#include "functions.h"
#include "toolbox.h"
//==============================================================================
// DEFINES AND MACROS
#define SAMPLE_RATE		(0)
#define NPOINTS			(1)
#define FPASS 			(800)
#define FSTOP			(1000)
#define DEFAULT_LED_VAL_START 1 //1 or 0
#define DOWNSAMPLED_ITEMS	  (8)
#define MAX_numberOfSamples_FREQ (4096)
#define NUMBER_OF_COEFICIENTS (251)
double* spectrumArray = 0;
double* downsampledArray = 0;
size_t	downsampledArray_size = 0;
double* ChebyshevFilterArray = 0;
double* EquiRplFilterArray = 0;
double deltaEq;
double EquiRplCOEFS[NUMBER_OF_COEFICIENTS];
const char* FILE_PATH ="../Resources/";
double* temp_dsAfilt = 0;
//med7 = sum 0 - 7 / 8
//med8 = med7-b0+b8/8


//==============================================================================
// Global variables
int waveInfo[2]; //waveInfo[0] = sampleRate
				 //waveInfo[1] = number of elements
int			val = DEFAULT_LED_VAL_START;
WindowConst winConst;
WindowConst winConst_Filtered;
Envelope 	envelope;
boolean 	zoomedIN 				= FALSE;
double		sampleRate 				= 0.0;
int 		npoints 				= 0;
uint16_t	Number_Of_Samples 		= 0; //Later in code will be npoints/sr::[0-1][1-2][2-3][3-4][4-5][5-6]
double 		*waveData 				= 0;
int 		Histogram_Data[MAX_VALUES_FOR_HISTOGRAM];
double 		axis[MAX_VALUES_FOR_HISTOGRAM];
static 		uint16_t 	start_second_sampled 	= 0;
static int  mainPanel;
static int  acqPanel;
static int  numberOfSamples_FREQ = 1024;
double*		autoSpectrum = (void*)0;
double*		autoSpectrum_Filtered = (void*)0;
int			freqStartOffset = 0;
double      freqPeak=0;
double      powerPeak=0;
int main (int argc, char *argv[])
{
	int error = 0;
	
	/* initialize and load resources */
	nullChk (InitCVIRTE (0, argv, 0));
	errChk (mainPanel = LoadPanel (0, "mainUserinterface.uir", MAIN_PANEL));
	errChk (acqPanel = LoadPanel (0, "mainUserinterface.uir", ACQ_PANEL));
	
	/* display the panel and run the user interface */
	errChk (DisplayPanel (mainPanel));
	errChk (RunUserInterface ());

Error:
	/* clean up */
	if (mainPanel > 0)
		DiscardPanel (mainPanel);
	if(waveData)
		free(waveData);
	if(downsampledArray)
		free(downsampledArray);
	if(spectrumArray)
		free(spectrumArray);
	if(ChebyshevFilterArray)
		free(ChebyshevFilterArray);
	if(EquiRplFilterArray)
		free(EquiRplFilterArray);
	if(envelope.data)	 
		free(envelope.data);
	return 0;
}

int CVICALLBACK OnMainPanel (int panel, int event, void *callbackData,	 int eventData1, int eventData2)
{
	int mainWindow_ret_val;
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			mainWindow_ret_val = QuitUserInterface (0);
			

			break;
	}
	return 0;
}

int CVICALLBACK OnLoadButton (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			//executa script python pentru conversia unui fisierului .wav in .txt
			//LaunchExecutable("python main.py");
			
			//astept sa fie generate cele doua fisiere (modificati timpul daca este necesar
			//Delay(4);
			
			//incarc informatiile privind rata de esantionare si numarul de valori
			FileToArray("waveInfo.txt", waveInfo, VAL_INTEGER, 2, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
			sampleRate = waveInfo[SAMPLE_RATE];
			npoints = waveInfo[NPOINTS];
			Number_Of_Samples = ceil(sampleRate/npoints); //Should be ~ 6
			
			//alocare memorie pentru numarul de puncte
			if(waveData)
				free(waveData);
			waveData = (double *) calloc(npoints, sizeof(double));
			SetCtrlAttribute(panel,MAIN_PANEL_SWITCH_PANNELS,ATTR_DIMMED,FALSE);
			//incarcare din fisierul .txt in memorie (vector)
			FileToArray("waveData.txt", waveData, VAL_DOUBLE, npoints, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
			
			//afisare pe grapf
			PlotY (panel, MAIN_PANEL_RAW_GRAPH, waveData, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
			int min_val;
			int max_val;
			unsigned int min_val_index;
			unsigned int max_val_index;
			double mean;
			double std_dev;
			double median_val;
			uint32_t throughZero;
			
			getInfo(waveData,npoints,&min_val,&max_val,&min_val_index, &max_val_index, &throughZero,&mean);
			std_dev = StdDev(waveData,npoints,&mean,&std_dev);
			
			if(npoints&0x0001)
			{
				median_val = waveData[npoints/2];
			}
			else
			{
				median_val = waveData[npoints/2] + waveData[npoints/2 - 1];
			}
			
			double skeweness, kurtosis;
			
			Moment(waveData,SKEWENESS_SAMPLE_SIZE,THIRD_ORDER,&skeweness);
			Moment(waveData,KURTOSIS_SAMPLE_SIZE,FOURTH_ORDER,&kurtosis);
			
			SetCtrlVal(panel,MAIN_PANEL_MIN_NUMERIC,min_val);
			SetCtrlVal(panel,MAIN_PANEL_MAX_NUMERIC,max_val);
			SetCtrlVal(panel,MAIN_PANEL_THR0_NUMERIC,throughZero);
			SetCtrlVal(panel,MAIN_PANEL_MIN_IDX_NUMERIC,min_val_index);
			SetCtrlVal(panel,MAIN_PANEL_MAX_IDX_NUMERIC,max_val_index);
			SetCtrlVal(panel,MAIN_PANEL_MEAN_NUMERIC,mean);
			SetCtrlVal(panel,MAIN_PANEL_STD_DEV_NUMERIC,std_dev);
			SetCtrlVal(panel,MAIN_PANEL_MEDIAN_NUMERIC,median_val);
			SetCtrlVal(panel,MAIN_PANEL_KURTOSIS_NUMERIC,kurtosis);
			SetCtrlVal(panel,MAIN_PANEL_SKEWENESS_NUMERIC,skeweness);
			
			Histogram(waveData,npoints,(double)min_val,(double)(max_val),Histogram_Data,axis,10);
			PlotXY(panel,MAIN_PANEL_HISTOGRAM_RAW_DATA,axis,Histogram_Data,10,VAL_DOUBLE,VAL_INTEGER,VAL_VERTICAL_BAR,VAL_EMPTY_SQUARE,VAL_SOLID,1,VAL_RED);
			SetCtrlAttribute (panel, MAIN_PANEL_ENVELOPE_CHECKBOX, ATTR_DIMMED, 0);
			
			envelope = CalculateEnvelope(waveData,npoints);
			downsampledArray=GetDownSampled(waveData,npoints,DOWNSAMPLED_ITEMS,downsampledArray,&downsampledArray_size);
			
			if( downsampledArray == 0)
			{
				QuitUserInterface(-1);
			}
			
			ChebyshevFilterArray = (double*)malloc(sizeof(double) * downsampledArray_size);
			EquiRplFilterArray = (double*)malloc(sizeof(double) * (downsampledArray_size+NUMBER_OF_COEFICIENTS-1));
			for(size_t i=0;i<downsampledArray_size+NUMBER_OF_COEFICIENTS-1;i++)
			{
				if(i<downsampledArray_size)
					ChebyshevFilterArray[i]	=	0;
				EquiRplFilterArray[i] 	=	0;
			}
			/* Chebyshev I*/
			Ch_HPF(downsampledArray,downsampledArray_size,sampleRate,1600,0.5,1,ChebyshevFilterArray);
			
			/* FIR EquiRpl High Pass */
			EquiRpl_HPF(sampleRate,FPASS,FSTOP,NUMBER_OF_COEFICIENTS,EquiRplCOEFS,&deltaEq);
			Convolve (EquiRplCOEFS, NUMBER_OF_COEFICIENTS, downsampledArray, downsampledArray_size, EquiRplFilterArray);
			break;
	}

	return 0;
}

int CVICALLBACK OnNextButton (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(waveData)
			{
				zoomedIN=TRUE;
				start_second_sampled+=1;
				start_second_sampled%=(int)(npoints/sampleRate);
				SetCtrlVal(panel,MAIN_PANEL_START_NUMERIC,start_second_sampled);
				DeleteGraphPlot (panel, MAIN_PANEL_RAW_GRAPH, -1, VAL_IMMEDIATE_DRAW);
				DeleteGraphPlot (panel, MAIN_PANEL_FILTERED_GRAPH, -1, VAL_IMMEDIATE_DRAW);
				double* result = (double*)0;
				
				//alocare memorie pentru numarul de puncte
				uint8_t value;
				double alpha;
				GetCtrlVal(panel, MAIN_PANEL_FILTER_RING, &value);
				switch(value)
				{
					case FILTER_NONE:
						break;
					case FILTER_MEDIATE:
						if(waveData != (void*) 0 )
						{
							result =(double*)mediate(waveData,npoints,SAMPLES_TO_MEDIATE);
						}
						else return 0;
						break;
					case FILTER_ALPHA:
						if(waveData != (void*) 0)
						{
							GetCtrlVal(panel, MAIN_PANEL_ALPHA_SLIDE, &alpha);
							result = (double*)alphaFilter(waveData+(int)(sampleRate*start_second_sampled),sampleRate, alpha);
						}
						break;
						
					default:
						break;
						
				}
				PlotY (panel, MAIN_PANEL_RAW_GRAPH, waveData+(int)(sampleRate*start_second_sampled), sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
				if(result)
					PlotY (panel, MAIN_PANEL_FILTERED_GRAPH,  result+((int)(sampleRate*start_second_sampled)*(FILTER_MEDIATE == value)), sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
				SetCtrlAttribute(panel,MAIN_PANEL_RAW_GRAPH,ATTR_XAXIS_OFFSET,sampleRate*start_second_sampled); //PRINT POINTS = sr-(sr-totalpoints)
				boolean AnvelopeWasActive = FALSE;
				GetCtrlVal(panel,MAIN_PANEL_ENVELOPE_CHECKBOX,&AnvelopeWasActive);
				if(envelope.data && AnvelopeWasActive)
					PlotY (panel, MAIN_PANEL_RAW_GRAPH, envelope.data+(int)(sampleRate*start_second_sampled), sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_CYAN);
				
			}
			break;
	}
	return 0;
}

int CVICALLBACK OnPrevButton (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(waveData)
			{
				zoomedIN=TRUE;
				start_second_sampled-=1;
				if(start_second_sampled>=(int)(npoints/sampleRate))
					start_second_sampled=5;
				double* result = (double*)0;
				
				//alocare memorie pentru numarul de puncte
				uint8_t value;
				double alpha;
				GetCtrlVal(panel, MAIN_PANEL_FILTER_RING, &value);
				switch(value)
				{
					case FILTER_NONE:
						break;
					case FILTER_MEDIATE:
						if(waveData != (void*) 0 )
						{
							result =(double*)mediate(waveData,npoints,SAMPLES_TO_MEDIATE);
						}
						else return 0;
						break;
					case FILTER_ALPHA:
						if(waveData != (void*) 0)
						{
							GetCtrlVal(panel, MAIN_PANEL_ALPHA_SLIDE, &alpha);
							result = (double*)alphaFilter(waveData+(int)(sampleRate*start_second_sampled),sampleRate, alpha);
						}
						break;
						
					default:
						break;
						
				}
				DeleteGraphPlot (panel, MAIN_PANEL_FILTERED_GRAPH, -1, VAL_IMMEDIATE_DRAW);
				SetCtrlVal(panel,MAIN_PANEL_START_NUMERIC,start_second_sampled);
				DeleteGraphPlot (panel, MAIN_PANEL_RAW_GRAPH, -1, VAL_IMMEDIATE_DRAW);
				
				if(result)
					PlotY (panel, MAIN_PANEL_FILTERED_GRAPH, result+((int)(sampleRate*start_second_sampled)*(FILTER_MEDIATE == value)), 
						   sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
				
				PlotY (panel, MAIN_PANEL_RAW_GRAPH, waveData+(int)(sampleRate*start_second_sampled), sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
				SetCtrlAttribute(panel,MAIN_PANEL_RAW_GRAPH,ATTR_XAXIS_OFFSET,sampleRate*start_second_sampled);
				boolean AnvelopeWasActive = FALSE;
				GetCtrlVal(panel,MAIN_PANEL_ENVELOPE_CHECKBOX,&AnvelopeWasActive);
				if(envelope.data && AnvelopeWasActive)
					PlotY (panel, MAIN_PANEL_RAW_GRAPH, envelope.data+(int)(sampleRate*start_second_sampled), sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_CYAN);
				
			}break;
	}
	return 0;
}

int CVICALLBACK OnFilterRing (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(waveData != (void*)0)
			{
			double* result = (double*)0;
			//alocare memorie pentru numarul de puncte
			uint8_t value;
			double alpha;
			GetCtrlVal(panel, MAIN_PANEL_FILTER_RING, &value);
			switch(value)
			{
				case FILTER_NONE:
					return 0;
				case FILTER_MEDIATE:
					if(waveData != (void*) 0 )
					{
						result =(double*)mediate(waveData,npoints,SAMPLES_TO_MEDIATE);
						fillBuffer(waveData,1,1);
					}
					else return 0;
					break;
				case FILTER_ALPHA:
					if(waveData != (void*) 0)
					{
						GetCtrlVal(panel, MAIN_PANEL_ALPHA_SLIDE, &alpha);
						result = (double*)alphaFilter(waveData,npoints, alpha);
					}
					break;
					
				default:
					return 0;
					
			}
			
			//afisare pe grapf
			DeleteGraphPlot (panel, MAIN_PANEL_FILTERED_GRAPH, -1, VAL_IMMEDIATE_DRAW);
			PlotY (panel, MAIN_PANEL_FILTERED_GRAPH, result, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
			free(result);
			}
			break;
	}
	return 0;
}

int CVICALLBACK OnLoadFilter (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	
	double* result = (double*)0;
	switch (event)
	{
		case EVENT_COMMIT:
			
			//alocare memorie pentru numarul de puncte
			uint8_t value;
			double alpha;
			GetCtrlVal(panel, MAIN_PANEL_FILTER_RING, &value);
			switch(value)
			{
				case FILTER_NONE:
					return 0;
				case FILTER_MEDIATE:
					if(waveData != (void*) 0 )
					{
						result =(double*)mediate(waveData,npoints,SAMPLES_TO_MEDIATE);
					}
					else return 0;
					break;
				case FILTER_ALPHA:
					if(waveData != (void*) 0)
					{
						GetCtrlVal(panel, MAIN_PANEL_ALPHA_SLIDE, &alpha);
						result = (double*)alphaFilter(waveData,npoints, alpha);
					}							
					break;
				default:
					return 0;
					
			}
			
			//afisare pe grapf
			DeleteGraphPlot (panel, MAIN_PANEL_FILTERED_GRAPH, -1, VAL_IMMEDIATE_DRAW);
			PlotY (panel, MAIN_PANEL_FILTERED_GRAPH, result, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
			

			break;
	}
	free(result);
	return 0;
}

int CVICALLBACK onSnapFilterButton (int panel, int control, int event,
									void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
			int bitmapID;
		case EVENT_COMMIT:
			char fileName[64];
			sprintf(fileName, "../Resources/grafic_filter.jpg"); //generare nume fisier
			GetCtrlDisplayBitmap(panel, MAIN_PANEL_FILTERED_GRAPH, 1, &bitmapID);
			SaveBitmapToJPEGFile(bitmapID, fileName, JPEG_PROGRESSIVE, 100);
			break;
	}
	return 0;
}

int CVICALLBACK onSnapRawButton (int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		int bitmapID;
		case EVENT_COMMIT:
			char fileName[64];
			sprintf(fileName, "../Resources/grafic_raw.jpg"); //generare nume fisier
			GetCtrlDisplayBitmap(panel, MAIN_PANEL_RAW_GRAPH, 1, &bitmapID);
			SaveBitmapToJPEGFile(bitmapID, fileName, JPEG_PROGRESSIVE, 100);
			break;
	}
	return 0;
}

int CVICALLBACK OnResetView (int panel, int control, int event,
							 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(waveData)
			{
				zoomedIN=FALSE;
				start_second_sampled=0;
				SetCtrlVal(panel,MAIN_PANEL_START_NUMERIC,start_second_sampled);
				DeleteGraphPlot (panel, MAIN_PANEL_RAW_GRAPH, -1, VAL_IMMEDIATE_DRAW);
				PlotY (panel, MAIN_PANEL_RAW_GRAPH, waveData+(int)(sampleRate*start_second_sampled), npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
								SetCtrlAttribute(panel,MAIN_PANEL_RAW_GRAPH,ATTR_XAXIS_OFFSET,sampleRate*start_second_sampled); //PRINT POINTS = sr-(sr-totalpoints)
				boolean AnvelopeWasActive = FALSE;
				GetCtrlVal(panel,MAIN_PANEL_ENVELOPE_CHECKBOX,&AnvelopeWasActive);
				if(AnvelopeWasActive)
				{
					PlotY (panel, MAIN_PANEL_RAW_GRAPH, envelope.data, envelope.length, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_CYAN);
				}
			}break;
	}
	return 0;
}

int CVICALLBACK onStartSelection (int panel, int control, int event,
								  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(waveData)
			{
				GetCtrlVal(panel,MAIN_PANEL_START_NUMERIC,&start_second_sampled);
				if(start_second_sampled>=(int)(npoints/sampleRate))
				{
					start_second_sampled%=(int)(npoints/sampleRate);
					SetCtrlVal(panel,MAIN_PANEL_START_NUMERIC,start_second_sampled);
				}
				DeleteGraphPlot (panel, MAIN_PANEL_RAW_GRAPH, -1, VAL_IMMEDIATE_DRAW);
				PlotY (panel, MAIN_PANEL_RAW_GRAPH, waveData+(int)(sampleRate*start_second_sampled), sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
				SetCtrlAttribute(panel,MAIN_PANEL_RAW_GRAPH,ATTR_XAXIS_OFFSET,sampleRate*start_second_sampled); //PRINT POINTS = sr-(sr-totalpoints)
			}
			break;
	}
	return 0;
}
int plotEnvelope=0;
int CVICALLBACK onShowEnvelope (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(waveData)
			{
				int state;
				GetCtrlVal(panel,MAIN_PANEL_ENVELOPE_CHECKBOX,&state);
				if(state)
				{
					if(!zoomedIN)
					{
					plotEnvelope=PlotY (panel, MAIN_PANEL_RAW_GRAPH, envelope.data, envelope.length, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_CYAN);
					}
					else
					{
					plotEnvelope=PlotY (panel, MAIN_PANEL_RAW_GRAPH, envelope.data+(int)(sampleRate*start_second_sampled), sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_CYAN);
					}	
					
				}
				else
				{
					DeleteGraphPlot (panel, MAIN_PANEL_RAW_GRAPH,plotEnvelope , VAL_IMMEDIATE_DRAW);
					if(zoomedIN)
						PlotY (panel, MAIN_PANEL_RAW_GRAPH, waveData+(int)(sampleRate*start_second_sampled), sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
					else
						PlotY (panel, MAIN_PANEL_RAW_GRAPH, waveData, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
				}
				
			}
			break;
	}
	return 0;
}

int CVICALLBACK onSwitchPanels (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(panel == mainPanel)
			{
				//SetCtrlVal(acqPanel, MAIN_PANEL_SWITCH_TOGGLE, 1);
				DisplayPanel(acqPanel);
				HidePanel(panel);
			}
			else
			{
				//SetCtrlVal(mainPanel, MAIN_PANEL_SWITCH_TOGGLE, 0);
				DisplayPanel(mainPanel);
				HidePanel(panel);
			}
			break;

	}
	return 0;
}

int CVICALLBACK onAcqPanel (int panel, int event, void *callbackData,
							int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			QuitUserInterface(0);
			break;
	}
	return 0;
}

int CVICALLBACK OnStartTimer_Freq (int panel, int control, int event,
								   void *callbackData, int eventData1, int eventData2)
{
	int state;
	freqStartOffset = 0;
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal(panel,ACQ_PANEL_START_BUTTON_FR,&state);
			SetCtrlAttribute(panel,ACQ_PANEL_FREQ_PANEL_UPD_TIMER,ATTR_ENABLED,state);
			SetCtrlAttribute(panel,ACQ_PANEL_SAMPLES_RING_FRQ,ATTR_DIMMED,state);
			//Delete Graph?

			break;
	}
	return 0;
}

int CVICALLBACK OnTimer_freq (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
			
		case EVENT_TIMER_TICK:
			/* Led Blink */
			SetCtrlVal(panel,ACQ_PANEL_TESTLED,val);
			val++;
			val%=2;
			if(freqStartOffset > downsampledArray_size-numberOfSamples_FREQ-1)
				freqStartOffset = 0;
			int selectedWindow;
			int selected_filter; 
			GetCtrlVal(panel,ACQ_PANEL_RING,&selectedWindow);
			GetCtrlVal(panel,ACQ_PANEL_FILTER_RING,&selected_filter);
			/*-------------------------------------------------------------------------------------*/
			/*							  HANDLE datasample spectrum							   */
			/*-------------------------------------------------------------------------------------*/
			
			/* Spectrum Plot */
			DeleteGraphPlot(panel,ACQ_PANEL_SPECTRU_GRAPH,-1,VAL_DELAYED_DRAW);
			if(spectrumArray)
				free(spectrumArray);
			spectrumArray = (double*)malloc(numberOfSamples_FREQ * sizeof(double));
			

			
			/* copy to spectrum array the window from startOffset numberOfSamples_FREQ samples */
			
			memcpy(spectrumArray,downsampledArray+freqStartOffset,sizeof(double)*numberOfSamples_FREQ);
			
			
			/* apply scaledwindowex on the mentioned interval */

			ScaledWindowEx(spectrumArray,numberOfSamples_FREQ,RECTANGLE,0,&winConst);

			
			
			if( autoSpectrum == (void*)0 )
			{
				autoSpectrum=(double*)malloc(numberOfSamples_FREQ/2 * sizeof(double));
			}
			
			if (autoSpectrum_Filtered == (void*)0)
			{
				autoSpectrum_Filtered=(double*)malloc(numberOfSamples_FREQ/2 * sizeof(double));
			}
			double df=0;
			double df_filtered = 0;
			AutoPowerSpectrum(spectrumArray,numberOfSamples_FREQ,1.0/sampleRate,autoSpectrum,&df);
			PowerFrequencyEstimate(autoSpectrum,numberOfSamples_FREQ/2,-1,winConst,df,7,&freqPeak,&powerPeak);
			PlotWaveform (panel, ACQ_PANEL_SPECTRU_GRAPH, autoSpectrum, numberOfSamples_FREQ/2, VAL_DOUBLE, 
						  1.0, 0.0, 0.0, df, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_RED);

			/* move the interval to the next one */
			freqStartOffset += numberOfSamples_FREQ;
			if(freqStartOffset > downsampledArray_size-numberOfSamples_FREQ)
				freqStartOffset = 0;
			if(autoSpectrum)
			/*make sure it doesn't go beyond array's length*/
				free(autoSpectrum);
			autoSpectrum=0;



			/*-------------------------------------------------------------------------------------*/
			/*								HANDLE datasample array								   */
			/*-------------------------------------------------------------------------------------*/
			
			DeleteGraphPlot(panel,ACQ_PANEL_DOWNSAMPLED_GRAPH,-1,VAL_DELAYED_DRAW);
			PlotY (panel, ACQ_PANEL_DOWNSAMPLED_GRAPH, downsampledArray+freqStartOffset, numberOfSamples_FREQ, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
						

			/*-------------------------------------------------------------------------------------*/
			/*					 		HANDLE filtered datasample array						   */
			/*-------------------------------------------------------------------------------------*/

			
			DeleteGraphPlot(panel,ACQ_PANEL_DSAMPLED_GRAPH_FILTER,-1,VAL_DELAYED_DRAW);
			double* selectedFilterArray = 0;
			switch(selected_filter)
			{
				case 1:
					selectedFilterArray = ChebyshevFilterArray;
						break;
				case 2:
					selectedFilterArray	= EquiRplFilterArray;
					break;
				default:
					selectedFilterArray = ChebyshevFilterArray;
					break;
			}
			
			
			PlotY (panel, ACQ_PANEL_DSAMPLED_GRAPH_FILTER, selectedFilterArray+freqStartOffset, numberOfSamples_FREQ, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
				
			/*-------------------------------------------------------------------------------------*/
			/*					 		HANDLE filtered datasample array						   */
			/*-------------------------------------------------------------------------------------*/	
			
			if(temp_dsAfilt)
				free(temp_dsAfilt);
			temp_dsAfilt = (double*)malloc(numberOfSamples_FREQ * sizeof(double));
			if(temp_dsAfilt)
			{	
					DeleteGraphPlot(panel,ACQ_PANEL_SPECTRU_GRAPH_FILTERE,-1,VAL_DELAYED_DRAW);
					
					memcpy(temp_dsAfilt,selectedFilterArray+freqStartOffset,sizeof(double)*numberOfSamples_FREQ);
				switch (selectedWindow)
				{
					case 0:
						ScaledWindowEx(temp_dsAfilt,numberOfSamples_FREQ,BLKMAN,0,&winConst_Filtered);
						break;
					case 1:
						ScaledWindowEx(temp_dsAfilt,numberOfSamples_FREQ,BLKHARRIS,0,&winConst_Filtered);
						break;
					default:
						ScaledWindowEx(temp_dsAfilt,numberOfSamples_FREQ,BLKMAN,0,&winConst_Filtered);
						break;
				}
				AutoPowerSpectrum(temp_dsAfilt,numberOfSamples_FREQ,1.0/sampleRate,autoSpectrum_Filtered,&df_filtered);
				PowerFrequencyEstimate(autoSpectrum_Filtered,numberOfSamples_FREQ/2,-1,winConst_Filtered,df_filtered,7,&freqPeak,&powerPeak);
				PlotWaveform (panel, ACQ_PANEL_SPECTRU_GRAPH_FILTERE, autoSpectrum_Filtered, numberOfSamples_FREQ/2, VAL_DOUBLE, 
							  1.0, 0.0, 0.0, df_filtered, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_RED);
				if(autoSpectrum_Filtered)
					free(autoSpectrum_Filtered);
				autoSpectrum_Filtered=0;

			}

			
			break;
	}
	return 0;
}

int CVICALLBACK OnRingSamples_Freq (int panel, int control, int event,
									void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal(panel,ACQ_PANEL_SAMPLES_RING_FRQ,&numberOfSamples_FREQ);
			freqStartOffset=0;

			break;
	}
	return 0;
}

int CVICALLBACK OnNumericTimerSpeed (int panel, int control, int event,
									 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			double value=0;
			GetCtrlVal(panel,ACQ_PANEL_NUMERIC_TIMER_SPEED,&value);
			if(value < MIN_SPEED_TIMER)
				value = MIN_SPEED_TIMER, SetCtrlVal(panel,ACQ_PANEL_NUMERIC_TIMER_SPEED,value);
			SetCtrlAttribute(panel,ACQ_PANEL_FREQ_PANEL_UPD_TIMER,ATTR_INTERVAL,value);
			break;
	}
	return 0;
}

int CVICALLBACK onSnapSpecButton (int panel, int control, int event,
								  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			int bitmapID;
			char filePath[64];
			char fileName[64];
			
			GetCtrlVal(panel,ACQ_PANEL_FILENAME_SPEC_STRING,fileName);
			char ConcatenatedPath[128];
			strcpy(ConcatenatedPath,FILE_PATH);
			strcat(ConcatenatedPath,fileName);
			sprintf(filePath, ConcatenatedPath); //generare nume fisier
			GetCtrlDisplayBitmap(panel, MAIN_PANEL_RAW_GRAPH, 1, &bitmapID);
			SaveBitmapToJPEGFile(bitmapID, filePath, JPEG_PROGRESSIVE, 100);
			break;
	}
	return 0;
}

int CVICALLBACK onSnapSpecFilteredButton (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			int bitmapID;
			char filePath[64];
			char fileName[64];
			
			GetCtrlVal(panel,ACQ_PANEL_FILENAME_SPEC_STRIN_2,fileName);
			char ConcatenatedPath[128];
			strcpy(ConcatenatedPath,FILE_PATH);
			strcat(ConcatenatedPath,fileName);
			sprintf(filePath, ConcatenatedPath); //generare nume fisier
			GetCtrlDisplayBitmap(panel, MAIN_PANEL_RAW_GRAPH, 1, &bitmapID);
			SaveBitmapToJPEGFile(bitmapID, filePath, JPEG_PROGRESSIVE, 100);
			break;
	}
	return 0;
}

int CVICALLBACK onFilterRing (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
				freqStartOffset = 0;
				DeleteGraphPlot(panel,ACQ_PANEL_DSAMPLED_GRAPH_FILTER,-1,VAL_DELAYED_DRAW);
				DeleteGraphPlot(panel,ACQ_PANEL_SPECTRU_GRAPH,-1,VAL_DELAYED_DRAW);
			break;
	}
	return 0;
}

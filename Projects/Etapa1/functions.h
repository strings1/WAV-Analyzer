//==============================================================================
//
// Title:		functions.h
// Purpose:		A short description of the interface.
//
// Created on:	11/2/2023 at 11:26:20 AM by .
// Copyright:	. All Rights Reserved.
//
//==============================================================================

#ifndef __functions_H__
#define __functions_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"
#include <stdint.h>
//==============================================================================
// Constants

//==============================================================================
// Types n macros
#define GET_PARITY_8(x)  		 ((0x80 & x) != 0) //Return 0 if >=0 , 1 if < 0
#define SAMPLES_TO_MEDIATE 		 (16)
#define FILTER_NONE		  		 (0u)
#define FILTER_MEDIATE    		 (1u)
#define FILTER_ALPHA 			 (2u)
//#define FILTER_SKEWENESS     	 (3u)
//#define FILTER_KURTOSIS			 (4u)
#define MAX_VALUES_FOR_HISTOGRAM (100u)
#define ENVELOPE_WINDOW_SIZE	 (1000)
#define THIRD_ORDER				 (3u)
#define FOURTH_ORDER			 (4u)
#define SKEWENESS_SAMPLE_SIZE 	 (256)
#define KURTOSIS_SAMPLE_SIZE 	 (256)
typedef int boolean;
#define FALSE 					 (0u)
#define TRUE					 (1u)
#define MIN_SPEED_TIMER          (0.025)
		
typedef struct {
  double *data;
  int length;
} Envelope;

//==============================================================================
// External variables

//==============================================================================
// Global functions

void	 fillBuffer	(double* buffer, size_t number_of_values, int8_t value);
void*	 mediate	(double* buffer, size_t number_of_values,size_t sampleSize);
void	 getInfo	(double* buffer, size_t num_of_samples, int* min, int* max, unsigned int* minidx, unsigned int* maxidx, size_t* through_zero,double *median);
void*    alphaFilter(double* buffer, size_t number_of_values,double alpha);
void*	 GetDownSampled(double* buffer, size_t number_of_values, unsigned int downsampledItems, double* downSampledArray, unsigned int* downSampledArray_size);


Envelope CalculateEnvelope(double *signal, int length);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __functions_H__ */

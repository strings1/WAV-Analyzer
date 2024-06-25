//==============================================================================
//
// Title:		functions.c
// Purpose:		A short description of the implementation.
//
// Created on:	11/2/2023 at 11:26:44 AM by .
// Copyright:	. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

#include <ansi_c.h>
#include "functions.h"

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables

//==============================================================================
// Static functions

//==============================================================================
// Global variables

//==============================================================================
// Global functions


void fillBuffer(double* buffer, size_t number_of_values, int8_t value)
{
    if ((void*)0 == buffer)
    {
		return;
	}

    for (size_t i = 0; i < number_of_values; i++)
    {
		buffer[i] = value;
	}

    return;
}


void* mediate(double* buffer, size_t number_of_values,size_t sampleSize)
{
    double* result = (double*)malloc(number_of_values*sizeof(double));
    fillBuffer(result, number_of_values, 0);

    if ((void*)0 == buffer)
    {
		return (void*)0;
	}

    double sum = 0;

    if (sampleSize > number_of_values)
    {
        for (size_t i = 0; i < number_of_values; i++)
        {
			sum += buffer[i];
		}
        fillBuffer(result, number_of_values, sum / number_of_values);
        return result;
	}
	else // HERE STARTS
	{
        size_t i;
		for (i = 0; i < sampleSize; i++)
		{
            sum += buffer[i];                                        //Calculate the first sum
        }
        result[0]=sum/sampleSize;                                    //Store the first average
        for (i = 1; i < number_of_values - sampleSize + 1; i++)
        {
			sum=sum-buffer[i-1]+buffer[i+sampleSize-1]; //Calculate the sum of the next sample
            result[i]=sum/sampleSize;                                //Store the next average
		}
        return result;
    }
}

void getInfo(double* buffer, size_t num_of_samples,
			  int* min, int* max, unsigned int* minidx, unsigned int* maxidx, 
			  size_t* through_zero, double *mean) 
{
  // Initialize the variables.
  if(buffer == (void*)0)
  {
	  return; // ERROR
  }
  
  if (min!=(void*)0)
	*min = (int)buffer[0];

  if (min!=(void*)0)
	*max = (int)buffer[0];
  
  if (through_zero!=(void*)0)
    *through_zero = 0;
  
  if(minidx != (void*)0)
	  *minidx = 0;
  
  if(maxidx != (void*)0)
	  *maxidx=0;
  
  if(mean != (void*)0)
	  *mean = 0;
  
  // Iterate over the buffer and update the variables.
  for (int i = 1; i < num_of_samples; i++)
  {
	  if(mean!=(void*)0)
	    *mean += buffer[i]/num_of_samples;
	  
	    if (min!=(void*)0 && buffer[i] < *min) {
	      *min = (int)buffer[i];
		  if(minidx != (void*)0)
		  {
		    *minidx = i;
		  }
	    }

	    if (max!=(void*)0 &&buffer[i] > *max) {
	      *max = (int)buffer[i];
		  if(maxidx != (void*)0)
		  {
		    *maxidx = i;
		  }
	    }

	    if ( through_zero!=(void*)0 && 	/* if through zero exists	*/
			GET_PARITY_8((int)buffer[i]) != GET_PARITY_8((int)buffer[i - 1]) ) /* dif. parity */
	 	{
		    	*through_zero +=1;
	    }
  }
  mean=mean;
}

void* alphaFilter(double* buffer, size_t number_of_values,double alpha)
{
	double* alphaBuffer= (double*)malloc(number_of_values*sizeof(double));
	if(alphaBuffer == (void*)0)
	{
		return (void*)0;
	}
	alphaBuffer[0] = buffer[0];
	for(size_t i = 1; i<number_of_values; ++i)
	{
		alphaBuffer[i]=(1-alpha)*alphaBuffer[i-1]+alpha*buffer[i];
	}
	return alphaBuffer;
}

Envelope CalculateEnvelope(double *signal, int length) {
  Envelope envelope;								// create anvelope								*/
  envelope.data = malloc(length * sizeof(double));	// malloc space									*/
  envelope.length = length;							// set length									*/
  
  if(envelope.data == (void*)0)
  {
	  envelope.length = -1;							// check if length >0 when using this func		*/
	  return envelope;
  }

  /* ------------------------------------initialize the envelope----------------------------------- */
  for (int i = 0; i < length; i++) {
    envelope.data[i] = 0;				// set envelope's initial values 				*/
  }

  /* -------------------------Calculate the envelope using a sliding window------------------------ */
  int windowSize = ENVELOPE_WINDOW_SIZE;			// set a chunk 									*/
  double last_max = signal[0];
  double last_max_pos = 0;
  int k = 0;
  double ratio = 0;
  int actual_max_pos = 0;
  for (int i = 0; i < length - windowSize; i+= windowSize) {	// set boundaries 								*/
    double max = envelope.data[i];					// set initial max value as the first element   */
    for (int j = 0; j < windowSize; j++) {			// parse the window							    */
      if (fabs(signal[i + j]) > max) {				// find max (i+ offset > max )					*/
        max = fabs(signal[i + j]);					// update max value								*/
		actual_max_pos = i+j;
      }												//												*/
    }												//												*/
    envelope.data[actual_max_pos] = max;			// set it's final max found 					*/
	k = (actual_max_pos - last_max_pos + 1);
	ratio = (max-last_max)/k;
	for(int idx = actual_max_pos-1; idx>last_max_pos && idx<length ;--idx)
	{
		envelope.data[idx]=envelope.data[idx+1]-ratio;
	}
	last_max_pos=actual_max_pos;
	last_max=max;
	
  }

  return envelope;									/* return the struct with it's values			*/
}


void*	 GetDownSampled(double* buffer, size_t number_of_values, unsigned int downsampledItems, double* downSampledArray, unsigned int* downSampledArray_size)
{
	if(buffer == (void*)0)								/* Error if buffer is null :P */
		return 0;
	
	if(downSampledArray_size == (void*)0)				/* Error if the addr of supposed returned size is null */
		return 0;
	
	*downSampledArray_size = number_of_values/\			
							 downsampledItems;			/* Set dSA_size as all the totalValues_buffer / the number of elements we take to downsample */
	
	if(downSampledArray != (void*)0)					/* Clean if there's something in it already*/
	{
		free(downSampledArray);
		downSampledArray = 0;
	}		
	
	/* Now that we're sure downsampled is empty without an address and all: */
	downSampledArray = (double*)malloc((*downSampledArray_size) * sizeof(double));
	
	double sum=0;
	int k=0;
	size_t rounded_edge = number_of_values-number_of_values%downsampledItems;
	for(size_t i = 0; i < rounded_edge; i++)
	{
		sum+=buffer[i];

		if(i%downsampledItems==0)						/* every {downsampledItems} elements, we find the mean value and assign it to dSArray[k]*/
		{
			downSampledArray[k++] = sum/downsampledItems;
			sum=0;
		}
		
	}
	return downSampledArray;
		
}






















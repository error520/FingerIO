
/* 
	 *  @brief   ???MATLAB??spectrogram?????STFT???? 
	 *  @param   signalVector ????????? 
	 *  @param   N            ??????? 	 
	 *  @param   win          ?????? 
	 *  @param   noverlap     ?????????? 
	 *  @param   nfft         FFT/DFT????? 
	 *  @param   fs           ??????? 
	 *  @param   drawRect     ?????????rect 
     *  @param   context      ??????????? 
	 *  @return  ???????????? 
	 */  
#include <math.h>       /* cos */
#include <iostream>
#include <complex>
#include <stdio.h>
#include "Spectrogram.h"
#include <time.h>
#include <stdlib.h>
//#include <syswait.h>
#include <unistd.h>
#include <cstring>

using namespace std;
#define M_PI 3.1415926535897932384
static double **SinTable = NULL;

Spectrogram::Spectrogram()
{
	m_iFFTLen = 8192;
	//m_in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * m_iFFTLen);
    //m_out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * m_iFFTLen);

	m_iFFTGrade = 1;
	while(m_iFFTLen != 1<<m_iFFTGrade)
	{
		m_iFFTGrade++;
	}
	
	InitialFFT();
}

Spectrogram::~Spectrogram()
{
 
    //fftw_free(m_in); 
    //fftw_free(m_in);

}

int Spectrogram::log2(int N)    /*function to calculate the log2(.) of int numbers*/
{
  int k = N, i = 0;
  while(k) {
    k >>= 1;
    i++;
  }
  return i - 1;
}

int Spectrogram::check(int n)    //checking if the number of element is a power of 2
{
  return n > 0 && (n & (n - 1)) == 0;
}

int Spectrogram::reverse(int N, int n)    //calculating revers number
{
  int j, p = 0;
  for(j = 1; j <= log2(N); j++) {
    if(n & (1 << (log2(N) - j)))
      p |= 1 << (j - 1);
  }
  return p;
}

void Spectrogram::ordina(complex<double>* f1, int N) //using the reverse order in the array
{
  complex<double> * f2 = new complex<double>[N];
  for(int i = 0; i < N; i++)
    f2[i] = f1[reverse(N, i)];
  for(int j = 0; j < N; j++)
    f1[j] = f2[j];
  delete f2;
}

void Spectrogram::transform(complex<double>* f, int N) //
{
  ordina(f, N);    //first: reverse order
  complex<double> *W;
  W = (complex<double> *)malloc(N / 2 * sizeof(complex<double>));
  W[1] = polar(1., -2. * M_PI / N);
  W[0] = 1;
  for(int i = 2; i < N / 2; i++)
    W[i] = pow(W[1], i);
  int n = 1;
  int a = N / 2;
  for(int j = 0; j < log2(N); j++) {
    for(int i = 0; i < N; i++) {
      if(!(i & n)) {
        complex<double> temp = f[i];
        complex<double> Temp = W[(i * a) % (n * a)] * f[i + n];
        f[i] = temp + Temp;
        f[i + n] = temp - Temp;
      }
    }
    n *= 2;
    a = a / 2;
  }

  free(W);
}

void Spectrogram::FFT(complex<double>* f, int N, double d)
{
  transform(f, N);
  for(int i = 0; i < N; i++)
    f[i] *= d; //multiplying by step
}




void Spectrogram::doFFT(float * pfSignal, int iDataSize, int iNfft, float* pfFFtReal, float * pfFFtImage)
{
	complex<double> *vComplex = new complex<double>[iDataSize];		
		
	for(int i = 0; i < iDataSize; i++) {
		complex<double> cData(pfSignal[i],0.0f);
		vComplex[i] = cData;		
		//cout << "specify element number: data[ " << i << "]:" << vComplex[i].real()<< endl;
	}


	double d = 1;
	FFT(vComplex, iNfft, d);

	for(int i = 0; i < iDataSize; i++) {		
		//cout << "fft [ " << i << "]:" << vComplex[i].real() << "|" << vComplex[i].imag() << endl;
		pfFFtReal[i] = vComplex[i].real();
		pfFFtImage[i] = vComplex[i].imag();
	}
	delete vComplex;
}



void Spectrogram::doFastFFT(float * pfSignal, int iDataSize, int iNfft, float* pfFFtReal, float * pfFFtImage)
{
	/*fftw_plan p;
    for( int i=0; i < iDataSize; i++)
    {       
		m_in[i][0] = pfSignal[i];
        m_in[i][1] = 0.0;
    }
    p=fftw_plan_dft_1d(m_iFFTLen,m_in,m_out, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(p); /* repeat as needed
    fftw_destroy_plan(p);
	for(int i = 0; i < iDataSize; i++) {		
		//cout << "fft [ " << i << "]:" << vComplex[i].real() << "|" << vComplex[i].imag() << endl;
		pfFFtReal[i] = m_out[i][0];
		pfFFtImage[i] = m_out[i][1];
	}*/
}


void Spectrogram::doMidFFT(float * pfSignal, int iDataSize, int iNfft, float* pfFFtReal, float * pfFFtImage)
{
	double *vec = new double[iDataSize];
	double * outVec = new double[iDataSize];
    for( int i=0; i < iDataSize; i++)
    {       
		vec[i] = pfSignal[i];
        outVec[i] = 0.0;
    }
	tableFFT(vec, outVec,iNfft);
	for(int i = 0; i < iDataSize; i++) {		
		//cout << "fft [ " << i << "]:" << vComplex[i].real() << "|" << vComplex[i].imag() << endl;
		pfFFtReal[i] = vec[i];
		pfFFtImage[i] = outVec[i];
	}
	delete vec;
	delete outVec;
	
}

void Spectrogram::doFFTDouble(float * pfSignal, int iDataSize, int iNfft, double* pfFFtReal, double * pfFFtImage)
{
	complex<double> *vComplex = new complex<double>[iDataSize];		
		
	for(int i = 0; i < iDataSize; i++) {
		complex<double> cData(pfSignal[i],0.0f);
		vComplex[i] = cData;		
		//cout << "specify element number: data[ " << i << "]:" << vComplex[i].real()<< endl;
	}


	double d = 1;
	FFT(vComplex, iNfft, d);

	for(int i = 0; i < iDataSize; i++) {		
		//cout << "fft [ " << i << "]:" << vComplex[i].real() << "|" << vComplex[i].imag() << endl;
		pfFFtReal[i] = vComplex[i].real();
		pfFFtImage[i] = vComplex[i].imag();
	}
	delete vComplex;

}

bool Spectrogram::spectrogram(float* signalVector, int N, int win, int noverlap, int nfft, int fs, float ***pLogPXX, int& iDstRow, int &iDstColumn)
  {  
 	    bool isSpecOK = false;  

	    //hamming??????  
	    float* hammingWindow = new float[win];
		float windowPowValue = 0.0;  
	    for(int i = 0; i < win; i++)  
	    {  
	        hammingWindow[i] = 0.54 - 0.46 * cos(2*M_PI*i/(win-1));  
	        windowPowValue += powf(hammingWindow[i], 2);  
	    }  
	      
	    //???????????????????????????????????=????????????=??????  
	    int row, column, halfNfft;  
	    row = (N - noverlap)/(win - noverlap);  
		column = win;  
	    halfNfft = nfft/2+1;  
	      
	    //????t????  
	    float* timeVector = new float [row];  
	    for(int i = 0; i < row; i++)  
	    {  
	        timeVector[i] = ((float)i) /((float)(fs*(win/2+1+(win-noverlap)*i)));  
	    }  
		
	      
	    //??signal???  
	    float **signalXY = new float *[row];
		for(int i=0; i < row; i++)
			signalXY[i] = new float[column];  

	    for(int i = 0; i < row; i++)  
	    {  
	        for(int j = 0; j < column; j++)  
	        {  
	            signalXY[i][j] = signalVector[i*(win - noverlap) + j];  
	            signalXY[i][j] *= hammingWindow[j];  
	        }  
	    }  
	      
	    //????????????FFT  
	    float* fftReal = new float[nfft];
		float* freq = new float[nfft];

		float* fftImg = new float[nfft];
		float **Sxx = new float *[row];
		float **Pxx = new float *[row];
		float **logPxx = new float *[row];
		for(int i=0; i<row; i++)
		{
			Sxx[i] = new float[halfNfft];
			Pxx[i] = new float[halfNfft];
		    logPxx[i] = new float[halfNfft];
		}

		iDstRow = row;
		iDstColumn = halfNfft;
		float freqStep, pxxMax, pxxMin;  
	      
	    freqStep = ((float)fs)/((float)nfft);  
	    freq[0] = 0.0;  
	    for(int i = 1; i < nfft; i++)  
	    {  
	        freq[i] = freqStep + freq[i-1];  
	    }  



		
	   time_t tBegin = time(NULL);
	   clock_t start = clock(); 
	    //??Sxx  
	    for(int i = 0; i < row; i++)  
	    {  
	        //[FFT fft:signalXY[i] andOriginalLength:column andFFTCount:nfft andFFTReal:fftReal andFFTYImg:fftImg];  

			//doFFT(signalXY[i],column,nfft,fftReal,fftImg);
			//doFastFFT(signalXY[i],column,nfft,fftReal,fftImg);
			doMidFFT(signalXY[i],column,nfft,fftReal,fftImg);

			//FFT(signalXY[i], N, d);
	        for(int j = 0; j < halfNfft; j++)  
	        {  
	            Sxx[i][j] = (fftReal[j]*fftReal[j] + fftImg[j]*fftImg[j])/windowPowValue;  
	        }
	         //sleep(1);

	    }  
		 time_t tEnd = time(NULL);
		 time_t tCost = tEnd - tBegin;
		 clock_t finish = clock(); 
         double duration = (double)(finish - start) / CLOCKS_PER_SEC; 
	      	      
	    //??Pxx  
	    float fsFloat = (float)fs;  
	    Pxx[0][0] = Sxx[0][0]/fsFloat;  
	    logPxx[0][0] = 10*log10f(fabsf(Pxx[0][0]));  
	    pxxMax = logPxx[0][0];  
        pxxMin = logPxx[0][0];  
	      
	    for(int i = 1; i < row; i++)  
	    {  
	        Pxx[i][0] = Sxx[i][0]/fsFloat;  
	        logPxx[i][0] = 10*log10f(fabsf(Pxx[i][0]));  
         
	        if(logPxx[i][0] > pxxMax)  
	            pxxMax = logPxx[i][0];  
	        if(logPxx[i][0] < pxxMin)  
	            pxxMin = logPxx[i][0];  
	    }  
	    if(nfft%2)//????  
	    {  
	        for(int i = 0; i < row; i++)  
	        {  
	            for(int j = 1; j < halfNfft; j++)  
	            {  
	                Pxx[i][j] = Sxx[i][j]*2.0/((float)fs);  
	                logPxx[i][j] = 10*log10f(fabsf(Pxx[i][j]));  
	                  
	                if(logPxx[i][j] > pxxMax)  
	                    pxxMax = logPxx[i][j];  
	                if(logPxx[i][j] < pxxMin)  
	                    pxxMin = logPxx[i][j];  
	            }  
	        }  
	    }  
	    else//???  
	    {  
	        for(int i = 0; i < row; i++)  
			{  
	            Pxx[i][halfNfft-1] = Sxx[i][halfNfft-1]/((float)fs);  
	            logPxx[i][halfNfft-1] = 10*log10f(fabsf(Pxx[i][halfNfft-1]));  
	              
	            if(logPxx[i][halfNfft-1] > pxxMax)  
                pxxMax = logPxx[i][halfNfft-1];  
	            if(logPxx[i][halfNfft-1] < pxxMin)  
	                pxxMin = logPxx[i][halfNfft-1];  
	        }  
	        for(int i = 0; i < row; i++)  
	        {  
	            for(int j = 1; j < halfNfft-1; j++)  
	            {  
	                Pxx[i][j] = Sxx[i][j]*2.0/((float)fs);  
	                logPxx[i][j] = 10*log10f(fabsf(Pxx[i][j]));  
	                  
	                if(logPxx[i][j] > pxxMax)  
	                    pxxMax = logPxx[i][j];  
	                if(logPxx[i][j] < pxxMin)  
	                    pxxMin = logPxx[i][j];  
	            }  
	        }  
	    }  

		FILE* fd1 = fopen("./data/out/pxx.txt","wb");
		if(fd1 != NULL)
		{
			for(int j = 0; j < halfNfft; j++)  
			{
				stringstream os;

				/*for(int i = 0; i < row; i++)  
				{  
					//os << logPxx[i][j]<<"|";
				}
				if(j < halfNfft-1)
				{
					//os<<endl;
				}*/
				//fwrite(os.str().c_str(), sizeof(char),sizeof(os.str().c_str()), fd1);
				fputs(os.str().c_str(),fd1);
			 }
			fclose(fd1);
		}

		delete fftReal;
		delete fftImg;
		delete timeVector;
		delete Sxx;
		delete Pxx;
		delete hammingWindow;
		delete signalXY;
		delete freq;
		*pLogPXX = logPxx;
		return true;
		}

bool Spectrogram::spectrogramdouble(float* signalVector,int N, int win, int noverlap, int nfft,int fs)
  {  
 	    bool isSpecOK = false;  

	    //hamming??????  
	    double* hammingWindow = new double[win];
		double windowPowValue = 0.0;  
	    for(int i = 0; i < win; i++)  
	    {  
	        hammingWindow[i] = 0.54 - 0.46 * cos(2*M_PI*i/(win-1));  
	        windowPowValue += powf(hammingWindow[i], 2);  
	    }  
	      
	    //???????????????????????????????????=????????????=??????  
	    int row, column, halfNfft;  
	    row = (N - noverlap)/(win - noverlap);  
		column = win;  
	    halfNfft = nfft/2+1;  
	      
	    //????t????  
	    float* timeVector = new float [row];  
	    for(int i = 0; i < row; i++)  
	    {  
	        timeVector[i] = ((float)i) /((float)(fs*(win/2+1+(win-noverlap)*i)));  
	    }  
		
	      
	    //??signal???  
	    float **signalXY = new float *[row];
		for(int i=0; i < row; i++)
			signalXY[i] = new float[column];  

	    for(int i = 0; i < row; i++)  
	    {  
	        for(int j = 0; j < column; j++)  
	        {  
	            signalXY[i][j] = signalVector[i*(win - noverlap) + j];  
	            signalXY[i][j] *= hammingWindow[j];  
	        }  
	    }  
	      
	    //????????????FFT  
	    double* fftReal = new double[nfft];
		double* freq = new double[nfft];

		double* fftImg = new double[nfft];
		double **Sxx = new double *[row];
		double **Pxx = new double *[row];
		double **logPxx = new double *[row];
		for(int i=0; i<row; i++)
		{
			Sxx[i] = new double[halfNfft];
			Pxx[i] = new double[halfNfft];
		    logPxx[i] = new double[halfNfft];
		}
		double freqStep, pxxMax, pxxMin;  
	      
	    freqStep = ((double)fs)/((double)nfft);  
	    freq[0] = 0.0;  
	    for(int i = 1; i < nfft; i++)  
	    {  
	        freq[i] = freqStep + freq[i-1];  
	    }  



		
	      
	    //??Sxx  
	    for(int i = 0; i < row; i++)  
	    {  
	        //[FFT fft:signalXY[i] andOriginalLength:column andFFTCount:nfft andFFTReal:fftReal andFFTYImg:fftImg];  

			doFFTDouble(signalXY[i],column,nfft,fftReal,fftImg);

			//FFT(signalXY[i], N, d);
	        for(int j = 0; j < halfNfft; j++)  
	        {  
	            Sxx[i][j] = (fftReal[j]*fftReal[j] + fftImg[j]*fftImg[j])/windowPowValue;  
	        }  
	    }  
	      	      
	    //??Pxx  
	    double fsFloat = (double)fs;  
	    Pxx[0][0] = Sxx[0][0]/fsFloat;  
	    logPxx[0][0] = 10*log10f(fabsf(Pxx[0][0]));  
	    pxxMax = logPxx[0][0];  
        pxxMin = logPxx[0][0];  
	      
	    for(int i = 1; i < row; i++)  
	    {  
	        Pxx[i][0] = Sxx[i][0]/fsFloat;  
	        logPxx[i][0] = 10*log10f(fabsf(Pxx[i][0]));  
         
	        if(logPxx[i][0] > pxxMax)  
	            pxxMax = logPxx[i][0];  
	        if(logPxx[i][0] < pxxMin)  
	            pxxMin = logPxx[i][0];  
	    }  
	    if(nfft%2)//????  
	    {  
	        for(int i = 0; i < row; i++)  
	        {  
	            for(int j = 1; j < halfNfft; j++)  
	            {  
	                Pxx[i][j] = Sxx[i][j]*2.0/((float)fs);  
	                logPxx[i][j] = 10*log10f(fabsf(Pxx[i][j]));  
	                  
	                if(logPxx[i][j] > pxxMax)  
	                    pxxMax = logPxx[i][j];  
	                if(logPxx[i][j] < pxxMin)  
	                    pxxMin = logPxx[i][j];  
	            }  
	        }  
	    }  
	    else//???  
	    {  
	        for(int i = 0; i < row; i++)  
			{  
	            Pxx[i][halfNfft-1] = Sxx[i][halfNfft-1]/((float)fs);  
	            logPxx[i][halfNfft-1] = 10*log10f(fabsf(Pxx[i][halfNfft-1]));  
	              
	            if(logPxx[i][halfNfft-1] > pxxMax)  
                pxxMax = logPxx[i][halfNfft-1];  
	            if(logPxx[i][halfNfft-1] < pxxMin)  
	                pxxMin = logPxx[i][halfNfft-1];  
	        }  
	        for(int i = 0; i < row; i++)  
	        {  
	            for(int j = 1; j < halfNfft-1; j++)  
	            {  
	                Pxx[i][j] = Sxx[i][j]*2.0/((float)fs);  
	                logPxx[i][j] = 10*log10f(fabsf(Pxx[i][j]));  
	                  
	                if(logPxx[i][j] > pxxMax)  
	                    pxxMax = logPxx[i][j];  
	                if(logPxx[i][j] < pxxMin)  
	                    pxxMin = logPxx[i][j];  
	            }  
	        }  
	    }  

		FILE* fd1 = fopen("./data/out/pxxdouble.txt","wb");
		if(fd1 != NULL)
		{
			for(int j = 0; j < halfNfft; j++)  
			{
				stringstream os;

				/*for(int i = 0; i < row; i++)  
				{  
					os << logPxx[i][j]<<"|";
				}
				if(j < halfNfft-1)
				{
					os<<endl;
				}*/
				//fwrite(os.str().c_str(), sizeof(char),sizeof(os.str().c_str()), fd1);
				fputs(os.str().c_str(),fd1);
			 }
			fclose(fd1);
		}

		delete fftReal;
		delete fftImg;
		delete Sxx;
		delete Pxx;
		delete hammingWindow;
		delete signalXY;
		delete freq;
		delete timeVector;
		delete logPxx;
		return true;
		}
#if 1
		/* cos ?????? */
const  short g_nCosTable[257] =
{
	32767,	32766,	32758,	32746,	32729,	32706,	32679,	32647,
	32610,	32568,	32522,	32470,	32413,	32352,	32286,	32214,
	32138,	32058,	31972,	31881,	31786,	31686,	31581,	31471,
	31357,	31238,	31114,	30986,	30853,	30715,	30572,	30425,
	30274,	30118,	29957,	29792,	29622,	29448,	29269,	29086,
	28899,	28707,	28511,	28311,	28106,	27897,	27684,	27467,
	27246,	27020,	26791,	26557,	26320,	26078,	25833,	25583,
	25330,	25073,	24812,	24548,	24279,	24008,	23732,	23453,
	23170,	22884,	22595,	22302,	22006,	21706,	21403,	21097,
	20788,	20475,	20160,	19841,	19520,	19195,	18868,	18538,
	18205,	17869,	17531,	17190,	16846,	16500,	16151,	15800,
	15447,	15091,	14733,	14373,	14010,	13646,	13279,	12910,
	12540,	12167,	11793,	11417,	11039,	10660,	10279,	9896,
	9512,	9127,	8740,	8351,	7962,	7571,	7180,	6787,
	6393,	5998,	5602,	5205,	4808,	4410,	4011,	3612,
	3212,	2811,	2411,	2009,	1608,	1206,	804,	402,
	0,		-402,	-804,	-1206,	-1608,	-2009,	-2411,	-2811,
	-3212,	-3612,	-4011,	-4410,	-4808,	-5205,	-5602,	-5998,
	-6393,	-6787,	-7180,	-7571,	-7962,	-8351,	-8740,	-9127,
	-9512,	-9896,	-10279,	-10660,	-11039,	-11417,	-11793,	-12167,
	-12540,	-12910,	-13279,	-13646,	-14010,	-14373,	-14733,	-15091,
	-15447,	-15800,	-16151,	-16500,	-16846,	-17190,	-17531,	-17869,
	-18205,	-18538,	-18868,	-19195,	-19520,	-19841,	-20160,	-20475,
	-20788,	-21097,	-21403,	-21706,	-22006,	-22302,	-22595,	-22884,
	-23170,	-23453,	-23732,	-24008,	-24279,	-24548,	-24812,	-25073,
	-25330,	-25583,	-25833,	-26078,	-26320,	-26557,	-26791,	-27020,
	-27246,	-27467,	-27684,	-27897,	-28106,	-28311,	-28511,	-28707,
	-28899,	-29086,	-29269,	-29448,	-29622,	-29792,	-29957,	-30118,
	-30274,	-30425,	-30572,	-30715,	-30853,	-30986,	-31114,	-31238,
	-31357,	-31471,	-31581,	-31686,	-31786,	-31881,	-31972,	-32058,
	-32138,	-32214,	-32286,	-32352,	-32413,	-32470,	-32522,	-32568,
	-32610,	-32647,	-32679,	-32706,	-32729,	-32746,	-32758,	-32766,
	-32768
};


		/* ???????????cos */
float cos_( unsigned short i)
{
	unsigned short tab1, tab2;
	int last;
	float f = 0.f;
	tab1 = (i >> 7) & 0x01FF;
	tab2 = i & 0x007F;
	if (tab1 & 0x100)
	{
		tab1 = 0x1FF - tab1;
		tab2 = 0x80 - tab2;
	}
	last = (0x80L - tab2) * g_nCosTable[tab1];
	last += (int)tab2 * g_nCosTable[tab1 + 1];
	f = (float)((last >> 7) / 32768.f);
	return f;
}
#define sin_(x) cos_(0x4000 - (x))

/*
x-???????????????????n??????????????????
y-???????????????????n?????????????????
a-???????????????????n????????????????
b-???????????????????n???????????????
n-?????????????????
sign-???????????sign=1????????dft()????????????????????sign=-1???dft()??????????????????
*/
		void dft(double *x,double *y,double *p, double *a,double *b,int n,int sign)
{
	int i,k;
	double c,d,q,w,s;
	unsigned short d1;
	q = 6.28318530715/n;
	for(k=0;k<n;k++)
	{
		w = k*q;
		a[k] = b[k] = 0.0;
		for(i=0;i<n;i++)
		{
			d = i*w+p[i];
			if (d > 0)
				d1 = (unsigned short)(d* 10430.378 + 0.5);
			else
			{
				d1 = (unsigned short)(d * 10430.378 - 0.5);
			}
			c = cos_(d1);
			s = sin_(d1)*sign;
			a[k] += c*x[i] + s*y[i];
			b[k] += c*y[i] - s*x[i];
		}
	}
	if(sign == -1)
	{
		c=1.0/n;
		for(k=0;k<n;k++)
		{
			a[k] = c*a[k];
			b[k] = c*b[k];
		}
	}
}




void Spectrogram::InitialFFT()
{
	int nLen, nI,nJ, FFTLen;
	double dArg;
	if(SinTable != NULL) return ;
	SinTable = (double**)malloc(sizeof(double*)*m_iFFTGrade);
	memset(SinTable, 0, sizeof(double*)*m_iFFTGrade);

	FFTLen  =1;
	for (nJ=0;nJ<m_iFFTGrade;nJ++)
	{
		FFTLen = 1<<(nJ+2);
		SinTable[nJ]  = (double*)malloc(sizeof(double)*FFTLen);
		memset(SinTable[nJ], 0, sizeof(double)*FFTLen);
		nLen = FFTLen - FFTLen/4 + 1;
		dArg = M_PI / FFTLen * 2;
		SinTable[nJ][0] = 0;
		for (nI=1;nI<nLen;nI++)
		{
			SinTable[nJ][nI] = sin(dArg * (double) nI);
		}
		SinTable[nJ][FFTLen/2] = 0;
	}

}

int Spectrogram::tableFFT(double *dInput, double *dOutput, int nFFTLen)
{
	int nJ, nLMX, nI;
	int nIndex;
	double *pXp, *pYp;
	double *pSin, *pCos;
	int nLF, nLix;
	double dT1, dT2;

	nIndex = 1;
	while(nFFTLen != 1<<nIndex)
	{
		nIndex++;
	}
	pSin = SinTable[nIndex-2];

	nLF = 1;
	nLMX = nFFTLen;

	for (;;) 
	{
		nLix = nLMX;
		nLMX /= 2;
		if (nLMX <= 1)
		{
			break;
		}
		pSin = SinTable[nIndex-2];
		pCos = SinTable[nIndex-2] + nFFTLen / 4;
		for (nJ = 0; nJ < nLMX; nJ++) 
		{
			pXp = &dInput[nJ];
			pYp = &dOutput[nJ];
			for (nI = nLix; nI <= nFFTLen; nI += nLix) 
			{
				dT1 = *(pXp) - *(pXp + nLMX);
				dT2 = *(pYp) - *(pYp + nLMX);
				*(pXp) += *(pXp + nLMX);
				*(pYp) += *(pYp + nLMX);
				*(pXp + nLMX) = *pCos * dT1 + *pSin * dT2;
				*(pYp + nLMX) = *pCos * dT2 - *pSin * dT1;
				pXp += nLix;
				pYp += nLix;
			}
			pSin += nLF;
			pCos += nLF;
		}
		nLF += nLF;
	}

	pXp = dInput;
	pYp = dOutput;
	for (nI = nFFTLen / 2; nI--; pXp += 2, pYp += 2) 
	{
		dT1 = *(pXp) - *(pXp + 1);
		dT2 = *(pYp) - *(pYp + 1);
		*(pXp) += *(pXp + 1);
		*(pYp) += *(pYp + 1);
		*(pXp + 1) = dT1;
		*(pYp + 1) = dT2;
	}

	nJ = 0;
	pXp = dInput;
	pYp = dOutput;

	for (nLMX = 0; nLMX < nFFTLen - 1; nLMX++) 
	{
		if ((nI = nLMX - nJ) < 0) 
		{
			dT1 = *(pXp);
			dT2 = *(pYp);
			*(pXp) = *(pXp + nI);
			*(pYp) = *(pYp + nI);
			*(pXp + nI) = dT1;
			*(pYp + nI) = dT2;
		}
		nI = nFFTLen / 2;
		while (nI <= nJ) 
		{
			nJ -= nI;
			nI /= 2;
		}
		nJ += nI;
		pXp = dInput + nJ;
		pYp = dOutput + nJ;
	}
	return (0);
}


#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* convert non-interleaved floating-point to interleaved Q15 */
void
float_to_16bit_int(float **in, short *out, unsigned n_ch, unsigned buf_length)
{
  int i;
  for (i = 0; i < buf_length; i++)
    {
      int j;
      for (j = 0; j < n_ch; j++)
	{
	  float temp = (32768 * in[j][i] + 0.5);
	  if (temp >= 32768)
	    {
	      temp = 32767;
	    }
	  out[i*n_ch + j] = (short)temp;
	}
    }
}

/* generate pulse train */
void
synthesize_pulse(float *buf, unsigned length, unsigned long sample_rate)
{
  static unsigned time = 0;
  unsigned i;
  unsigned long interval = sample_rate / 2;
  
  int n_harmonics = 5;
  unsigned fundamental = 1000;


  for (i = 0; i < length; i++)
    {
      int j;
      float sample = 0;

      /* generate sum of sine tones for one sample*/
      for (j = 0; j < n_harmonics; j++)
	{
	  /* scale subsequent harmonics by progressively smaller gain */
	  float gain = powf(10, -2*n_harmonics*(j+1)/20.0);
	  sample += gain * cosf(2*M_PI*(2*j+1)*fundamental*(time + i)/(double)sample_rate);
	}
      
      /* fade in */
      if (time % interval < length)
	{
	  buf[i] = (i/(float)length) * sample;
	}
      /* unity */
      else if (time % interval < 5 * length)
	{
	  buf[i] = sample;
	}
      /* fade out */
      else if (time % interval < 6 * length)
	{
	  buf[i] = ((length - i)/(float)length) * sample;
	}
      /* silence */
      else
	{
	  buf[i] = 0;
	}     
    }
  time += length;
}

/* convert mono signal to horizontal B-format */
void
encode_wxy(int count, float azi, float ele, float *in, float **wxy)
{
  const float w_coef = 1/sqrtf(2);
  float *W = wxy[0];
  float *X = wxy[1];
  float *Y = wxy[2];
  
  int i;
  
  for (i = 0; i < count; i++)
    {
      W[i] = w_coef * in[i];
      X[i] = cosf(azi) * cosf(ele) * in[i];
      Y[i] = sinf(azi) * cosf(ele) * in[i];
    }
}

/* apply separate 154-tap FIR filters to each spherical harmonic (generated from Faust code) */
void compute (int count, float** input, float** output) {
  float* input0 = input[0];
  float* input1 = input[1];
  float* input2 = input[2];
  float* output0 = output[0];
  float* output1 = output[1];

  static float fVec0[256];
  static float fVec1[256];
  static float fVec2[256];
  static int IOTA = 0;

  for (int i=0; i<count; i++) {
    float fTemp0 = (float)input2[i];
    fVec0[IOTA&255] = fTemp0;
    float fTemp1 = (0.000762939453125f * fVec0[(IOTA-113)&255]);
    float fTemp2 = (float)input1[i];
    fVec1[IOTA&255] = fTemp2;
    float fTemp3 = (float)input0[i];
    fVec2[IOTA&255] = fTemp3;
    float fTemp4 = (0.00030517578125f * ((((((((((fVec1[(IOTA-136)&255] + (((((((((((((((((fVec2[(IOTA-128)&255] + (0.7f * fVec2[(IOTA-126)&255])) + (1.3f * fVec2[(IOTA-130)&255])) + (1.7f * fVec2[(IOTA-132)&255])) + (2.0f * fVec2[(IOTA-134)&255])) + (2.3f * fVec2[(IOTA-136)&255])) + (2.7f * fVec2[(IOTA-138)&255])) + (3.0f * fVec2[(IOTA-140)&255])) + (3.4f * fVec2[(IOTA-142)&255])) + (3.8f * fVec2[(IOTA-144)&255])) + (4.2f * fVec2[(IOTA-146)&255])) + (4.6f * fVec2[(IOTA-148)&255])) + (4.9f * fVec2[(IOTA-150)&255])) + (5.4f * fVec2[(IOTA-152)&255])) + (0.3f * fVec1[(IOTA-130)&255])) + (0.1f * fVec1[(IOTA-128)&255])) + (0.5f * fVec1[(IOTA-132)&255])) + (0.8f * fVec1[(IOTA-134)&255]))) + (1.2f * fVec1[(IOTA-138)&255])) + (1.4f * fVec1[(IOTA-140)&255])) + (1.6f * fVec1[(IOTA-142)&255])) + (1.8f * fVec1[(IOTA-144)&255])) + (2.0f * fVec1[(IOTA-146)&255])) + (2.3f * fVec1[(IOTA-148)&255])) + (2.8f * fVec1[(IOTA-152)&255])) + (2.5f * fVec1[(IOTA-150)&255])) - (((((((((((((((((fVec1[(IOTA-131)&255] + ((((((((((((((fVec2[(IOTA-127)&255] + (0.7f * ((fVec2[(IOTA-125)&255] + ((((fVec2[(IOTA-118)&255] + ((((2.142857142857143f * fVec2[(IOTA-112)&255]) + (2.5714285714285716f * fVec2[(IOTA-110)&255])) + (1.8571428571428572f * fVec2[(IOTA-114)&255])) + (1.4285714285714286f * fVec2[(IOTA-116)&255]))) + (0.42857142857142855f * fVec2[(IOTA-120)&255])) + (0.14285714285714285f * fVec2[(IOTA-121)&255])) + (0.5714285714285714f * fVec2[(IOTA-123)&255]))) - (((((((2.2857142857142856f * fVec2[(IOTA-111)&255]) + (2.857142857142857f * ((fVec2[(IOTA-109)&255] + (((fVec2[(IOTA-103)&255] + ((0.5f * fVec2[(IOTA-101)&255]) + (0.15f * ((fVec2[(IOTA-99)&255] + (fVec2[(IOTA-97)&255] + ((12.0f * fVec2[(IOTA-93)&255]) + (7.333333333333333f * fVec2[(IOTA-95)&255])))) - ((((21.0f * (((((1.1428571428571428f * fVec2[(IOTA-86)&255]) + fVec2[(IOTA-92)&255]) + (1.0317460317460319f * fVec2[(IOTA-88)&255])) + fVec2[(IOTA-90)&255]) - (((0.7301587301587301f * fVec2[(IOTA-91)&255]) + ((0.9523809523809523f * fVec2[(IOTA-87)&255]) + (0.8571428571428571f * fVec2[(IOTA-89)&255]))) + (0.9365079365079365f * (((fVec2[(IOTA-85)&255] + ((((((((((((((((((((((((((((((fVec2[(IOTA-20)&255] + (1.305084745762712f * fVec2[(IOTA-18)&255])) + (1.423728813559322f * ((fVec2[(IOTA-16)&255] + (fVec2[(IOTA-14)&255] + (((((((0.6904761904761905f * fVec2[IOTA&255]) + (0.7380952380952381f * fVec2[(IOTA-2)&255])) + (0.7976190476190477f * fVec2[(IOTA-4)&255])) + (0.8452380952380952f * fVec2[(IOTA-6)&255])) + (0.8928571428571429f * fVec2[(IOTA-8)&255])) + (0.9404761904761905f * fVec2[(IOTA-10)&255])) + (0.9761904761904762f * fVec2[(IOTA-12)&255])))) - (fVec2[(IOTA-15)&255] + (((((((0.7142857142857143f * fVec2[(IOTA-1)&255]) + (0.7619047619047619f * fVec2[(IOTA-3)&255])) + (0.8095238095238095f * fVec2[(IOTA-5)&255])) + (0.8690476190476191f * fVec2[(IOTA-7)&255])) + (0.9166666666666666f * fVec2[(IOTA-9)&255])) + (0.9642857142857143f * fVec2[(IOTA-11)&255])) + (0.9880952380952381f * fVec2[(IOTA-13)&255])))))) + (0.1694915254237288f * fVec2[(IOTA-22)&255])) + (0.7288135593220338f * fVec2[(IOTA-23)&255])) + (6.0508474576271185f * fVec2[(IOTA-25)&255])) + (57.186440677966104f * fVec2[(IOTA-28)&255])) + (1.0508474576271187f * fVec2[(IOTA-29)&255])) + (35.13559322033898f * fVec2[(IOTA-30)&255])) + (29.83050847457627f * fVec2[(IOTA-32)&255])) + (31.084745762711865f * fVec2[(IOTA-34)&255])) + (50.389830508474574f * fVec2[(IOTA-36)&255])) + (47.067796610169495f * fVec2[(IOTA-40)&255])) + (15.135593220338983f * fVec2[(IOTA-38)&255])) + (9.915254237288135f * fVec2[(IOTA-39)&255])) + (53.03389830508475f * fVec2[(IOTA-43)&255])) + (29.576271186440678f * fVec2[(IOTA-45)&255])) + (20.71186440677966f * fVec2[(IOTA-47)&255])) + (15.76271186440678f * fVec2[(IOTA-49)&255])) + (12.576271186440678f * fVec2[(IOTA-51)&255])) + (10.474576271186441f * fVec2[(IOTA-53)&255])) + (9.76271186440678f * fVec2[(IOTA-55)&255])) + (4.745762711864407f * fVec2[(IOTA-65)&255])) + (2.6271186440677967f * fVec2[(IOTA-71)&255])) + (2.711864406779661f * fVec2[(IOTA-63)&255])) + (4.220338983050848f * fVec2[(IOTA-67)&255])) + (1.8644067796610169f * fVec2[(IOTA-73)&255])) + (3.406779661016949f * fVec2[(IOTA-69)&255])) + (0.9661016949152542f * fVec2[(IOTA-75)&255])) + (0.576271186440678f * fVec2[(IOTA-79)&255])) + (0.2033898305084746f * fVec2[(IOTA-81)&255]))) + (0.7627118644067796f * fVec2[(IOTA-83)&255])) - ((((((((((fVec2[(IOTA-64)&255] + (((((((((((((((((((((((((((1.1864406779661016f * fVec2[(IOTA-19)&255]) + (1.3728813559322033f * fVec2[(IOTA-17)&255])) + (0.6779661016949152f * fVec2[(IOTA-21)&255])) + (2.3728813559322033f * fVec2[(IOTA-24)&255])) + (17.983050847457626f * fVec2[(IOTA-26)&255])) + (397.89830508474574f * fVec2[(IOTA-27)&255])) + (2.1186440677966103f * fVec2[(IOTA-31)&255])) + (8.169491525423728f * fVec2[(IOTA-33)&255])) + (19.64406779661017f * fVec2[(IOTA-35)&255])) + (297.1864406779661f * fVec2[(IOTA-37)&255])) + (7.305084745762712f * fVec2[(IOTA-46)&255])) + (117.20338983050847f * fVec2[(IOTA-41)&255])) + (119.15254237288136f * fVec2[(IOTA-42)&255])) + (16.627118644067796f * fVec2[(IOTA-44)&255])) + (4.610169491525424f * fVec2[(IOTA-48)&255])) + (3.610169491525424f * fVec2[(IOTA-50)&255])) + (3.4915254237288136f * fVec2[(IOTA-54)&255])) + (3.288135593220339f * fVec2[(IOTA-52)&255])) + (8.576271186440678f * fVec2[(IOTA-56)&255])) + (42.49152542372882f * fVec2[(IOTA-58)&255])) + (22.915254237288135f * fVec2[(IOTA-59)&255])) + (49.74576271186441f * fVec2[(IOTA-57)&255])) + (29.93220338983051f * fVec2[(IOTA-61)&255])) + (33.016949152542374f * fVec2[(IOTA-60)&255])) + (10.271186440677965f * fVec2[(IOTA-62)&255])) + (0.4576271186440678f * fVec2[(IOTA-66)&255])) + (1.7118644067796611f * fVec2[(IOTA-72)&255]))) + (0.8135593220338984f * fVec2[(IOTA-68)&255])) + (1.271186440677966f * fVec2[(IOTA-70)&255])) + (3.3559322033898304f * fVec2[(IOTA-76)&255])) + (1.5084745762711864f * fVec2[(IOTA-84)&255])) + (2.2203389830508473f * fVec2[(IOTA-74)&255])) + (0.1016949152542373f * fVec2[(IOTA-77)&255])) + (2.6779661016949152f * fVec2[(IOTA-78)&255])) + (2.406779661016949f * fVec2[(IOTA-80)&255])) + (2.135593220338983f * fVec2[(IOTA-82)&255]))))))) + (21.666666666666668f * fVec2[(IOTA-94)&255])) + (25.333333333333332f * fVec2[(IOTA-96)&255])) + (25.666666666666668f * fVec2[(IOTA-98)&255])))))) + (1.15f * fVec2[(IOTA-105)&255])) + (1.1f * fVec2[(IOTA-107)&255]))) - (((((3.35f * fVec2[(IOTA-100)&255]) + (2.4f * fVec2[(IOTA-102)&255])) + (1.7f * fVec2[(IOTA-104)&255])) + (1.3f * fVec2[(IOTA-106)&255])) + (1.1f * fVec2[(IOTA-108)&255]))))) + (1.7142857142857142f * fVec2[(IOTA-113)&255])) + (1.1428571428571428f * fVec2[(IOTA-115)&255])) + (0.7142857142857143f * fVec2[(IOTA-117)&255])) + (0.2857142857142857f * fVec2[(IOTA-119)&255])) + (0.5714285714285714f * fVec2[(IOTA-124)&255]))))) + (1.3f * fVec2[(IOTA-129)&255])) + (1.6f * fVec2[(IOTA-131)&255])) + (1.9f * fVec2[(IOTA-133)&255])) + (2.3f * fVec2[(IOTA-135)&255])) + (2.6f * fVec2[(IOTA-137)&255])) + (2.9f * fVec2[(IOTA-139)&255])) + (3.3f * fVec2[(IOTA-141)&255])) + (3.6f * fVec2[(IOTA-143)&255])) + (4.0f * fVec2[(IOTA-145)&255])) + (4.4f * fVec2[(IOTA-147)&255])) + (5.5f * fVec2[(IOTA-153)&255])) + (4.7f * fVec2[(IOTA-149)&255])) + (5.1f * fVec2[(IOTA-151)&255]))) + (0.8f * fVec1[(IOTA-129)&255])) + (0.5f * fVec1[(IOTA-125)&255])) + (0.1f * fVec1[(IOTA-126)&255])) + (0.6f * fVec1[(IOTA-127)&255])) + (0.3f * ((fVec1[(IOTA-124)&255] + ((fVec1[(IOTA-123)&255] + ((((((((((((((((10.666666666666666f * ((fVec1[(IOTA-92)&255] + (fVec1[(IOTA-90)&255] + ((((0.8125f * fVec1[(IOTA-86)&255]) + (0.96875f * fVec1[(IOTA-88)&255])) + (0.34375f * fVec1[(IOTA-84)&255])) + (0.125f * ((fVec1[(IOTA-75)&255] + (((((((fVec1[(IOTA-80)&255] + (((((((((((((((((((((50.75f * fVec1[(IOTA-34)&255]) + (9.25f * fVec1[(IOTA-31)&255])) + (0.75f * fVec1[(IOTA-32)&255])) + (17.0f * fVec1[(IOTA-29)&255])) + (3.9e+02f * fVec1[(IOTA-36)&255])) + (1174.25f * fVec1[(IOTA-39)&255])) + (3174.5f * fVec1[(IOTA-41)&255])) + (2584.75f * fVec1[(IOTA-42)&255])) + (426.75f * fVec1[(IOTA-44)&255])) + (212.25f * fVec1[(IOTA-46)&255])) + (138.0f * fVec1[(IOTA-48)&255])) + (101.75f * fVec1[(IOTA-50)&255])) + (80.25f * fVec1[(IOTA-52)&255])) + (66.5f * fVec1[(IOTA-54)&255])) + (49.75f * fVec1[(IOTA-58)&255])) + (56.75f * fVec1[(IOTA-56)&255])) + (44.25f * fVec1[(IOTA-60)&255])) + (4e+01f * fVec1[(IOTA-62)&255])) + (34.25f * fVec1[(IOTA-66)&255])) + (36.75f * fVec1[(IOTA-64)&255])) + (32.25f * fVec1[(IOTA-68)&255]))) + (31.25f * fVec1[(IOTA-70)&255])) + (25.75f * fVec1[(IOTA-78)&255])) + (33.25f * fVec1[(IOTA-74)&255])) + (48.75f * fVec1[(IOTA-76)&255])) + (31.0f * fVec1[(IOTA-72)&255])) + (12.25f * fVec1[(IOTA-77)&255]))) - ((((((((((((((((((((((((16.25f * fVec1[(IOTA-33)&255]) + (13.0f * fVec1[(IOTA-30)&255])) + (17.5f * ((fVec1[(IOTA-28)&255] + (((fVec1[(IOTA-22)&255] + (((((((((((0.44285714285714284f * fVec1[IOTA&255]) + (0.4714285714285714f * fVec1[(IOTA-2)&255])) + (0.5142857142857142f * fVec1[(IOTA-4)&255])) + (0.5571428571428572f * fVec1[(IOTA-6)&255])) + (0.6142857142857143f * fVec1[(IOTA-8)&255])) + (0.6571428571428571f * fVec1[(IOTA-10)&255])) + (0.7142857142857143f * fVec1[(IOTA-12)&255])) + (0.7714285714285715f * fVec1[(IOTA-14)&255])) + (0.8285714285714286f * fVec1[(IOTA-16)&255])) + (0.8857142857142857f * fVec1[(IOTA-18)&255])) + (0.9428571428571428f * fVec1[(IOTA-20)&255]))) + (1.042857142857143f * fVec1[(IOTA-24)&255])) + (1.0571428571428572f * fVec1[(IOTA-26)&255]))) - ((((((((((((((0.5285714285714286f * fVec1[(IOTA-1)&255]) + (0.5714285714285714f * fVec1[(IOTA-3)&255])) + (0.6142857142857143f * fVec1[(IOTA-5)&255])) + (0.6571428571428571f * fVec1[(IOTA-7)&255])) + (0.7f * fVec1[(IOTA-9)&255])) + (0.7571428571428571f * fVec1[(IOTA-11)&255])) + (0.8142857142857143f * fVec1[(IOTA-13)&255])) + (0.8714285714285714f * fVec1[(IOTA-15)&255])) + (0.9285714285714286f * fVec1[(IOTA-17)&255])) + (0.9857142857142858f * fVec1[(IOTA-19)&255])) + (1.042857142857143f * fVec1[(IOTA-21)&255])) + (1.1f * fVec1[(IOTA-23)&255])) + (1.1285714285714286f * fVec1[(IOTA-25)&255])) + (1.1142857142857143f * fVec1[(IOTA-27)&255]))))) + (128.0f * fVec1[(IOTA-35)&255])) + (5714.5f * fVec1[(IOTA-37)&255])) + (572.25f * fVec1[(IOTA-38)&255])) + (887.25f * fVec1[(IOTA-40)&255])) + (614.25f * fVec1[(IOTA-43)&255])) + (254.0f * fVec1[(IOTA-45)&255])) + (150.75f * fVec1[(IOTA-47)&255])) + (103.5f * fVec1[(IOTA-49)&255])) + (76.75f * fVec1[(IOTA-51)&255])) + (6e+01f * fVec1[(IOTA-53)&255])) + (33.5f * fVec1[(IOTA-59)&255])) + (48.25f * fVec1[(IOTA-55)&255])) + (4e+01f * fVec1[(IOTA-57)&255])) + (28.25f * fVec1[(IOTA-61)&255])) + (14.25f * fVec1[(IOTA-69)&255])) + (24.25f * fVec1[(IOTA-63)&255])) + (17.25f * fVec1[(IOTA-67)&255])) + (20.5f * fVec1[(IOTA-65)&255])) + (11.0f * fVec1[(IOTA-71)&255])) + (6.75f * fVec1[(IOTA-73)&255])) + (15.25f * fVec1[(IOTA-79)&255]))))))) - (((((((1.28125f * fVec1[(IOTA-91)&255]) + (1.46875f * fVec1[(IOTA-89)&255])) + (1.78125f * fVec1[(IOTA-87)&255])) + (4.46875f * fVec1[(IOTA-81)&255])) + (0.65625f * fVec1[(IOTA-82)&255])) + (3.1875f * fVec1[(IOTA-83)&255])) + (2.25f * fVec1[(IOTA-85)&255])))) + (1e+01f * fVec1[(IOTA-94)&255])) + (9.666666666666666f * fVec1[(IOTA-96)&255])) + (9.0f * fVec1[(IOTA-98)&255])) + (8.333333333333334f * fVec1[(IOTA-100)&255])) + (7.333333333333333f * fVec1[(IOTA-102)&255])) + (6.666666666666667f * fVec1[(IOTA-104)&255])) + (6.0f * fVec1[(IOTA-106)&255])) + (5.333333333333333f * fVec1[(IOTA-108)&255])) + (4.0f * fVec1[(IOTA-112)&255])) + (4.666666666666667f * fVec1[(IOTA-110)&255])) + (3.3333333333333335f * fVec1[(IOTA-114)&255])) + (3.0f * fVec1[(IOTA-116)&255])) + (2.3333333333333335f * fVec1[(IOTA-120)&255])) + (1.6666666666666667f * fVec1[(IOTA-122)&255])) + (2.6666666666666665f * fVec1[(IOTA-118)&255]))) + (0.3333333333333333f * fVec1[(IOTA-121)&255]))) - ((((((((((((((10.666666666666666f * fVec1[(IOTA-95)&255]) + (12.0f * fVec1[(IOTA-93)&255])) + (9.333333333333334f * fVec1[(IOTA-97)&255])) + (8.333333333333334f * fVec1[(IOTA-99)&255])) + (7.666666666666667f * fVec1[(IOTA-101)&255])) + (6.666666666666667f * fVec1[(IOTA-103)&255])) + (6.0f * fVec1[(IOTA-105)&255])) + (5.0f * fVec1[(IOTA-107)&255])) + (4.333333333333333f * fVec1[(IOTA-109)&255])) + (3.6666666666666665f * fVec1[(IOTA-111)&255])) + (3.0f * fVec1[(IOTA-113)&255])) + (2.3333333333333335f * fVec1[(IOTA-115)&255])) + (1.3333333333333333f * fVec1[(IOTA-117)&255])) + (0.3333333333333333f * fVec1[(IOTA-119)&255]))))) + (1.2f * fVec1[(IOTA-133)&255])) + (1.4f * fVec1[(IOTA-135)&255])) + (1.6f * fVec1[(IOTA-137)&255])) + (1.8f * fVec1[(IOTA-139)&255])) + (2.0f * fVec1[(IOTA-141)&255])) + (2.2f * fVec1[(IOTA-143)&255])) + (2.4f * fVec1[(IOTA-145)&255])) + (2.7f * fVec1[(IOTA-147)&255])) + (2.9f * fVec1[(IOTA-149)&255])) + (3.4f * fVec1[(IOTA-153)&255])) + (3.1f * fVec1[(IOTA-151)&255]))));
    float fTemp5 = (0.000701904296875f * fVec0[(IOTA-111)&255]);
    float fTemp6 = (0.000213623046875f * fVec0[(IOTA-112)&255]);
    float fTemp7 = (0.000244140625f * ((fVec0[(IOTA-110)&255] + ((fVec0[(IOTA-108)&255] + (((1.375f * fVec0[(IOTA-105)&255]) + (0.75f * fVec0[(IOTA-106)&255])) + (2.125f * fVec0[(IOTA-107)&255]))) + (2.625f * fVec0[(IOTA-109)&255]))) - ((((0.25f * fVec0[(IOTA-103)&255]) + (3.25f * fVec0[(IOTA-101)&255])) + (1.875f * fVec0[(IOTA-102)&255])) + (4.625f * (fVec0[(IOTA-100)&255] + (((((fVec0[(IOTA-95)&255] + ((((((0.6756756756756757f * fVec0[(IOTA-91)&255]) + (0.2702702702702703f * fVec0[(IOTA-90)&255])) + (0.6486486486486487f * (fVec0[(IOTA-89)&255] + ((((fVec0[(IOTA-87)&255] + (1.0416666666666667f * fVec0[(IOTA-85)&255])) + (0.16666666666666666f * fVec0[(IOTA-86)&255])) + (0.125f * (fVec0[(IOTA-84)&255] + ((fVec0[(IOTA-82)&255] + (((((((2.3333333333333335f * fVec0[(IOTA-78)&255]) + (11.666666666666666f * fVec0[(IOTA-79)&255])) + (4.333333333333333f * fVec0[(IOTA-76)&255])) + (14.0f * fVec0[(IOTA-77)&255])) + (17.0f * ((fVec0[(IOTA-75)&255] + (((((fVec0[(IOTA-70)&255] + ((((((1.7254901960784315f * fVec0[(IOTA-66)&255]) + (0.5490196078431373f * fVec0[(IOTA-64)&255])) + (2.196078431372549f * fVec0[(IOTA-65)&255])) + (2.372549019607843f * fVec0[(IOTA-67)&255])) + (1.411764705882353f * fVec0[(IOTA-68)&255])) + (1.9803921568627452f * fVec0[(IOTA-69)&255]))) + (1.5686274509803921f * fVec0[(IOTA-71)&255])) + (0.6470588235294118f * fVec0[(IOTA-72)&255])) + (1.2549019607843137f * fVec0[(IOTA-73)&255])) + (0.4117647058823529f * fVec0[(IOTA-74)&255]))) - ((((((((2.2745098039215685f * fVec0[(IOTA-63)&255]) + (92.94117647058823f * fVec0[(IOTA-57)&255])) + (68.15686274509804f * fVec0[(IOTA-58)&255])) + (47.13725490196079f * fVec0[(IOTA-59)&255])) + (52.35294117647059f * fVec0[(IOTA-60)&255])) + (57.0f * fVec0[(IOTA-61)&255])) + (14.882352941176471f * fVec0[(IOTA-62)&255])) + (12.941176470588236f * ((fVec0[(IOTA-56)&255] + ((((((((((((((fVec0[(IOTA-38)&255] + (((((((((((((((((((((((((0.1606060606060606f * fVec0[(IOTA-7)&255]) + (0.1787878787878788f * fVec0[(IOTA-9)&255])) + (0.2f * fVec0[(IOTA-11)&255])) + (0.12424242424242424f * fVec0[(IOTA-1)&255])) + (0.14696969696969697f * fVec0[(IOTA-5)&255])) + (0.13333333333333333f * fVec0[(IOTA-3)&255])) + (0.22727272727272727f * fVec0[(IOTA-13)&255])) + (0.26515151515151514f * fVec0[(IOTA-15)&255])) + (0.3151515151515151f * fVec0[(IOTA-17)&255])) + (1.5484848484848486f * fVec0[(IOTA-25)&255])) + (0.3924242424242424f * fVec0[(IOTA-19)&255])) + (0.5196969696969697f * fVec0[(IOTA-21)&255])) + (0.7757575757575758f * fVec0[(IOTA-23)&255])) + (3.25f * fVec0[(IOTA-30)&255])) + (6.265151515151516f * fVec0[(IOTA-28)&255])) + (1.2393939393939395f * fVec0[(IOTA-29)&255])) + (1.1242424242424243f * fVec0[(IOTA-31)&255])) + (2.2075757575757575f * fVec0[(IOTA-32)&255])) + (0.6530303030303031f * fVec0[(IOTA-35)&255])) + (0.8636363636363636f * fVec0[(IOTA-33)&255])) + (1.628787878787879f * fVec0[(IOTA-34)&255])) + (1.256060606060606f * fVec0[(IOTA-36)&255])) + (0.49696969696969695f * fVec0[(IOTA-37)&255])) + (0.28939393939393937f * fVec0[(IOTA-41)&255])) + (0.3787878787878788f * fVec0[(IOTA-39)&255]))) + (0.8166666666666667f * fVec0[(IOTA-40)&255])) + (0.49393939393939396f * fVec0[(IOTA-46)&255])) + (0.6787878787878788f * fVec0[(IOTA-42)&255])) + (0.2196969696969697f * fVec0[(IOTA-43)&255])) + (0.5742424242424242f * fVec0[(IOTA-44)&255])) + (0.16515151515151516f * fVec0[(IOTA-45)&255])) + (0.12121212121212122f * fVec0[(IOTA-47)&255])) + (0.4318181818181818f * fVec0[(IOTA-48)&255])) + (0.08181818181818182f * fVec0[(IOTA-49)&255])) + (0.38636363636363635f * fVec0[(IOTA-50)&255])) + (0.04393939393939394f * fVec0[(IOTA-51)&255])) + (0.3606060606060606f * fVec0[(IOTA-52)&255])) + (0.37727272727272726f * fVec0[(IOTA-54)&255]))) - (((((((((((((((((0.07727272727272727f * fVec0[IOTA&255]) + (0.12727272727272726f * fVec0[(IOTA-8)&255])) + (0.14696969696969697f * fVec0[(IOTA-10)&255])) + (0.11212121212121212f * fVec0[(IOTA-6)&255])) + (0.08787878787878788f * fVec0[(IOTA-2)&255])) + (0.09848484848484848f * fVec0[(IOTA-4)&255])) + (0.1712121212121212f * fVec0[(IOTA-12)&255])) + (0.20303030303030303f * fVec0[(IOTA-14)&255])) + (0.24545454545454545f * fVec0[(IOTA-16)&255])) + (0.30757575757575756f * fVec0[(IOTA-18)&255])) + (0.40606060606060607f * fVec0[(IOTA-20)&255])) + (0.5818181818181818f * fVec0[(IOTA-22)&255])) + (3.0787878787878786f * fVec0[(IOTA-26)&255])) + (0.9924242424242424f * fVec0[(IOTA-24)&255])) + (49.377272727272725f * fVec0[(IOTA-27)&255])) + (0.15303030303030302f * fVec0[(IOTA-55)&255])) + (0.006060606060606061f * fVec0[(IOTA-53)&255])))))))) + (1.3333333333333333f * fVec0[(IOTA-80)&255])) + (10.333333333333334f * fVec0[(IOTA-81)&255]))) + (9.0f * fVec0[(IOTA-83)&255]))))) + (0.2916666666666667f * fVec0[(IOTA-88)&255]))))) + (0.40540540540540543f * fVec0[(IOTA-92)&255])) + (0.7837837837837838f * fVec0[(IOTA-93)&255])) + (0.6216216216216216f * fVec0[(IOTA-94)&255]))) + (1.162162162162162f * fVec0[(IOTA-96)&255])) + (1.4594594594594594f * fVec0[(IOTA-97)&255])) + (1.2972972972972974f * fVec0[(IOTA-98)&255])) + (1.2162162162162162f * fVec0[(IOTA-99)&255])))))));
    float fTemp8 = (0.00018310546875f * fVec0[(IOTA-114)&255]);
    float fTemp9 = (0.000823974609375f * fVec0[(IOTA-115)&255]);
    float fTemp10 = (0.0001220703125f * fVec0[(IOTA-116)&255]);
    float fTemp11 = (0.0008544921875f * fVec0[(IOTA-117)&255]);
    float fTemp12 = (9.1552734375e-05f * fVec0[(IOTA-118)&255]);
    float fTemp13 = (0.00091552734375f * fVec0[(IOTA-119)&255]);
    float fTemp14 = (3.0517578125e-05f * fVec0[(IOTA-120)&255]);
    float fTemp15 = (0.0009765625f * fVec0[(IOTA-121)&255]);
    float fTemp16 = (0.001007080078125f * fVec0[(IOTA-123)&255]);
    float fTemp17 = (0.001068115234375f * fVec0[(IOTA-125)&255]);
    float fTemp18 = (0.001129150390625f * fVec0[(IOTA-127)&255]);
    float fTemp19 = (0.001190185546875f * fVec0[(IOTA-129)&255]);
    float fTemp20 = (0.001251220703125f * fVec0[(IOTA-131)&255]);
    float fTemp21 = (0.001312255859375f * fVec0[(IOTA-133)&255]);
    float fTemp22 = (0.00146484375f * fVec0[(IOTA-137)&255]);
    float fTemp23 = (0.00140380859375f * fVec0[(IOTA-135)&255]);
    float fTemp24 = (0.00152587890625f * fVec0[(IOTA-139)&255]);
    float fTemp25 = (0.001617431640625f * fVec0[(IOTA-141)&255]);
    float fTemp26 = (0.001678466796875f * fVec0[(IOTA-143)&255]);
    float fTemp27 = (0.0023193359375f * fVec0[(IOTA-153)&255]);
    float fTemp28 = (0.001800537109375f * fVec0[(IOTA-145)&255]);
    float fTemp29 = (0.00201416015625f * fVec0[(IOTA-149)&255]);
    float fTemp30 = (0.00189208984375f * fVec0[(IOTA-147)&255]);
    float fTemp31 = (0.002166748046875f * fVec0[(IOTA-151)&255]);
    float fTemp32 = (0.00054931640625f * fVec0[(IOTA-138)&255]);
    float fTemp33 = (9.1552734375e-05f * fVec0[(IOTA-124)&255]);
    float fTemp34 = (3.0517578125e-05f * fVec0[(IOTA-122)&255]);
    float fTemp35 = (0.000152587890625f * fVec0[(IOTA-126)&255]);
    float fTemp36 = (0.000213623046875f * fVec0[(IOTA-128)&255]);
    float fTemp37 = (0.000274658203125f * fVec0[(IOTA-130)&255]);
    float fTemp38 = (0.000335693359375f * fVec0[(IOTA-132)&255]);
    float fTemp39 = (0.000396728515625f * fVec0[(IOTA-134)&255]);
    float fTemp40 = (0.00048828125f * fVec0[(IOTA-136)&255]);
    float fTemp41 = (0.000640869140625f * fVec0[(IOTA-140)&255]);
    float fTemp42 = (0.000762939453125f * fVec0[(IOTA-142)&255]);
    float fTemp43 = (0.0009765625f * fVec0[(IOTA-146)&255]);
    float fTemp44 = (0.000885009765625f * fVec0[(IOTA-144)&255]);
    float fTemp45 = (0.0010986328125f * fVec0[(IOTA-148)&255]);
    float fTemp46 = (0.001251220703125f * fVec0[(IOTA-150)&255]);
    float fTemp47 = (0.00140380859375f * fVec0[(IOTA-152)&255]);
    output0[i] = ((fTemp47 + (fTemp46 + (fTemp45 + (fTemp44 + (fTemp43 + (fTemp42 + (fTemp41 + (fTemp40 + ((fTemp39 + (fTemp38 + (fTemp37 + (fTemp36 + (fTemp35 + (fTemp34 + fTemp33)))))) + fTemp32))))))))) - (fTemp31 + (fTemp30 + (fTemp29 + (fTemp28 + (fTemp27 + (fTemp26 + (fTemp25 + (fTemp24 + (fTemp23 + (fTemp22 + (fTemp21 + (fTemp20 + (fTemp19 + (fTemp18 + (fTemp17 + (fTemp16 + (fTemp15 + (fTemp14 + (fTemp13 + (fTemp12 + (fTemp11 + (fTemp10 + (fTemp9 + (fTemp8 + (fTemp7 + (fTemp6 + (fTemp5 + (fTemp4 + fTemp1)))))))))))))))))))))))))))));
    output1[i] = ((fTemp31 + (fTemp30 + (fTemp29 + (fTemp28 + (fTemp27 + (fTemp26 + (fTemp25 + (fTemp24 + (fTemp23 + ((fTemp21 + (((fTemp18 + (fTemp17 + ((((((((((fTemp7 + (fTemp6 + (fTemp5 + fTemp1))) + fTemp8) + fTemp9) + fTemp10) + fTemp11) + fTemp12) + fTemp13) + fTemp14) + fTemp15) + fTemp16))) + fTemp19) + fTemp20)) + fTemp22)))))))))) - (fTemp47 + (fTemp46 + (fTemp45 + (fTemp44 + (fTemp43 + (fTemp42 + (fTemp41 + (fTemp40 + (fTemp32 + (fTemp39 + (fTemp38 + (fTemp37 + (fTemp36 + (fTemp35 + (fTemp33 + (fTemp4 + fTemp34)))))))))))))))));
    // post processing
    IOTA = IOTA+1;
  }
}

int main(int argc, char **argv)
{
  const unsigned buf_length = 256;
  const unsigned long sample_rate = 44100;
  const unsigned n_out_ch = 2;
  const unsigned n_sph_harm = 3;

  /* allocate buffers */
  float *in = malloc(buf_length*sizeof(float));
  float **wxy = malloc(n_sph_harm*sizeof(float *));
  {
    unsigned i;
    for (i = 0; i < n_sph_harm; i++)
      {
	wxy[i] = malloc(buf_length * sizeof(float));
      }
  }
  float **outf = malloc(n_out_ch * sizeof(float *));
  {
    unsigned i;
    for (i = 0; i < n_out_ch; i++)
      {
	outf[i] = malloc(buf_length * sizeof(float));
      }
  }
  short *outs = malloc(n_out_ch * buf_length * sizeof(short));

  /* get params */
  FILE *f_out = fopen(argv[1], "wb");
  int length = atoi(argv[2]);

  /* process loop */
  int count = length;
  while (count > 0)
    {
      /* ramp azimuth from 0 to 2PI */
      float azi = 2*M_PI * (1 - (count / (double)length));
      synthesize_pulse(in, buf_length, sample_rate);
      encode_wxy(buf_length, azi, 0, in, wxy);
      compute(buf_length, wxy, outf);
      float_to_16bit_int(outf, outs, n_out_ch, buf_length);
      fwrite(outs, sizeof(short), n_out_ch*buf_length, f_out);
      count -= buf_length;
    }

  fclose(f_out);
  
  /* deallocate buffers */
  {
    unsigned i;
    for (i = 0; i < n_out_ch; i++)
      {
	free(outf[i]);
      }
    for (i = 0; i < n_sph_harm; i++)
      {
	free(wxy[i]);
      }
  }
  free(outf);
  free(outs);
  free(wxy);
  free(in);

  return 0;
}

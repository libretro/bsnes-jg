/* Name of package */
#define PACKAGE "libsamplerate"

/* Version number of package */
#define VERSION "0.2.2"

/* Target processor clips on negative float to int conversion. */
#define CPU_CLIPS_NEGATIVE 0

/* Target processor clips on positive float to int conversion. */
#define CPU_CLIPS_POSITIVE 0

/* Target processor is big endian. */
#define CPU_IS_BIG_ENDIAN 0

/* Target processor is little endian. */
#define CPU_IS_LITTLE_ENDIAN 1

/* Define to 1 if you have the `alarm' function. */
#define HAVE_ALARM 1

/* Set to 1 if you have libfftw3. */
#define HAVE_FFTW3 0

/* Define if you have signal SIGALRM. */
#define HAVE_SIGALRM 1

/* Define to 1 if you have the `signal' function. */
#define HAVE_SIGNAL 1

/* Define to 1 if you have the <stdbool.h> header file. */
#define HAVE_STDBOOL_H

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 0

/* Define to 1 if you have the <sys/times.h> header file. */
#define HAVE_SYS_TIMES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H

/* Define to 1 if the compiler supports simple visibility declarations. */
#define HAVE_VISIBILITY

/* define fast samplerate convertor */
#define ENABLE_SINC_FAST_CONVERTER

/* define balanced samplerate convertor */
#define ENABLE_SINC_MEDIUM_CONVERTER

/* define best samplerate convertor */
#define ENABLE_SINC_BEST_CONVERTER

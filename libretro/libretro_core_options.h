#ifndef LIBRETRO_CORE_OPTIONS_H__
#define LIBRETRO_CORE_OPTIONS_H__

#include <stdlib.h>
#include <string.h>

#include <libretro.h>
#include <retro_inline.h>

#ifndef HAVE_NO_LANGEXTRA
#include "libretro_core_options_intl.h"
#endif

/*
 ********************************
 * VERSION: 1.3
 ********************************
 *
 * - 1.3: Move translations to libretro_core_options_intl.h
 *        - libretro_core_options_intl.h includes BOM and utf-8
 *          fix for MSVC 2010-2013
 *        - Added HAVE_NO_LANGEXTRA flag to disable translations
 *          on platforms/compilers without BOM support
 * - 1.2: Use core options v1 interface when
 *        RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION is >= 1
 *        (previously required RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION == 1)
 * - 1.1: Support generation of core options v0 retro_core_option_value
 *        arrays containing options with a single value
 * - 1.0: First commit
*/

#ifdef __cplusplus
extern "C" {
#endif

/*
 ********************************
 * Core Option Definitions
 ********************************
*/

/* RETRO_LANGUAGE_ENGLISH */

/* Default language:
 * - All other languages must include the same keys and values
 * - Will be used as a fallback in the event that frontend language
 *   is not available
 * - Will be used as a fallback for any missing entries in
 *   frontend language definition */

struct retro_core_option_definition option_defs_us[] = {
   {
      "bsnes_jg_coproc_delaysync",
      "Delay LLE Coprocessor Sync (Restart)",
      "Delay sync when using Low Level Coprocessor Emulation for more speed at the cost of accuracy",
      {
         { "off", "Off (Default)" },
         { "on", "On" },
         { NULL, NULL },
      },
      "off"
   },
   {
      "bsnes_jg_coproc_preferhle",
      "Prefer HLE Coprocessor Emulation (Restart)",
      "Use High Level Coprocessor Emulation when possible for more speed at the cost of accuracy",
      {
         { "off", "Off (Default)" },
         { "on", "On" },
         { NULL, NULL },
      },
      "off"
   },
   {
      "bsnes_jg_hotfixes",
      "Hotfixes (Restart)",
      "Apply hotfixes when playing a handful of games released with major bugs "
      "(which are exhibited on real hardware). Games in question: Dirt Racer, "
      "Magical Drop, Rendering Ranger R2",
      {
         { "off", "Off (Default)" },
         { "on", "On" },
         { NULL, NULL },
      },
      "off"
   },
   {
      "bsnes_jg_runahead",
      "Internal Run-Ahead",
      "Simulate the system ahead of time and roll back to reduce input latency. Has very high system requirements.",
      {
         { "0",   "0 (Default)" },
         { "1",   "1 frame"  },
         { "2",   "2 frames" },
         { "3",   "3 frames" },
         { "4",   "4 frames" },
         { NULL, NULL },
      },
      "0"
   },
   {
      "bsnes_jg_rsqual",
      "Resampler Quality (Restart)",
      "Set the internal resampler's quality level (you may hear a difference if you use pro audio equipment "
      "and your imagination)",
      {
         { "fast", "Fast (Default)" },
         { "medium", "Medium" },
         { "best", "Best" },
         { NULL, NULL },
      },
      "fast"
   },
   {
      "bsnes_jg_spc_interp",
      "SPC Interpolation Algorithm",
      "Set the emulated sound chip sample interpolation algorithm: Gaussian is considered more accurate, "
      "while Sinc is cleaner, but still produces very accurate output.",
      {
         { "gaussian", "Gaussian (Default)" },
         { "sinc", "Sinc" },
         { NULL, NULL },
      },
      "gaussian"
   },
   {
      "bsnes_jg_aspect",
      "Aspect Ratio",
      "Set the Aspect Ratio",
      {
         { "Auto", "Region-based Pixel Aspect Ratio (Default)" },
         { "1:1", "Perfectly Square Pixels (1:1 PAR)" },
         { "8:7", "Ostensibly Accurate NTSC Aspect Ratio (8:7 PAR)" },
         { "11:8", "Ostensibly Accurate PAL Aspect Ratio (~11:8 PAR)" },
         { "4:3", "Very Traditional NTSC Aspect Ratio (4:3 DAR)" },
         { NULL, NULL },
      },
      "Auto"
   },
   {
      "bsnes_jg_overscan_t",
      "Mask Overscan (Top)",
      "Mask off pixels hidden by a bezel or border on original CRTs (top)",
      {
         { "0", NULL },
         { "4", NULL },
         { "8", "8 (Default)" },
         { "12", NULL },
         { "16", NULL },
         { "20", NULL },
         { NULL, NULL },
      },
      "8"
   },
   {
      "bsnes_jg_overscan_b",
      "Mask Overscan (Bottom)",
      "Mask off pixels hidden by a bezel or border on original CRTs (bottom)",
      {
         { "0", NULL },
         { "4", NULL },
         { "8", "8 (Default)" },
         { "12", NULL },
         { "16", NULL },
         { "20", NULL },
         { "21", NULL },
         { NULL, NULL },
      },
      "8"
   },
   {
      "bsnes_jg_overscan_l",
      "Mask Overscan (Left)",
      "Mask off pixels hidden by a bezel or border on original CRTs (left)",
      {
         { "0", "0 (Default)" },
         { "4", NULL },
         { "8", NULL },
         { NULL, NULL },
      },
      "0"
   },
   {
      "bsnes_jg_overscan_r",
      "Mask Overscan (Right)",
      "Mask off pixels hidden by a bezel or border on original CRTs (right)",
      {
         { "0", "0 (Default)" },
         { "4", NULL },
         { "8", NULL },
         { NULL, NULL },
      },
      "0"
   },
   {
      "bsnes_jg_luminance",
      "Colour Adjustment - Luminance",
      "Adjust Luminance",
      {
         { "0", "0%" },
         { "10", "10%" },
         { "20", "20%" },
         { "30", "30%" },
         { "40", "40%" },
         { "50", "50%" },
         { "60", "60%" },
         { "70", "70%" },
         { "80", "80%" },
         { "90", "90%" },
         { "100", "100% (Default)" },
         { NULL, NULL },
      },
      "100"
   },
   {
      "bsnes_jg_saturation",
      "Colour Adjustment - Saturation",
      "Adjust Saturation",
      {
         { "0", "0%" },
         { "10", "10%" },
         { "20", "20%" },
         { "30", "30%" },
         { "40", "40%" },
         { "50", "50%" },
         { "60", "60%" },
         { "70", "70%" },
         { "80", "80%" },
         { "90", "90%" },
         { "100", "100% (Default)" },
         { "110", "110%" },
         { "120", "120%" },
         { "130", "130%" },
         { "140", "140%" },
         { "150", "150%" },
         { "160", "160%" },
         { "170", "170%" },
         { "180", "180%" },
         { "190", "190%" },
         { "200", "200%" },
         { NULL, NULL },
      },
      "100"
   },
   {
      "bsnes_jg_gamma",
      "Colour Adjustment - Gamma",
      "Adjust Gamma",
      {
         { "100", "100%" },
         { "110", "110%" },
         { "120", "120% (Default)" },
         { "130", "130%" },
         { "140", "140%" },
         { "150", "150%" },
         { "160", "160%" },
         { "170", "170%" },
         { "180", "180%" },
         { "190", "190%" },
         { "200", "200%" },
         { NULL, NULL },
      },
      "120"
   },
   {
      "bsnes_jg_competition_timer",
      "Competition Timer",
      "Set the gameplay time for competition boards such as Campus Challenge '92 "
      "and PowerFest '94.",
      {
         { "3", "3 Minutes" },
         { "4", "4 Minutes" },
         { "5", "5 Minutes" },
         { "6", "6 Minutes (Default)" },
         { "7", "7 Minutes" },
         { "8", "8 Minutes" },
         { "9", "9 Minutes" },
         { "10", "10 Minutes" },
         { "11", "11 Minutes" },
         { "12", "12 Minutes" },
         { "13", "13 Minutes" },
         { "14", "14 Minutes" },
         { "15", "15 Minutes" },
         { "16", "16 Minutes" },
         { "17", "17 Minutes" },
         { "18", "18 Minutes" },
         { NULL, NULL },
      },
      "6"
   },
   { NULL, NULL, NULL, {{0}}, NULL },
};

/*
 ********************************
 * Language Mapping
 ********************************
*/

#ifndef HAVE_NO_LANGEXTRA
struct retro_core_option_definition *option_defs_intl[RETRO_LANGUAGE_LAST] = {
   option_defs_us, /* RETRO_LANGUAGE_ENGLISH */
   NULL,           /* RETRO_LANGUAGE_JAPANESE */
   NULL,           /* RETRO_LANGUAGE_FRENCH */
   NULL,           /* RETRO_LANGUAGE_SPANISH */
   NULL,           /* RETRO_LANGUAGE_GERMAN */
   NULL,           /* RETRO_LANGUAGE_ITALIAN */
   NULL,           /* RETRO_LANGUAGE_DUTCH */
   NULL,           /* RETRO_LANGUAGE_PORTUGUESE_BRAZIL */
   NULL,           /* RETRO_LANGUAGE_PORTUGUESE_PORTUGAL */
   NULL,           /* RETRO_LANGUAGE_RUSSIAN */
   NULL,           /* RETRO_LANGUAGE_KOREAN */
   NULL,           /* RETRO_LANGUAGE_CHINESE_TRADITIONAL */
   NULL,           /* RETRO_LANGUAGE_CHINESE_SIMPLIFIED */
   NULL,           /* RETRO_LANGUAGE_ESPERANTO */
   NULL,           /* RETRO_LANGUAGE_POLISH */
   NULL,           /* RETRO_LANGUAGE_VIETNAMESE */
   NULL,           /* RETRO_LANGUAGE_ARABIC */
   NULL,           /* RETRO_LANGUAGE_GREEK */
   NULL,           /* RETRO_LANGUAGE_TURKISH */
   NULL,           /* RETRO_LANGUAGE_SLOVAK */
   NULL,           /* RETRO_LANGUAGE_PERSIAN */
   NULL,           /* RETRO_LANGUAGE_HEBREW */
   NULL,           /* RETRO_LANGUAGE_ASTURIAN */
   NULL,           /* RETRO_LANGUAGE_FINNISH */

};
#endif

/*
 ********************************
 * Functions
 ********************************
*/

/* Handles configuration/setting of core options.
 * Should be called as early as possible - ideally inside
 * retro_set_environment(), and no later than retro_load_game()
 * > We place the function body in the header to avoid the
 *   necessity of adding more .c files (i.e. want this to
 *   be as painless as possible for core devs)
 */

static INLINE void libretro_set_core_options(retro_environment_t environ_cb)
{
   unsigned version = 0;

   if (!environ_cb)
      return;

   if (environ_cb(RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION, &version) && (version >= 1))
   {
#ifndef HAVE_NO_LANGEXTRA
      struct retro_core_options_intl core_options_intl;
      unsigned language = 0;

      core_options_intl.us    = option_defs_us;
      core_options_intl.local = NULL;

      if (environ_cb(RETRO_ENVIRONMENT_GET_LANGUAGE, &language) &&
          (language < RETRO_LANGUAGE_LAST) && (language != RETRO_LANGUAGE_ENGLISH))
         core_options_intl.local = option_defs_intl[language];

      environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_INTL, &core_options_intl);
#else
      environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS, &option_defs_us);
#endif
   }
   else
   {
      size_t i;
      size_t num_options               = 0;
      struct retro_variable *variables = NULL;
      char **values_buf                = NULL;

      /* Determine number of options */
      for (;;)
      {
         if (!option_defs_us[num_options].key)
            break;
         num_options++;
      }

      /* Allocate arrays */
      variables  = (struct retro_variable *)calloc(num_options + 1, sizeof(struct retro_variable));
      values_buf = (char **)calloc(num_options, sizeof(char *));

      if (!variables || !values_buf)
         goto error;

      /* Copy parameters from option_defs_us array */
      for (i = 0; i < num_options; i++)
      {
         const char *key                        = option_defs_us[i].key;
         const char *desc                       = option_defs_us[i].desc;
         const char *default_value              = option_defs_us[i].default_value;
         struct retro_core_option_value *values = option_defs_us[i].values;
         size_t buf_len                         = 3;
         size_t default_index                   = 0;

         values_buf[i] = NULL;

         if (desc)
         {
            size_t num_values = 0;

            /* Determine number of values */
            for (;;)
            {
               if (!values[num_values].value)
                  break;

               /* Check if this is the default value */
               if (default_value)
                  if (strcmp(values[num_values].value, default_value) == 0)
                     default_index = num_values;

               buf_len += strlen(values[num_values].value);
               num_values++;
            }

            /* Build values string */
            if (num_values > 0)
            {
               size_t j;

               buf_len += num_values - 1;
               buf_len += strlen(desc);

               values_buf[i] = (char *)calloc(buf_len, sizeof(char));
               if (!values_buf[i])
                  goto error;

               strcpy(values_buf[i], desc);
               strcat(values_buf[i], "; ");

               /* Default value goes first */
               strcat(values_buf[i], values[default_index].value);

               /* Add remaining values */
               for (j = 0; j < num_values; j++)
               {
                  if (j != default_index)
                  {
                     strcat(values_buf[i], "|");
                     strcat(values_buf[i], values[j].value);
                  }
               }
            }
         }

         variables[i].key   = key;
         variables[i].value = values_buf[i];
      }

      /* Set variables */
      environ_cb(RETRO_ENVIRONMENT_SET_VARIABLES, variables);

error:

      /* Clean up */
      if (values_buf)
      {
         for (i = 0; i < num_options; i++)
         {
            if (values_buf[i])
            {
               free(values_buf[i]);
               values_buf[i] = NULL;
            }
         }

         free(values_buf);
         values_buf = NULL;
      }

      if (variables)
      {
         free(variables);
         variables = NULL;
      }
   }
}

#ifdef __cplusplus
}
#endif

#endif

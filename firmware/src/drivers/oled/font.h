#ifndef _CNETIO_FONT_
#define _CNETIO_FONT_

#include <commendef.h>

typedef struct {
	uint8_t FontWidth;    /*!< Font width in pixels */
	uint8_t FontHeight;   /*!< Font height in pixels */
	const uint16_t *data; /*!< Pointer to data font data array */
} FontDef_t;

typedef struct {
	uint16_t Length;      /*!< String length in units of pixels */
	uint16_t Height;      /*!< String height in units of pixels */
} FONTS_SIZE_t;

extern FontDef_t Font_7x10;

extern FontDef_t Font_11x18;

extern FontDef_t Font_16x26;

#endif
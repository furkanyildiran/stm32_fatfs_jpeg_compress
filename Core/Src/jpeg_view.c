#define JPEG_INTERNALS
#include "jinclude.h"
#include "jpeglib.h"

extern uint16_t dbg_pixel_color;

struct jpeg_decompress_struct cinfo;
typedef struct RGB {
	uint8_t B;
	uint8_t G;
	uint8_t R;
} RGB_typedef;
struct jpeg_error_mgr jerr;

RGB_typedef *RGB_matrix;

uint16_t RGB16PixelColor;
static uint8_t *rowBuff;

static uint8_t jpeg_decode(JFILE *file, uint8_t *rowBuff, int posx, int posy,
		UINT *iw, UINT *ih) {
	uint32_t line_counter = 0;
	uint32_t i = 0, xc = 0, ratio;
	uint8_t offset = 1;
	JSAMPROW buffer[2] = { 0 };

	UINT lcdWidth, lcdHeight;

	buffer[0] = rowBuff;
	lcdWidth = 240;
	lcdHeight = 180;

	cinfo.err = jpeg_std_error(&jerr);

	jpeg_create_decompress(&cinfo);

	jpeg_stdio_src(&cinfo, file);

	jpeg_read_header(&cinfo, TRUE);
	if (cinfo.image_width > 6000) {
//	  showMessage("Image width exceeds 6000!!!", 5);
		return 0;
	}

	if (cinfo.image_width > lcdWidth) {
		ratio = cinfo.image_width / lcdWidth;
		cinfo.scale_num = 1;
		if (ratio <= 8) {
			cinfo.scale_denom = 1;
			for (int s = 0x8; s > 0x01; s /= 2) {
				if (ratio & s) {
					cinfo.scale_denom = s;
					break;
				}
			}
		} else {
			cinfo.scale_denom = 8;
		}
	}

	cinfo.dct_method = JDCT_IFAST;

	jpeg_start_decompress(&cinfo);
	if (cinfo.output_width > lcdWidth) {
		offset = cinfo.output_width / lcdWidth;
		if (cinfo.output_width % lcdWidth > lcdWidth / 4)
			offset++;
	}

	if (posx < 0 || posy < 0) {
		posx = (lcdWidth - (cinfo.output_width * (offset - 1) / offset)) / 2;
		posy = (lcdHeight - (cinfo.output_height * (offset - 1) / offset)) / 2;
	}
	*iw = cinfo.image_width;
	*ih = cinfo.image_height;

	//ILI9341_FillScreen(ILI9341_BLACK);

	if (posx > 0 && cinfo.output_width / offset < lcdWidth) {
		/*ILI9341_FillRectangle(posx - 1, posy - 1,
		 cinfo.output_width / offset + 2,
		 cinfo.output_height / offset + 2, ILI9341_WHITE);*/
	}

	while (cinfo.output_scanline < cinfo.output_height
			&& line_counter < lcdHeight - posy) {
		(void) jpeg_read_scanlines(&cinfo, buffer, 1);
		RGB_matrix = (RGB_typedef*) buffer[0];
		for (i = 0, xc = 0; i < cinfo.output_width && xc < (lcdWidth - posx);
				i += offset, xc++) {
			RGB16PixelColor = (uint16_t) (((RGB_matrix[i].R & 0x00F8) >> 3)
					| ((RGB_matrix[i].G & 0x00FC) << 3)
					| ((RGB_matrix[i].B & 0x00F8) << 8));
			//ILI9341_DrawPixel(xc + posx, line_counter + posy, RGB16PixelColor);

			/**debug_line**/
			dbg_pixel_color = RGB_matrix->R;
			/**debug_line**/

		}
		for (i = 0;
				i < offset - 1 && cinfo.output_scanline < cinfo.output_height;
				i++)
			(void) jpeg_read_scanlines(&cinfo, buffer, 1);

		line_counter++;

	}

	jpeg_finish_decompress(&cinfo);

	jpeg_destroy_decompress(&cinfo);

	return 1;
}

void jpeg_screen_view(char *fn, int px, int py, UINT *iw, UINT *ih) {
	FIL file;
	FATFS fs;

//  uint32_t oldBaudRate;

//char sf[256];

	rowBuff = JMALLOC(2048);

//  sprintf(sf, "%s%s", path, fn);
	if (f_mount(&fs, "", 0) != FR_OK) {
		JFREE(rowBuff);
		return;
	}
	if (f_open(&file, fn, FA_READ) == FR_OK) {
		jpeg_decode(&file, rowBuff, px, py, iw, ih);
		f_close(&file);
	} else {
//	  sprintf(sf, "%s\nFile open Error!!", sf);
		//ILI9341_WriteString(0, 0, "File open Error", Font_7x10, ILI9341_RED,
		//		ILI9341_WHITE);
	}
	f_mount(&fs, "", 0);
	JFREE(rowBuff);
//  HAL_SPI_DeInit(&hspi2);
//	hspi2.Init.BaudRatePrescaler = oldBaudRate;
//	HAL_SPI_Init(&hspi2);

}
uint8_t *jpeg_output_buffer = NULL;  // Başlangıçta boş bir pointer
uint8_t CompressToJPEG_RowByRow(void) {
	struct jpeg_compress_struct compress_cinfo;
	struct jpeg_error_mgr compress_jerr;

	// JPEG sıkıştırmayı başlat
	compress_cinfo.err = jpeg_std_error(&compress_jerr);
	jpeg_create_compress(&compress_cinfo);

	unsigned long jpeg_size = 0;

	jpeg_mem_dest(&compress_cinfo, &jpeg_output_buffer, &jpeg_size);

	// Görüntü parametreleri
	compress_cinfo.image_width = 60;   // Genişlik
	compress_cinfo.image_height = 30;  // Yükseklik
	compress_cinfo.input_components = 3; // RGB888 formatında her pikselde 3 bileşen
	compress_cinfo.in_color_space = JCS_RGB;  // Renk uzayı

	jpeg_set_defaults(&compress_cinfo);
	jpeg_set_quality(&compress_cinfo, 75, TRUE);  // Kalite %75

	jpeg_start_compress(&compress_cinfo, TRUE);

	// Her satır için dummy veri (RGB888 formatında)
	uint8_t b[60 * 3];
	for (int i = 0; i < 60 * 3; i++) {
		b[i] = 200;  // Her r, g, b kanalı için sabit bir değer
	}

	for (uint16_t row = 0; row < 30; row++) {
		JSAMPROW rowPointer[1] = { b };
		jpeg_write_scanlines(&compress_cinfo, rowPointer, 1);
	}

	// Sıkıştırmayı bitir
	jpeg_finish_compress(&compress_cinfo);
	jpeg_destroy_compress(&compress_cinfo);

	FIL file;
	FRESULT res;
	FATFS fs;
	// Dosya sistemi monte et
	res = f_mount(&fs, "", 1);
	if (res != FR_OK) {
		return 1;
	}

	// SD karta yaz
	res = f_open(&file, "img.jpg", FA_WRITE | FA_CREATE_ALWAYS);
	if (res != FR_OK) {
		//printf("Dosya açılamadı! Hata kodu: %d\n", res);
		return 2;
	}

	UINT bytesWritten;
	res = f_write(&file, jpeg_output_buffer, jpeg_size, &bytesWritten);
	if (res != FR_OK || bytesWritten != jpeg_size) {
		//printf("JPEG yazılamadı! Hata kodu: %d\n", res);
		return 3;
	} else {
		//printf("JPEG başarıyla kaydedildi.\n");
	}

	f_close(&file);

	return 0;
}


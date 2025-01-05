#ifndef INC_JPEG_VIEW_H_
#define INC_JPEG_VIEW_H_

void jpeg_screen_view(char* fn, int px, int py, UINT *iw, UINT *ih);
uint8_t CompressToJPEG_RowByRow(void);

#endif /* INC_JPEG_VIEW_H_ */

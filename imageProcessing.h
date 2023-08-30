#define LFT_R_ORIGIN  115
#define LFT_C_ORIGIN  80
#define LFT_WIDTH   280
#define LFT_HEIGHT   70
#define LFT_LENBUF  LFT_WIDTH * LFT_HEIGHT
#define TILE_SIZE       10
#define CLIP_LIMIT      0.03 

void reset_imgprcs_status(void);
void imgprcs_generateImage(unsigned char *, unsigned int , unsigned int, unsigned int);
void imgprcs_start(unsigned char *, unsigned int , unsigned int, unsigned int);
int imgprcs_adapthisteq(unsigned char*, unsigned int, unsigned int, unsigned int);
int imgprcs_medianFilter3x3(unsigned char *, unsigned int , unsigned int, unsigned int);
int imgprcs_lineDetection(unsigned char *, unsigned int , unsigned int);
int imgprcs_testResult(unsigned char *, unsigned int);
void SerialLog_image(unsigned char*, unsigned int, unsigned int, unsigned int,unsigned int );

extern int imgprcs_status ;
extern unsigned char testResult;
extern unsigned char* ROI_finalImage;

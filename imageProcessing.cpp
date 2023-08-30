#include <Arduino.h>
#include "imageProcessing.h"
#include "LCD_display.h"


int imgprcs_status =1 ;
unsigned char testResult;
unsigned char* ROI_finalImage;

void reset_imgprcs_status(void)
{
	imgprcs_status=1;
}

void imgprcs_generateImage( unsigned char* buf, unsigned int len, unsigned int width, unsigned int height)
{
  unsigned char* temp_croppedImage = (unsigned char*)malloc(LFT_LENBUF * sizeof(unsigned char));
  ROI_finalImage = (unsigned char*)malloc(LFT_LENBUF * sizeof(unsigned char));

  
  unsigned int index=0;
  
  // ROI  --CL----TL--
  for( int r= LFT_R_ORIGIN ; r< LFT_R_ORIGIN+LFT_HEIGHT; r++)
  {
    for (int c=LFT_C_ORIGIN; c< LFT_C_ORIGIN+LFT_WIDTH; c++)
    {
      temp_croppedImage[index]= buf[ (r*width) + c ];
      index+=1;
    }
  }

 
  //SerialLog_image(temp_croppedImage,IMG_LENBUF,IMG_WIDTH,5,IMG_WIDTH);

  // copying to ROI final
  for (int i = 0; i < LFT_LENBUF; i++) {
        ROI_finalImage[i] = temp_croppedImage[i];
    }

  
  free(temp_croppedImage);
  Serial.println("Cropped initial image");
  SerialLog_image(ROI_finalImage,LFT_LENBUF,LFT_WIDTH,5,LFT_WIDTH);
  imgprcs_start(ROI_finalImage,LFT_LENBUF,LFT_WIDTH,LFT_HEIGHT);
}

void imgprcs_start(unsigned char* image, unsigned int len, unsigned int width, unsigned int height)
{
	imgprcs_status= 1;
  Serial.println("imgprcs started");
  switch(imgprcs_status)
  {
	case 1:
		Serial.println("PASS1 started");
		imgprcs_status = imgprcs_adapthisteq(image, len, width, height);
		if (imgprcs_status != 2)
		{
			Serial.println("Failed attempt: adapthisteq");
			lcd_displayFailed();
			break;
		}
	case 2:
		Serial.println("PASS2 started");
		imgprcs_status = imgprcs_medianFilter3x3(image, len, width, height);
		if (imgprcs_status != 3)
		{
			Serial.println("Failed attempt: medianfilter");
			lcd_displayFailed();
			break;
		}
	case 3:
    Serial.println("PASS3 started");
		imgprcs_status = imgprcs_lineDetection(image, width, height);
		if (imgprcs_status != 4)
		{
			Serial.println("Failed attempt: linedetection");
			lcd_displayFailed();
			break;
		}
	case 4:
		Serial.println("PASS4 started");
		imgprcs_status = imgprcs_testResult(image, width);
		if (imgprcs_status != 1)
		{
			Serial.println("Failed attempt: could not evaluate result");
			lcd_displayFailed();
			break;
		}
   else
   {
      Serial.println("PASSED ALL");
      break;    
   }
	default:
		Serial.println("Failed attempt: Unknown");
		lcd_displayFailed();
		break;
  }
}


int imgprcs_adapthisteq(unsigned char* image, unsigned int len, unsigned int width, unsigned int height)
{
	  int x,y,r,c ;
    double clipLimit = TILE_SIZE * TILE_SIZE * CLIP_LIMIT;
    double excess = 0.0;
    double redistribute = 0.0;
    int pixelValue = 0;
    double newPixelValue = 0.0;
    
    for(r=0 ; r< height; r+= TILE_SIZE )
    {
        for (c=0 ; c< width ; c+= TILE_SIZE)
        {
            double histogram_distribution[256] = {0};
            for (y=r ; y< r+ TILE_SIZE ; y++)
            {
                for(x=c ; x< c+TILE_SIZE ; x++)
                {
                    histogram_distribution[image[(y*width) +x]]++ ;
                }
            }
        // Clip histogram
            
            excess = 0;
            for (x = 0; x < 256; x++)
            {
                if (histogram_distribution[x] > clipLimit) 
                {
                    excess += histogram_distribution[x] - clipLimit;
                    histogram_distribution[x] = clipLimit;
                }
            }
            // Redistribute excess counts
            redistribute =  excess / 256.0;
            for (x = 0; x < 256; x++)
            {
                histogram_distribution[x] += redistribute;
            }
            
            for(y=r ; y< r+TILE_SIZE ; y++ )
            {
                for(x=c ; x< c+TILE_SIZE ; x++)
                {
                    pixelValue = image[(y*width) + x];
                    newPixelValue = 0;
                    for (int z=0 ; z<pixelValue ; z++)
                    {
                        newPixelValue += histogram_distribution[z];
                    }
                    image[(y*width) + x] = (unsigned char) ( (newPixelValue*255)/(TILE_SIZE * TILE_SIZE) );
                }
            }
        }
    }

	// free unused memory
  Serial.println("PASS1");
  SerialLog_image(image,LFT_LENBUF,LFT_WIDTH,3,LFT_WIDTH);
	return(2);
}

/*
int imgprcs_histeq(unsigned char* image, unsigned int len, unsigned int width, unsigned int height)
{
  int histogram_distribution[256] = {0};
    float cumulative_distribution[256] = {0};
    unsigned char* temp_equalizedImage = (unsigned char*)malloc(len * sizeof(unsigned char));

    int x= imgprcs_adapthisteq(image, len, width, height);


    // Compute histogram
    for (int i = 0; i < len; i++) {
        histogram_distribution[image[i]]++;
    }

    // Compute cumulative distribution
    cumulative_distribution[0] = (float)histogram_distribution[0] / len;
    for (int i = 1; i < 256; i++) {
        cumulative_distribution[i] = cumulative_distribution[i - 1] + ((float)histogram_distribution[i] / (len));
    }

    // Map intensities
    for (int i = 0; i < len; i++) {
        temp_equalizedImage[i] = (unsigned char)(cumulative_distribution[image[i]] * (256 - 1));
    }

    // Replace original image with equalized image
    for (int i = 0; i < len; i++) {
        image[i] = temp_equalizedImage[i];
    }
  
  // free unused memory
    free(temp_equalizedImage);
    SerialLog_image(image,LFT_LENBUF,LFT_WIDTH,2,LFT_WIDTH);
  Serial.println("PASS1");
  return(2);
}
*/
int imgprcs_medianFilter3x3(unsigned char* image, unsigned int len, unsigned int width, unsigned int height)
{
	unsigned char* temp_filteredImage = (unsigned char*)malloc(len * sizeof(unsigned char));
    int r, c, i, j;
    int wndw[9];
    int index;
	
	for (r=1 ; r< height-1 ; r++)
	{
		for (c=1 ; c < width-1 ; c++)
		{
			index =0;
			for( i=-1 ; i<=1 ; i++)
			{
				for ( j=-1 ; j<=1 ; j++)
				{
					wndw[index] = image[ ((r+i)*width) + (c+j) ];
					index+=1;
				}
			}
			
			// Sort
			for (i = 0; i < 8; i++)
			{
			  for (j = 0; j < 8-i; j++)
				{
				  if (wndw[j] > wndw[j+i])
					{
						int temp = wndw[j];
						wndw[j] = wndw[j+1];
						wndw[j+1] = temp;
					}
				}
			}
			temp_filteredImage[ (r*width) + c] = wndw[4];
		}
	}
	
	// Replace original image with filtered image. Does not replace the 1px edge.
    for (r=1 ; r< height-1 ; r++)
	{
		for (c=1 ; c < width-1 ; c++)
		{
			image[ (r*width) + c] = temp_filteredImage[ (r*width) + c];
		}
	}
	
	free(temp_filteredImage);
  SerialLog_image(image,LFT_LENBUF,LFT_WIDTH,1,LFT_WIDTH);
  Serial.println("PASS2");
	return(3);
}

int imgprcs_lineDetection(unsigned char* image, unsigned int width, unsigned int height)
{
	unsigned int col_sum = 0;
	for (int c=0 ; c<width ; c++)
	{
		col_sum = 0;
		for (int r=0 ; r<height ; r++)
		{
			col_sum+= image[ (r*width) + c ];
		}
		
		//col_avg = (int)(row_sum/width);
		
		if ( (col_sum > 10000) || (c < 20) || (c > width-40) )
		{
			for (int r=0 ; r<height ; r++)
			{
				image[ (r*width) + c ] = 255;
			}
		}
		else
		{
			for (int r=0 ; r<height ; r++)
			{
				image[ (r*width) + c ] = 0;
			}
		}
   Serial.print(col_sum);
   Serial.print(" ");
	}
  Serial.println("PASS3");
	return(4);
}

int imgprcs_testResult(unsigned char* image, unsigned int width)
{
	
	bool TL = 0;
	bool CL = 0;
	testResult=0;
	
	for(int i=80; i<=110 ; i++)
  {
    if (image[i] == 0)
    {
      CL=1;
      break;
    }
  }

  for(int i=190; i<=220 ; i++)
  {
    if (image[i] == 0)
    {
      TL=1;
      break;
    }
  }

  if (CL == 1){
    if (TL == 1)
    {
      testResult = 1;
    }
    else{
      testResult=0;
    }
  }
  else{
    testResult=2;
  }

	Serial.println("PASS4");
  SerialLog_image(image,LFT_LENBUF,LFT_WIDTH,1,LFT_WIDTH);
	return(1);
}

void SerialLog_image(unsigned char* image, unsigned int len, unsigned int width, unsigned int height, unsigned int img_width )
{
  for (int i =0 ; i< height; i++)
  {
    for (int j =0 ; j< width; j++)
    {
    Serial.print(image[ (i*img_width) + j ]);
    Serial.print(",");
    }
    Serial.println();
  }
}

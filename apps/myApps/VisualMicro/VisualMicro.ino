// colorwheel demo for Adafruit RGBmatrixPanel library.
// Renders a nice circle of hues on our 32x32 RGB LED matrix:
// http://www.adafruit.com/products/607

// Written by Limor Fried/Ladyada & Phil Burgess/PaintYourDragon
// for Adafruit Industries.
// BSD license, all text above must be included in any redistribution.


#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library

#define CLK 8
#define OE  9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
#define D   A3

enum DataType
{
	COLOR_DATA = 9,
	EDIT_DATA = 8,
};

enum EditorMode
{
	DRAW = 0,
	MOVE = 1,
	DELETE = 2,
	BACK = 3,
	FORWARD = 4,
	CLEAR = 5,
};

RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false);

uint16_t getColor(int col, int row);
uint16_t currentColor;

int currentEdit;
uint16_t editColor;
uint16_t eraseColor;

int x = -1;
int y = -1;

void setup()
{
	// Open serial communications
	Serial.begin(56000);
	
	// Initialize the LED
	matrix.begin();

	// Initialize the color
	currentColor = matrix.Color888(255, 0, 0);
	editColor = matrix.Color888(153, 70, 0);
	eraseColor = matrix.Color888(0, 0, 0);

	// Initialize the edit mode
	currentEdit = DRAW;

	// Draw color zone
	matrix.drawPixel(0, 30, currentColor);
	matrix.drawPixel(1, 30, currentColor);
	matrix.drawPixel(0, 31, currentColor);
	matrix.drawPixel(1, 31, currentColor);

	// Draw edit zone
	matrix.drawPixel(3, 30, editColor);
	matrix.drawPixel(3, 31, editColor);
	matrix.drawPixel(4, 30, editColor);
	matrix.drawPixel(4, 31, editColor);
	matrix.drawPixel(5, 30, editColor);
	matrix.drawPixel(5, 31, editColor);
	matrix.drawPixel(6, 30, editColor);
	matrix.drawPixel(6, 31, editColor);

	// Just for stablization
	delay(1000);
}

void loop()
{
	if(Serial.available() > 0)
	{
		int header = Serial.peek() - '0';
		if(header == COLOR_DATA)
		{
			//Serial.println("COLOR");

			// Received data is color info
			while(Serial.available() >= 4)
			{
				int col = 0;
				int row = 0;

				// Detach the header
				Serial.read();

				row = Serial.read() - '0';
				for(int i = 0; i < 2; ++i)
				{
					int temp = Serial.read() - '0';
					col = col * 10 + temp;
				}

				//Serial.print("col: ");
				//Serial.print(col);
				//Serial.print(", row: ");
				//Serial.println(row);

				if((0 <= col && col < 13) && (0 <= row && row < 9))
				{
					currentColor = getColor(col, row);

					matrix.drawPixel(0, 30, currentColor);
					matrix.drawPixel(1, 30, currentColor);
					matrix.drawPixel(0, 31, currentColor);
					matrix.drawPixel(1, 31, currentColor);
				}

				break;
			}
		}
		else if(header == EDIT_DATA)
		{
			//Serial.println("EDIT");

			// Received data is edit info
			while(Serial.available() >= 2)
			{
				int drawPoint;
				int erasePoint;

				// Detach the header
				Serial.read();

				int nextEdit = Serial.read() - '0';
				switch(nextEdit)
				{
				case DRAW:
					drawPoint = 3; break;
				case MOVE:
					drawPoint = 8; break;
				case DELETE:
					drawPoint = 13; break;
				case BACK:
					drawPoint = 18; break;
				case FORWARD:
					drawPoint = 23; break;
				case CLEAR:
					drawPoint = 28; break;
				}

				matrix.drawPixel(drawPoint, 30, editColor);
				matrix.drawPixel(drawPoint, 31, editColor);
				matrix.drawPixel(drawPoint + 1, 30, editColor);
				matrix.drawPixel(drawPoint + 1, 31, editColor);
				matrix.drawPixel(drawPoint + 2, 30, editColor);
				matrix.drawPixel(drawPoint + 2, 31, editColor);
				matrix.drawPixel(drawPoint + 3, 30, editColor);
				matrix.drawPixel(drawPoint + 3, 31, editColor);

				if(nextEdit == BACK ||
				   nextEdit == FORWARD ||
				   nextEdit == CLEAR)
				{
					erasePoint = drawPoint;
					delay(100);
				}
				else
				{
					switch(currentEdit)
					{
					case DRAW:
						erasePoint = 3; break;
					case MOVE:
						erasePoint = 8; break;
					case DELETE:
						erasePoint = 13; break;
					}
				}

				if(currentEdit == nextEdit)
					break;

				matrix.drawPixel(erasePoint, 30, eraseColor);
				matrix.drawPixel(erasePoint, 31, eraseColor);
				matrix.drawPixel(erasePoint + 1, 30, eraseColor);
				matrix.drawPixel(erasePoint + 1, 31, eraseColor);
				matrix.drawPixel(erasePoint + 2, 30, eraseColor);
				matrix.drawPixel(erasePoint + 2, 31, eraseColor);
				matrix.drawPixel(erasePoint + 3, 30, eraseColor);
				matrix.drawPixel(erasePoint + 3, 31, eraseColor);				

				if(nextEdit == DRAW ||
				   nextEdit == MOVE ||
				   nextEdit == DELETE)
					currentEdit = nextEdit;

				break;
			}
		}
		else
		{
			//Serial.println("COORD");

			// Received data is coordinate info
			while(Serial.available() >= 4)
			{
				x = 0;
				y = 0;

				for(int i = 0; i < 2; ++i)
				{
					int temp = Serial.read() - '0';
					x = x * 10 + temp;

				}
				for(int i = 0; i < 2; ++i)
				{
					int temp = Serial.read() - '0';
					y = y * 10 + temp;
				}

				//Serial.print("(");
				//Serial.print(x);
				//Serial.print(", ");
				//Serial.print(y);
				//Serial.println(")");

				if((0 <= x && x < 32) && (0 <= y && y < 29))
					matrix.drawPixel(x, y, currentColor);

				break;
			}
		}
	}
}
uint16_t getColor(int col, int row)
{
	switch(col)
	{
	case 0:
		switch(row)
		{
		case 0:
			return matrix.Color888(51, 0, 0);
		case 1:
			return matrix.Color888(102, 0, 0);
		case 2:
			return matrix.Color888(153, 0, 0);
		case 3:
			return matrix.Color888(204, 0, 0);
		case 4:
			return matrix.Color888(255, 0, 0);
		case 5:
			return matrix.Color888(255, 51, 51);
		case 6:
			return matrix.Color888(255, 102, 102);
		case 7:
			return matrix.Color888(255, 153, 153);
		case 8:
			return matrix.Color888(255, 204, 204);
		}
		break;
	case 1:
		switch(row)
		{
		case 0:
			return matrix.Color888(51, 25, 0);
		case 1:
			return matrix.Color888(102, 51, 0);
		case 2:
			return matrix.Color888(153, 76, 0);
		case 3:
			return matrix.Color888(204, 102, 0);
		case 4:
			return matrix.Color888(255, 128, 0);
		case 5:
			return matrix.Color888(255, 153, 51);
		case 6:
			return matrix.Color888(255, 178, 102);
		case 7:
			return matrix.Color888(255, 204, 153);
		case 8:
			return matrix.Color888(255, 229, 204);
		}
		break;
	case 2:
		switch(row)
		{
		case 0:
			return matrix.Color888(51, 51, 0);
		case 1:
			return matrix.Color888(102, 102, 0);
		case 2:
			return matrix.Color888(153, 153, 0);
		case 3:
			return matrix.Color888(204, 204, 0);
		case 4:
			return matrix.Color888(255, 255, 0);
		case 5:
			return matrix.Color888(255, 255, 51);
		case 6:
			return matrix.Color888(255, 255, 102);
		case 7:
			return matrix.Color888(255, 255, 153);
		case 8:
			return matrix.Color888(255, 255, 204);
		}
		break;
	case 3:
		switch(row)
		{
		case 0:
			return matrix.Color888(25, 51, 0);
		case 1:
			return matrix.Color888(51, 102, 0);
		case 2:
			return matrix.Color888(76, 153, 0);
		case 3:
			return matrix.Color888(102, 204, 0);
		case 4:
			return matrix.Color888(128, 255, 0);
		case 5:
			return matrix.Color888(153, 255, 51);
		case 6:
			return matrix.Color888(178, 255, 102);
		case 7:
			return matrix.Color888(204, 255, 153);
		case 8:
			return matrix.Color888(229, 255, 204);
		}
		break;
	case 4:
		switch(row)
		{
		case 0:
			return matrix.Color888(0, 51, 0);
		case 1:
			return matrix.Color888(0, 102, 0);
		case 2:
			return matrix.Color888(0, 153, 0);
		case 3:
			return matrix.Color888(0, 204, 0);
		case 4:
			return matrix.Color888(0, 255, 0);
		case 5:
			return matrix.Color888(51, 255, 51);
		case 6:
			return matrix.Color888(102, 255, 102);
		case 7:
			return matrix.Color888(153, 255, 153);
		case 8:
			return matrix.Color888(204, 255, 204);
		}
		break;
	case 5:
		switch(row)
		{
		case 0:
			return matrix.Color888(0, 51, 25);
		case 1:
			return matrix.Color888(0, 102, 51);
		case 2:
			return matrix.Color888(0, 153, 76);
		case 3:
			return matrix.Color888(0, 204, 102);
		case 4:
			return matrix.Color888(0, 255, 128);
		case 5:
			return matrix.Color888(51, 255, 153);
		case 6:
			return matrix.Color888(102, 255, 178);
		case 7:
			return matrix.Color888(153, 255, 204);
		case 8:
			return matrix.Color888(204, 255, 209);
		}
		break;
	case 6:
		switch(row)
		{
		case 0:
			return matrix.Color888(0, 51, 51);
		case 1:
			return matrix.Color888(0, 102, 102);
		case 2:
			return matrix.Color888(0, 153, 153);
		case 3:
			return matrix.Color888(0, 204, 204);
		case 4:
			return matrix.Color888(0, 255, 255);
		case 5:
			return matrix.Color888(51, 255, 255);
		case 6:
			return matrix.Color888(102, 255, 255);
		case 7:
			return matrix.Color888(153, 255, 255);
		case 8:
			return matrix.Color888(204, 255, 255);
		}
		break;
	case 7:
		switch(row)
		{
		case 0:
			return matrix.Color888(0, 25, 51);
		case 1:
			return matrix.Color888(0, 51, 102);
		case 2:
			return matrix.Color888(0, 76, 153);
		case 3:
			return matrix.Color888(0, 102, 204);
		case 4:
			return matrix.Color888(0, 128, 255);
		case 5:
			return matrix.Color888(51, 153, 255);
		case 6:
			return matrix.Color888(102, 178, 255);
		case 7:
			return matrix.Color888(153, 204, 255);
		case 8:
			return matrix.Color888(204, 229, 255);
		}
		break;
	case 8:
		switch(row)
		{
		case 0:
			return matrix.Color888(0, 0, 51);
		case 1:
			return matrix.Color888(0, 0, 102);
		case 2:
			return matrix.Color888(0, 0, 153);
		case 3:
			return matrix.Color888(0, 0, 204);
		case 4:
			return matrix.Color888(0, 0, 255);
		case 5:
			return matrix.Color888(51, 51, 255);
		case 6:
			return matrix.Color888(102, 102, 255);
		case 7:
			return matrix.Color888(153, 153, 255);
		case 8:
			return matrix.Color888(204, 204, 255);
		}
		break;
	case 9:
		switch(row)
		{
		case 0:
			return matrix.Color888(25, 0, 51);
		case 1:
			return matrix.Color888(51, 0, 102);
		case 2:
			return matrix.Color888(76, 0, 153);
		case 3:
			return matrix.Color888(102, 0, 204);
		case 4:
			return matrix.Color888(127, 0, 255);
		case 5:
			return matrix.Color888(153, 51, 255);
		case 6:
			return matrix.Color888(178, 102, 255);
		case 7:
			return matrix.Color888(204, 153, 255);
		case 8:
			return matrix.Color888(229, 204, 255);
		}
		break;
	case 10:
		switch(row)
		{
		case 0:
			return matrix.Color888(51, 0, 51);
		case 1:
			return matrix.Color888(102, 0, 102);
		case 2:
			return matrix.Color888(153, 0, 153);
		case 3:
			return matrix.Color888(204, 0, 204);
		case 4:
			return matrix.Color888(255, 0, 255);
		case 5:
			return matrix.Color888(255, 51, 255);
		case 6:
			return matrix.Color888(255, 102, 255);
		case 7:
			return matrix.Color888(255, 153, 255);
		case 8:
			return matrix.Color888(255, 204, 255);
		}
		break;
	case 11:
		switch(row)
		{
		case 0:
			return matrix.Color888(51, 0, 25);
		case 1:
			return matrix.Color888(102, 0, 51);
		case 2:
			return matrix.Color888(153, 0, 76);
		case 3:
			return matrix.Color888(204, 0, 102);
		case 4:
			return matrix.Color888(255, 0, 127);
		case 5:
			return matrix.Color888(255, 51, 153);
		case 6:
			return matrix.Color888(255, 102, 178);
		case 7:
			return matrix.Color888(255, 153, 204);
		case 8:
			return matrix.Color888(255, 204, 229);
		}
		break;
	case 12:
		switch(row)
		{
		case 0:
			return matrix.Color888(0, 0, 0);
		case 1:
			return matrix.Color888(32, 32, 32);
		case 2:
			return matrix.Color888(64, 64, 64);
		case 3:
			return matrix.Color888(96, 96, 96);
		case 4:
			return matrix.Color888(128, 128, 128);
		case 5:
			return matrix.Color888(160, 160, 160);
		case 6:
			return matrix.Color888(192, 192, 192);
		case 7:
			return matrix.Color888(224, 224, 224);
		case 8:
			return matrix.Color888(255, 255, 255);
		}
		break;
	}
}
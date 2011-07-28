// tool "framework"
#include "maplib.h"

#include "mapload.h"
#include "pngsave.h"

#include "vector.h"

// Qt Gui
#include <QtGui/QImage>

struct PreviewColors
{
	Vector3i  cliffLow;
	Vector3i  cliffHigh;
	Vector3i  water;
	Vector3i  roadLow;
	Vector3i  roadHigh;
	Vector3i  groundLow;
	Vector3i  groundHigh;
};

// C1 - Arizona type
static const PreviewColors pArizonaColors = {
	Vector3i (0x68, 0x3C, 0x24),
	Vector3i (0xE8, 0x84, 0x5C),
	Vector3i (0x3F, 0x68, 0x9A),
	Vector3i (0x24, 0x1F, 0x16),
	Vector3i (0xB2, 0x9A, 0x66),
	Vector3i (0x24, 0x1F, 0x16),
	Vector3i (0xCC, 0xB2, 0x80)};

// C2 - Urban type
static const PreviewColors pUrbanColors = {
	Vector3i (0x3C, 0x3C, 0x3C),
	Vector3i (0x84, 0x84, 0x84),
	Vector3i (0x3F, 0x68, 0x9A),
	Vector3i (0x00, 0x00, 0x00),
	Vector3i (0x24, 0x1F, 0x16),
	Vector3i (0x1F, 0x1F, 0x1F),
	Vector3i (0xB2, 0xB2, 0xB2)};

// C3 - Rockies type
static const PreviewColors pRockiesColors = {
	Vector3i (0x3C, 0x3C, 0x3C),
	Vector3i (0xFF, 0xFF, 0xFF),
	Vector3i (0x3F, 0x68, 0x9A),
	Vector3i (0x24, 0x1F, 0x16),
	Vector3i (0x3D, 0x21, 0x0A),
	Vector3i (0x00, 0x1C, 0x0E),
	Vector3i (0xFF, 0xFF, 0xFF)};

Vector3i clanColours[]=
{   // see frontend2.png for team color order.
	// [r,g,b,a]
	Vector3i (0,255,0),			// green  Player 0
	Vector3i (255,210,40),		// orange Player 1
	Vector3i (255,255,255),		// grey   Player 2
	Vector3i (0,0,0),			// black  Player 3
	Vector3i (255,0,0),			// red	Player 4
	Vector3i (20,20,255),		// blue   Player 5
	Vector3i (255,0,255),		// pink   Player 6 (called purple in palette.txt)
	Vector3i (0,255,255),		// cyan   Player 7
	Vector3i (255,255,0),		// yellow Player 8
	Vector3i (192,0,255),		// pink   Player 9
	Vector3i (200,255,255),		// white  Player A (Should be brighter than grey, but grey is already maximum.)
	Vector3i (128,128,255),		// bright blue Player B
	Vector3i (128,255,128),		// neon green  Player C
	Vector3i (128,0,0),			// infrared	Player D
	Vector3i (64,0,128),		// ultraviolet Player E
	Vector3i (128,128,0)		// brown	   Player F
};

const uint16_t previewWidth = 512;
const uint16_t previewHeight = 512;

static void paintStructureData(uchar *imageData, GAMEMAP *map)
{
	uint32_t xx, yy;
	Vector3i color;
	
	for (int i = 0; i < map->numStructures; i++)
	{
		LND_OBJECT *psObj = &map->mLndObjects[IMD_STRUCTURE][i];

		if (strcmp(psObj->name, "A0PowMod1") == 0 ||
			strcmp(psObj->name, "A0FacMod1") == 0 ||
			strcmp(psObj->name, "A0ResearchModule1") == 0)
		{
			continue; // ignore modules.
		}

		color = clanColours[psObj->player];
		if (strcmp(psObj->name, "A0CommandCentre") == 0)
		{
			color = Vector3i(0xFF,0,0xFF); // base/palette.txt - WZCOL_MAP_PREVIEW_HQ
		}
		
		xx = map_coord(psObj->x);
		yy = map_coord(psObj->y);

		uchar* p = imageData + (3 * (yy * static_cast<int>(map->width) + xx));
		p[0] = color.x;
		p[1] = color.y;
		p[2] = color.z;
	}
}

int main(int argc, char **argv)
{
	char *filename, *p_filename;
	char *base, tmpFile[PATH_MAX];
	GAMEMAP *map;
	MAPTILE		 *psTile;

	if (argc != 2)
	{
		printf("Usage: %s <map>\n", argv[0]);
		return -1;
	}

	physfs_init(argv[0]);
	filename = physfs_addmappath(argv[1]);

	p_filename = strrchr(filename, '/');
	if (p_filename)
	{
		p_filename++;
		base = strdup(p_filename);
	}
	else
	{
		base = strdup(filename);
	}
	
	map = mapLoad(filename);
	free(filename);
	
	if (!map)
	{
		return EXIT_FAILURE;
	}

	const PreviewColors* tileColors;
	switch (map->tileset)
	{
	case TILESET_ARIZONA:
		tileColors = &pArizonaColors;
		break;
	case TILESET_URBAN:
		tileColors = &pUrbanColors;
		break;
	case TILESET_ROCKIES:
		tileColors = &pRockiesColors;
		break;
	}

	int mapWidth = static_cast<int>(map->width);
	int mapHeight = static_cast<int>(map->height);

	int col;
	uchar* imageData = (uchar*)malloc(sizeof(uchar) * mapWidth * mapHeight * 3);	 // used for the texture
	
	for (int y = 0; y < mapHeight; y++)
	{
		for (int x = 0; x < mapWidth; x++)
		{
			psTile = mapTile(map, x, y);
			uchar* const p = imageData + (3 * (y * mapWidth + x));
			col = psTile->height / 2; // 2 = ELEVATION_SCALE
			switch(terrainType(psTile))
			{
				case TER_CLIFFFACE:
					p[0] = tileColors->cliffLow.x + (tileColors->cliffHigh.x - tileColors->cliffLow.x) * col / 256;
					p[1] = tileColors->cliffLow.y + (tileColors->cliffHigh.y - tileColors->cliffLow.y) * col / 256;
					p[2] = tileColors->cliffLow.z + (tileColors->cliffHigh.z - tileColors->cliffLow.z) * col / 256;
				break;
				case TER_WATER:
					p[0] = tileColors->water.x;
					p[1] = tileColors->water.y;
					p[2] = tileColors->water.z;
				break;
				case TER_ROAD:
					p[0] = tileColors->roadLow.x + (tileColors->roadHigh.x - tileColors->roadLow.x) * col / 256;
					p[1] = tileColors->roadLow.y + (tileColors->roadHigh.y - tileColors->roadLow.y) * col / 256;
					p[2] = tileColors->roadLow.z + (tileColors->roadHigh.z - tileColors->roadLow.z) * col / 256;
				break;
				default:
					p[0] = tileColors->groundLow.x + (tileColors->groundHigh.x - tileColors->groundLow.x) * col / 256;
					p[1] = tileColors->groundLow.y + (tileColors->groundHigh.y - tileColors->groundLow.y) * col / 256;
					p[2] = tileColors->groundLow.z + (tileColors->groundHigh.z - tileColors->groundLow.z) * col / 256;
				break;
			}
		}
	}

	paintStructureData(imageData, map);

	strcpy(tmpFile, base);
	strcat(tmpFile, ".png");

	QImage image(imageData, mapWidth, mapHeight, QImage::Format_RGB888);
	image.scaled(previewWidth, previewHeight, Qt::KeepAspectRatio).save(tmpFile, "PNG");
	//savePng(tmpFile, imageData, mapWidth, mapHeight);

	free(imageData);
	
	mapFree(map);
		
	physfs_shutdown();

	return 0;
}

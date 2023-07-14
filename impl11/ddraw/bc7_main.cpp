#include "bc7_main.h"
#include "bc7decomp.h"

int BC7_Decode(const char* pBlock, char* pPixelsRGBA, int pPixelsRGBAWidth, int pPixelsRGBAHeight)
{
	char block[4 * 4 * 4]{};

	for (int y = 0; y < pPixelsRGBAHeight; y += 4)
	{
		for (int x = 0; x < pPixelsRGBAWidth; x += 4)
		{
			int maxJ = std::min(pPixelsRGBAHeight - y, 4);
			int maxI = std::min(pPixelsRGBAWidth - x, 4);

			bc7decomp::unpack_bc7(pBlock, (bc7decomp::color_rgba*)block);

			for (int j = 0; j < maxJ; j++)
			{
				for (int i = 0; i < maxI; i++)
				{
					int currentJ = j;
					int currentI = i;

					int sourceOffset = (y + currentJ) * pPixelsRGBAWidth * 4 + (x + currentI) * 4;
					int destinationOffset = j * 4 * 4 + i * 4;

					*(unsigned int*)(pPixelsRGBA + sourceOffset) = *(unsigned int*)(block + destinationOffset);

					pPixelsRGBA[sourceOffset + 2] = block[destinationOffset + 0];
					pPixelsRGBA[sourceOffset + 0] = block[destinationOffset + 2];
				}
			}

			pBlock += 16;
		}
	}

	return 0;
}

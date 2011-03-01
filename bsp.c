#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "bspfile.h"

FILE *f;
dheader_t header;

int open_bsp(const char *filename)
{
	f = fopen(filename, "rb");

	if (!f)
		return 0;

	fread(&header, sizeof(header), 1, f);

	return 1;
}

void *get_lump_data(size_t *count, int lump_id, size_t type_size)
{
	void *lump_data;
	lump_t *l = &header.lumps[lump_id];
	*count = l->filelen / type_size;
	lump_data = calloc(*count, type_size);

	fseek(f, l->fileofs, SEEK_SET);
	fread(lump_data, type_size, *count, f);

	return lump_data;
}

dvertex_t *get_vertices(size_t *count)
{
	return (dvertex_t *)get_lump_data(count, LUMP_VERTEXES, sizeof(dvertex_t));
}

dface_t *get_faces(size_t *count)
{
	return (dface_t *)get_lump_data(count, LUMP_FACES, sizeof(dface_t));
}

dedge_t *get_edges(size_t *count)
{
	return (dedge_t *)get_lump_data(count, LUMP_EDGES, sizeof(dedge_t));
}

int *get_ledges(size_t *count)
{
	return (int *)get_lump_data(count, LUMP_SURFEDGES, sizeof(int));
}

dmodel_t *get_models(size_t *count)
{
	return (dmodel_t *)get_lump_data(count, LUMP_MODELS, sizeof(dmodel_t));
}

dplane_t *get_planes(size_t *count)
{
	return (dplane_t *)get_lump_data(count, LUMP_PLANES, sizeof(dplane_t));
}

texinfo_t *get_texinfos(size_t *count)
{
	return (texinfo_t *)get_lump_data(count, LUMP_TEXINFO, sizeof(texinfo_t));
}

dmiptexlump_t *get_miptexture_header()
{
	dmiptexlump_t *miptex_header;
	lump_t *l = &header.lumps[LUMP_TEXTURES];	
	miptex_header = (dmiptexlump_t *)malloc(l->filelen);

	fseek(f, l->fileofs, SEEK_SET);
	fread(miptex_header, 1, l->filelen, f);

	return miptex_header;
}

miptex_t *get_miptextures(const dmiptexlump_t *mipheader, size_t *count)
{
	int i;
	miptex_t *miptexes;
	
	*count = mipheader->nummiptex;
	miptexes = (miptex_t *)calloc(*count, sizeof(miptex_t));

	for (i = 0; i < mipheader->nummiptex; i++)
	{
		fseek(f, header.lumps[LUMP_TEXTURES].fileofs + mipheader->dataofs[i], SEEK_SET);
		fread(&miptexes[i], sizeof(miptex_t), *count, f);
	}

	return miptexes;
}

unsigned char *get_texture(miptex_t *miptex, int mip_level)
{
	unsigned char *texture_data;

	// The miptex offsets are relative to the start of the texture lump.
	long offset = header.lumps[LUMP_TEXTURES].fileofs + miptex->offsets[mip_level];
	
	// There are several mip levels stored. 1, 1/2, 1/4, 1/8 of the original size.
	size_t size = (miptex->width * miptex->height) / (int)pow(2.0, mip_level);

	// This is 1 byte per pixel raw data. 1 byte for index into the pallete of size 256.
	texture_data = (unsigned char *)calloc(size, sizeof(unsigned char));
	fseek(f, offset, SEEK_SET);
	fread(texture_data, 1, size, f);

	return texture_data;
}


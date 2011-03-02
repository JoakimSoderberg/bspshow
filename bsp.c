#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

byte *get_miptexture_lump_data()
{
	dmiptexlump_t *miptex_header;
	lump_t *l = &header.lumps[LUMP_TEXTURES];
	byte *data = (unsigned char *)malloc(l->filelen);

	fseek(f, l->fileofs, SEEK_SET);
	fread(data, l->filelen, 1, f);

	return data;
}

miptex_t **get_miptextures(const byte *texture_lump_data, size_t *count)
{
	size_t i;
	const byte *d = texture_lump_data;
	miptex_t **mips;
	dmiptexlump_t *miptex_header;
	
	// Get the header.
	miptex_header = (dmiptexlump_t *)d;
	d += (sizeof(dmiptexlump_t) * sizeof(long) * miptex_header->nummiptex);

	// Allocate memory for pointers to the textures.
	*count = miptex_header->nummiptex;
	mips = (miptex_t **)calloc(*count, sizeof(miptex_t *));

	// Set the pointers based on the offsets in the header.
	for (i = 0; i < (*count); i++)
	{
		mips[i] = (miptex_t *)(&texture_lump_data[miptex_header->dataofs[i]]);
	}

	return mips;
}

byte *get_texture(const byte *texture_lump_data, const miptex_t *miptex, int mip_level)
{
	byte *texture_data;
	dmiptexlump_t *miptex_header = (dmiptexlump_t *)texture_lump_data;

	// The miptex offsets are relative to the start of the texture lump.
	long offset = miptex->offsets[mip_level];
	
	// There are several mip levels stored. 1, 1/2, 1/4, 1/8 of the original size.
	size_t width = miptex->width / (int) pow(2.0, mip_level); 
	size_t height = miptex->height / (int) pow(2.0, mip_level);
	size_t size = width * height;

	// Instead of RGB values, the texture contains indexes, which is used to get
	// the RGB values from the 256 color pallete.
	texture_data = (byte *)calloc(size, sizeof(byte));
	memcpy(texture_data, &texture_lump_data[offset], size);

	return texture_data;
}

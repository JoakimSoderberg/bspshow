#include "bspshow_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "bspfile.h"
#include "bsp.h"

#if 0

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
	miptex_t **mips;
	dmiptexlump_t *miptex_header;
	
	// Get the header.
	miptex_header = (dmiptexlump_t *)texture_lump_data;

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
#endif

// -----------

int bsp_read_header(FILE *f, dheader_t *header)
{
	assert(f);
	assert(header);

	if (fread(header, sizeof(dheader_t), 1, f) != 1)
		return -1;

	return 0;
}

void *bsp_read_lump_data(FILE *f, dheader_t *header, size_t *count, int lump_id, size_t type_size)
{
	void *lump_data;
	lump_t *l;

	assert(f);
	assert(header);
	assert(count);
	assert(type_size > 0);
	assert(lump_id >= 0);

	l = &header->lumps[lump_id];
	*count = l->filelen / type_size;

	if (!(lump_data = calloc(*count, type_size)))
	{
		fprintf(stderr, "Out of memory!\n");
		return NULL;
	}

	fseek(f, l->fileofs, SEEK_SET);
	if (fread(lump_data, type_size, *count, f) != *count)
	{
		fprintf(stderr, "Failed to read entire lump %d!\n", lump_id);
		free(lump_data);
		*count = 0;
		return NULL;
	}

	return lump_data;
}

dvertex_t *bsp_read_vertices(FILE *f, dheader_t *header, size_t *count)
{
	return (dvertex_t *)bsp_read_lump_data(f, header, count, LUMP_VERTEXES, sizeof(dvertex_t));
}

dface_t *bsp_read_faces(FILE *f, dheader_t *header, size_t *count)
{
	return (dface_t *)bsp_read_lump_data(f, header, count, LUMP_FACES, sizeof(dface_t));
}

dedge_t *bsp_read_edges(FILE *f, dheader_t *header, size_t *count)
{
	return (dedge_t *)bsp_read_lump_data(f, header, count, LUMP_EDGES, sizeof(dedge_t));
}

int *bsp_read_ledges(FILE *f, dheader_t *header, size_t *count)
{
	return (int *)bsp_read_lump_data(f, header,count, LUMP_SURFEDGES, sizeof(int));
}

dmodel_t *bsp_read_models(FILE *f, dheader_t *header,size_t *count)
{
	return (dmodel_t *)bsp_read_lump_data(f, header, count, LUMP_MODELS, sizeof(dmodel_t));
}

dplane_t *bsp_read_planes(FILE *f, dheader_t *header, size_t *count)
{
	return (dplane_t *)bsp_read_lump_data(f, header, count, LUMP_PLANES, sizeof(dplane_t));
}

texinfo_t *bsp_read_texinfos(FILE *f, dheader_t *header, size_t *count)
{
	return (texinfo_t *)bsp_read_lump_data(f, header, count, LUMP_TEXINFO, sizeof(texinfo_t));
}

byte *bsp_read_miptexture_data(FILE *f, dheader_t *header)
{
	// This is a special case, the lump contents is of variable
	// size. We can't use bsp_read_lump_data for this.
	lump_t *l;
	byte *data;

	assert(f);
	assert(header);

	l = &header->lumps[LUMP_TEXTURES];
	data = (unsigned char *)malloc(l->filelen);

	fseek(f, l->fileofs, SEEK_SET);

	if (fread(data, 1, l->filelen, f) != l->filelen)
	{
		return NULL;
	}

	return data;
}

miptex_t **bsp_read_miptextures(const byte *texture_lump_data, size_t *count)
{
	size_t i;
	miptex_t **mips;
	dmiptexlump_t *miptex_header;

	assert(texture_lump_data);
	assert(count);

	// Get the header.
	miptex_header = (dmiptexlump_t *)texture_lump_data;

	// Allocate memory for pointers to the textures.
	*count = miptex_header->nummiptex;

	if (!(mips = (miptex_t **)calloc(*count, sizeof(miptex_t *))))
	{
		fprintf(stderr, "Out of memory!\n");
		return NULL;
	}

	// Set the pointers based on the offsets in the header.
	for (i = 0; i < (*count); i++)
	{
		mips[i] = (miptex_t *)(&texture_lump_data[miptex_header->dataofs[i]]);
	}

	return mips;
}

int bsp_read_file(const char *filename, bsp_t *bsp)
{
	FILE *f;
	size_t count;

	assert(bsp);
	
	if (!filename)
		return -1;

	if (!(f = fopen(filename, "rb")))
	{
		fprintf(stderr, "Failed to open file %s for reading!\n", filename);
		return -1;
	}

	if (bsp_read_header(f, &bsp->header))
	{
		fprintf(stderr, "Failed to read BSP header\n");
		return -1;
	}

	bsp->faces = bsp_read_faces(f, &bsp->header, &count);
	bsp->face_count = count;

	bsp->edges = bsp_read_edges(f, &bsp->header, &count);
	bsp->edge_count = count;

	bsp->ledges = bsp_read_ledges(f, &bsp->header, &count);
	bsp->ledge_count = count;

	bsp->vertices = bsp_read_vertices(f, &bsp->header, &count);
	bsp->vertex_count = count;

	bsp->models = bsp_read_models(f, &bsp->header, &count);
	bsp->model_count = count;

	bsp->planes = bsp_read_planes(f, &bsp->header, &count);
	bsp->plane_count = count;

	bsp->texinfos = bsp_read_texinfos(f, &bsp->header, &count);
	bsp->texinfo_count = count;

	bsp->mip_data = bsp_read_miptexture_data(f, &bsp->header);

	bsp->mip_list = bsp_read_miptextures(bsp->mip_data, &count);
	bsp->mipmap_count;

	if (!bsp->faces 
		|| !bsp->edges
		|| !bsp->ledges
		|| !bsp->vertices
		|| !bsp->models
		|| !bsp->planes
		|| !bsp->mip_data
		|| !bsp->mip_list)
	{
		fprintf(stderr, "Failed to load one or more BSP data lumps\n");
		return -1;
	}

	return 0;
}

void bsp_destroy(bsp_t *bsp)
{
	if (!bsp)
		return;

	free(bsp->faces);
	free(bsp->edges);
	free(bsp->ledges);
	free(bsp->vertices);
	free(bsp->models);
	free(bsp->planes);
}

#include "bspshow_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "bspfile.h"
#include "bsp.h"
#include "md4.h"

char *lump_names[] = 
{
	"LUMP_ENTITIES",
	"LUMP_PLANES",
	"LUMP_TEXTURES",
	"LUMP_VERTEXES",
	"LUMP_VISIBILITY",
	"LUMP_NODES",
	"LUMP_TEXINFO",
	"LUMP_FACES",
	"LUMP_LIGHTING",
	"LUMP_CLIPNODES",
	"LUMP_LEAFS",
	"LUMP_MARKSURFACES",
	"LUMP_EDGES",
	"LUMP_SURFEDGES",
	"LUMP_MODELS",

	"HEADER_LUMPS"
};

int bsp_read_header(FILE *f, dheader_t *header)
{
	assert(f);
	assert(header);

	if (fread(header, sizeof(dheader_t), 1, f) != 1)
		return -1;

	return 0;
}

void *bsp_read_lump_data(FILE *f, dheader_t *header, size_t *count, int lump_id, size_t type_size, unsigned int *checksum)
{
	void *lump_data;
	lump_t *l;

	assert(f);
	assert(header);
	assert(count);
	assert(type_size > 0);
	assert(lump_id >= 0);

	l = &header->lumps[lump_id];

	if (l->filelen % type_size)
	{
		fprintf(stderr, "Lump %s of size %d is not evenly divisible" 
						" by its element size (%zu)\n", 
						lump_names[lump_id], l->filelen, type_size);
		return NULL;
	}

	*count = l->filelen / type_size;

	if (!(lump_data = calloc(*count, type_size)))
	{
		fprintf(stderr, "Out of memory!\n");
		return NULL;
	}

	fseek(f, l->fileofs, SEEK_SET);
	if (fread(lump_data, type_size, *count, f) != *count)
	{
		fprintf(stderr, "Failed to read entire lump %s!\n", lump_names[lump_id]);
		free(lump_data);
		*count = 0;
		return NULL;
	}

	if (checksum)
	{
		*checksum = Com_BlockChecksum(lump_data, l->filelen);
	}

	return lump_data;
}

dvertex_t *bsp_read_vertices(FILE *f, dheader_t *header, size_t *count, unsigned int *checksum)
{
	return (dvertex_t *)bsp_read_lump_data(f, header, count, LUMP_VERTEXES, sizeof(dvertex_t), checksum);
}

dface_t *bsp_read_faces(FILE *f, dheader_t *header, size_t *count, unsigned int *checksum)
{
	return (dface_t *)bsp_read_lump_data(f, header, count, LUMP_FACES, sizeof(dface_t), checksum);
}

dedge_t *bsp_read_edges(FILE *f, dheader_t *header, size_t *count, unsigned int *checksum)
{
	return (dedge_t *)bsp_read_lump_data(f, header, count, LUMP_EDGES, sizeof(dedge_t), checksum);
}

int *bsp_read_ledges(FILE *f, dheader_t *header, size_t *count, unsigned int *checksum)
{
	return (int *)bsp_read_lump_data(f, header,count, LUMP_SURFEDGES, sizeof(int), checksum);
}

dmodel_t *bsp_read_models(FILE *f, dheader_t *header,size_t *count, unsigned int *checksum)
{
	return (dmodel_t *)bsp_read_lump_data(f, header, count, LUMP_MODELS, sizeof(dmodel_t), checksum);
}

dplane_t *bsp_read_planes(FILE *f, dheader_t *header, size_t *count, unsigned int *checksum)
{
	return (dplane_t *)bsp_read_lump_data(f, header, count, LUMP_PLANES, sizeof(dplane_t), checksum);
}

char *bsp_read_entities(FILE *f, dheader_t *header, size_t *count, unsigned int *checksum)
{
	assert(f);
	assert(header);
	lump_t *l = &header->lumps[LUMP_ENTITIES];

	if (checksum)
	{
		*checksum = 0;
	}

	if (!l->filelen)
	{
		return NULL;
	}
	
	return (char *)bsp_read_lump_data(f, header, count, LUMP_ENTITIES, 1, checksum);
}

byte *bsp_read_visibility(FILE *f, dheader_t *header, size_t *count, unsigned int *checksum)
{
	return (byte *)bsp_read_lump_data(f, header, count, LUMP_VISIBILITY, 1, checksum);
}

dleaf_t *bsp_read_leafs(FILE *f, dheader_t *header, size_t *count, unsigned int *checksum)
{
	return (dleaf_t *)bsp_read_lump_data(f, header, count, LUMP_LEAFS, sizeof(dleaf_t), checksum);
}

dclipnode_t *bsp_read_clipnodes(FILE *f, dheader_t *header, size_t *count, unsigned int *checksum)
{
	return (dclipnode_t *)bsp_read_lump_data(f, header, count, LUMP_CLIPNODES, sizeof(dclipnode_t), checksum);
}

dnode_t *bsp_read_nodes(FILE *f, dheader_t *header, size_t *count, unsigned int *checksum)
{
	return (dnode_t *)bsp_read_lump_data(f, header, count, LUMP_NODES, sizeof(dnode_t), checksum);
}

byte *bsp_read_lights(FILE *f, dheader_t *header, size_t *count, unsigned int *checksum)
{
	return (byte *)bsp_read_lump_data(f, header, count, LUMP_LIGHTING, 1, checksum);
}

unsigned short *bsp_read_marksurfaces(FILE *f, dheader_t *header, size_t *count, unsigned int *checksum)
{
	return (unsigned short *)bsp_read_lump_data(f, header, count, LUMP_MARKSURFACES, sizeof(unsigned short), checksum);
}

int *bsp_read_surfedges(FILE *f, dheader_t *header, size_t *count, unsigned int *checksum)
{
	return (int *)bsp_read_lump_data(f, header, count, LUMP_SURFEDGES, sizeof(int), checksum);	
}

texinfo_t *bsp_read_texinfos(FILE *f, dheader_t *header, size_t *count, unsigned int *checksum)
{
	return (texinfo_t *)bsp_read_lump_data(f, header, count, LUMP_TEXINFO, sizeof(texinfo_t), checksum);
}

byte *bsp_read_miptexture_data(FILE *f, dheader_t *header, unsigned int *checksum)
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

	if (checksum)
	{
		*checksum = Com_BlockChecksum(data, l->filelen);
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

/*
int bsp_calculate_checskum(const char *filename, unsigned int *checksum, unsigned int *checksum_vis)
{
	size_t i;
	FILE *f;

	assert(filename);
	assert(checksum);
	assert(checksum_vis);

	if (!(f = fopen(filename, "rb")))
	{
		return -1;
	}

	fread()
}*/

int bsp_read_file(const char *filename, bsp_t *bsp)
{
	FILE *f;
	size_t count;
	unsigned int checksum;

	assert(bsp);
	
	if (!filename)
		return -1;

	bsp->checksum = bsp->checksum_vis = 0;

	if (!(f = fopen(filename, "rb")))
	{
		fprintf(stderr, "Failed to open file \"%s\" for reading!\n", filename);
		return -1;
	}

	if (bsp_read_header(f, &bsp->header))
	{
		fprintf(stderr, "Failed to read BSP header\n");
		return -1;
	}

	// Needed to draw the polygons:
	bsp->faces = bsp_read_faces(f, &bsp->header, &bsp->face_count, &bsp->faces_checksum);
	bsp->edges = bsp_read_edges(f, &bsp->header, &bsp->edge_count, &bsp->edges_checksum);
	bsp->ledges = bsp_read_ledges(f, &bsp->header, &bsp->ledge_count, &bsp->ledges_checksum);
	bsp->vertices = bsp_read_vertices(f, &bsp->header, &bsp->vertex_count, &bsp->vertices_checksum);
	bsp->models = bsp_read_models(f, &bsp->header, &bsp->model_count, &bsp->models_checksum);
	bsp->planes = bsp_read_planes(f, &bsp->header, &bsp->plane_count, &bsp->planes_checksum);
	bsp->texinfos = bsp_read_texinfos(f, &bsp->header, &bsp->texinfo_count, &bsp->texinfos_checksum);
	bsp->mip_data = bsp_read_miptexture_data(f, &bsp->header, &bsp->mip_checksum);
	bsp->mip_list = bsp_read_miptextures(bsp->mip_data, &bsp->mipmap_count);
	// TODO: Read all lump types!

	// BSP nodes/visibility.
	bsp->vis_data = bsp_read_visibility(f, &bsp->header, &bsp->vis_count, &bsp->vis_checksum);
	bsp->nodes = bsp_read_nodes(f, &bsp->header, &bsp->node_count, &bsp->nodes_checksum);
	bsp->clipnodes = bsp_read_clipnodes(f, &bsp->header, &bsp->clipnode_count, &bsp->clipnodes_checksum);
	bsp->leafs = bsp_read_leafs(f, &bsp->header, &bsp->leaf_count, &bsp->leafs_checksum);
	bsp->marksurfaces = bsp_read_marksurfaces(f, &bsp->header, &bsp->marksurface_count, &bsp->marksurfaces_checksum);
	bsp->surfedges = bsp_read_surfedges(f, &bsp->header, &bsp->surfedge_count, &bsp->surfedges_checksum);
	bsp->light_data = bsp_read_lights(f, &bsp->header, &bsp->light_count, &bsp->light_checksum);
	bsp->entity_data = bsp_read_entities(f, &bsp->header, &bsp->entity_count, &bsp->entity_checksum);

	if (!bsp->faces 
	 || !bsp->edges
	 || !bsp->ledges
	 || !bsp->vertices
	 || !bsp->models
	 || !bsp->planes
	 || !bsp->texinfos
	 || !bsp->mip_data
	 || !bsp->mip_list
	 || !bsp->vis_data
	 || !bsp->nodes
	 || !bsp->clipnodes
	 || !bsp->leafs
	 || !bsp->marksurfaces
	 || !bsp->surfedges
	 || !bsp->light_data
	 || !bsp->entity_data)
	{
		fprintf(stderr, "Failed to load one or more BSP data lumps\n");
		return -1;
	}

	// Checksum over all but entity data.
	bsp->checksum_vis = 
		bsp->faces_checksum ^
		bsp->edges_checksum ^
		bsp->ledges_checksum ^
		bsp->vertices_checksum ^
		bsp->models_checksum ^
		bsp->planes_checksum ^
		bsp->texinfos_checksum ^
		bsp->mip_checksum ^
		bsp->clipnodes_checksum ^
		bsp->nodes_checksum ^
		bsp->leafs_checksum ^
		bsp->vis_checksum ^
		bsp->marksurfaces_checksum ^
		bsp->surfedges_checksum ^
		bsp->light_checksum;

	// Checksum over all but visibility/leafs/nodes.
	bsp->checksum = 
		bsp->faces_checksum ^
		bsp->edges_checksum ^
		bsp->ledges_checksum ^
		bsp->vertices_checksum ^
		bsp->models_checksum ^
		bsp->planes_checksum ^
		bsp->texinfos_checksum ^
		bsp->mip_checksum ^
		bsp->clipnodes_checksum ^
		bsp->marksurfaces_checksum ^
		bsp->surfedges_checksum ^
		bsp->light_checksum;

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
	free(bsp->texinfos);
	free(bsp->mip_data);
	free(bsp->mip_list);
	memset(bsp, 0, sizeof(bsp_t));
}

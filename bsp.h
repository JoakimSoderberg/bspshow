
#ifndef __BSP_H__
#define __BSP_H__

#include "bspfile.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
//typedef unsigned char byte;

byte *get_texture(const byte *texture_lump_data, const miptex_t *miptex, int mip_level);

// ---------

typedef struct bsp_s
{
	dheader_t header;

	unsigned int checksum;
	unsigned int checksum_vis;

	dface_t *faces;
	size_t face_count;
	unsigned int faces_checksum;

	dedge_t *edges;
	size_t edge_count;
	unsigned int edges_checksum;

	int *ledges;
	size_t ledge_count;
	unsigned int ledges_checksum;

	dvertex_t *vertices;
	size_t vertex_count;
	unsigned int vertices_checksum;

	dmodel_t *models;
	size_t model_count;
	unsigned int models_checksum;

	dplane_t *planes;
	size_t plane_count;
	unsigned int planes_checksum;

	// Vis/entities/bsp nodes.
	dnode_t *nodes;
	size_t node_count;
	unsigned int nodes_checksum;

	dclipnode_t *clipnodes;
	size_t clipnode_count;
	unsigned int clipnodes_checksum;

	dleaf_t *leafs;
	size_t leaf_count;
	unsigned int leafs_checksum;

	byte *vis_data;
	size_t vis_count;
	unsigned int vis_checksum;

	unsigned short int *marksurfaces;
	size_t marksurface_count;
	unsigned int marksurfaces_checksum;

	int *surfedges;
	size_t surfedge_count;
	unsigned int surfedges_checksum;

	byte *light_data;
	size_t light_count;
	unsigned int light_checksum;

	char *entity_data;
	size_t entity_count;
	unsigned int entity_checksum;

	// Texture related.
	byte *mip_data;
	unsigned int mip_checksum;
	miptex_t **mip_list;
	size_t mipmap_count;

	texinfo_t *texinfos;
	size_t texinfo_count;
	unsigned int texinfos_checksum;
	
} bsp_t;

int bsp_read_file(const char *filename, bsp_t *bsp);

int bsp_read_header(FILE *f, dheader_t *header);

void *bsp_read_lump_data(FILE *f, dheader_t *header, size_t *count, int lump_id, size_t type_size, unsigned int *checksum);

dvertex_t *bsp_read_vertices(FILE *f, dheader_t *header, size_t *count, unsigned int *checksum);
dface_t *bsp_read_faces(FILE *f, dheader_t *header, size_t *count, unsigned int *checksum);
dedge_t *bsp_read_edges(FILE *f, dheader_t *header, size_t *count, unsigned int *checksum);
int *bsp_read_ledges(FILE *f, dheader_t *header, size_t *count, unsigned int *checksum);
dmodel_t *bsp_read_models(FILE *f, dheader_t *header,size_t *count, unsigned int *checksum);
dplane_t *bsp_read_planes(FILE *f, dheader_t *header, size_t *count, unsigned int *checksum);
texinfo_t *bsp_read_texinfos(FILE *f, dheader_t *header, size_t *count, unsigned int *checksum);
byte *bsp_read_miptexture_data(FILE *f, dheader_t *header, unsigned int *checksum);
miptex_t **bsp_read_miptextures(const byte *texture_lump_data, size_t *count);


#endif // __BSP_H__

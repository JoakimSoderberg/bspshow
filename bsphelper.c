#include "bspshow_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#ifdef WIN32
#include <windows.h>
#endif
#ifdef WITH_FREEGLUT
#include <gl/freeglut.h>
#else
#include <glut.h>
#endif
#include "qpallete.h"
#include "utils.h"
#include "config.h"
#include "bsphelper.h"

float calculate_face_area(dface_t *face)
{
	float total_area = 222220.0;
	/*float area = 0.0;
	int e;
	int lindex;
	dedge_t *edge;
	dvertex_t *first;
	dvertex_t *v0;
	dvertex_t *v1;
	float aa[3];
	float bb[3];
	float cross[3];
	
	lindex = ledges[face->firstedge];
	edge = (lindex > 0) ? &edges[lindex] : &edges[-lindex];
	first = &vertices[edge->v[0]];

	for (e = 1; e < face->firstedge; e++)
	{
		lindex = ledges[face->firstedge + e];

		edge = (lindex > 0) ? &edges[lindex] : &edges[-lindex];

		if (lindex > 0)
		{
			v0 = &vertices[edge->v[0]];
			v1 = &vertices[edge->v[1]];
		}
		else
		{
			v0 = &vertices[edge->v[1]];
			v1 = &vertices[edge->v[0]];
		}

		VectorSubtract(v0->point, first->point, aa);
		VectorSubtract(v1->point, first->point, bb);
		CrossProduct(aa, bb, cross);

		area = vector_length(cross);
		total_area += area;
	}

	return total_area;*/
	return 0.0;
}

int is_edge_long_enough(dvertex_t *v0, dvertex_t *v1)
{
	float d[3];
	float edge_length;
	VectorSubtract(v0->point, v1->point, d);

	edge_length = sqrt(d[0]*d[0] + d[1]*d[1] + d[2]*d[2]);

	return (edge_length >= config.min_edge_length);
}

polygon_t *bsp_build_polygon_list(bsp_t *bsp, size_t *count)
{
	size_t i;
	size_t e;
	int lindex;
	dface_t *face;
	dedge_t *edge;
	dvertex_t *v;
	polygon_t *p;
	polygon_t *polygons;

	assert(bsp);
	assert(count);

	polygons = (polygon_t *)calloc(bsp->face_count, sizeof(polygon_t));
	
	if (!polygons)
		return NULL;

	*count = bsp->face_count;

	for (i = 0; i < bsp->face_count; i++)
	{
		p = &polygons[i];

		// Save the face.
		face = &bsp->faces[i];
		p->face = face;

		// Save the plane the face is in.
		p->plane = &bsp->planes[face->planenum];

		// Allocate memory for the edges and vertices.
		p->edges = (dedge_t **)calloc(face->numedges, sizeof(dedge_t *));
		p->vertices = (dvertex_t **)calloc(face->numedges, sizeof(dvertex_t));

		// Get the edges of the face.
		for (e = 0; e < face->numedges; e++)
		{
			// The index in the ledge list can also be negative,
			// the sign is just an indication which way we should walk
			// around the edge of the face. We should always use the absolute
			// value as the actual index.
			lindex = bsp->ledges[face->firstedge + e];

			if (lindex > 0)
			{					
				edge = &bsp->edges[lindex];
				v = &bsp->vertices[edge->v[0]];
			}
			else
			{
				edge = &bsp->edges[-lindex];
				v = &bsp->vertices[edge->v[1]];
			}

			// Add the vertex to the list of face vertices.
			p->vertices[e] = v;
			p->vertex_count++;

			// Save the edge.
			p->edges[e] = edge;
			p->edge_count++;
		}
	}

	return polygons;
}

unsigned char *convert_8bit_to_24bit(const miptex_t *mip, const byte *texture, int mip_level)
{
	size_t i;
	size_t color_index;
	size_t width = mip->width / (1 << mip_level);
	size_t height = mip->height / (1 << mip_level);
	size_t original_size = width * height;
	size_t expanded_size = (original_size * 3);
	byte *expanded_data = (byte *)malloc(expanded_size);

	// Convert the image from indexed palette values to RGB values for each pixel.
	for (i = 0; i < original_size; i++)
	{
		color_index = texture[i];
		memcpy(&expanded_data[i*3], &quake_pallete[color_index], 3);
	}

	return expanded_data;
}

void upload_texture(texture_t *t, const miptex_t *mip, const byte *texture_data, int mip_level)
{
	size_t i;
	size_t width = mip->width / (1 << mip_level);
	size_t height = mip->height / (1 << mip_level);
	
	byte *expanded_data = convert_8bit_to_24bit(mip, texture_data, mip_level);

	glBindTexture(GL_TEXTURE_2D, t->texnum);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Upload the image to OpenGL.
	glTexImage2D(GL_TEXTURE_2D, mip_level, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, expanded_data);

	free(expanded_data);
}

#if 0
int read_textures()
{
	size_t i;
	size_t mip_level;
	size_t texnum = 0;
	texture_t *t;
	miptex_t *mip;
	texinfo_t *texinf; 
	unsigned char *texture_data;

	// Read the texture data.
	byte *texdata = get_miptexture_lump_data();
	mipmap_ptrs	= get_miptextures(texdata, &mipmap_count);
	textureinfos = get_texinfos(&texinfo_count);

	// Allocate space for the texture list.
	textures = (texture_t *)calloc(texinfo_count, sizeof(texture_t));

	// Several faces can share a texinfo.
	// Go through each face, and get its texinfo. Only create a new texture_t
	// if the texture hasn't already been loaded.
	for (i = 0; i < face_count; i++)
	{
		texinf = &textureinfos[faces[i].texinfo];
		texnum = texinf->miptex;

		if ((texnum == 0) || textures[texnum].is_loaded)
			continue;

		texinf = &textureinfos[texnum];

		t = &textures[texnum];
		t->texnum = texnum;

		vector_copy(t->s, texinf->vectorS);
		vector_copy(t->t, texinf->vectorT);
		t->distS = texinf->distS;
		t->distT = texinf->distT;

		// Get the pointer to the mipmap.
		mip = mipmap_ptrs[texinf->miptex];
		strcpy(t->name, mip->name);
		t->width = mip->width;
		t->height = mip->height;

		// Upload the texture data to OpenGL.
		for (mip_level = 0; mip_level < MIPLEVELS; mip_level++)
		{
			texture_data = get_texture(texdata, mip, mip_level);
			upload_texture(t, mip, texture_data, mip_level);
			free(texture_data);
		}

		t->is_loaded = 1;
		texture_count++;
	}

	return 1;
}
#endif


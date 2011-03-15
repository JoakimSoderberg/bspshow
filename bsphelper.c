#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include <gl/freeglut.h>
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


void get_polygons()
{
	/*
	for (e = 0; e < face->numedges; e++)
	{
		lindex = ledges[face->firstedge + e];

		if (lindex > 0)
		{					
			edge = &edges[lindex];
			v = &vertices[edge->v[0]];
		}
		else
		{
			edge = &edges[-lindex];
			v = &vertices[edge->v[1]];
		}

		vertex_color = fabs(v->point[2] / (max_z - min_z));
		glColor4f(vertex_color, vertex_color, vertex_color, 0.5);

		glTexCoord2d(0.0, 0.0);
		glTexCoord2d(0.0, 1.0);
		glTexCoord2d(1.0, 0.0);
		glTexCoord2d(1.0, 1.0);
		glVertex3fv(v->point);
	}
	*/
}

void upload_texture(texture_t *t, const miptex_t *mip, const byte *texture_data, int mip_level)
{
	size_t i;
	size_t tex_index;
	int color_index;
	size_t width = mip->width / (int)pow(2.0, mip_level);
	size_t height = mip->height / (int)pow(2.0, mip_level);
	size_t original_size = width * height;
	size_t expanded_size = (original_size * 3);
	byte *expanded_data = (byte *)malloc(expanded_size);
	t->width = width;
	t->height = height;

	// Convert the image from indexed palette values to RGB values for each pixel.
	for (i = 0, tex_index = 0; i < expanded_size; i += 3, tex_index++)
	{
		color_index = texture_data[tex_index];
		memcpy(&expanded_data[i], &quake_pallete[color_index], 3);
	}

	glBindTexture(GL_TEXTURE_2D, t->texnum);

	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexImage2D(GL_TEXTURE_2D, mip_level, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, expanded_data);
	
	/*
	if (t->texnum == 24)
	{
		FILE *f = fopen("texture.raw", "wb");
		fwrite(expanded_data, 1, expanded_size, f);
		fclose(f);
		exit(0);
	}*/

	printf("Uploaded texture %d miplevel %d\n", t->texnum, mip_level);

	free(expanded_data);
}

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

		if (texnum == 0)
			continue;

		if (!textures[texnum].is_loaded)
		{
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
	}

	return 1;
}

### @file	ml-Blender/tamy_material.py
### @brief	structures that represent Tamy material data
### ===========================================================================

from ctypes import *
import bpy
from . import tamy_misc

		
### ===========================================================================
	
### This class represents a texture exported to Tamy
class TamyTexture( Structure ):

	_fields_ = [ ("path", c_char_p) ]
	
	### Constructor
	def __init__( self, texturePath ):
		self.path = texturePath.encode( "utf-8" )
	
		
### ===========================================================================
	
### A class that represents a material
class TamyMaterial( Structure ):
	_fields_ = [("name", c_char_p),
				("ambientColor", tamy_misc.TamyColor),
				("diffuseColor", tamy_misc.TamyColor),
				("specularColor", tamy_misc.TamyColor),
				("normalTextureIndex", c_int),
				("specularTextureIndex", c_int),
				("diffuseTexturesIndices", POINTER(c_int)),
				("diffuseTexturesOperations", POINTER(c_int)),
				("diffuseTexturesOperationFactors", POINTER(c_float)),
				("diffuseTexturesCount", c_int) ]
	
	# -------------------------------------------------------------------------
	
	def __init__( self, material, texturesDict ):
	
		materialName = material.name if material else "None"
		self.name = materialName.encode( "utf-8" )

		if not material:
			self.ambientColor = tamy_misc.TamyColor( 0.0, 0.0, 0.0, 1.0 )
			self.diffuseColor = tamy_misc.TamyColor( 0.0, 0.0, 0.0, 1.0 )
			self.specularColor = tamy_misc.TamyColor( 0.0, 0.0, 0.0, 1.0 )
		else:
			r, g, b = ( material.ambient * material.diffuse_color )[:]
			self.ambientColor = tamy_misc.TamyColor( r, g, b, 1.0  )
			
			r, g, b = material.diffuse_color[:]
			self.diffuseColor = tamy_misc.TamyColor( r, g, b, 1.0 )
			
			r, g, b = material.specular_color[:]
			self.specularColor = tamy_misc.TamyColor( r, g, b, 1.0 )

		slots = self.get_material_image_texslots( material )  # can be None

		self.normalTextureIndex = -1
		self.specularTextureIndex = -1
		
		if slots:			
			# we only want one of each special texture ( i.e. normal, specular and opacity maps )
			normal = [s for s in slots if s.use_map_normal]
			if len( normal ) > 0:
				self.normalTextureIndex, blendMode, blendFactor = self.getTextureSettings( normal[0], texturesDict )
				
			specular = [s for s in slots if s.use_map_specular]
			if len( specular ) > 0:
				self.specularTextureIndex, blendMode, blendFactor = self.getTextureSettings( specular[0], texturesDict )

			# make sure no textures are lost. Everything that doesn't fit
			# into a channel is exported as diffuse texture.

			diffuseTextureSettings = []
			for s in slots:
				if ( s.use_map_color_diffuse ):
					texIdx, blendMode, blendFactor = self.getTextureSettings( s, texturesDict )
					if texIdx >= 0:
						# append a tuple that will contain the texture index, the blend mode and the blend factor value
						diffuseTextureSettings.append( ( texIdx, blendMode, blendFactor ) )

			# copy the diffuse maps to the dedicated export array
			self.diffuseTexturesCount = len( diffuseTextureSettings )
			self.diffuseTexturesIndices = ( c_int * self.diffuseTexturesCount )()
			self.diffuseTexturesOperations = ( c_int * self.diffuseTexturesCount )()
			self.diffuseTexturesOperationFactors = ( c_float * self.diffuseTexturesCount )()
			for i in range( self.diffuseTexturesCount ):
				self.diffuseTexturesIndices[i], self.diffuseTexturesOperations[i], self.diffuseTexturesOperationFactors[i] = diffuseTextureSettings[i]
				
	# -------------------------------------------------------------------------
	
	### Returns an export index assinged to this texture, as well as the blend mode and the associated blend factor
	def getTextureSettings( self, textureSlot, texturesDict ):
	
		textureIdx = -1
		blendMode = 16			# use the MIX mode by default
		blendFactor = 1.0
		
		if ( textureSlot.texture.type == 'IMAGE' ):
			texturePath = bpy.path.abspath( textureSlot.texture.image.filepath )

			# check if the texture was mapped and exported
			if ( texturePath in texturesDict ):
				textureIdx = texturesDict[texturePath]
			else:
				textureIdx = len( texturesDict )
				texturesDict[texturePath] = textureIdx

			# check the blend mode and the blend factor
			# The numerals assigned to blend modes need to be in sync with values from the MNRGBOperation enum ( core-Renderer/MNMixRGBs.h )
			if ( textureSlot.blend_type == 'MIX' ):
				blendMode = 16
			elif ( textureSlot.blend_type == 'ADD' ):
				blendMode = 15
			elif ( textureSlot.blend_type == 'SUBTRACT' ):
				blendMode = 13
			elif ( textureSlot.blend_type == 'MULTIPLY' ):
				blendMode = 14
			elif ( textureSlot.blend_type == 'SCREEN' ):
				blendMode = 12
			elif ( textureSlot.blend_type == 'DIFFERENCE' ):
				blendMode = 10
			elif ( textureSlot.blend_type == 'DIVIDE' ):
				blendMode = 11
			elif ( textureSlot.blend_type == 'DARKEN' ):
				blendMode = 9
			elif ( textureSlot.blend_type == 'LIGHTEN' ):
				blendMode = 8
			elif ( textureSlot.blend_type == 'HUE' ):
				blendMode = 4
			elif ( textureSlot.blend_type == 'SATURATION' ):
				blendMode = 3
			elif ( textureSlot.blend_type == 'VALUE' ):	# we don't have that operation implemented, so we'rejust gonna use the COLOR operation
				blendMode = 2
			elif ( textureSlot.blend_type == 'COLOR' ):
				blendMode = 2
			elif ( textureSlot.blend_type == 'SOFT_LIGHT' ):
				blendMode = 1
			elif ( textureSlot.blend_type == 'LINEAR_LIGHT' ):
				blendMode = 0

			# check the blend mode and the blend factor
			blendFactor = textureSlot.default_value
			
		# This is not a correct texture, or it hasn't been exported - anyway, it doesn't have an index assigned
		return textureIdx, blendMode, blendFactor

	# -------------------------------------------------------------------------
	
	### This helper functions enumerates all occupied texture slots
	def get_material_image_texslots( self, material ):
	
		# blender utility func.
		if material:
			return [s for s in material.texture_slots if s and s.texture.type == 'IMAGE' and s.texture.image]

		return []
	
### ===========================================================================

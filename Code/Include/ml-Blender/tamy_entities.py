### @file	ml-Blender/tamy_entities.py
### @brief	structures that represent various Tamy entities
### ===========================================================================

from ctypes import *
from . import tamy_misc
from . import tamy_mesh


### ===========================================================================
'''
One very important note - these structures represent various SpatialEntities,
and as such, they share two common components - a local matrix, and index of
their parent entity ( if applicable ).
The problem is that we can't use inheritance here - we're exporting to C,
and there's no inheritance there. That's why each structure NEEDS to keep
its own set of fields that represent those:
	- ( "localMatrix", tamy_misc.TamyMatrix ),
	- ( "parentIdx", c_int )
	
Python is less demanding in terms of type strictness, all it needs is a method,
and if an object has it - it calls it.
I took advantage of that and provided only one piece of code that deals in
setting all entity transforms and parent indices, but it requires all entity
classes to be equipped with 3 methods:
	- def set_parent( self, parentEntityIdx ):
	- def set_matrix( self, matrix ):
	- def export( self, tamyExportModule )
'''

### ===========================================================================

# A class that represents a basic Entity
class TamyEntity( Structure ):
	_fields_= [	( "name", c_char_p ),
				( "localMatrix", tamy_misc.TamyMatrix ),
				( "parentIdx", c_int ) ]
				
	def __init__( self, name, worldTransform ):
	
		self.name = name.encode( "utf-8" )
		self.parentIdx = -1
		self.localMatrix = tamy_misc.TamyMatrix( worldTransform )
			
	def set_parent( self, parentEntityIdx ):
		self.parentIdx = parentEntityIdx
		
	def set_matrix( self, matrix ):
		self.localMatrix = tamy_misc.TamyMatrix( matrix )
		
	def export( self, tamyExportModule ):
		tamyExportModule.export_basic_entity( self )
	
### ===========================================================================

### A class that represents a GeometryComponent
class TamyGeometry( Structure ):
	
	_fields_= [	( "name", c_char_p ),
				( "localMatrix", tamy_misc.TamyMatrix ),
				( "parentIdx", c_int ),
				( "mesh", tamy_mesh.TamyMesh ) ]
				
	def __init__( self, mesh, name, worldTransform, parentEntityIdx ):
	
		self.name = name.encode( "utf-8" )
		self.parentIdx = parentEntityIdx
				
		self.localMatrix = tamy_misc.TamyMatrix( worldTransform )
		self.mesh = mesh
			
	def set_parent( self, parentEntityIdx ):
		self.parentIdx = parentEntityIdx
		
	def set_matrix( self, matrix ):
		self.localMatrix = tamy_misc.TamyMatrix( matrix )
		
	def export( self, tamyExportModule ):
		tamyExportModule.export_geometry_component( self )
			
### ===========================================================================

### A helper structure that represents a light
###
### Parameter 'type' describes the type of light exported:
###   0 - DIRECTIONAL
###   1 - POINT
###   2 - SPOTLIGHT
###   3 - HEMI
###   4 - AREA
###
### Parameter 'falloffType' values:
###   0 - CONSTANT
###   1 - INVERSE_LINEAR
###   2 - INVERSE_SQUARE
class TamyLight( Structure ):

	_fields_= [ ( "name", c_char_p ),
				( "type", c_int ),
				( "lightColor", tamy_misc.TamyColor ),
				( "energy", c_float ),
				( "distance", c_float ),
				( "useSpecular", c_bool ),
				( "castShadows", c_bool ),
				( "shadowColor", tamy_misc.TamyColor ),
				( "linearAttenuation", c_float),
				( "quadraticAttenuation", c_float),
				( "falloffType", c_int ),
				( "spotAttenuation", c_float ),
				( "spotAngle", c_float ),
				( "spotlightCastsOnlyShadows", c_bool ),
				( "localMatrix", tamy_misc.TamyMatrix ),
				( "parentIdx", c_int ) ]
	
	### Constructor
	###
	### @param lightName
	### @param blenderLight
	### @param worldTransform ( Matrix )
	def __init__( self, lightName, blenderLight, worldTransform ):
	
		self.name = lightName.encode( "utf-8" )
		self.parentIdx = -1
		self.localMatrix = tamy_misc.TamyMatrix( worldTransform )
		
		self.lightColor = tamy_misc.TamyColor( blenderLight.color[0], blenderLight.color[1], blenderLight.color[2], 1.0 ) 
		self.energy = blenderLight.energy
		self.useSpecular = blenderLight.use_specular
		self.distance = blenderLight.distance
		
		lightTypeDict = { 'SUN' : 0, 'POINT' : 1, 'SPOT' : 2, 'HEMI' : 3, 'AREA' : 4 }
		self.type = lightTypeDict[blenderLight.type]
	
		# Spot light specific initialization
		if blenderLight.type == 'SPOT':
			self.spotAttenuation = blenderLight.spot_blend
			self.spotAngle = blenderLight.spot_size
			self.spotlightCastsOnlyShadows = blenderLight.use_only_shadow
			
		# Attenuated lights specific initialization	
		if blenderLight.type in { 'POINT', 'SPOT' }:
			self.linearAttenuation = blenderLight.linear_attenuation
			self.quadraticAttenuation = blenderLight.quadratic_attenuation
			
			if blenderLight.falloff_type == 'CONSTANT':
				self.falloffType = 0
			elif blenderLight.falloff_type == 'INVERSE_LINEAR':
				self.falloffType = 2
			else:
				# all other falloff types are treated as quadratic
				self.falloffType = 1
		
		
		# Shadow casting lights specific initialization
		if blenderLight.type != 'HEMI':
			self.shadowColor = tamy_misc.TamyColor( blenderLight.shadow_color[0], blenderLight.shadow_color[1], blenderLight.shadow_color[2], 1.0 )
			self.castShadows = blenderLight.shadow_method != 'NOSHADOW'
			
				
	def set_parent( self, parentEntityIdx ):
		self.parentIdx = parentEntityIdx
		
	def set_matrix( self, matrix ):
		self.localMatrix = tamy_misc.TamyMatrix( matrix )
		
	def export( self, tamyExportModule ):
		tamyExportModule.export_light_entity( self )

### ===========================================================================

### This entity represents a SkeletonComponent.
class TamySkeletonComponent( Structure ):
	
	_fields_= [ ( "name", c_char_p ) ,
				( "localMatrix", tamy_misc.TamyMatrix ),
				( "parentIdx", c_int ),
				( "skeletonIdx", c_int ) ]
	
	### Constructor
	def __init__( self, armature, skeletonIdx, worldTransform, parentEntityIdx ):
		self.name = armature.name.encode( "utf-8" )
		self.skeletonIdx = skeletonIdx
		self.parentIdx = parentEntityIdx
		self.localMatrix = tamy_misc.TamyMatrix( worldTransform )
		
		# store the armature reference in a non-serializable class member for future reference - 
		# - we're going to need it when we sample animation
		self.blenderArmature = armature
		
	def set_parent( self, parentEntityIdx ):
		self.parentIdx = parentEntityIdx
		
	def set_matrix( self, matrix ):
		self.localMatrix = tamy_misc.TamyMatrix( matrix )
		
	def export( self, tamyExportModule ):
		tamyExportModule.export_skeleton_component( self )
		
### ===========================================================================

### This structure holds global rendering settings.
class TamyWorld( Structure ):

	_fields_= [ ( "ambientLightColor", tamy_misc.TamyColor ) ]
	
	### Constructor
	###
	### @param blenderWorld ( bpy.types.World(ID) )
	def __init__( self, blenderWorld ):
		self.ambientLightColor = tamy_misc.TamyColor( blenderWorld.ambient_color[0], blenderWorld.ambient_color[1], blenderWorld.ambient_color[2], 1.0 )

### ===========================================================================

		
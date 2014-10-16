### @file	ml-Blender/tamy_scene.py
### @brief	structures that represent Tamy scene data
### ===========================================================================

import bpy
from ctypes import *
from . import tamy_mesh
from . import tamy_material
from . import tamy_entities
from . import tamy_animation
from . import tamy_misc

	
### ===========================================================================

#### A class that represents a Model
class TamyScene:
	
	def __init__( self, sceneName ):
		self.sceneName = sceneName
		self.textures = []
		self.entities = []
		self.components = []
		self.materials = []
		self.armatures = []
		self.animations = []
		
		self.materialsDict = {} # pairs of ( blenderMaterial : exportedMaterialIndex )
		self.entitiesDict = {}  # pairs of ( blenderObject : exportedEntityIndex )
		self.texturesDict = {}
		self.armaturesDict = {} # pairs of ( blenderArmature : exportedArmatureIndex )
		
	def export( self, filesystemRoot, exportDir, exportSettings ):
		
		import os
		tamyExportModule = cdll.LoadLibrary( "%s/addons/io_export_tamy/ml-Blender.dll" % os.environ['BLENDER_SCRIPTS'] )
		
		if tamyExportModule is None:
			print( "\nERROR: ml-Blender.dll not found" )
			return
			
		sceneNodesCount = len( self.entities ) + len( self.components )
		tamyExportModule.begin_export( filesystemRoot.encode( "utf-8" ), exportDir.encode( "utf-8" ), sceneNodesCount, exportSettings )
		
		# 1. export the textures
		texturesCount = len( self.textures )
		exportTexturesArr = ( tamy_material.TamyTexture * texturesCount )()
		for i in range( texturesCount ):
			exportTexturesArr[i] = self.textures[i]
			
		tamyExportModule.export_textures( exportTexturesArr, texturesCount )
		
		
		# 2. export the materials
		materialsCount = len( self.materials )
		exportMaterialsArr = ( tamy_material.TamyMaterial * materialsCount )()
		for i in range( materialsCount ):
			exportMaterialsArr[i] = self.materials[i]
		
		tamyExportModule.export_materials( exportMaterialsArr, materialsCount )

		# 3. export skeletons
		# Create skeletons used in the scene
		skeletonsCount = len( self.armatures )
		skeletonsArr = ( tamy_animation.TamySkeleton * skeletonsCount )()
		for i in range( skeletonsCount ):
			armature = self.armatures[i]
			skeletonsArr[i] = tamy_animation.TamySkeleton( armature )
		tamyExportModule.export_skeletons( skeletonsArr, skeletonsCount )
		
		# 4. export animations
		animationsCount = len( self.animations )
		animationsArr = ( tamy_animation.TamyAnimation * animationsCount )()
		for i in range( animationsCount ):
			animatedObject, animation, animatedEntityIdx = self.animations[i]
			exportedAnimation = tamy_animation.TamyAnimation( self.scene, animatedObject, animation, animatedEntityIdx, self.armatures )
			animationsArr[i] = exportedAnimation
		tamyExportModule.export_animations( animationsArr, animationsCount )
		
		# 5. export individual entities.
		#
		# CAUTION: Export order is crucial here. The C counterpart also knows that and
		# it will place the exported entities in the exact order in which there are exported.
		# But in order for the scene to be assembled correctly later on, the order needs to be 
		# preserved, since individual entities reference themselves not by pointers ( since we can't
		# really do that when exporting to C ), but by their indices in the entities[] array.
		for blenderObj, tamyEntity in self.entities:
			tamyEntity.export( tamyExportModule )
			
		# 6. export components
		for tamyComponent in self.components:
			tamyComponent.export( tamyExportModule )
				
			# we won't be exporting 'ARMATURE' entities - we consider it a storage for skeletons
			
		# 6. export world settings
		tamyExportModule.export_world_settings( self.worldSettings ) 
		
		# 7. assemble the scene
		tamyExportModule.assemble_scene( self.sceneName.encode( "utf-8" ) )
		
	### ===========================================================================

	### Builds the entities present in the scene and compiles the final scene that will be exported to blender
	def compile_scene( self, context, bUseSelection, selectedArmatureName ):

		self.scene = context.scene

		if bUseSelection:
			self.objects = (ob for ob in self.scene.objects if ob.is_visible(self.scene) and ob.select)
		else:
			self.objects = (ob for ob in self.scene.objects if ob.is_visible(self.scene))
			
		self.build_entities()
		
		# Create texture entires
		self.textures = [ None for i in range( len( self.texturesDict ) ) ]
		for texturePath in self.texturesDict.keys():
			textureIdx = self.texturesDict[texturePath]
			self.textures[textureIdx] = tamy_material.TamyTexture( texturePath )
			
		# Set the parenting hierarchy
		self.resolve_hierarchy()
		
		# Create the world
		self.worldSettings = tamy_entities.TamyWorld( self.scene.world )
		
		# Prepare animations for export
		if selectedArmatureName == 'None':
			self.compile_cutscene_animation()
		else:
			self.compile_animations_lib( selectedArmatureName ) 
		
			
	### ===========================================================================
	
	def build_entities( self ):
		
		from bpy_extras.io_utils import create_derived_objects, free_derived_objects
			
		for ob in self.objects:	
						
			# The object represents a mesh
			if ob.type == 'MESH':
			
				# get derived objects
				free, derived = create_derived_objects( self.scene, ob )

				if derived is None:
					continue
					
				# create a binding entity
				parentEntityIdx = -1
				parentArmatureObject = ob.find_armature()
				if parentArmatureObject is not None:						
					parentEntityIdx = self.add_armature_entity( parentArmatureObject )
				else:
					parentEntityIdx = self.add_node_entity( ob )
				
				# create meshes
				for obDerived, objectMtx in derived:
					try:
						derivedBlenderMesh = obDerived.to_mesh( self.scene, False, 'PREVIEW' )
					except:
						derivedBlenderMesh = None
					
					if derivedBlenderMesh:
						self.map_materials( derivedBlenderMesh )
					
					meshes = []
					meshName = "%s" % ob.name
					tamy_mesh.create_tamy_meshes( meshName, obDerived, derivedBlenderMesh, self.materialsDict, meshes )
					
					self.add_geometry_component( obDerived, meshes, parentEntityIdx )
					
					# cleanup
					if free:
						free_derived_objects( derivedBlenderMesh )
			
			# The object represents a light				
			elif ob.type == 'LAMP':
				self.add_light_entity( ob )
				
			elif ob.type == 'ARMATURE':
				self.add_armature_entity( ob)
	
	### -------------------------- Entity factory methods -------------------------

	def map_entity( self, entityObject, tamyEntity ):
		
		if tamyEntity is not None:
			entityIdx = len( self.entities )
			self.entitiesDict[entityObject] = entityIdx					
			self.entities.append( ( entityObject, tamyEntity ) )
			
			return entityIdx
		else:
			return -1
		
	def map_component( self, tamyComponent ):
	
		if tamyComponent is not None:				
			self.components.append( tamyComponent )

		
	def add_node_entity( self, object ):
		
		if object in self.entitiesDict:
			return self.entitiesDict[object]
			
		tamyEntity = tamy_entities.TamyEntity( object.name, object.matrix_local )
		entityIdx = self.map_entity( object, tamyEntity )
		return entityIdx

		
	def add_armature_entity( self, object ):
	
		if object in self.entitiesDict:
			return self.entitiesDict[object]
	
		armature = object.data
		skeletonIdx = self.map_armature( armature )
		
		tamyEntity = tamy_entities.TamyEntity( object.name, object.matrix_local )
		entityIdx = self.map_entity( object, tamyEntity )
		
		skeletonComponent = tamy_entities.TamySkeletonComponent( armature, skeletonIdx, object.matrix_local, entityIdx )
		self.map_component( skeletonComponent )
		
		return entityIdx
	
	
	def add_geometry_component( self, object, meshes, parentEntityIdx ):
	
		for mesh in meshes:
			skeletonComponent = tamy_entities.TamyGeometry( mesh, object.name, object.matrix_local, parentEntityIdx )
			self.map_component( skeletonComponent )
			
	
	def add_light_entity( self, object ):
	
		if object in self.entitiesDict:
			return self.entitiesDict[object]
		
		tamyEntity = tamy_entities.TamyLight( object.name, object.data, object.matrix_local )
		entityIdx = self.map_entity( object, tamyEntity )		
		return entityIdx
		

	### ===========================================================================	

	def map_materials( self, derivedBlenderMesh ):
			
		for material in derivedBlenderMesh.materials:
			if ( material is not None ) and ( ( material in self.materialsDict ) == False ):
				self.materialsDict[material] = len( self.materials )
				self.materials.append( tamy_material.TamyMaterial( material, self.texturesDict ) )
				
	### ===========================================================================	

	def map_armature( self, armature ):
		
		armatureIdx = -1
		
		if ( armature in self.armaturesDict ) == False:
			armatureIdx = len( self.armatures )
			self.armaturesDict[armature] = armatureIdx
			self.armatures.append( armature )
		else:
			armatureIdx = self.armaturesDict[armature]
			
		return armatureIdx
	
		
	### ===========================================================================

	import mathutils

	### @param entities - it's a tuple: ( blenderObject, TamyEntity, parentEntityIdx{ defined only for components } )
	def resolve_hierarchy( self ):
		
		for i in range( len( self.entities ) ):
			blenderObject = self.entities[i][0]
			tamyEntity = self.entities[i][1]
			
			if ( blenderObject.parent_type != 'OBJECT' and blenderObject.parent_type !='BONE' and blenderObject.parent_type !='LATTICE' ):
				# Object can't be a part of any hierearchy, but nonetheless set its local matrix
				continue
			
			if blenderObject.parent is None:
				# it's the root of a hierarchy - therefore its local matrix should be multipliend by the desired global
				# matrix to convert its coordinate system to Tamy
				continue
			
			# set the entity's parent index
			parentEntityIdx = self.entitiesDict[ blenderObject.parent ]			
			tamyEntity.set_parent( parentEntityIdx )
				
			
	### ===========================================================================
	
	### In the CUTSCENE mode, we're exporitng animations currently assigned to
	### the selected object
	def compile_cutscene_animation( self ):
		
		for entityObject in self.entitiesDict.keys():
			animatedEntityIdx = self.entitiesDict[entityObject]

			# Check if the object is animated
			if entityObject.animation_data is None:
				return
		
			# register an animation along with the index of an entity it animates
			blenderAnimation = entityObject.animation_data
			if ( blenderAnimation is not None ) and ( blenderAnimation.action is not None ):
				self.animations.append( ( entityObject, blenderAnimation.action, animatedEntityIdx ) )

	### ===========================================================================
	
	### In the ANIMATION_LIB mode, we're selecting a single armature and are going
	# to export all animations using it
	def compile_animations_lib( self, selectedArmatureName ):
		
		# find the export index of the armature
		selectedArmature = self.scene.objects[selectedArmatureName]
		if selectedArmature is None:
			return

		armatureExportIdx = self.entitiesDict[selectedArmature]

		# add all present animation actions
		for animAction in bpy.data.actions:
			self.animations.append( ( selectedArmature, animAction, armatureExportIdx ) )

	### ===========================================================================
	

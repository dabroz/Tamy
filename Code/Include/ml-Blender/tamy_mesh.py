### @file	ml-Blender/tamy_mesh.py
### @brief	structures that represent Tamy mesh data
### ===========================================================================

from ctypes import *
from array import *
import bpy
import copy

### ===========================================================================

### A helper structure representing a single mesh vertex
class TamyVertex( Structure ):

	_fields_ = [("x", c_float),
				("y", c_float),
				("z", c_float),
				("w", c_float),
				("nx", c_float),
				("ny", c_float),
				("nz", c_float),
				("tanx", c_float),
				("tany", c_float),
				("tanz", c_float),
				("u", c_float),
				("v", c_float) ]
	
	def __init__( self, coords, normal, uv=(0,0) ):
	
		self.x, self.y, self.z = coords[:]
		self.w = 1
		self.nx, self.ny, self.nz = normal[:]
		self.tanx, self.tany, self.tanz, = (0, 0, 0)
		self.u, self.v =  uv[:]
		
	def __init__( self, blenderVertex ):
	
		self.x, self.y, self.z = blenderVertex.co[:]
		self.w = 1
		self.nx, self.ny, self.nz = blenderVertex.normal[:]
		self.tanx, self.tany, self.tanz = ( 0, 0, 0 )
		self.u, self.v =  ( 0, 0 )
		
	def __copy__( self ):
		result = TamyVertex( ( self.x, self.y, self.z ), ( self.nx, self.ny, self.nz ), ( self.u, self.v ) )
		return result
		
	def setNormal( self, normalTuple ):
		self.nx, self.ny, self.nz = normalTuple[:]
		
	def setUV( self, u, v ):
		self.u = u
		self.v = -v
		
### ===========================================================================

### A helper structure representing a single exportable mesh face		
class TamyFace( Structure ):

	_fields_ = [( "idx", c_short * 3)]
	
	def __init__( self, indices ):
		self.idx[0], self.idx[1], self.idx[2] = indices[:]
		
	### Compares two faces
	def compare( self, otherFace ):
		if ( self.idx[0] != otherFace.idx[0] ):
			return False
			
		if ( self.idx[1] == otherFace.idx[1] ):
			return False
			
		if ( self.idx[2] == otherFace.idx[2] ):
			return False
		
		return True	

### ===========================================================================
		
### A helper structure that stores vertex weights
class TamyVertexWeights( Structure ):

	_fields_ = [("weight0", c_float),
				("weight1", c_float),
				("weight2", c_float),
				("weight3", c_float),
				("idxBone0", c_float),
				("idxBone1", c_float),
				("idxBone2", c_float),
				("idxBone3", c_float) ]
	
	def __init__( self, weights, boneIndices ):
		self.weight0, self.weight1, self.weight2, self.weight3 = weights[:]
		self.idxBone0, self.idxBone1, self.idxBone2, self.idxBone3 = boneIndices[:]
		
	def __init__( self, blenderVertex, vertexGroupToBoneDict ):
		# We're not gonna map vertex group indices to particular bones, because in order to do that,
		# we'd have to provide an armature instance the entity is attached to.
		# Instead, we'll assume that group indices correspond to bone indices - this
		# puts a constraint that every entity uses only one skeleton at most
		
		self.weight0 = 0
		self.weight1 = 0
		self.weight2 = 0
		self.weight3 = 0
		
		self.idxBone0 = -1
		self.idxBone1 = -1
		self.idxBone2 = -1
		self.idxBone3 = -1		
		
		# sort the groups according to their weights - we want the highest influences to come first
		sortedVertexGroups = sorted( blenderVertex.groups, key=lambda vertexGroup: vertexGroup.weight, reverse=True )	
		
		# filter the groups - leave behind only the ones that actually map onto existing bones and the ones 
		# that make up to a total of 4 influences per bone, and that actually contribute to bone's transform 
		# ( ones with weights > 0 )
		vertexGroupsCount = 0
		filteredVertexGroups = []
		for vertexGroup in sortedVertexGroups:
			
			groupIdx = vertexGroup.group
			if groupIdx not in vertexGroupToBoneDict:
				continue
				
			if vertexGroup.weight <= 0:
				continue
			
			filteredVertexGroups.append( vertexGroup )
			
		
		weightsSum = 0
		vertexGroupsCount = len( filteredVertexGroups )
		
		if ( vertexGroupsCount == 0 ):
			return
		
		if vertexGroupsCount > 0:
			vertexGroup = filteredVertexGroups[0]
			self.idxBone0 = vertexGroupToBoneDict[vertexGroup.group]
			self.weight0 = vertexGroup.weight
			weightsSum = weightsSum + self.weight0
		
		if vertexGroupsCount > 1:
			vertexGroup = filteredVertexGroups[1]
			self.idxBone1 = vertexGroupToBoneDict[vertexGroup.group]
			self.weight1 = vertexGroup.weight
			weightsSum = weightsSum + self.weight1
			
		if vertexGroupsCount > 2:
			vertexGroup = filteredVertexGroups[2]
			self.idxBone2 = vertexGroupToBoneDict[vertexGroup.group]
			self.weight2 = vertexGroup.weight
			weightsSum = weightsSum + self.weight2
		
		if vertexGroupsCount > 3:
			vertexGroup = filteredVertexGroups[3]
			self.idxBone3 = vertexGroupToBoneDict[vertexGroup.group]
			self.weight3 = vertexGroup.weight
			weightsSum = weightsSum + self.weight3
			
		if vertexGroupsCount > 4:
			print( "This vertex has more than 4 bones influencing its transform" )
			
		# normalize the weights
		self.weight0 = self.weight0 / weightsSum
		self.weight1 = self.weight1 / weightsSum
		self.weight2 = self.weight2 / weightsSum
		self.weight3 = self.weight3 / weightsSum
		
	def hasAnyWeigths( self ):
		return self.idxBone0 >= 0 or self.idxBone1 >= 0 or self.idxBone2 >= 0 or self.idxBone3 >= 0

### ===========================================================================

### A helper structure representing a mesh vertex used in the process of mesh restructuring
class TempMeshVertex( Structure ):
		
	def __init__( self, tamyVertex, tamyVertexWeights ):	
		self.vertex = TamyVertex( ( tamyVertex.x, tamyVertex.y, tamyVertex.z ), ( tamyVertex.nx, tamyVertex.ny, tamyVertex.nz ), ( tamyVertex.u, tamyVertex.v ) )
		self.weights = TamyVertexWeights( ( tamyVertexWeights.weight0, tamyVertexWeights.weight1, tamyVertexWeights.weight2, tamyVertexWeights.weight3 ), ( tamyVertexWeights.idxBone0, tamyVertexWeights.idxBone1, tamyVertexWeights.idxBone2, tamyVertexWeights.idxBone3 ) )

	def __init__( self, blenderVertex, vertexGroupToBoneDict ):
		self.vertex = TamyVertex( blenderVertex )
		self.weights = TamyVertexWeights( blenderVertex, vertexGroupToBoneDict )
		
	def __copy__( self ):
		result = TempMeshVertex( self.vertex, self.weights )
		return result
		
	def setNormal( self, normalTuple ):
		self.vertex.nx, self.vertex.ny, self.vertex.nz = normalTuple[:]
		
	def setUV( self, u, v ):
		self.vertex.u = u
		self.vertex.v = -v
		
### ===========================================================================

### A class that represents a mesh
class TamyMesh( Structure ):

	_fields_= [	( "name", c_char_p ),
				( "verticesList", POINTER( TamyVertex ) ),
				( "facesList", POINTER( TamyFace ) ),
				( "vertexWeightsList", POINTER( TamyVertexWeights ) ),
				( "materialsList", POINTER( c_int ) ),
				( "facesCount", c_int ),
				( "verticesCount", c_int ),
				( "materialsCount", c_int ),
				( "hasVertexWeights", c_bool ) ]
				
	# verticesList : TamyVertex[]
	# facesList : TamyFace[]	- array of triples
	
	def __init__( self, name ):
		self.name = name.encode( "utf-8" )
		self.facesCount = 0
		self.verticesCount = 0
		self.materialCount = 0
		self.hasVertexWeights = False
	
	# -------------------------------------------------------------------------	
	
	### Sets the faces of the mesh
	def setFaces( self, faces ):
		
		self.facesCount = len( faces )
		self.facesList = ( TamyFace * self.facesCount )()
		
		i = 0
		for face in faces:
			self.facesList[i] = face
			i += 1		
		
	# -------------------------------------------------------------------------
		
	### Compares two meshes in terms of the faces they use. If they have exactly same faces, the comparison result is positive
	def compareFaces( self, otherMesh ):
		
		# an early out - if the number of faces in the two meshes don't match, they're not equal
		if ( self.facesCount != otherMesh.facesCount ):
			return False
		
		for i in range( self.facesCount ):
			if self.facesList[i].compare( otherMesh.facesList[i] ) == False:
				# the meshes are not identical
				return False
				
		# the meshes are identical
		return True
		
	# -------------------------------------------------------------------------
		
	### Adds a new material to the mesh
	def addMaterial( self, materialIdx ):
		newMaterialsList = ( c_int * ( self.materialsCount + 1 ) )()
		
		for i in range( self.materialsCount ):
			newMaterialsList[i] = self.materialsList[i]
		
		self.materialsList = newMaterialsList
		self.materialsList[ self.materialsCount ] = materialIdx
		self.materialsCount += 1

	# -------------------------------------------------------------------------
		
	### Merges materials from both meshes, storing them in this mesh instance
	def mergeMaterials( self, otherMesh ):
		newSize = self.materialsCount + otherMesh.materialsCount
		newMaterialsList = ( c_int * newSize )()
		
		for i in range( self.materialsCount ):
			newMaterialsList[i] = self.materialsList[i]
		
		for i in range( self.materialsCount, newSize ):
			newMaterialsList[i] = otherMesh.materialsList[i - self.materialsCount]
			
		self.materialsList = newMaterialsList
		self.materialsCount = newSize
	
	# -------------------------------------------------------------------------
	
	### Maps material indices to the ones from the specified materials dictionary
	###
	### It's required that the `materialsExportOrderDict` should contain pairs ( Material : index ),
	### where the `index` stands for the ordinal number with which the material will be exported
	def mapMaterials( self, originalMeshMaterials, materialsExportOrderDict ):
	
		originalMatsCount = len( originalMeshMaterials )
		
		for i in range( self.materialsCount ):
		
			blenderMatIdx = self.materialsList[i]
			
			if blenderMatIdx < originalMatsCount:
				blenderMaterial = originalMeshMaterials[blenderMatIdx]
				mappedMaterialIdx = materialsExportOrderDict[blenderMaterial]
				self.materialsList[i] = mappedMaterialIdx
			else:
				self.materialsList[i] = -1
		
	# -------------------------------------------------------------------------
		
	### Sets mesh vertices nad adjusts face indices to index into the local array
	###
	### @param vertices : array< TempMeshVertex >
	def setVertices( self, vertices ):
		
		tmpVerticesList = []
		indicesMap = []
		
		for i in range( len( vertices ) ):
			indicesMap.append( -1 )
		
		# map the vertices
		for i in range( self.facesCount ):
			face = self.facesList[i]
			
			for j in range( 3 ):
				vertexIdx = face.idx[j]
				
				if ( indicesMap[ vertexIdx ] < 0 ):
					# map only the unmapped index
					remappedVtxIdx = len( tmpVerticesList )
					indicesMap[ vertexIdx ] = remappedVtxIdx
					tmpVerticesList.append( vertices[ vertexIdx ] )
				
		# Copy the mapped vertices to our local structure. Copy vertex weigths as well
		self.hasVertexWeights = False
		self.verticesCount = len( tmpVerticesList )
		self.verticesList = ( TamyVertex * self.verticesCount )()
		self.vertexWeightsList = ( TamyVertexWeights * self.verticesCount )()
		i = 0
		for vtx in tmpVerticesList:
			self.verticesList[i] = vtx.vertex
			self.vertexWeightsList[i] = vtx.weights
			
			if vtx.weights.hasAnyWeigths():
				self.hasVertexWeights = True
				
			i += 1
			
		# remap face indices
		for i in range( self.facesCount ):
			face = self.facesList[i]
			for j in range( 3 ):
				face.idx[j] = indicesMap[ face.idx[j] ]
				
		
### ===========================================================================

### Since Blender meshes are not compatible with Tamy meshes in terms of
### the number of vertices per face they may have, and the number of materials per face
### they may use ( Tamy assumes a mesh uses the same number of materials to render all of its faces ),
### we need to split them into sub meshes before we can export them to Tamy.
###
### This method accomplishes that, it splits the meshes and appends them on the export list.
###
### It's required that the `materialsExportOrderDict` should contain pairs ( Material : index ),
### where the `index` stands for the ordinal number with which the material will be exported
###
### @return		'True' if the defined meshes create a skin, 'False' otherwise
def create_tamy_meshes( meshName, parentObject, blenderMesh, materialsExportOrderDict, outMeshes ):

	# create a dictionary that maps vertex groups to particular bones
	vertexGroupToBoneDict = {} # { vertex_group_index, bone_index }
	create_vertex_groups_dict( parentObject, vertexGroupToBoneDict )
	
	# create a copy of all vertices and face indices - there's a possibility we'll be doing some splitting etc
	# and we might introduce new vertices as we go along
	vertices = []
	for blenderVtx in blenderMesh.vertices:
		vertices.append( TempMeshVertex( blenderVtx, vertexGroupToBoneDict ) )
		
	faces = []	
	for poly in blenderMesh.polygons:
		faces.append( poly.vertices )
	
	# create a map of UV coordinates assigned to each vertex
	vertices, faces = assign_uv_coords( blenderMesh, vertices, faces )
	
	# split the non-triangular faces
	materialPerFace = []
	faces = split_polygons_into_triangles( blenderMesh.polygons, faces, materialPerFace, vertices )
	
	# now - create Tamy meshes, each dedicated to a single material
	singleMaterialMeshes = []
	materialsSet = set( materialPerFace )
	for checkedMatIdx in materialsSet:
	
		newMeshName = "%s_%d" % ( meshName, len( singleMaterialMeshes ) )
		newMesh = TamyMesh( newMeshName )
		
		singleMaterialMeshes.append( newMesh )
		newMesh.addMaterial( checkedMatIdx )
		
		faceIdx = 0
		meshFaces = []
		for faceMatIdx in materialPerFace:
			if ( faceMatIdx == checkedMatIdx ):
				meshFaces.append( faces[faceIdx] )
			faceIdx += 1	
			
		newMesh.setFaces( meshFaces )
	
	# and now merge the meshes that reference exactly the same faces
	meshesCount = len( singleMaterialMeshes )
	for i in range( meshesCount ):
	
		if ( singleMaterialMeshes[i] is None ):
			continue
			
		for j in range( len( singleMaterialMeshes ) ):
			if ( i == j ):
				continue;
				
			if ( singleMaterialMeshes[j] is None ):
				continue
			
		if ( singleMaterialMeshes[i].compareFaces( singleMaterialMeshes[j] ) ):
			# the meshes have exactly the same faces, so merge them together
			singleMaterialMeshes[i].mergeMaterials( singleMaterialMeshes[j] )
			
			# reset that material instance, so that we don't merge it twice
			singleMaterialMeshes[j] = None		


	# set the meshes in the output array and initialize them with vertices
	for i in range( meshesCount ):
	
		mergedMesh = singleMaterialMeshes[i]
		
		if ( mergedMesh is not None ):
			
			outMeshes.append( mergedMesh )
			mergedMesh.setVertices( vertices )
			
			# map the materials indices to the ones from our materials dictionary
			mergedMesh.mapMaterials( blenderMesh.materials, materialsExportOrderDict )

	isSkin = len( vertexGroupToBoneDict.keys() ) > 0
	return isSkin

### ===========================================================================

### Creates a mapping between object's vertex groups and the armature that influences it
###
### @param object		object for which we define the mapping
### @param outDict 		- { vertex_group_index, bone_index }
def create_vertex_groups_dict( object, outDict ):

	influencingArmatureObject = object.find_armature()
	if influencingArmatureObject is None:
		return
		
	influencingArmature = influencingArmatureObject.data
	if influencingArmature is None:
		return

	print( "Defining vertex groups --> bones mapping" )
	
	meshVertexGroups = object.vertex_groups
	boneIdx = 0
	vertexGroupsCount = len( object.vertex_groups )
	for boneName in influencingArmature.bones.keys():
	
		vertexGroupIdx = object.vertex_groups.find( boneName )		
		if vertexGroupIdx >= 0 and vertexGroupIdx < vertexGroupsCount:
			vertexGroup = object.vertex_groups.values()[vertexGroupIdx]
			
			if vertexGroup is not None:
				print( "Bone", boneName, boneIdx, " to a vertex group", vertexGroup.name, vertexGroup.index )
				outDict[vertexGroupIdx] = boneIdx
			
		boneIdx += 1

### ===========================================================================

def uv_key( u, v ):
    return round(u, 6), round(v, 6)
	
### The method splits vertices so that each has only a single UV coordinate assigned.
###
### @param blenderMesh
### @param vertices : array< TempMeshVertex >
### @param faces : array< index tuples >
def assign_uv_coords( blenderMesh, vertices, faces ):

	if ( blenderMesh.uv_layers is None ) or ( blenderMesh.uv_layers.active is None ):
		# No uv's present
		return vertices, faces
	
	uvLayer = blenderMesh.uv_layers.active
	splitVertices = []

    # for each face uv coordinate, add it to the UniqueList of the vertex
	uvsDict = [ [] for i in range( len( vertices ) ) ]
	for poly in blenderMesh.polygons:
	
		for loopIndex in range(poly.loop_start, poly.loop_start + poly.loop_total):
			vtxIdx = blenderMesh.loops[loopIndex].vertex_index
			uvCoord = uvLayer.data[loopIndex].uv
			uvCoordKey = uv_key( uvCoord[0], uvCoord[1] )
			
			arrUVs = uvsDict[vtxIdx]
			arrUVs.append( uvCoordKey )
		
	# change them all into sets to eliminate duplicate UV coordinates
	outUVCoordsToIndicesMap = [ [] for i in range( len( vertices ) ) ]
	for vtxIdx in range( len( uvsDict ) ):
		uvsDict[vtxIdx] = set( uvsDict[vtxIdx] )

		for uvCoords in uvsDict[vtxIdx]:
			originalVertex = vertices[vtxIdx]
			
			outUVCoordsToIndicesMap[vtxIdx].append( ( uvCoords, len( splitVertices ) ) )
			
			copiedVertex = copy.deepcopy( originalVertex )
			copiedVertex.setUV( uvCoords[0], uvCoords[1] )
			splitVertices.append( copiedVertex )
			
		
	# split the polygons and duplicate the vertices so that each vertex has
	# only one set of UV coordinates assigned
	newFaces = []
	for poly in blenderMesh.polygons:	
	
		faceVtxIndices = []
		for loopIndex in range(poly.loop_start, poly.loop_start + poly.loop_total):
			vtxIdx = blenderMesh.loops[loopIndex].vertex_index
			uvCoord = uvLayer.data[loopIndex].uv
			uvCoord = uv_key( uvCoord[0], uvCoord[1] )
			
			for vtxUvCoords, newVtxIdx in outUVCoordsToIndicesMap[vtxIdx]:
				if vtxUvCoords == uvCoord:
					faceVtxIndices.append( newVtxIdx )
					break
			
		newFaces.append( faceVtxIndices )
		
	return splitVertices, newFaces
	
	
### The method splits vertices so that each has only a single UV coordinate assigned.
###
### @param blenderMesh
### @param vertices : array< TempMeshVertex >
### @param faces : array< index tuples >
def calculate_unique_uv_coords( blenderMesh, vertices, faces ):

	if bool( blenderMesh.tessface_uv_textures ) != True:
		# No uv's present
		return vertices, faces
	
	splitVertices = []

    # for each face uv coordinate, add it to the UniqueList of the vertex
	uvsDict = [ [] for i in range( len( vertices ) ) ]
	polyIdx = 0
	for poly in blenderMesh.polygons:
	
		polyUVs = blenderMesh.tessface_uv_textures.active.data[polyIdx]
		
		uvOffset = 0
		for vtxIdx in poly.vertices:		
			uvCoord = uv_key( polyUVs.uv[uvOffset][0], polyUVs.uv[uvOffset][1] )
			
			arrUVs = uvsDict[vtxIdx]
			arrUVs.append( uvCoord )
			
			uvOffset += 1
				
		polyIdx += 1
		
	# change them all into sets to eliminate duplicate uvs
	outUVCoordsToIndicesMap = [ [] for i in range( len( vertices ) ) ]
	for vtxIdx in range( len( uvsDict ) ):
		uvsDict[vtxIdx] = set( uvsDict[vtxIdx] )

		for uvCoords in uvsDict[vtxIdx]:
			originalVertex = vertices[vtxIdx]
			
			outUVCoordsToIndicesMap[vtxIdx].append( ( uvCoords, len( splitVertices ) ) )
			
			copiedVertex = copy.deepcopy( originalVertex )
			copiedVertex.setUV( uvCoords[0], uvCoords[1] )
			splitVertices.append( copiedVertex )
			
		
	# split the polygons and duplicate the vertices so that each vertex has
	# only one set of UV coordinates assigned
	polyIdx = 0
	newFaces = []
	for poly in blenderMesh.polygons:	
	
		polyUVs = blenderMesh.tessface_uv_textures.active.data[polyIdx]
		faceVtxIndices = []
		for i in range( len( poly.vertices ) ):
		
			uvCoord = uv_key( polyUVs.uv[i][0], polyUVs.uv[i][1] )	
			vtxIdx = poly.vertices[i]
			
			for vtxUvCoords, newVtxIdx in outUVCoordsToIndicesMap[vtxIdx]:
				if vtxUvCoords == uvCoord:
					faceVtxIndices.append( newVtxIdx )
					break
			
		newFaces.append( faceVtxIndices )
		polyIdx += 1
		
	return splitVertices, newFaces
			
### ===========================================================================
	
### A helper method that splits non-triangular polygons into faces ( by rearranging their indices of course ;)
###
### @param polygons
### @param faces : array< index tuples >
### @param materialPerFace
### @param vertices : array< TempMeshVertex >
def split_polygons_into_triangles( polygons, faces, materialPerFace, vertices ):
	
	tamyFaces = []
		
	faceIdx = 0
	for faceIdx in range( len( polygons ) ):
		poly = polygons[faceIdx]
		face = faces[faceIdx]
		indicesCount = len( face )
		
		# First check to see if it's supposed to be smoothed or not
		# Smoothed polygons share vertices with other polygons, while hard polygons don't,
		# and we need to duplicate their vertices
		
		indices = array( 'i', face[:] )
		
		# 1. Duplicate the vertices as many times as necessary, so that each 
		#    vertex has only one set of UV coordinates assigned, and wears
		#    a unique normal if the face it belongs to is supposed to be rendered flat
		if ( poly.use_smooth == False ):	
			for i in range( len( indices ) ):				
				# duplicate the vertices
				originalVertex = vertices[indices[i]]
				duplicatedVertex = copy.deepcopy( originalVertex )
				duplicatedVertex.setNormal( poly.normal )
				
				indices[i] = len( vertices )
				vertices.append( duplicatedVertex )
		
		# 2. triangulate the polygon if needed
		if ( indicesCount == 3 ):
			# it's a regular face
			tamyFaces.append( TamyFace( [ indices[0], indices[1], indices[2] ] ) )
			materialPerFace.append( poly.material_index )
				
		elif ( indicesCount == 4 ):
			# it's a quad
			tamyFaces.append( TamyFace( [ indices[0], indices[1], indices[2] ] ) )
			tamyFaces.append( TamyFace( [ indices[0], indices[2], indices[3] ] ) )
			materialPerFace.append( poly.material_index )
			materialPerFace.append( poly.material_index )

		else:
			print( "\nWARNING Unsupported polygon with more than 4 vertices encountered" )
			
	return tamyFaces
			
### ===========================================================================

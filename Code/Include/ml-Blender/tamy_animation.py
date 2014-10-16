### @file	ml-Blender/tamy_skeleton.py
### @brief	structures that represent Tamy skeleton
### ===========================================================================

from ctypes import *
from array import *
from . import tamy_misc
from . import tamy_entities
import bpy
import copy

### ===========================================================================

### A helper structure that represents a bone
class TamyBone( Structure ):

	_fields_= [ ( "name", c_char_p ),
				( "modelSpaceTransform", tamy_misc.TamyMatrix ),
				( "parentBoneIdx", c_int ),
				( "boneLength", c_float ) ]
				
	### Constructor
	###
	### @param blenderBone ( bpy.types.Bone(bpy_struct) )
	def __init__( self, blenderBone, blenderBonesArr ):
		
		self.name = blenderBone.name.encode( "utf-8" )
				
		# look for the parent bone
		self.parentBoneIdx = -1
		if blenderBone.parent is not None:
		
			idx = 0
		
			for b in blenderBonesArr:
				if b == blenderBone.parent:
					self.parentBoneIdx = idx
					break
				
				idx += 1
			
		self.modelSpaceTransform = tamy_misc.TamyMatrix( blenderBone.matrix_local )
		self.boneLength = blenderBone.length
		
### ===========================================================================

### A helper structure that represents a skeleton
class TamySkeleton( Structure ):
	
	_fields_= [ ( "name", c_char_p ),
				( "bones", POINTER( TamyBone ) ),
				( "bonesCount", c_int )	]
	
	
	### Constructor
	###
	### @param blenderArmature ( bpy.types.Armature(ID) )
	def __init__( self, blenderArmature ):
		
		self.name = blenderArmature.name.encode( "utf-8" )
		
		bonesArr = blenderArmature.bones.values()
		self.bonesCount = len( bonesArr )
		self.bones = ( TamyBone * self.bonesCount )()
		
		i = 0
		for bone in bonesArr:
			self.bones[i] = TamyBone( bone, bonesArr )
			i += 1

### ===========================================================================

### This is a helper structure to TamyAnimation, which holds a single track of animation
class TamyAnimationTrack( Structure ):

	_fields_= [ ( "boneName", c_char_p ),
				( "trackIdx", c_int ),
				( "framesCount", c_int ),
				( "frameKeys", POINTER( tamy_misc.TamyTransform ) ) ]
	
	### Constructor
	###
	### @param trackIdx
	### @param framesCount
	def __init__( self, trackIdx, framesCount ):
		
		self.trackIdx = trackIdx
		
		# Allocate arrays
		self.framesCount = framesCount
		self.frameKeys = ( tamy_misc.TamyTransform * self.framesCount )()
		for frameIdx in range( self.framesCount ):
			self.frameKeys[frameIdx] = tamy_misc.TamyTransform()
	
	###	Samples the transform for this track and stores it in the specified keyframe
	###
	### @param animatedObject ( bpy.types.Object(ID) )
	### @param frameIdx ( int )
	def sample( self, animatedObject, frameIdx ):

		poseBone = animatedObject.pose.bones[self.trackIdx]

		loc, rot, scale = poseBone.matrix_basis.decompose()
		self.frameKeys[frameIdx].set( loc, rot )

### ===========================================================================

class AnimationSampler( Structure ):		
		
	### Returns number of tracks required to animate the specified object
	def getTracksCount( self ):
		pass

	### Returns the index of the motion track, or -1 if one wasn't defined
	def getMotionTrackIdx( self ):
		pass
		
	###	Samples the transform for this track and stores it in the specified keyframe
	###
	### @param frameIdx ( int )
	### @param trackIdx ( int )
	### @param track ( TamyAnimationTrack )
	def sample( self, frameIdx, track ):
		pass
	
	###	Sets the track name
	###
	### @param trackIdx ( int )
	### @param track ( TamyAnimationTrack )	
	def setName( self, trackIdx, track ):
		pass
		
	### Returns the export index of a skeleton this animation references
	###
	### @param armaturesArr
	def getSkeletonIdx( self, armaturesArr ):
		pass
		
	### Puts an identity keyframe into the track at the specified frame
	def sampleIdentity( self, frameIdx, track ):
	
		track.frameKeys[frameIdx].setIdentity()
		
### ===========================================================================

class ArmatureAnimationSampler( AnimationSampler ):

	animatedObject = None
	
	### Constructor
	###
	### @param animatedObject ( bpy.types.Object(ID) )
	def __init__( self, animatedObject ):
		
		self.animatedObject = animatedObject
		
	### -----------------------------------------------------------------------
	### AnimationSampler implementation
	### -----------------------------------------------------------------------
	def getTracksCount( self ):
		
		pose = self.animatedObject.pose
		tracksCount = len( pose.bones.keys() )
		return tracksCount
		
	def getMotionTrackIdx( self ):
	
		pose = self.animatedObject.pose
		bonesCount = len( pose.bones.keys() )
		for boneIdx in range( bonesCount ):
			boneName = pose.bones[boneIdx].name
			if boneName == "MoverChannel":
				return boneIdx
				
		# Bone name 'MoverChannel' wasn't defined
		return -1
	
	def setName( self, trackIdx, track ):
		pose = self.animatedObject.pose
		boneName = pose.bones[trackIdx].name
		track.boneName = boneName.encode( "utf-8" )
		
	def getSkeletonIdx( self, armaturesArr ):
		referencedArmature = self.animatedObject.data
	
		skeletonsCount = len( armaturesArr )
		for i in range( skeletonsCount ):
			armature = armaturesArr[i]
			if armature == referencedArmature:
				return i
		
		return -1
		
	def sample( self, frameIdx, track ):
		
		poseBone = self.animatedObject.pose.bones[track.trackIdx]

		# poseBone.matrix is in object space - we need to convert it to local space	
		if poseBone.parent is not None:
			parentRefPoseMtx = poseBone.parent.bone.matrix_local
			boneRefPoseMtx = poseBone.bone.matrix_local
			parentPoseMtx = poseBone.parent.matrix
			bonePoseMtx = poseBone.matrix
			
			boneLocMtx = ( parentRefPoseMtx.inverted() * boneRefPoseMtx ).inverted() * ( parentPoseMtx.inverted() * bonePoseMtx )
			
		else:
			boneRefPoseMtx = poseBone.bone.matrix_local
			bonePoseMtx = poseBone.matrix
			
			boneLocMtx = boneRefPoseMtx.inverted() * bonePoseMtx
			
		loc, rot, scale = boneLocMtx.decompose()
		track.frameKeys[frameIdx].set( loc, rot )
		
### ===========================================================================

class NodeAnimationSampler( AnimationSampler ):

	animatedObject = None
	
	### Constructor
	###
	### @param animatedObject ( bpy.types.Object(ID) )
	def __init__( self, animatedObject ):	
		self.animatedObject = animatedObject
		
	### -----------------------------------------------------------------------
	### AnimationSampler implementation
	### -----------------------------------------------------------------------
	def getTracksCount( self ):		
		return 1
		
	def getMotionTrackIdx( self ):	
		return -1
		
	def setName( self, trackIdx, track ):
		track.boneName = "EntityAnimTrack".encode( "utf-8" )
		
	def getSkeletonIdx( self, armaturesArr ):
		return -1
		
	def sample( self, frameIdx, track ):	
		loc, rot, scale = self.animatedObject.matrix_local.decompose()
		track.frameKeys[frameIdx].set( loc, rot )
		
### ===========================================================================

### This structure holds a definition of a transform-based animation 
class TamyAnimation( Structure ):

	_fields_= [ ( "name", c_char_p ),
				( "skeletonIdx", c_int ),
				( "animatedEntityIdx", c_int ),
				( "playbackSpeed", c_float ),
				( "framesCount", c_int ),
				( "poseTracksCount", c_int ),
				( "poseTracks", POINTER( TamyAnimationTrack ) ),
				( "motionTrack", TamyAnimationTrack ) ]
				
	### Constructor
	###
	### @param scene
	### @param animatedObject ( bpy.types.Object(ID) ) - animated object
	### @param blenderAnimation ( bpy.types.Action(bpy_struct) )
	### @param animatedEntityIdx
	### @param armaturesArr
	def __init__( self, scene, animatedObject, animAction, animatedEntityIdx, armaturesArr ):
			
		self.animatedEntityIdx = animatedEntityIdx
		self.playbackSpeed = 24
		
		self.name = animAction.name.encode( "utf-8" )
		self.framesCount = int( animAction.frame_range[1] ) + 1
		
		# create a sampler appropriate to the specified animated object's type
		animationSampler = None
		if animatedObject.type == 'ARMATURE':
			animationSampler = ArmatureAnimationSampler( animatedObject )

			# apply the sampled action to the armature
			animatedObject.animation_data.action = animAction
		else:
			animationSampler = NodeAnimationSampler( animatedObject )
		
		#determine the armature index
		self.skeletonIdx = animationSampler.getSkeletonIdx( armaturesArr )
		
		# determine number of tracks
		self.poseTracksCount = animationSampler.getTracksCount()
		motionTrackIdx = animationSampler.getMotionTrackIdx()
		
		# create pose tracks
		self.poseTracks = ( TamyAnimationTrack * self.poseTracksCount )()
		for trackIdx in range( self.poseTracksCount ):
			self.poseTracks[trackIdx] = TamyAnimationTrack( trackIdx, self.framesCount )
			animationSampler.setName( trackIdx, self.poseTracks[trackIdx] )
			
		#create a motion track
		self.motionTrack = TamyAnimationTrack( motionTrackIdx, self.framesCount )
		animationSampler.setName( motionTrackIdx, self.motionTrack )
		
		# sample animation
		for frameIdx in range( self.framesCount ):
		
			# set the frame of animation
			scene.frame_set( frameIdx )
			
			# sample the pose
			for poseTrackIdx in range( self.poseTracksCount ):
				
				poseTrack = self.poseTracks[poseTrackIdx]
				
				if poseTrackIdx == motionTrackIdx:
					# sample identity transforms into the respective pose track instead
					animationSampler.sampleIdentity( frameIdx, poseTrack )
					
				else:
					# sample this track as a pose channel
					animationSampler.sample( frameIdx, poseTrack )
					
			# sample motion track ( if one's present )
			if motionTrackIdx >= 0:
				animationSampler.sample( frameIdx, self.motionTrack )

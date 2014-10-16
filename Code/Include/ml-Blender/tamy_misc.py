### @file	ml-Blender/tamy_misc.py
### @brief	helper structures
### ===========================================================================

from ctypes import *


### ===========================================================================

### This class represents a color exported to Tamy
class TamyColor( Structure ):

	_fields_ = [ ("r", c_float), ("g", c_float), ("b", c_float), ("a", c_float) ]
	
### ===========================================================================

### A helper structure that represents an exportable matrix	
class TamyMatrix( Structure ):
	_fields_= [ ( "matRot", c_float * 16 ),
				( "pos", c_float * 4 ) ]
	
	def __init__( self, blenderMtx ):
		col0, col1, col2, col3 = blenderMtx[:]
		self.matRot[0] = col0[0]
		self.matRot[1] = col1[0]
		self.matRot[2] = col2[0]
		self.matRot[3] = col3[0]
		
		self.matRot[4] = col0[1]
		self.matRot[5] = col1[1]
		self.matRot[6] = col2[1]
		self.matRot[7] = col3[1]
		
		self.matRot[8] = col0[2]
		self.matRot[9] = col1[2]
		self.matRot[10] = col2[2]
		self.matRot[11] = col3[2]
		
		self.matRot[12] = 0
		self.matRot[13] = 0
		self.matRot[14] = 0
		self.matRot[15] = 1
		
		self.pos[0] = col0[3]
		self.pos[1] = col1[3]
		self.pos[2] = col2[3]
		self.pos[3] = col3[3]

		
### ===========================================================================

### A helper structure that represents an exportable transform
class TamyTransform( Structure ):

	_fields_= [ ( "translation", c_float * 4 ),
				( "rotation", c_float * 4 ) ]
	
	### Constructor
	def __init__( self ):
		self.translation[0] = 0
		self.translation[1] = 0
		self.translation[2] = 0
		self.translation[3] = 1
		
		self.rotation[0] = 0
		self.rotation[1] = 0
		self.rotation[2] = 0
		self.rotation[3] = 1
		
		
	### Sets new transform value
	###
	### @param translation ( a blender vector ( x, y, z ) )
	### @param rotation ( a blender quaternion ( w, x, y, z)
	def set( self, translation, rotation ):
		
		self.translation[0] = translation[0]
		self.translation[1] = translation[1]
		self.translation[2] = translation[2]
		self.translation[3] = 1
		
		self.rotation[0] = rotation[1]
		self.rotation[1] = rotation[2]
		self.rotation[2] = rotation[3]
		self.rotation[3] = rotation[0]
	
	### Sets the transform's value to identity
	def setIdentity( self ):
		self.translation[0] = 0
		self.translation[1] = 0
		self.translation[2] = 0
		self.translation[3] = 1
		
		self.rotation[0] = 0
		self.rotation[1] = 0
		self.rotation[2] = 0
		self.rotation[3] = 1
				
### ===========================================================================
